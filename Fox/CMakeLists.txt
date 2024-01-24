cmake_minimum_required(VERSION 3.8)

set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_SHARED_LIBRARY OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)
set(BUILD_MONOPP_TESTS OFF CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)

add_subdirectory("vendor/box2d")
add_subdirectory("vendor/FoxJson")

# set(BUILD_STATIC_LIBS ON)
# add_subdirectory("vendor/glm/glm")
# target_include_directories(glm_static PUBLIC "vendor/glm")

add_subdirectory("vendor/glad")
add_subdirectory("vendor/glfw")
add_subdirectory("vendor/entt")
add_subdirectory("vendor/imgui")
add_subdirectory("vendor/ImGuizmo")
add_subdirectory("vendor/yaml-cpp")

add_subdirectory("src")

file(GLOB_RECURSE GLM_CPP_LIST "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm/*.hpp")
file(GLOB_RECURSE MONO_CPP_LIST "${CMAKE_CURRENT_SOURCE_DIR}/vendor/mono/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/vendor/mono/*.h")

set(STB_IMAGE_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image/stb_image.cpp ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image/stb_image.h)

add_library(fox_engine STATIC ${FOX_ENGINE_SOURCES} ${GLM_CPP_LIST} ${MONO_CPP_LIST} ${STB_IMAGE_SRCS})
target_include_directories(fox_engine PUBLIC src/Fox src/Platform vendor/glm vendor/stb_image)
target_compile_features(fox_engine PUBLIC cxx_std_17)

set_target_properties(fox_engine PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    LINKER_LANGUAGE CXX
)

target_include_directories(fox_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/mono/include)
target_link_directories(fox_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/mono/lib/${CMAKE_BUILD_TYPE})
target_link_libraries(fox_engine PUBLIC "mono-2.0-sgen") # also adds the required include path

target_link_libraries(fox_engine
    PUBLIC
    ${OPENGL_LIBRARY}
#     glm_static
    glad
    glfw
    fox_json
    box2d
    yaml-cpp
    entt
    imgui
    imgui-gizmo
)

target_compile_definitions(fox_engine PUBLIC FOX_DEBUG)
