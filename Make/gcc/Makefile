#
# Do NOT modify or remove this copyright and license
#
# Copyright (c) 2012 - 2019 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
#
# This software is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# ******************************************************************************************

# Hand Written Makefile (Edit with caution) -Muhammad
# 

NAME=opensea-common
#Change the Major version when major interface changes are made. E.g. tDevice changes
MAJOR=1
#Change the Minor version when new features are added. 
MINOR=18
#Change the patch version when only bug fixes are made.
PATCH=18
VERSION=$(MAJOR).$(MINOR).$(PATCH)
SRC_DIR=../../src/
INC_DIR=-I../../include
CC ?= gcc
AR ?= ar
CFLAGS ?= -Wall -c -fPIC -I. -std=gnu99
CFLAGS += -c -fPIC -I. -std=gnu99
LFLAGS ?= -Wall
LIB_SRC_FILES = \
    $(SRC_DIR)common.c\
    $(SRC_DIR)common_platform.c\
    $(SRC_DIR)common_nix.c

PROJECT_DEFINES += #-DDISABLE_NVME_PASSTHROUGH  #-D_DEBUG

#All of the source files have associated object files
LIB_OBJ_FILES = $(LIB_SRC_FILES:.c=.o)
LIBS = lib$(NAME).a

FILE_OUTPUT_DIR=lib

.PHONY: all 

all: clean mkoutputdir $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(PROJECT_DEFINES) $(INC_DIR) $< -o $@

$(LIBS): $(LIB_OBJ_FILES)
	rm -f $(FILE_OUTPUT_DIR)/$@
	$(AR) cq $(FILE_OUTPUT_DIR)/$@ $(LIB_OBJ_FILES)
# Removing the line below since we don't need a shared object for this right now...a static library is enough for the common stuff
#	$(CC) -shared $(LIB_OBJ_FILES) -o lib$(NAME).so.$(VERSION)
clean:
	rm -f $(FILE_OUTPUT_DIR)/lib$(NAME).a $(FILE_OUTPUT_DIR)/lib$(NAME).so.$(VERSION) *.o ../../src/*.o
	rm -rf $(FILE_OUTPUT_DIR)

mkoutputdir:
	mkdir -p $(FILE_OUTPUT_DIR)

