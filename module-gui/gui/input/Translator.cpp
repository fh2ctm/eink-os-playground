// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Translator.hpp"
#include "log/log.hpp"
#include <algorithm>
#include <log/log.hpp>
#include <filesystem>

namespace gui
{
    namespace
    {
        constexpr auto profilesFolder = "assets/profiles";
        constexpr auto extension      = ".json";
    } // namespace

    void recon_long_press(InputEvent &evt, const RawKey &key, const RawKey &prev_key_press, uint32_t time)
    {
        if (key.state == RawKey::State::Released && prev_key_press.key_code == key.key_code) {
            // determine long press
            if (key.time_release - prev_key_press.time_press >= key_time_longpress_ms) {
                evt.state = InputEvent::State::keyReleasedLong;
            }
        }
    }

    InputEvent KeyBaseTranslation::set(RawKey key)
    {
        gui::InputEvent evt(key);
        if (key.state == RawKey::State::Pressed) {
            evt.state = InputEvent::State::keyPressed;
        }
        else if (key.state == RawKey::State::Released) {
            evt.state = InputEvent::State::keyReleasedShort;
        }
        recon_long_press(evt, key, prev_key_press, key_time_longpress_ms);
        // store last key press/release
        if (key.state == RawKey::State::Pressed) {
            prev_key_press = key;
        }
        if (key.state != RawKey::State::Released) {
            prev_key_released = false;
        }
        else {
            prev_key_released = true;
        }
        return evt;
    }

    bool KeyBaseTranslation::timeout(uint32_t time)
    {
        if (!prev_key_released && (prev_key_press.time_press != 0) &&
            (time - prev_key_press.time_press >= key_time_longpress_ms)) {
            prev_key_timedout = true;
            return true;
        }
        return false;
    }

    gui::KeyCode getKeyCode(bsp::KeyCodes code)
    {
        switch (code) {
        case bsp::KeyCodes::NumericKey1:
            return gui::KeyCode::KEY_1;
            break;
        case bsp::KeyCodes::NumericKey2:
            return gui::KeyCode::KEY_2;
            break;
        case bsp::KeyCodes::NumericKey3:
            return gui::KeyCode::KEY_3;
            break;
        case bsp::KeyCodes::NumericKey4:
            return gui::KeyCode::KEY_4;
            break;
        case bsp::KeyCodes::NumericKey5:
            return gui::KeyCode::KEY_5;
            break;
        case bsp::KeyCodes::NumericKey6:
            return gui::KeyCode::KEY_6;
            break;
        case bsp::KeyCodes::NumericKey7:
            return gui::KeyCode::KEY_7;
            break;
        case bsp::KeyCodes::NumericKey8:
            return gui::KeyCode::KEY_8;
            break;
        case bsp::KeyCodes::NumericKey9:
            return gui::KeyCode::KEY_9;
            break;
        case bsp::KeyCodes::NumericKey0:
            return gui::KeyCode::KEY_0;
            break;
        case bsp::KeyCodes::NumericKeyAst:
            return gui::KeyCode::KEY_AST;
            break;
        case bsp::KeyCodes::NumericKeyPnd:
            return gui::KeyCode::KEY_PND;
            break;
        case bsp::KeyCodes::JoystickLeft:
            return gui::KeyCode::KEY_LEFT;
            break;
        case bsp::KeyCodes::JoystickRight:
            return gui::KeyCode::KEY_RIGHT;
            break;
        case bsp::KeyCodes::JoystickUp:
            return gui::KeyCode::KEY_UP;
            break;
        case bsp::KeyCodes::JoystickDown:
            return gui::KeyCode::KEY_DOWN;
            break;
        case bsp::KeyCodes::JoystickEnter:
            return gui::KeyCode::KEY_ENTER;
            break;
        case bsp::KeyCodes::FnLeft:
            return gui::KeyCode::KEY_LF;
            break;
        case bsp::KeyCodes::FnRight:
            return gui::KeyCode::KEY_RF;
            break;
        case bsp::KeyCodes::VolUp:
            return gui::KeyCode::KEY_VOLUP;
            break;
        case bsp::KeyCodes::VolDown:
            return gui::KeyCode::KEY_VOLDN;
            break;
        case bsp::KeyCodes::Torch:
            return gui::KeyCode::KEY_TORCH;
            break;
        case bsp::KeyCodes::SSwitchUp:
            return gui::KeyCode::SWITCH_UP;
            break;
        case bsp::KeyCodes::SSwitchDown:
            return gui::KeyCode::SWITCH_DN;
            break;
        case bsp::KeyCodes::SSwitchMid:
            return gui::KeyCode::SWITCH_MID;
            break;
        default:
            LOG_ERROR("Unhandled bsp key!");
            return gui::KeyCode::KEY_UNDEFINED;
        }
    }

