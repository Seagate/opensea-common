// SPDX-License-Identifier: MPL-2.0

//! \file code_attributes.h
//! \brief Defines attributes that the compilers can use to better generate
//! warnings or optimizations.
//!
//!        Also gives access to macros for fallthrough statements, marking
//!        variables as unused and more to help resolve warnings or specify
//!        expected behavior without a warning being generated.
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "predef_env_detect.h"

#if defined(_WIN32)
#    include <sal.h>
#endif //_WIN32

#if defined(__cplusplus)
extern "C"
{
#endif //__cplusplus

//! \def M_RESTRICT
//! \brief Defines the restrict keyword based on the compiler and C standard version.
//!
//! This macro defines the restrict keyword to optimize pointer usage.
//! If using C99 and MSVC 2005 without C11, it defines M_RESTRICT as __restrict.
//! Otherwise, it defines M_RESTRICT as restrict. If not using C99, it leaves M_RESTRICT undefined.
#if defined USING_C99
#    if IS_MSVC_VERSION(MSVC_2005) && !defined USING_C11
#        define M_RESTRICT __restrict
#    else
#        define M_RESTRICT restrict
#    endif
#else
#    define M_RESTRICT /*restrict*/
#endif

//! \def M_INLINE
//! \brief Defines the inline keyword based on the compiler and language standard.
//!
//! This macro defines the inline keyword to optimize function inlining.
//! - If using C++98, it defines M_INLINE as inline.
//! - If using C99, it defines M_INLINE as inline.
//! - If using MSVC 6.0, it defines M_INLINE as __inline.
//! - If using GCC version 2.5 or Clang version 1.0, it defines M_INLINE as __inline__.
//! - Otherwise, it leaves M_INLINE undefined, indicating inline support is not available.
#if defined(USING_CPP98)
#    define M_INLINE inline
#else // C
#    if defined(USING_C99)
#        define M_INLINE inline
#    elif IS_MSVC_VERSION(MSVC_6_0)
#        define M_INLINE __inline
#    elif IS_GCC_VERSION(2, 5) || IS_CLANG_VERSION(1, 0)
#        define M_INLINE __inline__
#    else
#        define M_INLINE /*inline support not available*/
#    endif
#endif // C++/C check

//! \def M_NOINLINE
//! \brief Defines the noinline attribute based on the compiler and language standard.
//!
//! This macro defines the noinline attribute to prevent function inlining.
//! - If using GCC version 2.5 or Clang version 1.0, it defines M_NOINLINE as __attribute__((noinline)).
//! - If using MSVC 6.0, it defines M_NOINLINE as __declspec(noinline).
//! - If the compiler supports __has_attribute and has the noinline attribute, it defines M_NOINLINE as M_INLINE
//! __attribute__((noinline)).
//! - Otherwise, it defines M_NOINLINE as M_INLINE or leaves it undefined if no support for noinline is available.
#if IS_GCC_VERSION(2, 5) || IS_CLANG_VERSION(1, 0)
#    define M_NOINLINE __attribute__((noinline))
#elif IS_MSVC_VERSION(MSVC_6_0)
#    define M_NOINLINE __declspec(noinline)
#elif defined __has_attribute
#    if __has_attribute(noinline)
#        define M_NOINLINE M_INLINE __attribute__((noinline))
#    else
#        define M_NOINLINE M_INLINE
#    endif
#else
#    define M_NOINLINE /*no support for noinline*/
#endif

//! \def M_FORCEINLINE
//! \brief Defines the always_inline attribute based on the compiler and language standard.
//!
//! This macro defines the always_inline attribute to force function inlining.
//! - If using GCC version 4.1.3 or Clang version 1.0, it defines M_FORCEINLINE as M_INLINE
//! __attribute__((always_inline)).
//! - If using MSVC 6.0, it defines M_FORCEINLINE as __forceinline.
//! - If the compiler supports __has_attribute and has the always_inline attribute, it defines M_FORCEINLINE as M_INLINE
//! __attribute__((always_inline)).
//! - Otherwise, it defines M_FORCEINLINE as M_INLINE, suggesting normal inlining if forcing inline is not supported.
#if IS_GCC_FULL_VERSION(4, 1, 3) || IS_CLANG_VERSION(1, 0)
#    define M_FORCEINLINE M_INLINE __attribute__((always_inline))
#elif IS_MSVC_VERSION(MSVC_6_0)
#    define M_FORCEINLINE __forceinline
#elif defined __has_attribute
#    if __has_attribute(always_inline)
#        define M_FORCEINLINE M_INLINE __attribute__((always_inline))
#    else
#        define M_FORCEINLINE M_INLINE
#    endif
#else
/*Support for forcing inline is not present, however lets try the "normal"
 * inline option to suggest it at least*/
#    define M_FORCEINLINE M_INLINE
#endif

//! \def M_FALLTHROUGH
//! \brief Defines the fallthrough attribute for switch-case statements to avoid warnings.
//!
//! This macro defines the fallthrough attribute differently based on the compiler and language standard.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For C++17 and later, it uses [[fallthrough]].
//! - For C23, it uses [[fallthrough]] or [[__fallthrough__]].
//! - For GCC and Clang, it uses __attribute__((fallthrough)).
//! - For MSVC, it uses __fallthrough from sal.h.
//! - If no support is detected, it inserts a comment to indicate fallthrough.
//!
//! \note Update this macro as errors are found to ensure compatibility with different compilers and standards.
//! \author TJE
#if defined(__cplusplus)
#    if defined USING_CPP11 && defined __has_cpp_attribute
#        if __has_cpp_attribute(fallthrough)
#            define M_FALLTHROUGH [[fallthrough]]
#        endif
#    endif
#elif defined __has_c_attribute
#    if __has_c_attribute(fallthrough)
#        define M_FALLTHROUGH [[fallthrough]]
#    elif __has_c_attribute(__fallthrough__)
#        define M_FALLTHROUGH [[__fallthrough__]]
#    endif
#endif
#if !defined(M_FALLTHROUGH)
#    if defined __has_attribute
#        if __has_attribute(fallthrough)
#            define M_FALLTHROUGH                                                                                      \
                do                                                                                                     \
                {                                                                                                      \
                } while (0);                                                                                           \
                __attribute__((fallthrough))
#        endif
#    endif
#    if !defined M_FALLTHROUGH
#        if (IS_GCC_VERSION(7, 0) || IS_CLANG_VERSION(3, 9))
#            define M_FALLTHROUGH                                                                                      \
                do                                                                                                     \
                {                                                                                                      \
                } while (0);                                                                                           \
                __attribute__((fallthrough))
#        elif defined(__fallthrough) /* from sal.h*/
#            define M_FALLTHROUGH __fallthrough
#        else
#            define M_FALLTHROUGH /*FALLTHRU*/
#        endif
#    endif
#endif // Checking C/C++

//! \def M_USE_UNUSED
//! \brief Marks a variable as used to avoid compiler warnings about unused variables.
//!
//! This macro is used within a function to mark a variable as used, preventing compiler warnings
//! about unused variables. It casts the variable to void.
//!
//! \param var The variable to mark as used.
//!
//! \code
//! void Func(int unusedVar)
//! {
//!     M_USE_UNUSED(unusedVar);
//!     // Function implementation
//! }
//! \endcode
//! \author TJE
#define M_USE_UNUSED(var) M_STATIC_CAST(void, var)

//! \def M_ATTR_UNUSED
//! \brief Marks a variable, function, or parameter as unused to avoid compiler warnings.
//!
//! This macro is used to mark variables, functions, or parameters as unused, preventing compiler warnings
//! about unused entities. It defines the attribute differently based on the compiler and language standard.
//! - For C++17 and later, it uses [[maybe_unused]].
//! - For C23, it uses [[maybe_unused]] or [[__maybe_unused__]].
//! - For GCC and Clang, it uses __attribute__((unused)).
//! - For MSVC, it uses __pragma(warning(suppress : 4100 4101)).
//! - If no support is detected, it inserts a comment to indicate unused.
//!
//! \code
//! void Func(int someVar)
//! {
//!     M_ATTR_UNUSED int thisIsntUsed = 0;
//! }
//!
//! void Func2(M_ATTR_UNUSED int unusedVar)
//! {
//!     // Function implementation
//! }
//!
//! M_ATTR_UNUSED void Func3(int param)
//! {
//!     // Function implementation
//! }
//! \endcode
//! \author TJE
#if defined(__cplusplus)
#    if __cplusplus >= 201103L && defined __has_cpp_attribute
#        if __has_cpp_attribute(maybe_unused)
#            define M_ATTR_UNUSED [[maybe_unused]]
#        endif
#    endif
#elif defined __has_c_attribute
#    if __has_c_attribute(maybe_unused)
#        define M_ATTR_UNUSED [[maybe_unused]]
#    elif __has_c_attribute(__maybe_unused__)
#        define M_ATTR_UNUSED [[__maybe_unused__]]
#    endif
#endif
#if !defined(M_ATTR_UNUSED)
#    if defined __has_attribute
#        if __has_attribute(unused)
#            define M_ATTR_UNUSED __attribute__((unused))
#        endif
#    endif
#    if !defined(M_ATTR_UNUSED)
#        if IS_GCC_FULL_VERSION(2, 95, 3) || IS_CLANG_VERSION(1, 0)
#            define M_ATTR_UNUSED __attribute__((unused))
#        elif defined(_MSC_VER)
#            define M_ATTR_UNUSED __pragma(warning(suppress : 4100 4101))
#        else
#            define M_ATTR_UNUSED /*UNUSED*/
#        endif
#    endif
#endif

//! \def M_DEPRECATED
//! \brief Marks functions, variables, or parameters as deprecated to avoid compiler warnings.
//!
//! This macro defines the deprecated attribute differently based on the compiler and language standard.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For C++17 and later, it uses [[deprecated]].
//! - For C23, it uses [[deprecated]] or [[__deprecated__]].
//! - For GCC and Clang, it uses __attribute__((deprecated)).
//! - For MSVC, it uses __declspec(deprecated) with a pragma to suppress warnings.
//! - If no support is detected, it inserts a comment to indicate deprecation.
//!
//! \note Update this macro as errors are found to ensure compatibility with different compilers and standards.
//! \author TJE
#if defined(__cplusplus)
#    if defined USING_CPP11 && defined __has_cpp_attribute
#        if __has_cpp_attribute(deprecated)
#            define M_DEPRECATED [[deprecated]]
#        endif
#    endif
#elif defined __has_c_attribute
#    if __has_c_attribute(deprecated)
#        define M_DEPRECATED [[deprecated]]
#    elif __has_c_attribute(__deprecated__)
#        define M_DEPRECATED [[__deprecated__]]
#    endif
#endif
#if !defined M_DEPRECATED
#    if defined __has_attribute
#        if __has_attribute(deprecated)
#            define M_DEPRECATED __attribute__((deprecated))
#        endif
#    endif
#    if !defined M_DEPRECATED
#        if (IS_GCC_VERSION(4, 0) || IS_CLANG_VERSION(3, 0))
#            define M_DEPRECATED __attribute__((deprecated))
#        elif IS_MSVC_VERSION(MSVC_2017_15_9)
#            define M_DEPRECATED __declspec(deprecated) __pragma(warning(suppress : 4996))
#        else
#            define M_DEPRECATED /*DEPRECATED*/
#        endif
#    endif
#endif

//! \def M_NODISCARD
//! \brief Marks functions, variables, or parameters as nodiscard to avoid compiler warnings.
//!
//! This macro defines the nodiscard attribute to indicate that the return value of a function should not be ignored.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For C++17 and later, it uses [[nodiscard]].
//! - For C23, it uses [[nodiscard]] or [[__nodiscard__]].
//! - For GCC and Clang, it uses __attribute__((nodiscard)) or __attribute__((warn_unused_result)).
//! - For MSVC, it uses _Check_return_ from sal.h.
//! - If no support is detected, it inserts a comment to indicate nodiscard.
//!
//! \note Update this macro as errors are found to ensure compatibility with different compilers and standards.
//! \author TJE
#if defined(__cplusplus)
#    if defined(USING_CPP11) && defined __has_cpp_attribute
#        if __has_cpp_attribute(nodiscard)
#            define M_NODISCARD [[nodiscard]]
#        endif
#    endif
#elif defined __has_c_attribute
#    if __has_c_attribute(nodiscard)
#        define M_NODISCARD [[nodiscard]]
#    elif __has_c_attribute(__nodiscard__)
#        define M_NODISCARD [[__nodiscard__]]
#    endif
#endif
#if !defined M_NODISCARD
#    if defined __has_attribute
#        if __has_attribute(nodiscard)
#            define M_NODISCARD __attribute__((nodiscard))
#        elif __has_attribute(warn_unused_result)
#            define M_NODISCARD __attribute__((warn_unused_result))
#        endif
#    endif
#    if !defined M_NODISCARD
#        if (IS_GCC_VERSION(3, 4) || IS_CLANG_VERSION(1, 0))
#            define M_NODISCARD __attribute__((warn_unused_result))
#        elif defined(_Check_return_) /*from sal.h*/
#            define M_NODISCARD _Check_return_
#        else
#            define M_NODISCARD /*NODISCARD*/
#        endif
#    endif
#endif

//! \def M_NORETURN
//! \brief Marks functions as noreturn to indicate they do not return.
//!
//! This macro defines the noreturn attribute to indicate that a function does not return.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For C++17 and later, it uses [[noreturn]].
//! - For C11, it uses the noreturn convenience macro.
//! - For C23, it uses [[noreturn]] or [[__noreturn__]] or [[___Noreturn__]].
//! - For GCC and Clang, it uses __attribute__((noreturn)).
//! - For MSVC, it uses __declspec(noreturn).
//! - If no support is detected, it inserts a comment to indicate noreturn.
//!
//! \note Update this macro as errors are found to ensure compatibility with different compilers and standards.
//! \author TJE
//!
//! Examples of standard library functions with noreturn: abort, exit, _Exit, quick_exit, thrd_exit, longjmp.
#if defined USING_C11 && !defined USING_C23
#    include <stdnoreturn.h>
#endif
#if defined(__cplusplus)
#    if defined(USING_CPP11) && defined __has_cpp_attribute
#        if __has_cpp_attribute(noreturn)
#            define M_NORETURN [[noreturn]]
#        endif
#    endif
#elif defined noreturn
#    define M_NORETURN noreturn
#elif defined __has_c_attribute
#    if __has_c_attribute(noreturn)
#        define M_NORETURN [[noreturn]]
#    elif __has_c_attribute(__noreturn__)
#        define M_NORETURN [[__noreturn__]]
#    elif __has_c_attribute(___Noreturn__)
#        define M_NORETURN [[___Noreturn__]]
#    endif
#endif
#if !defined M_NORETURN
#    if defined __has_attribute
#        if __has_attribute(noreturn)
#            define M_NORETURN __attribute__((noreturn))
#        endif
#    endif
#    if !defined M_NORETURN
#        if (IS_GCC_VERSION(2, 5) || IS_CLANG_VERSION(1, 0))
#            define M_NORETURN __attribute__((noreturn))
#        elif IS_MSVC_VERSION(MSVC_6_0)
#            define M_NORETURN __declspec(noreturn)
#        else
#            define M_NORETURN /*NORETURN*/
#        endif
#    endif
#endif

//! \def M_FUNC_ATTR_MALLOC
//! \brief Marks functions that allocate memory and return a pointer to it.
//!
//! This macro defines the malloc attribute to indicate that a function allocates memory and returns a pointer to it.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For Clang 1.0 and GCC 4.1.3, it uses __attribute__((malloc)).
//! - For MSVC 2015, it uses __declspec(allocator) __declspec(restrict).
//! - For SAL, it uses _Ret_opt_valid_.
//! - If no support is detected, it inserts a comment to indicate the function allocates memory.
//!
//! \note Update this macro as errors are found to ensure compatibility with different compilers and standards.
//! \author TJE
#if IS_CLANG_VERSION(1, 0) || IS_GCC_FULL_VERSION(4, 1, 3)
#    define M_FUNC_ATTR_MALLOC __attribute__((malloc))
#elif IS_MSVC_VERSION(MSVC_2015)
#    define M_FUNC_ATTR_MALLOC __declspec(allocator) __declspec(restrict)
#elif defined(_Ret_opt_valid_) /*sal*/
#    define M_FUNC_ATTR_MALLOC _Ret_opt_valid_
#else
#    define M_FUNC_ATTR_MALLOC /* this function allocates memory and returns the pointer to you */
#endif

//! \def FUNC_ATTR_PRINTF
//! \brief Marks functions that use printf-style formatting.
//!
//! This macro defines the format attribute to indicate that a function uses printf-style formatting.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For Clang 1.0 and GCC 2.5, it uses __attribute__((format(printf, formatargpos, varargpos))).
//! - For SAL, it uses _Format_string_impl_("printf", formatargpos).
//! - If no support is detected, it inserts a comment to indicate the function uses printf-style formatting.
//!
//! \param formatargpos The position of the format string argument.
//! \param varargpos The position of the variadic arguments.
//!
//! \note Update this macro as errors are found to ensure compatibility with different compilers and standards.
//! \author TJE
#if IS_CLANG_VERSION(1, 0) || IS_GCC_VERSION(2, 5)
#    define FUNC_ATTR_PRINTF(formatargpos, varargpos) __attribute__((format(printf, formatargpos, varargpos)))
#elif defined(_Format_string_impl_)
#    define FUNC_ATTR_PRINTF(formatargpos, varargpos) _Format_string_impl_("printf", formatargpos)
#else
#    define FUNC_ATTR_PRINTF(formatargpos, varargpos)    /* this is a printf/fprintf/etc style function. Please use a  \
                                                            user defined constant string for the format! */
#endif

    //_Printf_format_string_impl_ for SAL. Not sure how to use it yet so not defined
    // currently

//! \def DLL_EXPORT
//! \brief Marks symbols for export in a shared library.
//!
//! This macro defines the attribute to mark symbols for export in a shared library.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For MSVC 4.0 and later, it uses __declspec(dllexport).
//! - For GCC 4.0 and Clang 1.0, it uses __attribute__((visibility("default"))).
//! - If no support is detected, it inserts a comment to indicate no equivalent.
//!
//! \note Update this macro as errors are found to ensure compatibility with different compilers and standards.
//! \author TJE
#if IS_MSVC_VERSION(MSVC_4_0)
#    define DLL_EXPORT __declspec(dllexport)
#    define DLL_IMPORT __declspec(dllimport)
#elif IS_GCC_VERSION(4, 0) || IS_CLANG_VERSION(1, 0)
#    define DLL_EXPORT __attribute__((visibility("default")))
#    define DLL_IMPORT /*no gcc equivalent*/
#else
#    define DLL_EXPORT /* no equivalent */
#    define DLL_IMPORT /* no equivalent */
#endif

//! \def M_ALIGNOF
//! \brief Determines the alignment requirement of a type.
//!
//! This macro defines the attribute to determine the alignment requirement of a type.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For C++11 and C23, it uses alignof(x).
//! - For C11, it uses _Alignof(x).
//! - For GCC 2.7 and Clang 1.0, it uses __alignof__(x).
//! - For MSVC 2005, it uses __alignof(x).
//! - If no support is detected, it defines NO_ALIGNOF.
//!
//! \note Update this macro as errors are found to ensure compatibility with different compilers and standards.
//! \author TJE
#if defined(USING_CPP11) || defined(USING_C23)
#    define M_ALIGNOF(x) alignof(x)
#    define M_ALIGNAS(x) alignas(x)
#elif defined(USING_C11)
#    define M_ALIGNOF(x) _Alignof(x)
#    define M_ALIGNAS(x) _Alignas(x)
#else
#    if IS_GCC_VERSION(2, 7) || IS_CLANG_VERSION(1, 0)
#        define M_ALIGNOF(x) __alignof__(x)
#        define M_ALIGNAS(x) __attribute__((aligned(x)))
#    elif IS_MSVC_VERSION(MSVC_2005)
#        define M_ALIGNOF(x) __alignof(x)
#        define M_ALIGNAS(x) __declspec(align(x))
#    else
#        define NO_ALIGNOF
#        define NO_ALIGNAS
#    endif
#endif

#if defined(__cplusplus)
}
#endif
