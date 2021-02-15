// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "CalendarListItem.hpp"
#include "EventDateItem.hpp"
#include <Label.hpp>
#include <Text.hpp>
#include <BoxLayout.hpp>

namespace gui
{
    namespace timeConstants
    {
        inline constexpr auto before_noon = "AM";
        inline constexpr auto after_noon  = "PM";
    } // namespace timeConstants
    class EventTimeItem : public CalendarListItem
    {
        gui::VBox *vBox                = nullptr;
        gui::HBox *hBox                = nullptr;
        gui::Label *colonLabel         = nullptr;
        gui::Label *descriptionLabel   = nullptr;
        gui::Label *hourInput          = nullptr;
        gui::Label *minuteInput        = nullptr;
        gui::Label *mode12hInput       = nullptr;
        bool mode24H                   = false;
        gui::EventTimeItem *secondItem = nullptr;
        gui::EventDateItem *dateItem   = nullptr;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

        void applyInputCallbacks();
        void prepareForTimeMode();
        void setTime(int keyValue, gui::Label &item);
        void onInputCallback(gui::Label &timeInput);
        void clearInput(gui::Label &timeInput);
        bool isPm(const std::string &text);
        void validateHour();
        void validateHourFor12hMode(std::chrono::hours start_hour,
                                    std::chrono::minutes end_hour,
                                    uint32_t start_minutes,
                                    uint32_t end_minutes);
        void validateHourFor24hMode(std::chrono::hours start_hour,
                                    std::chrono::minutes end_hour,
                                    uint32_t start_minutes,
                                    uint32_t end_minutes);
        TimePoint calculateEventTime(calendar::YearMonthDay date,
                                     std::chrono::hours hours,
                                     std::chrono::minutes minutes);

      public:
        EventTimeItem(const std::string &description,
                      bool mode24H,
                      std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                      std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~EventTimeItem() override = default;

        void setConnectionToSecondItem(gui::EventTimeItem *item);
        void setConnectionToDateItem(gui::EventDateItem *item);
    };

} /* namespace gui */
