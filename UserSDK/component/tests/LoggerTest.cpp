#include <gtest/gtest.h>
#include "../Logger.h"
#include <string>
#include <vector>

using namespace EP::Component;

static std::vector<uint8_t> outputBuffer;

void mockPrint(uint8_t* data, size_t len) {
    outputBuffer.insert(outputBuffer.end(), data, data + len);
}

TEST(LoggerTest, RawLogAndFlush) {
    outputBuffer.clear();
    Logger<mockPrint> logger;
    
    const char* msg = "Hello ";
    logger.log(msg, 6);
    
    const char* msg2 = "World";
    logger.log(msg2, 5);
    
    logger.flush();
    
    std::string result(outputBuffer.begin(), outputBuffer.end());
    EXPECT_EQ(result, "Hello World");
}

TEST(LoggerTest, FormattedLog) {
    outputBuffer.clear();
    Logger<mockPrint> logger;
    
    logger.log<Str{"Value: {}"}>(42);
    logger.flush();
    
    if (!outputBuffer.empty() && outputBuffer.back() == '\0') {
        outputBuffer.pop_back();
    }
    
    std::string result(outputBuffer.begin(), outputBuffer.end());
    EXPECT_EQ(result, "Value: 42");
}
