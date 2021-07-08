cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
project(FoxEngineProject VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE SCRIPT_SRCS "assets/*.cpp")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

find_package(fox_engine REQUIRED)

add_library(fox_native_script SHARED ${SCRIPT_SRCS})
target_include_directories(fox_native_script PUBLIC "assets")
target_link_libraries(fox_native_script PRIVATE fox_engine)
