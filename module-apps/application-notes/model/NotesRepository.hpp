// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>

#include <module-apps/Application.hpp>

#include <module-db/Interface/NotesRecord.hpp>

namespace app::notes
{
    class AbstractNotesRepository
    {
      public:
        using OnGetCallback    = std::function<bool(const std::vector<NotesRecord> &, unsigned int)>;
        using OnResultCallback = std::function<void(bool)>;

        virtual ~AbstractNotesRepository() noexcept = default;

        virtual void get(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback) = 0;
        virtual void save(const NotesRecord &note, const OnResultCallback &callback)               = 0;
        virtual void remove(const NotesRecord &note, const OnResultCallback &callback)             = 0;
    };

    class NotesDBRepository : public AbstractNotesRepository
    {
      public:
        explicit NotesDBRepository(Application *application);

        void get(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback) override;
        void save(const NotesRecord &note, const OnResultCallback &callback) override;
        void remove(const NotesRecord &note, const OnResultCallback &callback) override;

      private:
        Application *application;
    };
} // namespace app::notes
