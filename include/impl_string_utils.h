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
// \file impl_string_utils.h
// \brief This is an internal header to handle the internal definitions for string_utils.h
//        Do NOT include this and use these functions directly. Include string_utils.h and use the calls from there
//        The functions with "impl" in the name expect __FILE__, __func__, __LINE__, and an expression when called which
//        get wrapped to make this easier for you to use.

#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    errno_t safe_strcpy_impl(char* M_RESTRICT       dest,
                             rsize_t                destsz,
                             const char* M_RESTRICT src,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    errno_t safe_strmove_impl(char*       dest,
                              rsize_t     destsz,
                              const char* src,
                              const char* file,
                              const char* function,
                              int         line,
                              const char* expression);

    errno_t safe_strncpy_impl(char* M_RESTRICT       dest,
                              rsize_t                destsz,
                              const char* M_RESTRICT src,
                              rsize_t                count,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    errno_t safe_strnmove_impl(char*       dest,
                               rsize_t     destsz,
                               const char* src,
                               rsize_t     count,
                               const char* file,
                               const char* function,
                               int         line,
                               const char* expression);

    errno_t safe_strcat_impl(char* M_RESTRICT       dest,
                             rsize_t                destsz,
                             const char* M_RESTRICT src,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    errno_t safe_strncat_impl(char* M_RESTRICT       dest,
                              rsize_t                destsz,
                              const char* M_RESTRICT src,
                              rsize_t                count,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    char* safe_String_Token_impl(char* M_RESTRICT       str,
                                 rsize_t* M_RESTRICT    strmax,
                                 const char* M_RESTRICT delim,
                                 char** M_RESTRICT      saveptr,
                                 const char*            file,
                                 const char*            function,
                                 int                    line,
                                 const char*            expression);

    errno_t safe_strdup_impl(char**      dup,
                             const char* src,
                             const char* file,
                             const char* function,
                             int         line,
                             const char* expression);

    errno_t safe_strndup_impl(char**      dup,
                              const char* src,
                              rsize_t     size,
                              const char* file,
                              const char* function,
                              int         line,
                              const char* expression);

#if defined(__cplusplus)
}
#endif
