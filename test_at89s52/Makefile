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

# ----- Toolchain setup

CC      = sdcc
AS      = sdas8051
AR      = sdar
LD      = sdcc
OBJCOPY = sdobjcopy

# ----- Naive setup

OBJ_DIR = ./obj
SRC_DIR = ./src

APP = test_at89s52

APP_OBJS = $(OBJ_DIR)/main.rel

APP_HEX = $(OBJ_DIR)/$(APP).hex
APP_BIN = $(OBJ_DIR)/$(APP).bin

# ----- Compiler flags

CFLAGS = -c -mmcs51 --model-small --verbose

# ----- Includes

INCLUDES  = -I$(SRC_DIR)

# ----- Linker flags

LFLAGS = -L$(OBJ_DIR) --out-fmt-ihx --verbose

# ------ Build rules

.SUFFIXES:
.SUFFIXES: .c .rel

$(OBJ_DIR)/%.rel : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $<

# ------ Targets

.PHONY : clean help

all : clean hex bin

$(APP_HEX) : $(APP_OBJS)
	$(LD) $(LFLAGS) -o $(APP_HEX) $(APP_OBJS)

$(APP_BIN) : $(APP_HEX)
	$(OBJCOPY) -I ihex -O binary $(APP_HEX) $(APP_BIN)

hex : $(APP_HEX)

bin : $(APP_BIN)

clean :
	rm -f $(OBJ_DIR)/*.asm
	rm -f $(OBJ_DIR)/*.lst
	rm -f $(OBJ_DIR)/*.rel
	rm -f $(OBJ_DIR)/*.rst
	rm -f $(OBJ_DIR)/*.sym
	rm -f $(OBJ_DIR)/*.bin
	rm -f $(OBJ_DIR)/*.hex
	rm -f $(OBJ_DIR)/*.lk
	rm -f $(OBJ_DIR)/*.map
	rm -f $(OBJ_DIR)/*.mem
	rm -f $(SRC_DIR)/*~
	rm -rf *~
help:
	@echo "Usage: make clean"
	@echo "       make hex"
	@echo "       make bin"
	@echo "       make all"
