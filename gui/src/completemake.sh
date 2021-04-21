#!/bin/sh
cd ../../src
gcc -c -fpic cards.c game.c mtwister.c evaluator.c pots.c
gcc -shared -o libtex.so *.o
rm *.o
cd ../gui/src
g++ -L../../src gui.cpp `wx-config --cxxflags --libs` -o main -ltex
