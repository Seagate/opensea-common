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
// \brief Defines bounds checked versions of lsearch and lfind similar to qsort_s and bsearch_s checks
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
#include "constraint_handling.h"
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
    errno_t error = 0;
    if (nelp == M_NULLPTR)
    {
        error = EINVAL;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind: nelp == NULL", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch: nelp == NULL", M_NULLPTR, error);
        }
        errno = error;
        return M_NULLPTR;
    }
    if (*nelp > RSIZE_T_C(0) && base == M_NULLPTR)
    {
        error = EINVAL;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind: *nelp > 0 && base == NULL", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch: *nelp > 0 && base == NULL", M_NULLPTR, error);
        }
        errno = error;
        return M_NULLPTR;
    }
    else if (*nelp > RSIZE_T_C(0) && compar == M_NULLPTR)
    {
        error = EINVAL;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind: *nelp > 0 && compar == NULL", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch: *nelp > 0 && compar == NULL", M_NULLPTR, error);
        }
        errno = error;
        return M_NULLPTR;
    }
    else if (*nelp > RSIZE_T_C(0) && key == M_NULLPTR)
    {
        error = EINVAL;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind:*nelp > 0 && key == NULL", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch: *nelp > 0 && key == NULL", M_NULLPTR, error);
        }
        errno = error;
        return M_NULLPTR;
    }
     else if (*nelp > RSIZE_MAX)
    {
        error = ERANGE;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind: *nelp > RSIZE_MAX", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch: *nelp > RSIZE_MAX", M_NULLPTR, error);
        }
        errno = error;
        return M_NULLPTR;
    }
    else if (width > RSIZE_MAX)
    {
        error = ERANGE;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind: width > RSIZE_MAX", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch: width > RSIZE_MAX", M_NULLPTR, error);
        }
        errno = error;
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
                errno = error;
                return (ep);
            }
        }

        /* lfind() shall return when the key was not found. */
        if (!addelem)
        {
            errno = error;
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
    errno_t error = 0;
    if (nelp == M_NULLPTR)
    {
        error = EINVAL;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind_context: nelp == NULL", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch_context: nelp == NULL", M_NULLPTR, error);
        }
        errno = error;
        return M_NULLPTR;
    }
    if (*nelp > RSIZE_T_C(0) && base == M_NULLPTR)
    {
        error = EINVAL;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind_context: *nelp > 0 && base == NULL", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch_context: *nelp > 0 && base == NULL", M_NULLPTR, error);
        }
        errno = error;
        return M_NULLPTR;
    }
    else if (*nelp > RSIZE_T_C(0) && compar == M_NULLPTR)
    {
        error = EINVAL;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind_context: *nelp > 0 && compar == NULL", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch_context: *nelp > 0 && compar == NULL", M_NULLPTR, error);
        }
        errno = error;
        return M_NULLPTR;
    }
    else if (*nelp > RSIZE_T_C(0) && key == M_NULLPTR)
    {
        error = EINVAL;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind_context: *nelp > 0 && key == NULL", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch_context: *nelp > 0 && key == NULL", M_NULLPTR, error);
        }
        errno = error;
        return M_NULLPTR;
    }
     else if (*nelp > RSIZE_MAX)
    {
        error = ERANGE;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind_context: *nelp > RSIZE_MAX", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch_context: *nelp > RSIZE_MAX", M_NULLPTR, error);
        }
        errno = error;
        return M_NULLPTR;
    }
    else if (width > RSIZE_MAX)
    {
        error = ERANGE;
        if (addelem == 0)
        {
            invoke_Constraint_Handler("safe_lfind_context: width > RSIZE_MAX", M_NULLPTR, error);
        }
        else
        {
            invoke_Constraint_Handler("safe_lsearch_context: width > RSIZE_MAX", M_NULLPTR, error);
        }
        errno = error;
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
                errno = error;
                return (ep);
            }
        }

        /* lfind() shall return when the key was not found. */
        if (!addelem)
        {
            errno = error;
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
