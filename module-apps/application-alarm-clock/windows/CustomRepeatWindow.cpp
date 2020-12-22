// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CustomRepeatWindow.hpp"

namespace app::alarmClock
{

    CustomRepeatWindow::CustomRepeatWindow(app::Application *app,
                                           std::unique_ptr<CustomRepeatWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, style::alarmClock::window::name::customRepeat), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void CustomRepeatWindow::buildInterface()
    {
        AppWindow::buildInterface();

        topBar->setActive(gui::TopBar::Elements::TIME, true);
        topBar->setActive(gui::TopBar::Elements::SIM, false);
        topBar->setActive(gui::TopBar::Elements::NETWORK_ACCESS_TECHNOLOGY, false);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));

        setTitle(utils::localize.get("app_calendar_custom_repeat_title"));
        list = new gui::ListView(this,
                                 style::alarmClock::window::listView_x,
                                 style::alarmClock::window::listView_y,
                                 style::alarmClock::window::listView_w,
                                 style::alarmClock::window::listView_h,
                                 presenter->getItemProvider());
        setFocusItem(list);
    }

    void CustomRepeatWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        if (auto receivedData = dynamic_cast<WeekDaysRepeatData *>(data); receivedData != nullptr) {
            weekDaysOptData = *receivedData;
        }
        else {
            weekDaysOptData = WeekDaysRepeatData();
        }
        presenter->loadData(weekDaysOptData);
    }

    bool CustomRepeatWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            weekDaysOptData = presenter->getWeekDaysRepeatData();
            application->switchWindow(style::alarmClock::window::name::newEditAlarm,
                                      gui::ShowMode::GUI_SHOW_RETURN,
                                      std::make_unique<WeekDaysRepeatData>(weekDaysOptData));
            return true;
        }
        return false;
    }
} // namespace app::alarmClock