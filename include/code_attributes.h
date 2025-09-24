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
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//! \see https://en.cppreference.com/w/cpp/language/attributes.html
//! \see https://en.cppreference.com/w/c/language/attributes.html
//! \see https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html
//! \see https://clang.llvm.org/docs/AttributeReference.html
//! \see https://learn.microsoft.com/en-us/cpp/cpp/attributes
//! \see https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Annotations-for-C-and-C++.html

#pragma once

#include "predef_env_detect.h"

#if defined(_WIN32)
#    include <sal.h>
#    define SAL_INCLUDED
#endif //_WIN32

#if defined(__cplusplus)
extern "C"
{
#endif //__cplusplus

//! \def __has_attribute
//! \brief Checks if a GNU style attribute is supported by the compiler.
//! \details This is defined if not already present to return 0.
//! By defining this is makes sure all compilers know what this is so that
//! it is cleaner to check for attributes.
//! The other reason to define this is some compilers may not correctly short circuit
//! #if checks that combine checking if this is defined and then using it to check for an attribute.
#if !defined(__has_attribute)
#    define __has_attribute(x) 0
#endif

//! \def DETECT_STD_ATTR_CHECK
//! \brief A wrapper for __has_c_attribute or __has_cpp_attribute depending on language.
//! To be compatible with MSVC (C mode, not C++), this check must be used before using DETECT_STD_ATTR for any
//! non-standard attributes (i.e. gnu::fallthrough, clang::fallthrough, msvc::fallthrough).
//! \code
//! #if defined(DETECT_STD_ATTR_CHECK)
//! #    if DETECT_STD_ATTR(clang::fallthrough)
//! ...do something
//! \endcode

//! \def DETECT_STD_ATTR
//! \brief Checks if a standardized attribute is supported by the compiler.
//! Checks C for C code and C++ for C++ code.
//! Only use with with standardized attributes.
//! For namespaced attributes, use DETECT_STD_ATTR_QUAL instead.
//! \code
//! #if defined(DETECT_STD_ATTR_CHECK)
//! #    if DETECT_STD_ATTR(fallthrough)
//!  ...
//! #    elif DETECT_STD_ATTR_QUAL(clang::fallthrough)
//! ...do something
//! #    endif
//! #endif
//! \endcode

//! \def DETECT_STD_ATTR_QUAL
//! \brief Checks if a namespace qualified attribute is supported by the compiler.
//! Checks C for C code and C++ for C++ code.
//! Only use with with namespaced attributes. For non-namespaced attributes, use DETECT_STD_ATTR instead.
//! \code
//! #if defined(DETECT_STD_ATTR_CHECK)
//! #    if DETECT_STD_ATTR(fallthrough)
//!  ...
//! #    elif DETECT_STD_ATTR_QUAL(clang::fallthrough)
//! ...do something
//! #    endif
//! #endif
//! \endcode
#if defined(FORCE_GNU_STYLE_ATTRIBUTES)
#    define DETECT_STD_ATTR(attr) 0
// do not define DETECT_STD_ATTR_CHECK here or the above examples fail
#else
#    if defined(__cplusplus) && defined(__has_cpp_attribute)
#        define DETECT_STD_ATTR(attr) __has_cpp_attribute(attr)
#        define DETECT_STD_ATTR_CHECK
#        define DETECT_STD_ATTR_QUAL(attr) __has_cpp_attribute(attr)
#    else
#        if defined(__has_c_attribute)
#            define DETECT_STD_ATTR(attr) __has_c_attribute(attr)
#            define DETECT_STD_ATTR_CHECK
#            if (defined(_MSC_VER) && !defined(__clang__)) || defined(NO_COMPILER_UNIQUE_C_ATTRIBUTES)
// MSVC has a bug where a namespaced attribute causes a parsing error for "unexpected token" on ':'.
// This is a workaround to bypass this problem. Eventually MSVC will probably fix this, and when that happens,
// this check can be updated with a version number to check the compiler against
// NOTE: OpenBSD with clang 16.0.6 also does not support this, so added NO_COMPILER_UNIQUE_C_ATTRIBUTES to work around
// this too.
#                define DETECT_STD_ATTR_QUAL(attr) 0
#
#            else
#                define DETECT_STD_ATTR_QUAL(attr) __has_c_attribute(attr)
#            endif
#        endif
#    endif
#endif // FORCE_GNU_STYLE_ATTRIBUTES

//! \def DETECT_GNU_ATTR
//! \brief Checks if a GNU style attribute is supported by the compiler.
//! If this is true, then the attribute should be defined as __attribute__((attr))
#define DETECT_GNU_ATTR(attr) __has_attribute(attr)

//! \def M_RESTRICT
//! \brief Defines the restrict keyword for pointer aliasing optimization.
//!
//! This macro defines M_RESTRICT to enable compiler optimizations by indicating
//! that pointers do not alias. It adapts based on compiler and language standard:
//! - In C99 or later, it uses `restrict`.
//! - In MSVC (2005+), it uses `__restrict`.
//! - In GCC or Clang, it uses `__restrict__`.
//! - If none apply, it leaves M_RESTRICT empty.
#if defined USING_C99
#    if IS_MSVC_VERSION(MSVC_2005) && !defined USING_C11
#        define M_RESTRICT __restrict
#    else
#        define M_RESTRICT restrict
#    endif
#else
#    if IS_GCC_VERSION(3, 1) || IS_CLANG_VERSION(1, 0)
#        define M_RESTRICT __restrict__
#    elif IS_MSVC_VERSION(MSVC_2005)
#        define M_RESTRICT __restrict
#    endif
#endif

#if !defined(M_RESTRICT)
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
#    endif
#endif // C++/C check

#if !defined(M_INLINE)
#    define M_INLINE /*inline support not available*/
#endif

//! \def M_NOINLINE
//! \brief Defines the noinline attribute based on the compiler and language standard.
//!
//! This macro defines the noinline attribute to prevent function inlining.
//! - If using GCC version 2.5 or Clang version 1.0, it defines M_NOINLINE as __attribute__((noinline)).
//! - If using MSVC 6.0, it defines M_NOINLINE as __declspec(noinline).
//! - If the compiler supports __has_attribute and has the noinline attribute, it defines M_NOINLINE as M_INLINE
//! __attribute__((noinline)).
//! - Otherwise, it defines M_NOINLINE as M_INLINE or leaves it undefined if no support for noinline is available.
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::noinline)
#            define M_NOINLINE [[clang::noinline]]
#        elif DETECT_STD_ATTR_QUAL(gnu::noinline)
#            define M_NOINLINE [[gnu::noinline]]
#        elif DETECT_STD_ATTR_QUAL(msvc::noinline)
#            define M_NOINLINE [[msvc::noinline]]
#        endif
#    else
#        if DETECT_GNU_ATTR(noinline) || (IS_GCC_VERSION(2, 5) || IS_CLANG_VERSION(1, 0))
#            define M_NOINLINE __attribute__((noinline))
#        elif IS_MSVC_VERSION(MSVC_6_0)
#            define M_NOINLINE __declspec(noinline)
#        endif
#    endif
#endif

