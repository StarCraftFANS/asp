# /***********************************************************************
# *                                                                      *
# * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
# *                                                                      *
# * This program is free software; you can redistribute it and/or modify *
# * it under the terms of the GNU General Public License as published by *
# * the Free Software Foundation; either version 2 of the License, or    *
# * (at your option) any later version.                                  *
# *                                                                      *
# ************************************************************************/

# ----- Check that environment is defined 

ifndef SDCC_HOME
$(error SDCC_HOME not set)
endif

ifndef GPUTILS_HOME
$(error GPUTILS_HOME not set)
endif

# ----- Target setup

PIC_PROC = 18f2510
PIC_TYPE = pic16

# ----- Toolchain setup

CC      = sdcc
AS      = gpasm
LD      = gplink
DAS     = gpdasm

# ----- Naive setup

OBJ_DIR = ./obj
SRC_DIR = ./src

APP = pic_asp

APP_ASMS = $(OBJ_DIR)/main.asm \
           $(OBJ_DIR)/asp.asm \
           $(OBJ_DIR)/asp_ui.asm \
           $(OBJ_DIR)/at89s52_io.asm \
           $(OBJ_DIR)/at89s52_isp.asm \
           $(OBJ_DIR)/delay.asm \
           $(OBJ_DIR)/rs232.asm \
           $(OBJ_DIR)/user_io.asm \
           $(OBJ_DIR)/utility.asm \
           $(OBJ_DIR)/xmodem.asm

APP_OBJS = $(OBJ_DIR)/main.o \
           $(OBJ_DIR)/asp.o \
           $(OBJ_DIR)/asp_ui.o \
           $(OBJ_DIR)/at89s52_io.o \
           $(OBJ_DIR)/at89s52_isp.o \
           $(OBJ_DIR)/delay.o \
           $(OBJ_DIR)/rs232.o \
           $(OBJ_DIR)/user_io.o \
           $(OBJ_DIR)/utility.o \
           $(OBJ_DIR)/xmodem.o

APP_HEX  = $(OBJ_DIR)/$(APP).hex
APP_DASM = $(OBJ_DIR)/$(APP).dasm

# ----- Compiler flags

CFLAGS = -S -m$(PIC_TYPE) -p$(PIC_PROC) --use-non-free

# ----- Assembler flags

AFLAGS = -c -q

# ----- Includes

INCLUDES  = -I$(SRC_DIR)

# ----- Linker flags

LFLAGS = -w -m -c

# ------ Build rules

.SUFFIXES:
.SUFFIXES: .c .asm .o

$(OBJ_DIR)/%.asm : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/%.o : $(OBJ_DIR)/%.asm
	$(AS) $(AFLAGS) -o $@ $<

# ------ Targets

.PHONY : clean help

all : clean hex dasm

asm : $(APP_ASMS)

obj : asm $(APP_OBJS)

## A note on C runtime modules:
## crt0.o    Reset vector code and finally calls main()
## crt0i.o   crt0.o  + Initializes variables
## crt0iz.o  crt0i.o + Clear all RAM memory (before initialize)

hex : obj
	$(LD) $(LFLAGS) -o $(APP_HEX) \
	-s $(GPUTILS_HOME)/share/gputils/lkr/$(PIC_PROC)_g.lkr \
	$(APP_OBJS) \
	$(SDCC_HOME)/share/sdcc/lib/$(PIC_TYPE)/crt0i.o \
	$(SDCC_HOME)/share/sdcc/lib/$(PIC_TYPE)/libsdcc.lib \
	$(SDCC_HOME)/share/sdcc/non-free/lib/$(PIC_TYPE)/libdev$(PIC_PROC).lib

dasm : hex
	$(DAS) -p $(PIC_PROC) $(APP_HEX) > $(APP_DASM)

clean :
	rm -f $(OBJ_DIR)/*.asm
	rm -f $(OBJ_DIR)/*.lst
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OBJ_DIR)/*.cod
	rm -f $(OBJ_DIR)/*.cof
	rm -f $(OBJ_DIR)/*.map
	rm -f $(OBJ_DIR)/*.hex
	rm -f $(OBJ_DIR)/*.dasm
	rm -f $(SRC_DIR)/*~
	rm -rf *~
help:
	@echo "Usage: make clean"
	@echo "       make asm"
	@echo "       make obj"
	@echo "       make hex"
	@echo "       make dasm"
	@echo "       make all"
