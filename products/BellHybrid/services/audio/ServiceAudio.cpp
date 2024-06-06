﻿// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceAudio.hpp"
#include "system/Constants.hpp"

#include <audio/AudioMessage.hpp>
#include <service-db/Settings.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>

namespace
{
    // 4kB is too small because internally drflac_open() uses cache which by default has 4kB.
    // Alternatively smaller DR_FLAC_BUFFER_SIZE could be defined.
    constexpr auto serviceAudioStackSize = 1024 * 8;
    constexpr auto defaultVolume         = "11";
    constexpr auto defaultSnoozeVolume   = "10";
    constexpr auto defaultBedtimeVolume  = "12";
    constexpr auto maxVolumeToSet        = 15.f;
    constexpr auto minVolumeToSet        = 0.f;
    constexpr auto profileType           = audio::Profile::Type::PlaybackLoudspeaker;
    constexpr auto volumeSetting         = audio::Setting::Volume;

    namespace initializer
    {
        using namespace audio;
        // clang-format off
        constexpr std::initializer_list<std::pair<audio::DbPathElement, const char *>> values{
            {DbPathElement{Setting::Volume, PlaybackType::Meditation, Profile::Type::PlaybackLoudspeaker}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackLoudspeaker}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Alarm, Profile::Type::PlaybackLoudspeaker}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Bedtime, Profile::Type::PlaybackLoudspeaker}, defaultBedtimeVolume},
            {DbPathElement{Setting::Volume, PlaybackType::PreWakeUp, Profile::Type::PlaybackLoudspeaker}, defaultSnoozeVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Snooze, Profile::Type::PlaybackLoudspeaker}, defaultSnoozeVolume},
            {DbPathElement{Setting::Volume, PlaybackType::FocusTimer, Profile::Type::PlaybackLoudspeaker}, defaultVolume},

            /// Profiles below are not used but unfortunately, must exist in order to satisfy audio module requirements
            {DbPathElement{Setting::Volume, PlaybackType::Meditation, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Meditation, Profile::Type::PlaybackBluetoothA2DP}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Multimedia, Profile::Type::PlaybackBluetoothA2DP}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Alarm, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Alarm, Profile::Type::PlaybackBluetoothA2DP}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Bedtime, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Bedtime, Profile::Type::PlaybackBluetoothA2DP},defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::PreWakeUp, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::PreWakeUp, Profile::Type::PlaybackBluetoothA2DP},defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Snooze, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::Snooze, Profile::Type::PlaybackBluetoothA2DP}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::FocusTimer, Profile::Type::PlaybackHeadphones}, defaultVolume},
            {DbPathElement{Setting::Volume, PlaybackType::FocusTimer, Profile::Type::PlaybackBluetoothA2DP}, defaultVolume},
        };
        // clang-format on
    } // namespace initializer

    namespace internal
    {
        class AudioEOFNotificationMessage : public service::AudioNotificationMessage
        {
          public:
            explicit AudioEOFNotificationMessage(audio::Token token) : AudioNotificationMessage(token)
            {}
        };

        class AudioFileDeletedNotificationMessage : public service::AudioNotificationMessage
        {
          public:
            explicit AudioFileDeletedNotificationMessage(audio::Token token) : AudioNotificationMessage(token)
            {}
        };
    } // namespace internal
} // namespace

