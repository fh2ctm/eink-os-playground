// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-onboarding/ApplicationOnBoarding.hpp"
#include "OnBoardingLanguagesWindow.hpp"
#include <module-gui/gui/input/InputEvent.hpp>

namespace gui
{
    OnBoardingLanguagesWindow::OnBoardingLanguagesWindow(app::Application *app)
        : LanguagesWindow(app, window::name::onBoarding_languages)
    {}

    void OnBoardingLanguagesWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, false);

        LanguagesWindow::onBeforeShow(mode, data);
    }

    bool OnBoardingLanguagesWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress() && inputEvent.is(KeyCode::KEY_RF)) {
            return true;
        }
        else {
            return LanguagesWindow::onInput(inputEvent);
        }
    }
} /* namespace gui */