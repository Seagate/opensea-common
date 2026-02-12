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

//! \def DETECT_STD_ATTR_CHECK
//! \brief A wrapper for __has_c_attribute or __has_cpp_attribute depending on language.
//! To be compatible with MSVC (C mode, not C++), this check must be used before using DETECT_STD_ATTR for any
//! non-standard attributes (i.e. gnu::fallthrough, clang::fallthrough, msvc::fallthrough).
//! \code
//! #if defined(DETECT_STD_ATTR_CHECK)
//! #    if DETECT_STD_ATTR(fallthrough)
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
#    endif
#    if !defined(M_NOINLINE)
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
#    endif
#    if !defined(M_FORCEINLINE)
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
#endif
#if !defined(M_FALLTHROUGH)
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
#endif
#if !defined(M_ATTR_UNUSED)
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
#endif
#if !defined(M_DEPRECATED)
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

//! \def M_ENUM_DEPRECATED
//! \brief Marks enum values as deprecated to generate a compiler warning when it is used.
//! \code
//! M_DECLARE_ENUM_TYPE(MyEnum, int,
//!     VALUE_ONE = 1,
//!     VALUE_TWO M_ENUM_DEPRECATED = 2,
//!     VALUE_THREE M_ENUM_DEPRECATED_REASON("Use VALUE_ONE instead") = 3
//! );
#if __has_extension(enumerator_attributes) || defined(__cpp_enumerator_attributes)
#    define M_ENUM_DEPRECATED             M_DEPRECATED
#    define M_ENUM_DEPRECATED_REASON(msg) M_DEPRECATED_REASON(msg)
#endif // __has_extension(enumerator_attributes)

#if !defined(M_ENUM_DEPRECATED)
#    define M_ENUM_DEPRECATED /*ENUM DEPRECATED*/
#endif
#if !defined(M_ENUM_DEPRECATED_REASON)
#    define M_ENUM_DEPRECATED_REASON(msg) /*ENUM DEPRECATED FOR REASON: msg*/
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

//! \def M_NODISCARD_REASON
//! \brief Marks functions, variables, or parameters as nodiscard to avoid compiler warnings.
//! This version allows specifying a reason for the no discard to be emitted. This will only work when the language
//! supports a reason, otherwise this is the same as M_NODISCARD.
//!
//!
//! \note This attribute must be used before other attributes are added otherwise compilation errors may occur.
//! \author TJE
#if defined(DETECT_STD_ATTR_CHECK)
#    if DETECT_STD_ATTR(nodiscard)
#        define M_NODISCARD [[nodiscard]]
#        if defined USING_CPP20 && DETECT_STD_ATTR(nodiscard) >= 201907L
#            define M_NODISCARD_REASON(msg) [[nodiscard(msg)]]
#        elif DETECT_STD_ATTR(nodiscard) >= 202003L
#            define M_NODISCARD_REASON(msg) [[nodiscard(msg)]]
#        endif
#    elif DETECT_STD_ATTR(__nodiscard__)
#        define M_NODISCARD [[__nodiscard__]]
#        if DETECT_STD_ATTR(__nodiscard__) >= 202003L
#            define M_NODISCARD_REASON(msg) [[__nodiscard__(msg)]]
#        endif
#    elif DETECT_STD_ATTR_QUAL(clang::nodiscard)
#        define M_NODISCARD [[clang::nodiscard]]
#    elif DETECT_STD_ATTR_QUAL(gnu::nodiscard)
#        define M_NODISCARD [[gnu::nodiscard]]
#    elif DETECT_STD_ATTR_QUAL(clang::warn_unused_result)
#        define M_NODISCARD [[clang::warn_unused_result]]
#    elif DETECT_STD_ATTR_QUAL(gnu::warn_unused_result)
#        define M_NODISCARD [[gnu::warn_unused_result]]
#    endif
#endif
#if !defined(M_NODISCARD)
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

#if !defined(M_NODISCARD_REASON)
#    define M_NODISCARD_REASON(msg) M_NODISCARD /*msg not available*/
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
#endif
#if !defined(M_NORETURN)
#    if DETECT_GNU_ATTR(noreturn) || (IS_GCC_VERSION(2, 5) || IS_CLANG_VERSION(1, 0))
#        define M_NORETURN __attribute__((noreturn))
#    elif IS_MSVC_VERSION(MSVC_6_0)
#        define M_NORETURN __declspec(noreturn)
#    elif defined(noreturn)
#        define M_NORETURN noreturn
#    elif defined(SAL_INCLUDED) && defined(_Check_return_) /*from sal.h*/
#        define M_NORETURN _Check_return_
#    endif
#endif

#if !defined(M_NORETURN)
#    define M_NORETURN /*NORETURN*/
#endif

