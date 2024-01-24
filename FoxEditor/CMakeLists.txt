cmake_minimum_required(VERSION 3.8)

# Get source files

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")

add_executable(fox_editor ${SOURCES})
target_include_directories(fox_editor PRIVATE src)

target_link_libraries(fox_editor PUBLIC fox_engine)

message(STATUS "fox_editor: ${CMAKE_CURRENT_SOURCE_DIR}")

# set(link_src "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
# if(UNIX AND NOT APPLE) 
#     set(link_dst "${CMAKE_CURRENT_BINARY_DIR}/Resources") 
# else() 
# #    set(link_dst "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/Resources") 
#     set(link_dst "${CMAKE_CURRENT_BINARY_DIR}/Resources") 
# endif() 
# add_custom_command( 
#     TARGET fox_editor
#     POST_BUILD COMMAND ${CMAKE_COMMAND} 
#         -E create_symlink ${link_src} ${link_dst} 
#     DEPENDS ${link_dst} 
#     COMMENT "symbolic link ${link_src} => ${link_dst}" 
# )

set_target_properties(fox_editor
    PROPERTIES
#     ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_CURRENT_SOURCE_DIR}>" #$<1:xxx>: use to not add config subdirectory like Debug or Release
)
