#include "ContactsAddressTable.hpp"

ContactsAddressTable::ContactsAddressTable(Database *db) : Table(db)
{}

ContactsAddressTable::~ContactsAddressTable()
{}

bool ContactsAddressTable::create()
{
    return db->Execute(createTableQuery);
}

bool ContactsAddressTable::add(ContactsAddressTableRow entry)
{
    return db->Execute("insert or ignore into contact_address (contact_id, address, note, mail) "
                       "VALUES (%lu, '%q', '%q', '%q');",
                       entry.contactID,
                       entry.address.c_str(),
                       entry.note.c_str(),
                       entry.mail.c_str());
}

bool ContactsAddressTable::removeById(uint32_t id)
{
    return db->Execute("DELETE FROM contact_address where _id = %u;", id);
}

bool ContactsAddressTable::update(ContactsAddressTableRow entry)
{
    return db->Execute("UPDATE contact_address SET contact_id = %lu, address = '%q', note = '%q', mail = '%q' "
                       "WHERE _id=%lu;",
                       entry.contactID,
                       entry.address.c_str(),
                       entry.note.c_str(),
                       entry.mail.c_str(),
                       entry.ID);
}

ContactsAddressTableRow ContactsAddressTable::getById(uint32_t id)
{
    auto retQuery = db->Query("SELECT * FROM contact_address WHERE _id= %lu;", id);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return ContactsAddressTableRow();
    }

    return ContactsAddressTableRow{
        (*retQuery)[0].GetUInt32(), // ID
        (*retQuery)[1].GetUInt32(), // contactID
        (*retQuery)[2].GetString(), // address
        (*retQuery)[3].GetString(), // note
        (*retQuery)[4].GetString(), // mail
    };
}

std::vector<ContactsAddressTableRow> ContactsAddressTable::getLimitOffset(uint32_t offset, uint32_t limit)
{
    auto retQuery = db->Query("SELECT * from contact_address ORDER BY contact_id LIMIT %lu OFFSET %lu;", limit, offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ContactsAddressTableRow>();
    }

    std::vector<ContactsAddressTableRow> ret;

    do {
        ret.push_back(ContactsAddressTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // contactID
            (*retQuery)[2].GetString(), // address
            (*retQuery)[3].GetString(), // note
            (*retQuery)[4].GetString(), // mail
        });
    } while (retQuery->NextRow());

    return ret;
}

std::vector<ContactsAddressTableRow> ContactsAddressTable::getLimitOffsetByField(uint32_t offset,
                                                                                 uint32_t limit,
                                                                                 ContactAddressTableFields field,
                                                                                 const char *str)
{

    std::string fieldName;
    switch (field) {
    case ContactAddressTableFields ::Mail:
        fieldName = "mail";
        break;
    default:
        return std::vector<ContactsAddressTableRow>();
    }

    auto retQuery = db->Query("SELECT * from contact_address WHERE %q='%q' ORDER BY contact_id LIMIT %lu OFFSET %lu;",
                              fieldName.c_str(),
                              str,
                              limit,
                              offset);

    if ((retQuery == nullptr) || (retQuery->GetRowCount() == 0)) {
        return std::vector<ContactsAddressTableRow>();
    }

    std::vector<ContactsAddressTableRow> ret;

    do {
        ret.push_back(ContactsAddressTableRow{
            (*retQuery)[0].GetUInt32(), // ID
            (*retQuery)[1].GetUInt32(), // contactID
            (*retQuery)[2].GetString(), // address
            (*retQuery)[3].GetString(), // note
            (*retQuery)[4].GetString(), // mail
        });
    } while (retQuery->NextRow());

    return ret;
}

uint32_t ContactsAddressTable::count()
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM contact_address;");

    if (queryRet->GetRowCount() == 0) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}

uint32_t ContactsAddressTable::countByFieldId(const char *field, uint32_t id)
{
    auto queryRet = db->Query("SELECT COUNT(*) FROM contact_address WHERE %q=%lu;", field, id);

    if ((queryRet == nullptr) || (queryRet->GetRowCount() == 0)) {
        return 0;
    }

    return uint32_t{(*queryRet)[0].GetUInt32()};
}