/*
 * @file NotesTable.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Table.hpp"
#include "Record.hpp"
#include "Database/Database.hpp"
#include "utf8/UTF8.hpp"
#include "Common/Common.hpp"

struct NotesTableRow
{
    uint32_t ID = DB_ID_NONE;
    uint32_t date;
    UTF8 snippet; // up to 60 characters
    UTF8 path;
};

enum class NotesTableFields
{
    Date,
    Snippet,
    Path
};

/*
 *
 */
class NotesTable : public Table<NotesTableRow, NotesTableFields>
{
  public:
    NotesTable(Database *db);
    virtual ~NotesTable();

    bool create() override final;
    bool add(NotesTableRow entry) override final;
    bool removeById(uint32_t id) override final;
    bool removeByField(NotesTableFields field, const char *str) override final;
    bool update(NotesTableRow entry) override final;
    NotesTableRow getById(uint32_t id) override final;
    std::vector<NotesTableRow> getLimitOffset(uint32_t offset, uint32_t limit) override final;
    std::vector<NotesTableRow> getLimitOffsetByField(uint32_t offset,
                                                     uint32_t limit,
                                                     NotesTableFields field,
                                                     const char *str) override final;

    uint32_t count() override final;
    uint32_t countByFieldId(const char *field, uint32_t id) override final;

  private:
    const char *createTableQuery = "CREATE TABLE IF NOT EXISTS notes("
                                   "_id INTEGER PRIMARY KEY,"
                                   "date INTEGER,"
                                   "snippet TEXT DEFAULT '',"
                                   "path TEXT DEFAULT '')";
};
