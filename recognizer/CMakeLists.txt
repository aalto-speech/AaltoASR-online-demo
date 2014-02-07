PROJECT (recognizer)

Find_Package ( AaltoASR REQUIRED )
Find_Package ( FFTW REQUIRED )
Find_Package ( SNDFILE REQUIRED )
Find_Package ( BLAS REQUIRED )
Find_Package ( LAPACK REQUIRED )
find_package ( Threads REQUIRED )

link_libraries (
    ${AaltoASR_AKU_LIBRARY}
 ${LAPACKPP_LIBRARY} 
  ${FFTW_LIBRARIES}    ${SNDFILE_LIBRARIES}
    ${BLAS_LIBRARIES}
    ${LAPACK_LIBRARIES}
    common
)

include_directories (
 ${COMMON_HEADER_DIR}
 ${AaltoASR_INCLUDE_DIRS}
 
)


set(RECOGNIZERSOURCES 
recognizer.cc
	${MSG_SOURCE} Recognizer.cc Adapter.cc
)

add_executable( recognizer ${RECOGNIZERSOURCES}  )

target_link_libraries (recognizer ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS recognizer DESTINATION bin)
