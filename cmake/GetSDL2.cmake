#------------------------------------------------------------------------------
# External Project:     SDL2
# Downloads:            https://libsdl.org/release/
#------------------------------------------------------------------------------

include(ExternalProject)

set(SDL2_VERSION "2.0.9")

set(DEPENDENCY_DOWNLOAD_DIR_sdl2 "${DEPENDENCY_DOWNLOAD_DIR}/SDL2")
set(DEPENDENCY_EXTRACT_DIR_sdl2 "${DEPENDENCY_EXTRACT_DIR}/SDL2")

# For Windows it's faster to download the pre-compiled development binaries.
if (WIN32)

  if(MINGW) # download pre-compiled SDL2 dev package for MinGW 32/64-bit
    
    ExternalProject_Add(
      sdl2
      PREFIX ${DEPENDENCY_EXTRACT_DIR_sdl2}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR_sdl2}
      URL http://libsdl.org/release/SDL2-devel-${SDL2_VERSION}-mingw.tar.gz
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR_sdl2}/src/sdl2/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR_sdl2}/src/sdl2/include ${DEPENDENCY_INSTALL_DIR}/include/SDL2
    )

  endif(MINGW)

  if(MSVC) # download pre-compiled SDL2 dev package for Visual C++ 32/64-bit

    ExternalProject_Add(
      sdl2
      PREFIX ${DEPENDENCY_EXTRACT_DIR_sdl2}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR_sdl2}
      URL http://libsdl.org/release/SDL2-devel-${SDL2_VERSION}-VC.zip
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR_sdl2}/src/sdl2/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR_sdl2}/src/sdl2/include ${DEPENDENCY_INSTALL_DIR}/include/SDL2
    )

  endif(MSVC)

else() # build from source

  set(SDL2_CMAKE_ARGS
      -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
      -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> 
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
  )
   
  ExternalProject_Add(
    sdl2
    PREFIX ${DEPENDENCY_EXTRACT_DIR_sdl2}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR_sdl2}
    URL http://libsdl.org/release/SDL2-${SDL2_VERSION}.tar.gz
    INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
    CMAKE_ARGS ${SDL2_CMAKE_ARGS}
  )

endif()