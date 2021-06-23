# Usage:
# FIND_PACKAGE(foxjson REQUIRED)
# [...]
# TARGET_LINK_LIBRARIES(target_name foxjson)

CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

if (NOT foxjson_FOUND)
  INCLUDE(FetchContent)

  FetchContent_Declare(foxjson
    GIT_REPOSITORY "https://github.com/smbss1/FoxJson.git"
    GIT_TAG main
  )
  FetchContent_GetProperties(foxjson)

  if (NOT foxjson_POPULATED)
    SET(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(foxjson)
    SET(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    ADD_SUBDIRECTORY(${foxjson_SOURCE_DIR} ${foxjson_BINARY_DIR})
    SET(foxjson_FOUND TRUE)
  endif()
endif()
