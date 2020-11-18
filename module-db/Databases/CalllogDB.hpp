// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Database/Database.hpp"
#include "../Tables/CalllogTable.hpp"

class CalllogDB : public Database
{
  public:
    CalllogDB();
    ~CalllogDB() = default;

    CalllogTable calls;

    static const char *GetDBName()
    {
        return dbName;
    }

  private:
    static const char *dbName;
};
