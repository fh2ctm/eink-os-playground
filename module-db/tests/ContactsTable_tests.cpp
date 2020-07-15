#include <catch2/catch.hpp>

#include "Databases/ContactsDB.hpp"
#include "Tables/ContactsTable.hpp"

#include "vfs.hpp"

TEST_CASE("Contacts Table tests")
{
    Database::initialize();

    vfs.remove(ContactsDB::GetDBName());

    ContactsDB contactsdb;
    REQUIRE(contactsdb.IsInitialized());

    ContactsTableRow testRow1 = {.ID             = 0,
                                 .nameID         = 0,
                                 .numbersID      = "0 1 2 3 4",
                                 .ringID         = 0,
                                 .addressID      = 0,
                                 .isOnWhitelist  = true,
                                 .isOnBlacklist  = true,
                                 .isOnFavourites = false,
                                 .speedDial      = "666"

    };

    // Add 4 elements into table
    REQUIRE(contactsdb.contacts.Add(testRow1));
    REQUIRE(contactsdb.contacts.Add(testRow1));
    REQUIRE(contactsdb.contacts.Add(testRow1));
    REQUIRE(contactsdb.contacts.Add(testRow1));

    // Table should have 4 elements
    REQUIRE(contactsdb.contacts.GetCount() == 4);

    // Update existing element in table
    testRow1.ID        = 4;
    testRow1.speedDial = "777";
    REQUIRE(contactsdb.contacts.Update(testRow1));

    // Get table row using valid ID & check if it was updated
    auto sms = contactsdb.contacts.GetByID(4);
    REQUIRE(sms.speedDial == testRow1.speedDial);

    // Get table row using invalid ID(should return empty contactsdb.contactsRow)
    auto smsFailed = contactsdb.contacts.GetByID(100);
    REQUIRE(smsFailed.speedDial == "");

    // Get table rows using valid offset/limit parameters
    auto retOffsetLimit = contactsdb.contacts.GetLimitOffset(0, 4);
    REQUIRE(retOffsetLimit.size() == 4);

    // Get table rows using valid offset/limit parameters and specific field's ID
    REQUIRE(contactsdb.contacts.GetLimitOffsetByField(0, 4, ContactTableFields::SpeedDial, "666").size() == 3);

    // Get table rows using invalid limit parameters(should return 4 elements instead of 100)
    auto retOffsetLimitBigger = contactsdb.contacts.GetLimitOffset(0, 100);
    REQUIRE(retOffsetLimitBigger.size() == 4);

    // Get table rows using invalid offset/limit parameters(should return empty object)
    auto retOffsetLimitFailed = contactsdb.contacts.GetLimitOffset(5, 4);
    REQUIRE(retOffsetLimitFailed.size() == 0);

    // Get count of elements by field's ID
    REQUIRE(contactsdb.contacts.GetCountByFieldID("ring_id", 0) == 4);

    // Get count of elements by invalid field's ID
    REQUIRE(contactsdb.contacts.GetCountByFieldID("invalid_field", 0) == 0);

    REQUIRE(contactsdb.contacts.RemoveByID(2));

    // Table should have now 3 elements
    REQUIRE(contactsdb.contacts.GetCount() == 3);

    // Remove non existing element
    REQUIRE(contactsdb.contacts.RemoveByID(100));

    // Remove all elements from table
    REQUIRE(contactsdb.contacts.RemoveByID(1));
    REQUIRE(contactsdb.contacts.RemoveByID(3));
    REQUIRE(contactsdb.contacts.RemoveByID(4));

    // Table should be empty now
    REQUIRE(contactsdb.contacts.GetCount() == 0);

    Database::deinitialize();
}
