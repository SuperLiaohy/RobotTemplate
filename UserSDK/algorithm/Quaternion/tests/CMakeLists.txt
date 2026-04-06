add_executable(UnitQuatTest UnitQuatTest.cpp)

target_link_libraries(UnitQuatTest PRIVATE GTest::gtest_main GTest::gtest ExRobotSDK)

include(GoogleTest)
gtest_discover_tests(UnitQuatTest)
