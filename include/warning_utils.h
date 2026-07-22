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
//! This header provides portable command-line warning utilities adapted from BSD's err.h warnx() pattern.
//! Adaptations:
//!
//! - generic command-line error messaging (not tied to any specific option parser)
//!
//! - program name retrieval abstracted to get_progname() for cross-platform support
//!   (__progname on Linux/BSD, getexecname() on Solaris, __argv[0] on Windows)
//!
//! - error message types use generic CMDLINE_ERR_MSG prefix for broader application
//!
//! - parameter annotations from code_attributes.h for enhanced type safety and documentation

#pragma once

#include "common_types.h"
#include "io_utils.h"
#include "program_name_utils.h"
#include "string_utils.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    /*
     * Due to Warning about non-const format string, using an enum since this code uses
     * it's own internal implementation of warnings/errors to select a constant string
     * This warning only happens in Clang, but rather than disabling an otherwise useful
     * warning, this enum can map to the correct warning/error output for us.
     */
    M_DECLARE_ENUM(eCMDLineErrorMessage,
                   CMDLINE_ERR_MSG_RECARGCHAR   = 0,
                   CMDLINE_ERR_MSG_RECARGSTRING = 1,
                   CMDLINE_ERR_MSG_AMBIG        = 2,
                   CMDLINE_ERR_MSG_NOARG        = 3,
                   CMDLINE_ERR_MSG_ILLOPTCHAR   = 4,
                   CMDLINE_ERR_MSG_ILLOPTSTRING = 5);

    /*
     * some systems have warnx, _vwarnx from err.h, but not all have this.
     * defining our own versions here to help prevent them from coliding. -TJE
     */
    M_INLINE static void cmdline_vwarnx(eCMDLineErrorMessage errmsg, va_list ap)
    {
        char* progname       = get_progname();
        int   vfprintfresult = 0;
#if defined(HAVE_C11_ANNEX_K) || defined(HAVE_MSFT_SECURE_LIB)
        /*Use MSFT's/C11 Annex K's _s functions for fprintf and vfprintf*/
        if (progname != M_NULLPTR)
        {
            vfprintfresult = fprintf_s(stderr, "%s: ", progname);
            if (vfprintfresult < 0)
            {
                // TODO - should we handle the error condintion. If not, the clang-tidy is giving suggestion to fix the
                // code.
            }
        }

        switch (errmsg)
        {
        case CMDLINE_ERR_MSG_RECARGCHAR:
            vfprintfresult = vfprintf_s(stderr, "option requires an argument -- %c", ap);
            break;
        case CMDLINE_ERR_MSG_RECARGSTRING:
            vfprintfresult = vfprintf_s(stderr, "option requires an argument -- %s", ap);
            break;
        case CMDLINE_ERR_MSG_AMBIG:
            vfprintfresult = vfprintf_s(stderr, "ambiguous option -- %.*s", ap);
            break;
        case CMDLINE_ERR_MSG_NOARG:
            vfprintfresult = vfprintf_s(stderr, "option doesn't take an argument -- %.*s", ap);
            break;
        case CMDLINE_ERR_MSG_ILLOPTCHAR:
            vfprintfresult = vfprintf_s(stderr, "unknown option -- %c", ap);
            break;
        case CMDLINE_ERR_MSG_ILLOPTSTRING:
            vfprintfresult = vfprintf_s(stderr, "unknown option -- %s", ap);
            break;
        }
        if (vfprintfresult < 0)
        {
            // TODO - should we handle the error condintion. If not, the clang-tidy is giving suggestion to fix the
            // code.
            // call abort?
        }
        vfprintfresult = fprintf_s(stderr, "\n");
        if (vfprintfresult < 0)
        {
            // TODO - should we handle the error condintion. If not, the clang-tidy is giving suggestion to fix the
            // code.
        }
#else
    if (progname != M_NULLPTR)
    {
        vfprintfresult = fprintf(stderr, "%s: ", progname);
        if (vfprintfresult < 0)
        {
            // TODO - should we handle the error condintion. If not, the clang-tidy is giving suggestion to fix the
            // code.
        }
    }
    switch (errmsg)
    {
    case CMDLINE_ERR_MSG_RECARGCHAR:
        vfprintfresult = vfprintf(stderr, "option requires an argument -- %c", ap);
        break;
    case CMDLINE_ERR_MSG_RECARGSTRING:
        vfprintfresult = vfprintf(stderr, "option requires an argument -- %s", ap);
        break;
    case CMDLINE_ERR_MSG_AMBIG:
        vfprintfresult = vfprintf(stderr, "ambiguous option -- %.*s", ap);
        break;
    case CMDLINE_ERR_MSG_NOARG:
        vfprintfresult = vfprintf(stderr, "option doesn't take an argument -- %.*s", ap);
        break;
    case CMDLINE_ERR_MSG_ILLOPTCHAR:
        vfprintfresult = vfprintf(stderr, "unknown option -- %c", ap);
        break;
    case CMDLINE_ERR_MSG_ILLOPTSTRING:
        vfprintfresult = vfprintf(stderr, "unknown option -- %s", ap);
        break;
    }
    if (vfprintfresult < 0)
    {
        // TODO - should we handle the error condintion. If not, the clang-tidy is giving suggestion to fix the
        // code.
    }
    vfprintfresult = fprintf(stderr, "\n");
    if (vfprintfresult < 0)
    {
        // TODO - should we handle the error condintion. If not, the clang-tidy is giving suggestion to fix the
        // code.
    }
#endif
        if (progname != M_NULLPTR)
        {
            free(progname);
            progname = NULL;
        }
    }

    M_INLINE static void cmdline_warnx(eCMDLineErrorMessage errmsg, ...)
    {
        va_list ap;
        va_start(ap, errmsg);
        cmdline_vwarnx(errmsg, ap);
        va_end(ap);
    }

#if defined(__cplusplus)
}
#endif