//! \def M_UNREACHABLE()
//! \brief A macro that inserts an unreachable function to help the compiler optimize code paths.
//!
//! \details This macro should be used in places where the code should never reach. This is
//! useful for optimizations only and does not replace error handling or eliminate warnings.
//! For C++23 it uses std::unreachable() if available.
//! For C23 it uses the unreachable convenience macro if available.
//! For GCC and Clang it uses __builtin_unreachable().
//! For MSVC it uses __assume(0).
//! If no support is detected, it defines a simple unreachable function that does nothing.
//! \code
//! switch(value)
//! {
//!     case 1:
//!         ...
//!         break;
//!     case 2:
//!         ...
//!         break;
//!     default:
//!         M_UNREACHABLE();
//!         break;
//! }
//! \endcode
//! \code
//! enum SomeEnum { thing1, thing2 };
//! switch(value)
//! {
//!     case thing1:
//!         ...
//!         return;
//!     case thing2:
//!         ...
//!         return;
//! }
//! M_UNREACHABLE();
//! \endcode
//! \code
//! int someFunction(int value)
//! {
//!     if(value == 1)
//!     {
//!         abort();
//!         M_UNREACHABLE();
//!     }
//!     do more stuff...
//! \endcode
#if defined(USING_CPP23) && defined(__cpp_lib_unreachable) && (__cpp_lib_unreachable >= 202202L)
#    include <utility> // for std::unreachable
#    define M_UNREACHABLE() (std::unreachable())
#elif defined(unreachable) // C23
#    define M_UNREACHABLE() (unreachable())
#elif defined(HAVE_BUILT_IN_UNREACHABLE) || (IS_CLANG_VERSION(3, 2) || IS_GCC_VERSION(4, 5))
#    define M_UNREACHABLE() (__builtin_unreachable())
#elif IS_MSVC_VERSION(MSVC_2003)
#    define M_UNREACHABLE() (__assume(0))
#else
M_NORETURN M_INLINE void unreachable_func(void)
{
}
#    define M_UNREACHABLE() (unreachable_func())
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
#    endif
#    if !defined(M_FUNC_ATTR_MALLOC)
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
#    endif
#    if !defined(FUNC_ATTR_PRINTF)
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
#endif
#if !defined(DLL_EXPORT)
#    if DETECT_GNU_ATTR(dllexport) && DETECT_GNU_ATTR(dllimport)
#        define DLL_EXPORT __attribute__((dllexport))
#        define DLL_IMPORT __attribute__((dllimport))
#    elif IS_MSVC_VERSION(MSVC_4_0)
#        define DLL_EXPORT __declspec(dllexport)
#        define DLL_IMPORT __declspec(dllimport)
#    elif DETECT_GNU_ATTR(visibility) || (IS_GCC_VERSION(4, 0) || IS_CLANG_VERSION(1, 0))
#        define DLL_EXPORT __attribute__((visibility("default")))
#        define DLL_IMPORT __attribute__((visibility("default")))
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
#elif defined(USING_C11) || __has_feature(c_alignas) || __has_extension(c_alignas)
#    if (__has_feature(c_alignas) || __has_extension(c_alignas)) &&                                                    \
        (__has_feature(c_alignof) || __has_extension(c_alignof))
#        define M_ALIGNOF(x) _Alignof(x)
#        define M_ALIGNAS(x) _Alignas(x)
#    elif IS_MSVC_VERSION(MSVC_2019_16_8) /* Need new enough Windows SDK for this to be available - TJE */
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

    // Example for nullability annotations
    //! \def M_NULLABLE, M_NONNULL, M_NULL_UNSPECIFIED
    //! \brief Marks pointers with nullability annotations for better static analysis and documentation.
    //!
    //! This macro defines nullability annotations to indicate whether pointers can be null or not.
    //! \code
    //! M_NULLABLE char *nullableString;          // This pointer can be null
    //! M_NONNULL char *nonNullableString;        // This pointer cannot be null
    //! M_NULL_UNSPECIFIED char *unspecifiedString; // Nullability is unspecified
    //! \endcode

#if __has_feature(nullability) || __has_extension(nullability)
#    define M_NULLABLE         _Nullable
#    define M_NONNULL          _Nonnull
#    define M_NULL_UNSPECIFIED _Null_unspecified
#    define M_NONNULL_ARRAY    static
#endif // __has_feature(nullability)

#if !defined(M_NULLABLE)
#    define M_NULLABLE /* NULLABLE */
#endif

#if !defined(M_NONNULL)
#    define M_NONNULL /* NONNULL  */
#endif
#if !defined(M_NULL_UNSPECIFIED)
#    define M_NULL_UNSPECIFIED /* NULL_UNSPECIFIED */
#endif
#if !defined(M_NONNULL_ARRAY)
#    if !defined(REAL_MSVC) && defined(USING_C99)
// MSVC does not support the syntax [static N] for array parameters for unknown reasons.
#        define M_NONNULL_ARRAY static
#    else
#        define M_NONNULL_ARRAY /* NONNULL ARRAY */
#    endif
#endif

//! \def M_ALL_PARAMS_NONNULL
//! \brief Marks all pointer parameters of a function as non-null. THIS SHOULD ONLY BE USED IN
//! internal functions where you can guarantee that all parameters will never be null. The optimizer
//! will see this and remove all null pointer checks for these functions, which may lead to crashes if null.
//! For null warnings, use M_NONNULL above for clang or the M_DIAG_WARNING M_DIAG_ERROR checks
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

