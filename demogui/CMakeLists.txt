PROJECT ( demogui )

Find_Package ( AaltoASR REQUIRED )
Find_Package ( FFTW REQUIRED )
Find_Package ( SDL REQUIRED )
Find_Package ( SigC++ REQUIRED )
Find_Package ( Freetype REQUIRED )
Find_Package ( PhysFS REQUIRED )
Find_Package ( EXPAT REQUIRED )
Find_Package ( Portaudio REQUIRED )
Find_Package ( X11 REQUIRED )
Find_Package ( SNDFILE REQUIRED )

link_libraries (
 ${SDL_LIBRARY} ${SigC++_LIBRARIES} ${ParaGUI_LIBRARIES}
 ${PHYSFS_LIBRARY} ${EXPAT_LIBRARY} ${FREETYPE_LIBRARY} ${Portaudio_LIBRARY} ${FFTW_LIBRARIES}
 ${X11_LIBRARIES} ${SNDFILE_LIBRARIES}
 common
)

include_directories (
 ${COMMON_HEADER_DIR}
 ${ParaGUI_INCLUDE_DIRS}
 ${SDL_INCLUDE_DIR}
 ${SigC++_INCLUDE_DIRS}
 ${FREETYPE_INCLUDE_DIRS}
 ${AaltoASR_INCLUDE_DIRS}
)

set(DEMOGUISOURCES
demogui.cc AudioStream.cc Buffer.cc  
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
	WidgetMultiLineEdit.cc RecognizerProcess.cc WidgetAudioView.cc 
	comparison.cc WindowComparison.cc WidgetContainer.cc WindowTextEdit.cc 
	scrap.cc RecognizerStatus.cc WidgetStatus.cc
)

add_executable(demogui ${DEMOGUISOURCES})
add_dependencies(demogui paragui_ext)

install(TARGETS demogui DESTINATION bin)
