// SPDX-License-Identifier: BSD-3-Clause and MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file safe_qsort.c
// \brief Defines safe_qsort_context which behaves similarly to qsort_s with a context parameter.
//        This code is adapted from FreeBSD's qsort.c under BSD 3-clause license
//        Modifications are licensed under MPL 2.0

//Modifications: instead of ifdefs around how this is implemented, this is a standalone implementation of safe_qsort
//               setting order of compare func to match C11
//               Variables renamed to match standard (as needed)
//               Compare function order matches standard
//               return errno_t instead of void
//               Any other modifications are from security recommendations(initializing variables, variables declared on separate lines, etc)
//               Change casts to C_CAST macro
//               Added goto exit if error occurs during recursion
//               uintptr/intptr casts to resolve sign conversion warnings as needed

#include "sort_and_search.h"
#include "code_attributes.h"
#include "common_types.h"
#include "type_conversion.h"
#include "math_utils.h"

/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1992, 1993
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
 * Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".
 */

static M_INLINE void swapfunc(char* a, char* b, size_t es)
{
    char t = 0;
    do
    {
        t = *a;
        *a++ = *b;
        *b++ = t;
    } while (--es > 0);
}

#define	vecswap(a, b, n) \
	if ((n) > 0) swapfunc(a, b, n)

static M_INLINE char* med3(char* a, char* b, char* c, ctxcomparefn cmp, void* thunk)
{
    return cmp(a, b, thunk) < 0 ?
        (cmp(b, c, thunk) < 0 ? b : (cmp(a, c, thunk) < 0 ? c : a))
        : (cmp(b, c, thunk) > 0 ? b : (cmp(a, c, thunk) < 0 ? a : c));
}

errno_t safe_qsort_context(void* ptr, rsize_t count, rsize_t size, ctxcomparefn compare, void* context)
{
    if (count > RSIZE_MAX || size > RSIZE_MAX)
    {
        errno = ERANGE;
        return ERANGE;
    }
    else if (count > 0 && (ptr == M_NULLPTR || compare == M_NULLPTR))
    {
        errno = EINVAL;
        return EINVAL;
    }
    else
    {
        errno_t error = 0;
        errno = 0;
        if (count > 0)
        {
            char* pa = M_NULLPTR;
            char* pb = M_NULLPTR;
            char* pc = M_NULLPTR;
            char* pd = M_NULLPTR;
            char* pl = M_NULLPTR;
            char* pm = M_NULLPTR;
            char* pn = M_NULLPTR;
            size_t d1 = 0;
            size_t d2 = 0;
            int cmp_result = 0;
            int swap_cnt = 0;

            /* if there are less than 2 elements, then sorting is not needed */
            if (count < 2) //This had __predict_false which is not available here in cross-platform code. TODO: predictfalse attribute type macro
            {
                errno = error;
                return error;
            }
        loop:
            swap_cnt = 0;
            if (count < 7)
            {
                for (pm = C_CAST(char*, ptr) + size; pm < C_CAST(char*, ptr) + count * size; pm += size)
                {
                    for (pl = pm;
                        pl > C_CAST(char*, ptr) && compare(pl - size, pl, context) > 0;
                        pl -= size)
                    {
                        swapfunc(pl, pl - size, size);
                    }
                }
                errno = error;
                return error;
            }
            pm = C_CAST(char*, ptr) + (count / 2) * size;
            if (count > 7)
            {
                pl = ptr;
                pn = C_CAST(char*, ptr) + (count - 1) * size;
                if (count > 40)
                {
                    size_t d = (count / 8) * size;

                    pl = med3(pl, pl + d, pl + 2 * d, compare, context);
                    pm = med3(pm - d, pm, pm + d, compare, context);
                    pn = med3(pn - 2 * d, pn - d, pn, compare, context);
                }
                pm = med3(pl, pm, pn, compare, context);
            }
            swapfunc(ptr, pm, size);
            pa = pb = C_CAST(char*, ptr) + size;

            pc = pd = C_CAST(char*, ptr) + (count - 1) * size;
            for (;;)
            {
                while (pb <= pc && (cmp_result = compare(pb, ptr, context)) <= 0)
                {
                    if (cmp_result == 0)
                    {
                        swap_cnt = 1;
                        swapfunc(pa, pb, size);
                        pa += size;
                    }
                    pb += size;
                }
                while (pb <= pc && (cmp_result = compare(pc, ptr, context)) >= 0)
                {
                    if (cmp_result == 0)
                    {
                        swap_cnt = 1;
                        swapfunc(pc, pd, size);
                        pd -= size;
                    }
                    pc -= size;
                }
                if (pb > pc)
                {
                    break;
                }
                swapfunc(pb, pc, size);
                swap_cnt = 1;
                pb += size;
                pc -= size;
            }
            if (swap_cnt == 0)
            {
                /* Switch to insertion sort */
                for (pm = C_CAST(char*, ptr) + size; pm < C_CAST(char*, ptr) + count * size; pm += size)
                {
                    for (pl = pm;
                        pl > C_CAST(char*, ptr) && compare(pl - size, pl, context) > 0;
                        pl -= size)
                    {
                        swapfunc(pl, pl - size, size);
                    }
                }
                errno = error;
                return error;
            }

            pn = C_CAST(char*, ptr) + count * size;
            d1 = M_Min(C_CAST(uintptr_t, pa) - C_CAST(uintptr_t, ptr), C_CAST(uintptr_t, pb) - C_CAST(uintptr_t, pa));
            vecswap(ptr, pb - d1, d1);
            /*
             * Cast size to preserve signedness of right-hand side of MIN()
             * expression, to avoid sign ambiguity in the implied comparison.  size
             * is safely within [0, SSIZE_MAX].
             */
            //Seagate note: Added casts to ensure both sides of M_Min are treated as signed to prevent sign-comparison warning, then cast result to size_t for assignment to prevent conversion warning.
            //              The comment above is from the original source and seems reasonable to support adding these casts-TJE
            d1 = C_CAST(size_t, M_Min(C_CAST(intptr_t, pd) - C_CAST(intptr_t, pc), C_CAST(intptr_t, pn) - C_CAST(intptr_t, pd) - C_CAST(ssize_t, size)));
            vecswap(pb, pn - d1, d1);

            d1 = C_CAST(uintptr_t, pb) - C_CAST(uintptr_t, pa);
            d2 = C_CAST(uintptr_t, pd) - C_CAST(uintptr_t, pc);
            if (d1 <= d2)
            {
                /* Recurse on left partition, then iterate on right partition */
                if (d1 > size)
                {
                    error = safe_qsort_context(ptr, d1 / size, size, compare, context);
                    if (error != 0)
                    {
                        goto ret_error;
                    }
                }
                if (d2 > size)
                {
                    /* Iterate rather than recurse to save stack space */
                    /* qsort(pn - d2, d2 / size, size, cmp); */
                    ptr = pn - d2;
                    count = d2 / size;
                    goto loop;
                }
            }
            else
            {
                /* Recurse on right partition, then iterate on left partition */
                if (d2 > size)
                {
                    error = safe_qsort_context(pn - d2, d2 / size, size, compare, context);
                    if (error != 0)
                    {
                        goto ret_error;
                    }
                }
                if (d1 > size)
                {
                    /* Iterate rather than recurse to save stack space */
                    /* qsort(ptr, d1 / size, size, cmp); */
                    count = d1 / size;
                    goto loop;
                }
            }
        }
ret_error:
        errno = error;
        return error;
    }
}
