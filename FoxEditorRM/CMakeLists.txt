cmake_minimum_required(VERSION 3.8)

# Get source files

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")


# message("vsvfsdfbdfb: ${SOURCES}")
# xrepo_package("rmlui")

add_compile_definitions(RMLUI_STATIC_LIB)

add_executable(fox_editor_rm ${SOURCES})
target_include_directories(fox_editor_rm PRIVATE src ${CMAKE_CURRENT_SOURCE_DIR}/rmlui)
# xrepo_target_packages(fox_editor_rm rmlui)

target_link_directories(fox_editor_rm PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/rmlui/Static-${CMAKE_BUILD_TYPE}")

target_link_libraries(fox_editor_rm PRIVATE "RmlCore" "RmlDebugger" "freetype" fox_engine Shlwapi)


set_target_properties(fox_editor_rm PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    LINKER_LANGUAGE CXX
)