#if !defined(M_NOINLINE)
#    define M_NOINLINE /*inline support not available*/
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
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::always_inline)
#            define M_FORCEINLINE [[clang::always_inline]] M_INLINE
#        elif DETECT_STD_ATTR_QUAL(gnu::always_inline)
#            define M_FORCEINLINE [[gnu::always_inline]] M_INLINE
#        elif DETECT_STD_ATTR_QUAL(msvc::forceinline)
#            define M_FORCEINLINE [[msvc::forceinline]]
#        endif
#    else
#        if DETECT_GNU_ATTR(always_inline) || (IS_GCC_FULL_VERSION(4, 1, 3) || IS_CLANG_VERSION(1, 0))
#            define M_FORCEINLINE __attribute__((always_inline)) M_INLINE
#        elif IS_MSVC_VERSION(MSVC_6_0)
#            define M_FORCEINLINE __forceinline
#        endif
#    endif
#endif

#if !defined(M_FORCEINLINE)
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
#if defined(DETECT_STD_ATTR_CHECK)
#    if DETECT_STD_ATTR(fallthrough)
#        define M_FALLTHROUGH [[fallthrough]]
#    elif DETECT_STD_ATTR(__fallthrough__)
#        define M_FALLTHROUGH [[__fallthrough__]]
#    elif DETECT_STD_ATTR_QUAL(clang::fallthrough)
#        define M_FALLTHROUGH [[clang::fallthrough]]
#    elif DETECT_STD_ATTR_QUAL(gcc::fallthrough)
#        define M_FALLTHROUGH [[gcc::fallthrough]]
#    endif
#else
#    if DETECT_GNU_ATTR(fallthrough) || (IS_GCC_VERSION(7, 0) || IS_CLANG_VERSION(3, 9))
#        define M_FALLTHROUGH                                                                                          \
            do                                                                                                         \
            {                                                                                                          \
            } while (0);                                                                                               \
            __attribute__((fallthrough))
#    elif defined(SAL_INCLUDED) && defined(__fallthrough) /* from sal.h*/
#        define M_FALLTHROUGH __fallthrough
#    endif
#endif

#if !defined(M_FALLTHROUGH)
#    define M_FALLTHROUGH /*FALLTHRU*/
#endif

    //! \def M_ATTR_UNUSED
    //! \brief Marks a variable, function, or parameter as unused to avoid compiler warnings.
    //!
    //! This macro is used to mark variables, functions, or parameters as unused, preventing compiler warnings
    //! about unused entities. It defines the attribute differently based on the compiler and language standard.
    //! - For C++17 and later, it uses [[maybe_unused]].
    //! - For C23, it uses [[maybe_unused]] or [[__maybe_unused__]].
    //! - For GCC and Clang, it uses __attribute__((unused)).
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
#if defined(DETECT_STD_ATTR_CHECK)
#    if DETECT_STD_ATTR(maybe_unused)
#        define M_ATTR_UNUSED [[maybe_unused]]
#    elif DETECT_STD_ATTR(__maybe_unused__)
#        define M_ATTR_UNUSED [[__maybe_unused__]]
#    elif DETECT_STD_ATTR_QUAL(clang::unused)
#        define M_ATTR_UNUSED [[clang::unused]]
#    elif DETECT_STD_ATTR_QUAL(gnu::unused)
#        define M_ATTR_UNUSED [[gnu::unused]]
#    endif
#else
#    if DETECT_GNU_ATTR(unused) || (IS_GCC_FULL_VERSION(2, 95, 3) || IS_CLANG_VERSION(1, 0))
#        define M_ATTR_UNUSED __attribute__((unused))
#    endif
#endif

#if !defined(M_ATTR_UNUSED)
#    define M_ATTR_UNUSED /*UNUSED*/
#endif

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
#if defined(_MSC_VER) && !defined(__clang__)
#    define M_USE_UNUSED(var)                                                                                          \
        do                                                                                                             \
        {                                                                                                              \
            MSVC_PRAGMA(warning(suppress : 4100 4101));                                                                \
            M_STATIC_CAST(void, var);                                                                                  \
        } while (0)
#else
#    define M_USE_UNUSED(var) M_STATIC_CAST(void, var)
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
//! \def M_DEPRECATED_REASON(msg)
//! \brief Marks functions, variables, or parameters as deprecated with a custom reason.
//!
//! This macro defines the deprecated attribute with a message, adapting to compiler and language support.
//! - For C++17 and C23, it uses [[deprecated("message")]].
//! - For GCC and Clang, it uses __attribute__((deprecated("message"))).
//! - For MSVC, it uses __declspec(deprecated("message")).
//! - If no support is detected, it inserts a comment to indicate deprecation.
//!
//! \author TJE
#if defined(DETECT_STD_ATTR_CHECK)
#    if DETECT_STD_ATTR(deprecated)
#        define M_DEPRECATED             [[deprecated]]
#        define M_DEPRECATED_REASON(msg) [[deprecated(msg)]]
#    elif DETECT_STD_ATTR(__deprecated__)
#        define M_DEPRECATED             [[__deprecated__]]
#        define M_DEPRECATED_REASON(msg) [[__deprecated__(msg)]]
#    elif DETECT_STD_ATTR_QUAL(clang::deprecated)
#        define M_DEPRECATED             [[clang::deprecated]]
#        define M_DEPRECATED_REASON(msg) [[clang::deprecated(msg)]]
#    elif DETECT_STD_ATTR_QUAL(gnu::deprecated)
#        define M_DEPRECATED             [[gnu::deprecated]]
#        define M_DEPRECATED_REASON(msg) [[gnu::deprecated(msg)]]
#    endif
#else
#    if DETECT_GNU_ATTR(deprecated) || (IS_GCC_VERSION(4, 0) || IS_CLANG_VERSION(3, 0))
#        define M_DEPRECATED             __attribute__((deprecated))
#        define M_DEPRECATED_REASON(msg) __attribute__((deprecated(msg)))
#    elif IS_MSVC_VERSION(MSVC_2017_15_9)
#        define M_DEPRECATED             __declspec(deprecated) MSVC_PRAGMA(warning(suppress : 4996))
#        define M_DEPRECATED_REASON(msg) __declspec(deprecated(msg)) MSVC_PRAGMA(warning(suppress : 4996))
#    endif
#endif

