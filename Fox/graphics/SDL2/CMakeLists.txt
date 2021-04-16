cmake_minimum_required(VERSION 3.16)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(SDL2)
find_package(SDL2_ttf)

if (SDL2_FOUND AND SDL2_TTF_FOUND)
    file(GLOB_RECURSE SRCS "src/*.cpp")

    add_library(arcade_sdl2 SHARED ${SRCS} ${CORE_SYSTEM})
    set_target_properties(arcade_sdl2 PROPERTIES PREFIX "")

    target_include_directories(arcade_sdl2 PRIVATE
        include
        ${SDL2_INCLUDE_DIR}
        ${SDL2_TTF_INCLUDE_DIRS}
    )
    target_link_libraries(arcade_sdl2 PRIVATE ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
elseif(NOT SDL2_FOUND)
    message("[WARNING]: SDL2 library not found, arcade_sdl2 will not be compiled\n")
elseif(NOT SDL2_TTF_FOUND)
    message("[WARNING]: SDL2_ttf library not found, arcade_sdl2 will not be compiled\n")
endif()