//! \def M_NONNULL_PARAM_LIST(...)
//! \brief Marks specific pointer parameters of a function as non-null.  THIS SHOULD ONLY BE USED IN
//! internal functions where you can guarantee that all parameters will never be null. The optimizer
//! will see this and remove all null pointer checks for these functions, which may lead to crashes if null.
//! For null warnings, use M_NONNULL above for clang or the M_DIAG_WARNING M_DIAG_ERROR checks
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
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::nonnull)
#            define M_ALL_PARAMS_NONNULL      [[clang::nonnull]]
#            define M_NONNULL_PARAM_LIST(...) [[clang::nonnull(__VA_ARGS__)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::nonnull)
#            define M_ALL_PARAMS_NONNULL      [[gnu::nonnull]]
#            define M_NONNULL_PARAM_LIST(...) [[gnu::nonnull(__VA_ARGS__)]]
#        endif
#    endif
#    if !defined(M_ALL_PARAMS_NONNULL)
#        if DETECT_GNU_ATTR(nonnull) || IS_GCC_VERSION(3, 3)
#            define M_ALL_PARAMS_NONNULL      __attribute__((nonnull))
#            define M_NONNULL_PARAM_LIST(...) __attribute__((nonnull(__VA_ARGS__)))
#        endif
#    endif
#endif

#if !defined(M_ALL_PARAMS_NONNULL)
#    define M_ALL_PARAMS_NONNULL /*NONNULL PARAMS*/
#endif

#if !defined(M_NONNULL_PARAM_LIST)
#    define M_NONNULL_PARAM_LIST(...) /*NONNULL PARAMS*/
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
#    endif
#    if !defined(M_NONNULL_IF_NONZERO_SIZE)
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
#    endif
#    if !defined(M_NULL_TERM_STRING)
#        if DETECT_GNU_ATTR(null_terminated_string_arg)
#            define M_NULL_TERM_STRING(arg) __attribute__((null_terminated_string_arg(arg)))
#        endif
#    endif
#endif

#if !defined(M_NULL_TERM_STRING)
#    define M_NULL_TERM_STRING(arg) /*NULL_TERMINATED_STRING_ARGUMENT*/
#endif

//! \def M_DIAG_IF(condition, message, level)
//! \brief If supported, uses clang's diagnose_if attribute to generate errors or
//! warnings based on the provided condition
//! \param condition the condition to evaluate
//! \param message the message to display if this condition does not pass
//! \param level set to "warning" for a warning to be emitted and "error"
//! to generate a compilation error for the condition
//! \note This attribute must appear AFTER a function definition, not before it unlike
//! (most) other attributes which may be used in either location
//! \note All evaluations must be constant and cannot include casts. Functions can be used, but if they
//! do not produce a constant result, then they are ignored and no diagnostic is provided.
//! \note Can be used to check for M_NULLPTR for void* only like myvoidptr == M_NULLPTR
//! \note To check for other types of null pointers, such as a string, do either (!strptr) or in C23
//! constexpr const char* nullstr = M_NULLPTR; strptr == nullstr
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::diagnose_if)
#            define M_DIAGNOSE_IF(condition, message, level) [[clang::diagnose_if((condition), message, level)]]
#            define HAVE_DIAGNOSE_IF
#        elif DETECT_STD_ATTR_QUAL(gnu::diagnose_if)
#            define M_DIAGNOSE_IF(condition, message, level) [[gnu::diagnose_if((condition), message, level)]]
#            define HAVE_DIAGNOSE_IF
#        endif
#    endif
#    if !defined(M_DIAGNOSE_IF)
#        if DETECT_GNU_ATTR(diagnose_if)
#            define M_DIAGNOSE_IF(condition, message, level) __attribute__((diagnose_if((condition), message, level)))
#            define HAVE_DIAGNOSE_IF
#        endif
#    endif
#endif

#if !defined(M_DIAGNOSE_IF)
#    define M_DIAGNOSE_IF(condition, message, level /*DIAGNOSE_IF_NOT_SUPPORTED*/)
#endif

//! \def M_DIAG_WARNING(condition, message)
//! \brief Convenience macro for M_DIAGNOSE_IF to generate a warning
#define M_DIAG_WARNING(condition, message) M_DIAGNOSE_IF((condition), message, "warning")

//! \def M_DIAG_WARN(condition, message)
//! \brief Convenience macro for M_DIAGNOSE_IF to generate a warning
#define M_DIAG_WARN(condition, message) M_DIAG_WARNING((condition), message)

//! \def M_DIAG_ERROR(condition, message)
//! \brief Convenience macro for M_DIAGNOSE_IF to generate an error
#define M_DIAG_ERROR(condition, message) M_DIAGNOSE_IF((condition), message, "error")

//! \def M_DIAG_ERR(condition, message)
//! \brief Convenience macro for M_DIAGNOSE_IF to generate an error
#define M_DIAG_ERR(condition, message) M_DIAG_ERROR((condition), message)

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
#    endif
#    if !defined(M_RETURNS_NONNULL)
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
#    endif
#    if !defined(M_PARAM_RO)
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
#    endif
#    if !defined(M_ALLOC_ALIGN)
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
#    endif
#    if !defined(M_MALLOC_SIZE)
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
#    endif
#    if !defined(M_FILE_DESCRIPTOR)
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
#    endif
#    if !defined(M_FILE_DESCRIPTOR_R)
#        if DETECT_GNU_ATTR(fd_arg_read)
#            define M_FILE_DESCRIPTOR_R(argnum) __attribute__((fd_arg_read(argnum)))
#        endif
#    endif
#endif
#if !defined(M_FILE_DESCRIPTOR_R)
#    define M_FILE_DESCRIPTOR_R(argnum)    /* parameter # argnum is an integer file descriptor number that must be     \
                                              opened    with read permissions */
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
#    endif
#    if !defined(M_FILE_DESCRIPTOR_W)
#        if DETECT_GNU_ATTR(fd_arg_write)
#            define M_FILE_DESCRIPTOR_W(argnum) __attribute__((fd_arg_write(argnum)))
#        endif
#    endif
#endif
#if !defined(M_FILE_DESCRIPTOR_W)
#    define M_FILE_DESCRIPTOR_W(argnum)    /* parameter # argnum is an integer file descriptor number that must be     \
                                              opened    with write permissions */
