// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <Interface/NotesRecord.hpp>
#include <queries/notes/QueryNotesGet.hpp>
#include <queries/notes/QueryNotesGetByText.hpp>
#include <queries/notes/QueryNoteRemove.hpp>
#include <queries/notes/QueryNoteStore.hpp>

#include "Database/Database.hpp"
#include "Databases/NotesDB.hpp"

TEST_CASE("Notes Record tests")
{
    Database::initialize();

    auto notesDb = std::make_unique<NotesDB>();
    REQUIRE(notesDb->isInitialized());

    NotesRecordInterface notesRecordInterface{notesDb.get()};
    notesRecordInterface.RemoveAll(); // Empty the notes database.

    constexpr auto testSnippet = "TEST SNIPPET";

    NotesRecord recordIn;
    recordIn.snippet = testSnippet;

    notesRecordInterface.Add(recordIn);
    REQUIRE(notesRecordInterface.GetCount() == 1);

    SECTION("Get notes query")
    {
        auto query     = std::make_unique<db::query::QueryNotesGet>(0, notesRecordInterface.GetCount());
        auto response  = notesRecordInterface.runQuery(std::move(query));
        auto getResult = static_cast<db::query::NotesGetResult *>(response.get());

        REQUIRE(getResult->getRecords().size() == notesRecordInterface.GetCount());
        REQUIRE(getResult->getCount() == notesRecordInterface.GetCount());
        REQUIRE(getResult->getRecords()[0].snippet == testSnippet);
    }

    SECTION("Get notes by text query")
    {
        constexpr auto testSearch = "TEST";
        auto query                = std::make_unique<db::query::QueryNotesGetByText>(testSearch);
        auto response             = notesRecordInterface.runQuery(std::move(query));
        auto getResult            = static_cast<db::query::NotesGetByTextResult *>(response.get());

        REQUIRE(getResult->getRecords().size() == 1);
        REQUIRE(getResult->getRecords()[0].snippet == testSnippet);
    }

    SECTION("Add a note")
    {
        NotesRecord record;
        record.snippet = testSnippet;

        auto query     = std::make_unique<db::query::QueryNoteStore>(record);
        auto response  = notesRecordInterface.runQuery(std::move(query));
        auto addResult = static_cast<db::query::NoteStoreResult *>(response.get());

        REQUIRE(addResult->succeed());
        REQUIRE(notesRecordInterface.GetCount() == 2);
    }

    SECTION("Remove a note")
    {
        auto query        = std::make_unique<db::query::QueryNoteRemove>(1);
        auto response     = notesRecordInterface.runQuery(std::move(query));
        auto removeResult = static_cast<db::query::NoteRemoveResult *>(response.get());

        REQUIRE(removeResult->succeed());
        REQUIRE(notesRecordInterface.GetCount() == 0);
    }

    Database::deinitialize();
};