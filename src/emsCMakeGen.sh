#!/bin/bash

if [ "$#" -lt 3 ]; then
    echo ""
    echo "Please use $0 /emsdk/emscripten/1.38.8 Debug|RelWithDebInfo|Release|MinSizeRel wglChAoSP|wglChAoSP_lowres"
    echo ""
else    
        EMSCRIPTEN=$1
        BUILD_TYPE=$2
        PROG_TYPE=$3


    EMS_DIR=$RAMDISK"builds/ems"$BUILD_TYPE

    cmake -DCMAKE_TOOLCHAIN_FILE:STRING=$EMSCRIPTEN/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_BUILD_TYPE:STRING=$BUILD_TYPE -DBUILD_$PROG_TYPE:BOOL=TRUE -G "Ninja" -B$EMS_DIR

    cd $EMS_DIR
    ninja

fi

