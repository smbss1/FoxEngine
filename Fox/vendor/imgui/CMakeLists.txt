cmake_minimum_required(VERSION 3.5)

# Get source files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.h")

add_library(imgui STATIC ${SOURCES})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(imgui PUBLIC cxx_std_17)
target_link_libraries(imgui PUBLIC glfw glad)