#if !defined(M_DEPRECATED)
#    define M_DEPRECATED /*DEPRECATED*/
#endif

#if !defined(M_DEPRECATED_REASON)
#    define M_DEPRECATED_REASON(msg) /*DEPRECATED: msg*/
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
//! \note This attribute must be used before other attributes are added otherwise compilation errors may occur.
//! \author TJE
#if defined(DETECT_STD_ATTR_CHECK)
#    if DETECT_STD_ATTR(nodiscard)
#        define M_NODISCARD [[nodiscard]]
#    elif DETECT_STD_ATTR(__nodiscard__)
#        define M_NODISCARD [[__nodiscard__]]
#    elif DETECT_STD_ATTR_QUAL(clang::nodiscard)
#        define M_NODISCARD [[clang::nodiscard]]
#    elif DETECT_STD_ATTR_QUAL(gnu::nodiscard)
#        define M_NODISCARD [[gnu::nodiscard]]
#    elif DETECT_STD_ATTR_QUAL(clang::warn_unused_result)
#        define M_NODISCARD [[clang::warn_unused_result]]
#    elif DETECT_STD_ATTR_QUAL(gnu::warn_unused_result)
#        define M_NODISCARD [[gnu::warn_unused_result]]
#    endif
#else
#    if DETECT_GNU_ATTR(nodiscard)
#        define M_NODISCARD __attribute__((nodiscard))
#    elif DETECT_GNU_ATTR(warn_unused_result) || (IS_GCC_VERSION(3, 4) || IS_CLANG_VERSION(1, 0))
#        define M_NODISCARD __attribute__((warn_unused_result))
#    elif defined(SAL_INCLUDED) && defined(_Check_return_) /*from sal.h*/
#        define M_NODISCARD _Check_return_
#    endif
#endif

#if !defined(M_NODISCARD)
#    define M_NODISCARD /*NODISCARD*/
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
#if defined(__has_include)
#    if __has_include(<stdnoreturn.h>)
#        include <stdnoreturn.h>
#    endif
#elif defined USING_C11 && !defined USING_C23
#    if IS_MSVC_VERSION(MSVC_2019_16_8) /* Need new enough Windows SDK for this to be available - TJE */
#        if defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN10_20348
#            include <stdnoreturn.h>
#        endif
#    elif !defined(_MSC_VER)
#        include <stdnoreturn.h>
#    endif
#endif

#if defined(DETECT_STD_ATTR_CHECK)
#    if DETECT_STD_ATTR(noreturn)
#        define M_NORETURN [[noreturn]]
#    elif DETECT_STD_ATTR(__noreturn__)
#        define M_NORETURN [[__noreturn__]]
#    elif DETECT_STD_ATTR(_Noreturn)
#        define M_NORETURN [[_Noreturn]]
#    elif DETECT_STD_ATTR(___Noreturn__)
#        define M_NORETURN [[___Noreturn__]]
#    elif DETECT_STD_ATTR_QUAL(clang::noreturn)
#        define M_NORETURN [[clang::noreturn]]
#    elif DETECT_STD_ATTR_QUAL(gnu::noreturn)
#        define M_NORETURN [[gnu::noreturn]]
#    endif
#else
#    if defined(noreturn)
#        define M_NORETURN noreturn
#    elif DETECT_GNU_ATTR(noreturn) || (IS_GCC_VERSION(2, 5) || IS_CLANG_VERSION(1, 0))
#        define M_NORETURN __attribute__((noreturn))
#    elif IS_MSVC_VERSION(MSVC_6_0)
#        define M_NORETURN __declspec(noreturn)
#    endif
#endif

#if !defined(M_NORETURN)
#    define M_NORETURN /*NORETURN*/
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
//! \note Use only with malloc and calloc type functions. Do not use on realloc.

//! \def M_ALLOC_DEALLOC
//! \brief Marks functions which function should be used to deallocate memory with an allocating function
//! \param deallocatorFunc name of the function to deallocate memory
//! \param argpos position in the deallocator function that the pointer should be placed in to perform the deallocation
//! \note Use only with malloc and calloc type functions. Do not use on realloc.
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::malloc)
#            define M_FUNC_ATTR_MALLOC [[clang::malloc]]
#        elif DETECT_STD_ATTR_QUAL(gnu::malloc)
#            define M_FUNC_ATTR_MALLOC [[gnu::malloc]]
#            if IS_GCC_VERSION(11, 0)
#                define M_ALLOC_DEALLOC(deallocatorFunc, argpos) [[gnu::malloc(deallocatorFunc, argpos)]]
#            endif
#        endif
#    else
#        if DETECT_GNU_ATTR(malloc) || IS_CLANG_VERSION(1, 0) || IS_GCC_FULL_VERSION(4, 1, 3)
#            define M_FUNC_ATTR_MALLOC __attribute__((malloc))
#            if IS_GCC_VERSION(11, 0)
#                define M_ALLOC_DEALLOC(deallocatorFunc, argpos) __attribute__((malloc(deallocatorFunc, argpos)))
#            endif
#        elif IS_MSVC_VERSION(MSVC_2015)
#            define M_FUNC_ATTR_MALLOC __declspec(allocator) __declspec(restrict)
#        elif defined(_Ret_opt_valid_) /*sal*/
#            define M_FUNC_ATTR_MALLOC _Ret_opt_valid_
#        endif
#    endif
#endif

#if !defined(M_FUNC_ATTR_MALLOC)
#    define M_FUNC_ATTR_MALLOC /* this function allocates memory and returns the pointer to you */
#endif

#if !defined(M_ALLOC_DEALLOC)
#    define M_ALLOC_DEALLOC(                                                                                           \
        deallocatorFunc,                                                                                               \
        argpos) /* This marks the deallocation function needed by this allocator to be used properly */
#endif

