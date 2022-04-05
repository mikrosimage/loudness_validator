cmake_minimum_required(VERSION 3.5)
project(loudness-validator)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

## Project version
set(LOUDNESS_VALIDATOR_VERSION_MAJOR 1)
set(LOUDNESS_VALIDATOR_VERSION_MINOR 4)
set(LOUDNESS_VALIDATOR_VERSION_MICRO 3)

set(LOUDNESS_VALIDATOR_VERSION ${LOUDNESS_VALIDATOR_VERSION_MAJOR}.${LOUDNESS_VALIDATOR_VERSION_MINOR}.${LOUDNESS_VALIDATOR_VERSION_MICRO})

## Project dependencies
find_package(Boost 1.67.0 REQUIRED COMPONENTS filesystem)
find_library(SndFile sndfile)
if(DEFINED SndFile)
    message("-- Found sndfile library: ${SndFile}")
endif()

find_package(Qt4 4.0.0 COMPONENTS QtGui QtCore)

find_package(FFmpeg COMPONENTS avcodec avformat avutil swscale swresample avresample)
find_package(AvTranscoder)
find_package(ADMEngine)

## Project variables
set(LOUDNESS_VALIDATOR_SRC_PATH "${PROJECT_SOURCE_DIR}/src")

set(LOUDNESS_ADM_ANALYSER_SRC_PATH "${LOUDNESS_VALIDATOR_SRC_PATH}/admLoudnessAnalyser")
set(LOUDNESS_ANALYSER_SRC_PATH "${LOUDNESS_VALIDATOR_SRC_PATH}/loudnessAnalyser")
set(LOUDNESS_COMMON_SRC_PATH "${LOUDNESS_VALIDATOR_SRC_PATH}/loudnessCommon")
set(LOUDNESS_CORRECTOR_SRC_PATH "${LOUDNESS_VALIDATOR_SRC_PATH}/loudnessCorrector")
set(LOUDNESS_IO_SRC_PATH "${LOUDNESS_VALIDATOR_SRC_PATH}/loudnessIO")
set(LOUDNESS_TOOLS_SRC_PATH "${LOUDNESS_VALIDATOR_SRC_PATH}/loudnessTools")

file(GLOB_RECURSE LOUDNESS_VALIDATOR_SRC_FILES
        "${LOUDNESS_ANALYSER_SRC_PATH}/*.cpp" "${LOUDNESS_ANALYSER_SRC_PATH}/*.hpp"
        "${LOUDNESS_COMMON_SRC_PATH}/*.cpp" "${LOUDNESS_COMMON_SRC_PATH}/*.hpp"
        "${LOUDNESS_CORRECTOR_SRC_PATH}/*.cpp" "${LOUDNESS_CORRECTOR_SRC_PATH}/*.hpp"
        "${LOUDNESS_TOOLS_SRC_PATH}/*.cpp" "${LOUDNESS_TOOLS_SRC_PATH}/*.hpp"
        )

if(DEFINED SndFile)
    file(GLOB_RECURSE LOUDNESS_IO_SRC_FILES "${LOUDNESS_IO_SRC_PATH}/*.cpp" "${LOUDNESS_IO_SRC_PATH}/*.hpp")
endif()

if(ADM_ENGINE_FOUND)
    file(GLOB_RECURSE LOUDNESS_ADM_ANALYSER_SRC_FILES "${LOUDNESS_ADM_ANALYSER_SRC_PATH}/*.cpp" "${LOUDNESS_ADM_ANALYSER_SRC_PATH}/*.hpp")
endif()

## Project library
add_library(loudness-validator SHARED ${LOUDNESS_VALIDATOR_SRC_FILES} ${LOUDNESS_IO_SRC_FILES} ${LOUDNESS_ADM_ANALYSER_SRC_FILES})
target_include_directories(loudness-validator PUBLIC ${LOUDNESS_VALIDATOR_SRC_PATH} ${ADM_ENGINE_INCLUDE_DIR})

