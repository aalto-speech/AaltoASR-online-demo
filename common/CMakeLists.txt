PROJECT (common)

Find_Package ( AaltoASR REQUIRED )

link_libraries (
    ${AaltoASR_AKU_LIBRARY}
)
include_directories (
 ${AaltoASR_INCLUDE_DIRS}
)

include_directories ( . )
add_library(common msg.cc Process.cc)
