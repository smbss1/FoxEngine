cmake_minimum_required(VERSION 3.17)

find_package(SFML COMPONENTS audio graphics window system network REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(glfw REQUIRED)

# Get the folder name to set it as the plugin name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

if (SFML_FOUND)
    set(SFML_LIBRARIES sfml-audio sfml-graphics sfml-window sfml-system sfml-network)
    file(GLOB_RECURSE SRCS "src/*.cpp")

    add_library(${PROJECT_NAME} SHARED ${SRCS})

    set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

    target_include_directories(${PROJECT_NAME} PUBLIC
        "include"
    )
#    target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_INCLUDE_IMCONFIG_H)
#
#    target_include_directories(${PROJECT_NAME} PRIVATE
#        ${CMAKE_CURRENT_LIST_DIR} # needed for imgui.h to find our imconfig.h
#    )
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SFML_LIBRARIES} fox_engine glfw)
endif()