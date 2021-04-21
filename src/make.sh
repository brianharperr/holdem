#!/bin/sh
gcc -c -fpic cards.c game.c mtwister.c evaluator.c pots.c
gcc -shared -o libtex.so *.o
rm *.o
