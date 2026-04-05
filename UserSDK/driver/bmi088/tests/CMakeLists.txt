add_executable(Bmi088Test Bmi088Test.cpp)

target_link_libraries(Bmi088Test PRIVATE GTest::gtest_main GTest::gtest ExRobotSDK)

include(GoogleTest)
gtest_discover_tests(Bmi088Test)
