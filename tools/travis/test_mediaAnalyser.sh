#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$FFMPEG_INSTALL/lib:$AVTRANSCODER_INSTALL/lib

# Run the help of the application
$PLOUD_INSTALL/bin/media-loudness-analyser --help
