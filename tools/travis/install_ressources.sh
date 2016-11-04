#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# Install test ressources
if [[ ! -d "${EBU_PATH}/*" ]]; then
    export EBU_SET=ebu-loudness-test-setv04.zip
    wget https://tech.ebu.ch/files/live/sites/tech/files/shared/testmaterial/$EBU_SET --no-check-certificate
    unzip -o $EBU_SET -d $EBU_PATH
    rm $EBU_SET
fi
