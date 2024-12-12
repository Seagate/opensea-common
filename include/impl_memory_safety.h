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
// \file impl_memory_safety.h
// \brief This is an internal header to handle the internal definitions for memory_safety.h
//        Do NOT include this and use these functions directly. Include memory_safety.h and use the calls from there
//        The functions with "impl" in the name expect __FILE__, __func__, __LINE__, and an expression when called which
//        get wrapped by memory_safety.h to make this easier for you to use.

#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    errno_t safe_memset_impl(void*       dest,
                             rsize_t     destsz,
                             int         ch,
                             rsize_t     count,
                             const char* file,
                             const char* function,
                             int         line,
                             const char* expression);

    errno_t safe_memmove_impl(void*       dest,
                              rsize_t     destsz,
                              const void* src,
                              rsize_t     count,
                              const char* file,
                              const char* function,
                              int         line,
                              const char* expression);

    errno_t safe_memcpy_impl(void* M_RESTRICT       dest,
                             rsize_t                destsz,
                             const void* M_RESTRICT src,
                             rsize_t                count,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    errno_t safe_memccpy_impl(void* M_RESTRICT       dest,
                              rsize_t                destsz,
                              const void* M_RESTRICT src,
                              int                    c,
                              rsize_t                count,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    errno_t safe_memcmove_impl(void* M_RESTRICT       dest,
                               rsize_t                destsz,
                               const void* M_RESTRICT src,
                               int                    c,
                               rsize_t                count,
                               const char*            file,
                               const char*            function,
                               int                    line,
                               const char*            expression);

    M_FUNC_ATTR_MALLOC void* safe_malloc_impl(size_t      size,
                                              const char* file,
                                              const char* function,
                                              int         line,
                                              const char* expression);

    M_FUNC_ATTR_MALLOC void* safe_calloc_impl(size_t      count,
                                              size_t      size,
                                              const char* file,
                                              const char* function,
                                              int         line,
                                              const char* expression);

    M_FUNC_ATTR_MALLOC void* safe_malloc_aligned_impl(size_t      size,
                                                      size_t      alignment,
                                                      const char* file,
                                                      const char* function,
                                                      int         line,
                                                      const char* expression);

    M_FUNC_ATTR_MALLOC void* safe_calloc_aligned_impl(size_t      count,
                                                      size_t      size,
                                                      size_t      alignment,
                                                      const char* file,
                                                      const char* function,
                                                      int         line,
                                                      const char* expression);

#if defined(__cplusplus)
}
#endif
