#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

# capture coverage info
lcov --capture --directory ${TRAVIS_BUILD_DIR} --output-file coverage.info

# filter out system and test code
lcov --remove coverage.info '/usr/*' 'install-gtest/*' --output-file coverage.info

# debug before upload
lcov --list coverage.info

# uploads to coveralls
coveralls-lcov --repo-token bHIO8TqTbmtNoLarLh7cvHxC3U0z0RWKF coverage.info
