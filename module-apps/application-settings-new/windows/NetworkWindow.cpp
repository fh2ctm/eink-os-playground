// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NetworkWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"

#include "OptionSetting.hpp"

#include <i18n/i18n.hpp>

namespace gui
{
    NetworkWindow::NetworkWindow(app::Application *app,
                                 app::settingsInterface::SimParams *simParams,
                                 app::settingsInterface::OperatorsSettings *operatorsSettings)
        : OptionWindow(app, gui::window::name::network), simParams(simParams), operatorsSettings(operatorsSettings)
    {
    }
    void NetworkWindow::onBeforeShow(ShowMode m, SwitchData *d)
    {
        rebuild();
    }
    auto NetworkWindow::netOptList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optList;
        std::string simStr;
        auto phoneNumber = simParams->getNumber();
        auto sim         = simParams->getSim();
        switch (sim) {
        case Store::GSM::SIM::SIM1:
            simStr = utils::translateI18("app_settings_network_sim1");
            break;
        case Store::GSM::SIM::SIM2:
            simStr = utils::translateI18("app_settings_network_sim2");
            break;
        case Store::GSM::SIM::NONE:
        case Store::GSM::SIM::SIM_FAIL:
        case Store::GSM::SIM::SIM_UNKNOWN:
            simStr      = utils::translateI18("app_settings_network_sim_none");
            phoneNumber = {};
            break;
        }
        auto operatorsOn = operatorsSettings->getOperatorsOn();
        optList.emplace_back(std::make_unique<gui::OptionSettings>(
            utils::translateI18("app_settings_network_active_card") + ":" + simStr + " / " + phoneNumber,
            [=](gui::Item &item) {
                if (Store::GSM::SIM::SIM1 == sim) {
                    simParams->setSim(Store::GSM::SIM::SIM2);
                }
                else {
                    simParams->setSim(Store::GSM::SIM::SIM1);
                }

                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::localize.get(style::strings::common::Switch),
                                           BottomBar::Side::CENTER);
                }
                else {
                    this->setBottomBarText(utils::localize.get(style::strings::common::select),
                                           BottomBar::Side::CENTER);
                }
                return true;
            },
            this));

        optList.emplace_back(std::make_unique<gui::OptionSettings>(
            utils::translateI18("app_settings_network_operator_auto_select"),
            [=](gui::Item &item) {
                operatorsSettings->setOperatorsOn(!operatorsOn);
                rebuild();
                return true;
            },
            nullptr,
            nullptr,
            operatorsOn ? RightItem::On : RightItem::Off));
        if (!operatorsOn) {
            optList.emplace_back(std::make_unique<gui::OptionSettings>(
                utils::translateI18("app_settings_network_all_operators"),
                [=](gui::Item &item) {
                    this->application->switchWindow(gui::window::name::all_operators, nullptr);
                    return true;
                },
                nullptr,
                nullptr,
                RightItem::ArrowWhite,
                true));
        }
        optList.emplace_back(std::make_unique<gui::OptionSettings>(
            utils::translateI18("app_settings_network_import_contacts_from_sim_card"),
            [=](gui::Item &item) {
                this->application->switchWindow(gui::window::name::import_contacts, nullptr);
                return true;
            },
            nullptr,
            nullptr));

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));

        topBar->setActive(TopBar::Elements::SIGNAL, false);
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::SIM, false);
        return optList;
    }
    void NetworkWindow::rebuild()
    {
        clearOptions();
        addOptions(netOptList());
    }

} // namespace gui
