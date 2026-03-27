add_executable(MatrixTest MatrixTest.cpp)

target_link_libraries(MatrixTest PRIVATE GTest::gtest_main GTest::gtest ExRobotSDK)

include(GoogleTest)
gtest_discover_tests(MatrixTest)
