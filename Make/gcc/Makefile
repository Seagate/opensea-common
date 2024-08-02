# SPDX-License-Identifier: MPL-2.0
#
# Do NOT modify or remove this copyright and license
#
# Copyright (c) 2012 - 2022 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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
MAJOR=4
#Change the Minor version when new features are added. 
MINOR=0
#Change the patch version when only bug fixes are made.
PATCH=0
VERSION=$(MAJOR).$(MINOR).$(PATCH)
SRC_DIR=../../src/
INC_DIR=-I../../include
CC ?= gcc
AR ?= ar
CFLAGS ?= -Wall -Wextra -c -fPIC -I.
CFLAGS += -c -fPIC -I.
LFLAGS ?= -Wall
LIB_SRC_FILES = \
    $(SRC_DIR)bit_manip.c\
    $(SRC_DIR)env_detect.c\
    $(SRC_DIR)error_translation.c\
	$(SRC_DIR)io_utils.c\
	$(SRC_DIR)math_utils.c\
	$(SRC_DIR)memory_safety.c\
	$(SRC_DIR)pattern_utils.c\
	$(SRC_DIR)posix_env_detect.c\
	$(SRC_DIR)posix_secure_file.c\
	$(SRC_DIR)precision_timer.c\
	$(SRC_DIR)prng.c\
	$(SRC_DIR)secure_file.c\
	$(SRC_DIR)secured_env_vars.c\
	$(SRC_DIR)sleep.c\
	$(SRC_DIR)string_utils.c\
	$(SRC_DIR)time_utils.c\
	$(SRC_DIR)type_conversion.c\
	$(SRC_DIR)unit_conversion.c\
	$(SRC_DIR)validate_format.c

#NOTE -Wsign-conversion can be useful but warns way too much by default. Only enable it if debugging specific problems
COMPILER_VERSION := $(shell $(CC) --version)
ifneq (,$(findstring clang,$(COMPILER_VERSION)))
	#setup clang specific warnings/flags (centos 7's old version supports -Wno-unknown-warning-option so enabling all of them)
	CFLAGS += -Wno-unknown-warning-option -Wcast-align=strict -Wvla -Wfloat-equal -Wnull-dereference -Wunused-const-variable \
	-Wduplicated-cond -Wjump-misses-init -Wstringop-overflow -Wlogical-op -Wshift-overflow=2 -Wdouble-promotion -Wformat-security \
	-Wold-style-definition -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes
else 
	ifneq (,$(findstring GCC,$(COMPILER_VERSION)))
		#setup gcc specific warnings/flags
		GCC_VERSION_STRING = $(shell $(CC) -dumpversion)
		GCC_VER = $(subst ., ,$(GCC_VERSION_STRING))
		GCC_MAJOR = $(word 1,$(GCC_VER))
		GCC_MINOR = $(word 2,$(GCC_VER))
		GCC_SUBMINOR = $(word 3,$(GCC_VER))
		ifeq ($(GCC_MINOR),)
			GCC_MINOR = 0
		endif 
		ifeq ($(GCC_SUBMINOR),)
			GCC_SUBMINOR = 0
		endif
		#version 8.5 and higher
		ifeq ($(shell test $(GCC_MAJOR) -gt 7; echo $$?),0)
			ifeq ($(shell test $(GCC_MINOR) -gt 4; echo $$?),0)
				CFLAGS += -Wcast-align=strict
			else 
				CFLAGS += -Wcast-align
			endif
		else
			CFLAGS += -Wcast-align
		endif
		#version 7.5 and higher
		ifeq ($(shell test $(GCC_MAJOR) -gt 6; echo $$?),0)
			ifeq ($(shell test $(GCC_MINOR) -gt 4; echo $$?),0)
				CFLAGS += -Wshadow=compatible-local -Wstringop-overflow
			else 
				CFLAGS +=
			endif
		else
			CFLAGS +=
		endif
		#version 6.5 and higher
		ifeq ($(shell test $(GCC_MAJOR) -gt 5; echo $$?),0)
			ifeq ($(shell test $(GCC_MINOR) -gt 4; echo $$?),0)
				CFLAGS += -Wnull-dereference -Wunused-const-variable -Wduplicated-cond -Wshift-overflow=2
			else 
				CFLAGS +=
			endif
		else
			CFLAGS +=
		endif
		#version 5.5 and higher
		ifeq ($(shell test $(GCC_MAJOR) -gt 4; echo $$?),0)
			ifeq ($(shell test $(GCC_MINOR) -gt 4; echo $$?),0)
				CFLAGS += -Wlogical-not-parentheses
			endif
		else
			#GCC less that v 5.x.x needs to set gnu99 standard
			#as of 5.x.x, gnu11 is default
			CFLAGS += -std=gnu99
		endif
		
		CFLAGS += -Wvla -Wfloat-equal -Wjump-misses-init -Wlogical-op -Wdouble-promotion -Wformat-security\
			-Wold-style-definition -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes
	else
		#CFLAGS += -std=gnu99
		CFLAGS += -Wvla -Wfloat-equal -Wjump-misses-init -Wlogical-op -Wdouble-promotion -Wformat-security\
				-Wold-style-definition -Wstrict-prototypes -Wmissing-declarations -Wmissing-prototypes
	endif
endif
    
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

