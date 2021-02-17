﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-evtmgr/BatteryMessages.hpp"
#include "service-evtmgr/EVMessages.hpp"
#include "service-evtmgr/KbdMessage.hpp"
#include "service-evtmgr/Constants.hpp"
#include "service-evtmgr/WorkerEvent.hpp"
#include "battery-level-check/BatteryLevelCheck.hpp"

#include <Audio/AudioCommon.hpp>
#include <MessageType.hpp>
#include <Service/Worker.hpp>
#include <bsp/battery-charger/battery_charger.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <bsp/keyboard/keyboard.hpp>
#include <bsp/magnetometer/magnetometer.hpp>
#include <bsp/rtc/rtc.hpp>
#include <bsp/torch/torch.hpp>
#include <bsp/keypad_backlight/keypad_backlight.hpp>
#include <bsp/light_sensor/light_sensor.hpp>
#include <bsp/vibrator/vibrator.hpp>
#include <bsp/eink_frontlight/eink_frontlight.hpp>
#include <common_data/EventStore.hpp>
#include <common_data/RawKey.hpp>
#include <headset.hpp>
#include <log/log.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-desktop/Constants.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <SystemManager/SystemManager.hpp>
#include <SystemManager/messages/SentinelRegistrationMessage.hpp>

extern "C"
{
#include "FreeRTOS.h" // for xQueueHandle
#include "projdefs.h" // for pdTRUE
#include "queue.h"    // for xQueueReceive, QueueDefinition, QueueHandle_t
#include "task.h"     // for xTaskGetTickCount
}

#include <sys/types.h> // for time_t
#include <memory>      // for make_shared, __shared_ptr_access, shared_ptr, allocator
#include <optional>    // for optional
#include <string>      // for string
#include <vector>      // for vector

bool WorkerEvent::handleMessage(uint32_t queueID)
{

    auto &queue = queues[queueID];

    // service queue
    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueService)) {
        sys::WorkerCommand wcmd;
        if (!queue->Dequeue(&wcmd, 0)) {
            return false;
        }
        wcmd.command = 1;
        // place some code here to handle messages from service
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueKeyboardIRQ)) {
        uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }
        uint8_t state, code;
        bsp::keyboard_get_data(notification, state, code);

        processKeyEvent(static_cast<bsp::KeyEvents>(state), static_cast<bsp::KeyCodes>(code));
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueHeadsetIRQ)) {
        uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }

        if (bsp::headset::Handler(notification) == true) {
            bool state   = bsp::headset::IsInserted();
            auto message = std::make_shared<AudioEventRequest>(audio::EventType::JackState,
                                                               state ? audio::Event::DeviceState::Connected
                                                                     : audio::Event::DeviceState::Disconnected);
            service->bus.sendUnicast(message, service::name::evt_manager);
        }
    }

    if (queueID == static_cast<std::uint32_t>(WorkerEventQueues::queueBattery)) {
        std::uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }
        if (notification == static_cast<std::uint8_t>(bsp::battery_charger::batteryIRQSource::INTB)) {
            auto topINT = bsp::battery_charger::getTopControllerINTSource();
            if (topINT & static_cast<std::uint8_t>(bsp::battery_charger::topControllerIRQsource::CHGR_INT)) {
                bsp::battery_charger::getChargeStatus();
                auto message = std::make_shared<sevm::BatteryStatusChangeMessage>();
                service->bus.sendUnicast(std::move(message), service::name::evt_manager);
                bsp::battery_charger::clearAllChargerIRQs();
            }
            if (topINT & static_cast<std::uint8_t>(bsp::battery_charger::topControllerIRQsource::FG_INT)) {
                const auto status = bsp::battery_charger::getStatusRegister();
                if (status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minVAlert)) {
                    auto messageBrownout = std::make_shared<sevm::BatteryBrownoutMessage>();
                    service->bus.sendUnicast(std::move(messageBrownout), service::name::system_manager);
                }
                if (status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::SOCOnePercentChange)) {
                    bsp::battery_charger::clearFuelGuageIRQ(
                        static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::SOCOnePercentChange));
                    bsp::battery_charger::getBatteryLevel();
                    auto message = std::make_shared<sevm::BatteryStatusChangeMessage>();
                    service->bus.sendUnicast(std::move(message), service::name::evt_manager);
                    battery_level_check::checkBatteryLevelCritical();
                }
                if (status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::maxTemp) ||
                    status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minTemp)) {
                    bsp::battery_charger::clearFuelGuageIRQ(
                        static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::maxTemp) |
                        static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minTemp));
                    bsp::battery_charger::checkTemperatureRange();
                    bsp::battery_charger::getChargeStatus();
                    auto message = std::make_shared<sevm::BatteryStatusChangeMessage>();
                    service->bus.sendUnicast(std::move(message), service::name::evt_manager);
                }
            }
        }
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueRTC)) {
        uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }

        time_t timestamp;
        bsp::rtc_GetCurrentTimestamp(&timestamp);
        bsp::rtc_SetMinuteAlarm(timestamp);

        auto message       = std::make_shared<sevm::RtcMinuteAlarmMessage>(MessageType::EVMMinuteUpdated);
        message->timestamp = timestamp;
        service->bus.sendUnicast(message, service::name::evt_manager);
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueCellular)) {
        uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }

        if (notification == bsp::cellular::statusPin) {
            auto GSMstatus = bsp::cellular::status::getStatus();
            LOG_DEBUG("GSM Status pin change: %s",
                      (GSMstatus == bsp::cellular::status::value::ACTIVE ? "ACTIVE" : "INACTIVE"));

            auto message   = std::make_shared<sevm::StatusStateMessage>(MessageType::EVMModemStatus);
            message->state = GSMstatus;
            service->bus.sendUnicast(message, "EventManager");
        }

        if (notification == bsp::cellular::trayPin) {
            Store::GSM::Tray pinstate = bsp::cellular::sim::getTray();
            LOG_DEBUG("SIM state change: %d", static_cast<int>(pinstate));
            bsp::cellular::sim::hotswap_trigger();
        }

        if (notification == bsp::cellular::ringIndicatorPin) {
            auto message = std::make_shared<sevm::StatusStateMessage>(MessageType::EVMRingIndicator);
            service->bus.sendUnicast(message, "EventManager");
        }
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueMagnetometerIRQ)) {
        uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }

        if (std::optional<bsp::KeyCodes> key = bsp::magnetometer::WorkerEventHandler()) {
            LOG_DEBUG("magneto IRQ handler: %s", c_str(*key));
            processKeyEvent(bsp::KeyEvents::Pressed, *key);
        }
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueLightSensor)) {
        uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }
        LOG_DEBUG("Light sensor IRQ");
    }

    if (queueID == static_cast<uint32_t>(WorkerEventQueues::queueChargerDetect)) {
        uint8_t notification;
        if (!queue->Dequeue(&notification, 0)) {
            return false;
        }
        LOG_DEBUG("USB charger type: %d", notification);
    }

    return true;
}

