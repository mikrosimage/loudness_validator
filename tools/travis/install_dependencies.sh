#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# Install additional dependencies in case of osx environment
if [ ${TRAVIS_OS_NAME} = "osx" ]; then
    brew update
    brew install scons lcov libsndfile qt gcc@7
fi

# Install GTest framework
if [[ ! -d "${GTEST_INSTALL}/*" ]]; then
    if [ "$TRAVIS_OS_NAME" == "linux" ] && [ "$TRAVIS_COMPILER" == "gcc" ]; then
        export CC=/usr/bin/gcc-7
        export CXX=/usr/bin/g++-7
    fi
    git clone https://github.com/google/googletest.git
    cd googletest
    cmake . -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL}
    make -k
    make install
fi

# Install avtranscoder
if [[ ! -d "${AVTRANSCODER_INSTALL}/*" ]]; then
    export ARCHIVE_FILE=avtranscoder-${TRAVIS_OS_NAME}-gcc-ffmpeg-${FFMPEG_VERSION}.tgz
    wget https://github.com/avTranscoder/avTranscoder/releases/download/v${AVTRANSCODER_VERSION}/${ARCHIVE_FILE}
    tar xzf ${ARCHIVE_FILE} -C ${TRAVIS_BUILD_DIR}
    rm ${ARCHIVE_FILE}
fi