#endif

//! \def M_STRICT_FLEX_ARRAY(level)
//! \brief Used to mark a flexible array parameter at the end of a structure with a specific level
//! of strictness for the compiler to evaluate.
//! This overrides the compiler's strict flex array evaluation when used on an individual struct.
//! For example, when compiling at level 2
//! \param level The level of strictness (0, 1, or 2) for the flexible array
//! \see https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html
//! \note level 0: for all trailing arrays at the end of a structure
//! \note level 1: for [], [0], and [1] flexible arrays
//! \note level 2: for [] or [0] flexible arrays only
//! \note level 3: for []  flexible arrays only as per C99 and later standards
//! \note [0] is a GNU extension.
//! \code
//! struct example_struct {
//!     int count;
//!     M_COUNTED_BY(count) M_STRICT_FLEX_ARRAY(1) char data[1];
//! };
//! \endcode
//! \code
//! struct example_struct {
//!     int count;
//!     M_COUNTED_BY(count) M_STRICT_FLEX_ARRAY(2)) char data[0];
//! };
//! \endcode
//! \code
//! struct example_struct {
//!     int count;
//!     M_COUNTED_BY(count) M_STRICT_FLEX_ARRAY(3)) char data[];
//! };
//! \endcode
//! \see M_STRICT_FLEX_ARRAY_AUTO

//! \def FLEX_ARRAY
//! \brief Defines the appropriate flexible array declaration based on the detected C standard and compiler
//! \details For C99 and up this expands to [] as per the standard. For older C standards, it expands to [0] or [1]
//! depending on compiler support. Can be adjuster per-compiler as needed with workarounds.

//! \def FLEX_LEVEL
//! \brief defines the flexible array level for M_STRICT_FLEX_ARRAY based on detected C standard and compiler
//! \details This is set to 3 for C11 and up, 1 for older C99 compilers without full support, and 2 for GCC/Clang with
//! extensions
#if defined(USING_C11)
#    define FLEX_ARRAY
#    define FLEX_LEVEL 3
#elif defined(USING_C99)
#    if IS_GCC_VERSION(3, 0) || IS_CLANG_VERSION(1, 0) || IS_MSVC_VERSION(MSVC_2015)
// Flex arrays at the end of structure in C99 mode works in these compilers
#        define FLEX_ARRAY
#        define FLEX_LEVEL 3
#    else // other compilers. While assuming C99 VLA would be great, this is probably safer.
#        define FLEX_ARRAY 1
#        define FLEX_LEVEL 1
#    endif
#else
#    if defined(__GNUC__) || defined(__clang__)
// GCC supported VLAs as an extension very early on.
// Rather
#        define FLEX_ARRAY 0
#        define FLEX_LEVEL 2
#    else
#        define FLEX_ARRAY 1
#        define FLEX_LEVEL 1
#    endif
#endif

//! \def FLEX_ARRAY_ANY
//! \brief Level 0 for M_STRICT_FLEX_ARRAY: for all trailing arrays at the end of a structure
#define FLEX_ARRAY_ANY (0)

//! \def FLEX_ARRAY_COMMON
//! \brief Level 1 for M_STRICT_FLEX_ARRAY: for [], [0], and [1] flexible arrays
#define FLEX_ARRAY_COMMON (1)

//! \def FLEX_ARRAY_GNU
//! \brief Level 2 for M_STRICT_FLEX_ARRAY: for [] or [0] flexible arrays only
#define FLEX_ARRAY_GNU (2)

//! \def FLEX_ARRAY_STDC_ONLY
//! \brief Level 3 for M_STRICT_FLEX_ARRAY: for []  flexible arrays only as per C99 and later standards
#define FLEX_ARRAY_STDC_ONLY (3)

#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::strict_flex_array)
#            define M_STRICT_FLEX_ARRAY(level) [[clang::strict_flex_array(level)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::strict_flex_array)
#            define M_STRICT_FLEX_ARRAY(level) [[gnu::strict_flex_array(level)]]
#        endif
#    endif
#    if !defined(M_STRICT_FLEX_ARRAY)
#        if DETECT_GNU_ATTR(strict_flex_array)
#            define M_STRICT_FLEX_ARRAY(level) __attribute__((strict_flex_array(level)))
#        endif
#    endif
#endif // DISABLE_ATTRIBUTES

#if !defined(M_STRICT_FLEX_ARRAY)
#    define M_STRICT_FLEX_ARRAY(level) /* parameter is a strict flexible array with the specified level */
#endif

//! \def M_STRICT_FLEX_ARRAY_AUTO
//! \brief Automatically defines the strict flex array level based on detected compiler and C standard
//! \details When adding a flex array at the end of a structure, you must use the definition \a FLEX_ARRAY
//! as this automatically expands to [], [0], [1] depending on what the compiler supports. It is recommended
//! to also use the M_COUNTED_BY macro when possible as well to help track the size of the array and detect
//! out of bounds accesses.
//! \code
//! struct example_struct {
//!     int count;
//!     M_COUNTED_BY(count) M_STRICT_FLEX_ARRAY_AUTO char data[FLEX_ARRAY];
//! };
//! \endcode
#define M_STRICT_FLEX_ARRAY_AUTO M_STRICT_FLEX_ARRAY(FLEX_LEVEL)

