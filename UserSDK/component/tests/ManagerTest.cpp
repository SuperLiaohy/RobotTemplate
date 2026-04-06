#include <gtest/gtest.h>
#include "../Manager.h"

using namespace EP::Component;

struct TestObj1 {
    int value;
    explicit TestObj1(int v) : value(v) {}
};

struct TestObj2 {
    int value;
    explicit TestObj2(int v) : value(v) {}
};

struct TestObj3 {
    int value;
    explicit TestObj3(int v) : value(v) {}
};

struct TestObj4 {
    int value;
    explicit TestObj4(int v) : value(v) {}
};

TEST(ManagerTest, InitializationAndCapacity) {
    auto& mgr = Manager<TestObj1, 5>::instance();
    EXPECT_EQ((Manager<TestObj1, 5>::capacity()), 5);
    EXPECT_EQ(mgr.size(), 0);
    EXPECT_EQ(mgr.available(), 5);
}

TEST(ManagerTest, MakeManagedAndAccess) {
    auto& mgr = Manager<TestObj2, 5>::instance();
    
    auto ptr = mgr.make_managed(42);
    EXPECT_EQ(mgr.size(), 1);
    EXPECT_EQ(mgr.available(), 4);
    
    EXPECT_EQ(ptr->value, 42);
    ptr->value = 100;
    EXPECT_EQ(ptr->value, 100);
}

TEST(ManagerTest, MoveSemanticsAndDestruction) {
    auto& mgr = Manager<TestObj3, 5>::instance();
    
    {
        auto ptr1 = mgr.make_managed(10);
        EXPECT_EQ(mgr.size(), 1);
        
        auto ptr2 = std::move(ptr1);
        EXPECT_EQ(mgr.size(), 1);
        EXPECT_EQ(ptr2->value, 10);
    }
    
    EXPECT_EQ(mgr.size(), 0);
    EXPECT_EQ(mgr.available(), 5);
}

TEST(ManagerTest, Iterator) {
    auto& mgr = Manager<TestObj4, 5>::instance();
    
    auto ptr1 = mgr.make_managed(1);
    auto ptr2 = mgr.make_managed(2);
    auto ptr3 = mgr.make_managed(3);
    
    int sum = 0;
    int count = 0;
    for (auto it = mgr.begin(); it != mgr.end(); ++it) {
        sum += it->value;
        count++;
    }
    
    EXPECT_EQ(count, 3);
    EXPECT_EQ(sum, 6);
}