// TODO: Investigate more about clang's similar allocation attributes:
// ownership_holds, ownership_returns, ownership_takes (Clang Static Analyzer)
// https://clang.llvm.org/docs/AttributeReference.html#ownership-holds-ownership-returns-ownership-takes-clang-static-analyzer

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

//! \def FUNC_ATTR_SCANF
//! \brief Marks functions that use scanf-style formatting.
//!
//! This macro defines the format attribute to indicate that a function uses scanf-style formatting.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For Clang 1.0 and GCC 2.5, it uses __attribute__((format(scanf, formatargpos, varargpos))).
//! - For SAL, it uses _Format_string_impl_("scanf", formatargpos).
//! - If no support is detected, it inserts a comment to indicate the function uses scanf-style formatting.
//!
//! \param formatargpos The position of the format string argument.
//! \param varargpos The position of the variadic arguments.
//!
//! \note Update this macro as errors are found to ensure compatibility with different compilers and standards.
//! \author TJE
//! \note Only use with scanf, not scanf_s type functions!

//! \def FUNC_ATTR_SCANF_S
//! \brief Marks functions that use scanf_s-style formatting.
//!
//! This macro defines the format attribute to indicate that a function uses scanf_s-style formatting.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For Clang 1.0 and GCC 2.5, it uses __attribute__((format(scanf, formatargpos, varargpos))).
//! - For SAL, it uses _Format_string_impl_("scanf_s", formatargpos).
//! - If no support is detected, it inserts a comment to indicate the function uses scanf_s-style formatting.
//!
//! \param formatargpos The position of the format string argument.
//! \param varargpos The position of the variadic arguments.
//!
//! \note Update this macro as errors are found to ensure compatibility with different compilers and standards.
//! \author TJE
//! \note Only use with scanf_s, not scanf type functions!
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::format)
#            define FUNC_ATTR_PRINTF(formatargpos, varargpos)  [[clang::format(printf, formatargpos, varargpos)]]
#            define FUNC_ATTR_SCANF(formatargpos, varargpos)   [[clang::format(scanf, formatargpos, varargpos)]]
#            define FUNC_ATTR_SCANF_S(formatargpos, varargpos) [[clang::format(scanf, formatargpos, varargpos)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::format)
#            define FUNC_ATTR_PRINTF(formatargpos, varargpos)  [[gnu::format(printf, formatargpos, varargpos)]]
#            define FUNC_ATTR_SCANF(formatargpos, varargpos)   [[gnu::format(scanf, formatargpos, varargpos)]]
#            define FUNC_ATTR_SCANF_S(formatargpos, varargpos) [[gnu::format(scanf, formatargpos, varargpos)]]
#        endif
#    else
#        if DETECT_GNU_ATTR(format) || (IS_GCC_VERSION(2, 5) || IS_CLANG_VERSION(1, 0))
#            define FUNC_ATTR_PRINTF(formatargpos, varargpos)  __attribute__((format(printf, formatargpos, varargpos)))
#            define FUNC_ATTR_SCANF(formatargpos, varargpos)   __attribute__((format(scanf, formatargpos, varargpos)))
#            define FUNC_ATTR_SCANF_S(formatargpos, varargpos) __attribute__((format(scanf, formatargpos, varargpos)))
#        elif defined(SAL_INCLUDED)
#            if defined(_Printf_format_string_params_)
#                define FUNC_ATTR_PRINTF(formatargpos, varargpos) _Printf_format_string_params_(formatargpos)
#            endif
#            if defined(_Scanf_format_string_params_)
#                define FUNC_ATTR_SCANF(formatargpos, varargpos) _Scanf_format_string_params_(formatargpos)
#            endif
#            if defined(_Scanf_s_format_string_params_)
#                define FUNC_ATTR_SCANF_S(formatargpos, varargpos) _Scanf_s_format_string_params_(formatargpos)
#            endif
#        endif
#    endif
#endif

#if !defined(FUNC_ATTR_PRINTF)
#    define FUNC_ATTR_PRINTF(formatargpos, varargpos)    /* this is a printf/fprintf/etc style function. Please use a  \
                                                            user defined constant string for the format! */
#endif

#if !defined(FUNC_ATTR_SCANF)
#    define FUNC_ATTR_SCANF(formatargpos, varargpos)     /* this is a scanf style function. Please use a               \
                                                             user defined constant string for the format! */
#endif

#if !defined(FUNC_ATTR_SCANF_S)
#    define FUNC_ATTR_SCANF_S(formatargpos, varargpos)   /* this is a scanf_s style function. Please use a             \
                                                           user defined constant string for the format! */
#endif

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
//! \see https://gcc.gnu.org/onlinedocs/gcc/Microsoft-Windows-Function-Attributes.html
#if defined(DETECT_STD_ATTR_CHECK)
#    if DETECT_STD_ATTR_QUAL(msvc::dllexport) && DETECT_STD_ATTR_QUAL(msvc::dllimport)
#        define DLL_EXPORT [[msvc::dllexport]]
#        define DLL_IMPORT [[msvc::dllimport]]
#    elif DETECT_STD_ATTR_QUAL(clang::dllexport) && DETECT_STD_ATTR_QUAL(clang::dllimport)
#        define DLL_EXPORT [[clang::dllexport]]
#        define DLL_IMPORT [[clang::dllimport]]
#    elif DETECT_STD_ATTR_QUAL(gnu::dllexport) && DETECT_STD_ATTR_QUAL(gnu::dllimport)
#        define DLL_EXPORT [[gnu::dllexport]]
#        define DLL_IMPORT [[gnu::dllimport]]
#    elif DETECT_STD_ATTR_QUAL(clang::visibility)
#        define DLL_EXPORT [[clang::visibility("default")]]
#    elif DETECT_STD_ATTR_QUAL(gnu::visibility)
#        define DLL_EXPORT [[gnu::visibility("default")]]
#    endif
#else
#    if DETECT_GNU_ATTR(dllexport) && DETECT_GNU_ATTR(dllimport)
#        define DLL_EXPORT __attribute__((dllexport))
#        define DLL_IMPORT __attribute__((dllimport))
#    elif IS_MSVC_VERSION(MSVC_4_0)
#        define DLL_EXPORT __declspec(dllexport)
#        define DLL_IMPORT __declspec(dllimport)
#    elif DETECT_GNU_ATTR(visibility) || (IS_GCC_VERSION(4, 0) || IS_CLANG_VERSION(1, 0))
#        define DLL_EXPORT __attribute__((visibility("default")))
#    endif
#endif

