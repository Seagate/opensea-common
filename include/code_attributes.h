// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file code_attributes.h
// \brief Defines attributes that the compilers can use to better generate warnings or optimizations depending on attributes applied.
//        Also gives access to macros for fallthrough statements, marking variables as unused and more to help resolve warnings
//        or specify expected behavior without a warning being generated.
//
#pragma once

#include "env_detect.h"

#if defined (_WIN32)
    #include <sal.h>
#endif //_WIN32

#if defined (__cplusplus)
extern "C"
{
#endif //__cplusplus

    #if defined USING_C99
    #if defined _MSC_VER && !defined USING_C11
        #define M_RESTRICT __restrict
    #else
        #define M_RESTRICT restrict
    #endif
    #else
        #define M_RESTRICT /*restrict*/
    #endif

    #if defined (USING_CPP98)
        #define M_INLINE inline
    #else //C
        #if defined (USING_C99)
            #define M_INLINE inline
        #elif defined (_MSC_VER) /*version check? It's not clear when this was first supported*/
            #define M_INLINE __inline
        #elif defined (__GNUC__) || defined (__clang__)
            #define M_INLINE __inline__
        #else
            #define M_INLINE /*inline support not available*/
        #endif
    #endif //C++/C check

    #if defined (__GNUC__) || defined (__clang__)
        #define M_NOINLINE __attribute__((noinline))
    #elif defined (_MSC_VER) //version check? It's not clear when this was first supported
        #define M_NOINLINE __declspec(noinline)
    #else 
        #define M_NOINLINE /*no support for noinline*/
    #endif

    #if defined (__GNUC__) || defined (__clang__)
        #define M_FORCEINLINE static M_INLINE __attribute__((always_inline))
    #elif defined (_MSC_VER) //version check? It's not clear when this was first supported
        #define M_FORCEINLINE __forceinline
    #else
        /*Support for forcing inline is not present, however lets try the "normal" inline option to suggest it at least*/
        #define M_FORCEINLINE M_INLINE
    #endif

    //Defining the M_FALLTHROUGH macro to make it easy to correctly specify when to fallthrough on switch-case statements without extra warnings.
    //This is defined differently depending on c/c++ and which version of the standard is currently being supported by the compiler. In addition
    //some compilers support this and others do not, so this is a little complicated to define in a way that works with everything, but it is
    //attempted below...update this as errors are found. -TJE
    #if defined (__cplusplus)
        //check if the standardized way to check for support is available...
        #if defined __has_cpp_attribute
            #if __has_cpp_attribute(fallthrough)
                //This is the standardized way intriduced in C++17
                #define M_FALLTHROUGH [[fallthrough]]
            #endif
        #endif
    #elif defined __has_c_attribute
        #if __has_c_attribute(fallthrough)
            //C23 style
            #define M_FALLTHROUGH [[fallthrough]]
        #elif __has_c_attribute(__fallthrough__)
            //C23 style
            #define M_FALLTHROUGH [[__fallthrough__]]
        #endif
    #endif
    #if !defined (M_FALLTHROUGH)
        //not C++ and doesn't have __has_c_attribute so do something for older C code
        #if defined __has_attribute
            //GCC type compiler check
            #if __has_attribute(fallthrough)
                #define M_FALLTHROUGH do { } while (0); __attribute__((fallthrough))
            #endif
        #endif
        #if !defined M_FALLTHROUGH
            #if (defined (__GNUC__) && __GNUC__ >= 7) || (defined __clang__ && defined (__clang_major__) && defined (__clang_minor__) && (__clang_major__ > 3 || (__clang_major__ >=3 && __clang_minor__ >= 9)))
                //GCC 7+ support the fallthrough attribute.
                //Clang 3.9+ has similar fallthrough support.
                #define M_FALLTHROUGH do { } while (0); __attribute__((fallthrough))
            #elif defined (__fallthrough) /* from sal.h*/
                #define M_FALLTHROUGH __fallthrough
            #else
                //Insert a comment instead since other methods were not detected.
                #define M_FALLTHROUGH /*FALLTHRU*/
            
            #endif
        #endif
    #endif //Checking C/C++
    
    //Now defining a set of macros for unused variables, unused functions, unused parameters. This can be used to deal with specific cases where these warnings need to be
    //turned off in compiling code. This may be because these things are only used at certain times, or under certain OSs, or other strange flags.
    //Ideally, these macros aren't used at all, but they are available to help cleanup code warnings that may show up sometimes.
    //Some of these may be unique to C/C++ standards or to certain compilers.
    //HOW TO USE Each:
    //M_USE_UNUSED(var): This is mean to be used within a function:
    // void Func(int unusedVar)
    // ...
    //    M_USE_UNUSED(unusedVar);
    // ...
    //    return;
    //
    //M_ATTR_UNUSED: This is meant to be used in front of unused variables withing functions, unused functions, and unused parameters in functions
    // void Func(int someVar)
    //    M_ATTR_UNUSED int thisIsntUsed = 0;
    //
    // void Func2 (M_ATTR_UNUSED int unusedVar)
    //
    // M_ATTR_UNUSED void Func3(int param)
    //
    //NOTE: Please adjust these as necessary if they aren't quite working. Hopefully, not too many are compiler unique...-TJE
    //Define M_USE_UNUSED firstt since it is likely to be common on various compilers...(adjust as needed)
    #define M_USE_UNUSED(var) (void)(var)
    //Define all others below since they are used differently
    #if defined (__cplusplus)
        /*check if the standardized way to check for support is available...
        * NOTE: Checking for >= C++11 for __has_cpp_attribute due to issues where this shows up even when the GNU C++ compiler 
        * is set to 98 or 03 and thows errors for not knowing what to do. Seemed easiest to add this additional version check to get rid of this error
        */
        #if __cplusplus >= 201103L && defined __has_cpp_attribute
            #if __has_cpp_attribute(maybe_unused)
                /*This is the standardized way intriduced in C++17*/
                #define M_ATTR_UNUSED [[maybe_unused]]
            #endif
        #endif
    #elif defined __has_c_attribute /*C23*/
        #if __has_c_attribute(maybe_unused)
            /*C23 style*/
            #define M_ATTR_UNUSED [[maybe_unused]]
        #elif __has_c_attribute(__maybe_unused__) /*check for standardized double underscore version*/
            #define M_DEPRECATED [[__maybe_unused__]]
        #endif
    #endif
    #if !defined (M_ATTR_UNUSED)
        /*older C or CPP or no standard way to define this.*/
        /*Use compiler specific checks*/
        #if defined __has_attribute
            /*GCC type compiler check*/
            #if __has_attribute(unused)
                #define M_ATTR_UNUSED __attribute__((unused))
            #endif
        #endif
        #if !defined (M_ATTR_UNUSED)/*__has_attribute is available, but doesn't have what we need-TJE*/
            #if (defined (__GNUC__) || defined (__clang__)
                /* GCC as far back as 2.95.3's online manual supports unused on variables */
                #define M_ATTR_UNUSED __attribute__((unused))
            #elif defined (_MSC_VER)
                #define M_ATTR_UNUSED __pragma(warning(suppress:4100 4101)) /*4102?*/
            #else
                /*Insert a comment instead since other methods were not detected.*/
                #define M_ATTR_UNUSED /*UNUSED*/
            
            #endif
        #endif
    #endif

    //Macro for marking things as deprecated
    #if defined (__cplusplus)
        /*check if the standardized way to check for support is available...
        * NOTE: Checking for >= C++11 for __has_cpp_attribute due to issues where this shows up even when the GNU C++ compiler 
        * is set to 98 or 03 and thows errors for not knowing what to do. Seemed easiest to add this additional version check to get rid of this error
        */
        #if __cplusplus >= 201103L && defined __has_cpp_attribute
            #if __has_cpp_attribute(deprecated)
                /*This is the standardized way introduced in C++17*/
                #define M_DEPRECATED [[deprecated]]
            #endif
        #endif
    #elif defined __has_c_attribute /*C23*/
        #if __has_c_attribute(deprecated)
            /*C23 style*/
            #define M_DEPRECATED [[deprecated]]
        #elif __has_c_attribute(__deprecated__) /*check for standardized double underscore version*/
            #define M_DEPRECATED [[__deprecated__]]
        #endif
    #endif
    #if !defined M_DEPRECATED /*standard ways to set this did not work, so try compiler specific means*/
        #if defined __has_attribute 
            #if __has_attribute(deprecated)
                #define M_DEPRECATED __attribute__((deprecated))
            #endif
        #endif  
        #if !defined M_DEPRECATED /*if a test macro didn't work above, check the compiler to set this correctly -TJE*/
            #if (defined (__GNUC__) && __GNUC__ >= 4) || (defined (__clang__) && defined (__clang_major__) && __clang_major__ >= 3)
                /*GCC 4 added deprecated attribute*/
                /*Unclear when added to clang, but somewhere around version 3.0*/
                #define M_DEPRECATED __attribute__((deprecated))
            #elif defined (_MSC_VER)
                #define M_DEPRECATED __declspec(deprecated) __pragma(warning(suppress : 4996))
            #else
                /*Insert a comment instead since other methods were not detected.*/
                #define M_DEPRECTED /*DEPRECATED*/
            
            #endif
        #endif
    #endif

    //Macro for marking things as nodiscard
    //if a function returns something that should not be ignored, this should be used to mark it and help generate compiler warnings.
    #if defined (__cplusplus)
        /*check if the standardized way to check for support is available...
        * NOTE: Checking for >= C++11 for __has_cpp_attribute due to issues where this shows up even when the GNU C++ compiler 
        * is set to 98 or 03 and thows errors for not knowing what to do. Seemed easiest to add this additional version check to get rid of this error
        */
        #if __cplusplus >= 201103L && defined __has_cpp_attribute
            #if __has_cpp_attribute(nodiscard)
                /*This is the standardized way introduced in C++17*/
                #define M_NODISCARD [[nodiscard]]
            #endif
        #endif
    #elif defined __has_c_attribute /*C23*/
        #if __has_c_attribute(nodiscard)
            /*C23 style*/
            #define M_NODISCARD [[nodiscard]]
        #elif __has_c_attribute(__nodiscard__) /*check for standardized double underscore version*/
            #define M_NODISCARD [[__nodiscard__]]
        #endif
    #endif
    #if !defined M_NODISCARD /*standard ways to set this did not work, so try compiler specific means*/
        #if defined __has_attribute 
            #if __has_attribute(nodiscard)
                #define M_NODISCARD __attribute__((nodiscard))
            #elif __has_attribute(warn_unused_result)
                #define M_NODISCARD __attribute__((warn_unused_result))
            #endif
        #endif  
        #if !defined M_NODISCARD /*if a test macro didn't work above, check the compiler to set this correctly -TJE*/
            #if (defined (__GNUC__) && __GNUC__ >= 3) || defined (__clang__)
                /*first in GCC 3.4*/
                #define M_NODISCARD __attribute__((warn_unused_result))
            #elif defined (_Check_return_) /*from sal.h*/
                #define M_NODISCARD _Check_return_
            #else
                /*Insert a comment instead since other methods were not detected.*/
                #define M_NODISCARD /*NODISCARD*/
            
            #endif
        #endif
    #endif

    //Macro for marking things as noreturn
    //to be used if a function does not return.
    //examples of stdlib functions with noreturn: abort, exit, _Exit, quick_exit, thrd_exit, longjmp
    #if defined USING_C11 && !defined USING_C23
        //added in C11, then deprecated for function attributes instead in C23
        //https://en.cppreference.com/w/c/language/_Noreturn
        #include <stdnoreturn.h>
    #endif
    #if defined (__cplusplus)
        //check if the standardized way to check for support is available...
        //NOTE: Checking for >= C++11 for __has_cpp_attribute due to issues where this shows up even when the GNU C++ compiler 
        //is set to 98 or 03 and thows errors for not knowing what to do. Seemed easiest to add this additional version check to get rid of this error
        #if __cplusplus >= 201103L && defined __has_cpp_attribute
            #if __has_cpp_attribute(noreturn)
                //This is the standardized way introduced in C++17
                #define M_NORETURN [[noreturn]]
            #endif
        #endif
    #elif defined noreturn
        //C11 convenience macro
        //https://en.cppreference.com/w/c/language/_Noreturn
        #define M_NORETURN noreturn
    #elif defined __has_c_attribute /*C23*/
        #if __has_c_attribute(noreturn)
            /*C23 style*/
            #define M_NORETURN [[noreturn]]
        #elif __has_c_attribute(__noreturn__) /*check for standardized double underscore version*/
            #define M_NORETURN [[__noreturn__]]
        /* next 2 checks are deprecated versions, but also worth checking */
        #elif __has_c_attribute(___Noreturn__)
            #define M_NORETURN [[___Noreturn__]]
        #endif
    #endif
    #if !defined M_NORETURN /*standard ways to set this did not work, so try compiler specific means*/
        #if defined __has_attribute 
            #if __has_attribute(noreturn)
                #define M_NORETURN __attribute__((noreturn))
            #endif
        #endif  
        #if !defined M_NORETURN /*if a test macro didn't work above, check the compiler to set this correctly -TJE*/
            #if (defined (__GNUC__) && __GNUC__ >= 3) || defined (__clang__)
                /*GCC 2.5 added this support*/
                #define M_NORETURN __attribute__((noreturn))
            #elif defined (_MSC_VER)
                #define M_NORETURN __declspec(noreturn)
            #else
                /*Insert a comment instead since other methods were not detected.*/
                #define M_NORETURN /*NORETURN*/
            
            #endif
        #endif
    #endif

    #if defined (__clang__)
        #define M_FUNC_ATTR_MALLOC __attribute__((malloc))
    #elif defined (__GNUC__)
        /*GCC 4.1.3 added attribute for malloc*/
        #if __GNUC__ > 4 || (defined (__GNUC_MINOR__) && __GNUC__ >=4 && __GNUC_MINOR__ >= 1)
            #define M_FUNC_ATTR_MALLOC __attribute__((malloc))
        #else
            #define M_FUNC_ATTR_MALLOC /* this function allocates memory and returns the pointer to you */
        #endif
    #elif defined (_Ret_opt_valid_) /* from SAL. Not exactly right, but closer to how this works for clang and gcc. There is a similar SAL annotation where you name the parameter specifying the size, but I didn't want to change on the other attributes for now - TJE */
        #define M_FUNC_ATTR_MALLOC _Ret_opt_valid_
    #else
        #define M_FUNC_ATTR_MALLOC /* this function allocates memory and returns the pointer to you */
    #endif

    #if defined (__GNUC__)
        //This attribute was first suppored in GCC 2.5
        //Not doing a version check with how old that is at this time.
        //If we need to add a version check in the future for other compilers that have some GCC support, we can - TJE
        //Varargpos should be set to zero when used with functions like vfprintf
        #define FUNC_ATTR_PRINTF(formatargpos, varargpos) __attribute__((format(printf, formatargpos, varargpos)))
    #else //!__GNUC__
        #define FUNC_ATTR_PRINTF(formatargpos, varargpos) /* this is a printf/fprintf/etc style function. Please use a user defined constant string for the format! */
    #endif //__GNUC__

        //_Printf_format_string_impl_ for SAL. Not sure how to use it yet so not defined currently

    #if defined (_WIN32)
        #define DLL_EXPORT __declspec(dllexport)
        #define DLL_IMPORT __declspec(dllimport)
    #elif defined (__GNUC__) && __GNUC__ >= 4
        //See https://gcc.gnu.org/wiki/Visibility
        #define DLL_EXPORT __attribute__((visibility("default")))
        #define DLL_IMPORT /*no gcc equivalent*/
    #else
        #define DLL_EXPORT /* no equivalent */
        #define DLL_IMPORT /* no equivalent */
    #endif

#if defined (__cplusplus)
}
#endif
