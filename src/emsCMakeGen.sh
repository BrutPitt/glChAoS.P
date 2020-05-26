#!/bin/bash

if [ -z "$EMSCRIPTEN" ] && [ "$#" -lt 1 ]; then
    echo ""
    echo "Please set EMSCRIPTEN environment var, or pass location via command line"
    echo "    for example: $0 /emsdk/emscripten/1.38.8"
    echo "             or: $0 /emsdk/emscripten/1.38.8 [Debug|RelWithDebInfo|Release|MinSizeRel]"
    echo "                  \"MinSizeRel\" is default build option"
    echo "                  \"emsOut\" is default directory"
    echo ""
else    
    if [ "$#" -gt 1 ]; then
        EMSCRIPTEN=$1
    fi        

    if [ "$#" -eq 2 ]; then
        BUILD_TYPE=$2
    else
        BUILD_TYPE='MinSizeRel'
    fi


    EMS_DIR=$RAMDISK"builds/ems"$BUILD_TYPE

    cmake -DCMAKE_TOOLCHAIN_FILE:STRING=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE:STRING=$BUILD_TYPE -DBUILD_wglChAoSp:BOOL=TRUE -G "Ninja" -B$EMS_DIR

    cd $EMS_DIR
    ninja

fi

