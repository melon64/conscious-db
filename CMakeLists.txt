cmake_minimum_required(VERSION 3.10)
project(DBMS)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0
  SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

include_directories(src)

add_subdirectory(src)
add_subdirectory(tst)
