cmake_minimum_required(VERSION 3.17)

file(GLOB_RECURSE SRCS "*.cpp")

add_library(imgui-node-editor ${SRCS})

target_include_directories(imgui-node-editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imgui-node-editor PRIVATE imgui)
