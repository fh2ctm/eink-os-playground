
/*
 * @file ContactRecord.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 29.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_CONTACTRECORD_HPP
#define PUREPHONE_CONTACTRECORD_HPP

#include "Record.hpp"
#include "utf8/UTF8.hpp"
#include "../Common/Common.hpp"

struct ContactRecord{
    uint32_t dbID;
    UTF8 primaryName;
    UTF8 alternativeName;
    UTF8 numberUser;
    UTF8 numberE164;
    ContactType contactType;

    ContactNumberType numberType;

    UTF8 country;
    UTF8 city;
    UTF8 street;
    UTF8 number;
    UTF8 note;
    UTF8 mail;
    ContactAddressType addressType;

    UTF8 assetPath;

    bool isOnWhitelist;
    bool isOnBlacklist;
    bool isOnFavourites;
    uint8_t speeddial;
};


class ContactRecordInterface : public RecordInterface<ContactRecord>{

public:

    bool Add(const ContactRecord& rec) override final;
    bool RemoveByID(uint32_t id) override final;
    bool RemoveByName(const char* str) override final;
    bool Update(const ContactRecord& rec) override final;
    ContactRecord GetByID(uint32_t id) override final;
    ContactRecord GetByName(const char* str) override final;

    uint32_t GetCount() override final;

    std::unique_ptr<std::vector<ContactRecord>> GetLimitOffset(uint32_t offset,uint32_t limit) override final;

};


#endif //PUREPHONE_CONTACTRECORD_HPP