//! \def M_COUNTED_BY(member)
//! \brief Used to mark a flexible array parameter that is counted by another member to track its size.
//! Using this helps better track out of bounds accesses to these arrays. Only used for C99 flex arrays (i.e. [])
//! \param member The parameter name that counts the number of elements in the array
//! \see https://clang.llvm.org/docs/AttributeReference.html#counted-by-counted-by-or-null-sized-by-sized-by-or-null
//! \note In clang, this attribute only works on C99 style flexible arrays (i.e. []).
//! Therefore this is conditionally defined based on FLEX_LEVEL being set to 3 above. GCC does not warn, but the manual
//! specifically says for C99 flexible arrays -TJE
//! \code
//! struct example_struct {
//!     int count;
//!     M_COUNTED_BY(count) M_STRICT_FLEX_ARRAY_AUTO char data[FLEX_ARRAY];
//! };
//! \endcode
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::counted_by)
#            define M_COUNTED_BY(member) [[clang::counted_by(member)]]
#        endif
#        if DETECT_STD_ATTR_QUAL(gnu::counted_by)
#            define M_COUNTED_BY(member) [[gnu::counted_by(member)]]
#        endif
#        if DETECT_STD_ATTR_QUAL(gnu::element_count)
#            define M_COUNTED_BY(member) [[gnu::element_count(member)]]
#        endif
#    endif
#    if !defined(M_COUNTED_BY) && (FLEX_LEVEL == 3)
#        if DETECT_GNU_ATTR(counted_by)
#            define M_COUNTED_BY(member) __attribute__((counted_by(member)))
#        elif DETECT_GNU_ATTR(element_count)
#            define M_COUNTED_BY(member) __attribute__((element_count(member)))
#        endif
#    endif
#endif // DISABLE_ATTRIBUTES

//! \def M_COUNTED_BY_OR_NULL(member)
//! \brief Used to mark a flexible array parameter that is counted by another member to track its size or that it is a
//! NULL pointer. Using this helps better track out of bounds accesses to these arrays.
//! \param member The parameter name that counts the number of elements in the array
//! \see https://clang.llvm.org/docs/AttributeReference.html#counted-by-counted-by-or-null-sized-by-sized-by-or-null
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::counted_by_or_null)
#            define M_COUNTED_BY_OR_NULL(member) [[clang::counted_by_or_null(member)]]
#        endif
#        if DETECT_STD_ATTR_QUAL(gnu::counted_by_or_null)
#            define M_COUNTED_BY_OR_NULL(member) [[gnu::counted_by_or_null(member)]]
#        endif
#    endif
#    if !defined(M_COUNTED_BY_OR_NULL) && (FLEX_LEVEL == 3)
#        if DETECT_GNU_ATTR(counted_by_or_null)
#            define M_COUNTED_BY_OR_NULL(member) __attribute__((counted_by_or_null(member)))
#        endif
#    endif
#endif // DISABLE_ATTRIBUTES

#if !defined(M_COUNTED_BY)
#    define M_COUNTED_BY(member) /* parameter is counted by member */
#endif

#if !defined(M_COUNTED_BY_OR_NULL)
#    define M_COUNTED_BY_OR_NULL(member) /* parameter is counted by member or null */
#endif

//! \def M_SIZED_BY(member)
//! \brief Used to mark a flexible array parameter that is counted by another member to track its size. Unlike
//! M_COUNTED_BY, this is for size specified in bytes rather than number of elements.
//! \param member The parameter name that counts the number of elements in the array
//! \see https://clang.llvm.org/docs/AttributeReference.html#counted-by-counted-by-or-null-sized-by-sized-by-or-null
//! \code
//! struct example_struct {
//!     int count;
//!     M_SIZED_BY(count) M_STRICT_FLEX_ARRAY_AUTO char data[FLEX_ARRAY];
//! };
//! \endcode
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::sized_by)
#            define M_SIZED_BY(member) [[clang::sized_by(member)]]
#        endif
#        if DETECT_STD_ATTR_QUAL(gnu::sized_by)
#            define M_SIZED_BY(member) [[gnu::sized_by(member)]]
#        endif
#    endif
#    if !defined(M_SIZED_BY) && (FLEX_LEVEL == 3)
#        if DETECT_GNU_ATTR(sized_by)
#            define M_SIZED_BY(member) __attribute__((sized_by(member)))
#        endif
#    endif
#endif // DISABLE_ATTRIBUTES

//! \def M_SIZED_BY_OR_NULL(member)
//! \brief Used to mark a flexible array parameter that is counted by another member to track its size or that it is a
//! NULL pointer. Using this helps better track out of bounds accesses to these arrays. Unlike M_COUNTED_BY_OR_NULL,
//! this is for size specified in bytes rather than number of elements.
//! \param member The parameter name that counts the number of elements in the array
//! \see https://clang.llvm.org/docs/AttributeReference.html#counted-by-counted-by-or-null-sized-by-sized-by-or-null
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::sized_by_or_null)
#            define M_SIZED_BY_OR_NULL(member) [[clang::sized_by_or_null(member)]]
#        endif
#        if DETECT_STD_ATTR_QUAL(gnu::sized_by_or_null)
#            define M_SIZED_BY_OR_NULL(member) [[gnu::sized_by_or_null(member)]]
#        endif
#    endif
#    if !defined(M_SIZED_BY_OR_NULL) && (FLEX_LEVEL == 3)
#        if DETECT_GNU_ATTR(sized_by_or_null)
#            define M_SIZED_BY_OR_NULL(member) __attribute__((sized_by_or_null(member)))
#        endif
#    endif
#endif // DISABLE_ATTRIBUTES

