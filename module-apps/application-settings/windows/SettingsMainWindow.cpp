// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SettingsMainWindow.hpp"
#include "../ApplicationSettings.hpp"
#include "Info.hpp"
#include "module-utils/i18n/i18n.hpp"
#include "log/log.hpp"
#include "CellularPassthroughWindow.hpp"
#include "FotaWindow.hpp"
#include "USSDWindow.hpp"
#include "EinkModeWindow.hpp"

std::list<gui::Option> mainWindowOptions(app::Application *app)
{
    std::list<gui::Option> l;

    auto i18     = [](std::string text) { return utils::localize.get(text); };
    auto addMenu = [&](UTF8 name, std::string window = "") {
        l.emplace_back(gui::Option{name,
                                   [=](gui::Item &item) {
                                       if (window == "") {
                                           return false;
                                       }
                                       LOG_INFO("switching to %s page", window.c_str());
                                       app->switchWindow(window, nullptr);
                                       return true;
                                   },
                                   gui::Arrow::Enabled});
    };

    addMenu(i18("Information"), gui::window::hw_info);
    addMenu("UI TEST", "TEST_UI");
    addMenu(i18("app_settings_bt"), "Bluetooth");
    addMenu(i18("app_settings_language"), "Languages");
    addMenu("SIM SELECT", app::sim_select);
    addMenu("Change setting", app::change_setting);
    addMenu(i18("app_settings_date_and_time"), "DateTime");
    if (dynamic_cast<app::ApplicationSettings *>(app)->board == bsp::Board::T4) {
        addMenu(i18("app_settings_cellular_passthrough"), gui::window::cellular_passthrough::window_name);
    }
    addMenu(i18("Fota update"), gui::window::name::fota_window);
    addMenu(i18("USSD test"), gui::window::name::ussd_window);
    addMenu("Eink Mode", gui::window::name::eink);
    addMenu(i18("app_settings_display"));
    addMenu(i18("app_settings_phone_modes"));
    addMenu(i18("app_settings_security"));
    addMenu(i18("app_settings_about"));
    return l;
}
