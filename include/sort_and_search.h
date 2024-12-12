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

#include "code_attributes.h"
#include "common_types.h"
#include "impl_sort_and_search.h"
#include "predef_env_detect.h"

#if defined(__cplusplus)
extern "C"
{
#endif //__cplusplus

#if !defined(COMPARE_FUNC_TYPES)
    typedef int (*comparefn)(const void*, const void*);

    typedef int (*ctxcomparefn)(const void*, const void*, void*);
#    define COMPARE_FUNC_TYPES
#endif // COMPARE_FUNC_TYPES

    // regular qsort without context, but added bounds checking
#if defined(DEV_ENVIRONMENT)
    M_INLINE errno_t safe_qsort(void* ptr, rsize_t count, rsize_t size, comparefn compare)
    {
        return safe_qsort_impl(ptr, count, size, compare, __FILE__, __func__, __LINE__,
                               "safe_qsort(ptr, count, size, compare)");
    }
#else
#    define safe_qsort(ptr, count, size, compare)                                                                      \
        safe_qsort_impl(ptr, count, size, compare, __FILE__, __func__, __LINE__,                                       \
                        "safe_qsort(" #ptr ", " #count ", " #size ", " #compare ")")
#endif

    // like qsort_s with bounds checking and optional context parameter
#if defined(DEV_ENVIRONMENT)
    M_INLINE errno_t safe_qsort_context(void* ptr, rsize_t count, rsize_t size, comparefn compare, void* context)
    {
        return safe_qsort_context_impl(ptr, count, size, compare, context, __FILE__, __func__, __LINE__,
                                       "safe_qsort_context(ptr, count, size, compare, context)");
    }
#else
#    define safe_qsort_context(ptr, count, size, compare, context)                                                     \
        safe_qsort_context_impl(ptr, count, size, compare, context, __FILE__, __func__, __LINE__,                      \
                                "safe_qsort_context(" #ptr ", " #count ", " #size ", " #compare ", " #context ")")
#endif

// regular bsearch without context, but added bounds checking
#if defined(DEV_ENVIRONMENT)
    M_INLINE void* safe_bsearch(const void* key, const void* ptr, rsize_t count, rsize_t size, comparefn compare)
    {
        return safe_bsearch_impl(key, ptr, count, size, compare, __FILE__, __func__, __LINE__,
                                 "safe_bsearch(key, ptr, count, size, compare)");
    }
#else
#    define safe_bsearch(key, ptr, count, size, compare)                                                               \
        safe_bsearch_impl(key, ptr, count, size, compare, __FILE__, __func__, __LINE__,                                \
                          "safe_bsearch(" #key ", " #ptr ", " #count ", " #size ", " #compare ")")
#endif

    // like bsearch_s with bounds checking and optional context parameter
#if defined(DEV_ENVIRONMENT)
    M_INLINE void* safe_bsearch_context(const void* key,
                                        const void* ptr,
                                        rsize_t     count,
                                        rsize_t     size,
                                        comparefn   compare,
                                        void*       context)
    {
        return safe_bsearch_context_impl(key, ptr, count, size, compare, context, __FILE__, __func__, __LINE__,
                                         "safe_bsearch_context(key, ptr, count, size, compare, context)");
    }
#else
#    define safe_bsearch_context(key, ptr, count, size, compare, context)                                              \
        safe_bsearch_context_impl(key, ptr, count, size, compare, context, __FILE__, __func__, __LINE__,               \
                                  "safe_bsearch_context(" #key ", " #ptr ", " #count ", " #size ", " #compare          \
                                  ", " #context ")")
#endif

#if defined(DEV_ENVIRONMENT)
    M_INLINE void* safe_lsearch(const void* key, void* base, size_t* nelp, size_t width, comparefn compar)
    {
        return safe_lsearch_impl(key, base, nelp, width, compar, __FILE__, __func__, __LINE__,
                                 "safe_lsearch(key, base, nelp, width, compar)");
    }
#else
#    define safe_lsearch(key, base, nelp, width, compar)                                                               \
        safe_lsearch_impl(key, base, nelp, width, compar, __FILE__, __func__, __LINE__,                                \
                          "safe_lsearch(" #key ", " #base ", " #nelp ", " #width ", " #compar ")")
#endif

#if defined(DEV_ENVIRONMENT)
    M_INLINE void* safe_lsearch_context(const void* key,
                                        void*       base,
                                        size_t*     nelp,
                                        size_t      width,
                                        comparefn   compar,
                                        void*       context)
    {
        return safe_lsearch_context_impl(key, base, nelp, width, compar, context, __FILE__, __func__, __LINE__,
                                         "safe_lsearch_context(key, base, nelp, width, compar, context)");
    }
#else
#    define safe_lsearch_context(key, base, nelp, width, compar, context)                                              \
        safe_lsearch_context_impl(key, base, nelp, width, compar, context, __FILE__, __func__, __LINE__,               \
                                  "safe_lsearch_context(" #key ", " #base ", " #nelp ", " #width ", " #compar          \
                                  ", " #context ")")
#endif

#if defined(DEV_ENVIRONMENT)
    M_INLINE void* safe_lfind(const void* key, void* base, size_t* nelp, size_t width, comparefn compar)
    {
        return safe_lfind_impl(key, base, nelp, width, compar, __FILE__, __func__, __LINE__,
                               "safe_lfind(key, base, nelp, width, compar)");
    }
#else
#    define safe_lfind(key, base, nelp, width, compar)                                                                 \
        safe_lfind_impl(key, base, nelp, width, compar, __FILE__, __func__, __LINE__,                                  \
                        "safe_lfind(" #key ", " #base ", " #nelp ", " #width ", " #compar ")")
#endif

#if defined(DEV_ENVIRONMENT)
    M_INLINE void* safe_lfind_context(const void* key,
                                      void*       base,
                                      size_t*     nelp,
                                      size_t      width,
                                      comparefn   compar,
                                      void*       context)
    {
        return safe_lfind_context_impl(key, base, nelp, width, compar, context, __FILE__, __func__, __LINE__,
                                       "safe_lfind_context(key, base, nelp, width, compar, context)");
    }
#else
#    define safe_lfind_context(key, base, nelp, width, compar, context)                                                \
        safe_lfind_context_impl(key, base, nelp, width, compar, context, __FILE__, __func__, __LINE__,                 \
                                "safe_lfind_context(" #key ", " #base ", " #nelp ", " #width ", " #compar              \
                                ", " #context ")")
#endif

#if defined(__cplusplus)
}
#endif //__cplusplus
