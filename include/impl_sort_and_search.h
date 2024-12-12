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
// \file impl_sort_and_search.h
// \brief This is an internal header to handle the internal definitions for sort_and_search.h
//        Do NOT include this and use these functions directly. Include sort_and_search.h and use the calls from there
//        The functions with "impl" in the name expect __FILE__, __func__, __LINE__, and an expression when called which
//        get wrapped to make this easier for you to use.
 
#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#if !defined(COMPARE_FUNC_TYPES)
    typedef int (*comparefn)(const void*, const void*);

    typedef int (*ctxcomparefn)(const void*, const void*, void*);
#    define COMPARE_FUNC_TYPES
#endif // COMPARE_FUNC_TYPES

    // regular qsort without context, but added bounds checking
    errno_t safe_qsort_impl(void*       ptr,
                            rsize_t     count,
                            rsize_t     size,
                            comparefn   compare,
                            const char* file,
                            const char* function,
                            int         line,
                            const char* expression);

    // like qsort_s with bounds checking and optional context parameter
    errno_t safe_qsort_context_impl(void*        ptr,
                                    rsize_t      count,
                                    rsize_t      size,
                                    ctxcomparefn compare,
                                    void*        context,
                                    const char*  file,
                                    const char*  function,
                                    int          line,
                                    const char*  expression);

    // regular bsearch without context, but added bounds checking
    void* safe_bsearch_impl(const void* key,
                            const void* ptr,
                            rsize_t     count,
                            rsize_t     size,
                            comparefn   compare,
                            const char* file,
                            const char* function,
                            int         line,
                            const char* expression);

    // like bsearch_s with bounds checking and optional context parameter
    void* safe_bsearch_context_impl(const void*  key,
                                    void*        ptr,
                                    rsize_t      count,
                                    rsize_t      size,
                                    ctxcomparefn compare,
                                    void*        context,
                                    const char*  file,
                                    const char*  function,
                                    int          line,
                                    const char*  expression);

    void* safe_lsearch_impl(const void* key,
                            void*       base,
                            size_t*     nelp,
                            size_t      width,
                            comparefn   compar,
                            const char* file,
                            const char* function,
                            int         line,
                            const char* expression);

    void* safe_lsearch_context_impl(const void*  key,
                                    void*        base,
                                    size_t*      nelp,
                                    size_t       width,
                                    ctxcomparefn compar,
                                    void*        context,
                                    const char*  file,
                                    const char*  function,
                                    int          line,
                                    const char*  expression);

    void* safe_lfind_impl(const void* key,
                          const void* base,
                          size_t*     nelp,
                          size_t      width,
                          comparefn   compar,
                          const char* file,
                          const char* function,
                          int         line,
                          const char* expression);

    void* safe_lfind_context_impl(const void*  key,
                                  const void*  base,
                                  size_t*      nelp,
                                  size_t       width,
                                  ctxcomparefn compar,
                                  void*        context,
                                  const char*  file,
                                  const char*  function,
                                  int          line,
                                  const char*  expression);

#if defined(__cplusplus)
}
#endif
