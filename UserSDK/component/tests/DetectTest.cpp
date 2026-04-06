#include <gtest/gtest.h>
#include "../Detect.h"

using namespace EP::Component;
using namespace EP::Component::DetectDep;

static uint32_t mockTime = 0;
uint32_t getMockTime() {
    return mockTime;
}

static State lastCallbackState = State::WORKING;
static void* lastCallbackParam = nullptr;

void mockCallback(State s, void* param) {
    lastCallbackState = s;
    lastCallbackParam = param;
}

TEST(DetectTest, Initialization) {
    mockTime = 0;
    Detect<getMockTime> detector(100);
    EXPECT_EQ(detector.detect(), State::WORKING);
}

TEST(DetectTest, WorkingState) {
    mockTime = 0;
    Detect<getMockTime> detector(100);
    
    mockTime = 50;
    detector.update();
    
    mockTime = 90;
    EXPECT_EQ(detector.detect(), State::WORKING);
}

TEST(DetectTest, LoseAndMissingState) {
    mockTime = 0;
    Detect<getMockTime> detector(100);
    
    mockTime = 50;
    detector.update();
    
    mockTime = 150;
    EXPECT_EQ(detector.detect(), State::LOSE);
    
    mockTime = 200;
    EXPECT_EQ(detector.detect(), State::MISSING);
}

TEST(DetectTest, RecoverState) {
    mockTime = 0;
    Detect<getMockTime> detector(100);
    
    mockTime = 50;
    detector.update();
    
    mockTime = 150;
    EXPECT_EQ(detector.detect(), State::LOSE);
    
    mockTime = 160;
    detector.update();
    
    mockTime = 170;
    EXPECT_EQ(detector.detect(), State::RECOVER);
    
    mockTime = 180;
    EXPECT_EQ(detector.detect(), State::WORKING);
}

TEST(DetectTest, CallbackInvocation) {
    mockTime = 0;
    int dummyParam = 42;
    Function cb = {mockCallback, &dummyParam};
    
    Detect<getMockTime> detector(100, cb);
    
    mockTime = 150;
    detector.callbackFun();
    
    EXPECT_EQ(lastCallbackState, State::LOSE);
    EXPECT_EQ(lastCallbackParam, &dummyParam);
}
