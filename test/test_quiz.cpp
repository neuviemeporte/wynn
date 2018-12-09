#include "database.h"
#include "quiz.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <vector>
#include <memory>

using namespace wynn::db;
using namespace std;
using namespace testing;

class DatabaseMock : public DatabaseInterface {
public:
    MOCK_CONST_METHOD0(name, const QString&());
    MOCK_CONST_METHOD0(entryCount, int());
    MOCK_CONST_METHOD1(entry, const Entry&(int idx));
    MOCK_METHOD4(add, Error(const QString &item, const QString &desc, const QUuid &uuid, const bool dupIgnore));
    MOCK_METHOD1(remove, Error(int idx));
    MOCK_METHOD1(remove, Error(const QList<int> &idxs));
    MOCK_METHOD4(alter, Error(int idx, const QString &item, const QString &desc, bool dupIgnore));
    MOCK_METHOD2(point, Error(const int idx, const QuizDirection type));
    MOCK_METHOD2(fail, Error(const int idx, const QuizDirection type));
    MOCK_METHOD0(reset, Error());
    MOCK_CONST_METHOD1(saveXML, void(const QString &path));
    MOCK_CONST_METHOD1(saveXMLExtraAttributes, void(QXmlStreamWriter &));
    MOCK_CONST_METHOD1(saveXMLExtraElements, void(QXmlStreamWriter &));
    MOCK_METHOD1(loadXML, Error(const QString &path));
    MOCK_METHOD1(loadXMLExtraAttributes, void(QXmlStreamReader&));
    MOCK_METHOD2(loadXMLExtraElements, void(QXmlStreamReader &xml, const QString &name));
    MOCK_METHOD0(loadXmlPostCheck, Error());
};

const vector<Entry> DB_ENTRIES = {
    { QUuid{}, -1, -1, "qA", "dA", 0, 0, 0, 0 },
    { QUuid{}, -1, -1, "qB", "dB", 0, 0, 0, 0 },
    { QUuid{}, -1, -1, "qC", "dC", 0, 0, 0, 0 },
    { QUuid{}, -1, -1, "qD", "dD", 0, 0, 0, 0 },
    { QUuid{}, -1, -1, "qE", "dE", 0, 0, 0, 0 },
    { QUuid{}, -1, -1, "qF", "dF", 0, 0, 0, 0 },
    { QUuid{}, -1, -1, "qG", "dG", 0, 0, 0, 0 },
    { QUuid{}, -1, -1, "qH", "dH", 0, 0, 0, 0 },
    { QUuid{}, -1, -1, "qI", "dI", 0, 0, 0, 0 },
    { QUuid{}, -1, -1, "qJ", "dJ", 0, 0, 0, 0 },
};

class QuizTest : public ::testing::Test {
public:
    const QString DB_NAME = "quizdb";
    DatabaseMock dbMock_;

    void SetUp() override {
        EXPECT_CALL(dbMock_, name).WillOnce(ReturnRef(DB_NAME));
        EXPECT_CALL(dbMock_, entryCount).WillRepeatedly(Return(static_cast<int>(DB_ENTRIES.size())));
        EXPECT_CALL(dbMock_, entry(_)).WillRepeatedly(Invoke([](int i){
            return DB_ENTRIES.at(i);
        }));
    }
};

TEST_F(QuizTest, Sanity) {
    const QuizDirection qdir = DIR_SHOWDESC;
    QuizSettings qset;
    Quiz quiz(&dbMock_, qset);

}
