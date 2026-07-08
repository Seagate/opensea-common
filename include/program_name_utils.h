// SPDX-License-Identifier: MPL-2.0

//! \file wingetopt_utils.h
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