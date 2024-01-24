cmake_minimum_required(VERSION 3.13.0)

project(MyProject VERSION 1.0
                  DESCRIPTION "Very nice project"
                  LANGUAGES CXX)

# Let's ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# Let's nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

# Download xrepo.cmake if not exists in build directory.
# if(NOT EXISTS "${CMAKE_BINARY_DIR}/xrepo.cmake")
#     message(STATUS "Downloading xrepo.cmake from https://github.com/xmake-io/xrepo-cmake/")
#     # mirror https://cdn.jsdelivr.net/gh/xmake-io/xrepo-cmake@main/xrepo.cmake
#     file(DOWNLOAD "https://raw.githubusercontent.com/xmake-io/xrepo-cmake/main/xrepo.cmake"
#                   "${CMAKE_BINARY_DIR}/xrepo.cmake"
#                   TLS_VERIFY ON)
# endif()

# Include xrepo.cmake so we can use xrepo_package function.
# include(${CMAKE_BINARY_DIR}/xrepo.cmake)



add_subdirectory("Fox")
add_subdirectory("FoxEditor")
add_subdirectory("FoxEditorRM")

# add_subdirectory("sandbox")