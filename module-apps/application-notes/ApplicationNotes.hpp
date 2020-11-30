// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/application-notes/model/NotesListModel.hpp"
#include "Application.hpp"

namespace gui::name::window
{
    inline constexpr auto note_preview        = "NotePreview";
    inline constexpr auto note_edit           = "NoteEdit";
    inline constexpr auto note_confirm_dialog = "ConfirmDialog";
} // namespace gui::name::window

namespace app
{
    inline constexpr auto name_notes = "ApplicationNotes";

    class ApplicationNotes : public Application
    {
      public:
        explicit ApplicationNotes(std::string name                    = name_notes,
                                  std::string parent                  = {},
                                  StartInBackground startInBackground = {false});

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

        void createUserInterface() override;
        void destroyUserInterface() override;
    };

    template <> struct ManifestTraits<ApplicationNotes>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
