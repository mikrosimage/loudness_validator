cmake_minimum_required(VERSION 3.5)
project(loudness-validator)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(LoudnessValidatorMacros)

## Project version
set(LOUDNESS_VALIDATOR_VERSION_MAJOR 1)
set(LOUDNESS_VALIDATOR_VERSION_MINOR 4)
set(LOUDNESS_VALIDATOR_VERSION_MICRO 5)

set(LOUDNESS_VALIDATOR_VERSION ${LOUDNESS_VALIDATOR_VERSION_MAJOR}.${LOUDNESS_VALIDATOR_VERSION_MINOR}.${LOUDNESS_VALIDATOR_VERSION_MICRO})

## Project dependencies
find_package(Boost 1.67.0 REQUIRED COMPONENTS filesystem)
find_package(SndFile)
find_package(FFmpeg COMPONENTS avcodec avformat avutil swscale swresample avresample)
find_package(AvTranscoder)
find_package(ADMEngine)
find_package(Qt4 4.0.0 COMPONENTS QtGui QtCore)

# CPP flags on debug / release mode
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Ox")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fPIC -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -fPIC -O3")
endif()

## Project library
set(LOUDNESS_VALIDATOR_SOURCE_FILES ${LOUDNESS_VALIDATOR_SRC_FILES})
set(LOUDNESS_VALIDATOR_INCLUDE_DIRS ${LOUDNESS_VALIDATOR_SRC_PATH} ${Boost_INCLUDE_DIRS})

if(SNDFILE_FOUND)
    file(GLOB_RECURSE LOUDNESS_IO_SRC_FILES "${LOUDNESS_IO_SRC_PATH}/*.cpp" "${LOUDNESS_IO_SRC_PATH}/*.hpp")
    set(LOUDNESS_VALIDATOR_SOURCE_FILES ${LOUDNESS_VALIDATOR_SOURCE_FILES} ${LOUDNESS_IO_SRC_FILES})
endif()

if(ADM_ENGINE_FOUND)
    file(GLOB_RECURSE LOUDNESS_ADM_ANALYSER_SRC_FILES "${LOUDNESS_ADM_ANALYSER_SRC_PATH}/*.cpp" "${LOUDNESS_ADM_ANALYSER_SRC_PATH}/*.hpp")
    set(LOUDNESS_VALIDATOR_SOURCE_FILES ${LOUDNESS_VALIDATOR_SOURCE_FILES} ${LOUDNESS_ADM_ANALYSER_SRC_FILES})
    set(LOUDNESS_VALIDATOR_INCLUDE_DIRS ${LOUDNESS_VALIDATOR_INCLUDE_DIRS} ${ADM_ENGINE_INCLUDE_DIR})
endif()

add_library(loudness-validator SHARED ${LOUDNESS_VALIDATOR_SOURCE_FILES})
target_link_libraries(loudness-validator PUBLIC ${Boost_LIBRARIES})
target_include_directories(loudness-validator PUBLIC ${LOUDNESS_VALIDATOR_INCLUDE_DIRS})
if(WIN32)
    set_target_properties(loudness-validator PROPERTIES OUTPUT_NAME "loudness-validator-${LOUDNESS_VALIDATOR_VERSION}")
else()
    set_target_properties(loudness-validator PROPERTIES OUTPUT_NAME loudness-validator)
endif()

add_library(loudness-validator-static STATIC ${LOUDNESS_VALIDATOR_SOURCE_FILES})
target_link_libraries(loudness-validator-static PUBLIC ${Boost_LIBRARIES})
target_include_directories(loudness-validator-static PUBLIC ${LOUDNESS_VALIDATOR_INCLUDE_DIRS})
set_target_properties(loudness-validator-static PROPERTIES OUTPUT_NAME loudness-validator)

if(ADM_ENGINE_FOUND)
    target_link_libraries(loudness-validator PUBLIC ${ADM_ENGINE_LIBRARIES})
    target_include_directories(loudness-validator PUBLIC ${BW64_INCLUDE_DIR})
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

install(TARGETS loudness-validator loudness-validator-static DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY "${LOUDNESS_VALIDATOR_SRC_PATH}"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include"
        FILES_MATCHING
        PATTERN "*.hpp")

