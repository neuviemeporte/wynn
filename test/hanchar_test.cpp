#include "hanchar.h"
#include "gtest/gtest.h"

class HancharTest : public ::testing::Test 
{
};

TEST_F(HancharTest, DefaultConstruction) 
{
    HanCharacter c;
    ASSERT_TRUE(c.character().isNull());
    ASSERT_TRUE(c.characterStr().isEmpty());
    ASSERT_EQ(c.strokes(), 0);
    ASSERT_TRUE(c.onyomiStr().isEmpty());
    ASSERT_TRUE(c.kunyomiStr().isEmpty());
    ASSERT_TRUE(c.nanoriStr().isEmpty());
    ASSERT_TRUE(c.pinyinStr().isEmpty());
    for (auto s : { c.jpReadingsStr(), c.jpMeaningsStr(), c.cnMeaningsStr(), c.simplifiedStr(), c.traditionalStr() } ) {
        ASSERT_TRUE(s.isEmpty());
    }
}
