PROJECT (decoder)

Find_Package ( AaltoASR REQUIRED )

link_libraries (
${AaltoASR_DECODER_LIBRARY} ${AaltoASR_FSALM_LIBRARY} ${AaltoASR_MISC_LIBRARY} common
)

include_directories (
 ${AaltoASR_INCLUDE_DIRS}
 ${COMMON_HEADER_DIR}
)


set(DECODERSOURCES
decoder.cc Decoder.cc  
)

add_executable( decoder ${DECODERSOURCES}  )

install(TARGETS decoder DESTINATION bin)

