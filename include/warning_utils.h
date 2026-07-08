// SPDX-License-Identifier: MPL-2.0

//! \file winCMDLINE_utils.h
//! \brief Implements psuedo random number generation functionality.
//! Currently implemented using XOR shift+
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

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