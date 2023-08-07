#!/bin/bash

# For magics
# g++ -Wall -std=c++11 -c find_magic.cpp FindMagic.cpp BBoard.cpp
# g++ -std=c++11 -o find_magic.bin find_magic.o FindMagic.o BBoard.o
# ./find_magic.bin > Magic.h

if [ "$1" = "clean" ]; then
    rm -r build
fi

mkdir -p build
pushd build
    cmake ..
    make -j4
popd
