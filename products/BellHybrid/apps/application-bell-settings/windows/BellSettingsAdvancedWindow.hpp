// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/windows/OptionWindow.hpp>

namespace gui
{

    class BellSettingsAdvancedWindow : public OptionWindow
    {
      public:
        explicit BellSettingsAdvancedWindow(app::Application *app);

      private:
        std::list<Option> settingsOptionsList();
        void buildInterface() override;
    };

} // namespace gui