#if !defined(DLL_EXPORT)
#    define DLL_EXPORT /* no equivalent */
#endif
#if !defined(DLL_IMPORT)
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
#    if IS_MSVC_VERSION(MSVC_2019_16_8) /* Need new enough Windows SDK for this to be available - TJE */
#        if defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN10_20348
#            include <stdalign.h>
#            define M_ALIGNOF(x) _Alignof(x)
#            define M_ALIGNAS(x) _Alignas(x)
#        endif
#    elif !defined(_MSC_VER)
#        include <stdalign.h>
#        define M_ALIGNOF(x) _Alignof(x)
#        define M_ALIGNAS(x) _Alignas(x)
#    endif
#endif
#if !defined(M_ALIGNOF)
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

//! \def M_ALL_PARAMS_NONNULL
//! \brief Marks all pointer parameters of a function as non-null.
//!
//! This macro defines the nonnull attribute to indicate that all pointer parameters of a function must be non-null.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For GCC 3.3 and later, it uses __attribute__((nonnull)).
//! - If no support is detected, it inserts a comment to indicate the non-null requirement.
//!
//! \code
//! M_ALL_PARAMS_NONNULL void example_function(int *param1, char *param2);
//! \endcode
//!
//! \sa M_NONNULL_PARAM_LIST, M_PARAM_RO, M_PARAM_WO, M_PARAM_RW
//! \note This attribute is only used in debug builds (_DEBUG defined). This is due to how this
//! attribute gets handled during optimizations and may cause crashes by removing checks for NULL that a
//! function may still be performing (such as safe_memcpy). When developing in debug mode this will help
//! generate warnings about incorrect usage while preserving runtime checks in optimized released builds

//! \def M_NONNULL_PARAM_LIST(...)
//! \brief Marks specific pointer parameters of a function as non-null.
//!
//! This macro defines the nonnull attribute to indicate that specific pointer parameters of a function must be
//! non-null. The arguments to this macro are the indices of the parameters (starting from 1).
//! - For GCC 3.3 and later, it uses __attribute__((nonnull(__VA_ARGS__))).
//! - If no support is detected, it inserts a comment to indicate the non-null requirement.
//!
//! \param ... The indices of the parameters that must be non-null.
//!
//! \code
//! M_NONNULL_PARAM_LIST(1, 2) void example_function(int *param1, char *param2);
//! \endcode
//!
//! \sa M_ALL_PARAMS_NONNULL, M_PARAM_RO, M_PARAM_WO, M_PARAM_RW
//! \note This attribute is only used in debug builds (_DEBUG defined). This is due to how this
//! attribute gets handled during optimizations and may cause crashes by removing checks for NULL that a
//! function may still be performing (such as safe_memcpy). When developing in debug mode this will help
//! generate warnings about incorrect usage while preserving runtime checks in optimized released builds
#if defined(_DEBUG) && !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::nonnull)
#            define M_ALL_PARAMS_NONNULL      [[clang::nonnull]]
#            define M_NONNULL_PARAM_LIST(...) [[clang::nonnull(__VA_ARGS__)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::nonnull)
#            define M_ALL_PARAMS_NONNULL      [[gnu::nonnull]]
#            define M_NONNULL_PARAM_LIST(...) [[gnu::nonnull(__VA_ARGS__)]]
#        endif
#    else
#        if DETECT_GNU_ATTR(nonnull) || IS_GCC_VERSION(3, 3)
#            define M_ALL_PARAMS_NONNULL      __attribute__((nonnull))
#            define M_NONNULL_PARAM_LIST(...) __attribute__((nonnull(__VA_ARGS__)))
#        endif
#    endif
#endif

#if !defined(M_ALL_PARAMS_NONNULL)
#    define M_ALL_PARAMS_NONNULL /*NONNULL*/
#endif

#if !defined(M_NONNULL_PARAM_LIST)
#    define M_NONNULL_PARAM_LIST(...) /*NONNULL*/
#endif

//! \def M_NONNULL_IF_NONZERO_SIZE(arg, sizearg)
//! \brief Marks a pointer parameter as non-null if a corresponding integral parameter is non-zero.
//!
//! This macro defines the nonnull_if_nonzero attribute to indicate that a pointer parameter must be non-null if a
//! corresponding integral parameter is non-zero. It attempts to provide a compatible definition for various compilers
//! and standards.
//! - For GCC 12 and later, it uses __attribute__((nonnull_if_nonzero(arg, sizearg))).
//! - If no support is detected, it inserts a comment to indicate the conditional non-null requirement.
//!
//! \param arg The index of the pointer parameter.
//! \param sizearg The index of the integral parameter.
//!
//! \code
//! M_NONNULL_IF_NONZERO_SIZE(2, 1) void example_function(int size, char *buffer);
//! \endcode
//!
//! \sa M_ALL_PARAMS_NONNULL, M_NONNULL_PARAM_LIST, M_PARAM_RO, M_PARAM_WO, M_PARAM_RW

