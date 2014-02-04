#  Portaudio_FOUND - system has Portaudio
#  Portaudio_INCLUDE_DIR - the Portaudio include directory
#  Portaudio_LIBRARY - Link these to use Portaudio

find_path(Portaudio_INCLUDE_DIR
NAMES portaudio.h
PATHS
/usr/include
/usr/local/include
/opt/local/include
/sw/include
)

find_library(Portaudio_LIBRARY
NAMES portaudio
PATHS
/usr/lib
/usr/local/lib
/opt/local/lib
/sw/lib
)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Portaudio
                                  Portaudio_LIBRARY
                                  Portaudio_INCLUDE_DIR )
