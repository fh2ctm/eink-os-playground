// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include <application-settings-new/widgets/CategoryWidget.hpp>
#include <application-settings-new/models/CategoriesModel.hpp>
#include <module-gui/gui/widgets/ListView.hpp>
#include <module-apps/InternalModel.hpp>

namespace gui
{
    class QuoteCategoriesWindow : public AppWindow
    {
      public:
        QuoteCategoriesWindow(app::Application *app, std::shared_ptr<app::CategoriesModel> model);

      private:
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        std::shared_ptr<app::CategoriesModel> categoriesModel = nullptr;
        gui::ListView *list                                   = nullptr;
    };

} // namespace gui
