cmake_minimum_required(VERSION 3.22)
project(ExRobotSDK)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE Sources algorithm/*.cpp bsp/*.cpp component/*.cpp driver/*.cpp)

add_library(ExRobotSDK ${Sources})

target_include_directories(ExRobotSDK PUBLIC algorithm bsp component driver)