bool WorkerEvent::init(std::list<sys::WorkerQueueInfo> queuesList)
{
    Worker::init(queuesList);
    bsp::vibrator::init();
    bsp::keyboard_Init(queues[static_cast<int32_t>(WorkerEventQueues::queueKeyboardIRQ)]->GetQueueHandle());
    bsp::headset::Init(queues[static_cast<int32_t>(WorkerEventQueues::queueHeadsetIRQ)]->GetQueueHandle());
    auto queueBatteryHandle = queues[static_cast<int32_t>(WorkerEventQueues::queueBattery)]->GetQueueHandle();
    auto queueChargerDetect = queues[static_cast<int32_t>(WorkerEventQueues::queueChargerDetect)]->GetQueueHandle();
    bsp::battery_charger::init(queueBatteryHandle, queueChargerDetect);
    bsp::rtc_Init(queues[static_cast<int32_t>(WorkerEventQueues::queueRTC)]->GetQueueHandle());
    bsp::cellular::init(queues[static_cast<int32_t>(WorkerEventQueues::queueCellular)]->GetQueueHandle());
    bsp::magnetometer::init(queues[static_cast<int32_t>(WorkerEventQueues::queueMagnetometerIRQ)]->GetQueueHandle());
    bsp::torch::init(queues[static_cast<int32_t>(WorkerEventQueues::queueTorch)]->GetQueueHandle());
    bsp::keypad_backlight::init();
    bsp::eink_frontlight::init();
    bsp::light_sensor::init(queues[static_cast<int32_t>(WorkerEventQueues::queueLightSensor)]->GetQueueHandle());

    time_t timestamp;
    bsp::rtc_GetCurrentTimestamp(&timestamp);
    bsp::rtc_SetMinuteAlarm(timestamp);

    cpuSentinel = std::make_shared<sys::CpuSentinel>(
        service::name::evt_manager, service, [this](bsp::CpuFrequencyHz newFrequency) {
            updateResourcesAfterCpuFrequencyChange(newFrequency);
        });

    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    service->bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);

    return true;
}

void WorkerEvent::init(std::list<sys::WorkerQueueInfo> queuesList, std::shared_ptr<settings::Settings> settings)
{
    init(queuesList);
    battery_level_check::init(service, settings);
}

bool WorkerEvent::deinit(void)
{
    Worker::deinit();
    bsp::keyboard_Deinit();
    bsp::headset::Deinit();
    bsp::battery_charger::deinit();
    bsp::torch::deinit();
    bsp::keypad_backlight::deinit();
    bsp::eink_frontlight::deinit();
    bsp::light_sensor::deinit();
    bsp::vibrator::deinit();

    battery_level_check::deinit();

    return true;
}

void WorkerEvent::updateResourcesAfterCpuFrequencyChange(bsp::CpuFrequencyHz newFrequency)
{
    bsp::eink_frontlight::updateClockFrequency(newFrequency);
}

void WorkerEvent::processKeyEvent(bsp::KeyEvents event, bsp::KeyCodes code)
{
    auto message = std::make_shared<sevm::KbdMessage>();

    message->key.key_code = code;

    if (event == bsp::KeyEvents::Pressed) {
        if (lastState == bsp::KeyEvents::Pressed) {
            return;
        }

        message->key.state      = RawKey::State::Pressed;
        message->key.time_press = xTaskGetTickCount();

        // Slider sends only press, not release state so it would block the entire keyboard
        if ((code != bsp::KeyCodes::SSwitchUp) && (code != bsp::KeyCodes::SSwitchMid) &&
            (code != bsp::KeyCodes::SSwitchDown)) {
            lastPressed = code;
            lastState   = event;
        }
    }
    else {
        if (lastState != bsp::KeyEvents::Pressed) {
            return;
        }
        if (lastPressed != code) {
            return;
        }

        lastState = bsp::KeyEvents::Released;
        {
            message->key.state        = RawKey::State::Released;
            message->key.time_release = xTaskGetTickCount();
        }
    }
    service->bus.sendUnicast(message, service::name::evt_manager);
}

void WorkerEvent::checkBatteryLevelCritical()
{
    battery_level_check::checkBatteryLevelCriticalWithConfirmation();
}