#if !defined(M_SIZED_BY)
#    define M_SIZED_BY(member) /* parameter is sized by member */
#endif

#if !defined(M_SIZED_BY_OR_NULL)
#    define M_SIZED_BY_OR_NULL(member) /* parameter is sized by member or null */
#endif

//! \def M_NONSTRING
//! \brief used to mark char[n] that may not contain a NULL terminating character to provide warnings when
//! they are passed to functions that would expect a NULL terminated string
//! \code
//! M_NONSTRING char buffer[16];
//! \endcode
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::nonstring)
#            define M_NONSTRING [[clang::nonstring]]
#        elif DETECT_STD_ATTR_QUAL(gnu::nonstring)
#            define M_NONSTRING [[gnu::nonstring]]
#        endif
#    endif
#    if !defined(M_NONSTRING)
#        if DETECT_GNU_ATTR(nonstring)
#            define M_NONSTRING __attribute__((nonstring))
#        endif
#    endif
#endif // DISABLE_ATTRIBUTES

#if !defined(M_NONSTRING)
#    define M_NONSTRING /* This char array may not be null terminated */
#endif

//! \def M_ASSUME(condition)
//! \brief Tells the compiler to assume a condition is always true for optimization purposes.
//!
//! This attribute enables the compiler to optimize code based on the assumption that the given
//! condition is always true at this point. Unlike assertions, this does NOT check the condition
//! at runtime. If the assumption is violated, behavior is undefined.
//!
//! Available in:
//! - C++23 with [[assume()]]
//! - MSVC with __assume()
//! - Clang/GCC with __builtin_assume()
//!
//! \param condition The condition to assume is true (must not have side effects)
//!
//! \warning If the condition is false at runtime, behavior is undefined. Use only when you can
//!          guarantee the condition is always true.
//!
//! \code
//! void process_positive(int x) {
//!     M_ASSUME(x > 0);
//!     // Compiler can optimize knowing x is positive
//!     int result = 100 / x;  // No divide-by-zero check needed
//! }
//! \endcode
//!
//! \code
//! void process_aligned_buffer(void* ptr) {
//!     M_ASSUME((reinterpret_cast<uintptr_t>(ptr) & 15) == 0);
//!     // Compiler can use aligned memory operations
//! }
//! \endcode
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR(assume)
#            define M_ASSUME(condition) [[assume(condition)]]
#        elif DETECT_STD_ATTR_QUAL(clang::assume)
#            define M_ASSUME(condition) [[clang::assume(condition)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::assume)
#            define M_ASSUME(condition) [[gnu::assume(condition)]]
#        endif
#    endif
#    if !defined(M_ASSUME)
#        if DETECT_GNU_ATTR(assume)
#            define M_ASSUME(condition) __attribute__((assume(condition)))
#        elif defined(__has_builtin)
#            if __has_builtin(__builtin_assume)
#                define M_ASSUME(condition) __builtin_assume(condition)
#            endif
#        elif IS_MSVC_VERSION(MSVC_2003)
#            define M_ASSUME(condition) __assume(condition)
#        endif
#    endif
#endif

#if !defined(M_ASSUME)
// Fallback: use M_UNREACHABLE pattern for false conditions
#    define M_ASSUME(condition)                                                                                        \
        do                                                                                                             \
        {                                                                                                              \
            if (!(condition))                                                                                          \
                M_UNREACHABLE();                                                                                       \
        } while (0)
#endif

//! \def M_LIKELY
//! \brief Hints to the compiler that a branch is likely to be taken.
//! \see https://clang.llvm.org/docs/AttributeReference.html#likely-and-unlikely
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if defined(USING_CPP11) && DETECT_STD_ATTR_CHECK(likely)
#            define M_LIKELY [[likely]]
#        elif DETECT_STD_ATTR_QUAL(clang::likely)
#            define M_LIKELY [[clang::likely]]
#        elif DETECT_STD_ATTR_QUAL(gnu::likely)
#            define M_LIKELY [[gnu::likely]]
#        endif
#    endif
#    if !defined(M_LIKELY)
#        if DETECT_GNU_ATTR(likely)
#            define M_LIKELY __attribute__((likely))
#        endif
#    endif
#endif // DISABLE_ATTRIBUTES

//! \def M_UNLIKELY
//! \brief Hints to the compiler that a branch is unlikely to be taken.
//! \see https://clang.llvm.org/docs/AttributeReference.html#likely-and-unlikely
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if defined(USING_CPP11) && DETECT_STD_ATTR_CHECK(unlikely)
#            define M_UNLIKELY [[unlikely]]
#        elif DETECT_STD_ATTR_QUAL(clang::unlikely)
#            define M_UNLIKELY [[clang::unlikely]]
#        elif DETECT_STD_ATTR_QUAL(gnu::unlikely)
#            define M_UNLIKELY [[gnu::unlikely]]
#        endif
#    endif
#    if !defined(M_UNLIKELY)
#        if DETECT_GNU_ATTR(unlikely)
#            define M_UNLIKELY __attribute__((unlikely))
#        endif
#    endif
#endif // DISABLE_ATTRIBUTES

