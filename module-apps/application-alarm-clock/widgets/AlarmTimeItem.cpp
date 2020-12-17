// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmTimeItem.hpp"
#include "AlarmClockStyle.hpp"
#include <ListView.hpp>
#include <Style.hpp>
#include <time/time_conversion.hpp>
#include <time/time_date_validation.hpp>

namespace gui
{
    namespace timeItem = style::alarmClock::window::item::time;

    AlarmTimeItem::AlarmTimeItem(bool mode24H,
                                 std::function<void(const UTF8 &text)> bottomBarTemporaryMode,
                                 std::function<void()> bottomBarRestoreFromTemporaryMode)
        : mode24H{mode24H}, bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        setMinimumSize(style::window::default_body_width, timeItem::height);

        setEdges(RectangleEdge::None);
        setMargins(gui::Margins(style::margins::small, timeItem::margin, 0, timeItem::marginBot));

        hBox = new gui::HBox(this, 0, 0, 0, 0);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->activeItem = false;

        hourInput = new gui::Text(hBox, 0, 0, 0, 0);
        applyItemSpecificProperties(hourInput);

        colonLabel = new gui::Label(hBox, 0, 0, 0, 0);
        colonLabel->setMinimumSize(timeItem::separator, timeItem::height - timeItem::separator);
        colonLabel->setEdges(gui::RectangleEdge::None);
        colonLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        colonLabel->setFont(style::window::font::medium);
        colonLabel->setText(":");
        colonLabel->activeItem = false;