if(ADM_ENGINE_FOUND)
    target_link_libraries(loudness-validator PUBLIC ${ADM_ENGINE_LIBRARIES})
    target_include_directories(loudness-validator PUBLIC ${BW64_INCLUDE_DIR})
endif()

## Project executables
if(DEFINED SndFile)
    ## loudness-analyser
    file(GLOB_RECURSE LOUDNESS_ANALYSER_SRC_FILES "app/analyser/*.cpp")
    add_executable(loudness-analyser ${LOUDNESS_ANALYSER_SRC_FILES})
    target_link_libraries(loudness-analyser loudness-validator ${SndFile})
    target_include_directories(loudness-analyser PUBLIC ${LOUDNESS_VALIDATOR_SRC_PATH})

    ## loudness-corrector
    file(GLOB_RECURSE LOUDNESS_CORRECTOR_SRC_FILES "app/corrector/*.cpp")
    add_executable(loudness-corrector ${LOUDNESS_CORRECTOR_SRC_FILES})
    target_link_libraries(loudness-corrector loudness-validator ${SndFile})
    target_include_directories(loudness-corrector PUBLIC ${LOUDNESS_VALIDATOR_SRC_PATH})

    if(${Qt4_FOUND})
        ## loudness-validator-gui
        file(GLOB_RECURSE LOUDNESS_VALIDATOR_GUI_SRC_FILES "app/LoudnessValidator/*.cpp" "app/LoudnessValidator/*.hpp" "app/LoudnessValidator/*.h")
        add_executable(loudness-validator-gui ${LOUDNESS_VALIDATOR_GUI_SRC_FILES})
        target_link_libraries(loudness-validator-gui loudness-validator ${SndFile} Qt4::QtCore Qt4::QtGui)
        target_include_directories(loudness-validator-gui PUBLIC ${LOUDNESS_VALIDATOR_SRC_PATH})
    endif()
endif()

if(${FFMPEG_FOUND} AND ${AVTRANSCODER_FOUND})
    ## media-analyser
    file(GLOB_RECURSE LOUDNESS_MEDIA_ANALYSER_SRC_FILES "app/mediaAnalyser/*.cpp" "app/mediaAnalyser/*.hpp")
    add_executable(media-analyser ${LOUDNESS_MEDIA_ANALYSER_SRC_FILES})
    target_link_libraries(media-analyser loudness-validator ${FFMPEG_LIBRARIES} ${AVTRANSCODER_LIBRARY} ${SndFile})
    target_include_directories(media-analyser PUBLIC ${LOUDNESS_VALIDATOR_SRC_PATH} ${AVTRANSCODER_INCLUDE_DIR})
endif()

if(${ADM_ENGINE_FOUND})
    ## adm-loudness-analyser
    file(GLOB_RECURSE ADM_LOUDNESS_ANALYSER_SRC_FILES "app/AdmLoudnessAnalyser/*.cpp" "app/AdmLoudnessAnalyser/*.hpp")
    add_executable(adm-loudness-analyser ${ADM_LOUDNESS_ANALYSER_SRC_FILES})
    target_link_libraries(adm-loudness-analyser loudness-validator ${ADM_ENGINE_LIBRARIES} ${SndFile})
    target_include_directories(adm-loudness-analyser PUBLIC ${LOUDNESS_VALIDATOR_SRC_PATH})
endif()

## Unit tests
find_package(GTest)
if(${GTest_FOUND} AND DEFINED SndFile)
    enable_testing()
    add_executable(loudness-validator_tests test/loudness-analyser.cpp)
    target_link_libraries(loudness-validator_tests loudness-validator ${GTEST_LIBRARIES} pthread ${SndFile})
    target_include_directories(loudness-validator_tests PUBLIC ${LOUDNESS_VALIDATOR_SRC_PATH} ${GTEST_INCLUDE_DIRS})
    add_test(NAME loudness-validator-tests COMMAND loudness-validator_tests)
else()
    message("Test dependencies not found!")
endif()
