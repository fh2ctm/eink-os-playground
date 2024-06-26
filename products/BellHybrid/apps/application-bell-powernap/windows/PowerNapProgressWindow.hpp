// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "presenter/PowerNapProgressPresenter.hpp"
#include <apps-common/widgets/BarGraph.hpp>
#include <apps-common/widgets/TimeMinuteSecondWidget.hpp>
#include <common/widgets/BellStatusClock.hpp>
#include <gui/widgets/Icon.hpp>
#include <AppWindow.hpp>
#include <Application.hpp>
#include <InputEvent.hpp>
#include <Text.hpp>

namespace gui
{
    class Text;
    class PowerNapProgressWindow : public AppWindow, public app::powernap::PowerNapProgressContract::View
    {
      public:
        PowerNapProgressWindow(app::ApplicationCommon *app,
                               std::shared_ptr<app::powernap::PowerNapProgressContract::Presenter> &&windowPresenter);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;
        void buildInterface() override;
        void progressFinished() override;
        void pause() override;
        void resume() override;

      private:
        std::shared_ptr<app::powernap::PowerNapProgressContract::Presenter> presenter;
        VBox *mainVBox{nullptr};
        ArcProgressBar *progress{nullptr};
        TimeMinuteSecondWidget *timer{nullptr};
        BellStatusClock *clock{nullptr};
        Icon *iconPause{nullptr};
        Icon *iconRing{nullptr};

        void setTime(std::time_t newTime) override;
        void setTimeFormat(utils::time::Locale::TimeFormat fmt) override;
        RefreshModes updateTime() override;
        void buildLayout();
        void configureTimer();

        void napEnded() override;
    };
} // namespace gui