    InputEvent KeyInputSimpleTranslation::translate(RawKey key)
    {
        auto evt = KeyBaseTranslation::set(key);
        // when last action timed out we don't want to handle key release
        if (prev_key_timedout && key.state == RawKey::State::Released) {
            evt.state         = InputEvent::State::Undefined;
            prev_key_timedout = false;
        }
        evt.keyCode = getKeyCode(key.key_code);
        return evt;
    }

    InputEvent KeyInputSimpleTranslation::translate(uint32_t timeout)
    {
        RawKey key;
        key.state        = RawKey::State::Released;
        key.key_code     = prev_key_press.key_code;
        key.time_press   = 0;
        key.time_release = timeout;
        InputEvent evt(key);
        evt.state   = InputEvent::State::keyReleasedLong;
        evt.keyCode = getKeyCode(key.key_code);
        return evt;
    }

    uint32_t KeyInputMappedTranslation::handle(RawKey key, const std::string &keymap)
    {
        // get shortpress
        if (prev_key_press.key_code != key.key_code) {
            times = 0;
        }
        else if (key.state == RawKey::State::Released) {
            /// TODO use key_time_cycle_ms from keymap (if exists in keymap...)
            if (key.time_release - prev_key_press.time_release < key_time_cycle_ms) {
                ++times;
            }
            else {
                times = 0;
            }
        }
        if (key.state == RawKey::State::Released) {
            prev_key_press = key;
        }
        return profiles.get(keymap).getCharKey(key.key_code, times);
    }

    uint32_t KeyInputMappedTranslation::getTimes() const noexcept
    {
        return times;
    }

    void Profiles::loadProfile(const std::string &filepath)
    {
        LOG_INFO("Load profile: %s", filepath.c_str());
        auto p = Profile(filepath);
        if (auto name = p.getName(); !name.empty()) {
            profilesList.insert({p.getName(), std::move(p)});
        }
    }

    std::vector<std::string> Profiles::getProfilesPaths()
    {
        std::vector<std::string> profileFiles;
        LOG_INFO("Scanning %s profiles folder: %s", extension, profilesFolder);

        for (const auto &entry : std::filesystem::directory_iterator(profilesFolder)) {
            profileFiles.push_back(std::filesystem::path(entry.path()));
        }

        LOG_INFO("Total number of profiles: %u", static_cast<unsigned int>(profileFiles.size()));
        return profileFiles;
    }

    void Profiles::init()
    {
        std::vector<std::string> profileFilesPaths = getProfilesPaths();
        for (std::string filePath : profileFilesPaths) {
            if (std::size(filePath)) {
                loadProfile(filePath);
            }
        }
        if (std::size(profilesList) == 0) {
            LOG_ERROR("No keyboard profiles loaded");
        }
    }

    Profiles &Profiles::get()
    {
        static Profiles *p;
        if (p == nullptr) {
            p = new Profiles();
            p->init();
        }
        return *p;
    }

    Profile &Profiles::get(const std::string &name)
    {
        auto filepath = std::string(profilesFolder) + "/" + name + extension;
        // if profile not in profile map -> load
        if (std::size(filepath) == 0) {
            LOG_ERROR("Request for nonexistent profile: %s", filepath.c_str());
            return get().empty;
        }
        if (get().profilesList.find(filepath) == get().profilesList.end()) {
            get().loadProfile(filepath);
        }
        return get().profilesList[filepath];
    }

} /* namespace gui */
