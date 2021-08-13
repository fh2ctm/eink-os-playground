﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <service-db/Settings.hpp>
#include <service-db/SettingsMessages.hpp>
#include <Service/Service.hpp>
#include <functional>
#include <thread> // for Message_t, ResponseMessage, DataMessage, Message

#include <evtmgr/EventManager.hpp>
#include <module-services/service-db/ServiceDB.hpp>
#include <module-sys/SystemManager/SystemManager.hpp>

#include <service-evtmgr/Constants.hpp>

#include "test-service-db-settings-testmsgs.hpp"
#include "test-service-db-settings-testservices.hpp"
#include "test-service-db-settings-testapps.hpp"
#include "Database.cpp"

TEST_CASE("SettingsApi")
{
    SECTION("variable/profile/mode register/set/get/unregister")
    {
        auto manager = std::make_shared<sys::SystemManager>(std::vector<std::unique_ptr<sys::BaseServiceCreator>>{});
        std::shared_ptr<settings::MyService> varWritter;
        std::shared_ptr<settings::MyService> varReader;
        std::shared_ptr<settings::AppTest> testVar;
        std::shared_ptr<std::mutex> testStart;

        std::shared_ptr<settings::Settings> postMortemSetting;

        manager->StartSystem(nullptr, [manager, &varWritter, &varReader, &testVar, &testStart, &postMortemSetting]() {
            // preliminary
            testStart = std::make_shared<std::mutex>();
            testStart->lock();
            std::cout << "start thr_id: " << std::this_thread::get_id() << std::endl << std::flush;
            auto ret = sys::SystemManager::RunSystemService(std::make_shared<EventManager>(service::name::evt_manager),
                                                            manager.get());
            ret &= sys::SystemManager::RunSystemService(std::make_shared<ServiceDB>(), manager.get());

            varWritter = std::make_shared<settings::MyService>("writterVar");
            varReader  = std::make_shared<settings::MyService>("readerVar");

            postMortemSetting = varWritter->getSettings();

            ret &= sys::SystemManager::RunSystemService(varWritter, manager.get());
            ret &= sys::SystemManager::RunSystemService(varReader, manager.get());

            testVar = std::make_shared<settings::AppTest>("appTest", varWritter, varReader, testStart);
            ret &= sys::SystemManager::RunSystemService(testVar, manager.get());

            std::cout << "koniec start thr_id: " << std::this_thread::get_id() << std::endl << std::flush;
            testStart->unlock();
            auto msgStart = std::make_shared<settings::UTMsg::UTMsgStart>();
            manager->bus.sendUnicast(std::move(msgStart), "appTest");

            msgStart = std::make_shared<settings::UTMsg::UTMsgStart>();
            manager->bus.sendUnicast(std::move(msgStart), "appTestProfile");

            msgStart = std::make_shared<settings::UTMsg::UTMsgStart>();
            manager->bus.sendUnicast(std::move(msgStart), "appTestMode");

            return ret;
        });

        try {
            // start application
            cpp_freertos::Thread::StartScheduler();

            // check the results
            std::cout << "testVar values:" << std::endl << std::flush;
            for (const auto &s : testVar->v) {
                std::cout << s << std::endl << std::flush;
            }
            REQUIRE(testVar->v.size() == 3);
            REQUIRE(testVar->v[1] == testVar->v[0] + "1");
            REQUIRE(testVar->v[2] == testVar->v[1] + "2");
        }
        catch (std::exception &error) {
            std::cout << error.what() << std::endl;
            exit(1);
        }
    }
}
