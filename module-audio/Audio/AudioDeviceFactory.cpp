// Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AudioDeviceFactory.hpp"

using namespace audio;

AudioDeviceFactory::AudioDeviceFactory(Observer *observer) : _observer(observer)
{}

std::shared_ptr<AudioDevice> AudioDeviceFactory::CreateDevice(AudioDevice::Type deviceType)
{
    std::shared_ptr<AudioDevice> device = getDeviceFromType(deviceType);

    if (_observer != nullptr && device) {
        _observer->onDeviceCreated(device);
    }

    return device;
}

void AudioDeviceFactory::setObserver(Observer *observer) noexcept
{
    _observer = observer;
}
