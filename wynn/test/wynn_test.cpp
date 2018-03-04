#include "wynn_test.h"

#include "ixplog_active.h"
#include "database.h"

#define TEST_BASIC 0

#ifdef __linux__
#include <unistd.h>
#define SLEEP(x) sleep(x)
#define CLEAR system("clear")
#elif defined(_WIN32)
#include <WinBase.h>
#define SLEEP(x) Sleep(1000 * x)
#define CLEAR system("cls")
#endif

QTSLogger 
	APP_LOG("test_log.txt"),
	*APP_LOGSTREAM = &APP_LOG;

QTextStream out(stdout);

void TestTask::run()
{
	CLEAR;

    DatabaseTest dbTest;
    ixp::TestResult result = dbTest.run();

    out << "Completed test '" << result.name << "', ok = " << result.oks << ", fail = " << result.fails << endl;

	emit finished();
}

void DatabaseTest::execute()
{
#if TEST_BASIC > 0

	msg("DbClocks");
	{
		const QUuid 
			master1 = QUuid::createUuid(),
			master2 = QUuid::createUuid(),
			id1 = QUuid::createUuid(),
			id2 = QUuid::createUuid();

		DbClocks c1(master1);
		c1.tick(master1);
		DbClocks c2(master2);
		c2.fork(c1);
		test("operator==", c1 == c2);

		c2.tick(id1);
		test("operator!=", c1 != c2);
	}

    msg("Basic Database functionality");
    {
        idup();

        const QString dbName("Test");
        Database db(NULL, dbName);
        test("Database()", db.name() == dbName && db.entryCount() == 0 && db.dirty() && !db.error() && !db.locked() && !db.external());

        const QString itemAStr("itemA"), descAStr("descA");
        DbError err = db.add(itemAStr, descAStr);
        Q_ASSERT(err == DbError::ERROR_OK);
        const DbEntry dbEntryA = db.entry(0);
        test("add()", err == DbError::ERROR_OK && db.entryCount() == 1 &&
             dbEntryA.item() == itemAStr && dbEntryA.description() == descAStr &&
             dbEntryA.points(DbEntry::DIR_SHOWDESC) == 0 && dbEntryA.points(DbEntry::DIR_SHOWITEM) == 0 &&
             dbEntryA.fails(DbEntry::DIR_SHOWDESC) == 0  && dbEntryA.fails(DbEntry::DIR_SHOWITEM) == 0);

        const QString itemBStr("itemB"), descBStr("descB"), itemCStr("itemC"), descCStr("descC");
        db.add(itemBStr, descBStr);
        db.add(itemCStr, descCStr);
        test("more add()", db.entryCount() == 3);

        DbError errDup = db.add(itemAStr, "sth");
        test("duplicate item add()", errDup == DbError::ERROR_DUPLI && db.entryCount() == 3);
        errDup = db.add("sth", descAStr);
        test("duplicate desc add()", errDup == DbError::ERROR_DUPLI && db.entryCount() == 3);

        const DbEntry dbEntryB = db.entry(1);
        const int idx = db.entryIndex(dbEntryB.uuid());
        test("entryIndex()", idx == 1);
        test("findEntry()", db.findEntry(itemCStr) == 2 && db.findEntry(descAStr) == 0);

        QDateTime date = QDateTime::currentDateTime();
        const DbEntry
                dbEntryC = db.entry(2),
                entryDupA(QUuid(), date, itemAStr + "dup", "dup"),
                entryDupC(QUuid(), date, "dup", descCStr + "dup");
        test("findDuplicate()",
             db.findDuplicate(entryDupA) == db.entryIndex(dbEntryA.uuid()) &&
             db.findDuplicate(entryDupC) == db.entryIndex(dbEntryC.uuid()));

        const QString itemDStr("itemD"), descDStr("descD");
        db.setLocked(true);
        test("setLocked(true)", db.locked() && db.add(itemDStr, descDStr) == DbError::ERROR_LOCK && db.entryCount() == 3);
        db.setLocked(false);
        test("setLocked(false)", !db.locked() && db.add(itemDStr, descDStr) == DbError::ERROR_OK && db.entryCount() == 4);

        db.remove(1);
        const DbEntry &entry1 = db.entry(1);
        test("remove()", db.entryCount() == 3 && db.entryIndex(dbEntryB.uuid()) < 0 && entry1 == dbEntryC);

        const QString itemAlterC(itemCStr + "alter"), descAlterC(descCStr + "alter");
        DbError alterError = db.alter(1, itemAlterC, descAlterC);
        const DbEntry entryAlterC = db.entry(1);
        test("alter()", alterError == DbError::ERROR_OK && entryAlterC.item() == itemAlterC && entryAlterC.description() == descAlterC);
        alterError = db.alter(1, itemAStr, "sth");
        test("duplicate item alter()", alterError == DbError::ERROR_DUPLI && db.entry(1) == entryAlterC);
        alterError = db.alter(1, "sth", descAStr);
        test("duplicate desc alter()", alterError == DbError::ERROR_DUPLI && db.entry(1) == entryAlterC);

        DbError pointError = db.point(1, DbEntry::DIR_SHOWDESC);
        test("point(SHOWDESC)", pointError == DbError::ERROR_OK && entry1.points(DbEntry::DIR_SHOWDESC) == 1);
        pointError = db.point(1, DbEntry::DIR_SHOWITEM);
        test("point(SHOWITEM)", pointError == DbError::ERROR_OK && entry1.points(DbEntry::DIR_SHOWITEM) == 1);

        DbError failError = db.fail(1, DbEntry::DIR_SHOWDESC);
        test("fail(SHOWDESC)", failError == DbError::ERROR_OK
             && entry1.points(DbEntry::DIR_SHOWDESC) == 0
             && entry1.fails(DbEntry::DIR_SHOWDESC) == 1);
        failError = db.fail(1, DbEntry::DIR_SHOWITEM);
        test("fail(SHOWITEM)", pointError == DbError::ERROR_OK
             && entry1.points(DbEntry::DIR_SHOWITEM) == 0
             && entry1.fails(DbEntry::DIR_SHOWITEM) == 1);

        const QString xmlFile("test_db.xml");
        db.saveXML(xmlFile);
        test("saveXML()", QFile(xmlFile).exists());

        Database db2(NULL);
        db2.loadXML(xmlFile);
        test("loadXML()", !db2.error() && db2.equal(db, true));

        iddn();
    }
#endif // TEST_BASIC


    msg("Database synchronization");
    {
        idup();

        Database dbA(NULL, "Test");
        dbA.add("item", "desc");

        msg("A: point 1"); dbA.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
        msg("A: point 2"); dbA.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
        msg("A: point 3"); dbA.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);

        Database dbB(NULL);
        dbB.fork(dbA);

		msg("B: point 4"); dbB.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
		msg("A: point 4"); dbA.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
		msg("B: point 5"); dbB.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);

		Database dbC(NULL);
		dbC.fork(dbB);

		msg("A: point 5"); dbA.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
		msg("B: point 6"); dbB.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
		msg("C: point 6"); dbC.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);

		Database dbD(NULL);
		dbD.fork(dbA);

		msg("A: point 6"); dbA.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
		msg("C: point 7"); dbC.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
		msg("B: point 7"); dbB.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
		msg("D: point 6"); dbD.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
		msg("C: point 8"); dbC.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);

		dbB.sync(dbC);
		test("dbB consistentcy", dbB.checkEventsClocksConsistency());
		test("dbC consistentcy", dbC.checkEventsClocksConsistency());
		test("B and C clocks equal", dbB.clocks() == dbC.clocks());

		msg("A: point 7"); dbA.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);

		dbA.sync(dbD);
		test("dbA consistentcy", dbA.checkEventsClocksConsistency());
		test("dbD consistentcy", dbD.checkEventsClocksConsistency());
		test("A and D clocks equal", dbA.clocks() == dbD.clocks());

		msg("B: point 11"); dbB.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);
		msg("B: point 12"); dbB.point(0, DbEntry::DIR_SHOWDESC); SLEEP(1);

		dbB.sync(dbA);
		test("dbB consistentcy", dbB.checkEventsClocksConsistency());
		test("dbA consistentcy", dbA.checkEventsClocksConsistency());
		test("B and A clocks equal", dbB.clocks() == dbA.clocks());

		const int 
			ptsA = dbA.entry(0).points(DbEntry::DIR_SHOWDESC),
			ptsB = dbB.entry(0).points(DbEntry::DIR_SHOWDESC);

		test("Point value of item after sync", ptsA == ptsB && ptsA == 17);

		dbA.saveXML("test_dbA.xml");
        dbB.saveXML("test_dbB.xml");
		dbC.saveXML("test_dbC.xml");
        dbD.saveXML("test_dbD.xml");

        iddn();
    }



}
