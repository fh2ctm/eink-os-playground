﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothSettingsModel.hpp"

#include <Constants.hpp>
#include <service-bluetooth/messages/BondedDevices.hpp>
#include <service-bluetooth/messages/Connect.hpp>
#include <service-bluetooth/messages/DeviceName.hpp>
#include <service-bluetooth/messages/Disconnect.hpp>
#include <service-bluetooth/messages/Status.hpp>
#include <service-bluetooth/messages/SetStatus.hpp>
#include <service-bluetooth/messages/SetDeviceName.hpp>
#include <service-bluetooth/messages/Passkey.hpp>
#include <service-bluetooth/messages/Unpair.hpp>

BluetoothSettingsModel::BluetoothSettingsModel(app::Application *application) : application{application}
{}

void BluetoothSettingsModel::requestStatus()
{
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestStatus>(), service::name::bluetooth);
}

void BluetoothSettingsModel::setStatus(const bool desiredBluetoothState, const bool desiredVisibility)
{
    BluetoothStatus status{.state = desiredBluetoothState ? BluetoothStatus::State::On : BluetoothStatus::State::Off,
                           .visibility = desiredVisibility};
    message::bluetooth::SetStatus setStatus(status);
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::SetStatus>(std::move(setStatus)),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestDeviceName()
{
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestDeviceName>(), service::name::bluetooth);
}

void BluetoothSettingsModel::setDeviceName(const UTF8 &deviceName)
{
    application->bus.sendUnicast(std::make_shared<message::bluetooth::SetDeviceName>(deviceName),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestBondedDevices()
{
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestBondedDevices>(),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestScan()
{
    application->bus.sendUnicast(std::make_shared<BluetoothMessage>(BluetoothMessage::Request::Scan),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::stopScan()
{
    application->bus.sendUnicast(std::make_shared<BluetoothMessage>(BluetoothMessage::Request::StopScan),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestDevicePair(std::string addr)
{
    application->bus.sendUnicast(std::make_shared<BluetoothPairMessage>(std::move(addr)), service::name::bluetooth);
}

void BluetoothSettingsModel::requestDeviceUnpair(std::string addr)
{
    application->bus.sendUnicast(std::make_shared<message::bluetooth::Unpair>(std::move(addr)),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::responsePasskey(std::string passkey)
{
    application->bus.sendUnicast(std::make_shared<message::bluetooth::ResponsePasskey>(std::move(passkey)),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestConnection(std::string addr)
{
    application->bus.sendUnicast(std::make_shared<message::bluetooth::Connect>(std::move(addr)),
                                 service::name::bluetooth);
}

void BluetoothSettingsModel::requestDisconnection()
{
    application->bus.sendUnicast(std::make_shared<message::bluetooth::Disconnect>(), service::name::bluetooth);
    application->bus.sendUnicast(std::make_shared<::message::bluetooth::RequestBondedDevices>(),
                                 service::name::bluetooth);
}
