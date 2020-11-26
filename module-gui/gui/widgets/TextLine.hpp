// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <list>
#include "Common.hpp"
#include "Label.hpp"
#include "TextDocument.hpp"
#include "BoxLayout.hpp"
#include "TextLineCursor.hpp"

namespace gui
{
    enum class UnderlineDrawMode
    {
        WholeLine,
        Concurrent
    };

    /// interface element for TextDocument->getLine() <-- Text
    class TextLine
    {
        unsigned int shownLetterCount = 0;
        Length widthUsed              = 0;
        Length heightUsed             = 0;
        Length maxWidth               = 0;
        std::list<Label *> lineContent;
        Rect *underline                     = nullptr;
        bool drawUnderline                  = false;
        UnderlineDrawMode drawUnderlineMode = UnderlineDrawMode::Concurrent;
        Position underlinePadding           = 0;
        TextBlock::End end                  = TextBlock::End::None;
        Position storedYOffset              = 0;
        bool lineEnd                        = false;

        void createUnderline(unsigned int max_w, unsigned int max_height);
        void updateUnderline(const short &x, const short &y);

      public:
        /// creates TextLine with data from text based on TextCursor position filling max_width
        TextLine(BlockCursor &, unsigned int max_width);
        TextLine(TextLine &) = delete;
        TextLine(TextLine &&) noexcept;

        TextLine(BlockCursor &cursor,
                 unsigned int max_width,
                 unsigned int init_height,
                 bool drawUnderline,
                 UnderlineDrawMode drawUnderlineMode,
                 Position underlinePadding)
            : TextLine(cursor, max_width)
        {
            this->drawUnderline     = drawUnderline;
            this->drawUnderlineMode = drawUnderlineMode;
            this->underlinePadding  = underlinePadding;

            createUnderline(max_width, init_height);
        }

        ~TextLine();

        /// number of letters in Whole TextLines
        [[nodiscard]] unsigned int length() const
        {
            return shownLetterCount;
        }

        /// count of elements in whole TextLine
        [[nodiscard]] unsigned int count() const
        {
            return lineContent.size();
        }

        [[nodiscard]] Length width() const
        {
            return widthUsed;
        }

        [[nodiscard]] Length height() const
        {
            return heightUsed;
        }

        [[nodiscard]] TextBlock::End getEnd() const
        {
            return end;
        }

        [[nodiscard]] bool getLineEnd() const
        {
            return lineEnd;
        }

        [[nodiscard]] const Item *getElement(unsigned int pos) const
        {
            unsigned int local_pos = 0;
            for (auto &el : lineContent) {
                local_pos += el->getTextLength();
                if (local_pos >= pos) {
                    return el;
                }
            }
            return nullptr;
        }

        [[nodiscard]] int32_t getX() const
        {
            return lineContent.front()->area().pos(Axis::X);
        }

        void setPosition(const short &x, const short &y);
        void setParent(Item *parent);
        [[nodiscard]] Length getWidth() const;
        [[nodiscard]] Length getWidthTo(unsigned int pos) const;
        void erase();
        /// align TextLine due to alignment axis in parent_width
        ///
        /// moves Text parts in Text. To not call n times callbacks on resize, call prior to setting parent
        void alignH(Alignment align, Length parent_length) const;
        void alignV(Alignment align, Length parent_length, Length lines_height);
        [[nodiscard]] auto getText(unsigned int pos) const -> UTF8;
    };
} // namespace gui
