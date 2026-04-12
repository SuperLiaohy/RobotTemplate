add_executable(EkfTest EkfTest.cpp)

target_link_libraries(EkfTest PRIVATE GTest::gtest_main GTest::gtest ExRobotSDK)

include(GoogleTest)
gtest_discover_tests(EkfTest)
