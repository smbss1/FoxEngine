cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
project(FoxEngine VERSION 1.0.0 LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/CMakeUtils/foxCmakeUtils.cmake)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_compile_definitions(FOX_PLUGIN_DIRECTORY="${CMAKE_SOURCE_DIR}/")

add_subdirectory(Fox)
add_subdirectory(plugins)
add_subdirectory(sandbox)
add_subdirectory(Project)
add_subdirectory(editor)
