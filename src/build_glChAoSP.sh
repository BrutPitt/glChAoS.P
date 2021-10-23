#!/bin/bash

# To build glChAoS.P in Linux/MacOS/Windows (with MinGW) uses follow command:
#
# cmake -DBuildTarget:String=<BuildVer> -G <MakeTool> -B<FolderToBuild>
#   where:
#       <BuildVer> must be one of follow strings:
#           OpenGL_45
#           OpenGL_41
#           OpenGL_41_LowRes
#           OpenGL_ES
#           OpenGL_ES_LowRes
#       <MakeTool> is your preferred generator like "Unix Makefiles" or "ninja"
#       <FolderToBuild> is the folder where will be generated Makefile, move in it and run your generator
#       - Default build is "Release" but it can be changed via CMAKE_BUILD_TYPE definition:
#           command line: -DCMAKE_BUILD_TYPE:STRING=<Debug|Release|MinSizeRel|RelWithDebInfo>
#           cmake-gui: from combo associated to CMAKE_BUILD_TYPE var
#       - Executable will be generated directly in glChAoS.P root folder
#
# for Windows users, Visual Studio "solution" is also provided (msBuilds folder)
#
#
# To build wlChAoS.P with EMSCRIPTEN uses follow command:
# cmake cmake -DCMAKE_TOOLCHAIN_FILE:STRING=<EMSDK_PATH>/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DBuildTarget:String=<BuildVer> -G "Unix Makefiles"|"ninja" -B<FolderToBuild>
#   where:
#       <EMSDK_PATH> is where was installed EMSCRIPTEN: you need to have it in EMSDK environment variable
#       <BuildVer> must be one of follow strings:
#           WebGL
#           WebGL_LowRes
#       <MakeTool> is your preferred generator like "Unix Makefiles" or "ninja"
#           Windows users need to use MinGW-make utility (by EMSCRIPTEN specification): ninja or othe can not work.
#       <FolderToBuild> is the folder where will be generated Makefile, move in it and run your generator
#       - Default build is "MinSizeRel" but it can be changed via CMAKE_BUILD_TYPE definition:
#           command line: -DCMAKE_BUILD_TYPE:STRING=<Debug|Release|MinSizeRel|RelWithDebInfo>
#           cmake-gui: from combo associated to CMAKE_BUILD_TYPE var
#       - HTML and WASM files will be generated in "<glChAoSP_root>/emsout/<CMAKE_BUILD_TYPE>" folder
#
# More information, necessary dependencies, required packages are described: https://github.com/BrutPitt/glChAoS.P


echo ""
echo "    glChAoS.p build starts . . ."
echo ""
echo "    ---  CMake stage  ---"
echo ""

if test "$RAMDISK"; then      
    BUILD_DIR=$RAMDISK/builds/cmakeRelease
else
    BUILD_DIR=./build
fi

cmake -DBuildTarget:String=OpenGL_45 -G Unix\ Makefiles -DCMAKE_BUILD_TYPE:String=Release -B$BUILD_DIR
#-DOpenGL_GL_PREFERENCE="GLVND"
#-DCMAKE_EXE_LINKER_FLAGS="-static"
echo ""
echo "    ---  make stage  ---"
echo ""
echo ""

cd $BUILD_DIR
make -j 8
cd ..

echo ""
echo "    . . . end glChAoS.P"
echo ""
echo ""
