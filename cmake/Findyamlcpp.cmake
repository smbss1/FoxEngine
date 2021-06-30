# Usage:
# FIND_PACKAGE(yamlcpp REQUIRED)
# [...]
# TARGET_LINK_LIBRARIES(target_name yamlcpp)

CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

if (NOT yamlcpp_FOUND)
  INCLUDE(FetchContent)

  FetchContent_Declare(yamlcpp
    GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp.git"
    GIT_TAG master
  )
  FetchContent_GetProperties(yamlcpp)

  if (NOT yamlcpp_POPULATED)
    SET(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(yamlcpp)
    SET(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    ADD_SUBDIRECTORY(${yamlcpp_SOURCE_DIR} ${yamlcpp_BINARY_DIR})
    SET(yamlcpp_FOUND TRUE)
  endif()
endif()
