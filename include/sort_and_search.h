// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All
// Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file sort_and_search.h
// \brief Provides safe sort and search function definitions. These work similar
// to C11's Annex K
//        checking bounds, checking for valid pointers, etc.
//

#pragma once
// this file provides qsort_s and bsearch_s type functions, named <>_context.
// There are also wrapper macros to handle differences between Windows, C11
// Annex K, and POSIX APIs so that this works "the same" on all platforms The
// functions in here provide the same parameter validation mentioned in C11
// annex K

#include "common_types.h"
#include "predef_env_detect.h"

#if defined(__cplusplus)
extern "C"
{
#endif //__cplusplus

    typedef int (*comparefn)(const void*, const void*);

    typedef int (*ctxcomparefn)(const void*, const void*, void*);

    // regular qsort without context, but added bounds checking
    errno_t safe_qsort(void* ptr, rsize_t count, rsize_t size, comparefn compare);

    // like qsort_s with bounds checking and optional context parameter
    errno_t safe_qsort_context(void* ptr, rsize_t count, rsize_t size, ctxcomparefn compare, void* context);

    // regular bsearch without context, but added bounds checking
    void* safe_bsearch(const void* key, const void* ptr, rsize_t count, rsize_t size, comparefn compare);

    // like bsearch_s with bounds checking and optional context parameter
    void* safe_bsearch_context(const void*  key,
                               void*        ptr,
                               rsize_t      count,
                               rsize_t      size,
                               ctxcomparefn compare,
                               void*        context);

    void* safe_lsearch(const void* key, void* base, size_t* nelp, size_t width, comparefn compar);

    void* safe_lsearch_context(const void*  key,
                               void*        base,
                               size_t*      nelp,
                               size_t       width,
                               ctxcomparefn compar,
                               void*        context);

    void* safe_lfind(const void* key, const void* base, size_t* nelp, size_t width, comparefn compar);

    void* safe_lfind_context(const void*  key,
                             const void*  base,
                             size_t*      nelp,
                             size_t       width,
                             ctxcomparefn compar,
                             void*        context);

#if defined(__cplusplus)
}
#endif //__cplusplus
