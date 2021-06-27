# Usage:
# FIND_PACKAGE(glfw REQUIRED)
# [...]
# TARGET_LINK_LIBRARIES(target_name glfw)

CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

if (NOT glfw_FOUND)
  INCLUDE(FetchContent)

  FetchContent_Declare(glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.3.4"
  )
  FetchContent_GetProperties(glfw)

  if (NOT glfw_POPULATED)
    SET(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(glfw)
    SET(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    ADD_SUBDIRECTORY(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
    SET(glfw_FOUND TRUE)
  endif()
endif()
