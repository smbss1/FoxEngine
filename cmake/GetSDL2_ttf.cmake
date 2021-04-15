#------------------------------------------------------------------------------
# External Project:     SDL2
# Downloads:            https://libsdl.org/release/
#------------------------------------------------------------------------------

include(ExternalProject)

set(SDL2_TTF_VERSION "2.0.15")

set(DEPENDENCY_DOWNLOAD_DIR_ttf "${DEPENDENCY_DOWNLOAD_DIR}/SDL2_ttf")
set(DEPENDENCY_EXTRACT_DIR_ttf "${DEPENDENCY_EXTRACT_DIR}/SDL2_ttf")

# For Windows it's faster to download the pre-compiled development binaries.
if (WIN32)

  if(MINGW) # download pre-compiled SDL2 dev package for MinGW 32/64-bit
    
    ExternalProject_Add(
      sdl2_ttf
      PREFIX ${DEPENDENCY_EXTRACT_DIR_ttf}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR_ttf}
      https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-${SDL2_TTF_VERSION}-mingw.tar.gz
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR_ttf}/src/sdl2_ttf/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR_ttf}/src/sdl2_ttf/include ${DEPENDENCY_INSTALL_DIR}/include/SDL2
    )

  endif(MINGW)

  if(MSVC) # download pre-compiled SDL2 dev package for Visual C++ 32/64-bit

    ExternalProject_Add(
      sdl2_ttf
      PREFIX ${DEPENDENCY_EXTRACT_DIR_ttf}
      DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR_ttf}
      URL https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-${SDL2_TTF_VERSION}-VC.zip
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR_ttf}/src/sdl2_ttf/lib     ${DEPENDENCY_INSTALL_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPENDENCY_EXTRACT_DIR_ttf}/src/sdl2_ttf/include ${DEPENDENCY_INSTALL_DIR}/include/SDL2
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
    sdl2_ttf
    PREFIX ${DEPENDENCY_EXTRACT_DIR_ttf}
    DOWNLOAD_DIR ${DEPENDENCY_DOWNLOAD_DIR_ttf}
    URL https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-${SDL2_TTF_VERSION}.tar.gz
    INSTALL_DIR ${DEPENDENCY_INSTALL_DIR}
    CMAKE_ARGS ${SDL2_CMAKE_ARGS}
  )

endif()