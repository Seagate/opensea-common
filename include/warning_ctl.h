// SPDX-License-Identifier: MPL-2.0

//! \file warning_ctl.h
//! \brief Macros to assist with disabling and restoring compiler warnings in limited situations.
//! Sometimes you need to disable a warning for a small section of code. These macros help with that.
//! Only use these when necessary and restore the warnings as soon as possible!
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2025-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "predef_env_detect.h"

#if defined(__cplusplus)
extern "C"
{
#endif //__cplusplus

//! \def DISABLE_WARNING_4146
//! \brief disables MSVC warning 4146.
//! \details These warning disables are only needed in MSVC for the C11 generic min/max implementations.
//! Without them you get a warning about applying a unary - on an unsigned type.

//! \def RESTORE_WARNING_4146
//! \brief restores MSVC warning 4146 back to being enabled.
//! \details These warning disables are only needed in MSVC for the C11 generic min/max implementations.
//! Without them you get a warning about applying a unary - on an unsigned type.
#if IS_MSVC_VERSION(MSVC_2012)
#    define DISABLE_WARNING_4146 MSVC_PRAGMA(warning(push)) MSVC_PRAGMA(warning(disable : 4146))
#    define RESTORE_WARNING_4146 MSVC_PRAGMA(warning(pop))
#else
#    define DISABLE_WARNING_4146
#    define RESTORE_WARNING_4146
#endif //_MSVC && !clang workaround for min/max macros

//! \def DISABLE_WARNING_SIGN_CONVERSION
//! \brief Disables warning about sign conversion

//! \def RESTORE_WARNING_SIGN_CONVERSION
//! \brief Restored warning about sign conversion
#if IS_CLANG_VERSION(3, 0)
#    define DISABLE_WARNING_SIGN_CONVERSION                                                                            \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wsign-conversion\"")
#    define RESTORE_WARNING_SIGN_CONVERSION _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(4, 3)
#    define DISABLE_WARNING_SIGN_CONVERSION                                                                            \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wsign-conversion\"")
#    define RESTORE_WARNING_SIGN_CONVERSION _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_WARNING_SIGN_CONVERSION
#    define RESTORE_WARNING_SIGN_CONVERSION
#endif

//! \def DISABLE_WARNING_ZERO_LENGTH_ARRAY
//! \brief Disables warning about a zero length array at the end of a structure

//! \def RESTORE_WARNING_ZERO_LENGTH_ARRAY
//! \brief Restores warning about a zero length array at the end of a structure
#if IS_CLANG_VERSION(3, 0)
#    define DISABLE_WARNING_ZERO_LENGTH_ARRAY                                                                          \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wzero-length-array\"")
#    define RESTORE_WARNING_ZERO_LENGTH_ARRAY _Pragma("clang diagnostic pop")
#elif IS_MSVC_VERSION(MSVC_2005)
#    define DISABLE_WARNING_ZERO_LENGTH_ARRAY MSVC_PRAGMA(warning(push)) MSVC_PRAGMA(warning(disable : 4200))
#    define RESTORE_WARNING_ZERO_LENGTH_ARRAY MSVC_PRAGMA(warning(pop))
#else
#    define DISABLE_WARNING_ZERO_LENGTH_ARRAY
#    define RESTORE_WARNING_ZERO_LENGTH_ARRAY
#endif

//! \def DISABLE_WARNING_FORMAT_NONLITERAL
//! \brief Disables warning about a using something other than a string literal as a format string.

//! \def RESTORE_WARNING_FORMAT_NONLITERAL
//! \brief Restores warning about a using something other than a string literal as a format string.
#if IS_CLANG_VERSION(2, 6)
#    define DISABLE_WARNING_FORMAT_NONLITERAL                                                                          \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wformat-nonliteral\"")
#    define RESTORE_WARNING_FORMAT_NONLITERAL _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(3, 4)
#    define DISABLE_WARNING_FORMAT_NONLITERAL                                                                          \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wformat-nonliteral\"")
#    define RESTORE_WARNING_FORMAT_NONLITERAL _Pragma("GCC diagnostic pop")
#elif IS_MSVC_VERSION(MSVC_2005)
#    define DISABLE_WARNING_FORMAT_NONLITERAL MSVC_PRAGMA(warning(push)) MSVC_PRAGMA(warning(disable : 4774))
#    define RESTORE_WARNING_FORMAT_NONLITERAL MSVC_PRAGMA(warning(pop))
#else
#    define DISABLE_WARNING_FORMAT_NONLITERAL
#    define RESTORE_WARNING_FORMAT_NONLITERAL
#endif

//! \def DISABLE_WARNING_CPP11_COMPAT
//! \brief Disables warning about using something that may collide with a part of the C++11 standard.
//! This is used around a null pointer type/class for C++98

//! \def RESTORE_WARNING_CPP11_COMPAT
//! \brief Restores warning about using something that may collide with a part of the C++11 standard.
//! This is used around a null pointer type/class for C++98
#if IS_CLANG_VERSION(1, 0)
#    define DISABLE_WARNING_CPP11_COMPAT                                                                               \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wc++0x-compat\"")
#    define RESTORE_WARNING_CPP11_COMPAT _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(4, 7)
#    define DISABLE_WARNING_CPP11_COMPAT                                                                               \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wc++11-compat\"")
#    define RESTORE_WARNING_CPP11_COMPAT _Pragma("GCC diagnostic pop")
#elif IS_GCC_FULL_VERSION(4, 3, 6)
#    define DISABLE_WARNING_CPP11_COMPAT                                                                               \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wc++0x-compat\"")
#    define RESTORE_WARNING_CPP11_COMPAT _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_WARNING_CPP11_COMPAT
#    define RESTORE_WARNING_CPP11_COMPAT
#endif

//! \def DISABLE_WARNING_FLOAT_EQUAL
//! \brief Disables warning about using == for comparing double/float values.
//! Generally you should never do this. There are rare uses which is why this needs to be able to be
//! disabled and restored in those rare cases.

//! \def RESTORE_WARNING_FLOAT_EQUAL
//! \brief Restores warning about using == for comparing double/float values.
//! Generally you should never do this. There are rare uses which is why this needs to be able to be
//! disabled and restored in those rare cases.
#if IS_CLANG_VERSION(3, 0)
#    define DISABLE_WARNING_FLOAT_EQUAL                                                                                \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wfloat-equal\"")
#    define RESTORE_WARNING_FLOAT_EQUAL _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(4, 5)
#    define DISABLE_WARNING_FLOAT_EQUAL                                                                                \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")
#    define RESTORE_WARNING_FLOAT_EQUAL _Pragma("GCC diagnostic pop")
#elif IS_GCC_FULL_VERSION(4, 3, 6)
#    define DISABLE_WARNING_FLOAT_EQUAL                                                                                \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")
#    define RESTORE_WARNING_FLOAT_EQUAL _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_WARNING_FLOAT_EQUAL
#    define RESTORE_WARNING_FLOAT_EQUAL
#endif

//! \def DISABLE_NONNULL_COMPARE
//! \brief Disables warning comparing a pointer to NULL when it is marked with an attribute as must be non-null.
//! This is used internally within safe_ functions to maintain null pointer checks for bounds checking

//! \def RESTORE_NONNULL_COMPARE
//! \brief Restores Disables warning comparing a pointer to NULL when it is marked with an attribute as must be
//! non-null. This is used internally within safe_ functions to maintain null pointer checks for bounds checking
#if IS_CLANG_VERSION(3, 2)
#    define DISABLE_NONNULL_COMPARE                                                                                    \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wtautological-pointer-compare\"")
#    define RESTORE_NONNULL_COMPARE _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(7, 1)
#    define DISABLE_NONNULL_COMPARE                                                                                    \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wnonnull-compare\"")
#    define RESTORE_NONNULL_COMPARE _Pragma("GCC diagnostic pop")
#elif IS_GCC_VERSION(4, 3)
#    define DISABLE_NONNULL_COMPARE _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wnonnull\"")
#    define RESTORE_NONNULL_COMPARE _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_NONNULL_COMPARE
#    define RESTORE_NONNULL_COMPARE
#endif

//! \def DISABLE_WARNING_UNDEF
//! \brief Disables warning about checking a macro in #if that is not defined.
//! Only use this when including external headers that cause this issue. This warning should not occur
//! within opensea-libs

//! \def RESTORE_WARNING_UNDEF
//! \brief Restores warning about checking a macro in #if that is not defined.
//! Only use this when including external headers that cause this issue. This warning should not occur
//! within opensea-libs
#if IS_CLANG_VERSION(2, 8)
#    define DISABLE_WARNING_UNDEF _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wundef\"")
#    define RESTORE_WARNING_UNDEF _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(3, 0)
#    define DISABLE_WARNING_UNDEF _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wundef\"")
#    define RESTORE_WARNING_UNDEF _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_WARNING_UNDEF
#    define RESTORE_WARNING_UNDEF
#endif

//! \def DISABLE_WARNING_INCOMPATIBLE_POINTER_TYPES
//! \brief Disables warning about incompatible pointer types

//! \def RESTORE_WARNING_INCOMPATIBLE_POINTER_TYPES
//! \brief Restored warning about incompatible pointer types
#if IS_CLANG_VERSION(3, 0) // unknown exact clang version
#    define DISABLE_WARNING_INCOMPATIBLE_POINTER_TYPES                                                                 \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wincompatible-pointer-types\"")
#    define RESTORE_WARNING_INCOMPATIBLE_POINTER_TYPES _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(5, 0)
#    define DISABLE_WARNING_INCOMPATIBLE_POINTER_TYPES                                                                 \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wincompatible-pointer-types\"")
#    define RESTORE_WARNING_INCOMPATIBLE_POINTER_TYPES _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_WARNING_INCOMPATIBLE_POINTER_TYPES
#    define RESTORE_WARNING_INCOMPATIBLE_POINTER_TYPES
#endif

#if defined(__cplusplus)
}

//! \def DISABLE_WARNING_OLD_STYLE_CAST
//! \brief Disables warning about checking a macro in #if that is not defined.
//! Only use this when including external headers that cause this issue. This warning should not occur
//! within opensea-libs

//! \def RESTORE_WARNING_OLD_STYLE_CAST
//! \brief Restores warning about checking a macro in #if that is not defined.
//! Only use this when including external headers that cause this issue. This warning should not occur
//! within opensea-libs
#    if IS_CLANG_VERSION(3, 0)
#        define DISABLE_WARNING_OLD_STYLE_CAST                                                                         \
            _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wold-style-cast\"")
#        define RESTORE_WARNING_OLD_STYLE_CAST _Pragma("clang diagnostic pop")
#    elif IS_GCC_VERSION(4, 2)
#        define DISABLE_WARNING_OLD_STYLE_CAST                                                                         \
            _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wold-style-cast\"")
#        define RESTORE_WARNING_OLD_STYLE_CAST _Pragma("GCC diagnostic pop")
#    else
#        define DISABLE_WARNING_OLD_STYLE_CAST
#        define RESTORE_WARNING_OLD_STYLE_CAST
#    endif
#endif //__cplusplus
