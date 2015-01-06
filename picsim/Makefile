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

ARCH ?= x86_64

TC_PREFIX ?=

AR  = $(TC_PREFIX)ar
CC  = $(TC_PREFIX)gcc
CPP = $(TC_PREFIX)g++
AS  = $(TC_PREFIX)gcc
LD  = $(TC_PREFIX)gcc

# ----- Naive setup

OPTIMIZE = -O0 -g3
KIND = dbg

OBJ_DIR = ./obj
SRC_DIR = ./src

APP_OBJS = $(OBJ_DIR)/rs232.o \
           $(OBJ_DIR)/delay.o \
           $(OBJ_DIR)/user_io.o \
           $(OBJ_DIR)/utility.o \
           $(OBJ_DIR)/xmodem.o \
           $(OBJ_DIR)/at89s52_io.o \
           $(OBJ_DIR)/at89s52_isp.o \
           $(OBJ_DIR)/asp_ui.o \
           $(OBJ_DIR)/asp_hw.o \
           $(OBJ_DIR)/asp.o \
           $(OBJ_DIR)/picsim_main.o

APP_NAME = $(OBJ_DIR)/picsim_$(KIND).$(ARCH)

# ----- Compiler flags

CFLAGS = -Wall -Wextra -Werror -Dlinux -Wno-packed-bitfield-compat
CFLAGS += $(OPTIMIZE)
CFLAGS += $(DEBUG_PRINTS)

COMP_FLAGS = $(CFLAGS) -c
COMP_FLAGS_C   = $(COMP_FLAGS) -std=gnu99
COMP_FLAGS_CPP = $(COMP_FLAGS) -std=c++98 -pedantic -Wno-long-long -Wno-variadic-macros

# ----- Includes

APP_INCLUDE  = -I$(SRC_DIR)

INCLUDE = $(APP_INCLUDE)

# ----- Linker paths

# ----- Linker libraries

LIBSX = -lstdc++ -lgcc -lpthread -lrt

LIBS =  $(LIBSX)

# ------ Build rules

.SUFFIXES:
.SUFFIXES: .c .cpp .o .h .d

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(COMP_FLAGS_C) $(INCLUDE) -o $@ $<

$(OBJ_DIR)/%.d : $(SRC_DIR)/%.c
	@$(CC) -MM -MT '$(patsubst %d,%o,$@)' $(COMP_FLAGS_C) $(INCLUDE) $< > $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(CPP) $(COMP_FLAGS_CPP) $(INCLUDE) -o $@ $<

$(OBJ_DIR)/%.d : $(SRC_DIR)/%.cpp
	@$(CPP) -MM -MT '$(patsubst %d,%o,$@)' $(COMP_FLAGS_CPP) $(INCLUDE) $< > $@

# ------ Targets

.PHONY : clean help

-include $(APP_OBJS:.o=.d)

all : clean app

app : $(APP_OBJS)
	$(CC) -o $(APP_NAME) $(APP_OBJS) $(LIB_DIRS) $(LIBS)

clean :
	rm -f $(APP_OBJS)
	rm -f $(APP_OBJS:.o=.d)
	rm -f $(OBJ_DIR)/*.$(ARCH)
	rm -f $(SRC_DIR)/*~
	rm -rf *~

help:
	@echo "Usage: make clean"
	@echo "       make app"
	@echo "       make all"