//! \def M_NONNULL_IF_NONZERO_SIZE_COUNT(arg, sizearg, countarg)
//! \brief Marks a pointer parameter as non-null if a corresponding integral parameter is non-zero. This version is
//! useful for functions where total size is a combination of size and count, such as fread, to determine correct
//! sizing.
//!
//! This macro defines the nonnull_if_nonzero attribute to indicate that a pointer parameter must be non-null if a
//! corresponding integral parameter is non-zero. It attempts to provide a compatible definition for various compilers
//! and standards.
//! - For GCC 16 and later, it uses __attribute__((nonnull_if_nonzero(arg, sizearg, countarg))).
//! - If no support is detected, it inserts a comment to indicate the conditional non-null requirement.
//!
//! \param arg The index of the pointer parameter.
//! \param sizearg The index of the integral parameter.
//!
//! \code
//! M_NONNULL_IF_NONZERO_SIZE_COUNT(3, 1, 2) void example_function(int size, int count, char *buffer);
//! \endcode
//!
//! \sa M_ALL_PARAMS_NONNULL, M_NONNULL_PARAM_LIST, M_PARAM_RO, M_PARAM_WO, M_PARAM_RW
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::nonnull_if_nonzero)
#            define M_NONNULL_IF_NONZERO_SIZE(arg, sizearg) [[clang::nonnull_if_nonzero(arg, sizearg)]]
#            define M_NONNULL_IF_NONZERO_SIZE_COUNT(arg, sizearg, countarg)                                            \
                [[clang::nonnull_if_nonzero(arg, sizearg)]] [[clang::nonnull_if_nonzero(arg, countarg)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::nonnull_if_nonzero)
#            define M_NONNULL_IF_NONZERO_SIZE(arg, sizearg) [[gnu::nonnull_if_nonzero(arg, sizearg)]]
#            if IS_GCC_VERSION(16, 0)
#                define M_NONNULL_IF_NONZERO_SIZE_COUNT(arg, sizearg, countarg)                                        \
                    [[gnu::nonnull_if_nonzero(arg, sizearg, countarg)]]
#            else
#                define M_NONNULL_IF_NONZERO_SIZE_COUNT(arg, sizearg, countarg)                                        \
                    [[gnu::nonnull_if_nonzero(arg, sizearg)]] [[gnu::nonnull_if_nonzero(arg, countarg)]]
#            endif
#        endif
#    else
#        if DETECT_GNU_ATTR(nonnull_if_nonzero)
#            define M_NONNULL_IF_NONZERO_SIZE(arg, sizearg) __attribute__((nonnull_if_nonzero(arg, sizearg)))
#            if IS_GCC_VERSION(16, 0)
#                define M_NONNULL_IF_NONZERO_SIZE_COUNT(arg, sizearg, countarg)                                        \
                    __attribute__((nonnull_if_nonzero(arg, sizearg, countarg)))
#            else
#                define M_NONNULL_IF_NONZERO_SIZE_COUNT(arg, sizearg, countarg)                                        \
                    __attribute__((nonnull_if_nonzero(arg, sizearg))) __attribute__((nonnull_if_nonzero(arg, countarg)))
#            endif
#        endif
#    endif
#endif

#if !defined(M_NONNULL_IF_NONZERO_SIZE)
#    define M_NONNULL_IF_NONZERO_SIZE(arg, sizearg) /*NONNULL_IF_SIZE_IS_NONZERO*/
#endif

//! \def M_NONNULL_IF_NONZERO_PARAM
//! \brief Use M_NONNULL_IF_NONZERO_SIZE instead. This is a helper to transition to this name
#define M_NONNULL_IF_NONZERO_PARAM M_NONNULL_IF_NONZERO_SIZE

#if !defined(M_NONNULL_IF_NONZERO_SIZE_COUNT)
#    define M_NONNULL_IF_NONZERO_SIZE_COUNT(arg, sizearg, countarg) /*NONNULL_IF_NONZERO_SIZE_COUNT*/
#endif

//! \def M_NULL_TERM_STRING(arg)
//! \brief Marks a parameter as a null-terminated string.
//!
//! This macro defines the null_terminated_string_arg attribute to indicate that a parameter is a null-terminated
//! string. It attempts to provide a compatible definition for various compilers and standards.
//! - If no support is detected, it inserts a comment to indicate the null-terminated string requirement.
//!
//! \param arg The index of the parameter that is a null-terminated string.
//!
//! \code
//! M_NULL_TERM_STRING(1) void example_function(char *str);
//! \endcode
//!
//! \sa M_ALL_PARAMS_NONNULL, M_NONNULL_PARAM_LIST, M_NONNULL_IF_NONZERO_PARAM, M_PARAM_RO, M_PARAM_WO, M_PARAM_RW
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::null_terminated_string_arg)
#            define M_NULL_TERM_STRING(arg) [[clang::null_terminated_string_arg(arg)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::null_terminated_string_arg)
#            define M_NULL_TERM_STRING(arg) [[gnu::null_terminated_string_arg(arg)]]
#        endif
#    else
#        if DETECT_GNU_ATTR(null_terminated_string_arg)
#            define M_NULL_TERM_STRING(arg) __attribute__((null_terminated_string_arg(arg)))
#        endif
#    endif
#endif

#if !defined(M_NULL_TERM_STRING)
#    define M_NULL_TERM_STRING(arg) /*NULL_TERMINATED_STRING_ARGUMENT*/
#endif

//! \def M_RETURNS_NONNULL
//! \brief Marks a function as returning a non-null pointer.
//!
//! This macro defines the returns_nonnull attribute to indicate that a function returns a non-null pointer.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - For GCC, it uses __attribute__((returns_nonnull)).
//! - For SAL, it uses _Ret_notnull_.
//! - If no support is detected, it inserts a comment to indicate the non-null return requirement.
//!
//! \code
//! M_RETURNS_NONNULL char* example_function(void);
//! \endcode
//!
//! \sa M_ALL_PARAMS_NONNULL, M_NONNULL_PARAM_LIST, M_NONNULL_IF_NONZERO_PARAM, M_NULL_TERM_STRING, M_PARAM_RO,
//! M_PARAM_WO, M_PARAM_RW
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::returns_nonnull)
#            define M_RETURNS_NONNULL [[clang::returns_nonnull]]
#        elif DETECT_STD_ATTR_QUAL(gnu::returns_nonnull)
#            define M_RETURNS_NONNULL [[gnu::returns_nonnull]]
#        endif
#    else
#        if DETECT_GNU_ATTR(returns_nonnull)
#            define M_RETURNS_NONNULL __attribute__((returns_nonnull))
#        elif defined(SAL_INCLUDED) && defined(_Ret_notnull_) /* SAL.h */
#            define M_RETURNS_NONNULL _Ret_notnull_
#        endif
#    endif
#endif

#if !defined(M_RETURNS_NONNULL)
#    define M_RETURNS_NONNULL /*Function returns non-null pointer*/
#endif

//! \def M_PARAM_RO(arg)
//! \brief Marks a parameter as read-only.
//!
//! This macro defines the access(read_only) attribute to indicate that a parameter is read-only.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - If no support is detected, it inserts a comment to indicate the read-only requirement.
//!
//! \param arg The index of the parameter that is read-only.
//!
//! \code
//! M_PARAM_RO(1) void example_function(const char *buffer);
//! \endcode
//!
//! \sa M_PARAM_WO, M_PARAM_RW, M_PARAM_RO_SIZE, M_PARAM_WO_SIZE, M_PARAM_RW_SIZE, M_ALL_PARAMS_NONNULL,
//! M_NONNULL_PARAM_LIST, M_NONNULL_IF_NONZERO_PARAM, M_NULL_TERM_STRING

