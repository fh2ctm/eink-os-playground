
/*
 * @file Database.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_DATABASE_HPP
#define PUREPHONE_DATABASE_HPP

#include "sqlite3.h"

#include "QueryResult.hpp"
#include <memory>


class Database {
public:

    Database(const char *name);

    virtual ~Database();

    std::unique_ptr<QueryResult> Query(const char *format, ...);

    bool Execute(const char *format, ...);

private:

    const uint32_t maxQueryLen = (8 * 1024);

    /*
     * Arguments:
     *
     *   usrPtr - Pointer to user data
     *    count - The number of columns in the result set
     *     data - The row's data
     *  columns - The column names
     */
    static int queryCallback(void *usrPtr, int count, char **data, char **columns);

protected:
    sqlite3 *dbConnection;

};


#endif //PUREPHONE_DATABASE_HPP
