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
   GIT_TAG master
   UPDATE_COMMAND ./autogen.sh
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

Find_Package ( AaltoASR REQUIRED )
Find_Package ( SDL REQUIRED )
Find_Package ( SigC++ REQUIRED )
Find_Package ( Freetype REQUIRED )
Find_Package ( PhysFS REQUIRED )
Find_Package ( EXPAT REQUIRED )
Find_Package ( Portaudio REQUIRED )
Find_Package ( FFTW REQUIRED )
Find_Package ( SNDFILE REQUIRED )
Find_Package ( BLAS REQUIRED )
Find_Package ( LAPACK REQUIRED )

link_libraries (
    ${AaltoASR_AKU_LIBRARY}
${AaltoASR_DECODER_LIBRARY} ${AaltoASR_FSALM_LIBRARY} ${AaltoASR_MISC_LIBRARY} ${LAPACKPP_LIBRARY} ${SDL_LIBRARY} ${SigC++_LIBRARIES} ${ParaGUI_LIBRARIES}
${PHYSFS_LIBRARY} ${EXPAT_LIBRARY} ${FREETYPE_LIBRARY} ${Portaudio_LIBRARY} ${FFTW_LIBRARIES}    ${SNDFILE_LIBRARIES}
    ${BLAS_LIBRARIES}
    ${LAPACK_LIBRARIES}
)

include_directories (
 ${AaltoASR_INCLUDE_DIRS}
 ${ParaGUI_INCLUDE_DIRS}
 ${SDL_INCLUDE_DIR}
 ${SigC++_INCLUDE_DIRS}
 ${FREETYPE_INCLUDE_DIRS}
)

set(RECOGNIZERSOURCES 
recognizer.cc
 conf.cc msg.cc 
	Recognizer.cc Process.cc Adapter.cc
)

add_executable( recognizer ${RECOGNIZERSOURCES}  )

set(DECODERSOURCES
decoder.cc Decoder.cc conf.cc msg.cc endian.cc
)

add_executable( decoder ${DECODERSOURCES}  )

set(DEMOGUISOURCES
demogui.cc AudioStream.cc Buffer.cc 
	Process.cc msg.cc endian.cc 
	AudioInputController.cc 
	Application.cc Window.cc 
	WidgetWave.cc 
	WindowRecognizer.cc 
	WidgetRecognitionArea.cc WidgetRecognitionText.cc 
	WidgetScrollArea.cc WindowFile.cc WindowMessageBox.cc 
	WidgetSpectrogram.cc WindowChild.cc WindowSettings.cc 
	RecognizerListener.cc WindowStartProcess.cc 
	WidgetScrollBar.cc WindowSaveFile.cc WindowOpenFile.cc 
	WidgetTimeAxis.cc WidgetComparisonArea.cc 
	WidgetMultiLineEdit.cc conf.cc RecognizerProcess.cc WidgetAudioView.cc 
	comparison.cc WindowComparison.cc WidgetContainer.cc WindowTextEdit.cc 
	scrap.cc RecognizerStatus.cc WidgetStatus.cc
)

add_executable( demogui ${DEMOGUISOURCES} )
add_dependencies( demogui paragui_ext)

install(TARGETS recognizer decoder demogui DESTINATION bin)
