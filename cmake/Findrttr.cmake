# Usage:
# FIND_PACKAGE(foxjson REQUIRED)
# [...]
# TARGET_LINK_LIBRARIES(target_name rttr)

CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

if (NOT rttr_FOUND)
  INCLUDE(FetchContent)

  FetchContent_Declare(rttr
    GIT_REPOSITORY "https://github.com/rttrorg/rttr.git"
    GIT_TAG master
  )
  FetchContent_GetProperties(rttr)

  if (NOT rttr_POPULATED)
    SET(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(rttr)
    SET(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    SET(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    SET(BUILD_DOCUMENTATION OFF CACHE BOOL "" FORCE)
    SET(BUILD_INSTALLER OFF CACHE BOOL "" FORCE)
    SET(BUILD_PACKAGE OFF CACHE BOOL "" FORCE)
    SET(CUSTOM_DOXYGEN_STYLE OFF CACHE BOOL "" FORCE)
    ADD_SUBDIRECTORY(${rttr_SOURCE_DIR} ${rttr_BINARY_DIR})
    SET(rttr_FOUND TRUE)
  endif()
endif()
