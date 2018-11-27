#include "database.h"
#include "gtest/gtest.h"
#include "ixplog_active.h"

// declared extern in wynndb, link error if not defined
QTSLogger *APP_LOGSTREAM = nullptr;

TEST(DbaseTest, Sanity) {
    const QString dbName{"testdb"};
    Database db(nullptr, dbName);
    EXPECT_EQ(db.name(), dbName);
    EXPECT_EQ(db.entryCount(), 0);
    EXPECT_FALSE(db.dirty());
    EXPECT_FALSE(db.error());
    EXPECT_FALSE(db.locked());
    EXPECT_FALSE(db.external());
}