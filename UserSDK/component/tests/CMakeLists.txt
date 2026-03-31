add_executable(ComponentTests 
    CountTest.cpp
    DetectTest.cpp
    FormatTest.cpp
    LoggerTest.cpp
    ManagerTest.cpp
    RingBufferTest.cpp
)

target_link_libraries(ComponentTests PRIVATE GTest::gtest_main GTest::gtest ExRobotSDK)

include(GoogleTest)
gtest_discover_tests(ComponentTests)
