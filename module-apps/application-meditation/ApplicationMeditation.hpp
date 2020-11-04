// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <Application.hpp>
#include "data/OptionsData.hpp"

namespace app
{
    inline const std::string name_meditation = "ApplicationMeditation";

    class ApplicationMeditation : public Application
    {
      public:
        explicit ApplicationMeditation(std::string name                    = name_meditation,
                                       std::string parent                  = {},
                                       StartInBackground startInBackground = {false});

        auto InitHandler() -> sys::ReturnCodes override;
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::Message_t override;
        auto SwitchPowerModeHandler(sys::ServicePowerMode mode) -> sys::ReturnCodes final;

        void createUserInterface() override;
        void destroyUserInterface() override;
        std::unique_ptr<gui::OptionsData> state;
    };

    template <> struct ManifestTraits<ApplicationMeditation>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
