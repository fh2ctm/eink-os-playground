// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuoteWidget.hpp"

#include <BottomBar.hpp>
#include <InputEvent.hpp>
#include <utility>
#include <i18n/i18n.hpp>

namespace style::quotes
{
    namespace widget
    {
        inline constexpr uint32_t w = style::window::default_body_width;
        inline constexpr uint32_t h = 50;

        inline constexpr uint32_t input_box_w           = 55;
        inline constexpr uint32_t input_box_h           = h;
        inline constexpr int32_t input_box_right_margin = 20;

        inline constexpr uint32_t description_label_w           = 280;
        inline constexpr uint32_t description_label_h           = 33;
        inline constexpr int32_t description_label_right_margin = 40;

        inline constexpr int32_t tick_image_left_margin  = -64;
        inline constexpr int32_t tick_image_right_margin = 32;

    } // namespace widget

} // namespace style::quotes

namespace gui
{

    QuoteWidget::QuoteWidget(const app::QuoteRecord &quote,
                             std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                             std::function<void()> bottomBarRestoreFromTemporaryMode)
        : bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode)), quote(quote)
    {

        setMinimumSize(style::quotes::widget::w, style::quotes::widget::h);

        setMargins(gui::Margins(0, style::margins::big, 0, 0));

        hBox = new gui::HBox(this, 0, 0, this->getWidth(), 80);
        hBox->setEdges(gui::RectangleEdge::None);
        hBox->setPenFocusWidth(style::window::default_border_focus_w);
        hBox->setPenWidth(style::window::default_border_rect_no_focus);

        inputBoxLabel = new gui::Label(hBox, 0, 0, 0, 0);
        inputBoxLabel->setMinimumSize(style::quotes::widget::input_box_w, style::quotes::widget::input_box_h);

        inputBoxLabel->setMargins(gui::Margins(0, 0, style::quotes::widget::input_box_right_margin, 0));
        inputBoxLabel->setEdges(gui::RectangleEdge::Bottom);
        inputBoxLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        inputBoxLabel->setFont(style::window::font::medium);
        inputBoxLabel->activeItem = false;

        tickImage = new gui::Image(hBox, 0, 0, 0, 0);
        tickImage->setAlignment(Alignment(gui::Alignment::Vertical::Center));
        // Not ideal -> best solution would be to create separate widget with image inside box.
        tickImage->setMargins(gui::Margins(
            style::quotes::widget::tick_image_left_margin, 0, style::quotes::widget::tick_image_right_margin, 0));
        tickImage->set("small_tick_W_M");
        tickImage->setVisible(true);
        tickImage->activeItem = false;

        descriptionLabel = new gui::Label(hBox, 0, 0, 0, 0);
        descriptionLabel->setMinimumSize(style::quotes::widget::description_label_w,
                                         style::quotes::widget::description_label_h);
        descriptionLabel->setMargins(gui::Margins(0, 0, style::quotes::widget::description_label_right_margin, 0));
        descriptionLabel->setEdges(gui::RectangleEdge::None);
        descriptionLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        descriptionLabel->setFont(style::window::font::medium);
        descriptionLabel->activeItem = false;

        descriptionLabel->setText(quote.quote);

        focusChangedCallback = [&](gui::Item &item) {
            if (item.focus) {
                descriptionLabel->setFont(style::footer::font::bold);
                setFocusItem(inputBoxLabel);
                auto bottorBarText =
                    tickImage->visible ? utils::localize.get("common_uncheck") : utils::localize.get("common_check");
                this->bottomBarTemporaryMode(bottorBarText);
            }
            else {
                descriptionLabel->setFont(style::footer::font::medium);
                setFocusItem(nullptr);
                this->bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };

        activatedCallback = [&](gui::Item &item) {
            tickImage->setVisible(!tickImage->visible);
            auto bottorBarText =
                tickImage->visible ? utils::localize.get("common_uncheck") : utils::localize.get("common_check");
            this->bottomBarTemporaryMode(bottorBarText);
            hBox->resizeItems();
            return true;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            hBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        setEdges(gui::RectangleEdge::None);
    }
} /* namespace gui */
