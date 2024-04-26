// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "MeditationProgressPresenter.hpp"
#include "models/ChimeInterval.hpp"
#include "models/Statistics.hpp"

#include <common/models/TimeModel.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <audio/AudioMessage.hpp>
#include <service-db/Settings.hpp>
#include <LanguageUtils.hpp>

namespace
{
    constexpr std::chrono::minutes emptyValue{0};
    constexpr std::chrono::seconds endWindowTimeout{6};

    std::chrono::seconds to_interval(double ratio, std::chrono::minutes total_duration)
    {
        const long interval = std::chrono::seconds{total_duration}.count() * ratio;
        return std::chrono::seconds{interval};
    }
} // namespace

namespace app::meditation
{
    MeditationProgressPresenter::MeditationProgressPresenter(app::ApplicationCommon *app,
                                                             settings::Settings *settings,
                                                             std::unique_ptr<AbstractTimeModel> timeModel,
                                                             models::ChimeInterval &chimeIntervalModel,
                                                             models::Statistics &statisticsModel)
        : app{app}, settings{settings}, timeModel{std::move(timeModel)}, chimeIntervalModel{chimeIntervalModel},
          statisticsModel{statisticsModel}
    {
        duration = std::chrono::minutes{
            utils::getNumericValue<int>(settings->getValue(meditationDBRecordName, settings::SettingsScope::AppLocal))};

        interval = to_interval(this->chimeIntervalModel.getValue().to_double(), duration);
    }

    void MeditationProgressPresenter::setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer)
    {
        timer = std::move(_timer);
        timer->registerOnFinishedCallback([this]() { onProgressFinished(); });
        if (interval != emptyValue) {
            timer->registerOnIntervalCallback([this]() { onIntervalReached(); });
        }
    }

    void MeditationProgressPresenter::handleUpdateTimeEvent()
    {
        getView()->setTime(timeModel->getCurrentTime());
    }

    bool MeditationProgressPresenter::isTimerStopped()
    {
        return timer->isStopped();
    }

    void MeditationProgressPresenter::start()
    {
        static_cast<app::Application *>(app)->suspendIdleTimer();
        timer->reset(std::chrono::seconds(duration), interval);
        timer->start();
    }

    void MeditationProgressPresenter::stop()
    {
        finish();
    }

    void MeditationProgressPresenter::pause()
    {
        timer->stop();
        getView()->pause();
    }

    void MeditationProgressPresenter::resume()
    {
        timer->start();
        getView()->resume();
    }

    void MeditationProgressPresenter::abandon()
    {
        timer->stop();
        const auto elapsed     = std::chrono::duration_cast<std::chrono::minutes>(timer->getElapsed());
        const auto summaryText = utils::translate("app_meditation_summary") + std::to_string(elapsed.count()) + " " +
                                 utils::language::getCorrectMinutesNumeralForm(elapsed.count());

        addMeditationEntry(elapsed);

        app->switchWindow(
            gui::window::bell_finished::defaultName,
            gui::BellFinishedWindowData::Factory::create("big_namaste_W_G",
                                                         "MeditationTimerWindow",
                                                         summaryText,
                                                         gui::BellFinishedWindowData::ExitBehaviour::SwitchWindow,
                                                         endWindowTimeout));
    }

    void MeditationProgressPresenter::finish()
    {
        timer->stop();
        const auto elapsed     = std::chrono::duration_cast<std::chrono::minutes>(timer->getElapsed());
        const auto summaryText = utils::translate("app_meditation_summary") + std::to_string(elapsed.count()) + " " +
                                 utils::language::getCorrectMinutesAccusativeForm(elapsed.count());

        addMeditationEntry(elapsed);

        app->switchWindow(
            gui::window::bell_finished::defaultName,
            gui::BellFinishedWindowData::Factory::create("big_namaste_W_G",
                                                         "",
                                                         summaryText,
                                                         gui::BellFinishedWindowData::ExitBehaviour::ReturnToHomescreen,
                                                         endWindowTimeout));
    }

    void MeditationProgressPresenter::onBeforeShow()
    {
        getView()->setTimeFormat(timeModel->getTimeFormat());
    }

    void MeditationProgressPresenter::playGongSound()
    {
        auto msg = std::make_shared<service::AudioStartPlaybackRequest>(app::meditation::getMeditationGongSoundPath(),
                                                                        audio::PlaybackType::Meditation);
        app->bus.sendUnicast(std::move(msg), service::audioServiceName);
    }

    void MeditationProgressPresenter::playEndSound()
    {
        auto msg = std::make_shared<service::AudioStartPlaybackRequest>(app::meditation::getMeditationEndSoundPath(),
                                                                        audio::PlaybackType::Meditation);
        app->bus.sendUnicast(std::move(msg), service::audioServiceName);
    }

    void MeditationProgressPresenter::onProgressFinished()
    {
        getView()->progressFinished();
        finish();
    }

    void MeditationProgressPresenter::onIntervalReached()
    {
        if (interval != emptyValue) {
            getView()->intervalReached();
        }
    }

    void MeditationProgressPresenter::addMeditationEntry(const std::chrono::minutes elapsed)
    {
        if (elapsed > std::chrono::minutes::zero()) {
            statisticsModel.addEntry(elapsed);
        }
    }
} // namespace app::meditation
