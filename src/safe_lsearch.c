// SPDX-License-Identifier: LicenseRef-RobertDrehmel-2002 and MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024 Seagate Technology LLC and/or its Affiliates, All Rights
// Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file safe_lsearch.c
// \brief Defines safe_bsearch_context which behaves similarly to bsearch_s with
// a context parameter.
//        This code is adapted from FreeBSD's lsearch.c under Robert Drehmel's
//        original license Modifications are licensed under MPL 2.0

// Modifications:
//              checks for null, sizes as required for C11 annex k bounds
//              checking functions set errno as needed added version with
//              comparison function with context parameter casts changed to
//              M_CONST_CAST to resolve warnings functions are named safe_ to
//              imply the bounds checking capabilities.

#include "common_types.h"
#include "memory_safety.h"
#include "sort_and_search.h"
#include "type_conversion.h"

/*
 * Initial implementation:
 * Copyright (c) 2002 Robert Drehmel
 * All rights reserved.
 *
 * As long as the above copyright statement and this notice remain
 * unchanged, you can do what ever you want with this file.
 */

static void* safe_lwork(const void*, const void*, size_t*, size_t, comparefn, int);

void* safe_lsearch(const void* key, void* base, size_t* nelp, size_t width, comparefn compar)
{
    return (safe_lwork(key, base, nelp, width, compar, 1));
}

void* safe_lfind(const void* key, const void* base, size_t* nelp, size_t width, comparefn compar)
{
    return (safe_lwork(key, base, nelp, width, compar, 0));
}

static void* safe_lwork(const void* key, const void* base, size_t* nelp, size_t width, comparefn compar, int addelem)
{
    if (nelp == M_NULLPTR || (*nelp > 0 && (key == M_NULLPTR || base == M_NULLPTR || compar == M_NULLPTR)))
    {
        errno = EINVAL;
        return M_NULLPTR;
    }
    else if (*nelp > RSIZE_MAX || width > RSIZE_MAX)
    {
        errno = ERANGE;
        return M_NULLPTR;
    }
    else
    {
        uint8_t* ep   = M_CONST_CAST(uint8_t*, base);
        uint8_t* endp = M_NULLPTR;
        for (endp = M_REINTERPRET_CAST(uint8_t*, ep + width * (*nelp)); ep < endp; ep += width)
        {
            if (compar(key, ep) == 0)
            {
                return (ep);
            }
        }

        /* lfind() shall return when the key was not found. */
        if (!addelem)
        {
            return (M_NULLPTR);
        }
        /*
         * lsearch() adds the key to the end of the table and increments
         * the number of elements.
         */
        safe_memcpy(endp, width, key, width);
        ++*nelp;

        return (endp);
    }
}

// following versions are written by Seagate technology to allow compare
// function to provide context The modification is simply to provide a context
// pointer to the functions to pass to the context function

static void* safe_lwork_context(const void*, const void*, size_t*, size_t, ctxcomparefn, void*, int);

void* safe_lsearch_context(const void* key, void* base, size_t* nelp, size_t width, ctxcomparefn compar, void* context)
{
    return (safe_lwork_context(key, base, nelp, width, compar, context, 1));
}

void* safe_lfind_context(const void*  key,
                         const void*  base,
                         size_t*      nelp,
                         size_t       width,
                         ctxcomparefn compar,
                         void*        context)
{
    return (safe_lwork_context(key, base, nelp, width, compar, context, 0));
}

static void* safe_lwork_context(const void*  key,
                                const void*  base,
                                size_t*      nelp,
                                size_t       width,
                                ctxcomparefn compar,
                                void*        context,
                                int          addelem)
{
    if (nelp == M_NULLPTR || (*nelp > 0 && (key == M_NULLPTR || base == M_NULLPTR || compar == M_NULLPTR)))
    {
        errno = EINVAL;
        return M_NULLPTR;
    }
    else if (*nelp > RSIZE_MAX || width > RSIZE_MAX)
    {
        errno = ERANGE;
        return M_NULLPTR;
    }
    else
    {
        uint8_t* ep   = M_CONST_CAST(uint8_t*, base);
        uint8_t* endp = M_NULLPTR;
        for (endp = M_REINTERPRET_CAST(uint8_t*, ep + width * (*nelp)); ep < endp; ep += width)
        {
            if (compar(key, ep, context) == 0)
            {
                return (ep);
            }
        }

        /* lfind() shall return when the key was not found. */
        if (!addelem)
        {
            return (M_NULLPTR);
        }
        /*
         * lsearch() adds the key to the end of the table and increments
         * the number of elements.
         */
        safe_memcpy(endp, width, key, width);
        ++*nelp;

        return (endp);
    }
}
