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

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest")
  message(STATUS "Fetching GoogleTest from git")
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
else()
  message(STATUS "GoogleTest directory exists")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/googletest)
endif()

include_directories(src)

add_subdirectory(src)
add_subdirectory(tst)