namespace service
{
    Audio::Audio()
        : sys::Service(audioServiceName, "", serviceAudioStackSize, sys::ServicePriority::Idle),
          audioMux([this](auto... params) { return AudioServicesCallback(params...); }),
          cpuSentinel(std::make_shared<sys::CpuSentinel>(audioServiceName, this)),
          settingsProvider(std::make_unique<settings::Settings>())
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);

        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
        bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);

        auto callback = [this](float volumeToSet) {
            const auto clampedValue = std::clamp(volumeToSet, minVolumeToSet, maxVolumeToSet);
            if (const auto activeInput = audioMux.GetActiveInput(); activeInput) {
                if (activeInput.value()) {
                    activeInput.value()->audio->SetOutputVolume(clampedValue);
                }
            }
        };
        volumeFadeIn = std::make_unique<audio::VolumeFadeIn>(this, std::move(callback));

        connect(typeid(AudioStartPlaybackRequest), [this](sys::Message *msg) -> sys::MessagePointer {
            auto *msgl = static_cast<AudioStartPlaybackRequest *>(msg);
            return handleStart(audio::Operation::Type::Playback, msgl->fadeIn, msgl->fileName, msgl->playbackType);
        });

        connect(typeid(internal::AudioEOFNotificationMessage), [this](sys::Message *msg) -> sys::MessagePointer {
            auto *msgl = static_cast<internal::AudioEOFNotificationMessage *>(msg);
            handleEOF(msgl->token);
            return sys::msgHandled();
        });

        connect(typeid(internal::AudioFileDeletedNotificationMessage),
                [this](sys::Message *msg) -> sys::MessagePointer {
                    auto *msgl = static_cast<internal::AudioEOFNotificationMessage *>(msg);
                    handleFileDeleted(msgl->token);
                    return sys::msgHandled();
                });

        connect(typeid(AudioStopRequest), [this](sys::Message *msg) -> sys::MessagePointer {
            volumeFadeIn->Stop();
            auto *msgl = static_cast<AudioStopRequest *>(msg);
            return handleStop(msgl->stopVec, msgl->token);
        });

        connect(typeid(AudioSetVolume), [this](sys::Message *msg) -> sys::MessagePointer {
            auto *msgl = static_cast<AudioSetVolume *>(msg);
            return handleSetVolume(msgl->playbackType, msgl->updateType, msgl->value);
        });

        connect(typeid(AudioGetVolume), [this](sys::Message *msg) -> sys::MessagePointer {
            auto *msgl = static_cast<AudioGetVolume *>(msg);
            return handleGetVolume(msgl->playbackType);
        });

        connect(typeid(AudioPauseRequest),
                [this]([[maybe_unused]] sys::Message *msg) -> sys::MessagePointer { return handlePause(); });

        connect(typeid(AudioResumeRequest),
                [this]([[maybe_unused]] sys::Message *msg) -> sys::MessagePointer { return handleResume(); });
    }

    Audio::~Audio()
    {}

    sys::MessagePointer Audio::DataReceivedHandler([[maybe_unused]] sys::DataMessage *msgl,
                                                   [[maybe_unused]] sys::ResponseMessage *resp)
    {
        return sys::msgNotHandled();
    }

    sys::ReturnCodes Audio::InitHandler()
    {
        settingsProvider->init(service::ServiceProxy(weak_from_this()));
        initializeDatabase();
        LOG_INFO("Initialized");
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes Audio::DeinitHandler()
    {
        settingsProvider->deinit();
        LOG_INFO("Deinitialized");
        return sys::ReturnCodes::Success;
    }

    void Audio::ProcessCloseReason([[maybe_unused]] sys::CloseReason closeReason)
    {
        if (const auto &activeInputOpt = audioMux.GetActiveInput(); activeInputOpt.has_value()) {
            const auto activeInput = activeInputOpt.value();
            activeInput->audio->Stop();
        }
    }

    auto Audio::handleStart(audio::Operation::Type opType,
                            audio::FadeIn fadeIn,
                            const std::string &fileName,
                            const audio::PlaybackType &playbackType) -> std::unique_ptr<AudioResponseMessage>
    {
        auto retCode  = audio::RetCode::Failed;
        auto retToken = audio::Token::MakeBadToken();

        auto AudioStart = [&](auto &input) {
            if (input) {
                for (auto &audioInput : audioMux.GetAllInputs()) {
                    stopInput(&audioInput);
                }
                retToken = audioMux.ResetInput(input);

                try {
                    retCode = (*input)->audio->Start(opType, retToken, fileName, playbackType);
                }
                catch (const audio::AudioInitException &audioException) {
                    retCode = audio::RetCode::FailedToAllocateMemory;
                }
            }
        };
        auto input = audioMux.GetPlaybackInput(playbackType);
        AudioStart(input);
        manageCpuSentinel();
        if (fadeIn == audio::FadeIn::Enable) {
            volumeFadeIn->Start(getVolume(playbackType), minVolumeToSet, maxVolumeToSet);
        }

        return std::make_unique<AudioStartPlaybackResponse>(retCode, retToken);
    }

    auto Audio::handleStop(const std::vector<audio::PlaybackType> &stopTypes, const audio::Token &token)
        -> std::unique_ptr<AudioResponseMessage>
    {
        std::vector<std::pair<audio::Token, audio::RetCode>> retCodes;

        // if stopType is not provided then stop all inputs, otherwise stop specific ones from stopType
        for (auto &input : audioMux.GetAllInputs()) {
            const auto &currentOperation = input.audio->GetCurrentOperation();
            const auto isOperationInStopTypes =
                std::find(stopTypes.begin(), stopTypes.end(), currentOperation.GetPlaybackType()) != stopTypes.end();
            if (stopTypes.empty() || isOperationInStopTypes) {
                auto t = input.token;
                if (token.IsValid() && t == token) {
                    retCodes.emplace_back(t, stopInput(&input));
                }
                if (token.IsUninitialized()) {
                    retCodes.emplace_back(t, stopInput(&input));
                }
            }
        }

        // on failure return first false code
        auto it = std::find_if_not(
            retCodes.begin(), retCodes.end(), [](auto p) { return p.second == audio::RetCode::Success; });
        if (it != retCodes.end()) {
            return std::make_unique<AudioStopResponse>(it->second, it->first);
        }

        return std::make_unique<AudioStopResponse>(audio::RetCode::Success, token);
    }

    auto Audio::stopInput(audio::AudioMux::Input *input, Audio::StopReason stopReason) -> audio::RetCode
    {
        if (input->audio->GetCurrentState() == audio::Audio::State::Idle) {
            return audio::RetCode::Success;
        }
        const auto retCode = input->audio->Stop();

        // Send notification that audio file was stopped
        std::shared_ptr<AudioNotificationMessage> msg;
        switch (stopReason) {
        case StopReason::Eof:
            msg = std::make_shared<AudioEOFNotification>(input->token);
            break;
        case StopReason::FileDeleted:
            msg = std::make_shared<AudioFileDeletedNotification>(input->token);
            break;
        default:
            msg = std::make_shared<AudioStopNotification>(input->token);
            break;
        }
        bus.sendMulticast(std::move(msg), sys::BusChannel::ServiceAudioNotifications);
        audioMux.ResetInput(input);
        manageCpuSentinel();
        return retCode;
    }

    constexpr auto Audio::shouldLoop(const std::optional<audio::PlaybackType> &type) const -> bool
    {
        return type == audio::PlaybackType::Alarm;
    }

    auto Audio::isBusy() const -> bool
    {
        const auto &inputs = audioMux.GetAllInputs();
        return std::any_of(inputs.begin(), inputs.end(), [](const auto &input) {
            return input.audio->GetCurrentState() != audio::Audio::State::Idle;
        });
    }

    void Audio::handleEOF(const audio::Token &token)
    {
        if (const auto input = audioMux.GetInput(token); input) {
            if (shouldLoop((*input)->audio->GetCurrentOperationPlaybackType())) {
                (*input)->audio->Start();
                if (volumeFadeIn->IsActive()) {
                    volumeFadeIn->Restart();
                }

                if ((*input)->audio->IsMuted()) {
                    (*input)->audio->Mute();
                }
            }
            else {
                stopInput(*input, StopReason::Eof);
            }
        }
    }

    void Audio::handleFileDeleted(const audio::Token &token)
    {
        if (const auto input = audioMux.GetInput(token); input) {
            stopInput(*input, StopReason::FileDeleted);
        }
    }

    auto Audio::AudioServicesCallback(const sys::Message *msg) -> std::optional<std::string>
    {
        std::optional<std::string> ret;
        if (const auto eofMsg = dynamic_cast<const AudioServiceMessage::EndOfFile *>(msg); eofMsg) {
            bus.sendUnicast(std::make_shared<internal::AudioEOFNotificationMessage>(eofMsg->GetToken()),
                            audioServiceName);
        }
        else if (const auto fileDeletedMsg = dynamic_cast<const AudioServiceMessage::FileDeleted *>(msg);
                 fileDeletedMsg) {
            bus.sendUnicast(std::make_shared<internal::AudioFileDeletedNotificationMessage>(fileDeletedMsg->GetToken()),
                            audioServiceName);
        }
        else if (const auto dbRequestMsg = dynamic_cast<const AudioServiceMessage::DbRequest *>(msg); dbRequestMsg) {
            auto selectedPlayback = dbRequestMsg->playback;
            auto selectedProfile  = dbRequestMsg->profile;
            if (const auto result =
                    settingsProvider->getValue(dbPath(dbRequestMsg->setting, selectedPlayback, selectedProfile));
                not result.empty()) {
                ret.emplace(result);
            }
        }
        else {
            LOG_DEBUG("Message received but not handled - no effect");
        }

        return ret;
    }

    auto Audio::handleSetVolume(const audio::PlaybackType &playbackType,
                                const audio::VolumeUpdateType &updateType,
                                const std::string &value) -> std::unique_ptr<AudioResponseMessage>
    {
        const auto clampedValue = std::clamp(utils::getNumericValue<float>(value), minVolumeToSet, maxVolumeToSet);
        auto retCode            = audio::RetCode::Success;

        if (const auto activeInput = audioMux.GetActiveInput(); activeInput.has_value()) {
            if (activeInput.value() != nullptr) {
                retCode = activeInput.value()->audio->SetOutputVolume(clampedValue);
            }
        }

        if ((updateType == audio::VolumeUpdateType::UpdateDB) && (retCode == audio::RetCode::Success)) {
            constexpr auto setting = audio::Setting::Volume;
            settingsProvider->setValue(dbPath(setting, playbackType, profileType), std::to_string(clampedValue));
        }
        return std::make_unique<AudioResponseMessage>(retCode);
    }

    auto Audio::handleGetVolume(const audio::PlaybackType &playbackType) -> std::unique_ptr<AudioResponseMessage>
    {
        const auto path = dbPath(volumeSetting, playbackType, profileType);
        if (const auto value = settingsProvider->getValue(path); not value.empty()) {
            return std::make_unique<AudioResponseMessage>(audio::RetCode::Success, value);
        }

        return std::make_unique<AudioResponseMessage>(audio::RetCode::Failed);
    }

    auto Audio::getVolume(const audio::PlaybackType &playbackType) -> audio::Volume
    {
        const auto path  = dbPath(volumeSetting, playbackType, profileType);
        const auto value = settingsProvider->getValue(path);
        return value.empty() ? utils::getNumericValue<audio::Volume>(defaultVolume)
                             : utils::getNumericValue<audio::Volume>(value);
    }

    sys::ReturnCodes Audio::SwitchPowerModeHandler([[maybe_unused]] const sys::ServicePowerMode mode)
    {
        return sys::ReturnCodes::Success;
    }

    auto Audio::handlePause() -> std::unique_ptr<AudioResponseMessage>
    {
        auto retCode = audio::RetCode::InvokedInIncorrectState;
        if (const auto activeInput = audioMux.GetActiveInput(); activeInput) {
            auto playbackType = (*activeInput)->audio->GetCurrentOperationPlaybackType();
            if (isResumable(playbackType)) {
                retCode = activeInput.value()->audio->Pause();
            }
            else {
                retCode = audio::RetCode::UnsupportedEvent;
            }
        }
        manageCpuSentinel();
        return std::make_unique<AudioResponseMessage>(retCode);
    }

    auto Audio::handleResume() -> std::unique_ptr<AudioResponseMessage>
    {
        auto retCode = audio::RetCode::InvokedInIncorrectState;
        if (const auto activeInput = audioMux.GetActiveInput();
            activeInput && activeInput.value()->audio->GetCurrentOperationState() == audio::Operation::State::Paused) {
            retCode = activeInput.value()->audio->Resume();
        }
        manageCpuSentinel();
        return std::make_unique<AudioResponseMessage>(retCode);
    }

    constexpr auto Audio::isResumable(audio::PlaybackType type) const -> bool
    {
        return type == audio::PlaybackType::Multimedia;
    }

    void Audio::manageCpuSentinel()
    {
        isBusy() ? cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_6)
                 : cpuSentinel->ReleaseMinimumFrequency();
    }

    void Audio::initializeDatabase()
    {
        for (const auto &entry : initializer::values) {
            const auto path = dbPath(entry.first);
            if (settingsProvider->getValue(path).empty()) {
                settingsProvider->setValue(path, entry.second);
            }
        }
    }
} // namespace service
