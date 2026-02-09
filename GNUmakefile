# SPDX-License-Identifier: MPL-2.0
#
# Do NOT modify or remove this copyright and license
#
# Copyright (c) 2012-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
#
# This software is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# ******************************************************************************************
#
# \file GNUmakefile
# \brief Standalone GNU makefile for opensea-common library
#
# This makefile can build opensea-common independently of the main project.
# It reuses platform detection and compiler configuration from the main build system.
# Requires GNU Make - not compatible with BSD make or other make implementations.

# Directories
COMMON_DIR := $(CURDIR)
SRC_DIR := $(COMMON_DIR)/src
INCLUDE_DIR := $(COMMON_DIR)/include

# Include main build system modules (reuse existing infrastructure)
MAKE_DIR := $(COMMON_DIR)/../../Make
include $(MAKE_DIR)/config.mk

# Override BUILD_DIR for standalone builds (use local build directory)
BUILD_DIR := $(COMMON_DIR)/build
OBJ_DIR := $(BUILD_DIR)/obj
LIB_DIR := $(BUILD_DIR)/lib
include $(MAKE_DIR)/compiler-detection.mk
include $(MAKE_DIR)/compiler-flags.mk
include $(MAKE_DIR)/security-hardening.mk
include $(MAKE_DIR)/platforms/$(PLATFORM).mk

# Include source list from this subproject
include $(COMMON_DIR)/sources.mk

# Set default goal (must be after all includes)
.DEFAULT_GOAL := all

# Compiler flags
CFLAGS := $(COMMON_CFLAGS) $(WARNING_FLAGS) $(SECURITY_CFLAGS) -I$(INCLUDE_DIR) $(PLATFORM_DEFINES)

# Object files
OBJS := $(addprefix $(OBJ_DIR)/,$(COMMON_SOURCES:.c=.o))
DEPS := $(OBJS:.o=.d)

# Library target
LIBCOMMON_STATIC := $(LIB_DIR)/libopensea-common.a

# vpath for source discovery
vpath %.c $(SRC_DIR)
vpath %.c $(SRC_DIR)/posix
vpath %.c $(SRC_DIR)/windows

#===============================================================================
# Targets
#===============================================================================

.PHONY: all clean distclean help

all: $(LIBCOMMON_STATIC)

$(LIBCOMMON_STATIC): $(OBJS) | $(LIB_DIR)
	@echo "  AR      $@"
	@$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@echo "  CC      $<"
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJ_DIR) $(LIB_DIR):
	@mkdir -p $@

clean:
	@echo "Cleaning opensea-common build artifacts..."
	@rm -rf $(BUILD_DIR)

distclean: clean
	@echo "Distclean complete for opensea-common"

help:
	@echo "opensea-common standalone build targets:"
	@echo "  all       - Build libopensea-common.a (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  distclean - Remove all generated files"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  BUILD_DIR - Build output directory (default: ./build)"
	@echo "  CC        - C compiler (default: auto-detected)"
	@echo "  PLATFORM  - Target platform (default: auto-detected)"

# Include dependency files
-include $(DEPS)
