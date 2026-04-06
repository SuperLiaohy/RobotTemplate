#include <gtest/gtest.h>
#include "../RingBuffer.h"

using namespace EP::Component;

TEST(RingBufferTest, Initialization) {
    RingBuffer<8> rb;
    EXPECT_TRUE(rb.isEmpty());
    EXPECT_FALSE(rb.isFull());
    EXPECT_EQ(rb.size(), 0);
    EXPECT_EQ(rb.available(), 7); 
}

TEST(RingBufferTest, PushPop_PowerOf2) {
    RingBuffer<8> rb; 
    
    for (uint8_t i = 0; i < 7; ++i) {
        EXPECT_TRUE(rb.push(i));
    }
    
    EXPECT_TRUE(rb.isFull());
    EXPECT_EQ(rb.size(),7);

    EXPECT_FALSE(rb.push(100)); 
    
    for (uint8_t i = 0; i < 7; ++i) {
        EXPECT_EQ(rb.pop(), i);
    }
    EXPECT_TRUE(rb.isEmpty());
}

TEST(RingBufferTest, PushPop_NonPowerOf2) {
    RingBuffer<10> rb; 
    
    for (uint8_t i = 0; i < 9; ++i) {
        EXPECT_TRUE(rb.push(i));
    }
    
    EXPECT_TRUE(rb.isFull());
    EXPECT_EQ(rb.size(),9);

    EXPECT_FALSE(rb.push(100)); 
    
    for (uint8_t i = 0; i < 9; ++i) {
        EXPECT_EQ(rb.pop(), i);
    }
    EXPECT_TRUE(rb.isEmpty());
}

TEST(RingBufferTest, PushForceAndOverwrite) {
    RingBuffer<4> rb; 
    
    rb.push(1);
    rb.push(2);
    rb.push(3);
    
    EXPECT_TRUE(rb.isFull());
    
    rb.pushForce(4);
    
    EXPECT_EQ(rb.size(), 3);
    EXPECT_EQ(rb.pop(), 2);
    EXPECT_EQ(rb.pop(), 3);
    EXPECT_EQ(rb.pop(), 4);
    EXPECT_TRUE(rb.isEmpty());
}

TEST(RingBufferTest, WriteAndRead) {
    RingBuffer<8> rb;
    uint8_t writeData[] = {1, 2, 3, 4, 5};
    uint8_t readData[5] = {0};
    
    EXPECT_TRUE(rb.write(writeData, 5));
    EXPECT_EQ(rb.size(), 5);
    
    EXPECT_TRUE(rb.read(readData, 5));
    EXPECT_TRUE(rb.isEmpty());
    
    for(int i = 0; i < 5; ++i) {
        EXPECT_EQ(readData[i], writeData[i]);
    }
}

TEST(RingBufferTest, WriteForceWrapAround) {
    RingBuffer<10> rb;
    uint8_t writeData1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    rb.write(writeData1, 9);
    
    uint8_t writeData2[] = {10, 11, 12};
    rb.writeForce(writeData2, 3);
    
    EXPECT_EQ(rb.size(), 9); 
    
    uint8_t readData[9] = {0};
    EXPECT_TRUE(rb.read(readData, 9));
    
    EXPECT_EQ(readData[0], 4);
    EXPECT_EQ(readData[5], 9);
    EXPECT_EQ(readData[6], 10);
    EXPECT_EQ(readData[7], 11);
    EXPECT_EQ(readData[8], 12);
}

TEST(RingBufferTest, AtomicHandle) {
    RingBuffer<8, uint8_t, RingBufferHandle::Atomic> rb;
    
    rb.push(42);
    EXPECT_EQ(rb.size(), 1);
    EXPECT_EQ(rb.pop(), 42);
    EXPECT_TRUE(rb.isEmpty());
}
