cmake_minimum_required(VERSION 2.8)
PROJECT(AaltoASR-online-demo)


# with -fPIC
IF(UNIX AND NOT WIN32)
  FIND_PROGRAM(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin )
  IF(CMAKE_UNAME)
    EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
    SET(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL
"processor type (i386 and x86_64)")
    IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
      ADD_DEFINITIONS(-fPIC)
    ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  ENDIF(CMAKE_UNAME)
ENDIF(UNIX AND NOT WIN32)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")




####### PARAGUI ############### BEGIN
include(ExternalProject)
ExternalProject_Add(paragui_ext
   PREFIX ${CMAKE_BINARY_DIR}/vendor/paragui
   GIT_REPOSITORY https://github.com/aalto-speech/paragui.git
   GIT_TAG onlinedemoversion 
   UPDATE_COMMAND "./autogen.sh; ./autogen.sh; ./autogen.sh"
   CONFIGURE_COMMAND <SOURCE_DIR>/configure --enable-static=yes --enable-shared=no --prefix=<INSTALL_DIR>
   BUILD_COMMAND make
   BUILD_IN_SOURCE 1
   INSTALL_COMMAND make install
)

add_library( ParaGUI UNKNOWN IMPORTED )
set_property( TARGET ParaGUI PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/vendor/paragui/lib/libparagui.a )
add_dependencies( ParaGUI paragui_ext )

set(ParaGUI_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/vendor/paragui/include
                          ${CMAKE_BINARY_DIR}/vendor/paragui/include/paragui )
set(ParaGUI_LIBRARIES ParaGUI)
####### PARAGUI ############### END

set(COMMON_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common )

add_subdirectory( common )
add_subdirectory( decoder )
add_subdirectory( recognizer )
add_subdirectory( demogui )
add_subdirectory( scripts )

install(DIRECTORY theme DESTINATION share/aalto-online-demo )

install(DIRECTORY ${Models_DIR} DESTINATION share/aalto-online-demo )

###############
set (CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Peter Smit")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
set(CPACK_PACKAGE_VERSION 0.1.3)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl1.2debian, libsigc++-1.2-5c2, libphysfs1, libexpat1, libfreetype6, libportaudio2, libfftw3-bin, libsndfile1, liblapack3gf, libgfortran3")
include(CPack)