        minuteInput = new gui::Text(hBox, 0, 0, 0, 0);
        applyItemSpecificProperties(minuteInput);

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? hBox : nullptr);
            if (!item.focus) {
                validateHour();
            }
            return true;
        };

        applyInputCallbacks();
        prepareForTimeMode();
    }

    void AlarmTimeItem::applyItemSpecificProperties(gui::Text *item)
    {
        item->setEdges(gui::RectangleEdge::Bottom);
        item->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        item->setFont(style::window::font::largelight);
        item->setInputMode(new InputMode({InputMode::digit}));
        item->setPenFocusWidth(style::window::default_border_focus_w);
        item->setPenWidth(style::window::default_border_rect_no_focus);
        item->setEditMode(gui::EditMode::Edit);
    }

    void AlarmTimeItem::applyInputCallbacks()
    {
        inputCallback = [&](Item &item, const InputEvent &event) {
            auto focusedItem = getFocusItem();
            if (!event.isShortPress()) {
                return false;
            }
            if (event.is(gui::KeyCode::KEY_ENTER) || event.is(gui::KeyCode::KEY_RF)) {
                return false;
            }

            if (focusedItem->onInput(event)) {
                uint32_t hours;
                try {
                    hours = std::stoi(hourInput->getText().c_str());
                }
                catch (const std::exception &e) {
                    LOG_INFO("AlarmTimeItem hours not valid: %s", e.what());
                    hours = 0;
                }
                uint32_t minutes;
                try {
                    minutes = std::stoi(minuteInput->getText().c_str());
                }
                catch (const std::exception &e) {
                    LOG_INFO("AlarmTimeItem minutes not valid: %s", e.what());
                    minutes = 0;
                }

                if (mode24H && hours > utils::time::hoursInday - 1) {
                    hourInput->setText("0");
                }
                else if (!mode24H && hours > utils::time::hoursInday / 2) {
                    hourInput->setText("12");
                }
                if (minutes > utils::time::minutesInHour - 1) {
                    minuteInput->setText("00");
                }
                return true;
            }
            else if (hBox->onInput(event)) {
                return true;
            }

            return false;
        };

        onSaveCallback = [&](std::shared_ptr<AlarmsRecord> record) {
            validateHour();
            auto hours   = std::chrono::hours(std::stoi(hourInput->getText().c_str()));
            auto minutes = std::chrono::minutes(std::stoi(minuteInput->getText().c_str()));
            if (!mode24H) {
                hours = date::make24(hours, isPm(mode12hInput->getText()));
            }
            record->time = TimePointFromYearMonthDay(TimePointToYearMonthDay(TimePointNow())) + hours + minutes;
        };

        onInputCallback(*hourInput);
        onInputCallback(*minuteInput);
    }

    void AlarmTimeItem::onInputCallback(gui::Text &textItem)
    {
        textItem.inputCallback = [&](Item &item, const InputEvent &event) {
            if (!event.isShortPress()) {
                return false;
            }
            if (textItem.getText().length() > 1 && !event.is(gui::KeyCode::KEY_LEFT) &&
                !event.is(gui::KeyCode::KEY_RIGHT) && !event.is(gui::KeyCode::KEY_PND) &&
                !event.is(gui::KeyCode::KEY_UP) && !event.is(gui::KeyCode::KEY_DOWN)) {
                return true;
            }
            return false;
        };
    }

    void AlarmTimeItem::prepareForTimeMode()
    {
        if (!mode24H) {
            mode12hInput = new gui::Label(hBox, 0, 0, 0, 0);
            mode12hInput->setEdges(gui::RectangleEdge::Bottom);
            mode12hInput->setAlignment(
                gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
            mode12hInput->setFont(style::window::font::largelight);
            mode12hInput->setPenFocusWidth(style::window::default_border_focus_w);
            mode12hInput->setPenWidth(style::window::default_border_rect_no_focus);
            mode12hInput->setText(utils::localize.get(utils::time::Locale::getAM()));
            mode12hInput->inputCallback = [&](Item &item, const InputEvent &event) {
                if (event.state != gui::InputEvent::State::keyReleasedShort) {
                    return false;
                }
                if (event.keyCode == gui::KeyCode::KEY_LF) {
                    if (mode12hInput->getText() == utils::localize.get(utils::time::Locale::getAM())) {
                        mode12hInput->setText(utils::localize.get(utils::time::Locale::getPM()));
                    }
                    else {
                        mode12hInput->setText(utils::localize.get(utils::time::Locale::getAM()));
                    }
                    return true;
                }
                return false;
            };
            mode12hInput->focusChangedCallback = [&](Item &item) {
                if (item.focus) {
                    bottomBarTemporaryMode(utils::localize.get("common_switch"));
                }
                else {
                    bottomBarRestoreFromTemporaryMode();
                }
                return true;
            };

            mode12hInput->setMinimumSize(timeItem::timeInput12h, timeItem::height - timeItem::separator);
            mode12hInput->setMargins(gui::Margins(timeItem::separator, 0, 0, 0));
            hourInput->setMinimumSize(timeItem::timeInput12h, timeItem::height - timeItem::separator);
            minuteInput->setMinimumSize(timeItem::timeInput12h, timeItem::height - timeItem::separator);

            onLoadCallback = [&](std::shared_ptr<AlarmsRecord> alarm) {
                hourInput->setText(TimePointToHourString12H(alarm->time));
                minuteInput->setText(TimePointToMinutesString(alarm->time));
                if (date::is_am(TimePointToHourMinSec(alarm->time).hours())) {
                    mode12hInput->setText(utils::localize.get(utils::time::Locale::getAM()));
                }
                else {
                    mode12hInput->setText(utils::localize.get(utils::time::Locale::getPM()));
                }
            };
        }
        else {
            hourInput->setMinimumSize(timeItem::timeInput24h, timeItem::height - timeItem::separator);
            minuteInput->setMinimumSize(timeItem::timeInput24h, timeItem::height - timeItem::separator);

            onLoadCallback = [&](std::shared_ptr<AlarmsRecord> alarm) {
                hourInput->setText(TimePointToHourString24H(alarm->time));
                minuteInput->setText(TimePointToMinutesString(alarm->time));
            };
        }
    }

    bool AlarmTimeItem::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        hBox->setPosition(0, 0);
        hBox->setSize(newDim.w, newDim.h);
        return true;
    }

    bool AlarmTimeItem::isPm(const std::string &text) const
    {
        return !(text == utils::localize.get(utils::time::Locale::getAM()));
    }

    void AlarmTimeItem::validateHour()
    {
        if (!utils::time::validateTime(hourInput->getText(), minuteInput->getText(), !mode24H)) {
            hourInput->setText("0");
            minuteInput->setText("00");
        }
    }

} /* namespace gui */