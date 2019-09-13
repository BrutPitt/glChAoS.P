#!/bin/bash

echo ""
echo "    start to build Particles..."
echo ""
echo ""

cmake -DBUILD_wglChAoSP:BOOL=FALSE -G Unix\ Makefiles -DCMAKE_BUILD_TYPE:String=Release
#-DOpenGL_GL_PREFERENCE="GLVND"
#-DCMAKE_EXE_LINKER_FLAGS="-static"
make -j 8

echo ""
echo "    ... end Particles"
echo ""
echo ""
