cmake_minimum_required(VERSION 3.17)

file(GLOB_RECURSE SRCS "*.cpp")

add_library(imgui-gizmo ${SRCS})

target_include_directories(imgui-gizmo PUBLIC ImGuizmo)
target_link_libraries(imgui-gizmo PRIVATE imgui)
