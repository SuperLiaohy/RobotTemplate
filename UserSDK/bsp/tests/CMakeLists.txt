add_executable(BspTests
    UartTest.cpp
    USBCDCTest.cpp
)

target_link_libraries(BspTests PRIVATE GTest::gtest_main GTest::gtest ExRobotSDK)

include(GoogleTest)
gtest_discover_tests(BspTests)
