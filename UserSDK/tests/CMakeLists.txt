include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../algorithm/Matrix/tests ${CMAKE_CURRENT_BINARY_DIR}/MatrixTests)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../algorithm/Quaternion/tests ${CMAKE_CURRENT_BINARY_DIR}/QuaternionTests)
