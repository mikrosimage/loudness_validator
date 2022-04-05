# - Find ADMEngine library
# Find the native ADMEngine includes and library.
# 
# This module defines:
# ADM_ENGINE_FOUND
# ADM_ENGINE_LIBRARIES - The ADMEngine libraries.
#

find_package(adm)
find_package(ear)

find_path(BW64_INCLUDE_DIR bw64/bw64.hpp)
if(BW64_INCLUDE_DIR)
	message("-- Found bw64 headers: ${BW64_INCLUDE_DIR}")
endif()

find_library(ADM_ENGINE_LIBRARY admengine)
find_path(ADM_ENGINE_INCLUDE_DIR adm_engine/utils.hpp)

# Get ADMEngine from custom install
if(adm_FOUND AND ear_FOUND AND BW64_INCLUDE_DIR AND ADM_ENGINE_LIBRARY AND ADM_ENGINE_INCLUDE_DIR)
	set(ADM_ENGINE_LIBRARIES ${ADM_ENGINE_LIBRARY} adm ear)
	message("-- Found ADMEngine: " ${ADM_ENGINE_LIBRARY})
	set(ADM_ENGINE_FOUND TRUE)
else()
	set(ADM_ENGINE_FOUND FALSE)
endif()
