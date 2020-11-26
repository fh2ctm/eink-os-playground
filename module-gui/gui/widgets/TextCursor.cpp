// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextCursor.hpp"
#include "Common.hpp"
#include "Text.hpp"
#include "TextBlockCursor.hpp"
#include "TextDocument.hpp"
#include "log/log.hpp"
#include "TextLine.hpp"
#include <cassert>
#include <RawFont.hpp>

#define debug_text_cursor(...)
// #define debug_text_cursor(...) LOG_DEBUG(__VA_ARGS__)

namespace gui
{
    const unsigned int TextCursor::default_width = 2;

    TextCursor::TextCursor(gui::Text *parent, unsigned int pos, unsigned int block)
        : Rect(parent, 0, 0, default_width, 1),
          BlockCursor(parent != nullptr ? parent->document.get() : nullptr,
                      pos,
                      block,
                      parent != nullptr ? parent->getTextFormat().getFont() : nullptr),
          text(parent)
    {
        setFilled(true);
        setVisible(false);

        pos_on_screen = document->getText().length();
    }

    TextCursor::Move TextCursor::moveCursor(NavigationDirection direction, unsigned int n)
    {
        auto ret = TextCursor::Move::Start;

        for (unsigned int i = 0; i < n; i++) {
            ret = moveCursor(direction);

            if (ret == Move::Start || ret == Move::End || ret == Move::Error) {
                break;
            }
        }
        return ret;
    }

    TextCursor::Move TextCursor::moveCursor(NavigationDirection direction)
    {
        debug_text_cursor("Before move cursor: screen pos: %d block: %d pos: %d %s",
                          pos_on_screen,
                          getBlockNr(),
                          BlockCursor::getPosition(),
                          atBegin() ? "at begin" : "middle");

        if (!checkDocument()) {
            return Move::Error;
        }

        /// left - corner case
        if ((checkNpos() || atBegin()) && (direction == NavigationDirection::LEFT)) {
            return Move::Start;
        }

        /// right - corner case
        if ((checkNpos() || atEnd()) && (direction == NavigationDirection::RIGHT)) {
            return Move::End;
        }

        auto nr = getBlockNr();
        if (direction == NavigationDirection::LEFT) {
            operator--();

            if (pos_on_screen > 0) {
                --pos_on_screen;
            }

            if (nr != getBlockNr() && checkCurrentBlockNoNewLine()) {
                operator--();
            }

            return Move::Left;
        }

        if (direction == NavigationDirection::RIGHT) {
            operator++();

            if (pos_on_screen < document->getText().length()) {
                ++pos_on_screen;
            }

            if (nr != getBlockNr() && checkPreviousBlockNoNewLine()) {
                operator++();
            }

            return Move::Right;
        }

        debug_text_cursor("After move cursor: screen pos: %d block: %d pos: %d %s",
                          pos_on_screen,
                          getBlockNr(),
                          BlockCursor::getPosition(),
                          atBegin() ? "at begin" : "middle");

        return Move::Error;
    }

    std::tuple<const TextLine *, unsigned int, unsigned int> TextCursor::getSelectedLine()
    {
        unsigned int offset_pos = 0;
        unsigned int row        = 0;
        if (text == nullptr) {
            return {nullptr, text::npos, text::npos};
        }
        for (auto &line : text->lines->get()) {

            auto lineSize = line.length() - (line.getEnd() == TextBlock::End::Newline ? 1 : 0);

            if (offset_pos + lineSize >= pos_on_screen) {
                auto column = pos_on_screen - offset_pos;
                return {&line, column, row};
            }
            offset_pos += line.length();
            ++row;
        }
        return {nullptr, text::npos, text::npos};
    }

    void TextCursor::updateView()
    {
        int32_t x = 0, y = 0;
        uint32_t w = default_width, h = 0;
        if (text == nullptr) {
            setArea({x, y, w, h});
            return;
        }
        auto default_font = text->format.getFont();
        if (document->isEmpty() && default_font != nullptr) {
            h += default_font->info.line_height;
            x = getAxisAlignmentValue(Axis::X, w);
            y = getAxisAlignmentValue(Axis::Y, h);
        }
        else if (text != nullptr || text->lines->size() > 0) {
            auto [line, column, row] = getSelectedLine();
            if (line == nullptr || column == text::npos || row == text::npos) {
                setArea({x, y, w, h});
                return;
            }
            auto el = line->getElement(column);
            assert(el != nullptr);
            x = line->getX() + line->getWidthTo(column);
            y = el->getY();
            h = el->getHeight();
        }
        setArea({x, y, w, h});
    }

    void TextCursor::addChar(uint32_t utf_val)
    {
        BlockCursor::addChar(utf_val);
        moveCursor(NavigationDirection::RIGHT);
    }

    TextCursor &TextCursor::operator<<(const UTF8 &textString)
    {
        for (unsigned int i = 0; i < textString.length(); ++i) {
            if (text->checkAdditionBounds(textString[i]) == InputBound::CAN_ADD) {
                addChar(textString[i]);
            }
            else {
                break;
            }
        }
        return *this;
    }

    TextCursor &TextCursor::operator<<(const TextBlock &textBlock)
    {
        auto [addBoundResult, processedTextBlock] = text->checkAdditionBounds(textBlock);

        if (addBoundResult == InputBound::CAN_ADD || addBoundResult == InputBound::CAN_ADD_PART) {

            auto len = processedTextBlock.length();
            auto end = processedTextBlock.getEnd();

            BlockCursor::addTextBlock(std::move(processedTextBlock));

            // +1 is for block barrier
            moveCursor(NavigationDirection::RIGHT, len + 1);

            // If new added block ends with newline split it for additional empty block at end
            if (end == TextBlock::End::Newline) {
                document->addNewline(*this, TextBlock::End::Newline);
                moveCursor(NavigationDirection::RIGHT);
            }
        }

        return *this;
    }

    void TextCursor::removeChar()
    {
        moveCursor(NavigationDirection::LEFT);
        BlockCursor::removeChar();
    }

} // namespace gui

const char *c_str(enum gui::TextCursor::Move what)
{
    switch (what) {
    case gui::TextCursor::Move::Start:
        return "Start";
    case gui::TextCursor::Move::End:
        return "End";
    case gui::TextCursor::Move::Up:
        return "Up";
    case gui::TextCursor::Move::Down:
        return "Down";
    case gui::TextCursor::Move::Left:
        return "Left";
    case gui::TextCursor::Move::Right:
        return "Right";
    case gui::TextCursor::Move::Error:
        return "Error";
    }
    return "";
}
