#include "gtest/gtest.h"
#include "hanchar.h"
#include "log.hpp"

using namespace std;

TEST(HanCharacter, Sanity)
{
    // construct HanCharacter with specified values
    const QChar ch{'A'};
    const QStringList
            on{"on1", "on2", "on3"},
            kun{"kun1", "kun2"},
            nan{"nan1"},
            pin{"pin1", "pin2", "pin3"},
            mean{"mean1", "mean2"};
    const int stroke = 13;
    HanCharacter hc(ch, on, kun, nan, pin, mean, stroke);

    LOG("Checking basic equalities");
    EXPECT_EQ(hc.character(), ch);
    EXPECT_EQ(hc.characterStr(), ch);
    EXPECT_EQ(hc.strokes(), stroke);

    LOG("check Japanese readings");
    for (auto &strList : { on, kun, nan })
        for (auto &str : strList)
            EXPECT_TRUE(hc.jpReadingsContain(str));
    const QStringList readList{"foo", kun.front(), "bar"};
    EXPECT_TRUE(hc.jpReadingsContain(readList));
    for (auto &str : pin) EXPECT_FALSE(hc.jpReadingsContain(str));

    LOG("check Japanese meanings");
    for (auto &str : mean) EXPECT_TRUE(hc.jpMeaningsContain(str));
    for (auto &str : kun) EXPECT_FALSE(hc.jpMeaningsContain(str));

    LOG("check Chinese readings");
    for (auto &str : pin) EXPECT_TRUE(hc.cnReadingsContain(str));
    for (auto &str : kun) EXPECT_FALSE(hc.cnReadingsContain(str));
    LOG("check Chinese meanings, these are empty originally (filled by Unihan class)");
    for (auto &str : mean) EXPECT_FALSE(hc.cnMeaningsContain(str));

    LOG("check stroke match conditions");
    EXPECT_TRUE(hc.strokesMatch(stroke, EQUALS));
    EXPECT_TRUE(hc.strokesMatch(stroke+1, LESS));
    EXPECT_TRUE(hc.strokesMatch(stroke-1, MORE));
    EXPECT_FALSE(hc.strokesMatch(stroke+1, EQUALS));
    EXPECT_FALSE(hc.strokesMatch(stroke-1, LESS));
    EXPECT_FALSE(hc.strokesMatch(stroke+1, MORE));
    EXPECT_FALSE(hc.strokesMatch(stroke, NO_STROKE));
}