#if !defined(M_LIKELY)
#    define M_LIKELY /* likely branch */
#endif

#if !defined(M_UNLIKELY)
#    define M_UNLIKELY /* unlikely branch */
#endif

//! \def M_HOT_FUNC
//! \brief Marks a function as a hot function that is called frequently.
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::hot)
#            define M_HOT_FUNC [[clang::hot]]
#        elif DETECT_STD_ATTR_QUAL(gnu::hot)
#            define M_HOT_FUNC [[gnu::hot]]
#        endif
#    endif
#    if !defined(M_HOT_FUNC)
#        if DETECT_GNU_ATTR(hot)
#            define M_HOT_FUNC __attribute__((hot))
#        endif
#    endif
#endif

#if !defined(M_HOT_FUNC)
#    define M_HOT_FUNC /* This function is a hot function called frequently */
#endif

//! \def M_COLD_FUNC
//! \brief Marks a function as a cold function that is called infrequently.
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::cold)
#            define M_COLD_FUNC [[clang::cold]]
#        elif DETECT_STD_ATTR_QUAL(gnu::cold)
#            define M_COLD_FUNC [[gnu::cold]]
#        endif
#    endif
#    if !defined(M_COLD_FUNC)
#        if DETECT_GNU_ATTR(cold)
#            define M_COLD_FUNC __attribute__((cold))
#        endif
#    endif
#endif

#if !defined(M_COLD_FUNC)
#    define M_COLD_FUNC /* This function is a cold function called infrequently */
#endif

//! \def M_FLAG_ENUM
//! \brief Marks an enumeration as a flag enumeration for better type safety and warnings.
//! \see https://clang.llvm.org/docs/AttributeReference.html#flag-enum
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::flag_enum)
#            define M_FLAG_ENUM [[clang::flag_enum]]
#        elif DETECT_STD_ATTR_QUAL(gnu::flag_enum)
#            define M_FLAG_ENUM [[gnu::flag_enum]]
#        endif
#    endif
#    if !defined(M_FLAG_ENUM)
#        if DETECT_GNU_ATTR(flag_enum)
#            define M_FLAG_ENUM __attribute__((flag_enum))
#        endif
#    endif
#endif

#if !defined(M_FLAG_ENUM)
#    define M_FLAG_ENUM /* This enum is a flag enumeration */
#endif

//! \def M_CLOSED_ENUM
//! \brief Marks an enumeration as a closed enumeration to prevent implicit conversions.
//! \see https://clang.llvm.org/docs/AttributeReference.html#closed-enum
//! \note This can be combined with M_FLAG_ENUM to create a closed flag enumeration.

//! \def M_OPEN_ENUM
//! \brief Marks an enumeration as an open enumeration to allow implicit conversions.
//! \see https://clang.llvm.org/docs/AttributeReference.html#open-enum
//! \note This can be combined with M_FLAG_ENUM to create an open flag enumeration.
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::enum_extensibility)
#            define M_CLOSED_ENUM [[clang::enum_extensibility(closed)]]
#            define M_OPEN_ENUM   [[clang::enum_extensibility(open)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::enum_extensibility)
#            define M_CLOSED_ENUM [[gnu::enum_extensibility(closed)]]
#            define M_OPEN_ENUM   [[gnu::enum_extensibility(open)]]
#        endif
#    endif
#    if !defined(M_CLOSED_ENUM)
#        if DETECT_GNU_ATTR(enum_extensibility)
#            define M_CLOSED_ENUM __attribute__((enum_extensibility(closed)))
#            define M_OPEN_ENUM   __attribute__((enum_extensibility(open)))
#        endif
#    endif
#endif

//! \def M_WARN_IF_NOT_ALIGNED(alignment)
//! \brief Warns if a pointer parameter is not aligned to the specified alignment.
//! \code
//! // Will not warn:
//! struct M_ALIGNAS(16) foo
//! {
//!   int i1;
//!   int i2;
//!   M_WARN_IF_NOT_ALIGNED(16) unsigned long long x;
//! };
//! \endcode
//! \code
//! // Will warn:
//! struct foo
//! {
//!   int i1;
//!   int i2;
//!   M_WARN_IF_NOT_ALIGNED(16) unsigned long long x;
//! };
//! \endcode
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::warn_if_not_aligned)
#            define M_WARN_IF_NOT_ALIGNED(alignment) [[clang::warn_if_not_aligned(alignment)]]
#        elif DETECT_STD_ATTR_QUAL(gnu::warn_if_not_aligned)
#            define M_WARN_IF_NOT_ALIGNED(alignment) [[gnu::warn_if_not_aligned(alignment)]]
#        endif
#    endif
#    if !defined(M_WARN_IF_NOT_ALIGNED)
#        if DETECT_GNU_ATTR(warn_if_not_aligned)
#            define M_WARN_IF_NOT_ALIGNED(alignment) __attribute__((warn_if_not_aligned(alignment)))
#        endif
#    endif
#endif

#if !defined(M_WARN_IF_NOT_ALIGNED)
#    define M_WARN_IF_NOT_ALIGNED(alignment) /* Warn if pointer is not aligned to 'alignment' bytes */
#endif

