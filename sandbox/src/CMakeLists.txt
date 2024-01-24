cmake_minimum_required(VERSION 3.8)

# Get source files
file(GLOB CPP_LIST "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.hpp")
set(SOURCES ${CPP_LIST} CACHE INTERNAL STRINGS)