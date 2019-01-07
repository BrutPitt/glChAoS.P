#!/bin/bash

# buildLinuxGLFW.sh
# buildLinuxParticles.sh

#!/bin/bash
#export CC=/usr/bin/gcc-5
#export CXX=/usr/bin/g++-5

echo ""
echo "    start to build GLFW Lib..."
echo ""
echo ""

sh buildGLFW.sh OSX

echo ""
echo "    ... end GLFW"
echo ""
echo ""

echo ""
echo "    start to build glChAoSP..."
echo ""
echo ""

sh build_glChAoSP.sh OSX

echo ""
echo "    ... end glChAoSP"
echo ""
echo ""
