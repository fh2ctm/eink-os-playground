// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"

#include <module-apps/Application.hpp>
#include <module-apps/application-notes/presenter/SearchEngineWindowPresenter.hpp>

#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/Text.hpp>

namespace app::notes
{
    class SearchEngineWindow : public gui::AppWindow, public SearchEngineWindowContract::View
    {
      public:
        SearchEngineWindow(Application *application,
                           std::unique_ptr<SearchEngineWindowContract::Presenter> &&windowPresenter);
        ~SearchEngineWindow() noexcept override;

        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

        void notesFound(const std::vector<NotesRecord> &notes, const std::string &searchText) override;

      private:
        std::unique_ptr<SearchEngineWindowContract::Presenter> presenter;
        gui::Text *input = nullptr;
    };
} // namespace app::notes
