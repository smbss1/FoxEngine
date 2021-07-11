cmake_minimum_required(VERSION 3.17)

file(GLOB_RECURSE SRCS "*.cpp")

add_library(imgui ${SRCS})

target_include_directories(imgui PUBLIC ImGui)
