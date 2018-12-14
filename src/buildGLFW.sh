#!/bin/bash

if [ "${1}" == "OSX" ]; then
    BUILD_TYPE=buildOSX
else
    BUILD_TYPE=buildLinux
fi


echo ""
echo "    start to build GLFW Lib..."
echo ""
echo ""

cd src/libs/glfw

echo ""
echo "          build GLFW static Lib..."
echo ""
echo ""

cmake -G Unix\ Makefiles -DCMAKE_INSTALL_PREFIX:String=$BUILD_TYPE -DGLFW_BUILD_EXAMPLES:Bool=OFF -DGLFW_BUILD_TESTS:Bool=OFF  -DGLFW_BUILD_DOCS:Bool=OFF -DBUILD_SHARED_LIBS:Bool=OFF -DCMAKE_BUILD_TYPE:String=Release
make install

echo ""
echo "          build GLFW shared Lib..."
echo ""
echo ""

cmake -G Unix\ Makefiles -DCMAKE_INSTALL_PREFIX:String=$BUILD_TYPE -DGLFW_BUILD_EXAMPLES:Bool=OFF -DGLFW_BUILD_TESTS:Bool=OFF  -DGLFW_BUILD_DOCS:Bool=OFF -DBUILD_SHARED_LIBS:Bool=ON -DCMAKE_BUILD_TYPE:String=Release
make install

cd ../../..

echo ""
echo "    ... end GLFW"
echo ""
echo ""

