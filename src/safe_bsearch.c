// SPDX-License-Identifier: BSD-3-Clause and MPL-2.0

//! \file safe_bsearch.c
//! \brief Defines safe_bsearch_context which behaves similarly to bsearch_s with
//! a context parameter.
//! \details This code is adapted from FreeBSD's bsearch.c under BSD 3-clause license.
//! Modifications are licensed under MPL 2.0
//! Modifications:
//!
//! - checks for null, sizes as required for C11 annex k bsearch_s
//!
//! - set errno as needed
//!
//! - change to comparison function with context parameter
//!
//! - casts changed to M_CONST_CAST to resolve warnings
//!
//! - variable scope changed to within for loop for lim and cmp
//!
//! - call constraint handler for errors detected per C11 Annex K
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "code_attributes.h"
#include "common_types.h"
#include "constraint_handling.h"
#include "sort_and_search.h"
#include "type_conversion.h"

/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Perform a binary search.
 *
 * The code below is a bit sneaky.  After a comparison fails, we
 * divide the work in half by moving either left or right. If lim
 * is odd, moving left simply involves halving lim: e.g., when lim
 * is 5 we look at item 2, so we change lim to 2 so that we will
 * look at items 0 & 1.  If lim is even, the same applies.  If lim
 * is odd, moving right again involves halving lim, this time moving
 * the base up one item past p: e.g., when lim is 5 we change base
 * to item 3 and make lim 2 so that we will look at items 3 and 4.
 * If lim is even, however, we have to shrink it by one before
 * halving: e.g., when lim is 4, we still looked at item 2, so we
 * have to make lim 3, then halve, obtaining 1, so that we will only
 * look at item 3.
 */

void* safe_bsearch_context_impl(const void*  key,
                                const void*  ptr,
                                rsize_t      count,
                                rsize_t      size,
                                ctxcomparefn compare,
                                void*        context,
                                const char*  file,
                                const char*  function,
                                int          line,
                                const char*  expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (count > RSIZE_T_C(0) && ptr == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_bsearch_context: count > 0 && ptr == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (count > RSIZE_T_C(0) && compare == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_bsearch_context: count > 0 && compare == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (count > RSIZE_T_C(0) && key == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_bsearch_context: count > 0 && key == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (count > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_bsearch_context: count > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (size > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_bsearch_context: size > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else
    {
        const char* base = ptr;
        const void* p    = M_NULLPTR;
        errno            = 0;

        for (size_t lim = count; lim != SIZE_T_C(0); lim >>= 1)
        {
            p       = base + (lim >> 1) * size;
            int cmp = compare(key, p, context);
            if (cmp == 0)
            {
                return M_CONST_CAST(void*, p);
            }
            if (cmp > 0)
            {
                /* key > p: move right */
                base = C_CAST(const char*, p) + size;
                lim--;
            } /* else move left */
        }
    }
    RESTORE_NONNULL_COMPARE
    return M_NULLPTR;
}
