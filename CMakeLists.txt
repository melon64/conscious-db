cmake_minimum_required(VERSION 3.10)
project(DB)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(DB db.cc)

enable_testing()

add_executable(
  read_test
  tests/read_test.cc
)

add_executable(
  example_test
  tests/example_test.cc
)

target_link_libraries(
  read_test
  GTest::gtest_main
)

target_link_libraries(
  example_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(read_test)
gtest_discover_tests(example_test)
