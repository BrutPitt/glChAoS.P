#!/bin/bash

echo ""
echo "    start to build Particles..."
echo ""
echo ""

cmake -G Unix\ Makefiles -DCMAKE_BUILD_TYPE:String=Release
#-DOpenGL_GL_PREFERENCE="GLVND"
#-DCMAKE_EXE_LINKER_FLAGS="-static"
make

echo ""
echo "    ... end Particles"
echo ""
echo ""
