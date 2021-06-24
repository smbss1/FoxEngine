# Usage:
# FIND_PACKAGE(imgui REQUIRED)
# [...]
# TARGET_LINK_LIBRARIES(target_name imgui)

CMAKE_MINIMUM_REQUIRED(VERSION 3.17)

if (NOT imgui_FOUND)
    INCLUDE(FetchContent)

    FetchContent_Declare(imgui
            GIT_REPOSITORY "https://github.com/tamaskenez/imgui-cmake.git"
            )
    FetchContent_GetProperties(imgui)

    if (NOT imgui_POPULATED)
        SET(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(imgui)
        ADD_SUBDIRECTORY(${imgui_SOURCE_DIR} ${imgui_BINARY_DIR})
        SET(imgui_FOUND TRUE)
    endif()
endif()
