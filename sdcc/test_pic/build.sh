#!/bin/bash

# Setup environment
GPUTILS_HOME=/opt/gputils
SDCC_HOME=/opt/sdcc

PATH=$GPUTILS_HOME/bin:$SDCC_HOME/bin:$PATH
export PATH

# Clean build
rm -rf *~
rm -rf *.asm
rm -rf *.lst
rm -rf *.o
rm -rf *.hex
rm -rf *.map
rm -rf *.cod
rm -rf *.cof

# To turn a .c file into a .asm file:
# output : test.asm
echo "---- COMPILER"
sdcc -S -V \
    -mpic14 -p16f628a -D__16f628a --use-non-free \
    test.c

# To turn a .asm file into a .o file:
# output : test.o, test.lst
echo "---- ASSEMBLER"
gpasm -c test.asm

# To link multiple .o files into one .hex file:
# output : test.hex, test.map, test.cod
echo "---- LINKER"
gplink -w -m -c \
    -o test.hex \
    -s $GPUTILS_HOME/share/gputils/lkr/16f628a_g.lkr \
    $SDCC_HOME/share/sdcc/non-free/lib/pic14/pic16f628a.lib \
    $SDCC_HOME/share/sdcc/lib/pic14/libsdcc.lib \
    test.o

