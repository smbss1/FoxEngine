# Usage:
# FIND_PACKAGE(imgui-sfml REQUIRED)
# [...]
# TARGET_LINK_LIBRARIES(target_name imgui-sfml)

CMAKE_MINIMUM_REQUIRED(VERSION 3.17)

if (NOT imgui-sfml_FOUND)
    INCLUDE(FetchContent)

    FetchContent_Declare(imgui-sfml
            GIT_REPOSITORY "https://github.com/eliasdaler/imgui-sfml.git"
            )
    FetchContent_GetProperties(imgui-sfml)

    if (NOT imgui-sfml_POPULATED)
        SET(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(imgui-sfml)
        SET(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)
        ADD_SUBDIRECTORY(${imgui-sfml_SOURCE_DIR} ${imgui-sfml_BINARY_DIR})
        SET(imgui-sfml_FOUND TRUE)
    endif()
endif()
