// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <popups/WindowWithTimer.hpp>
#include <Text.hpp>
#include <gui/widgets/Image.hpp>

namespace gui
{
    class PhoneLockChangeInfoWindow : public WindowWithTimer
    {
        gui::Image *infoImage = nullptr;
        gui::Text *infoText   = nullptr;

      public:
        PhoneLockChangeInfoWindow(app::Application *app, const std::string &name);

        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;
    };
} /* namespace gui */