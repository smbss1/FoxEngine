cmake_minimum_required(VERSION 3.17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(glfw REQUIRED)
find_package(OpenGL REQUIRED)
# Get the folder name to set it as the plugin name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

file(GLOB_RECURSE SRCS "src/*.cpp")

add_library(${PROJECT_NAME} SHARED ${SRCS})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME} PUBLIC "include")
target_link_libraries(${PROJECT_NAME} PRIVATE fox_engine realsense2 glfw OpenGL::GL)
