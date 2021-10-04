// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationBellSettings.hpp"
#include "presenter/BedtimeSettingsPresenter.hpp"
#include <apps-common/windows/AppWindow.hpp>

namespace gui
{
    class SideListView;

    class BellSettingsBedtimeToneWindow : public AppWindow,
                                          public app::bell_settings::BedtimeSettingsWindowContract::View
    {
      public:
        static constexpr auto name = "BellSettingsBedtimeToneWindow";
        explicit BellSettingsBedtimeToneWindow(
            app::ApplicationCommon *app,
            std::unique_ptr<app::bell_settings::BedtimeSettingsPresenter::Presenter> presenter);

        void buildInterface() override;
        void onClose(CloseReason reason) override;
        bool onInput(const InputEvent &inputEvent) override;
        void rebuild() override;
        void exit() override;

      private:
        SideListView *sidelistview{};
        std::unique_ptr<app::bell_settings::BedtimeSettingsPresenter::Presenter> presenter;
    };
} /* namespace gui */
