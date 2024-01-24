cmake_minimum_required(VERSION 3.8)

# Get source files
add_subdirectory("src")

add_executable(sandbox ${SOURCES})
target_include_directories(sandbox PRIVATE include)

target_link_libraries(sandbox PRIVATE fox_engine)