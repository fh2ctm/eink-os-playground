// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/WindowWithTimer.hpp>
#include <AsyncTask.hpp>

namespace app
{
    class ApplicationCommon;
}
namespace gui
{
    namespace charging_notification
    {} // namespace charging_notification

    class Icon;

    class ChargingNotificationWindow : public WindowWithTimer, public app::AsyncCallbackReceiver
    {
      protected:
        app::ApplicationCommon *app;
        Icon *icon = nullptr;

        bool onInput(const InputEvent &inputEvent) override;
        void returnToPreviousWindow();
        void buildInterface() override;

      public:
        explicit ChargingNotificationWindow(app::ApplicationCommon *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

    class ChargingDoneNotificationWindow : public ChargingNotificationWindow
    {
      public:
        explicit ChargingDoneNotificationWindow(app::ApplicationCommon *app);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
} /* namespace gui */
