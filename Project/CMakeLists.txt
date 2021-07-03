cmake_minimum_required(VERSION 3.17)

file(GLOB_RECURSE SCRIPT_SRCS "assets/*.cpp")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_library(fox_native_script SHARED ${SCRIPT_SRCS})
target_include_directories(fox_native_script PUBLIC "assets")
target_link_libraries(fox_native_script PRIVATE fox_engine)
