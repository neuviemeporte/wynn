#include "dictionary_store.h"
#include "gtest/gtest.h"
#include "log.hpp"

#include <vector>

using namespace std;

struct Foo {
    int a;
};

bool operator==(const Foo &l, const Foo &r) { return l.a == r.a; }
bool operator!=(const Foo &l, const Foo &r) { return !(l == r); }
ostream& operator<<(ostream &os, const Foo &foo) { return os << "Foo{" << foo.a << "}"; }
QDataStream& operator<<(QDataStream &ds, const Foo &foo) { return ds << foo.a; }
QDataStream& operator>>(QDataStream &ds, Foo &foo) { return ds >> foo.a; }

class FooDict : public Dictionary<Foo> {
public:
    using Dictionary<Foo>::addEntry;
};

// simple test of the basic dictionary interface
TEST(DictTest, Sanity) {
    // construction of empty dictionary
    FooDict dict;
    EXPECT_FALSE(dict.error());
    EXPECT_EQ(dict.entryCount(), 0);

    // add some items and extract them
    vector<Foo> foo{Foo{1}, Foo{2}, Foo{3}};
    auto addCount = foo.size() - 1;
    auto it = foo.begin();
    for (size_t i = 0; i < addCount; ++i) {
        LOG("Adding item " << *it);
        dict.addEntry(*it++);
    }
    EXPECT_EQ(static_cast<size_t>(dict.entryCount()), addCount);
    LOG("Dictionary contents:");
    for (int i = 0; i < dict.entryCount(); ++i) {
        LOG("Item " << i << ": " << dict.entry(i));
        EXPECT_EQ(dict.entry(i), foo[i]);
    }

    // add more items and check contents of all items
    auto remainCount = foo.size() - addCount;
    for (size_t i = 0; i < remainCount; ++i) {
        LOG("Adding item " << *it);
        dict.addEntry(*it++);
    }
    EXPECT_EQ(static_cast<size_t>(dict.entryCount()), foo.size());
    LOG("Dictionary contents:");
    for (int i = 0; i < dict.entryCount(); ++i) {
        LOG("Item " << i << ": " << dict.entry(i));
        EXPECT_EQ(dict.entry(i), foo[i]);
    }

    // alter a single item
    Foo evil{666};
    const int alterIndex = 1;
    LOG("Altering item at index " << alterIndex << " to " << evil);
    dict.entry(alterIndex) = evil;
    EXPECT_NE(dict.entry(alterIndex), foo[alterIndex]);
    LOG("Dictionary contents:");
    for (int i = 0; i < dict.entryCount(); ++i) {
        LOG("Item " << i << ": " << dict.entry(i));
    }

    // save to file
    EXPECT_FALSE(dict.error());
    const QString dataFile = "dict_test.dat";
    LOG("Saving dictionary to file " << dataFile.toStdString());
    EXPECT_TRUE(dict.saveData(dataFile));

    // load back from file and compare contents
    FooDict dict2;
    EXPECT_EQ(dict2.entryCount(), 0);
    LOG("Loading dictionary from file " << dataFile.toStdString());
    dict2.loadData(dataFile);
    ASSERT_FALSE(dict2.error());
    ASSERT_EQ(dict2.entryCount(), dict.entryCount());
    for (int i = 0; i < dict.entryCount(); ++i) {
        LOG("Comparing item at index " << i << ": " << dict2.entry(i) << " to " << dict.entry(i));
        EXPECT_EQ(dict2.entry(i), dict.entry(i));
    }
}
