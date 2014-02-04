# - Try to find SigC++1.2
# Once done, this will define
#
#  SigC++_FOUND - system has SigC++
#  SigC++_INCLUDE_DIRS - the SigC++ include directories
#  SigC++_LIBRARIES - link these to use SigC++

include(LibFindMacros)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(SigC++_PKGCONF sigc++-1.2)

# Include dir
find_path(SigC++_INCLUDE_DIR1
  NAMES sigc++/sigc++.h
  PATHS ${SigC++_PKGCONF_INCLUDE_DIRS}
  PATH_SUFFIXES sigc++-1.2
)

# Include dir
find_path(SigC++_INCLUDE_DIR2
  NAMES sigcconfig.h
  PATHS ${SigC++_PKGCONF_INCLUDE_DIRS}
)


# Finally the library itself
find_library(SigC++_LIBRARY
  NAMES sigc-1.2
  PATHS ${SigC++_PKGCONF_LIBRARY_DIRS}
)

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this this lib depends on.
set(SigC++_PROCESS_INCLUDES SigC++_INCLUDE_DIR1 SigC++_INCLUDE_DIR2)
set(SigC++_PROCESS_LIBS SigC++_LIBRARY)
libfind_process(SigC++)