//! \def M_PARAM_WO(arg)
//! \brief Marks a parameter as write-only.
//!
//! This macro defines the access(write_only) attribute to indicate that a parameter is write-only.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - If no support is detected, it inserts a comment to indicate the write-only requirement.
//!
//! \param arg The index of the parameter that is write-only.
//!
//! \code
//! M_PARAM_WO(1) void example_function(char *buffer);
//! \endcode
//!
//! \sa M_PARAM_RO, M_PARAM_RW, M_PARAM_RO_SIZE, M_PARAM_WO_SIZE, M_PARAM_RW_SIZE, M_ALL_PARAMS_NONNULL,
//! M_NONNULL_PARAM_LIST, M_NONNULL_IF_NONZERO_PARAM, M_NULL_TERM_STRING

//! \def M_PARAM_RW(arg)
//! \brief Marks a parameter as read-write.
//!
//! This macro defines the access(read_write) attribute to indicate that a parameter is read-write.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - If no support is detected, it inserts a comment to indicate the read-write requirement.
//!
//! \param arg The index of the parameter that is read-write.
//!
//! \code
//! M_PARAM_RW(1) void example_function(char *buffer);
//! \endcode
//!
//! \sa M_PARAM_RO, M_PARAM_WO, M_PARAM_RO_SIZE, M_PARAM_WO_SIZE, M_PARAM_RW_SIZE, M_ALL_PARAMS_NONNULL,
//! M_NONNULL_PARAM_LIST, M_NONNULL_IF_NONZERO_PARAM, M_NULL_TERM_STRING

//! \def M_PARAM_RO_SIZE(arg, sizearg)
//! \brief Marks a parameter as read-only with a specified size.
//!
//! This macro defines the access(read_only) attribute to indicate that a parameter is read-only with a specified size.
//! It attempts to provide a compatible definition for various compilers and standards.
//! - If no support is detected, it inserts a comment to indicate the read-only requirement with size.
//!
//! \param arg The index of the parameter that is read-only.
//! \param sizearg The index of the parameter that specifies the size.
//!
//! \code
//! M_PARAM_RO_SIZE(1, 2) void example_function(const char *buffer, size_t size);
//! \endcode
//!
//! \sa M_PARAM_RO, M_PARAM_WO, M_PARAM_RW, M_PARAM_WO_SIZE, M_PARAM_RW_SIZE, M_ALL_PARAMS_NONNULL,
//! M_NONNULL_PARAM_LIST, M_NONNULL_IF_NONZERO_PARAM, M_NULL_TERM_STRING

//! \def M_PARAM_WO_SIZE(arg, sizearg)
//! \brief Marks a parameter as write-only with a specified size.
//!
//! This macro defines the access(write_only) attribute to indicate that a parameter is write-only with a specified
//! size. It attempts to provide a compatible definition for various compilers and standards.
//! - If no support is detected, it inserts a comment to indicate the write-only requirement with size.
//!
//! \param arg The index of the parameter that is write-only.
//! \param sizearg The index of the parameter that specifies the size.
//!
//! \code
//! M_PARAM_WO_SIZE(1, 2) void example_function(char *buffer, size_t size);
//! \endcode
//!
//! \sa M_PARAM_RO, M_PARAM_WO, M_PARAM_RW, M_PARAM_RO_SIZE, M_PARAM_RW_SIZE, M_ALL_PARAMS_NONNULL,
//! M_NONNULL_PARAM_LIST, M_NONNULL_IF_NONZERO_PARAM, M_NULL_TERM_STRING

//! \def M_PARAM_RW_SIZE(arg, sizearg)
//! \brief Marks a parameter as read-write with a specified size.
//!
//! This macro defines the access(read_write) attribute to indicate that a parameter is read-write with a specified
//! size. It attempts to provide a compatible definition for various compilers and standards.
//! - If no support is detected, it inserts a comment to indicate the read-write requirement with size.
//!
//! \param arg The index of the parameter that is read-write.
//! \param sizearg The index of the parameter that specifies the size.
//!
//! \code
//! M_PARAM_RW_SIZE(1, 2) void example_function(char *buffer, size_t size);
//! \endcode
//!
//! \sa M_PARAM_RO, M_PARAM_WO, M_PARAM_RW, M_PARAM_RO_SIZE, M_PARAM_WO_SIZE, M_ALL_PARAMS_NONNULL,
//! M_NONNULL_PARAM_LIST, M_NONNULL_IF_NONZERO_PARAM, M_NULL_TERM_STRING
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::access)
#            define M_PARAM_RO(arg)               [[clang::access(read_only, arg)]]
#            define M_PARAM_WO(arg)               [[clang::access(write_only, arg)]]
#            define M_PARAM_RW(arg)               [[clang::access(read_write, arg)]]
#            define M_PARAM_RO_SIZE(arg, sizearg) [[clang::access(read_only, arg, sizearg)]]
#            define M_PARAM_WO_SIZE(arg, sizearg) [[clang::access(write_only, arg, sizearg)]]
#            define M_PARAM_RW_SIZE(arg, sizearg) [[clang::access(read_write, arg, sizearg)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::access)
#            define M_PARAM_RO(arg)               [[gnu::access(read_only, arg)]]
#            define M_PARAM_WO(arg)               [[gnu::access(write_only, arg)]]
#            define M_PARAM_RW(arg)               [[gnu::access(read_write, arg)]]
#            define M_PARAM_RO_SIZE(arg, sizearg) [[gnu::access(read_only, arg, sizearg)]]
#            define M_PARAM_WO_SIZE(arg, sizearg) [[gnu::access(write_only, arg, sizearg)]]
#            define M_PARAM_RW_SIZE(arg, sizearg) [[gnu::access(read_write, arg, sizearg)]]
#        endif
#    else
#        if DETECT_GNU_ATTR(access)
#            define M_PARAM_RO(arg)               __attribute__((access(read_only, arg)))
#            define M_PARAM_WO(arg)               __attribute__((access(write_only, arg)))
#            define M_PARAM_RW(arg)               __attribute__((access(read_write, arg)))
#            define M_PARAM_RO_SIZE(arg, sizearg) __attribute__((access(read_only, arg, sizearg)))
#            define M_PARAM_WO_SIZE(arg, sizearg) __attribute__((access(write_only, arg, sizearg)))
#            define M_PARAM_RW_SIZE(arg, sizearg) __attribute__((access(read_write, arg, sizearg)))
#        endif
#    endif
#endif

