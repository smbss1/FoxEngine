cmake_minimum_required(VERSION 3.17)

file(GLOB_RECURSE CORE_SRCS "src/*.cpp")

# set(THREADS_PREFER_PTHREAD_FLAG ON)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC" )
set( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC" )

# find_package(Threads REQUIRED)
find_package(foxjson REQUIRED)
find_package(yamlcpp REQUIRED)
find_package(rttr REQUIRED)

add_subdirectory(vendor)

add_library(fox_engine SHARED ${CORE_SRCS})
target_include_directories(fox_engine PUBLIC "include")
target_link_libraries(fox_engine PUBLIC ${CMAKE_DL_LIBS} fox_json yaml-cpp imgui imgui-gizmo rttr_core)
target_compile_definitions(fox_engine PRIVATE FOX_BUILD_LIB=1)
target_compile_definitions(fox_engine PUBLIC
    $<$<CONFIG:DEBUG>:FOX_DEBUG=1>
)
# target_link_libraries(app PRIVATE Threads::Threads ${CMAKE_DL_LIBS})