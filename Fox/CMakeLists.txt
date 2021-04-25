cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE CORE_SRCS "src/*.cpp")
add_subdirectory(graphics)

# set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR})

# find_package(Threads REQUIRED)

add_library(fox_engine STATIC ${CORE_SRCS})
target_include_directories(fox_engine PUBLIC "include")
target_link_libraries(fox_engine PUBLIC ${CMAKE_DL_LIBS})
target_compile_definitions(fox_engine PRIVATE FOX_BUILD_LIB=1)
# target_link_libraries(app PRIVATE Threads::Threads ${CMAKE_DL_LIBS})