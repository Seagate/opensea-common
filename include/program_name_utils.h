/* SPDX-License-Identifier: BSD-2-Clause and MPL-2.0 */
/* Modifications Copyright 2022 Seagate Technology and/or its Affiliates */

/*	$OpenBSD: getopt.h,v 1.3 2013/11/22 21:32:49 millert Exp $	*/
/*	$NetBSD: getopt.h,v 1.4 2000/07/07 10:43:54 ad Exp $	*/

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Dieter Baron and Thomas Klausner.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * DISCLAIMER
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the w64 mingw-runtime package.
 *
 * The w64 mingw-runtime package and its code is distributed in the hope that it
 * will be useful but WITHOUT ANY WARRANTY.  ALL WARRANTIES, EXPRESSED OR
 * IMPLIED ARE HEREBY DISCLAIMED.  This includes but is not limited to
 * warranties of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
//! Adaptations are licensed under MPL 2.0
//! This header provides portable program name retrieval for cross-platform applications.
//! Adaptations:
//!
//! - abstracts platform-specific program name sources into portable get_progname() function
//!   (__progname on Linux/BSD, getexecname() on Solaris, __argv[0] on Windows)
//!
//! - platform detection preprocessor logic for transparent selection of best available method
//!
//! - fallback global storage (NEED_PROGNAME case) for systems without standard program name support
//!
//! - parameter annotations and code_attributes.h integration for enhanced type safety and documentation

#pragma once

#include "common_types.h"
#include "io_utils.h"
#include "string_utils.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * expand this long list of definitions for systems that DO have __progname
 * create an elif defined list for systems that have something similar, but named differently or other functions
 * check UEFI first since it's cross compiled from Win or Lin which will contain other preprocessor flags
 * Try relying on what is provided by these systems instead of using our own global whenever necessary
 */
#if defined(HAS_GETPROGNAME) || defined(UEFI_C_SOURCE) || defined(__APPLE__)
/*
 * this case has the function getprogname available to use
 * note: I have found references that getprogname exists in solaris 11+, but using the getexecname instead for all
 * solaris versions at this time-TJE The BSDs also have this function. If necessary, add version checks to the previous
 * case to fall into here. I'm fairly certain those are not necessary at this point as the references I have point to
 * only needing this check for very old versions-TJE
 */
#    if !defined(HAS_GETPROGNAME)
#        define HAS_GETPROGNAME
#    endif /*HAS_GETPROGNAME*/
#elif defined(HAS_PROGNAME) || defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) ||                   \
    defined(__NetBSD__) || defined(__DragonFly__) || defined(__QNX__) || defined(__CYGWIN__)
/*This case has the __progname available to use*/
#    if defined(__CYGWIN__)
extern char __declspec(dllimport) * __progname; // NOLINT
#    else
extern const char* __progname; // NOLINT
#    endif
#    if !defined(HAS_PROGNAME)
#        define HAS_PROGNAME
#    endif /*HAS_PROGNAME*/
#elif defined(HAS_ARGV0) || defined(_WIN32) && !defined(__CYGWIN__)
#    if !defined(HAS_ARGV0)
#        define HAS_ARGV0
#    endif
#elif defined(HAS_GETEXECNAME) || defined(__sun)
#    if !defined(HAS_GETEXECNAME)
#        define HAS_GETEXECNAME
#    endif /*HAS_GETEXECNAME*/
#else
#    define NEED_PROGNAME
/*This will define our own global to store the programe name into -TJE*/
#endif /*Checking PROGNAME capabilities*/

#if defined(NEED_PROGNAME)
    char* getopt_progname = M_NULLPTR;
#endif /*NEED_PROGNAME*/

    M_NODISCARD M_FUNC_ATTR_MALLOC static M_INLINE char* get_progname(void)
    {
        char*       programName = M_NULLPTR;
        const char* progname    = M_NULLPTR;
#if defined(HAS_PROGNAME)
        progname = __progname;
#elif defined(HAS_ARGV0)
    /*Win32 most likely*/
#    if defined(_MSC_VER)
    progname = __argv[0];
#    else  /*mingw???*/
    progname = __argv[0];
#    endif /*_MSC_VER*/
#elif defined(HAS_GETPROGNAME)
    progname = getprogname();
#elif defined(HAS_GETEXECNAME)
    progname = getexecname();
    if (progname != M_NULLPTR)
    {
        char* execfullname = M_NULLPTR;
        if (0 != safe_strdup(&execfullname, progname))
        {
            // TODO - print error message
            return M_NULLPTR;
        }
        char* execname = M_NULLPTR;
        if (0 !=
            safe_strdup(&execname, basename(execfullname))) /* basename can return internal pointers, modified memory,
                                                        may get changed, so dup it to return this just in case -TJE */
        {
            free(execfullname);
            return M_NULLPTR;
        }
        free(execfullname);
        return execname;
    }
#elif defined(NEED_PROGNAME)
    /* own global declared that can be accessed -TJE */
    progname = getopt_progname;
#else /*This is the "we don't know how to get this" case. */
#endif
        if (progname == M_NULLPTR)
        {
#if defined(_DEBUG)
            if (0 != safe_strdup(&programName, "Unknown progname"))
            {
                // TODO - print error message
                return M_NULLPTR;
            }
#else
        if (0 != safe_strdup(&programName, ""))
        {
            // TODO - print error message
            return M_NULLPTR;
        }
#endif
        }
        else
        {
            if (0 != safe_strdup(&programName, progname))
            {
                // TODO - print error message
                return M_NULLPTR;
            }
        }

        return programName;
    }

#if defined(__cplusplus)
}
#endif