// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * Window.cpp
 *
 *  Created on: 6 mar 2019
 *      Author: robert
 */
#include <algorithm>
// gui
#include "../Common.hpp"
#include "../core/BoundingBox.hpp"
#include "../core/DrawCommand.hpp"
#include "Window.hpp"
#include <InputEvent.hpp>

namespace gui
{
    Window::Window(std::string name) : Item(), name{name}
    {}

    void Window::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    void Window::onClose()
    {}

    void Window::getRefreshArea(uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h)
    {
        x    = widgetArea.x;
        y    = widgetArea.y;
        w    = widgetArea.w;
        h    = widgetArea.h;
    }

    bool Window::handleSwitchData(SwitchData *data)
    {
        return false;
    }

    void Window::buildDrawListImplementation(std::list<Command> &commands)
    {
        auto clearCommand         = std::make_unique<DrawCommand>();
        clearCommand->id          = DrawCommandID::GUI_DRAW_CLEAR;
        commands.emplace_back(std::move(clearCommand));
    }

    bool Window::onInput(const InputEvent &inputEvent)
    {
        if (focusItem != nullptr && focusItem->onInput(inputEvent)) {
            return true;
        }
        if (inputCallback != nullptr && inputCallback(*this, inputEvent)) {
            return true;
        }
        if (handleNavigation(inputEvent)) {
            return true;
        }
        return inputEvent.state == InputEvent::State::keyReleasedShort &&
               inputEvent.keyCode == gui::KeyCode::KEY_ENTER && onActivated(nullptr);
    }

    void Window::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }
} /* namespace gui */
