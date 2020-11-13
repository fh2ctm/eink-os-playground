// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Icons.hpp"

#include "application-call/data/CallAppStyle.hpp"

namespace gui
{
    namespace
    {
        constexpr auto crossImg      = "cross";
        constexpr auto addContactStr = "app_call_contact";
        constexpr auto messageImg    = "mail";
        constexpr auto sendSmstStr   = "app_call_message";
        constexpr auto muteImg       = "microphone_on";
        constexpr auto mutedImg      = "microphone_off";
        constexpr auto muteStr       = "app_call_mute";
        constexpr auto mutedStr      = "app_call_muted";
        constexpr auto speakerImg    = "speaker_off";
        constexpr auto speakerOnImg  = "speaker_on";
        constexpr auto speakerStr    = "app_call_speaker";
        constexpr auto speakerOnStr  = "app_call_speaker_on";

        const Icon<AddContactIconState>::IconMap contactIconMap = {
            {AddContactIconState::ADD_CONTACT, {crossImg, addContactStr}}};
        const Icon<SendSmsIconState>::IconMap smsIconMap = {{SendSmsIconState::SEND_SMS, {messageImg, sendSmstStr}}};
        const Icon<MicrophoneIconState>::IconMap microphoneIconMap = {
            {MicrophoneIconState::MUTE, {muteImg, muteStr}}, {MicrophoneIconState::MUTED, {mutedImg, mutedStr}}};
        const Icon<SpeakerIconState>::IconMap speakerIconMap = {
            {SpeakerIconState::SPEAKER, {speakerImg, speakerStr}},
            {SpeakerIconState::SPEAKERON, {speakerOnImg, speakerOnStr}}};
    } // namespace

    AddContactIcon::AddContactIcon(Item *parent, std::uint32_t x, std::uint32_t y)
        : Icon(parent, x, y, callAppStyle::icon::x_margin, AddContactIconState::ADD_CONTACT, contactIconMap)
    {}

    SendSmsIcon::SendSmsIcon(Item *parent, std::uint32_t x, std::uint32_t y)
        : Icon(parent, x, y, callAppStyle::icon::x_margin, SendSmsIconState::SEND_SMS, smsIconMap)
    {}

    MicrophoneIcon::MicrophoneIcon(Item *parent, std::uint32_t x, std::uint32_t y)
        : Icon(parent, x, y, callAppStyle::icon::x_margin, MicrophoneIconState::MUTE, microphoneIconMap)
    {}

    SpeakerIcon::SpeakerIcon(Item *parent, std::uint32_t x, std::uint32_t y)
        : Icon(parent, x, y, callAppStyle::icon::x_margin, SpeakerIconState::SPEAKER, speakerIconMap)
    {}

} // namespace gui
