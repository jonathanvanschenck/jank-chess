#!/bin/bash

# For magics
# g++ -Wall -std=c++11 -c find_magic.cpp FindMagic.cpp BBoard.cpp
# g++ -std=c++11 -o find_magic.bin find_magic.o FindMagic.o BBoard.o
# ./find_magic.bin > Magic.h

g++ -Wall -std=c++11 -c Game.cpp BBoard.cpp Board.cpp Attack.cpp FindMagic.cpp Magic.cpp Zobrist.cpp Move.cpp Transposition.cpp
g++ -Wformat=0 -std=c++11 -c main.cpp 
g++ -std=c++11 -o main.bin main.o Game.o Board.o BBoard.o Attack.o FindMagic.o Magic.o Move.o Zobrist.o Transposition.o
./main.bin

# g++ -Wall -std=c++11 -c BBoard.cpp Board.cpp Attack.cpp FindMagic.cpp Magic.cpp Move.cpp Zobrist.cpp Transposition.cpp
# g++ -Wformat=0 -std=c++11 -c perft.cpp 
# g++ -Wall -std=c++11 -o perft.bin perft.o Board.o BBoard.o Attack.o FindMagic.o Magic.o Move.o Zobrist.o Transposition.o
# ./perft.bin
