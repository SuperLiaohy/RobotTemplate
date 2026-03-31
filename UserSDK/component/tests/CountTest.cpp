#include <gtest/gtest.h>
#include "../Count.h"

using namespace EP::Component;

TEST(CountTest, Initialization) {
    Count count;
    EXPECT_EQ(count.getCount(), 0);
    EXPECT_EQ(count.getMax(), 0);
}

TEST(CountTest, IncrementAndAdd) {
    Count count;
    ++count;
    EXPECT_EQ(count.getCount(), 1);
    
    count.add();
    EXPECT_EQ(count.getCount(), 2);
    EXPECT_EQ(count.getMax(), 0);
}

TEST(CountTest, RecordMax) {
    Count count;
    count.add();
    count.add();
    count.add(); 
    
    count.record();
    EXPECT_EQ(count.getCount(), 0);
    EXPECT_EQ(count.getMax(), 3);
    
    count.add();
    count.add();
    count.record();
    EXPECT_EQ(count.getCount(), 0);
    EXPECT_EQ(count.getMax(), 2); 
}
