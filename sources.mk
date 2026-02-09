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
# \file sources.mk
# \brief opensea-common source file list
#
# This file defines all source files needed to build libopensea-common.
# It consumes the PLATFORM variable from the parent makefile to select
# platform-specific sources.

#===============================================================================
# Common Sources (all platforms)
#===============================================================================

COMMON_SOURCES := \
    bit_manip.c \
    constraint_handling.c \
    env_detect.c \
    error_translation.c \
    io_utils.c \
    math_utils.c \
    memory_safety.c \
    pattern_utils.c \
    precision_timer.c \
    prng.c \
    safe_bsearch.c \
    safe_lsearch.c \
    safe_qsort.c \
    safe_strtok.c \
    secure_file.c \
    secured_env_vars.c \
    sleep.c \
    sort_and_search.c \
    string_utils.c \
    time_utils.c \
    type_conversion.c \
    unit_conversion.c \
    validate_format.c \
    version_sort.c

#===============================================================================
# Platform-Specific Common Sources
#===============================================================================

ifeq ($(PLATFORM),windows)
    # Windows-specific sources
    COMMON_SOURCES += \
        windows_env_detect.c \
        windows_secure_file.c \
        windows_version_detect.c

else
    # All POSIX platforms (Linux, BSD, Solaris, AIX, VMware, HP-UX)
    COMMON_SOURCES += \
        posix_env_detect.c \
        posix_secure_file.c
endif
