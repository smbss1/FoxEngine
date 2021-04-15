cmake_minimum_required(VERSION 3.16)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(SFML 2 COMPONENTS audio graphics window system network)

if (SFML_FOUND)
    file(GLOB_RECURSE SRCS "src/*.cpp")

    add_library(fox_sfml SHARED ${SRCS} ${CORE_SYSTEM})
    set_target_properties(fox_sfml PROPERTIES PREFIX "")

    target_include_directories(fox_sfml PRIVATE
        include
        ${SFML_INCLUDE_DIR}
    )
    target_link_libraries(fox_sfml PRIVATE ${SFML_LIBRARIES})
else()
    message("[WARNING]: SFML library not found, fox_sfml will not be compiled\n")
endif()