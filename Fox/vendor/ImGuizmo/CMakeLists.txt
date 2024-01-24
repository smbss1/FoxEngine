cmake_minimum_required(VERSION 3.5)

# Get source files
file(GLOB IMGUI_GIZMO_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo/*.h")

add_library(imgui-gizmo STATIC ${IMGUI_GIZMO_SRCS})
target_include_directories(imgui-gizmo PUBLIC ImGuizmo)
target_compile_features(imgui-gizmo PUBLIC cxx_std_17)
target_link_libraries(imgui-gizmo PRIVATE imgui)
