# Get useful variables
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
