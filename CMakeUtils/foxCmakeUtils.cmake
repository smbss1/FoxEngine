
function(fox_add_all_subdirs)

    # find all cmake files below this directory
    file (GLOB SUB_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/*/CMakeLists.txt")

    foreach (VAR ${SUB_DIRS})

        get_filename_component (RES ${VAR} DIRECTORY)

        add_subdirectory (${RES})

    endforeach ()

endfunction()
