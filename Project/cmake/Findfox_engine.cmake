# Usage:
# FIND_PACKAGE(fox_engine REQUIRED)
# [...]
# TARGET_LINK_LIBRARIES(target_name fox_engine)

CMAKE_MINIMUM_REQUIRED(VERSION 3.17)

if (NOT fox_engine_FOUND)
  INCLUDE(FetchContent)

  FetchContent_Declare(fox_engine
    GIT_REPOSITORY "https://github.com/smbss1/FoxEngine.git"
    GIT_TAG main
  )
  FetchContent_GetProperties(fox_engine)

  if (NOT fox_engine_POPULATED)
    SET(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(fox_engine)
    ADD_SUBDIRECTORY(${fox_engine_SOURCE_DIR} ${fox_engine_BINARY_DIR})
    SET(fox_engine_FOUND TRUE)
  endif()
endif()
