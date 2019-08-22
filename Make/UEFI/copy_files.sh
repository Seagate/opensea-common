#!/bin/bash

function usage {
    echo "This script will copy all files required to build opensea-common to the edk2/UDK path specified."
    echo "How to use: copy_files.sh <path to edk2 directory>"
}

#check that we were given the correct number of parameters
if [ "$#" -lt 1 ]; then
    usage
fi
scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
openseaCommonBaseDir=$(dirname "$(dirname "$scriptDir")")

#check that the directory exists
if [ -d "$1" ]; then
    mkdir -p "$1/opensea-libs/opensea-common"
    opensealibsDir="$1/opensea-libs"
    openseacommonDir="$opensealibsDir/opensea-common"
    #start by copying the files to build the lib
    cp -r "$openseaCommonBaseDir/include" "$openseacommonDir"
    cp -r "$openseaCommonBaseDir/src" "$openseacommonDir"
    #copy UEFI makefiles
    cp -r "$openseaCommonBaseDir/Make/UEFI/opensea-common"* "$openseacommonDir"
else
    echo "Cannot find specified path: $1"
fi
