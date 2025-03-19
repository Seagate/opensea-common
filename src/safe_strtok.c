// SPDX-License-Identifier: BSD-3-Clause and MPL-2.0

//! \file safe_strtok.c
//! \brief Defines safe_strtok which functions as C11 annex K's strtok_s
//! \details This code is adapted from FreeBSD's strtok.c under BSD 3-clause license.
//! Modifications are licensed under MPL 2.0
//! Modifications:
//!
//! - checks for M_NULLPTR, sizes as required for C11 annex k bsearch_s
//!
//! - set errno as needed
//!
//! - casts changed to opensea-common M_CONST_CAST
//!
//! - detection of C11 annex K error conditions
//!
//! - added strmax parameter
//!
//! - call constraint handler for errors detected per C11 Annex K
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2025-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "code_attributes.h"
#include "common_types.h"
#include "constraint_handling.h"
#include "sort_and_search.h"
#include "string_utils.h"
#include "type_conversion.h"

#include <stdio.h>

/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1998 Softweyr LLC.  All rights reserved.
 *
 * strtok_r, from Berkeley strtok
 * Oct 13, 1998 by Wes Peters <wes@softweyr.com>
 *
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notices, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notices, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SOFTWEYR LLC, THE REGENTS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL SOFTWEYR LLC, THE
 * REGENTS, OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

char* safe_strtok_impl(char* M_RESTRICT str,
                       rsize_t*         strmax,
                       const char*      delim,
                       char**           saveptr,
                       const char*      file,
                       const char*      function,
                       int              line,
                       const char*      expression)
{
    char*             spanp = M_NULLPTR;
    char*             tok   = M_NULLPTR;
    int               c     = 0;
    int               sc    = 0;
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (strmax == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtok: strmax = NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (delim == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtok: delim = NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return M_NULLPTR;
    }
    else if (saveptr == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtok: saveptr = NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    if (str != M_NULLPTR)
    {
        // Initial call of the function.
        *saveptr = str;
        *strmax  = safe_strnlen(str, RSIZE_MAX);
        if (*strmax == RSIZE_T_C(0))
        {
            *saveptr = M_NULLPTR;
            error    = ERANGE;
            invoke_Constraint_Handler("safe_strtok: *strmax = 0 on initial call",
                                      set_Env_Info(&envInfo, file, function, expression, line), error);
            errno = error;
            return M_NULLPTR;
        }
        else if (*strmax >= RSIZE_MAX &&
                 str[*strmax + RSIZE_T_C(1)] != '\0') // This is to determine if the string is really
                                                      // RSIZE_MAX or not. safe_strnlen will return
                                                      // RSIZE_MAX if NULL is found at that size OR if it
                                                      // is not found in the string. So adding 1 to check
                                                      // if it found NULL terminator or not to make this
                                                      // check match annex k
        {
            *saveptr = M_NULLPTR;
            error    = ERANGE;
            invoke_Constraint_Handler("safe_strtok: *strmax > RSIZE_MAX on initial call",
                                      set_Env_Info(&envInfo, file, function, expression, line), error);
            errno = error;
            return M_NULLPTR;
        }
        // add 1 to include null terminator in the total size to check and stay within these bounds
        *strmax += RSIZE_T_C(1);
    }
    else
    {
        // non-initial call of the function
        if (*saveptr == M_NULLPTR)
        {
            *saveptr = M_NULLPTR;
            if (*strmax != RSIZE_T_C(0))
            {
                error = EINVAL;
                invoke_Constraint_Handler("safe_strtok: *saveptr = NULL on non-initial call",
                                          set_Env_Info(&envInfo, file, function, expression, line), error);
                errno = error;
            }
            return M_NULLPTR;
        }
    }

    // Seagate note: Not sure this is necessary with other checks above.
    // Leaving in place as-is though
    if (str == M_NULLPTR && (str = *saveptr) == M_NULLPTR)
    {
        *strmax = RSIZE_T_C(0); // Seagate modification
        return (M_NULLPTR);
    }

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
    //Seagate modification:
    //This code previously used a goto.
    //In order to stay within bounds of str, this was removed to check strmax > 0 each time through.
    while (*strmax > RSIZE_T_C(0))
    {
        bool breakdelimloop = false;
        c = *str++;
        *strmax -= RSIZE_T_C(1); // Seagate modification
        for (spanp = M_CONST_CAST(char*, delim); (sc = *spanp++) != 0 && !breakdelimloop;)
        {
            if (c == sc)
            {
                breakdelimloop = true;
                break;
            }
        }
        if (breakdelimloop)
        {
            continue;
        }
        else
        {
            break;
        }
    }
    if (*strmax == 0 && c != 0)
    {
        // Seagate modification
        // If this is reached then end of the string was reached without a null terminator then this is an error
        // Finding null terminator should happen in the loop above, so this check exists in case the string is modified
        // while being tokenized.
        *saveptr = M_NULLPTR;
        error    = ERANGE;
        invoke_Constraint_Handler("safe_strtok: reached end of source "
                                "string without encountering null terminator while scanning for first non-deliminator",
                                set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return (M_NULLPTR);
    }

    if (c == 0)
    { /* no non-delimiter characters */
        *saveptr = M_NULLPTR;
        *strmax  = RSIZE_T_C(0);
        return (M_NULLPTR);
    }
    tok = str - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    // Original source loop: for(;;)
    // Seagate modification: Added staying within bounds of memory with strmax > 0
    for (; *strmax > RSIZE_T_C(0);)
    {
        c = *str++;
        *strmax -= RSIZE_T_C(1); // Seagate modification
        spanp = M_CONST_CAST(char*, delim);
        do
        {
            if ((sc = *spanp++) == c)
            {
                if (c == 0)
                {
                    str = M_NULLPTR;
                }
                else
                {
                    str[-1] = '\0';
                }
                *saveptr = str;
                return (tok);
            }
        } while (sc != 0);
    }
    // Seagate modification
    // If this is reached then end of the string was reached without a null terminator then this is an error
    // Finding null terminator should happen in the loop above, so this check exists in case the string is modified
    // while being tokenized.
    *saveptr = M_NULLPTR;
    error    = ERANGE;
    invoke_Constraint_Handler("safe_strtok: reached end of source "
                              "string without encountering null terminator",
                              set_Env_Info(&envInfo, file, function, expression, line), error);
    errno = error;
    RESTORE_NONNULL_COMPARE
    return M_NULLPTR;
}