## Project executables
if(SNDFILE_FOUND)
    ## loudness-analyser
    file(GLOB_RECURSE LOUDNESS_ANALYSER_SRC_FILES "app/analyser/*.cpp")
    add_executable(loudness-analyser ${LOUDNESS_ANALYSER_SRC_FILES})

    target_link_libraries(loudness-analyser loudness-validator-static ${SNDFILE_LIBRARIES})
    target_include_directories(loudness-analyser PUBLIC ${LOUDNESS_VALIDATOR_INCLUDE_DIRS})

    ## loudness-corrector
    file(GLOB_RECURSE LOUDNESS_CORRECTOR_SRC_FILES "app/corrector/*.cpp")
    add_executable(loudness-corrector ${LOUDNESS_CORRECTOR_SRC_FILES})

    target_link_libraries(loudness-corrector loudness-validator-static ${SNDFILE_LIBRARIES})
    target_include_directories(loudness-corrector PUBLIC ${LOUDNESS_VALIDATOR_INCLUDE_DIRS})

    install(TARGETS loudness-analyser DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(TARGETS loudness-corrector DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

    if(Qt4_FOUND)
        ## loudness-validator-gui
        file(GLOB_RECURSE LOUDNESS_VALIDATOR_GUI_SRC_FILES "app/LoudnessValidator/*.cpp" "app/LoudnessValidator/*.hpp" "app/LoudnessValidator/*.h")
        add_executable(loudness-validator-gui ${LOUDNESS_VALIDATOR_GUI_SRC_FILES})

        target_link_libraries(loudness-validator-gui loudness-validator-static ${SNDFILE_LIBRARIES} Qt4::QtCore Qt4::QtGui)
        target_include_directories(loudness-validator-gui PUBLIC ${LOUDNESS_VALIDATOR_INCLUDE_DIRS})

        install(TARGETS loudness-validator-gui DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    endif()

endif()

if(FFMPEG_FOUND AND AVTRANSCODER_FOUND)
    ## media-analyser
    file(GLOB_RECURSE LOUDNESS_MEDIA_ANALYSER_SRC_FILES "app/mediaAnalyser/*.cpp" "app/mediaAnalyser/*.hpp")
    add_executable(media-analyser ${LOUDNESS_MEDIA_ANALYSER_SRC_FILES})

    target_link_libraries(media-analyser loudness-validator-static ${FFMPEG_LIBRARIES} ${AVTRANSCODER_LIBRARY})
    target_include_directories(media-analyser PUBLIC ${LOUDNESS_VALIDATOR_INCLUDE_DIRS} ${AVTRANSCODER_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIR})

    if(SNDFILE_FOUND)
        target_link_libraries(media-analyser ${SNDFILE_LIBRARIES})
    endif()

    install(TARGETS media-analyser DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

if(ADM_ENGINE_FOUND)
    ## adm-loudness-analyser
    file(GLOB_RECURSE ADM_LOUDNESS_ANALYSER_SRC_FILES "app/AdmLoudnessAnalyser/*.cpp" "app/AdmLoudnessAnalyser/*.hpp")
    add_executable(adm-loudness-analyser ${ADM_LOUDNESS_ANALYSER_SRC_FILES})

    target_link_libraries(adm-loudness-analyser loudness-validator-static ${ADM_ENGINE_LIBRARIES})
    target_include_directories(adm-loudness-analyser PUBLIC ${LOUDNESS_VALIDATOR_INCLUDE_DIRS})

    if(SNDFILE_FOUND)
        target_link_libraries(adm-loudness-analyser ${SNDFILE_LIBRARIES})
    endif()

    install(TARGETS adm-loudness-analyser DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()

## Unit tests
find_package(GTest)
find_path(EBU_TEST_DIRECTORY NAMES seq-3341-1-16bit.wav HINTS ${EBU_TEST_ESSENCES})
if(GTest_FOUND AND SNDFILE_FOUND AND EBU_TEST_DIRECTORY)
    message(STATUS "Found EBU test material directory: ${EBU_TEST_DIRECTORY}")
    enable_testing()
    add_executable(loudness-validator_tests test/loudness-analyser.cpp)
    target_link_libraries(loudness-validator_tests loudness-validator ${GTEST_LIBRARIES} pthread ${SNDFILE_LIBRARIES})
    target_include_directories(loudness-validator_tests PUBLIC ${LOUDNESS_VALIDATOR_SRC_PATH} ${GTEST_INCLUDE_DIRS})
    target_compile_options(loudness-validator_tests PUBLIC -DEBU_TEST_ESSENCES=${EBU_TEST_DIRECTORY})
    add_test(NAME loudness-validator-tests COMMAND loudness-validator_tests)
elseif(NOT GTest_FOUND)
    message(STATUS "GTest not found!")
elseif(NOT SNDFILE_FOUND)
    message(STATUS "Test dependencies not found!")
elseif(NOT EBU_TEST_DIRECTORY)
    message(WARNING "Test material not found! Set the `EBU_TEST_ESSENCES` variable to indicate the corresponding directory.")
endif()
