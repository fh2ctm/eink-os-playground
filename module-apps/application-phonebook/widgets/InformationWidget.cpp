// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "InformationWidget.hpp"

#include "AppWindow.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"

#include <ContactRecord.hpp>
#include <module-utils/i18n/i18n.hpp>

namespace gui
{
    InformationWidget::InformationWidget(app::Application *app)
    {
        setMargins(gui::Margins(0, style::margins::huge, 0, 0));

        setMinimumSize(phonebookStyle::informationWidget::w,
                       phonebookStyle::informationWidget::title_label_h + style::margins::huge);

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);

        titleLabel = new Label(vBox, 0, 0, 0, 0, utils::localize.get("app_phonebook_contact_information"));
        titleLabel->setMinimumSize(phonebookStyle::informationWidget::w,
                                   phonebookStyle::informationWidget::title_label_h);
        titleLabel->setEdges(RectangleEdge::None);
        titleLabel->setMargins(Margins(0, 0, 0, style::margins::very_big));
        titleLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        titleLabel->setFont(style::window::font::verysmall);
        titleLabel->setLineMode(true);
        titleLabel->activeItem = false;

        onLoadCallback = [=](std::shared_ptr<ContactRecord> contact) {
            if (contact->numbers.size() > 0) {

                setMinimumHeight(widgetMinimumArea.h + phonebookStyle::numbersWithIconsWidget::h);

                primaryNumberHBox = new NumberWithIconsWidget(
                    app, contact->numbers[0].number, style::window::font::mediumbold, nullptr);
                vBox->addWidget(primaryNumberHBox);
            }
            if (contact->numbers.size() > 1) {
                setMinimumHeight(widgetMinimumArea.h + phonebookStyle::numbersWithIconsWidget::h);
                secondNumberHBox =
                    new NumberWithIconsWidget(app, contact->numbers[1].number, style::window::font::medium, nullptr);

                vBox->addWidget(secondNumberHBox);

                // Set proper navigation if second number is present
                primaryNumberHBox->smsImage->setNavigationItem(NavigationDirection::DOWN, secondNumberHBox->smsImage);
                primaryNumberHBox->phoneImage->setNavigationItem(NavigationDirection::DOWN,
                                                                 secondNumberHBox->phoneImage);

                secondNumberHBox->smsImage->setNavigationItem(NavigationDirection::UP, primaryNumberHBox->smsImage);
                secondNumberHBox->phoneImage->setNavigationItem(NavigationDirection::UP, primaryNumberHBox->phoneImage);
            }
            if (contact->mail.length() > 0) {
                setMinimumHeight(widgetMinimumArea.h + phonebookStyle::informationWidget::email_text_h +
                                 style::margins::very_big);

                emailText = new Text(vBox, 0, 0, 0, 0);
                emailText->setMaximumSize(phonebookStyle::informationWidget::w,
                                          phonebookStyle::informationWidget::email_text_h);
                emailText->setMargins(Margins(0, style::margins::very_big, 0, 0));
                emailText->setFont(style::window::font::medium);
                emailText->setEditMode(EditMode::BROWSE);
                emailText->setEdges(RectangleEdge::None);
                emailText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
                emailText->setText(contact->mail);
                emailText->activeItem = false;
            }
        };

        focusChangedCallback = [&, app](Item &item) {
            if (focus) {
                if (savedFocusItem == nullptr) {
                    setFocusItem(vBox);
                }
                else {
                    savedFocusItem->parent->focusChangedCallback(*savedFocusItem);
                }
            }
            else {
                savedFocusItem = getFocusItem();
                app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }

            // Clear VBox down navigation if second number is present.
            if (secondNumberHBox != nullptr) {
                primaryNumberHBox->clearNavigationItem(NavigationDirection::DOWN);
            }

            return vBox->onInput(event);
        };

        setEdges(RectangleEdge::None);
    }

    auto InformationWidget::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        vBox->setPosition(0, 0);
        vBox->setSize(newDim.w, newDim.h);
        return true;
    }
} /* namespace gui */
