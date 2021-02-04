﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalllogModel.hpp"
#include "InputEvent.hpp"
#include "ListView.hpp"
#include "data/CallLogInternals.hpp"
#include "data/CallLogSwitchData.hpp"
#include "widgets/CalllogItem.hpp"
#include <module-utils/time/DateAndTimeSettings.hpp>
#include <module-utils/Utils.hpp>

#include <service-db/DBServiceAPI.hpp>
#include <service-appmgr/Controller.hpp>
#include "application-call/data/CallSwitchData.hpp"

using namespace calllog;

CalllogModel::CalllogModel(app::Application *app) : DatabaseModel(app)
{}

unsigned int CalllogModel::requestRecordsCount()
{
    recordsCount = DBServiceAPI::CalllogGetCount(application);
    return recordsCount;
}

void CalllogModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    DBServiceAPI::CalllogGetLimitOffset(application, offset, limit);
}

bool CalllogModel::updateRecords(std::vector<CalllogRecord> records)
{
#if DEBUG_DB_MODEL_DATA == 1
    LOG_DEBUG("Offset: %" PRIu32 ", Limit: %" PRIu32 " Count: %" PRIu32 "", offset, limit, count);
    for (uint32_t i = 0; i < records.get()->size(); ++i) {
        LOG_DEBUG(
            "id: %" PRIu32 ", name: %s", records.get()->operator[](i).ID, records.get()->operator[](i).name.c_str());
    }
#endif

    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();

    return true;
}

unsigned int CalllogModel::getMinimalItemHeight() const
{

    return gui::clItemStyle::h;
}

gui::ListItem *CalllogModel::getItem(gui::Order order)
{

    std::shared_ptr<CalllogRecord> call = getRecord(order);
    if (call.get() == nullptr) {
        return nullptr;
    }

    auto item = new gui::CalllogItem(this, !(utils::dateAndTimeSettings.isTimeFormat12()));

    auto callCallback = [this, item](gui::Item &, const gui::InputEvent &event) {
        if (event.state != gui::InputEvent::State::keyReleasedShort) {
            return false;
        }
        if (event.keyCode == gui::KeyCode::KEY_LF) {
            return app::manager::Controller::sendAction(
                application,
                app::manager::actions::Dial,
                std::make_unique<app::ExecuteCallData>(item->getCall().phoneNumber));
        }
        return false;
    };

    item->focusChangedCallback = [=](gui::Item &item) {
        if (item.focus == true) {
            auto &callLogItem = dynamic_cast<gui::CalllogItem &>(item);
            if (callLogItem.getCall().presentation == PresentationType::PR_UNKNOWN) {
                // disable call button since the number is unknown
                application->getCurrentWindow()->setBottomBarActive(gui::BottomBar::Side::LEFT, false);
                item.inputCallback = nullptr;
            }
            else {
                application->getCurrentWindow()->setBottomBarActive(gui::BottomBar::Side::LEFT, true);
                item.inputCallback = callCallback;
            }
        }
        return true;
    };

    item->setCall(call);
    item->activatedCallback = [=](gui::Item &item) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<calllog::CallLogSwitchData>(*call);
        application->switchWindow(calllog::settings::DetailsWindowStr, std::move(data));
        return true;
    };

    return item;
}
