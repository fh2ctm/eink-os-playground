// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmSetSpinner.hpp"
#include <FontManager.hpp>
#include <RawFont.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/ImageBox.hpp>
#include <apps-common/widgets/TimeSetFmtSpinner.hpp>

namespace gui
{
    AlarmSetSpinner::AlarmSetSpinner(Item *parent, Position x, Position y, Length w, Length h)
        : HBox(parent, x, y, w, h)
    {
        setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        setEdges(RectangleEdge::None);

        leftArrow = new ImageBox(this, 0, 0, 0, 0, new Image("bell_arrow_left"));
        leftArrow->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        leftArrow->setMargins(Margins(0, 0, 0, 0));
        leftArrow->setMinimumSize(style::alarm_set_spinner::arrow::w, style::alarm_set_spinner::arrow::h);

        alarmImg = new ImageBox(this, 0, 0, 0, 0, new Image("bell_alarm_deactivated"));
        alarmImg->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        alarmImg->setMargins(Margins(0, 0, 0, 0));
        alarmImg->setMinimumSize(style::alarm_set_spinner::arrow::w, style::alarm_set_spinner::arrow::h);

        timeSpinner = new TimeSetFmtSpinner(this);
        timeSpinner->setFont(style::window::font::largelight);
        timeSpinner->setEditMode(EditMode::Browse);
        timeSpinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        timeSpinner->setMargins(Margins(0, 0, 0, 0));

        rightArrow = new ImageBox(this, 0, 0, 0, 0, new Image("bell_arrow_right"));
        rightArrow->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        rightArrow->setMargins(Margins(0, 0, 0, 0));
        rightArrow->setMinimumSize(style::alarm_set_spinner::arrow::w, style::alarm_set_spinner::arrow::h);

        resizeItems();

        focusChangedCallback = [&](Item &) {
            if (focus && editMode == EditMode::Edit) {
                setFocusItem(timeSpinner);
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };
    }

    auto AlarmSetSpinner::onInput(const InputEvent &inputEvent) -> bool
    {
        // Ignore input event when not in edit mode
        if (editMode != EditMode::Edit) {
            return false;
        }
        return this->focusItem->onInput(inputEvent);
    }

    auto AlarmSetSpinner::setFont(std::string newFontName) noexcept -> void
    {
        fontName = std::move(newFontName);
        timeSpinner->setFont(fontName);
    }

    auto AlarmSetSpinner::setEditMode(EditMode mode) noexcept -> void
    {
        editMode = mode;
        if (editMode == EditMode::Edit) {
            leftArrow->setVisible(true);
            rightArrow->setVisible(true);
        }
        else {
            setFocusItem(nullptr);
            leftArrow->setVisible(false);
            rightArrow->setVisible(false);
        }
        resizeItems();
    }

    auto AlarmSetSpinner::getHour() const noexcept -> int
    {
        return timeSpinner->getHour();
    }

    auto AlarmSetSpinner::getMinute() const noexcept -> int
    {
        return timeSpinner->getMinute();
    }

    auto AlarmSetSpinner::setAlarmStatus(Status status) noexcept -> void
    {
        alarmStatus = status;

        switch (alarmStatus) {
        case Status::ACTIVATED:
            alarmImg->setImage("bell_alarm_activated");
            break;
        case Status::DEACTIVATED:
            alarmImg->setImage("bell_alarm_deactivated");
            break;
        case Status::RINGING:
            alarmImg->setImage("bell_alarm_ringing");
            break;
        case Status::SNOOZE:
            alarmImg->setImage("bell_alarm_snooze");
            break;
        case Status::UNKNOWN:
            alarmImg->setImage("bell_alarm_deactivated");
            break;
        default:
            alarmImg->setImage("bell_alarm_deactivated");
            break;
        }
        resizeItems();
    }

    auto AlarmSetSpinner::setAlarmTimeVisible(bool value) noexcept -> void
    {
        timeSpinner->setVisible(value);
        timeSpinner->setFocusItem(nullptr);
    }

    auto AlarmSetSpinner::setTimeFormat(utils::time::Locale::TimeFormat fmt) noexcept -> void
    {
        timeSpinner->setTimeFormat(fmt);
        resizeItems();
    }

    auto AlarmSetSpinner::getAlarmStatus() const noexcept -> Status
    {
        return alarmStatus;
    }
    auto AlarmSetSpinner::isPM() const noexcept -> bool
    {
        return timeSpinner->isPM();
    }
    auto AlarmSetSpinner::setTime(std::time_t time) noexcept -> void
    {
        timeSpinner->setTime(time);
    }
    auto AlarmSetSpinner::getTime() const noexcept -> std::time_t
    {
        return timeSpinner->getTime();
    }
    auto AlarmSetSpinner::setMinute(std::uint8_t minute) noexcept -> void
    {
        timeSpinner->setMinute(minute);
    }

} /* namespace gui */