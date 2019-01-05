#!/bin/bash
if [ "$#" -lt 1 ]; then
        echo ""
        echo "Please type: $0 [Debug|RelWithDebInfo|Release|MinSizeRel]"
        echo ""
        exit
fi

EMS_DIR="cmake-ems-"$1

if [ ! -d "$EMS_DIR" ] || [ ! -f "$EMS_DIR/Makefile" ]; then
    echo ""
    echo "Have you generate Makefile?"
    echo "    please run: ./emsCMakeGen.sh"
    echo ""
else
    cd $EMS_DIR
    make
fi

