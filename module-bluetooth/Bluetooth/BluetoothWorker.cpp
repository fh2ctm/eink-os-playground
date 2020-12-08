// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothWorker.hpp"
#include "BtCommand.hpp"
#include "log/log.hpp"
#include "interface/profiles/A2DP/A2DP.hpp"
#include "interface/profiles/HSP/HSP.hpp"
extern "C"
{
#include "module-bluetooth/lib/btstack/src/btstack_util.h"
};

#if DEBUG_BLUETOOTH_HCI_COMS == 1
#define logHciComs(...) LOG_DEBUG(__VA_ARGS__)
#else
#define logHciComs(...)
#endif

#if DEBUG_BLUETOOTH_HCI_BYTES == 1
#include <sstream>
#define logHciBytes(...) LOG_DEBUG(__VA_ARGS__)
#else
#define logHciBytes(...)
#endif

using namespace bsp;

const char *c_str(Bt::Error::Code code)
{
    switch (code) {
    case Bt::Error::Code::Success:
        return "Success";
    case Bt::Error::Code::NotReady:
        return "NotReady";
    case Bt::Error::Code::SystemError:
        return "SystemError";
    case Bt::Error::Code::LibraryError:
        return "LibraryError";
    }
    return "";
}

BluetoothWorker::BluetoothWorker(sys::Service *service) : Worker(service), currentProfile(std::make_shared<Bt::HSP>())
{
    init({
        {"qBtIO", sizeof(Bt::Message), 10},
        {"qBtWork", sizeof(Bt::EvtWorker), 10},
    });
};

BluetoothWorker::~BluetoothWorker()
{
    if (this->bt_worker_task != nullptr) {
        vTaskDelete(this->bt_worker_task);
    }
    LOG_INFO("Worker removed");
}

bool BluetoothWorker::run()
{
    LOG_INFO("-> BluetoothWorker run request");
    if (is_running) {
        return true;
    }
    if (Worker::run()) {
        is_running                          = true;
        auto el                             = queues[queueIO_handle];
        BlueKitchen::getInstance()->qHandle = el;
        Bt::initialize_stack();
        Bt::register_hw_error_callback();
        Bt::GAP::register_scan();
        std::string name = "PurePhone";
        Bt::set_name(name);
        // set local namne
        Bt::GAP::set_visibility(true);
        // Bt::GAP::
        Bt::run_stack(&this->bt_worker_task);
        return true;
    }
    else {
        return false;
    }
}

bool BluetoothWorker::scan()
{
    std::vector<Device> empty;
    Bt::GAP::setOwnerService(service);
    auto ret = Bt::GAP::scan();
    if (ret.err != Bt::Error::Success) {
        LOG_ERROR("Cant start scan!: %s %" PRIu32 "", c_str(ret.err), ret.lib_code);
        return false;
    }
    else {
        LOG_INFO("Scan started!");
        // open new scan window
        return true;
    }
}

void BluetoothWorker::stopScan()
{
    Bt::GAP::stop_scan();
}

bool BluetoothWorker::toggleVisibility()
{
    static bool visibility = true;
    Bt::GAP::set_visibility(visibility);
    visibility = !visibility;

    return visibility;
}

bool BluetoothWorker::start_pan()
{
    Bt::PAN::bnep_setup();
    auto err = Bt::PAN::bnep_start();
    if (err.err != Bt::Error::Success) {
        LOG_ERROR("PAN setup error: %s %" PRIu32, c_str(err.err), err.lib_code);
    }
    return false;
}

bool BluetoothWorker::handleMessage(uint32_t queueID)
{

    QueueHandle_t queue = queues[queueID];
    if (queueID == queueService) {
        LOG_DEBUG("not interested");
        return true;
    }
    if (queueID != queueIO_handle) {
        LOG_ERROR("Wrong queue! %" PRIu32, queueID);
        return false;
    }

    Bt::Message notification = Bt::Message::EvtErrorRec;
    if (xQueueReceive(queue, &notification, 0) != pdTRUE) {
        LOG_ERROR("Queue receive failure!");
        return false;
    }
    auto bt = BlueKitchen::getInstance();
    switch (notification) {
    case Bt::Message::EvtSending:
        logHciComs("[evt] sending");
        break;
    case Bt::Message::EvtSent:
        logHciComs("[evt] sent");
        if (bt->write_done_cb) {
            bt->write_done_cb();
        }
        break;
    case Bt::Message::EvtReceiving:
        logHciComs("[evt] receiving");
        break;
    case Bt::Message::EvtReceived: {
        logHciBytes("[evt] BT DMA received <-- [%ld]>%s<",
                    bt->read_len,
                    [&]() -> std::string {
                        std::stringstream ss;
                        for (int i = 0; i < bt->read_len; ++i) {
                            ss << " 0x" << std::hex << (int)*(bt->read_buff + i);
                        }
                        return ss.str();
                    }()
                                 .c_str());

        bt->read_len = 0;

        if (bt->read_ready_cb) {
            bt->read_ready_cb();
        }
    } break;
    case Bt::Message::EvtSendingError:
    case Bt::Message::EvtReceivingError:
    case Bt::Message::EvtUartError:
    case Bt::Message::EvtRecUnwanted:
        LOG_ERROR("Uart error [%d]: %s", notification, Bt::MessageCstr(notification));
        break;
    default:
        LOG_ERROR("ERROR");
    }

    return true;
}

bool BluetoothWorker::establishAudioConnection()
{
    currentProfile->setOwnerService(service);
    if (currentProfile->init() != Bt::Error::Success) {
        return false;
    }
    currentProfile->connect();
    return true;
}
bool BluetoothWorker::disconnectAudioConnection()
{
    currentProfile->disconnect();
    return true;
}
void BluetoothWorker::setDeviceAddress(bd_addr_t addr)
{
    Bt::GAP::do_pairing(addr);
    currentProfile->setDeviceAddress(addr);
}
