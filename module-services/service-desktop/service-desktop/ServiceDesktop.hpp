﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory> // for allocator, unique_ptr

#include "WorkerDesktop.hpp"
#include "endpoints/update/UpdateMuditaOS.hpp"
#include "Service/Common.hpp"  // for ReturnCodes, ServicePowerMode
#include "Service/Message.hpp" // for MessagePointer, DataMessage (ptr only), ResponseMessage (ptr only)
#include "Service/Service.hpp" // for Service
#include "Constants.hpp"
#include "WorkerDesktop.hpp"
#include <endpoints/update/UpdateMuditaOS.hpp>
#include <service-db/DBServiceName.hpp>

namespace settings
{
    class Settings;
}

namespace sdesktop
{
    inline constexpr auto service_stack         = 8192;
    inline constexpr auto cdc_queue_len             = 32;
    inline constexpr auto cdc_queue_object_size     = 1024;
    inline constexpr auto file_transfer_timeout     = 5000;
    inline constexpr auto RECEIVE_QUEUE_BUFFER_NAME = "receiveQueueBuffer";
    inline constexpr auto SEND_QUEUE_BUFFER_NAME    = "sendQueueBuffer";
}; // namespace sdesktop

class ServiceDesktop : public sys::Service
{
  public:
    ServiceDesktop();
    ~ServiceDesktop() override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;
    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
    sys::MessagePointer DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;

    std::unique_ptr<UpdateMuditaOS> updateOS;
    std::unique_ptr<WorkerDesktop> desktopWorker;
    void storeHistory(const std::string &historyValue);

  private:
    std::unique_ptr<settings::Settings> settings;
    std::unique_ptr<sys::Timer> transferTimer;
};

namespace sys
{
    template <> struct ManifestTraits<ServiceDesktop>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::service_desktop;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys
