# - Find libsndfile
# Find the native libsndfile library.
#
# This module defines:
# SNDFILE_FOUND
# SNDFILE_LIBRARIES - The libsndfile libraries.
#

find_library(SNDFILE sndfile)
if(NOT ${SNDFILE} MATCHES "SNDFILE-NOTFOUND")
    set(SNDFILE_LIBRARIES ${SNDFILE})
    message(STATUS "Found libsndfile: ${SNDFILE_LIBRARIES}")
    set(SNDFILE_FOUND TRUE)
else()
    set(SNDFILE_FOUND FALSE)
endif()
