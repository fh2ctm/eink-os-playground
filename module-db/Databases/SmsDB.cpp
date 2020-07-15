
/*
 * @file SmsDB.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 28.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <module-vfs/vfs.hpp>
#include "SmsDB.hpp"

const char *SmsDB::dbName = USER_PATH("sms.db");

SmsDB::SmsDB() : Database(dbName), sms(this), threads(this), templates(this)
{
    if (sms.create() == false)
        return;
    if (threads.create() == false)
        return;
    if (templates.create() == false)
        return;

    isInitialized = true;
}

SmsDB::~SmsDB()
{}