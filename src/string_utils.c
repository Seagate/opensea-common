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
// \file string_utils.h
// \brief Implements various functions to work with C style strings.
//        Many of these implement best practices for safety as well.

#include "string_utils.h"
#include "common_types.h"
#include "constraint_handling.h"
#include "env_detect.h"
#include "math_utils.h"
#include "memory_safety.h"
#include "type_conversion.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

// making it look similar to a std lib function like isPrint.
int is_ASCII(int c)
{
    // this should be even faster than the if/else approach in CPU cycles
    // instead of branching, this bit manipulation will operate quicker.
    // Basically it will check for any bits outside of 7F being set to 1
    // returning a positive value if they are-TJE
    return !(c & ~0x7F);
}

// This is good for checks on "is<>", not for "to<>"
static int handle_eof(int result)
{
    if (result == EOF)
    {
        errno = 0;
        return 0;
    }
    return result;
}

static int is_valid_unsigned_char_range(int c)
{
    return (c >= 0 && c <= UCHAR_MAX);
}

int safe_isascii(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(is_ASCII(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isalnum(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isalnum(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isalpha(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isalpha(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_islower(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(islower(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_tolower(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return tolower(C_CAST(unsigned char, c));
    }
    else if (c == EOF)
    {
        errno = 0;
        return EOF;
    }
    else
    {
        errno = ERANGE;
        return c;
    }
}

int safe_isupper(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isupper(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_toupper(int c)
{
    if (c >= 0 && c <= UCHAR_MAX)
    {
        errno = 0;
        return toupper(C_CAST(unsigned char, c));
    }
    else if (c == EOF)
    {
        errno = 0;
        return EOF;
    }
    else
    {
        errno = ERANGE;
        return c;
    }
}

int safe_isdigit(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isdigit(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isxdigit(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isxdigit(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_iscntrl(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(iscntrl(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isgraph(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isgraph(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isspace(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isspace(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

// isblank added in C99
// May need version check if we run into an environment that does not support
// this check-TJE
int safe_isblank(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isblank(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isprint(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isprint(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_ispunct(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(ispunct(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

errno_t safe_strcpy_impl(char* M_RESTRICT       dest,
                         rsize_t                destsz,
                         const char* M_RESTRICT src,
                         const char*            file,
                         const char*            function,
                         int                    line,
                         const char*            expression)
{
    errno_t           error  = 0;
    size_t            srclen = safe_strnlen(src, destsz);
    constraintEnvInfo envInfo;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strcpy: dest is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strcpy: src is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        if (destsz > RSIZE_T_C(0) && destsz <= RSIZE_MAX)
        {
            dest[0] = 0;
        }
        errno = error;
        return error;
    }
    else if (destsz == RSIZE_T_C(0))
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strcpy: destsz is zero",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strcpy: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (destsz <= srclen)
    {
        dest[0] = 0;
        error   = ERANGE;
        invoke_Constraint_Handler("safe_strcpy: destsz <= srclen",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (memory_regions_overlap(dest, destsz, src, srclen))
    {
        dest[0] = 0;
        error   = EINVAL;
        invoke_Constraint_Handler("safe_strcpy: src and destination overlap!",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
#if defined(HAVE_MSFT_SECURE_LIB)
        // call MSFT implementation to reduce warnings. It does not detect as
        // many cases as standard which is why it's down here.-TJE
        error = strcpy_s(dest, destsz, src);
#else
        error        = safe_memccpy(dest, destsz, src, '\0', srclen + SIZE_T_C(1));
        dest[srclen] = '\0'; // ensuring NULL termination
#endif
        errno = error;
        return error;
    }
}

errno_t safe_strmove_impl(char*       dest,
                          rsize_t     destsz,
                          const char* src,
                          const char* file,
                          const char* function,
                          int         line,
                          const char* expression)
{
    errno_t           error  = 0;
    size_t            srclen = safe_strnlen(src, destsz);
    constraintEnvInfo envInfo;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strmove: dest is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strmove: src is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        if (destsz > 0 && destsz <= RSIZE_MAX)
        {
            dest[0] = 0;
        }
        errno = error;
        return error;
    }
    else if (destsz == RSIZE_T_C(0))
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strmove: destsz is zero",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strmove: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (destsz <= srclen)
    {
        dest[0] = 0;
        error   = ERANGE;
        invoke_Constraint_Handler("safe_strmove: destsz <= srclen",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
        error        = safe_memcmove(dest, destsz, src, '\0', srclen + SIZE_T_C(1));
        dest[srclen] = '\0'; // ensuring NULL termination
        errno        = error;
        return error;
    }
}

errno_t safe_strncpy_impl(char* M_RESTRICT       dest,
                          rsize_t                destsz,
                          const char* M_RESTRICT src,
                          rsize_t                count,
                          const char*            file,
                          const char*            function,
                          int                    line,
                          const char*            expression)
{
    errno_t           error  = 0;
    size_t            srclen = safe_strnlen(src, count);
    constraintEnvInfo envInfo;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strncpy: dest is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strncpy: src is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        if (destsz > RSIZE_T_C(0) && destsz <= RSIZE_MAX)
        {
            dest[0] = 0;
        }
        errno = error;
        return error;
    }
    else if (destsz == RSIZE_T_C(0))
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strncpy: destsz is zero",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strncpy: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (count > RSIZE_MAX)
    {
        dest[0] = 0;
        error   = ERANGE;
        invoke_Constraint_Handler("safe_strncpy: count > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (count >= destsz && destsz <= srclen)
    {
        dest[0] = 0;
        error   = ERANGE;
        invoke_Constraint_Handler("safe_strncpy: count >= destsz",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (memory_regions_overlap(dest, destsz, src, count))
    {
        dest[0] = 0;
        error   = EINVAL;
        invoke_Constraint_Handler("safe_strncpy: src and destination overlap!",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
#if defined(HAVE_MSFT_SECURE_LIB)
        // call MSFT implementation to reduce warnings. It does not detect as
        // many cases as standard which is why it's down here.-TJE
        error = strncpy_s(dest, destsz, src, count);
#else
        error        = safe_memccpy(dest, destsz, src, '\0', count);
        if (srclen < count)
        {
            dest[srclen] = '\0'; // ensuring NULL termination
        }
        else
        {
            dest[count] = '\0'; // ensuring NULL termination
        }
#endif
        errno = error;
        return error;
    }
}

errno_t safe_strnmove_impl(char*       dest,
                           rsize_t     destsz,
                           const char* src,
                           rsize_t     count,
                           const char* file,
                           const char* function,
                           int         line,
                           const char* expression)
{
    errno_t           error  = 0;
    size_t            srclen = safe_strnlen(src, count);
    constraintEnvInfo envInfo;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strnmove: dest is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strnmove: src is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        if (destsz > RSIZE_T_C(0) && destsz <= RSIZE_MAX)
        {
            dest[0] = 0;
        }
        errno = error;
        return error;
    }
    else if (destsz == RSIZE_T_C(0))
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strnmove: destsz is zero",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strnmove: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (count > RSIZE_MAX)
    {
        dest[0] = 0;
        error   = ERANGE;
        invoke_Constraint_Handler("safe_strnmove: count > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (count >= destsz && destsz <= srclen)
    {
        dest[0] = 0;
        error   = ERANGE;
        invoke_Constraint_Handler("safe_strnmove: count >= destsz",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
        error = safe_memcmove(dest, destsz, src, '\0', count);
        if (srclen < count)
        {
            dest[srclen] = '\0'; // ensuring NULL termination
        }
        else
        {
            dest[count] = '\0'; // ensuring NULL termination
        }
        errno = error;
        return error;
    }
}

errno_t safe_strcat_impl(char* M_RESTRICT       dest,
                         rsize_t                destsz,
                         const char* M_RESTRICT src,
                         const char*            file,
                         const char*            function,
                         int                    line,
                         const char*            expression)
{
    errno_t           error    = 0;
    size_t            srclen   = safe_strnlen(src, destsz);
    char*             destnull = M_NULLPTR;
    constraintEnvInfo envInfo;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strcat: dest is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strcat: src is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        if (destsz > RSIZE_T_C(0) && destsz <= RSIZE_MAX)
        {
            dest[0] = 0;
        }
        errno = error;
        return error;
    }
    else if (destsz == RSIZE_T_C(0))
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strcat: destsz is zero",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strcat: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (M_NULLPTR == (destnull = M_REINTERPRET_CAST(char*, memchr(dest, '\0', destsz))))
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strcat: No NULL terminator found in dest",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if ((destsz + M_STATIC_CAST(uintptr_t, destnull)) <= srclen) // truncation
    {
        dest[0] = 0;
        error   = ERANGE;
        invoke_Constraint_Handler("safe_strcat: destsz too small. Src will be truncated.",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (memory_regions_overlap(dest, destsz, src, srclen))
    {
        dest[0] = 0;
        error   = EINVAL;
        invoke_Constraint_Handler("safe_strcat: src and destination overlap!",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
#if defined(HAVE_MSFT_SECURE_LIB)
        // call MSFT implementation to reduce warnings. It does not detect as
        // many cases as standard which is why it's down here.-TJE
        error = strcat_s(dest, destsz, src);
#else
        error =
            safe_memccpy(destnull, destsz - (C_CAST(uintptr_t, destnull) - C_CAST(uintptr_t, dest)), src, '\0', srclen);
        destnull[srclen] = '\0';
#endif
        errno = error;
        return error;
    }
}

errno_t safe_strncat_impl(char* M_RESTRICT       dest,
                          rsize_t                destsz,
                          const char* M_RESTRICT src,
                          rsize_t                count,
                          const char*            file,
                          const char*            function,
                          int                    line,
                          const char*            expression)
{
    errno_t           error    = 0;
    size_t            srclen   = safe_strnlen(src, destsz);
    char*             destnull = M_NULLPTR;
    constraintEnvInfo envInfo;
    if (dest == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strncat: dest is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strncat: src is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        if (destsz > RSIZE_T_C(0) && destsz <= RSIZE_MAX)
        {
            dest[0] = 0;
        }
        errno = error;
        return error;
    }
    else if (destsz == RSIZE_T_C(0))
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strncat: destsz is zero",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (destsz > RSIZE_MAX)
    {
        error = ERANGE;
        invoke_Constraint_Handler("safe_strncat: destsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (M_NULLPTR == (destnull = M_REINTERPRET_CAST(char*, memchr(dest, '\0', destsz))))
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strncat: No NULL terminator found in dest",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (((destsz + M_STATIC_CAST(uintptr_t, destnull)) <= srclen) ||
             ((destsz + M_STATIC_CAST(uintptr_t, destnull)) <= count)) // truncation
    {
        dest[0] = 0;
        error   = ERANGE;
        invoke_Constraint_Handler("safe_strncat: destsz too small. Src will be truncated.",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (memory_regions_overlap(dest, destsz, src, count))
    {
        dest[0] = 0;
        error   = EINVAL;
        invoke_Constraint_Handler("safe_strncat: src and destination overlap!",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
#if defined(HAVE_MSFT_SECURE_LIB)
        // call MSFT implementation to reduce warnings. It does not detect as
        // many cases as standard which is why it's down here.-TJE
        error = strncat_s(dest, destsz, src, count);
#else
        error =
            safe_memccpy(destnull, destsz - (C_CAST(uintptr_t, destnull) - C_CAST(uintptr_t, dest)), src, '\0', count);
        destnull[count] = '\0';
#endif
        errno = error;
        return error;
    }
}

char* safe_String_Token_impl(char* M_RESTRICT       str,
                             rsize_t* M_RESTRICT    strmax,
                             const char* M_RESTRICT delim,
                             char** M_RESTRICT      saveptr,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression)
{
    errno_t           error   = 0;
    char*             token   = M_NULLPTR;
    char*             tokiter = M_NULLPTR;
    constraintEnvInfo envInfo;
    if (strmax == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_String_Token: strmax = NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (delim == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_String_Token: delim = NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
    else if (saveptr == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_String_Token: saveptr = NULL",
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
            invoke_Constraint_Handler("safe_String_Token: *strmax = 0 on initial call",
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
            invoke_Constraint_Handler("safe_String_Token: *strmax > RSIZE_MAX on initial call",
                                      set_Env_Info(&envInfo, file, function, expression, line), error);
            errno = error;
            return M_NULLPTR;
        }
    }
    else
    {
        // non-initial call of the function
        if (*saveptr == M_NULLPTR)
        {
            *saveptr = M_NULLPTR;
            error    = EINVAL;
            invoke_Constraint_Handler("safe_String_Token: *saveptr = NULL on non-initial call",
                                      set_Env_Info(&envInfo, file, function, expression, line), error);
            errno = error;
            return M_NULLPTR;
        }
    }
    token   = *saveptr;
    tokiter = *saveptr;
    while (*strmax > RSIZE_T_C(0) && *tokiter)
    {
        if (strchr(delim, *tokiter))
        {
            break; // Found a delimiter
        }
        ++tokiter;
        --(*strmax);
    }
    if (*strmax == RSIZE_T_C(0))
    {
        if (*tokiter != '\0')
        {
            *saveptr = M_NULLPTR;
            error    = ERANGE;
            invoke_Constraint_Handler("safe_String_Token: reached end of source "
                                      "string without encountering null terminator",
                                      set_Env_Info(&envInfo, file, function, expression, line), error);
            errno = error;
        }
        return M_NULLPTR;
    }
    if (*tokiter)
    {
        *tokiter = '\0';
        *saveptr = tokiter + 1;
        --(*strmax);
    }
    else
    {
        *saveptr = tokiter;
    }
    return token;
}

size_t safe_strnlen(const char* string, size_t n)
{
#if defined(HAVE_C11_ANNEX_K) || defined(HAVE_MSFT_SECURE_LIB)
    // Does not invoke constraint handler of it's own so we can use this here - TJE
    return strnlen_s(string, n);
#elif defined(POSIX_2008) || defined(USING_SUS4) || defined(HAVE_STRNLEN) /*also glibc 2.0, openbsd 4.8*/
    if (string != M_NULLPTR)
    {
        return strnlen(string, n);
    }
    else
    {
        return SIZE_T_C(0);
    }
#else
    // implement this ourselves with memchr after making sure string is not a
    // null pointer
    if (string != M_NULLPTR)
    {
        const char* found = memchr(string, '\0', n);
        if (found != M_NULLPTR)
        {
            return C_CAST(size_t, C_CAST(uintptr_t, found) - C_CAST(uintptr_t, string));
        }
        else
        {
            return n;
        }
    }
    return SIZE_T_C(0);
#endif
}

#if !defined(__STDC_ALLOC_LIB__) && !defined(POSIX_2008) && !defined(USING_C23)
M_FUNC_ATTR_MALLOC char* strndup(const char* src, size_t size)
{
    size_t length = memchr(src, '\0', size) != M_NULLPTR
                        ? C_CAST(size_t, C_CAST(uintptr_t, memchr(src, '\0', size)) - C_CAST(uintptr_t, src))
                        : SIZE_T_C(0);
    char*  dupstr = M_REINTERPRET_CAST(char*, malloc(length + 1));
    if (dupstr == M_NULLPTR)
    {
        return M_NULLPTR;
    }
    memcpy(dupstr, src, length);
    dupstr[length] = '\0';
    return dupstr;
}
#endif // checks for strndup

errno_t safe_strdup_impl(char**      dup,
                         const char* src,
                         const char* file,
                         const char* function,
                         int         line,
                         const char* expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    errno = 0;
    if (dup == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strdup: dup == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strdup: src == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else
    {
        rsize_t srclen = safe_strnlen(src, RSIZE_MAX);
        if (srclen == RSIZE_MAX && (srclen + RSIZE_T_C(1)) != '\0')
        {
            error = EINVAL;
            invoke_Constraint_Handler("safe_strdup: src longer than RSIZE_MAX",
                                      set_Env_Info(&envInfo, file, function, expression, line), error);
            errno = error;
            return error;
        }
        else if (srclen == RSIZE_T_C(0))
        {
            error = EINVAL;
            invoke_Constraint_Handler("safe_strdup: src is len 0",
                                      set_Env_Info(&envInfo, file, function, expression, line), error);
            errno = error;
            return error;
        }
        *dup = malloc(srclen + RSIZE_T_C(1));
        if (*dup != M_NULLPTR)
        {
            safe_memcpy(*dup, srclen + RSIZE_T_C(1), src, srclen);
            (*dup)[srclen] = '\0';
        }
        else
        {
            error = ENOMEM;
            // TODO: Call constraint handler
        }
    }
    return error;
}

errno_t safe_strndup_impl(char**      dup,
                          const char* src,
                          rsize_t     size,
                          const char* file,
                          const char* function,
                          int         line,
                          const char* expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    errno = 0;
    if (dup == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strndup: dup == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else if (src == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strndup: src == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else if (size > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strndup: size > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (size == RSIZE_T_C(0))
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strndup: size == 0", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else
    {
        *dup = safe_malloc(size + RSIZE_T_C(1));
        if (*dup != M_NULLPTR)
        {
            safe_memcpy(*dup, size + 1, src, size);
            (*dup)[size] = '\0';
        }
        else
        {
            error = ENOMEM;
            // TODO: Call constraint handler
        }
    }
    errno = error;
    return error;
}

void byte_Swap_String_Len(char* stringToChange, size_t stringlen)
{
    if (stringlen > SIZE_T_C(1)) // Check if the string has more than one character
    {
        for (size_t stringIter = SIZE_T_C(0); stringIter < stringlen - SIZE_T_C(1); stringIter += SIZE_T_C(2))
        {
            // Swap the characters
            char temp                                = stringToChange[stringIter];
            stringToChange[stringIter]               = stringToChange[stringIter + SIZE_T_C(1)];
            stringToChange[stringIter + SIZE_T_C(1)] = temp;
        }
    }
}

// use this to swap the bytes in a string...useful for ATA strings
void byte_Swap_String(char* stringToChange)
{
    size_t stringlen = safe_strlen(stringToChange);
    if (stringlen > SIZE_T_C(1)) // Check if the string has more than one character
    {
        for (size_t stringIter = SIZE_T_C(0); stringIter < stringlen - SIZE_T_C(1); stringIter += SIZE_T_C(2))
        {
            // Swap the characters
            char temp                                = stringToChange[stringIter];
            stringToChange[stringIter]               = stringToChange[stringIter + SIZE_T_C(1)];
            stringToChange[stringIter + SIZE_T_C(1)] = temp;
        }
    }
}

void remove_Whitespace_Left(char* stringToChange)
{
    size_t iter = SIZE_T_C(0);
    size_t len  = SIZE_T_C(0);
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    len = strspn(stringToChange,
                 " \t\n\v\f"); // only touch spaces at the beginning of the
                               // string, not the whole string
    if (len == SIZE_T_C(0))
    {
        return;
    }

    while ((iter < (safe_strlen(stringToChange) - SIZE_T_C(1)) &&
            stringToChange[iter])) // having issues with the isspace command
                                   // leaving extra chars in the string
    {
        stringToChange[iter] = stringToChange[iter + len];
        iter++;
    }
}

void remove_Trailing_Whitespace(char* stringToChange)
{
    size_t iter = SIZE_T_C(0);
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    iter = (safe_strlen(stringToChange));
    if (iter == SIZE_T_C(0))
    {
        return;
    }
    while (iter > SIZE_T_C(0) && safe_isascii(stringToChange[iter - SIZE_T_C(1)]) &&
           safe_isspace(stringToChange[iter - SIZE_T_C(1)]))
    {
        stringToChange[iter - SIZE_T_C(1)] = '\0'; // replace spaces with null terminators
        iter--;
    }
}

void remove_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR || stringlen == SIZE_T_C(0))
    {
        return;
    }

    size_t iter = stringlen;
    while (iter > SIZE_T_C(0) && safe_isascii(stringToChange[iter - SIZE_T_C(1)]) &&
           safe_isspace(stringToChange[iter - SIZE_T_C(1)]))
    {
        stringToChange[iter - SIZE_T_C(1)] = '\0'; // Replace spaces with null terminators
        iter--;
    }
}

void remove_Leading_Whitespace(char* stringToChange)
{
    size_t iter              = SIZE_T_C(0);
    size_t stringToChangeLen = SIZE_T_C(0);
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    stringToChangeLen = safe_strlen(stringToChange);
    while (safe_isascii(stringToChange[iter]) && safe_isspace(stringToChange[iter]) && iter < stringToChangeLen)
    {
        iter++;
    }
    if (iter > SIZE_T_C(0))
    {
        safe_memmove(&stringToChange[0], stringToChangeLen, &stringToChange[iter], stringToChangeLen - iter);
        safe_memset(&stringToChange[stringToChangeLen - iter], stringToChangeLen - iter, 0,
                    iter); // should this be a null? Or a space? Leaving as null
                           // for now since it seems to work...
    }
}

void remove_Leading_Whitespace_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR || stringlen == SIZE_T_C(0))
    {
        return;
    }

    size_t iter = SIZE_T_C(0);
    while (iter < stringlen && safe_isascii(stringToChange[iter]) && safe_isspace(stringToChange[iter]))
    {
        iter++;
    }

    if (iter > SIZE_T_C(0))
    {
        safe_memmove(stringToChange, stringlen, &stringToChange[iter], stringlen - iter);
        safe_memset(&stringToChange[stringlen - iter], stringlen - iter, 0,
                    iter); // Null-terminate the shifted string
    }
}

void remove_Leading_And_Trailing_Whitespace(char* stringToChange)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }

    size_t stringlen = safe_strlen(stringToChange);
    if (stringlen == SIZE_T_C(0))
    {
        return;
    }

    // Remove leading whitespace (calculate for memmove later)
    size_t start = SIZE_T_C(0);
    while (start < stringlen && safe_isascii(stringToChange[start]) && safe_isspace(stringToChange[start]))
    {
        start++;
    }

    // Remove trailing whitespace
    size_t end = stringlen;
    while (end > start && safe_isascii(stringToChange[end - SIZE_T_C(1)]) &&
           safe_isspace(stringToChange[end - SIZE_T_C(1)]))
    {
        end--;
    }

    // Calculate new length after removing whitespace
    size_t newlen = end - start;

    // If there's leading whitespace, shift the string to the start
    if (start > SIZE_T_C(0))
    {
        safe_memmove(stringToChange, stringlen, &stringToChange[start], newlen);
    }

    // Null-terminate the string after the last non-whitespace character
    stringToChange[newlen] = '\0';
}

void remove_Leading_And_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR || stringlen == SIZE_T_C(0))
    {
        return;
    }

    // Remove leading whitespace (calculate for memmove later)
    size_t start = SIZE_T_C(0);
    while (start < stringlen && safe_isascii(stringToChange[start]) && safe_isspace(stringToChange[start]))
    {
        start++;
    }

    // Remove trailing whitespace
    size_t end = stringlen;
    while (end > start && safe_isascii(stringToChange[end - SIZE_T_C(1)]) &&
           safe_isspace(stringToChange[end - SIZE_T_C(1)]))
    {
        end--;
    }

    // Calculate new length after removing whitespace
    size_t newlen = end - start;

    // If there's leading whitespace, shift the string to the start
    if (start > SIZE_T_C(0))
    {
        safe_memmove(stringToChange, stringlen, &stringToChange[start], newlen);
    }

    // Null-terminate the string after the last non-whitespace character
    stringToChange[newlen] = '\0';
}

void convert_String_To_Upper_Case(char* stringToChange)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = SIZE_T_C(0); stringToChange[iter] != '\0'; iter++)
    {
        stringToChange[iter] = C_CAST(char, safe_toupper(stringToChange[iter]));
    }
}

void convert_String_To_Upper_Case_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = SIZE_T_C(0); iter < stringlen; iter++)
    {
        stringToChange[iter] = C_CAST(char, safe_toupper(stringToChange[iter]));
    }
}

void convert_String_To_Lower_Case(char* stringToChange)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = SIZE_T_C(0); stringToChange[iter] != '\0'; iter++)
    {
        stringToChange[iter] = C_CAST(char, safe_tolower(stringToChange[iter]));
    }
}

void convert_String_To_Lower_Case_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = SIZE_T_C(0); iter < stringlen; iter++)
    {
        stringToChange[iter] = C_CAST(char, safe_tolower(stringToChange[iter]));
    }
}

void convert_String_To_Inverse_Case(char* stringToChange)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = SIZE_T_C(0); stringToChange[iter] != '\0'; iter++)
    {
        if (safe_islower(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, safe_toupper(stringToChange[iter]));
        }
        else if (safe_isupper(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, safe_tolower(stringToChange[iter]));
        }
    }
}

void convert_String_To_Inverse_Case_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = SIZE_T_C(0); iter < stringlen; iter++)
    {
        if (safe_islower(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, safe_toupper(stringToChange[iter]));
        }
        else if (safe_isupper(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, safe_tolower(stringToChange[iter]));
        }
    }
}

size_t find_last_occurrence_in_string(const char* originalString, const char* stringToFind)
{
    if (originalString == M_NULLPTR || stringToFind == M_NULLPTR)
    {
        return SIZE_MAX;
    }

    size_t last_occurrence = SIZE_MAX;
    size_t stringToFindLen = safe_strlen(stringToFind);
    if (stringToFindLen == SIZE_T_C(0))
    {
        return SIZE_MAX; // Searching for an empty string is undefined
    }

    const char* stringToCompare = originalString;
    while ((stringToCompare = strstr(stringToCompare, stringToFind)) != M_NULLPTR)
    {
        last_occurrence = C_CAST(uintptr_t, stringToCompare) - C_CAST(uintptr_t, originalString);
        stringToCompare += stringToFindLen; // Move past the current found substring
    }

    return (last_occurrence != SIZE_MAX) ? last_occurrence : safe_strlen(originalString);
}

size_t find_first_occurrence_in_string(const char* originalString, const char* stringToFind)
{
    if (originalString == M_NULLPTR || stringToFind == M_NULLPTR)
    {
        return SIZE_MAX;
    }

    const char* partialString = strstr(originalString, stringToFind);
    return (partialString != M_NULLPTR) ? (C_CAST(uintptr_t, partialString) - C_CAST(uintptr_t, originalString))
                                        : SIZE_MAX;
}

bool wildcard_Match(const char* pattern, const char* data)
{
    if (pattern == M_NULLPTR || data == M_NULLPTR)
    {
        return false;
    }
    if (*pattern == '\0' && *data == '\0')
    {
        return true;
    }
    if (*pattern == *data || *pattern == '?')
    {
        return *data != '\0' && wildcard_Match(pattern + 1, data + 1);
    }
    if (*pattern == '*')
    {
        return wildcard_Match(pattern + 1, data) || (*data != '\0' && wildcard_Match(pattern, data + 1));
    }
    return false;
}