//! \def M_UNSEQUENCED
//! \brief indicates that a function is stateless, effectless, idempotent and independent
//! \see https://en.cppreference.com/w/c/language/attributes/reproducible.html
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR(unsequenced)
#            define M_UNSEQUENCED [[unsequenced]]
#        elif DETECT_STD_ATTR(__unsequenced__)
#            define M_UNSEQUENCED [[__unsequenced__]]
#        elif DETECT_STD_ATTR_QUAL(clang::unsequenced)
#            define M_UNSEQUENCED [[clang::unsequenced]]
#        elif DETECT_STD_ATTR_QUAL(gnu::unsequenced)
#            define M_UNSEQUENCED [[gnu::unsequenced]]
#        endif
#    endif
#    if !defined(M_UNSEQUENCED)
#        if DETECT_GNU_ATTR(unsequenced)
#            define M_UNSEQUENCED __attribute__((unsequenced))
#        endif
#    endif
#endif

#if !defined(M_UNSEQUENCED)
#    define M_UNSEQUENCED /* This function is stateless, effectless, idempotent and independent */
#endif

//! \def M_REPRODUCIBLE
//! \brief Indicates that a function is effectless and idempotent
//! \see https://en.cppreference.com/w/c/language/attributes/reproducible.html
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR(reproducible)
#            define M_REPRODUCIBLE [[reproducible]]
#        elif DETECT_STD_ATTR(__reproducible__)
#            define M_REPRODUCIBLE [[__reproducible__]]
#        elif DETECT_STD_ATTR_QUAL(clang::reproducible)
#            define M_REPRODUCIBLE [[clang::reproducible]]
#        elif DETECT_STD_ATTR_QUAL(gnu::reproducible)
#            define M_REPRODUCIBLE [[gnu::reproducible]]
#        endif
#    endif
#    if !defined(M_REPRODUCIBLE)
#        if DETECT_GNU_ATTR(reproducible)
#            define M_REPRODUCIBLE __attribute__((reproducible))
#        endif
#    endif
#endif

#if !defined(M_REPRODUCIBLE)
#    define M_REPRODUCIBLE /* This function is effectless and idempotent */
#endif

//! \def M_PURE_FUNC
//! \brief Marks a function as a pure function (no side effects, return value depends only on parameters).
//! \note Similar to M_REPRODUCIBLE. Pure also requires finiteness.
//! \note takes priority over M_REPRODUCIBLE
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::pure)
#            define M_PURE_FUNC [[clang::pure]]
#        elif DETECT_STD_ATTR_QUAL(gnu::pure)
#            define M_PURE_FUNC [[gnu::pure]]
#        endif
#    endif
#    if !defined(M_PURE_FUNC)
#        if DETECT_GNU_ATTR(pure)
#            define M_PURE_FUNC __attribute__((pure))
#        endif
#    endif
#endif
#if !defined(M_PURE_FUNC)
#    define M_PURE_FUNC /* This function is a pure function with no side effects */
#endif

//! \def M_CONST_FUNC
//! \brief Marks a function as a const function (no side effects, return value depends only on parameters and global
//! variables). \note similar to M_UNSEQUENCED, but requires finiteness. \note takes priority over M_UNSEQUENCED
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK) && !defined(REAL_MSVC)
#        if DETECT_STD_ATTR_QUAL(clang::const)
#            define M_CONST_FUNC [[clang::const]]
#        elif DETECT_STD_ATTR_QUAL(gnu::const)
#            define M_CONST_FUNC [[gnu::const]]
#        endif
#    endif
#    if !defined(M_CONST_FUNC)
#        if DETECT_GNU_ATTR(const)
#            define M_CONST_FUNC __attribute__((const))
#        endif
#    endif
#endif

#if !defined(M_CONST_FUNC)
#    define M_CONST_FUNC /* This function is a const function with no side effects */
#endif

//! \def M_MS_STRUCT
//! \brief Marks a structure to use MSVC layout for better compatibility with MSVC compiled code.
//! \note This is the default mode when targeting Windows, but may be helpful in other cases.
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(clang::ms_struct)
#            define M_MS_STRUCT [[clang::ms_struct]]
#        elif DETECT_STD_ATTR_QUAL(gnu::ms_struct)
#            define M_MS_STRUCT [[gnu::ms_struct]]
#        endif
#    endif
#    if !defined(M_MS_STRUCT)
#        if DETECT_GNU_ATTR(ms_struct)
#            define M_MS_STRUCT __attribute__((ms_struct))
#        endif
#    endif
#endif

#if !defined(M_MS_STRUCT)
#    define M_MS_STRUCT /* This structure uses MSVC layout */
#endif

//! \def M_TAINTED_ARGS
//! \brief Marks a function as having tainted arguments that require sanitization for trust.
//! \see
//! https://best.openssf.org/Compiler-Hardening-Guides/Compiler-Annotations-for-C-and-C++.html#mark-functions-with-arguments-that-require-sanitization
#if !defined(DISABLE_ATTRIBUTES)
#    if defined(DETECT_STD_ATTR_CHECK)
#        if DETECT_STD_ATTR_QUAL(gnu::tainted_args)
#            define M_TAINTED_ARGS [[gnu::tainted_args]]
#        endif
#    endif
#    if !defined(M_TAINTED_ARGS)
#        if DETECT_GNU_ATTR(tainted_args)
#            define M_TAINTED_ARGS __attribute__((tainted_args))
#        endif
#    endif
#endif // DISABLE_ATTRIBUTES

#if !defined(M_TAINTED_ARGS)
#    define M_TAINTED_ARGS /* This function has tainted arguments that require sanitization for trust */
#endif

#if defined(__cplusplus)
}
#endif
