#  AaltoASR_INCLUDE_DIRS   - where to find include files
#  AaltoASR_LIBRARIES      - List of libraries when using AaltoASR
#  AaltoASR_FOUND          - True if AaltoASR found

# HINTS:
# AaltoASR_PREFIX_

if(DEFINED AaltoASR_PREFIX)
if(NOT IS_ABSOLUTE ${AaltoASR_PREFIX} )
set(AaltoASR_PREFIX ${CMAKE_BINARY_DIR}/${AaltoASR_PREFIX})
endif()

message("Looking for AaltoASR in ${AaltoASR_PREFIX} ") 
FIND_PATH(AaltoASR_INCLUDE_DIR NAMES HmmNetBaumWelch.hh  
PATHS ${AaltoASR_PREFIX}/include )
message("AaltoASR_INCLUDE_DIR = ${AaltoASR_INCLUDE_DIR}")
FIND_PATH(AaltoASR_INCLUDE_DIR_Lapack NAMES lapackpp.h
PATHS ${AaltoASR_PREFIX}/include
PATH_SUFFIXES lapackpp )
FIND_LIBRARY(AaltoASR_AKU_LIBRARY aku PATHS ${AaltoASR_PREFIX}/lib )
FIND_LIBRARY(AaltoASR_DECODER_LIBRARY decoder PATHS ${AaltoASR_PREFIX}/lib )
FIND_LIBRARY(AaltoASR_FSALM_LIBRARY fsalm PATHS ${AaltoASR_PREFIX}/lib )
FIND_LIBRARY(AaltoASR_MISC_LIBRARY misc PATHS ${AaltoASR_PREFIX}/lib )
FIND_LIBRARY(LAPACKPP_LIBRARY lapackpp PATHS ${AaltoASR_PREFIX}/lib )


FIND_PACKAGE_HANDLE_STANDARD_ARGS(AaltoASR
    REQUIRED_VARS AaltoASR_INCLUDE_DIR AaltoASR_INCLUDE_DIR_Lapack AaltoASR_AKU_LIBRARY AaltoASR_DECODER_LIBRARY AaltoASR_FSALM_LIBRARY AaltoASR_MISC_LIBRARY LAPACKPP_LIBRARY)

SET(AaltoASR_LIBRARIES ${AaltoASR_AKU_LIBRARY} ${AaltoASR_DECODER_LIBRARY} ${AaltoASR_FSALM_LIBRARY} ${AaltoASR_MISC_LIBRARY} ${LAPACKPP_LIBRARY} CACHE FILEPATH "AaltoASR libraries")
SET(AaltoASR_INCLUDE_DIRS ${AaltoASR_INCLUDE_DIR} ${AaltoASR_INCLUDE_DIR_Lapack} CACHE PATH "AaltoASR Include dirs")

endif()
