#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# Run tests
./build/release/test/loudness-analyser
