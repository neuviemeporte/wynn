#include <QVector>
#include "database.h"
#include "quiz.h"
#include "gtest/gtest.h"
#include "ixplog_active.h"
#include "log.hpp"

using namespace wynn::db;

namespace {

std::ostream& operator<<(std::ostream &os, const Entry &e) {
    return os << "{" << e.item().toStdString() << "," << e.description().toStdString()
              << "," << e.points(DIR_SHOWDESC)
              << "," << e.fails(DIR_SHOWDESC)
              << "," << e.points(DIR_SHOWITEM)
              << "," << e.fails(DIR_SHOWITEM) << "}";
}

// stringwise compare two Entry's, the overloaded equality operator
// takes uuids and points into account as well
bool equal(const Entry &e1, const Entry &e2) {
    return e1.item() == e2.item() && e1.description() == e2.description()
            && e1.points(DIR_SHOWDESC) == e2.points(DIR_SHOWDESC)
            && e1.fails(DIR_SHOWDESC) == e2.fails(DIR_SHOWDESC)
            && e1.points(DIR_SHOWITEM) == e2.points(DIR_SHOWITEM)
            && e1.fails(DIR_SHOWITEM) == e2.fails(DIR_SHOWITEM);
}

// check if vector of entries and database have the same contents stringwise
void checkConsistency(const Database &db, const QVector<Entry> &entries) {
    const int entryCount = entries.size();
    for (int i = 0; i < entryCount; ++i) {
        auto &e1 = db.entry(i);
        auto &e2 = entries[i];
        EXPECT_TRUE(equal(e1, e2));
        // check if it is possible to find the current item in the database
        // and that the returned index is as expected
        auto i1 = db.findEntry(e2.item());
        auto i2 = db.findEntry(e2.description());
        EXPECT_EQ(i1, i);
        EXPECT_EQ(i1, i2);
    }
}

void dumpDbase(const Database &db) {
    LOG("Database contents [" << db.entryCount() << "]");
    for (int i = 0; i < db.entryCount(); ++i) {
        LOG(i << ": " << db.entry(i));
    }
}

} // namespace

TEST(DbaseTest, Sanity) {
    const QString dbName{"testdb"}, item{"item"}, desc{"description"};
    LOG("Creating database instance, name = " << dbName);
    Database db(nullptr, dbName);

    EXPECT_EQ(db.name(), dbName);
    EXPECT_EQ(db.entryCount(), 0);
    EXPECT_TRUE(db.dirty());
    EXPECT_FALSE(db.error());
    EXPECT_FALSE(db.locked());
    EXPECT_FALSE(db.external());

    // generate some entries
    int entryCount = 10;
    LOG("Generating entries, count = " << entryCount);
    QVector<Entry> entries;
    for (int i = 0; i < entryCount; ++i) {
        Entry entry;
        entry.setItem(item + QString::number(i));
        entry.setDescription(desc + QString::number(i));
        entries.push_back(entry);
    }

    // add entries to database
    LOG("Adding entries to database");
    for (auto &e : entries) {
        ASSERT_EQ(db.add(e.item(), e.description()), Error::OK);
    }
    // try to add duplicate, should fail with error
    EXPECT_EQ(db.add(entries.front().item(), entries.front().description()), Error::DUPLI);
    dumpDbase(db);
    EXPECT_EQ(db.entryCount(), entryCount);

    // get entries back from database and inspect contents
    LOG("Getting entries back from database");
    checkConsistency(db, entries);
    ASSERT_EQ(db.entryCount(), entries.size());

    // remove entry at specified index
    const int removeIdx = entryCount / 2;
    LOG("Removing item at index " << removeIdx);
    ASSERT_EQ(db.remove(removeIdx), Error::OK);
    dumpDbase(db);
    entryCount--;
    EXPECT_EQ(db.entryCount(), entryCount);
    {
        auto &e = db.entry(removeIdx);
        auto &removed = entries[removeIdx];
        EXPECT_FALSE(equal(e, removed));
        entries.removeAt(removeIdx);
    }
    ASSERT_EQ(db.entryCount(), entries.size());

    // remove set of entries with specified indices
    const QList<int> removeIdxs{0, removeIdx, entryCount-1};
    LOG("Removing set of " << removeIdxs.size() << " items");
    ASSERT_EQ(db.remove(removeIdxs), Error::OK);
    dumpDbase(db);
    entryCount -= removeIdxs.size();
    EXPECT_EQ(db.entryCount(), entryCount);
    for (auto it = removeIdxs.rbegin(); it != removeIdxs.rend(); ++it)
        entries.removeAt(*it);
    checkConsistency(db, entries);
    ASSERT_EQ(db.entryCount(), entries.size());

    // alter entry at specfic index
    const int alterIndex = entries.size() / 2;
    auto &ae2 = entries[alterIndex];
    const QString alterItem = "alteredItem", alterDesc = "alteredDesc";
    ae2.setItem(alterItem);
    ae2.setDescription(alterDesc);
    LOG("Altering item at index " << alterIndex);
    EXPECT_EQ(db.alter(alterIndex, alterItem, alterDesc), Error::OK);
    auto &ae1 = db.entry(alterIndex);
    EXPECT_TRUE(equal(ae1, ae2));
    // try to alter entry so that it becomes a duplicate of another, should fail
    EXPECT_EQ(db.alter(alterIndex, entries.front().item(), entries.front().description()), Error::DUPLI);

    dumpDbase(db);
    checkConsistency(db, entries);
    ASSERT_EQ(db.entryCount(), entries.size());

    // add some quiz points to one item
    const int pointIdx = alterIndex - 1;
    const int pointCount = 3;
    const auto pointType = DIR_SHOWITEM;
    auto &pe2 = entries[pointIdx];
    LOG("Adding " << pointCount << " points " << " for type " << pointType << " to item at index " << pointIdx);
    for (int i = 0; i < pointCount; ++i) {
        db.point(pointIdx, pointType);
        pe2.addPoint(pointType);
    }
    EXPECT_EQ(db.entry(pointIdx).points(pointType), pointCount);
    EXPECT_TRUE(equal(db.entry(pointIdx), entries.at(pointIdx)));

    // add some quiz failures to one item
    const int failIdx = alterIndex + 1;
    const int failCount = 5;
    const auto failType = DIR_SHOWDESC;
    auto &fe2 = entries[failIdx];
    LOG("Adding " << failCount << " fails " << " for type " << failType << " to item at index " << failIdx);
    for (int i = 0; i < failCount; ++i) {
        db.fail(failIdx, failType);
        fe2.addFail(failType);
    }
    EXPECT_EQ(db.entry(failIdx).fails(failType), failCount);
    EXPECT_TRUE(equal(db.entry(failIdx), entries.at(failIdx)));

    dumpDbase(db);
    checkConsistency(db, entries);
    ASSERT_EQ(db.entryCount(), entries.size());

    // save database to XML, read back and examine contents
    const QString xmlFile = "db_test.xml";
    LOG("Saving database to file: " << xmlFile);
    db.saveXML(xmlFile);
    Database xmlDb(nullptr);
    LOG("Reading database back");
    ASSERT_EQ(xmlDb.loadXML(xmlFile), Error::OK);
    dumpDbase(xmlDb);
    checkConsistency(xmlDb, entries);
    ASSERT_EQ(xmlDb.entryCount(), entries.size());
}
