// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-alarm/AlarmMessage.hpp>

#include <apps-common/AsyncTask.hpp>
#include <module-sys/Service/Service.hpp>

#include <ctime>
#include <vector>

class EventRecord;
class SingleEvent;
class AlarmEventRecord;

/**
 * @brief Basic interface alarm API
 */
namespace alarmEvents
{

    class AbstractAlarmEventsRepository
    {
      public:
        using OnGetAlarmEventCallback          = std::function<void(std::vector<AlarmEventRecord>)>;
        using OnGetAlarmEventsCallback         = std::function<void(std::vector<AlarmEventRecord>)>;
        using OnGetAlarmEventsInRangeCallback  = std::function<void(std::vector<AlarmEventRecord>)>;
        using OnAddAlarmEventCallback          = std::function<void(bool, AlarmEventRecord)>;
        using OnUpdateAlarmEventCallback       = std::function<void(bool, AlarmEventRecord)>;
        using OnRemoveAlarmEventCallback       = std::function<void(bool)>;
        using OnGetSingleEventsInRangeCallback = std::function<void(std::vector<SingleEvent>)>;
        using OnGetNextSingleEventsCallback    = std::function<void(std::vector<SingleEvent>)>;
        using OnGetAlarmsFromNowCallback       = std::function<void(unsigned int)>;

      public:
        /**
         * Default destructor
         */
        virtual ~AbstractAlarmEventsRepository() noexcept = default;

        /**
         * Get alarm from the alarm repository
         * @param AlarmEventRecord Id in the alarm repository
         * @retval alarm status and Event structure
         */
        virtual void getAlarmEvent(const unsigned int alarmId, OnGetAlarmEventCallback callback) = 0;

        /**
         * Get alarms Ids from the alarm repository
         * @param offset of the first alarm in the alarm repo
         * @param max number of alarms ids to be returned
         */
        virtual void getAlarmEvents(unsigned int offset, unsigned int limit, OnGetAlarmEventsCallback callback) = 0;

        /**
         * Get alarms from the alarm repository
         * @param start time point for first alarm to be returned
         * @param end time point for last alarm to be returned
         */
        virtual void getAlarmEventsInRange(std::chrono::time_point<std::chrono::system_clock> start,
                                           std::chrono::time_point<std::chrono::system_clock> end,
                                           OnGetAlarmEventsInRangeCallback callback) = 0;

        /**
         * Add alarm to the alarm repository
         * @param AlarmEventRecord structure
         * @retval operation status
         */
        virtual void addAlarmEvent(const AlarmEventRecord &alarmEvent, OnAddAlarmEventCallback callback) = 0;

        /**
         * Update alarm in the alarm repository
         * @param AlarmEventRecord structure
         * @retval operation status
         */
        virtual void updateAlarmEvent(const AlarmEventRecord &alarmEvent, OnUpdateAlarmEventCallback callback) = 0;

        /**
         * Remove alarm from the alarm repository
         * @param id of the alarm to be deleted
         * @retval operation status
         */
        virtual void removeAlarmEvent(const unsigned int id, OnRemoveAlarmEventCallback callback) = 0;

        /**
         * Get single alarms generated by alarms from the alarm repository
         * @param start time point for first single alarm to be returned
         * @param end time point for last single alarm to be returned
         */
        virtual void getSingleEventsInRange(std::chrono::time_point<std::chrono::system_clock> start,
                                            std::chrono::time_point<std::chrono::system_clock> end,
                                            OnGetSingleEventsInRangeCallback callback) = 0;

        /**
         * Get next single alarms generated by alarms from the alarm repository
         * There might be multiple alarms with same timestamp
         * @param start time point for first single alarm to be returned
         * @param end time point for last single alarm to be returned
         */
        virtual void getNextSingleEvents(OnGetNextSingleEventsCallback callback) = 0;

        /**
         * Get alarm count in the alarm repository
         * @retval number of alarms in the alarm repository
         */
        virtual unsigned int getAlarmsFromNowCount(OnGetAlarmsFromNowCallback callback) = 0;
    };

    class AlarmEventsRepository : public AbstractAlarmEventsRepository, public app::AsyncCallbackReceiver
    {
      public:
        AlarmEventsRepository(sys::Service *service);

        void getAlarmEvent(const unsigned int alarmId, OnGetAlarmEventCallback callback) override;
        void getAlarmEvents(unsigned int offset, unsigned int limit, OnGetAlarmEventsCallback callback) override;
        void getAlarmEventsInRange(std::chrono::time_point<std::chrono::system_clock> start,
                                   std::chrono::time_point<std::chrono::system_clock> end,
                                   OnGetAlarmEventsInRangeCallback callback) override;
        void addAlarmEvent(const AlarmEventRecord &alarmEvent, OnAddAlarmEventCallback callback) override;
        void updateAlarmEvent(const AlarmEventRecord &alarmEvent, OnUpdateAlarmEventCallback callback) override;
        void removeAlarmEvent(const unsigned int id, OnRemoveAlarmEventCallback callback) override;
        void getSingleEventsInRange(std::chrono::time_point<std::chrono::system_clock> start,
                                    std::chrono::time_point<std::chrono::system_clock> end,
                                    OnGetSingleEventsInRangeCallback callback) override;
        void getNextSingleEvents(OnGetNextSingleEventsCallback callback) override;
        unsigned int getAlarmsFromNowCount(OnGetAlarmsFromNowCallback callback) override;
    };
} // namespace alarmEvents
