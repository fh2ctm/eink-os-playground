// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <limits>
#include <list>

#include <ContactRecord.hpp>

namespace notifications
{
    enum class NotificationType
    {
        Unknown,
        NotSeenSms,
        NotSeenCall,
        Tethering
    };

    enum class NotificationPriority
    {
        Next,
        Highest,
        Lowest
    };

    class Notification
    {
        static constexpr auto highestPriority = std::numeric_limits<uint32_t>::max();
        static constexpr auto lowestPriority  = 0;
        static uint32_t priorityPool;

        NotificationType type;
        uint32_t priority;

      protected:
        explicit Notification(NotificationType type, NotificationPriority priorityType = NotificationPriority::Next);

      public:
        [[nodiscard]] auto getType() const noexcept -> NotificationType;
        [[nodiscard]] auto getPriority() const noexcept -> uint32_t;

        virtual ~Notification() = default;
    };

    class NotificationWithContact : public Notification
    {
        unsigned value = 0;
        std::optional<ContactRecord> record;

      protected:
        NotificationWithContact(NotificationType type, unsigned value, std::optional<ContactRecord> record);

      public:
        [[nodiscard]] auto hasRecord() const noexcept -> bool;
        [[nodiscard]] auto getRecord() const noexcept -> const ContactRecord &;
        [[nodiscard]] auto getValue() const noexcept -> unsigned;
    };

    class NotSeenSMSNotification : public NotificationWithContact
    {
      public:
        NotSeenSMSNotification(unsigned value, std::optional<ContactRecord> record);
    };

    class NotSeenCallNotification : public NotificationWithContact
    {
      public:
        NotSeenCallNotification(unsigned value, std::optional<ContactRecord> record);
    };

    class TetheringNotification : public Notification
    {
      public:
        TetheringNotification();
    };

} // namespace notifications