#if !defined(M_PARAM_RO)
#    define M_PARAM_RO(arg) /* PARAM_ACCESS_RO */
#endif
#if !defined(M_PARAM_WO)
#    define M_PARAM_WO(arg) /* PARAM_ACCESS_WO */
#endif
#if !defined(M_PARAM_RW)
#    define M_PARAM_RW(arg) /* PARAM_ACCESS_RW */
#endif
#if !defined(M_PARAM_RO_SIZE)
#    define M_PARAM_RO_SIZE(arg, sizearg) /* PARAM_ACCESS_RO_SIZE */
#endif
#if !defined(M_PARAM_WO_SIZE)
#    define M_PARAM_WO_SIZE(arg, sizearg) /* PARAM_ACCESS_WO_SIZE */
#endif
#if !defined(M_PARAM_RW_SIZE)
#    define M_PARAM_RW_SIZE(arg, sizearg) /* PARAM_ACCESS_RW_SIZE */
#endif

//! \def M_ALLOC_ALIGN
//! \brief Marks a function as allocating aligned memory with alignment in a specified position
//!
//! \code
//! M_ALLOC_ALIGN(1) char* example_function(alignment);
//! \endcode
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::alloc_align)
#            define M_ALLOC_ALIGN(alignmentArg) [[clang::alloc_align(alignmentArg)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::alloc_align)
#            define M_ALLOC_ALIGN(alignmentArg) [[gnu::alloc_align(alignmentArg)]]
#        endif
#    else
#        if DETECT_GNU_ATTR(alloc_align) || (IS_GCC_VERSION(4, 9) || IS_CLANG_VERSION(3, 7))
#            define M_ALLOC_ALIGN(alignmentArg) __attribute__((alloc_align(alignmentArg)))
#        endif
#    endif
#endif

#if !defined(M_ALLOC_ALIGN)
#    define M_ALLOC_ALIGN(alignmentArg)  /* This allocates aligned memory with alignment at alignmentArg position      \
                                          */
#endif

//! \def M_MALLOC_SIZE
//! \brief Marks a function as allocating memory of a size specified in a specific parameter position
//!
//! \code
//! M_MALLOC_SIZE(1) char* example_function(size);
//! \endcode

//! \def M_CALLOC_SIZE
//! \brief Marks a function as allocating memory of a size specified in a 2 positions; one for
//! number of elements and one for the size of the elements, similar to the calloc function
//!
//! \code
//! M_CALLOC_SIZE(1, 2) char* example_function(elements, size);
//! \endcode
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::alloc_size)
#            define M_MALLOC_SIZE(sizearg)             [[clang::alloc_size(sizearg)]]
#            define M_CALLOC_SIZE(elementarg, sizearg) [[clang::alloc_size(elementarg, sizearg)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::alloc_size)
#            define M_MALLOC_SIZE(sizearg)             [[gnu::alloc_size(sizearg)]]
#            define M_CALLOC_SIZE(elementarg, sizearg) [[gnu::alloc_size(elementarg, sizearg)]]
#        endif
#    else
#        if DETECT_GNU_ATTR(alloc_size) || (IS_GCC_VERSION(4, 9) || IS_CLANG_VERSION(3, 7))
#            define M_MALLOC_SIZE(sizearg)             __attribute__((alloc_size(sizearg)))
#            define M_CALLOC_SIZE(elementarg, sizearg) __attribute__((alloc_size(elementarg, sizearg)))
#        endif
#    endif
#endif

#if !defined(M_MALLOC_SIZE)
#    define M_MALLOC_SIZE(sizearg) /* allocates memory of size specified in sizearg */
#endif

#if !defined(M_CALLOC_SIZE)
#    define M_CALLOC_SIZE(elementarg, sizearg) /* allocates memory of size specified by elementarg * sizearg */
#endif

//! \def M_FILE_DESCRIPTOR
//! \brief Marks a parameter in a function as being a unix file descriptor (int fd)
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::fd_arg)
#            define M_FILE_DESCRIPTOR(argnum) [[clang::fd_arg(argnum)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::fd_arg)
#            define M_FILE_DESCRIPTOR(argnum) [[gnu::fd_arg(argnum)]]
#        endif
#    else
#        if DETECT_GNU_ATTR(fd_arg)
#            define M_FILE_DESCRIPTOR(argnum) __attribute__((fd_arg(argnum)))
#        endif
#    endif
#endif
#if !defined(M_FILE_DESCRIPTOR)
#    define M_FILE_DESCRIPTOR(argnum)  /* parameter # argnum is an integer file descriptor number that must be opened  \
                                        */
#endif

//! \def M_FILE_DESCRIPTOR_R
//! \brief Marks a parameter in a function as being a unix file descriptor (int fd) and that the function may read from
//! it
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::fd_arg_read)
#            define M_FILE_DESCRIPTOR_R(argnum) [[clang::fd_arg_read(argnum)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::fd_arg_read)
#            define M_FILE_DESCRIPTOR_R(argnum) [[gnu::fd_arg_read(argnum)]]
#        endif
#    else
#        if DETECT_GNU_ATTR(fd_arg_read)
#            define M_FILE_DESCRIPTOR_R(argnum) __attribute__((fd_arg_read(argnum)))
#        endif
#    endif
#endif
#if !defined(M_FILE_DESCRIPTOR_R)
#    define M_FILE_DESCRIPTOR_R(argnum)    /* parameter # argnum is an integer file descriptor number that must be opened \
                                              with read permissions */
#endif

//! \def M_FILE_DESCRIPTOR_W
//! \brief Marks a parameter in a function as being a unix file descriptor (int fd) and that the function may write to
//! it
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::fd_arg_write)
#            define M_FILE_DESCRIPTOR_W(argnum) [[clang::fd_arg_write(argnum)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::fd_arg_write)
#            define M_FILE_DESCRIPTOR_W(argnum) [[gnu::fd_arg_write(argnum)]]
#        endif
#    else
#        if DETECT_GNU_ATTR(fd_arg_write)
#            define M_FILE_DESCRIPTOR_W(argnum) __attribute__((fd_arg_write(argnum)))
#        endif
#    endif
#endif
#if !defined(M_FILE_DESCRIPTOR_W)
#    define M_FILE_DESCRIPTOR_W(argnum)    /* parameter # argnum is an integer file descriptor number that must be opened \
                                              with write permissions */
#endif

#if defined(__cplusplus)
}
#endif
