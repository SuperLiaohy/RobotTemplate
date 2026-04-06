cmake_minimum_required(VERSION 3.22)
project(RobotTemplate)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_executable(RobotTemplate main.cpp)

add_subdirectory(UserSDK)

target_link_libraries(RobotTemplate PRIVATE ExRobotSDK)
