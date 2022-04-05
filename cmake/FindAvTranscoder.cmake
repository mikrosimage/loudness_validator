# - Find AvTranscoder library
# Find the native AvTranscoder includes and library.
# 
# This module defines:
# AVTRANSCODER_FOUND
# AVTRANSCODER_LIBRARIES - The AvTranscoder libraries.
#

find_library(AVTRANSCODER_LIBRARY avtranscoder)
find_path(AVTRANSCODER_INCLUDE_DIR AvTranscoder/common.hpp)

# Get AvTranscoder from custom install
if(AVTRANSCODER_LIBRARY AND AVTRANSCODER_INCLUDE_DIR)
    set(AVTRANSCODER_LIBRARIES "${AVTRANSCODER_LIBRARY}")
    message(STATUS "Found AvTranscoder: " ${AVTRANSCODER_LIBRARY})
    set(AVTRANSCODER_FOUND TRUE)
else()
    set(AVTRANSCODER_FOUND FALSE)
endif()
