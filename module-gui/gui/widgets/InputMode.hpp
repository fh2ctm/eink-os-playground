#pragma once

#include <functional>
#include <list>
#include <utf8/UTF8.hpp>

// TODO read from keymap file ...
namespace gui
{
    inline const std::vector<char> special_chars = {'.', ',', '_', ':', ';', ')', '(', '?', '!', '/', '*', '+'};
}

/// this element has one goal - nicely change input parsing which is done in application in it's widgets
class InputMode
{
  public:
    /// ! when adding modes please update next method and input_mode (in cpp file)
    enum Mode
    {
        digit,
        abc,
        ABC,
        phone,
    };

  private:
    InputMode() = delete;
    // list of enabled input modes
    std::list<Mode> input_mode_list                    = {};
    uint32_t input_mode_list_pos                       = 0;
    std::function<void(const UTF8 &text)> show_type_cb = nullptr;
    std::function<void()> restore_after_show_type_cb   = nullptr;
    std::function<void()> show_special_char_selector   = nullptr;
    Mode modeNow();

    void show_input_type();

  public:
    void show_restore();
    InputMode(std::list<InputMode::Mode> mode_list,
              std::function<void(const UTF8 &text)> show_type_cb = nullptr,
              std::function<void()> restore_after_show_type_cb   = nullptr,
              std::function<void()> show_special_char_selector   = nullptr);
    void on_focus(bool focus)
    {
        if (!focus)
            show_restore();
    }
    void next();
    const std::string &get();
    void select_special_char();
};
