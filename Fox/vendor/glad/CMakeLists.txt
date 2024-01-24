cmake_minimum_required(VERSION 3.5)

set(GLAD_SOURCE_FILES
  glad/glad.cpp)

set(GLAD_HEADER_FILES
  glad/glad.h
  KHR/khrplatform.h)

add_library(glad STATIC ${GLAD_SOURCE_FILES} ${GLAD_HEADER_FILES})
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GLAD_SOURCE_FILES} ${GLAD_HEADER_FILES})