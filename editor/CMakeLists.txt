cmake_minimum_required(VERSION 3.17)

file(GLOB_RECURSE SRCS "src/*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(fox_editor ${SRCS})

target_include_directories(fox_editor PRIVATE "include")
target_link_libraries(fox_editor PRIVATE ${CMAKE_DL_LIBS} fox_engine CppScriptPlugin AnimatorPlugin OpenGLRendererPlugin)