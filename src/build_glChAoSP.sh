#!/bin/bash

echo ""
echo "    glChAoS.p build starts . . ."
echo ""
echo "    ---  CMake stage  ---"
echo ""

BUILD_DIR=cmake-build-Release
cmake -DBUILD_wglChAoSP:BOOL=FALSE -G Unix\ Makefiles -DCMAKE_BUILD_TYPE:String=Release -B$BUILD_DIR
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
