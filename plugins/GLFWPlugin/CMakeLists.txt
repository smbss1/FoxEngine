cmake_minimum_required(VERSION 3.17)

# Get the folder name to set it as the plugin name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

file(GLOB_RECURSE SRCS "src/*.cpp")

find_package(glfw REQUIRED)

add_library(${PROJECT_NAME} SHARED ${SRCS})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

target_include_directories(${PROJECT_NAME} PUBLIC
    "include"
)
target_link_libraries(${PROJECT_NAME} PRIVATE fox_engine glfw)
