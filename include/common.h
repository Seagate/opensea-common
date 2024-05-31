// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012-2023 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file common.h
// \brief Defines the constants structures and function headers that are common to OS & Non-OS code.
//
#pragma once
//test comment
#if defined (__cplusplus)
//defining these macros for C++ to make older C++ compilers happy and work like the newer C++ compilers
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
extern "C"
{
#endif

    //Set macros to make it easier to check specific C standards compatibility.
    //NOTE: Do not rely strictly on these macros for all features from all standards, especially in MSVC
    //      Prior to VS2022 17.2
    //      You may still need additional checks before using certain APIs or features since there are a lot of
    //      missing pieces in some Windows environements.
    #if defined(__STDC__)
        #define USING_C89
        #if defined(__STDC_VERSION__)
            #if __STDC_VERSION__ >= 199409L 
                #define USING_C94
            #endif //__STDC_VERSION__ >= 199409L 
            #if __STDC_VERSION__ >= 199901L
                #define USING_C99
            #endif //__STDC_VERSION__ >= 199901L
            #if __STDC_VERSION__ >= 201112L
                #define USING_C11
            #endif //__STDC_VERSION__ >= 201112L
            #if __STDC_VERSION__ >= 201710L
                #define USING_C17
            #endif //__STDC_VERSION__ >= 201710L
            #if __STDC_VERSION__ >= 202311L
                #define USING_C23
            #endif //__STDC_VERSION__ >= 202311L
        #endif //__STDC_VERSION__
    #elif defined (_MSC_VER)
            //Set an approximate standard based on MSVC_VER
            //https://learn.microsoft.com/en-us/cpp/overview/compiler-versions?view=msvc-170#service-releases-starting-with-visual-studio-2017
            //https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
            #if _MSC_VER >= 600
                #define USING_C89
            #endif //_MSC_VER >= 600
            #if defined (_WCHAR_T_DEFINED)
                //https://learn.microsoft.com/en-us/cpp/build/reference/zc-wchar-t-wchar-t-is-native-type?view=msvc-170
                //NOTE: This may not be perfect, but it's the best thing I can find to check-TJE
                #define USING_C94
            #endif //_WCHAR_T_DEFINED
            #if _MSC_VER >= 1800
                //NOTE: This is not exactly true, but close enough.
                //      MSFT does not support VLAs which is the biggest missing piece of C99 support
                //      However, starting in MSVC2013 most everything else from C99 was implemented.
                //      Setting this since we don't use VLAs and don't need the missing math calls
                //      in our code base
                #define USING_C99
            #endif //_MSC_VER >= 1800
            #if _MSC_VER >= 1920 && _MSC_VER < 1932 && defined (_MSC_EXTENSIONS) \
                && (defined (__STDC_NO_VLA__) || defined (__STDC_NO_ATOMICS__) || defined (__STDC_NO_COMPLEX__) \
                    || defined (__STDC_NO_THREADS__))
                //NOTE: This is far from perfect.
                //      VS2019 added a way to specify the standard, however the __STDC__ macros above are only defined
                //      when using /Za until VS2022 17.2 where these are defined when specifying /std::c11 or c17
                //      So in this case, we are detecting VS2019 running without /Za, and specifying C11 or C17
                //      by also checking __STDC_NO_VLA__ since that appears to be defined when specifying C11 or C17
                //      and using extensions. I'm sure this can be further refined.
                //NOTE: When using C11 or C17, MSFT documentation says these are implemented the same in their case
                //      So set both of these version macros
                //__STDC_HOSTED__???
                #define USING_C11
                #define USING_C17
            #endif //MSC_VER and lots of other flags to figure out if it's running in C11/17 mode
            //NOTE: Not sure when/if C23 will be supported, but hopefully it will show up under the standard macros above going forward-TJE
    #endif //__STDC__

    #if defined (__cplusplus)
        #define USING_CPP98
        #if __cplusplus >= 201103L
            #define USING_CPP11
        #endif
        #if __cplusplus >= 201402L
            #define USING_CPP14
        #endif
        #if __cplusplus >= 201703L
            #define USING_CPP17
        #endif
        #if __cplusplus >= 202002L
            #define USING_CPP20
        #endif
        #if __cplusplus >= 202302L
            #define USING_CPP23
        #endif
        #if defined (_MSC_VER)
            //Check _MSVC_LANG  as it may or maynot match __cplusplus in some versions of msvc
            //This may not be an issue with VS2022, but it was an issue at least in VS2015
            #if !defined (USING_CPP11) && _MSVC_LANG >= 201103L
                #define USING_CPP11
            #endif
            #if !defined (USING_CPP14) && _MSVC_LANG >= 201402L 
                #define USING_CPP14
            #endif
            #if !defined (USING_CPP17) && _MSVC_LANG >= 201703L 
                #define USING_CPP17
            #endif
            #if !defined (USING_CPP20) && _MSVC_LANG >= 202002L
                #define USING_CPP20
            #endif
            #if !defined (USING_CPP23) && _MSVC_LANG >= 202302L
                #define USING_CPP23
            #endif
        #endif//_MSC_VER
    #endif//__cplusplus

#if defined USING_C99
    #if defined _MSC_VER && !defined USING_C11
        #define M_RESTRICT __restrict
    #else
        #define M_RESTRICT restrict
    #endif
#else
    #define M_RESTRICT /*restrict*/
#endif

//Detect the LP64 pr LLP64 mode for the system/compiler.
//This is meant to be used within various opensea-common functions, but defined in this header so that it can be used easily for other uses
#if defined (__LP64__) || defined (_LP64)
    #define LP64_DATA_MODEL
#elif defined (_WIN32)
    #if defined (_WIN64)
        #define LLP64_DATA_MODEL
    #else
        #define ILP32_DATA_MODEL
    #endif
#elif defined (__ILP32__) || defined (_ILP32)
    #define ILP32_DATA_MODEL
#elif defined (__ILP64__) || defined (_ILP64)
    #define ILP64_DATA_MODEL
#elif defined (__LP32__) || defined (_LP32)
    #define LP32_DATA_MODEL
#else 
    //Add more data model info here as necessary.
    //If a data model is not already defined above, then some built in logic is used to decide functions to call in 
    //some scenarios.
#endif


//asking to get C11 _s functions since there is some ability to use them in some places.
#define __STDC_WANT_LIB_EXT1__ 1
//asking to get dynamic memory functions (mostly strdup) in C11. Only guaranteed available if __STDC_ALLOC_LIB__ defined, but asking anyways...most systems we support will have this-TJE
#define __STDC_WANT_LIB_EXT2__ 1

    #include <stdio.h>
    #include <time.h>
    #include <string.h>
#if (defined (__STDC_LIB_EXT1__) && defined (__STDC_WANT_LIB_EXT1__)) 
    #define HAVE_C11_ANNEX_K
    //NOTE: These are _s functions, but they MAY NOT MATCH the _s functions Microsoft has available.
    //      Some have different parameters or different parameter order among other things.
    //      Some do match and are the same.
    //      To check for Microsoft's version, check for __STDC_SECURE_LIB__
#endif //check for annex k
    #include <stdlib.h>
    #include <inttypes.h>
    #include <stdbool.h>
    #include <errno.h> //for printing std errors to the screen...more useful for 'nix OSs, but useful everywhere since it is at least standard functions
#if defined (__unix__) || defined(__APPLE__)
    #include <unistd.h> //to ensure we can check for POSIX versions
#endif //__unix__
    //POSIX version checks. NOTE: not taking into account xopen/sus versions here, just posix
    //May need to check for other optional posix features using these other macros: https://linux.die.net/man/7/posixoptions
    #if defined (_POSIX_VERSION)
        #if _POSIX_VERSION >= 198808L
            #define POSIX_1988
        #endif
        #if _POSIX_VERSION >= 199009L
            #define POSIX_1990
        #endif
        #if defined (_POSIX2_C_VERSION) && _POSIX2_C_VERSION >= 199209L
            #define POSIX_1992
        #endif
        #if _POSIX_VERSION >= 199309L
            #define POSIX_1993
        #endif
        #if _POSIX_VERSION >= 199506L
            #define POSIX_1996
        #endif
        #if _POSIX_VERSION >= 200112L
            #define POSIX_2001
        #endif
        #if _POSIX_VERSION >= 200809L
            #define POSIX_2008
        #endif
    #endif //_POSIX_VERSION 

    //detect X/open portability guide/single unix specification support
    //X/Open is a portability guide
    //This eventually became the single unix specifications which are defined under here as well.
    #if defined (_XOPEN_VERSION)
        //version 1 and 2 of XOpen?
        //V1 published in 1985
        //V2 published in 1987
        #if _XOPEN_VERSION >= 3
            #define USING_XPG3 //1989
        #endif
        #if _XOPEN_VERSION >= 4
            #define USING_XPG4 //1992
            #if defined (_XOPEN_UNIX)
                #define USING_SUS //UNIX95
            #endif
        #endif
        #if _XOPEN_VERSION >= 500
            #define USING_SUS2 //UNIX98
        #endif
        #if _XOPEN_VERSION >= 600
            #define USING_SUS3 //UNIX03
        #endif
        #if _XOPEN_VERSION >= 700
            #define USING_SUS4
        #endif
    #endif //_XOPEN_VERSION
    //NOTE: Some functions that were added to posix, xopen, sus may have come from BSD 4.x. Use sys/param.h to check for these old bsd versions


    #if !defined(UINTPTR_MAX)
    //need uintptr_t type for NVMe capabilities to prevent warnings/errors
    //TODO: if C11, _Static_assert can be used to check against sizeof(void*) to make sure this is defined in a way that should work.
        #if defined (_WIN64) || defined (_M_IA64) || defined (_M_ALPHA) || defined (_M_X64) || defined (_M_AMD64) || defined (__alpha__) || defined (__amd64__) || defined (__x86_64__) || defined (__aarch64__) || defined (__ia64__) || defined (__IA64__) || defined (__powerpc64__) || defined (__PPC64__) || defined (__ppc64__) || defined (_ARCH_PPC64)//64bit
            typedef uint64_t uintptr_t;
            #define UINTPTR_MAX UINT64_MAX
        #else //assuming 32bit
            typedef uint32_t uintptr_t;
            #define UINTPTR_MAX UINT32_MAX
        #endif
    #endif

    // Including strings.h to have string case compare functionality and working around Windows.
    // TODO: improve this check as needed for other systems.
    // TODO: make definitions for other functions in strings.h
    #if defined (__unix__) || defined (POSIX_2001)
        #include <strings.h>
    #elif defined (_WIN32) 
        #if !defined (strcasecmp)
            #define strcasecmp(s1, s2) _stricmp(s1, s2)
        #endif //strcasecmp
        #if !defined (strncasecmp)
            #define strncasecmp(s1, s2, n) _strnicmp(s1, s2, n)
        #endif //strncasecmp
    #else 
        #error "Need string case compare definition."
    #endif // __unix__, POSIX, WIN32

    //Macro to help make casts more clear and searchable. Can be very helpful while debugging.
    //If using C++, use static_cast, reinterpret_cast, dynamic_cast before trying a C_CAST.
    #define C_CAST(type, val) (type)(val)

    //Microsoft doesn't have snprintf compliant with C99 until VS2015, so make our own definition using what microsoft does have available.
    #if defined (_MSC_VER) && _MSC_VER <= 1800 && defined _WIN32
    int snprintf(char *buffer, size_t bufsz, const char *format, ...);
    #endif

    

    //Macro to help make casts more clear and searchable. Can be very helpful while debugging.
    //If using C++, use static_cast, reinterpret_cast, dynamic_cast before trying a C_CAST.
    #define C_CAST(type, val) (type)(val)

#if defined (__cplusplus)
//get a specific double word
    #define M_DoubleWord0(l) ( static_cast<uint32_t> ( ( (l) & UINT64_C(0x00000000FFFFFFFF) ) >>  0 ) )
    #define M_DoubleWord1(l) ( static_cast<uint32_t> ( ( (l) & UINT64_C(0xFFFFFFFF00000000) ) >> 32 ) )

    //get a specific double word
    #define M_DoubleWordInt0(l) ( static_cast<int32_t> ( ( (l) & UINT64_C(0x00000000FFFFFFFF) ) >>  0 ) )
    #define M_DoubleWordInt1(l) ( static_cast<int32_t> ( ( (l) & UINT64_C(0xFFFFFFFF00000000) ) >> 32 ) )

    //get a specific word
    #define M_Word0(l) ( static_cast<uint16_t> ( ( (l) & UINT64_C(0x000000000000FFFF) ) >>  0 ) )
    #define M_Word1(l) ( static_cast<uint16_t> ( ( (l) & UINT64_C(0x00000000FFFF0000) ) >> 16 ) )
    #define M_Word2(l) ( static_cast<uint16_t> ( ( (l) & UINT64_C(0x0000FFFF00000000) ) >> 32 ) )
    #define M_Word3(l) ( static_cast<uint16_t> ( ( (l) & UINT64_C(0xFFFF000000000000) ) >> 48 ) )

    //get a specific word as int's
    #define M_WordInt0(l) ( static_cast<int16_t> ( ( (l) & UINT64_C(0x000000000000FFFF) ) >>  0 ) )
    #define M_WordInt1(l) ( static_cast<int16_t> ( ( (l) & UINT64_C(0x00000000FFFF0000) ) >> 16 ) )
    #define M_WordInt2(l) ( static_cast<int16_t> ( ( (l) & UINT64_C(0x0000FFFF00000000) ) >> 32 ) )
    #define M_WordInt3(l) ( static_cast<int16_t> ( ( (l) & UINT64_C(0xFFFF000000000000) ) >> 48 ) )

    //need to validate that this macro sets the correct bits on 32bit and 64bit
    #define BITSPERBYTE UINT8_C(8)
    #define M_ByteN(n) ((UINT8_MAX << (n * BITSPERBYTE)))

    //Get a specific byte
    #define M_Byte0(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x00000000000000FF) ) >>  0 ) )
    #define M_Byte1(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x000000000000FF00) ) >>  8 ) )
    #define M_Byte2(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x0000000000FF0000) ) >> 16 ) )
    #define M_Byte3(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x00000000FF000000) ) >> 24 ) )
    #define M_Byte4(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x000000FF00000000) ) >> 32 ) )
    #define M_Byte5(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x0000FF0000000000) ) >> 40 ) )
    #define M_Byte6(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x00FF000000000000) ) >> 48 ) )
    #define M_Byte7(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0xFF00000000000000) ) >> 56 ) )

    //Get a specific byte int
    #define M_ByteInt0(l) ( static_cast<int8_t> ( ( (l) & UINT64_C(0x00000000000000FF) ) >>  0 ) )
    #define M_ByteInt1(l) ( static_cast<int8_t> ( ( (l) & UINT64_C(0x000000000000FF00) ) >>  8 ) )
    #define M_ByteInt2(l) ( static_cast<int8_t> ( ( (l) & UINT64_C(0x0000000000FF0000) ) >> 16 ) )
    #define M_ByteInt3(l) ( static_cast<int8_t> ( ( (l) & UINT64_C(0x00000000FF000000) ) >> 24 ) )
    #define M_ByteInt4(l) ( static_cast<int8_t> ( ( (l) & UINT64_C(0x000000FF00000000) ) >> 32 ) )
    #define M_ByteInt5(l) ( static_cast<int8_t> ( ( (l) & UINT64_C(0x0000FF0000000000) ) >> 40 ) )
    #define M_ByteInt6(l) ( static_cast<int8_t> ( ( (l) & UINT64_C(0x00FF000000000000) ) >> 48 ) )
    #define M_ByteInt7(l) ( static_cast<int8_t> ( ( (l) & UINT64_C(0xFF00000000000000) ) >> 56 ) )

    //get a specific nibble
    #define M_Nibble0(l)  ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x000000000000000F) ) >>  0 ) )
    #define M_Nibble1(l)  ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x00000000000000F0) ) >>  4 ) )
    #define M_Nibble2(l)  ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x0000000000000F00) ) >>  8 ) )
    #define M_Nibble3(l)  ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x000000000000F000) ) >> 12 ) )
    #define M_Nibble4(l)  ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x00000000000F0000) ) >> 16 ) )
    #define M_Nibble5(l)  ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x0000000000F00000) ) >> 20 ) )
    #define M_Nibble6(l)  ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x000000000F000000) ) >> 24 ) )
    #define M_Nibble7(l)  ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x00000000F0000000) ) >> 28 ) )
    #define M_Nibble8(l)  ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x0000000F00000000) ) >> 32 ) )
    #define M_Nibble9(l)  ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x000000F000000000) ) >> 36 ) )
    #define M_Nibble10(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x00000F0000000000) ) >> 40 ) )
    #define M_Nibble11(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x0000F00000000000) ) >> 44 ) )
    #define M_Nibble12(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x000F000000000000) ) >> 48 ) )
    #define M_Nibble13(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x00F0000000000000) ) >> 52 ) )
    #define M_Nibble14(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0x0F00000000000000) ) >> 56 ) )
    #define M_Nibble15(l) ( static_cast<uint8_t> ( ( (l) & UINT64_C(0xF000000000000000) ) >> 60 ) )

    #define M_NibblesTo1ByteValue(n1, n0) ( \
    static_cast<uint8_t> ( (static_cast<uint8_t> ((n1) & 0x0F) << 4) | (static_cast<uint8_t> ((n0) & 0x0F) << 0)) \
                                           )

    // Big endian parameter order, little endian value
    #define M_BytesTo4ByteValue(b3, b2, b1, b0)                    (        \
    static_cast<uint32_t> (  (static_cast<uint32_t> (b3) << 24) | (static_cast<uint32_t> (b2) << 16) |          \
                 (static_cast<uint32_t> (b1) <<  8) | (static_cast<uint32_t> (b0) <<  0)  )         \
                                                                   )
    // Big endian parameter order, little endian value
    #define M_BytesTo8ByteValue(b7, b6, b5, b4, b3, b2, b1, b0)    (        \
    static_cast<uint64_t> ( (static_cast<uint64_t> (b7) << 56) | (static_cast<uint64_t> (b6) << 48) |           \
                (static_cast<uint64_t> (b5) << 40) | (static_cast<uint64_t> (b4) << 32) |           \
                (static_cast<uint64_t> (b3) << 24) | (static_cast<uint64_t> (b2) << 16) |           \
                (static_cast<uint64_t> (b1) <<  8) | (static_cast<uint64_t> (b0) <<  0)  )          \
                                                                   )

    // Big endian parameter order, little endian value
    #define M_BytesTo2ByteValue(b1, b0)                            (        \
    static_cast<uint16_t> (  (static_cast<uint16_t>(b1) << 8) | (static_cast<uint16_t>(b0) <<  0)  )          \
                                                                   )

    // Big endian parameter order, little endian value
    #define M_WordsTo4ByteValue(w1, w0)                            (        \
    static_cast<uint32_t> (  (static_cast<uint32_t> (w1) << 16) | (static_cast<uint32_t> (w0) <<  0)  )         \
                                                                   )

    #define M_WordsTo8ByteValue(w3, w2, w1, w0)                    (   \
    static_cast<uint64_t> (  (static_cast<uint64_t> (w3) << 48) | (static_cast<uint64_t> (w2) << 32) |     \
                 (static_cast<uint64_t> (w1) << 16) | (static_cast<uint64_t> (w0) <<  0)  )    \
                                                                   )

    // Big endian parameter order, little endian value
    #define M_DWordsTo8ByteValue(d1, d0)                           (        \
    static_cast<uint64_t> (  (static_cast<uint64_t> (d1) << 32) | (static_cast<uint64_t> (d0) <<  0)  )         \
                                                                   )

    #define M_GETBITRANGE(input, msb, lsb) (((input) >> (lsb)) & ~(~UINT64_C(0) << ((msb) - (lsb) + 1)))
    // get bit range for signed values
    #define M_IGETBITRANGE(input, msb, lsb) (static_cast<int64_t>(M_GETBITRANGE(input, msb, lsb)))

    // MACRO to round the number of x so that it will not round up when formating the float
    #define ROUNDF(f, c) ((static_cast<float>(static_cast<int>((f) * (c))) / (c)))

    // Bit access macros

#else //C, not c++
    //get a specific double word
    #define M_DoubleWord0(l) ( C_CAST(uint32_t, ( ( (l) & UINT64_C(0x00000000FFFFFFFF) ) >>  0 ) ) )
    #define M_DoubleWord1(l) ( C_CAST(uint32_t, ( ( (l) & UINT64_C(0xFFFFFFFF00000000) ) >> 32 ) ) )

    //get a specific double word
    #define M_DoubleWordInt0(l) ( C_CAST(int32_t, ( ( (l) & UINT64_C(0x00000000FFFFFFFF) ) >>  0 ) ) )
    #define M_DoubleWordInt1(l) ( C_CAST(int32_t, ( ( (l) & UINT64_C(0xFFFFFFFF00000000) ) >> 32 ) ) )

    //get a specific word
    #define M_Word0(l) ( C_CAST(uint16_t, ( ( (l) & UINT64_C(0x000000000000FFFF) ) >>  0 ) ) )
    #define M_Word1(l) ( C_CAST(uint16_t, ( ( (l) & UINT64_C(0x00000000FFFF0000) ) >> 16 ) ) )
    #define M_Word2(l) ( C_CAST(uint16_t, ( ( (l) & UINT64_C(0x0000FFFF00000000) ) >> 32 ) ) )
    #define M_Word3(l) ( C_CAST(uint16_t, ( ( (l) & UINT64_C(0xFFFF000000000000) ) >> 48 ) ) )

    //get a specific word as int's
    #define M_WordInt0(l) ( C_CAST(int16_t, ( ( (l) & UINT64_C(0x000000000000FFFF) ) >>  0 ) ) )
    #define M_WordInt1(l) ( C_CAST(int16_t, ( ( (l) & UINT64_C(0x00000000FFFF0000) ) >> 16 ) ) )
    #define M_WordInt2(l) ( C_CAST(int16_t, ( ( (l) & UINT64_C(0x0000FFFF00000000) ) >> 32 ) ) )
    #define M_WordInt3(l) ( C_CAST(int16_t, ( ( (l) & UINT64_C(0xFFFF000000000000) ) >> 48 ) ) )

    //need to validate that this macro sets the correct bits on 32bit and 64bit
    #define BITSPERBYTE UINT8_C(8)
    #define M_ByteN(n) ((UINT8_MAX << (n * BITSPERBYTE)))

    //Get a specific byte
    #define M_Byte0(l) ( C_CAST(uint8_t, ( ( (l) & UINT64_C(0x00000000000000FF) ) >>  0 ) ) )
    #define M_Byte1(l) ( C_CAST(uint8_t, ( ( (l) & UINT64_C(0x000000000000FF00) ) >>  8 ) ) )
    #define M_Byte2(l) ( C_CAST(uint8_t, ( ( (l) & UINT64_C(0x0000000000FF0000) ) >> 16 ) ) )
    #define M_Byte3(l) ( C_CAST(uint8_t, ( ( (l) & UINT64_C(0x00000000FF000000) ) >> 24 ) ) )
    #define M_Byte4(l) ( C_CAST(uint8_t, ( ( (l) & UINT64_C(0x000000FF00000000) ) >> 32 ) ) )
    #define M_Byte5(l) ( C_CAST(uint8_t, ( ( (l) & UINT64_C(0x0000FF0000000000) ) >> 40 ) ) )
    #define M_Byte6(l) ( C_CAST(uint8_t, ( ( (l) & UINT64_C(0x00FF000000000000) ) >> 48 ) ) )
    #define M_Byte7(l) ( C_CAST(uint8_t, ( ( (l) & UINT64_C(0xFF00000000000000) ) >> 56 ) ) )

    //Get a specific byte int
    #define M_ByteInt0(l) ( C_CAST(int8_t, ( ( (l) & UINT64_C(0x00000000000000FF) ) >>  0 ) ) )
    #define M_ByteInt1(l) ( C_CAST(int8_t, ( ( (l) & UINT64_C(0x000000000000FF00) ) >>  8 ) ) )
    #define M_ByteInt2(l) ( C_CAST(int8_t, ( ( (l) & UINT64_C(0x0000000000FF0000) ) >> 16 ) ) )
    #define M_ByteInt3(l) ( C_CAST(int8_t, ( ( (l) & UINT64_C(0x00000000FF000000) ) >> 24 ) ) )
    #define M_ByteInt4(l) ( C_CAST(int8_t, ( ( (l) & UINT64_C(0x000000FF00000000) ) >> 32 ) ) )
    #define M_ByteInt5(l) ( C_CAST(int8_t, ( ( (l) & UINT64_C(0x0000FF0000000000) ) >> 40 ) ) )
    #define M_ByteInt6(l) ( C_CAST(int8_t, ( ( (l) & UINT64_C(0x00FF000000000000) ) >> 48 ) ) )
    #define M_ByteInt7(l) ( C_CAST(int8_t, ( ( (l) & UINT64_C(0xFF00000000000000) ) >> 56 ) ) )

    //get a specific nibble
    #define M_Nibble0(l)  C_CAST(uint8_t, C_CAST(uint8_t,l) & C_CAST(uint8_t, UINT8_C(0x0F)) )
    #define M_Nibble1(l)  C_CAST(uint8_t, ( ( C_CAST(uint8_t,l) & UINT8_C(0xF0) ) >>  4 ) )
    #define M_Nibble2(l)  C_CAST(uint8_t, ( ( C_CAST(uint16_t,l) & UINT16_C(0x0F00) ) >>  8 ) )
    #define M_Nibble3(l)  C_CAST(uint8_t, ( ( C_CAST(uint16_t,l) & UINT16_C(0xF000) ) >> 12 ) )
    #define M_Nibble4(l)  C_CAST(uint8_t, ( ( C_CAST(uint32_t,l) & UINT32_C(0x000F0000) ) >> 16  ) )
    #define M_Nibble5(l)  C_CAST(uint8_t, ( ( C_CAST(uint32_t,l) & UINT32_C(0x00F00000) ) >> 20  ) )
    #define M_Nibble6(l)  C_CAST(uint8_t, ( ( C_CAST(uint32_t,l) & UINT32_C(0x0F000000) ) >> 24  ) )
    #define M_Nibble7(l)  C_CAST(uint8_t, ( ( C_CAST(uint32_t,l) & UINT32_C(0xF0000000) ) >> 28  ) )
    #define M_Nibble8(l)  C_CAST(uint8_t, ( ( C_CAST(uint64_t,l) & UINT64_C(0x0000000F00000000) ) >> 32 ) )
    #define M_Nibble9(l)  C_CAST(uint8_t, ( ( C_CAST(uint64_t,l) & UINT64_C(0x000000F000000000) ) >> 36 ) )
    #define M_Nibble10(l) C_CAST(uint8_t, ( ( C_CAST(uint64_t,l) & UINT64_C(0x00000F0000000000) ) >> 40 ) )
    #define M_Nibble11(l) C_CAST(uint8_t, ( ( C_CAST(uint64_t,l) & UINT64_C(0x0000F00000000000) ) >> 44 ) )
    #define M_Nibble12(l) C_CAST(uint8_t, ( ( C_CAST(uint64_t,l) & UINT64_C(0x000F000000000000) ) >> 48 ) )
    #define M_Nibble13(l) C_CAST(uint8_t, ( ( C_CAST(uint64_t,l) & UINT64_C(0x00F0000000000000) ) >> 52 ) )
    #define M_Nibble14(l) C_CAST(uint8_t, ( ( C_CAST(uint64_t,l) & UINT64_C(0x0F00000000000000) ) >> 56 ) )
    #define M_Nibble15(l) C_CAST(uint8_t, ( ( C_CAST(uint64_t,l) & UINT64_C(0xF000000000000000) ) >> 60 ) )

    #define M_NibblesTo1ByteValue(n1, n0) ( \
    C_CAST(uint8_t, ( (C_CAST(uint8_t, ((n1) & 0x0F) << 4) | (C_CAST(uint8_t, ((n0) & 0x0F) << 0)) \
                                           ) ) ) )

    // Big endian parameter order, little endian value
    #define M_BytesTo4ByteValue(b3, b2, b1, b0)                    (        \
    C_CAST(uint32_t, (  (C_CAST(uint32_t, b3) << 24) | (C_CAST(uint32_t, b2) << 16) |          \
                 (C_CAST(uint32_t, b1) <<  8) | (C_CAST(uint32_t, b0) <<  0)  )         \
                                                                   ) )
    // Big endian parameter order, little endian value
    #define M_BytesTo8ByteValue(b7, b6, b5, b4, b3, b2, b1, b0)    (        \
    C_CAST(uint64_t, ( (C_CAST(uint64_t, b7) << 56) | (C_CAST(uint64_t, b6) << 48) |           \
                (C_CAST(uint64_t, b5) << 40) | (C_CAST(uint64_t, b4) << 32) |           \
                (C_CAST(uint64_t, b3) << 24) | (C_CAST(uint64_t, b2) << 16) |           \
                (C_CAST(uint64_t, b1) <<  8) | (C_CAST(uint64_t, b0) <<  0)  )          \
                                                                   ) )

    // Big endian parameter order, little endian value
    #define M_BytesTo2ByteValue(b1, b0)                            (        \
    C_CAST(uint16_t, (  (C_CAST(uint16_t, b1) << 8) | (C_CAST(uint16_t, b0) <<  0)  )          \
                                                                   ) )

    // Big endian parameter order, little endian value
    #define M_WordsTo4ByteValue(w1, w0)                            (        \
    C_CAST(uint32_t, (  (C_CAST(uint32_t, w1) << 16) | (C_CAST(uint32_t, w0) <<  0)  )         \
                                                                   ) )

    #define M_WordsTo8ByteValue(w3, w2, w1, w0)                    (   \
    C_CAST(uint64_t, (  (C_CAST(uint64_t, w3) << 48) | (C_CAST(uint64_t, w2) << 32) |     \
                 (C_CAST(uint64_t, w1) << 16) | (C_CAST(uint64_t, w0) <<  0)  )    \
                                                                   ) )

    // Big endian parameter order, little endian value
    #define M_DWordsTo8ByteValue(d1, d0)                           (        \
    C_CAST(uint64_t, (  (C_CAST(uint64_t, d1) << 32) | (C_CAST(uint64_t, d0) <<  0)  )         \
                                                                   ) )

    // MACRO to round the number of x so that it will not round up when formating the float
    #define ROUNDF(f, c) (((float)((int)((f) * (c))) / (c)))

    #define M_GETBITRANGE(input, msb, lsb) (((input) >> (lsb)) & ~(~UINT64_C(0) << ((msb) - (lsb) + 1)))
    // get bit range for signed values
    #define M_IGETBITRANGE(input, msb, lsb) C_CAST(int64_t, M_GETBITRANGE(input, msb, lsb))

#endif //__cplusplus

// Bit access macros

    #define M_BitN(n)   (UINT64_C(1) << n)

    #define M_BitN8(n)    (UINT8_C(1) << n)
    #define M_BitN16(n)   (UINT16_C(1) << n)
    #define M_BitN32(n)   (UINT32_C(1) << n)
    #define M_BitN64(n)   (UINT64_C(1) << n)


#if !defined(UEFI_C_SOURCE)//defined in EDK2 MdePkg and causes conflicts, so checking this define for now to avoid conflicts

    #define BIT0      (M_BitN8(0))
    #define BIT1      (M_BitN8(1))
    #define BIT2      (M_BitN8(2))
    #define BIT3      (M_BitN8(3))
    #define BIT4      (M_BitN8(4))
    #define BIT5      (M_BitN8(5))
    #define BIT6      (M_BitN8(6))
    #define BIT7      (M_BitN8(7))
    #define BIT8      (M_BitN16(8))
    #define BIT9      (M_BitN16(9))
    #define BIT10     (M_BitN16(10))
    #define BIT11     (M_BitN16(11))
    #define BIT12     (M_BitN16(12))
    #define BIT13     (M_BitN16(13))
    #define BIT14     (M_BitN16(14))
    #define BIT15     (M_BitN16(15))
    #define BIT16     (M_BitN32(16))
    #define BIT17     (M_BitN32(17))
    #define BIT18     (M_BitN32(18))
    #define BIT19     (M_BitN32(19))
    #define BIT20     (M_BitN32(20))
    #define BIT21     (M_BitN32(21))
    #define BIT22     (M_BitN32(22))
    #define BIT23     (M_BitN32(23))
    #define BIT24     (M_BitN32(24))
    #define BIT25     (M_BitN32(25))
    #define BIT26     (M_BitN32(26))
    #define BIT27     (M_BitN32(27))
    #define BIT28     (M_BitN32(28))
    #define BIT29     (M_BitN32(29))
    #define BIT30     (M_BitN32(30))
    #define BIT31     (M_BitN32(31))
    #define BIT32     (M_BitN64(32))
    #define BIT33     (M_BitN64(33))
    #define BIT34     (M_BitN64(34))
    #define BIT35     (M_BitN64(35))
    #define BIT36     (M_BitN64(36))
    #define BIT37     (M_BitN64(37))
    #define BIT38     (M_BitN64(38))
    #define BIT39     (M_BitN64(39))
    #define BIT40     (M_BitN64(40))
    #define BIT41     (M_BitN64(41))
    #define BIT42     (M_BitN64(42))
    #define BIT43     (M_BitN64(43))
    #define BIT44     (M_BitN64(44))
    #define BIT45     (M_BitN64(45))
    #define BIT46     (M_BitN64(46))
    #define BIT47     (M_BitN64(47))
    #define BIT48     (M_BitN64(48))
    #define BIT49     (M_BitN64(49))
    #define BIT50     (M_BitN64(50))
    #define BIT51     (M_BitN64(51))
    #define BIT52     (M_BitN64(52))
    #define BIT53     (M_BitN64(53))
    #define BIT54     (M_BitN64(54))
    #define BIT55     (M_BitN64(55))
    #define BIT56     (M_BitN64(56))
    #define BIT57     (M_BitN64(57))
    #define BIT58     (M_BitN64(58))
    #define BIT59     (M_BitN64(59))
    #define BIT60     (M_BitN64(60))
    #define BIT61     (M_BitN64(61))
    #define BIT62     (M_BitN64(62))
    #define BIT63     (M_BitN64(63))

#endif //UEFI_C_SOURCE

    //set a bit to 1 within a value
    #define M_SET_BIT(val, bitNum) (val |= M_BitN(bitNum))

    #define M_SET_BIT8(val, bitNum) (val |= M_BitN8(bitNum))
    #define M_SET_BIT16(val, bitNum) (val |= M_BitN16(bitNum))
    #define M_SET_BIT32(val, bitNum) (val |= M_BitN32(bitNum))
    #define M_SET_BIT64(val, bitNum) (val |= M_BitN64(bitNum))

    //clear a bit to 0 within a value
    #define M_CLEAR_BIT(val, bitNum) (val &= (~M_BitN(bitNum)))

    #define M_CLEAR_BIT8(val, bitNum) (val &= C_CAST(uint8_t, (~M_BitN8(bitNum)))) //Cast added because UINT8_C() macro most of the time does not add additional qualifiers, so it ends up thinking this is a sign conversion issue.-TJE
    #define M_CLEAR_BIT16(val, bitNum) (val &= C_CAST(uint16_t, (~M_BitN16(bitNum)))) //Cast added because UINT16_C() macro most of the time does not add additional qualifiers, so it ends up thinking this is a sign conversion issue.-TJE
    #define M_CLEAR_BIT32(val, bitNum) (val &= (~M_BitN32(bitNum)))
    #define M_CLEAR_BIT64(val, bitNum) (val &= (~M_BitN64(bitNum)))


    #define M_2sCOMPLEMENT(val) (~(val) + 1)

    //define something called reserved that has a value of zero. Use it to set reserved bytes to 0
    #define RESERVED 0
    #define OBSOLETE 0

    //Defining the M_FALLTHROUGH macro to make it easy to correctly specify when to fallthrough on switch-case statements without extra warnings.
    //This is defined differently depending on c/c++ and which version of the standard is currently being supported by the compiler. In addition
    //some compilers support this and others do not, so this is a little complicated to define in a way that works with everything, but it is
    //attempted below...update this as errors are found. -TJE
    #if defined (__cplusplus)
        //check if the standardized way to check for support is available...
        #if defined __has_cpp_attribute
            #if __has_cpp_attribute(fallthrough)
                //This is the standardized way intriduced in C++17
                #define M_FALLTHROUGH [[fallthrough]];
            #endif
        #endif
    #elif defined __has_c_attribute
        #if __has_c_attribute(fallthrough)
            //C23 style
            #define M_FALLTHROUGH [[fallthrough]];
        #elif __has_c_attribute(__fallthrough__)
            //C23 style
            #define M_FALLTHROUGH [[__fallthrough__]];
        #endif
    #endif
    #if !defined (M_FALLTHROUGH)
        //not C++ and doesn't have __has_c_attribute so do something for older C code
        //Weird case: Clang 3.4.2 (CentOS7) doesn't like the attribute definition below and throws tons
        //of warnings for "declaration does not declare anything", but this is not a problem in newer versions
        //So currently solution to prevent excessive warnings that don't make sense is to define this differently for
        // clang 3 and earlier. This may need adjusting to specific versions of clang later, but this is what I've been able to do so far - TJE
        #if defined (__clang__) && defined (__clang_major__) && __clang_major__ <= 3
            #define M_FALLTHROUGH /*FALLTHRU*/ \

        #elif defined __has_attribute
            //GCC type compiler check
            #if __has_attribute(fallthrough)
                #define M_FALLTHROUGH __attribute__((fallthrough));
            #endif
        #endif
        #if !defined M_FALLTHROUGH
            #if defined (__GNUC__) && __GNUC__ >= 3
                //GCC 3 & 4 support the unused attribute...you just don't have a convenient way to detect it otherwise
                #define M_ATTR_UNUSED __attribute__ ((fallthrough))
            #else
                //Insert a comment instead since other methods were not detected.
                #define M_FALLTHROUGH /*FALLTHRU*/ \
            
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
        //check if the standardized way to check for support is available...
        //NOTE: Checking for >= C++11 for __has_cpp_attribute due to issues where this shows up even when the GNU C++ compiler 
        //is set to 98 or 03 and thows errors for not knowing what to do. Seemed easiest to add this additional version check to get rid of this error
        #if __cplusplus >= 201103L && defined __has_cpp_attribute
            #if __has_cpp_attribute(maybe_unused)
                //This is the standardized way intriduced in C++17
                #define M_ATTR_UNUSED [[maybe_unused]]
            #endif
        #endif
    #elif defined __has_c_attribute //C23
        #if __has_c_attribute(maybe_unused)
            //C23 style
            #define M_ATTR_UNUSED [[maybe_unused]]
        #elif __has_c_attribute(__maybe_unused__) //check for standardized double underscore version
            #define M_DEPRECATED [[__maybe_unused__]]
        #endif
    #endif
    #if !defined (M_ATTR_UNUSED)
        //older C or CPP or no standard way to define this.
        //Use compiler specific checks
        #if defined __has_attribute
            //GCC type compiler check
            #if __has_attribute(unused)
                #define M_ATTR_UNUSED __attribute__ ((unused))
            #endif
        #endif
        #if !defined (M_ATTR_UNUSED)//__has_attribute is available, but doesn't have what we need-TJE
            #if defined (__GNUC__) && __GNUC__ >= 3
                //GCC 3 & 4 support the unused attribute...you just don't have a convenient way to detect it otherwise
                #define M_ATTR_UNUSED __attribute__ ((unused))
            #elif defined (_MSC_VER)
                #define M_ATTR_UNUSED __pragma(warning(suppress:4100 4101)) //4102?
            #else
                //Insert a comment instead since other methods were not detected.
                #define M_ATTR_UNUSED /*UNUSED*/ \
            
            #endif
        #endif
    #endif

    //Macro for marking things as deprecated
    #if defined (__cplusplus)
        //check if the standardized way to check for support is available...
        //NOTE: Checking for >= C++11 for __has_cpp_attribute due to issues where this shows up even when the GNU C++ compiler 
        //is set to 98 or 03 and thows errors for not knowing what to do. Seemed easiest to add this additional version check to get rid of this error
        #if __cplusplus >= 201103L && defined __has_cpp_attribute
            #if __has_cpp_attribute(deprecated)
                //This is the standardized way introduced in C++17
                #define M_DEPRECATED [[deprecated]]
            #endif
        #endif
    #elif defined __has_c_attribute //C23
        #if __has_c_attribute(deprecated)
            //C23 style
            #define M_DEPRECATED [[deprecated]]
        #elif __has_c_attribute(__deprecated__) //check for standardized double underscore version
            #define M_DEPRECATED [[__deprecated__]]
        #endif
    #endif
    #if !defined M_DEPRECATED //standard ways to set this did not work, so try compiler specific means
        #if defined __has_attribute 
            #if __has_attribute(deprecated)
                #define M_DEPRECATED __attribute__ ((deprecated))
            #endif
        #endif  
        #if !defined M_DEPRECATED //if a test macro didn't work above, check the compiler to set this correctly -TJE
            #if defined (__GNUC__) && __GNUC__ >= 3
                //GCC 3 & 4 support the unused attribute...you just don't have a convenient way to detect it otherwise
                #define M_DEPRECATED __attribute__ ((deprecated))
            #elif defined (_MSC_VER)
                #define M_DEPRECATED __declspec(deprecated)
            #else
                //Insert a comment instead since other methods were not detected.
                #define M_DEPRECTED /*DEPRECATED*/ \
            
            #endif
        #endif
    #endif

    //Macro for marking things as nodiscard
    //if a function returns something that should not be ignored, this should be used to mark it and help generate compiler warnings.
    #if defined (__cplusplus)
        //check if the standardized way to check for support is available...
        //NOTE: Checking for >= C++11 for __has_cpp_attribute due to issues where this shows up even when the GNU C++ compiler 
        //is set to 98 or 03 and thows errors for not knowing what to do. Seemed easiest to add this additional version check to get rid of this error
        #if __cplusplus >= 201103L && defined __has_cpp_attribute
            #if __has_cpp_attribute(nodiscard)
                //This is the standardized way introduced in C++17
                #define M_NODISCARD [[nodiscard]]
            #endif
        #endif
    #elif defined __has_c_attribute //C23
        #if __has_c_attribute(nodiscard)
            //C23 style
            #define M_NODISCARD [[nodiscard]]
        #elif __has_c_attribute(__nodiscard__) //check for standardized double underscore version
            #define M_NODISCARD [[__nodiscard__]]
        #endif
    #endif
    #if !defined M_NODISCARD //standard ways to set this did not work, so try compiler specific means
        #if defined __has_attribute 
            #if __has_attribute(nodiscard)
                #define M_NODISCARD __attribute__ ((nodiscard))
            #elif __has_attribute(warn_unused_result)
                #define M_NODISCARD __attribute__ ((warn_unused_result))
            #endif
        #endif  
        #if !defined M_NODISCARD //if a test macro didn't work above, check the compiler to set this correctly -TJE
            #if defined (__GNUC__) && __GNUC__ >= 3
                #define M_NODISCARD __attribute__ ((warn_unused_result))
            #else
                //Insert a comment instead since other methods were not detected.
                #define M_NODISCARD /*NODISCARD*/ \
            
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
    #elif defined __has_c_attribute //C23
        #if __has_c_attribute(noreturn)
            //C23 style
            #define M_NORETURN [[noreturn]]
        #elif __has_c_attribute(__noreturn__) //check for standardized double underscore version
            #define M_NORETURN [[__noreturn__]]
        /* next 2 checks are deprecated versions, but also worth checking */
        #elif __has_c_attribute(___Noreturn__)
            #define M_NORETURN [[___Noreturn__]]
        #endif
    #endif
    #if !defined M_NORETURN //standard ways to set this did not work, so try compiler specific means
        #if defined __has_attribute 
            #if __has_attribute(noreturn)
                #define M_NORETURN __attribute__ ((noreturn))
            #endif
        #endif  
        #if !defined M_NORETURN //if a test macro didn't work above, check the compiler to set this correctly -TJE
            #if defined (__GNUC__) && __GNUC__ >= 3
                //GCC 3 & 4 support the unused attribute...you just don't have a convenient way to detect it otherwise
                #define M_NORETURN __attribute__ ((noreturn))
            #elif defined (_MSC_VER)
                #define M_NORETURN __declspec(noreturn)
            #else
                //Insert a comment instead since other methods were not detected.
                #define M_NORETURN /*NORETURN*/ \
            
            #endif
        #endif
    #endif

    //NULL vs nullptr can be an issue with overloads in C++ and _GENERIC selection in C
    //While NULL should be defined as nullptr in C23, adding this definition to remove ambiguity
    //when we are checking for valid pointers in our code. This will use a proper type depending on the standard detected at compile time
    //https://stackoverflow.com/questions/1282295/what-is-the-nullptr-keyword-and-why-is-it-better-than-null
    #if defined (USING_CPP11)
        #define M_NULLPTR nullptr
    #elif defined (USING_CPP98)
        //NOTE: G++ defines NULL as __null which should be safe
        //      https://gcc.gnu.org/onlinedocs/gcc-4.9.2/libstdc++/manual/manual/support.html#std.support.types.null
        //      Can add a special case to use NULL macro instead if this template class doesn't work as expected
        const class nullptr_t
        {
        public:
           template<class T>
           operator T*() const
              { return 0; }

           template<class C, class T>
              operator T C::*() const
              { return 0; }
        private:
           void operator&() const;
        } nullptr = {};
        #define M_NULLPTR nullptr
    #else //C
        #if defined (USING_C23)
            #define M_NULLPTR nullptr
        #elif defined (NULL)
            //use NULL since this is commonly available and likely to be safe for the environment we are in.
            //NOTE: If this is ever an issue, consider improving this check to fall into the #else below
            //      instead where it is defined as a void* of 0 which should work most of the time unless
            //      the platform is doing something funky.
            //      https://stackoverflow.com/questions/2597142/when-was-the-null-macro-not-0
            #define M_NULLPTR NULL
        #else
            //for unknown reasons NULL was not defined so define it the most common way we can to be safe
            #define M_NULLPTR ((void *)0)
        #endif//C23
    #endif //C & C++ NULL PTR definitions

    //round an integer up to nearest multiple
    //Example: Need to round up to nearest 512B for a data transfer
    #define INT_ROUND_UP(value, roundto) (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value) )

    //round an integer down to nearest multiple
    #define INT_ROUND_DOWN(value, roundto) (((value) % (roundto)) ? ((value) - ((value) % (roundto))) : (value) )

    //Use this macro to declare enums for C and enum classes for C++11 and later.
    //Without enum classes there are a lot of excess warnings and this will make it easier to resolve all warnings.
    #if defined (USING_CPP11)
        #define M_DECLARE_ENUM(name, ...) enum class name { __VA_ARGS__ };
    #elif defined (USING_CPP98)
        #define M_DECLARE_ENUM(name, ...) enum name { __VA_ARGS__ };
    #else //!CPP11
        #define M_DECLARE_ENUM(name, ...) typedef enum _ ## name { __VA_ARGS__ }name;
    #endif //CPP11

    //If you want a specific type underlying the enum, use M_DECLAR_ENUM_TYPE to specify it.
    //This can be something like M_DECLARE_ENUM_TYPE(myEnum, uint32_t, ...)
    //If you set type to "int" then it's the same as M_DECLARE_ENUM
    #if defined (USING_CPP11)
        #define M_DECLARE_ENUM_TYPE(name, type, ...) enum class name : type { __VA_ARGS__ };
    #elif defined (USING_CPP98)
        #define M_DECLARE_ENUM_TYPE(name, type, ...) enum name { __VA_ARGS__ };
    #else//!CPP11...old CPP or C
        #if defined (USING_C23)
            #define M_DECLARE_ENUM_TYPE(name, type, ...) typedef enum _ ## name : type { __VA_ARGS__ }name;
        #else
            /*cannot specify the type, so just ignore the input for now-TJE*/
            #define M_DECLARE_ENUM_TYPE(name, type, ...) typedef enum _ ## name { __VA_ARGS__ }name;
        #endif
    #endif//CPP11 

    M_DECLARE_ENUM(eReturnValues,
        SUCCESS = 0,
        FAILURE = 1,
        NOT_SUPPORTED = 2,
        COMMAND_FAILURE = 3,
        IN_PROGRESS = 4, /* another command is in progress, or a command has started and is now in progress in the background */
        ABORTED = 5, /* Command was aborted */
        BAD_PARAMETER = 6, /* the only time this return code should be seen is when a developer is writing code to add something. This should not appear in a finished application */
        MEMORY_FAILURE = 7, /* could not allocate memory */
        OS_PASSTHROUGH_FAILURE = 8, /* For some unknown reason, the OS API call to issue the pass-through command failed. */
        LIBRARY_MISMATCH = 9,
        FROZEN = 10, /* use this to communicate back when the device is in a frozen state for a commmand like sanitize or ata security */
        PERMISSION_DENIED = 11, /* OS returned Access/permission denied */
        FILE_OPEN_ERROR = 12,
        WARN_INCOMPLETE_RFTRS = 13, /* command was issued, and some RTFRs were received, but we were unable to get a complete RTFR result. This is most likely due to a SATL limitation. */
        OS_COMMAND_TIMEOUT = 14, /* command took longer than the timeout that was provided to the OS */
        WARN_NOT_ALL_DEVICES_ENUMERATED = 15,
        WARN_INVALID_CHECKSUM = 16, /* The checksum on the data for a command didn't calculate correctly (EX: Identify device, some ATA Logs) */
        OS_COMMAND_NOT_AVAILABLE = 17, /* This is returned when the OS does not have a way to issue the requested command. (EX: Trying to send an NVMe command without Win10, or trying a 32byte SCSI command pre-Win8) */
        OS_COMMAND_BLOCKED = 18, /* This is returned when the OS is blocking the command from being issued (EX: TCG - linux, lib ATA......or Sanitize in Windos 8+) */
        COMMAND_INTERRUPTED = 19, /* Added for SCT commands, if interrupted by some other SCT command. */
        VALIDATION_FAILURE = 20, /* validation check*/
        STRIP_HDR_FOOTER_FAILURE = 21, /* For UDS */
        PARSE_FAILURE = 22,
        INVALID_LENGTH = 23,  /* Binary file has a invalid length or the parameters for the length don't match the size of the file */
        ERROR_WRITING_FILE = 24, /*added for fwrite check on May20'20 */
        TIMEOUT = 25, /*added for indicating operation timeout for SeaQueue */
        OS_TIMEOUT_TOO_LARGE = 26, /*added for cases where a requested timeout is larger than the OS is capable of supporting in passthrough */
        PARSING_EXCEPTION_FAILURE = 27, /*For C/C++ exception failure while parsing */
        DIR_CREATION_FAILED = 28, /*For Telemetry Log Parser, when the creation of output folder fails */
        FILE_READ_ERROR = 29, /*For Telemetry Log Parser, when reading logfile to buffer fails */
        POWER_CYCLE_REQUIRED = 30, /*For some firmware update scenarios, a power cycle is required to complete the update. This code is returned in these situations. */
        DEVICE_ACCESS_DENIED = 31, /*While similar to PERMISSION_DENIED, this is meant for a response from the drive telling us it is not allowing something versus the OS telling us something is not allowed.-TJE */
        NOT_PARSED = 32,           /* File was not parsed */
        MISSING_INFORMATION = 33,  /* file is missing information inorder to be parsed correctly */
        TRUNCATED_FILE = 34,       /* file is truncated and might be missing data for parsing it correctly */
        UNKNOWN
    )

    M_DECLARE_ENUM(eDataTransferDirection,
        XFER_NO_DATA,
        XFER_DATA_IN,     /* Transfer from target to host                             */
        XFER_DATA_OUT,    /* Transfer from host to target                             */
        XFER_DATA_OUT_IN, /* Transfer from host to target, followed by target to host */
        XFER_DATA_IN_OUT, /* Transfer from target to host, followed by host to target */
    )

    M_DECLARE_ENUM(eVerbosityLevels,
        VERBOSITY_QUIET             = 0,
        VERBOSITY_DEFAULT           = 1,
        VERBOSITY_COMMAND_NAMES     = 2,
        VERBOSITY_COMMAND_VERBOSE   = 3,
        VERBOSITY_BUFFERS           = 4,
        VERBOSITY_MAX /*This should always be at the end of this list. This allows for a function to use this to check bounds even if we add more levels ahead of this. -TJE*/
    )
    // json data type sets. used for formating data to a customer demands 
    M_DECLARE_ENUM(eDataFormat,
        JSON_DATA = 0,   /*default*/
        PREPYTHON_DATA = 1,
        )

    // Max & Min Helpers
    #define  M_Min(a,b)    (((a)<(b))?(a):(b))
    #define  M_Max(a,b)    (((a)>(b))?(a):(b))

    // Convert the result to a boolean true or false (ternary operator)
    // This is especially good for C++ since you get conversion warnings, but this is an acceptible method for C++ code
    #define M_ToBool(expression) ((expression) > 0 ? true : false)

    M_DECLARE_ENUM(eOutputFormat,
        SEAC_OUTPUT_TEXT, /*default way everything is output            */
        SEAC_OUTPUT_RAW,  /*This will output the data as raw binary glob*/
        SEAC_OUTPUT_JSON,
        /*TODO: add other output formats as we want to support them*/
        SEAC_OUTPUT_CSV, /*just an idea...isn't implemented now*/
        SEAC_OUTPUT_XML, /*just an idea...isn't implemented now*/
        )


    //NOTE: I don't like how this is done right now...the hardcoded path and filename lengths are the same as OPENSEA_PATH_MAX, but I cannot use that definition due to include structure at this time...
    //Might need to add some other include to handle output format stuff...
    typedef struct _OutputInfo
    {
        eOutputFormat outputFormat;
        FILE *outputFilePtr;
        const char **outputPath;//must be NULL terminated
        const char **outputFileName;//must be NULL terminated, outputPath + outputFileName should be no bigger than OPENSEA_PATH_MAX or files may not go to the right place
        const char **utilityName;//can be NULL. If given, it may be used in JSON file creation
        const char **utilityVersion;//can be NULL. If given, it may be used in JSON file creation
    }OutputInfo;

    // Data buffer pointer and length block.
    typedef struct _tDataPtr {
       void* pData;      // If pData is NULL, AllocLen must be zero also.
       size_t DataLen;   // Number of valid bytes.
       size_t AllocLen;  // If AllocLen is zero, pData must be NULL also.
    } tDataPtr;

    #define TIME_STRING_LENGTH 26
    #define CURRENT_TIME_STRING_LENGTH TIME_STRING_LENGTH
    // global timestamp
    extern time_t CURRENT_TIME;
    extern char CURRENT_TIME_STRING[CURRENT_TIME_STRING_LENGTH];

    // get the current timestamp and use same for all log pulls and utility banner.
    bool get_current_timestamp(void);

    //-----------------------------------------------------------------------------
    //
    //  delay_Milliseconds()
    //
    //! \brief   Description:  delay for a specified number of milliseconds
    //
    //  Entry:
    //!   \param[in] milliseconds = number of milliseconds to delay for
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void delay_Milliseconds(uint32_t milliseconds);
    //-----------------------------------------------------------------------------
    //
    //  delay_Seconds()
    //
    //! \brief   Description:  delay for a specified number of seconds
    //
    //  Entry:
    //!   \param[in] seconds = number of seconds to delay for
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void delay_Seconds(uint32_t seconds);

    //-----------------------------------------------------------------------------
    //
    //  nibble_Swap()
    //
    //! \brief   Description:  swap the nibbles in a byte
    //
    //  Entry:
    //!   \param[out] byteToSwap = a pointer to the byte containing the data in which to have the nibbles swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void nibble_Swap(uint8_t *byteToSwap);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_16()
    //
    //! \brief   Description:  swap the bytes in a word
    //
    //  Entry:
    //!   \param[out] wordToSwap = a pointer to the word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_16(uint16_t *wordToSwap);
    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_Int16()
    //
    //! \brief   Description:  swap the bytes in a singned word 
    //
    //  Entry:
    //!   \param[out] signedWordToSwap = a pointer to the signed word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_Int16(int16_t *signedWordToSwap);
    //-----------------------------------------------------------------------------
    //
    //  big_To_Little_Endian_16()
    //
    //! \brief   Description:  swap the bytes in a word only if on little endian system. 
    //
    //  Entry:
    //!   \param[out] wordToSwap = a pointer to the word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void big_To_Little_Endian_16(uint16_t *wordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_32()
    //
    //! \brief   Description:  swap the bytes in a double word - useful for converting between endianness
    //
    //  Entry:
    //!   \param[out] doubleWordToSwap = a pointer to the double word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_32(uint32_t *doubleWordToSwap);
    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_Int32()
    //
    //! \brief   Description:  swap the bytes in a Signed double word - useful for converting between endianness
    //
    //  Entry:
    //!   \param[out] signedDWord = a pointer to the Signed double word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_Int32(int32_t *signedDWord);
    //-----------------------------------------------------------------------------
    //
    //  big_To_Little_Endian_32()
    //
    //! \brief   Description:  swap the bytes in a double word only if running on little endian system
    //
    //  Entry:
    //!   \param[out] doubleWordToSwap = a pointer to the double word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void big_To_Little_Endian_32(uint32_t *doubleWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  word_Swap_32()
    //
    //! \brief   Description:  swap the words in a double word
    //
    //  Entry:
    //!   \param[out] doubleWordToSwap = a pointer to the double word containing the data in which to have the words swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void word_Swap_32(uint32_t *doubleWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_64()
    //
    //! \brief   Description:  swap the bytes in a quad word - useful for conversion between endianness
    //
    //  Entry:
    //!   \param[out] quadWordToSwap = a pointer to the quad word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_64(uint64_t *quadWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  word_Swap_64()
    //
    //! \brief   Description:  swap the words in a quad word
    //
    //  Entry:
    //!   \param[out] quadWordToSwap = a pointer to the quad word containing the data in which to have the words swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void word_Swap_64(uint64_t *quadWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  double_Word_Swap_64()
    //
    //! \brief   Description:  swap the double words in a quad word
    //
    //  Entry:
    //!   \param[out] quadWordToSwap = a pointer to the quad word containing the data in which to have the double words swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void double_Word_Swap_64(uint64_t *quadWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  celsius_To_Fahrenheit()
    //
    //! \brief   Description:  convert a temperature in celsius to fahrenheit
    //
    //  Entry:
    //!   \param[in] celsius = a pointer to the data containing a temperature in celsius
    //!
    //  Exit:
    //!   \return The temperature in fahrenheit
    //
    //-----------------------------------------------------------------------------
    int16_t celsius_To_Fahrenheit(int16_t *celsius);

    //-----------------------------------------------------------------------------
    //
    //  fahrenheit_To_celsius()
    //
    //! \brief   Description:  convert a temperature in fahrenheit to celsius
    //
    //  Entry:
    //!   \param[in] fahrenheit = a pointer to the data containing a temperature in fahrenheit
    //!
    //  Exit:
    //!   \return The temperature in celsius
    //
    //-----------------------------------------------------------------------------
    int16_t fahrenheit_To_celsius(int16_t *fahrenheit);

    //-----------------------------------------------------------------------------
    //
    //  celsius_To_Kelvin()
    //
    //! \brief   Description:  convert a temperature in celsius to kelvin
    //
    //  Entry:
    //!   \param[in] celsius = a pointer to the data containing a temperature in celsius
    //!
    //  Exit:
    //!   \return The temperature in celsius
    //
    //-----------------------------------------------------------------------------
    int16_t celsius_To_Kelvin(int16_t *celsius);

    //-----------------------------------------------------------------------------
    //
    //  fahrenheit_To_Kelvin()
    //
    //! \brief   Description:  convert a temperature in fahrenheit to kelvin
    //
    //  Entry:
    //!   \param[in] fahrenheit = a pointer to the data containing a temperature in fahrenheit
    //!
    //  Exit:
    //!   \return The temperature in celsius
    //
    //-----------------------------------------------------------------------------
    int16_t fahrenheit_To_Kelvin(int16_t *fahrenheit);

    //-----------------------------------------------------------------------------
    //
    //  kelvin_To_Celsius()
    //
    //! \brief   Description:  convert a temperature in kelvin to celsius
    //
    //  Entry:
    //!   \param[in] kelvin = a pointer to the data containing a temperature in kelvin
    //!
    //  Exit:
    //!   \return The temperature in celsius
    //
    //-----------------------------------------------------------------------------
    int16_t kelvin_To_Celsius(int16_t *kelvin);

    //-----------------------------------------------------------------------------
    //
    //  kelvin_To_Fahrenheit()
    //
    //! \brief   Description:  convert a temperature in kelvin to fahrenheit
    //
    //  Entry:
    //!   \param[in] kelvin = a pointer to the data containing a temperature in kelvin
    //!
    //  Exit:
    //!   \return The temperature in celsius
    //
    //-----------------------------------------------------------------------------
    int16_t kelvin_To_Fahrenheit(int16_t *kelvin);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_String()
    //
    //! \brief   Description:  swap the bytes in a string. This is useful when interpretting ATA strings
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string that needs to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_String(char *stringToChange);

    //-----------------------------------------------------------------------------
    //
    //  remove_Whitespace_Left()
    //
    //! \brief   Description:  remove the whitespace at the beginning of a string with no repeating first char in string
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string that needs to have the beginning whitespace removed
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void remove_Whitespace_Left(char *stringToChange);

    //-----------------------------------------------------------------------------
    //
    //  remove_Trailing_Whitespace()
    //
    //! \brief   Description:  remove the whitespace at the end of a string
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string that needs to have the ending whitespace removed
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void remove_Trailing_Whitespace(char *stringToChange);

    //-----------------------------------------------------------------------------
    //
    //  remove_Leading_Whitespace()
    //
    //! \brief   Description:  remove the whitespace at the beginning of a string
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string that needs to have the beginning whitespace removed
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void remove_Leading_Whitespace(char *stringToChange);

    //-----------------------------------------------------------------------------
    //
    //  remove_Leading_And_Trailing_Whitespace()
    //
    //! \brief   Description:  remove the whitespace at the beginning and end of a string
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string that needs to have the beginning and ending whitespace removed
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void remove_Leading_And_Trailing_Whitespace(char *stringToChange);

    //-----------------------------------------------------------------------------
    //
    //  convert_String_To_Upper_Case()
    //
    //! \brief   Description:  convert a string to all uppercase letters
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string that needs to have all the characters converted to uppercase
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void convert_String_To_Upper_Case(char *stringToChange);

    //-----------------------------------------------------------------------------
    //
    //  convert_String_To_Lower_Case()
    //
    //! \brief   Description:  convert a string to all lowercase letters
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string that needs to have all the characters converted to lowercase
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void convert_String_To_Lower_Case(char *stringToChange);

    //-----------------------------------------------------------------------------
    //
    //   convert_String_To_Inverse_Case(char *stringToChange)
    //
    //! \brief   Description:  convert uppercase characters to lowercase and lowercase characters to uppercase in a string.
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string that will be modified
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void convert_String_To_Inverse_Case(char *stringToChange);

    //-----------------------------------------------------------------------------
    //
    //   find_last_occurrence_in_string(char *originalString, char *stringToFind)
    //
    //! \brief   Description:  convert uppercase characters to lowercase and lowercase characters to uppercase in a string.
    //
    //  Entry:
    //!   \param[in] originalString = a pointer to the data containing a string that will be searched(superset)
    //!   \param[in] stringToFind = a pointer to the data containing a string that is to be searched(subset)
    //!
    //  Exit:
    //!   \return size_t = last occurence of 'stringToFind' in 'originalString'
    //
    //-----------------------------------------------------------------------------
    size_t find_last_occurrence_in_string(const char *originalString, const char *stringToFind);

    //-----------------------------------------------------------------------------
    //
    //   find_first_occurrence_in_string(char *originalString, char *stringToFind)
    //
    //! \brief   Description:  convert uppercase characters to lowercase and lowercase characters to uppercase in a string.
    //
    //  Entry:
    //!   \param[in] originalString = a pointer to the data containing a string that will be searched(superset)
    //!   \param[in] stringToFind = a pointer to the data containing a string that is to be searched(subset)
    //!
    //  Exit:
    //!   \return size_t = first occurence of 'stringToFind' in 'originalString'
    //
    //-----------------------------------------------------------------------------
    size_t find_first_occurrence_in_string(char *originalString, char *stringToFind);

    //-----------------------------------------------------------------------------
    //
    //   wildcard_Match(char* pattern, char* data)
    //
    //! \brief   Description:  match the string with pattern consisting wildcard chars. A single char wildcard match is '?'
    //  and multi char wildcard match is '*'
    //
    //  Entry:
    //!   \param[in] pattern = a pointer to the pattern consisting wildcard chars
    //!   \param[in] data = a pointer to the data
    //!
    //  Exit:
    //!   \return bool = true is matches, false if not
    //
    //-----------------------------------------------------------------------------
    bool wildcard_Match(char* pattern, char* data);

    //-----------------------------------------------------------------------------
    //
    //  print_Data_Buffer()
    //
    //! \brief   Description:  print out a data buffer to the screen
    //
    //  Entry:
    //!   \param[in] dataBuffer = a pointer to the data buffer you want to print out
    //!   \param[in] bufferLen = the length that you want to print out. This can be the length of the buffer, or anything less than that
    //!   \param[in] showPrint = set to true to show printable characters on the side of the hex output for the buffer. Non-printable characters will be represented as dots.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Data_Buffer(uint8_t *dataBuffer, uint32_t bufferLen, bool showPrint);

    //-----------------------------------------------------------------------------
    //
    //  print_Pipe_Data()
    //
    //! \brief   Description:  print out a data buffer for piping to the next executable to the screen
    //
    //  Entry:
    //!   \param[in] dataBuffer = a pointer to the data buffer you want to print out
    //!   \param[in] bufferLen = the length that you want to print out. This can be the length of the buffer, or anything less than that
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Pipe_Data(uint8_t* dataBuffer, uint32_t bufferLen);

    //-----------------------------------------------------------------------------
    //
    //  print_Return_Enum()
    //
    //! \brief   Description:  Prints humanized eReturnValue for a given return value
    //
    //  Entry:
    //!   \param[in] funcName = Name of function value returning from.
    //!   \param[in] ret = value to humanize
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Return_Enum(const char *funcName, eReturnValues ret);

#define UNIT_STRING_LENGTH 4

    //-----------------------------------------------------------------------------
    //
    //  metric_Unit_Convert()
    //
    //! \brief   Description:  Takes a double value which is a a numberic value specifying a number of bytes, and 
    //!                        converts it to KB, MB, GB, TB, etc in a more easy to display manor. Sets the units for you for display purposes too.
    //
    //  Entry:
    //!   \param[in] byteValue = value specifying a number of bytes
    //!   \param[in] metricUnit = char ptr to hold the metric unit. 
    //!
    //  Exit:
    //!   \return SUCCESS on successful completion, !SUCCESS if problems encountered
    //
    //-----------------------------------------------------------------------------
    eReturnValues metric_Unit_Convert(double *byteValue, char** metricUnit);

    //-----------------------------------------------------------------------------
    //
    //  capacity_Unit_Convert()
    //
    //! \brief   Description:  Takes a double value which is a a numeric value specifying a number of bytes, and 
    //!                        converts it to KiB, MiB, GiB, TiB, etc in a more easy to display manor. Sets the units for you for display purposes too.
    //
    //  Entry:
    //!   \param[in] byteValue = value specifying a number of bytes
    //!   \param[in] capacityUnit = char ptr to hold the metric unit. 
    //!
    //  Exit:
    //!   \return SUCCESS on successful completion, !SUCCESS if problems encountered
    //
    //-----------------------------------------------------------------------------
    eReturnValues capacity_Unit_Convert(double *byteValue, char** capacityUnit);

    //-----------------------------------------------------------------------------
    //
    //  safe_Free()
    //
    //! \brief   Description:  Safely free dynamically allocated memory. This checks for a valid pointer, then frees it and set's it to NULL.
    //
    //  Entry:
    //!   \param[in] mem - heap memory you want to free.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    #define safe_Free(mem)  \
    if (mem)                \
    {                       \
        free(mem);          \
        (mem) = NULL;       \
    }                       \

    //-----------------------------------------------------------------------------
    //
    //  convert_Seconds_To_Displayable_Time()
    //
    //! \brief   Description:  Takes a time in seconds and converts it to something more friendly to display. Any input NULL pointers will be skipped.
    //
    //  Entry:
    //!   \param[in] secondsToConvert - a value specifying the number of seconds to convert
    //!   \param[out] years = this is a pointer to a value to hold a number representing years. Can be NULL.
    //!   \param[out] days = this is a pointer to a value to hold a number representing days. Can be NULL.
    //!   \param[out] hours = this is a pointer to a value to hold a number representing hours. Can be NULL.
    //!   \param[out] minutes = this is a pointer to a value to hold a number representing minutes. Can be NULL.
    //!   \param[out] seconds = this is a pointer to a value to hold a number representing seconds. Can be NULL.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void convert_Seconds_To_Displayable_Time(uint64_t secondsToConvert, uint8_t *years, uint16_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

    void convert_Seconds_To_Displayable_Time_Double(double secondsToConvert, uint8_t *years, uint16_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

    //-----------------------------------------------------------------------------
    //
    //  get_And_Validate_Integer_Input()
    //
    //! \brief   Description:   Validates an input as unsigned integer & converts it to unsigned type.
    //!                         This function supports hex values such as 0xFF AEh etc.     
    //
    //  Entry:
    //!   \param[in] strToConvert - const char * buffer to convert to integer
    //!   \param[out] outputInteger - pointer to integer to store the output
    //!
    //  Exit:
    //!   \return true if able to read in an integer number, false if invalid format.
    //
    //-----------------------------------------------------------------------------
    M_DEPRECATED /*user the bit width specific versions instead!*/
    bool get_And_Validate_Integer_Input(const char * strToConvert, uint64_t * outputInteger);

    //This enum specifies which units are allowed to be at the end of the user's input
    //it must match exactly the units described in the comments. If we need to expand the
    //list of allowed units in a given input type, that is OK. All callers should be using the output unit
    //as needed and any unknown unit must be treated as an error that way if these lists get expanded,
    //a new unit does not cause unexpected behavior.
    M_DECLARE_ENUM(eAllowedUnitInput,
        ALLOW_UNIT_NONE, /*no units allowed.*/
        ALLOW_UNIT_DATASIZE,/*BLOCKS, SECTORS, B, KB, KiB, MB, MiB, GB, GiB, TB, TiB*/
        ALLOW_UNIT_SECTOR_TYPE,/*l, p, logical, physical*/
        ALLOW_UNIT_TIME,/*h, m, s, ms, us, ns*/
        ALLOW_UNIT_POWER,/*w, mw*/
        ALLOW_UNIT_VOLTS,/*v, mv*/
        ALLOW_UNIT_AMPS, /*a, ma*/
        ALLOW_UNIT_TEMPERATURE/*f, c, k*/
    )

    M_NODISCARD bool get_And_Validate_Integer_Input_Uint64(const char * strToConvert, char** unit, eAllowedUnitInput unittype, uint64_t * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Uint32(const char * strToConvert, char** unit, eAllowedUnitInput unittype, uint32_t * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Uint16(const char * strToConvert, char** unit, eAllowedUnitInput unittype, uint16_t * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Uint8(const char * strToConvert, char** unit, eAllowedUnitInput unittype, uint8_t * outputInteger);

    M_NODISCARD bool get_And_Validate_Integer_Input_Int64(const char * strToConvert, char** unit, eAllowedUnitInput unittype, int64_t * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Int32(const char * strToConvert, char** unit, eAllowedUnitInput unittype, int32_t * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Int16(const char * strToConvert, char** unit, eAllowedUnitInput unittype, int16_t * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Int8(const char * strToConvert, char** unit, eAllowedUnitInput unittype, int8_t * outputInteger);

    M_NODISCARD bool get_And_Validate_Integer_Input_ULL(const char * strToConvert, char** unit, eAllowedUnitInput unittype, unsigned long long * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_UL(const char * strToConvert, char** unit, eAllowedUnitInput unittype, unsigned long * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_UI(const char * strToConvert, char** unit, eAllowedUnitInput unittype, unsigned int * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_US(const char * strToConvert, char** unit, eAllowedUnitInput unittype, unsigned short * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_UC(const char * strToConvert, char** unit, eAllowedUnitInput unittype, unsigned char * outputInteger);

    M_NODISCARD bool get_And_Validate_Integer_Input_LL(const char * strToConvert, char** unit, eAllowedUnitInput unittype, long long * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_L(const char * strToConvert, char** unit, eAllowedUnitInput unittype, long * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_I(const char * strToConvert, char** unit, eAllowedUnitInput unittype, int * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_S(const char * strToConvert, char** unit, eAllowedUnitInput unittype, short * outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_C(const char * strToConvert, char** unit, eAllowedUnitInput unittype, char * outputInteger);

    M_NODISCARD bool get_And_Validate_Float_Input(const char* strToConvert, char** unit, eAllowedUnitInput unittype, float * outputFloat);
    M_NODISCARD bool get_And_Validate_Double_Input(const char* strToConvert, char** unit, eAllowedUnitInput unittype, double * outputFloat);
    M_NODISCARD bool get_And_Validate_LDouble_Input(const char* strToConvert, char** unit, eAllowedUnitInput unittype, long double * outputFloat);

    #if defined(USING_C11)
    //This is a generic selection macro.
    //so based on the type of X, it will call the appropriate function for that type.
    //similar to an overloaded function in C++ for different types.
    //NOTE: Not using intX_t or uintX_t since these are type defs to one of the types in the macro below.
    //NOTE: No default case so we can get a warning when this doesn't expand correctly.
    #define get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger) _Generic((outputInteger), \
                        char*: get_And_Validate_Integer_Input_C,\
                        unsigned char*: get_And_Validate_Integer_Input_UC,\
                        short*: get_And_Validate_Integer_Input_S,\
                        unsigned short*: get_And_Validate_Integer_Input_US,\
                        int*: get_And_Validate_Integer_Input_I,\
                        unsigned int*: get_And_Validate_Integer_Input_UI,\
                        long*: get_And_Validate_Integer_Input_L,\
                        unsigned long*: get_And_Validate_Integer_Input_UL,\
                        long long*: get_And_Validate_Integer_Input_LL,\
                        unsigned long long*: get_And_Validate_Integer_Input_ULL,\
                        float*: get_And_Validate_Float_Input,\
                        double*: get_And_Validate_Double_Input,\
                        long double*: get_And_Validate_LDouble_Input\
                        )(strToConvert, unit, unittype, outputInteger)
    #endif //C11

    
    //-----------------------------------------------------------------------------
    //
    //  print_Time_To_Screen()
    //
    //! \brief   Description:  Takes a time represented by pointers and prints it to the screen in a friendly way. It will print a space before and after the time for you. If you do not wish to show one of the parameters, it may be null as it will be skipped. Values of zero are also skipped.
    //
    //  Entry:
    //!   \param[in] years = this is a pointer to a value holding a number representing years. Can be NULL.
    //!   \param[in] days = this is a pointer to a value holding a number representing days. Can be NULL.
    //!   \param[in] hours = this is a pointer to a value holding a number representing hours. Can be NULL.
    //!   \param[in] minutes = this is a pointer to a value holding a number representing minutes. Can be NULL.
    //!   \param[in] seconds = this is a pointer to a value holding a number representing seconds. Can be NULL.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Time_To_Screen(uint8_t *years, uint16_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

    extern uint32_t seed32Array[2];
    extern uint64_t seed64Array[2];
    //-----------------------------------------------------------------------------
    //
    //  seed_32()
    //
    //! \brief   Description:  Takes a 32bit unsigned integer for the seed start and plugs it into the seed32Array in 0. 1 is filled with seed>>1, then the random number generator is run twice replacing each seed value in the array
    //
    //  Entry:
    //!   \param[in] seed = value to use as a seed for the random number generator
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void seed_32(uint32_t seed);//start the seed for random number generation

    //-----------------------------------------------------------------------------
    //
    //  seed_64()
    //
    //! \brief   Description:  Takes a 64bit unsigned integer for the seed start and plugs it into the seed64Array in 0. 1 is filled with seed>>2, then the random number generator is run twice replacing each seed value in the array
    //
    //  Entry:
    //!   \param[in] seed = value to use as a seed for the random number generator
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void seed_64(uint64_t seed);//start the seed for random number generation

    //-----------------------------------------------------------------------------
    //
    //  xorshiftplus32()
    //
    //! \brief   Description:  Generate a 32bit (psuedo) random number using the xorshiftplus alorithm http://en.wikipedia.org/wiki/Xorshift. This was chosen for implemtnation for being fast and light weight
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return random number
    //
    //-----------------------------------------------------------------------------
    uint32_t xorshiftplus32(void);

    //-----------------------------------------------------------------------------
    //
    //  xorshiftplus64()
    //
    //! \brief   Description:  Generate a 64bit (psuedo) random number using the xorshiftplus alorithm http://en.wikipedia.org/wiki/Xorshift. This was chosen for implemtnation for being fast and light weight
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return random number
    //
    //-----------------------------------------------------------------------------
    uint64_t xorshiftplus64(void);

    //-----------------------------------------------------------------------------
    //
    //  random_Range_32()
    //
    //! \brief   Description:  Generate a 32bit (psuedo) random number using the xorshiftplus alorithm within the specified range (slightly biased)
    //
    //  Entry:
    //!   \param[in] rangeMin = value to use for minimum value of range
    //!   \param[in] rangeMax = value to use for maximum value of range
    //!
    //  Exit:
    //!   \return random number
    //
    //-----------------------------------------------------------------------------
    uint32_t random_Range_32(uint32_t rangeMin, uint32_t rangeMax);

    //-----------------------------------------------------------------------------
    //
    //  random_Range_64()
    //
    //! \brief   Description:  Generate a 64bit (psuedo) random number using the xorshiftplus alorithm within the specified range (slightly biased)
    //
    //  Entry:
    //!   \param[in] rangeMin = value to use for minimum value of range
    //!   \param[in] rangeMax = value to use for maximum value of range
    //!
    //  Exit:
    //!   \return random number
    //
    //-----------------------------------------------------------------------------
    uint64_t random_Range_64(uint64_t rangeMin, uint64_t rangeMax);

    uint64_t power_Of_Two(uint16_t exponent);

    double raise_to_power(double number, double power);

    //-----------------------------------------------------------------------------
    //
    //  fill_Random_Pattern_In_Buffer(uint8_t *ptrData, uint32_t dataLength)
    //
    //! \brief   Description:  Fills a buffer with random data. Data length must be at least sizeof(uint32_t) in size (4 bytes)
    //
    //  Entry:
    //!   \param[out] ptrData = pointer to the data buffer to fill
    //!   \param[in] dataLength = size of the data buffer in bytes (must be at least 4 bytes in size)
    //!
    //  Exit:
    //!   \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in function parameters
    //
    //-----------------------------------------------------------------------------
    eReturnValues fill_Random_Pattern_In_Buffer(uint8_t *ptrData, uint32_t dataLength);

    //-----------------------------------------------------------------------------
    //
    //  fill_Hex_Pattern_In_Buffer(uint32_t hexPattern, uint8_t *ptrData, uint32_t dataLength)
    //
    //! \brief   Description:  Fills a buffer with a hexadecimal pattern. The pattern is 32bits in length.
    //
    //  Entry:
    //!   \param[in] hexPattern = 32bit hexadecimal pattern to put into buffer.
    //!   \param[out] ptrData = pointer to the data buffer to fill
    //!   \param[in] dataLength = size of the data buffer in bytes (must be at least 4 bytes in size)
    //!
    //  Exit:
    //!   \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in function parameters
    //
    //-----------------------------------------------------------------------------
    eReturnValues fill_Hex_Pattern_In_Buffer(uint32_t hexPattern, uint8_t *ptrData, uint32_t dataLength);

    //-----------------------------------------------------------------------------
    //
    //  fill_Incrementing_Pattern_In_Buffer(uint8_t incrementStartValue, uint8_t *ptrData, uint32_t dataLength)
    //
    //! \brief   Description:  Fills a buffer with a incrementing pattern from a starting point
    //
    //  Entry:
    //!   \param[in] incrementStartValue = value to start incrementing from.
    //!   \param[out] ptrData = pointer to the data buffer to fill
    //!   \param[in] dataLength = size of the data buffer in bytes.
    //!
    //  Exit:
    //!   \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in function parameters
    //
    //-----------------------------------------------------------------------------
    eReturnValues fill_Incrementing_Pattern_In_Buffer(uint8_t incrementStartValue, uint8_t *ptrData, uint32_t dataLength);

    //-----------------------------------------------------------------------------
    //
    //  fill_ASCII_Pattern_In_Buffer(const char *asciiPattern, uint32_t patternLength, uint8_t *ptrData, uint32_t dataLength)
    //
    //! \brief   Description:  Fills a buffer with an ASCII pattern (string).
    //
    //  Entry:
    //!   \param[in] asciiPattern = pointer to ASCII pattern (string)
    //!   \param[in] patternLength = length of the ASCII pattern
    //!   \param[out] ptrData = pointer to the data buffer to fill
    //!   \param[in] dataLength = size of the data buffer in bytes.
    //!
    //  Exit:
    //!   \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in function parameters
    //
    //-----------------------------------------------------------------------------
    eReturnValues fill_ASCII_Pattern_In_Buffer(const char *asciiPattern, uint32_t patternLength, uint8_t *ptrData, uint32_t dataLength);

    //-----------------------------------------------------------------------------
    //
    //  fill_Pattern_Buffer_Into_Another_Buffer(uint8_t *inPattern, uint32_t inpatternLength, uint8_t *ptrData, uint32_t dataLength)
    //
    //! \brief   Description:  Fills a buffer from a pattern memory buffer and will repeat the pattern to fill the out buffer. (i.e., 1 sector in gets copied and put into all 4 sectors out)
    //
    //  Entry:
    //!   \param[in] inPattern = pointer to a uint8 buffer
    //!   \param[in] inpatternLength = length of the incomming pattern
    //!   \param[out] ptrData = pointer to the data buffer to fill
    //!   \param[in] dataLength = size of the data buffer in bytes.
    //!
    //  Exit:
    //!   \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in function parameters
    //
    //-----------------------------------------------------------------------------
    eReturnValues fill_Pattern_Buffer_Into_Another_Buffer(uint8_t *inPattern, uint32_t inpatternLength, uint8_t *ptrData, uint32_t dataLength);

    double convert_128bit_to_double(uint8_t * pData);

    //meant to replace calling gmtime() function as this handles all the cross-platform weirdness calling the safest possible version
    struct tm * get_UTCtime(const time_t *timer, struct tm *buf);

    //meant to replace calling localtime() function as this handles all the cross-platform weirdness calling the safest possible version
    struct tm * get_Localtime(const time_t *timer, struct tm *buf);

    //meant to replace calling asctime() function as this handles all the cross-platform weirdness calling the safest possible version
    char * get_Time_String_From_TM_Structure(const struct tm * timeptr, char *buffer, size_t bufferSize);

    //meant to replace calling ctime() function as this handles all the cross-platform weirdness calling the safest possible version
    char* get_Current_Time_String(const time_t* timer, char *buffer, size_t bufferSize);

    //-----------------------------------------------------------------------------
    //
    //  get_Future_Date_And_Time()
    //
    //! \brief   Description:  Takes an input time, and an amount of time in the future, and returns it in a time_t type. This function exists because C standard does not specify how time_t is stored.
    //!                        POSIX specifies it's a number of seconds since Jan 1, 1970 UTC, but that is not portable which is why this function uses ctime to try to be portable regardless of how time_t is stored internally by the compiler.
    //
    //  Entry:
    //!   \param[in] inputTime = time_t value representing a time to use as the input for the future time calculation
    //!   \param[in] secondsInTheFuture = the number of seconds in the future. This must be a uint64_t and not a time_t value as a system may represent time_t oddly.
    //!
    //  Exit:
    //!   \return time_t value representing a time in the future
    //
    //-----------------------------------------------------------------------------
    time_t get_Future_Date_And_Time(time_t inputTime, uint64_t secondsInTheFuture);

    M_DECLARE_ENUM(eCompiler,
        OPENSEA_COMPILER_UNKNOWN,
        OPENSEA_COMPILER_MICROSOFT_VISUAL_C_CPP,
        OPENSEA_COMPILER_GCC,
        OPENSEA_COMPILER_CLANG,
        OPENSEA_COMPILER_MINGW,
        OPENSEA_COMPILER_INTEL_C_CPP,
        OPENSEA_COMPILER_SUNPRO_C_CPP,
        OPENSEA_COMPILER_IBM_XL_C_CPP,
        OPENSEA_COMPILER_IBM_SYSTEMZ_C_CPP,
        OPENSEA_COMPILER_HP_A_CPP,
        /*Add other compilers here if we ever add more than those above (which not all listed above are supported!)*/
        OPENSEA_COMPILER_RESERVED
    );

    typedef struct _compilerVersion
    {
        uint16_t major;
        uint16_t minor;
        uint16_t patch;
    }compilerVersion, *ptrCompilerVersion;

    //-----------------------------------------------------------------------------
    //
    //  get_Compiler_Info(eCompiler *compilerUsed, ptrCompilerVersion compilerVersionInfo)
    //
    //! \brief   Description:  This call will return which compiled and version of that compiler was used when compiling opensea-common (and likely the rest of opensea-* libs) 
    //
    //  Entry:
    //!   \param[out] compilerUsed = pointer to a eCompiler type that will be set with an enum value representing the compiler that was used on successful completion..
    //!   \param[out] compilerVersionInfo = pointer to the compilerVersion struct. This will be filled in with version information on successful completion.
    //!
    //  Exit:
    //!   \return SUCCESS on successful completion, !SUCCESS if problems encountered
    //
    //-----------------------------------------------------------------------------
    eReturnValues get_Compiler_Info(eCompiler *compilerUsed, ptrCompilerVersion compilerVersionInfo);

    //-----------------------------------------------------------------------------
    //
    //  print_Compiler(eCompiler compilerUsed)
    //
    //! \brief   Description:  This takes an eCompiler type and prints out the name of the compiler.
    //
    //  Entry:
    //!   \param[in] compilerUsed = eCompiler type that will be printed to the screen in human readable form
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Compiler(eCompiler compilerUsed);

    //-----------------------------------------------------------------------------
    //
    //  print_Compiler_Version_Info(ptrCompilerVersion compilerVersionInfo)
    //
    //! \brief   Description:  prints out the compiler version information in the form major.minor.patch from the compilerVersion struct
    //
    //  Entry:
    //!   \param[in] compilerVersionInfo = pointer to the compilerVersion struct that holds the compiler version information.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Compiler_Version_Info(ptrCompilerVersion compilerVersionInfo);

    //-----------------------------------------------------------------------------
    //
    //  M_DEPRECATED get_File_Size(FILE *filePtr)
    //
    //! \brief   Description:  Gets the size of a file from the passing in file pointer. This uses fseek(filePtr,0,SEEK_END). 
    //!                        This function will attempt to return to the position it is in when called (leave the file at the same location). File must be opened in binary mode.
    //!                        This function may not work as expected on large files. For large files, use og_Get_File_Size in common_platform.h
    //
    //  Entry:
    //!   \param[in] filePtr = pointer to a file to get the size of.
    //!
    //  Exit:
    //!   \return long int representing file size as returned from ftell call.
    //
    //-----------------------------------------------------------------------------
    //Use os_Get_File_Size instead!!!
    M_DEPRECATED
    long int get_File_Size(FILE *filePtr);

    //-----------------------------------------------------------------------------
    //
    //  print_Errno_To_Screen(errno_t error);
    //
    //! \brief   Description:  Prints the error number and it's meaning to the screen followed by a newline character
    //
    //  Entry:
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Errno_To_Screen(int error);

    //-----------------------------------------------------------------------------
    //
    //  custom_Update(void *customData, char *message)
    //
    //! \brief   Description:  Provides a custom callback for updating important messages to a UI.  
    //!                        For instance "Performing 5000 Random Reads" or "Sequential Read Test".
    //!                        This allows the UI to get occasional refreshes
    //
    //  Entry:
    //!   \param[in] customData = Custom data used to help the Updater function.  Usually hidden to the calling function.
    //!   \param[in] message    = Message to go to the UI.
    //! 
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    typedef void (*custom_Update)(void *customData, char *message);

    //-----------------------------------------------------------------------------
    //
    //  malloc_aligned(size_t size, size_t alignment)
    //
    //! \brief   Description:  Allocates aligned memory based on the specified power of 2 alignement value
    //
    //  Entry:
    //!   \param[in] size = size of memory block in bytes to allocate
    //!   \param[in] alignment = alignment value required. This MUST be a power of 2.
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    void *malloc_aligned(size_t size, size_t alignment);

    //-----------------------------------------------------------------------------
    //
    //  free_aligned(void* ptr)
    //
    //! \brief   Description:  Deallocates memory that was allocated with one of malloc_aligned, calloc_aligned, or realloc_aligned
    //
    //  Entry:
    //!   \param[in] ptr = pointer to the aligned memory to free
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void free_aligned(void* ptr);

    //-----------------------------------------------------------------------------
    //
    //  safe_Free_aligned()
    //
    //! \brief   Description:  Safely free dynamically alligned allocated memory. This checks for a valid pointer, then frees it and set's it to NULL.
    //
    //  Entry:
    //!   \param[in] mem - heap memory you want to free.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    #define safe_Free_aligned(mem)  \
    if(mem)                 \
    {                       \
        free_aligned(mem);          \
        mem = NULL;         \
    }                       \

    //-----------------------------------------------------------------------------
    //
    //  calloc_aligned(size_t num, size_t size, size_t alignment)
    //
    //! \brief   Description:  Allocates aligned memory based on the specified power of 2 alignement value and zeroes it out.
    //
    //  Entry:
    //!   \param[in] num = # of elements to allocate
    //!   \param[in] size = size of each element
    //!   \param[in] alignment = alignment value required. This MUST be a power of 2.
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    void *calloc_aligned(size_t num, size_t size, size_t alignment);

    //-----------------------------------------------------------------------------
    //
    //  realloc_aligned(void *alignedPtr, size_t size, size_t alignment)
    //
    //! \brief   Description:  Reallocates aligned memory based on the specified power of 2 alignement value and zeroes it out.
    //
    //  Entry:
    //!   \param[in] alignedPtr = pointer to a memory block previously allocated with malloc_aligned, calloc_aligned, or realloc_aligned. If NULL, this is the same as malloc_aligned
    //!   \param[in] originalSize = size in bytes of the alignedPtr being passed in. This is used so that previous data can be preserved. Can be set to 0 if there is no care about the original data.
    //!   \param[in] size = size of memory block in bytes to allocate
    //!   \param[in] alignment = alignment value required. This MUST be a power of 2.
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    void *realloc_aligned(void *alignedPtr, size_t originalSize, size_t size, size_t alignment);

    //-----------------------------------------------------------------------------
    //
    //  get_System_Pagesize(void)
    //
    //! \brief   Description:  Gets the memory page size from a system if possible.
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return Pagesize of system OR -1 if it cannot be determined.
    //
    //-----------------------------------------------------------------------------
    size_t get_System_Pagesize(void);

    //-----------------------------------------------------------------------------
    //
    //  malloc_page_aligned(size_t size)
    //
    //! \brief   Description:  Allocates aligned memory based on the system page size. Same as calling malloc_aligned(size, get_System_Pagesize())
    //
    //  Entry:
    //!   \param[in] size = size of memory block in bytes to allocate
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    void *malloc_page_aligned(size_t size);

    //-----------------------------------------------------------------------------
    //
    //  free_aligned(void* ptr)
    //
    //! \brief   Description:  Deallocates memory that was allocated with one of malloc_page_aligned, calloc_page_aligned, or realloc_page_aligned
    //
    //  Entry:
    //!   \param[in] ptr = pointer to the aligned memory to free
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void free_page_aligned(void* ptr);

    //-----------------------------------------------------------------------------
    //
    //  safe_Free_page_aligned()
    //
    //! \brief   Description:  Safely free dynamically page alligned allocated memory. This checks for a valid pointer, then frees it and set's it to NULL.
    //
    //  Entry:
    //!   \param[in] mem - heap memory you want to free.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    #define safe_Free_page_aligned(mem)  \
    if(mem)                 \
    {                       \
        free_page_aligned(mem);          \
        mem = NULL;         \
    }                       \


    //-----------------------------------------------------------------------------
    //
    //  calloc_page_aligned(size_t num, size_t size)
    //
    //! \brief   Description:  Allocates aligned memory based on the system page size. Same as calling calloc_aligned(num, size, get_System_Pagesize());
    //
    //  Entry:
    //!   \param[in] num = # of elements to allocate
    //!   \param[in] size = size of each element
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    void *calloc_page_aligned(size_t num, size_t size);

    //-----------------------------------------------------------------------------
    //
    //  realloc_page_aligned(void *alignedPtr, size_t size)
    //
    //! \brief   Description:  Reallocates aligned memory based on the system page size. Same as calling realloc_aligned(alignedPtr, size, get_System_Pagesize());
    //
    //  Entry:
    //!   \param[in] alignedPtr = pointer to a memory block previously allocated with malloc_aligned, calloc_aligned, or realloc_aligned. If NULL, this is the same as malloc_aligned
    //!   \param[in] originalSize = size in bytes of the alignedPtr being passed in. This is used so that previous data can be preserved.
    //!   \param[in] size = size of memory block in bytes to allocate
    //!
    //  Exit:
    //!   \return ptrToAlignedMemory
    //
    //-----------------------------------------------------------------------------
    void *realloc_page_aligned(void *alignedPtr, size_t originalSize, size_t size);

    //-----------------------------------------------------------------------------
    //
    //  is_Empty(void *ptrData, size_t lengthBytes)
    //
    //! \brief   Description:  Checks if the provided pointer is cleared to zeros
    //
    //  Entry:
    //!   \param[in] ptrData = pointer to a memory block to check if zeros
    //!   \param[in] lengthBytes = size in bytes of the ptr to check
    //!
    //  Exit:
    //!   \return true = memory is filled with zeros. false = memory has nonzero values in it.
    //
    //-----------------------------------------------------------------------------
    bool is_Empty(void *ptrData, size_t lengthBytes);

    //-----------------------------------------------------------------------------
    //
    //  int is_ASCII(int c)
    //
    //! \brief   Description:  This function checks if the provided character is between 0 and 7F. A.K.A. part of the standard ascii character set.
    //
    //  Entry:
    //!   \param[in] c = character to check if is an ASCII character
    //!
    //  Exit:
    //!   \return 0 = not an ASCII character. 1 = is an ASCII character
    //
    //-----------------------------------------------------------------------------
    int is_ASCII(int c);

    //-----------------------------------------------------------------------------
    //
    //  void get_Decimal_From_4_byte_Float(uint32_t floatValue, double *decimalValue))
    //
    //! \brief   Description:  This function converts 4 Byte Representation of a Floating Point Number to Decimal Format
    //
    //  Entry:
    //!   \param[in] floatValue = 4 byte format value
    //!   \param[out] decimalValue = corresponding decimal format value
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void get_Decimal_From_4_byte_Float(uint32_t floatValue, double *decimalValue);

    //-----------------------------------------------------------------------------
    //
    //  char* common_String_Concat(char* destination, size_t destinationSizeBytes, const char* source);
    //
    //! \brief   Description:  To be used in place of strcat. This will work more like strlcat in the BSDs and will always null terminate.
    //
    //  Entry:
    //!   \param[in] destination = pointer to memory to write with zeroes. Must be non-NULL
    //!   \param[in] destinationSizeBytes = number of bytes pointed to by destination
    //!   \param[in] source = pointer to source string to concatenate onto destination. Must be NULL terminated.
    //!
    //  Exit:
    //!   \return pointer to destination
    //
    //-----------------------------------------------------------------------------
    char* common_String_Concat(char* destination, size_t destinationSizeBytes, const char* source);

    //-----------------------------------------------------------------------------
    //
    //  char* common_String_Concat_Len(char* destination, size_t destinationSizeBytes, const char* source, int sourceLength);
    //
    //! \brief   Description:  To be used in place of strncat. Will always null terminate destination string.
    //
    //  Entry:
    //!   \param[in] destination = pointer to memory to write with zeroes. Must be non-NULL
    //!   \param[in] destinationSizeBytes = number of bytes pointed to by destination
    //!   \param[in] source = pointer to source string to concatenate onto destination. null termination recommended
    //!   \param[in] sourceLength = number of bytes to use from source for concatenation
    //!
    //  Exit:
    //!   \return pointer to destination
    //
    //-----------------------------------------------------------------------------
    char* common_String_Concat_Len(char* destination, size_t destinationSizeBytes, const char* source, int sourceLength);

    //-----------------------------------------------------------------------------
    //
    //  char *common_String_Token(char *str, const char *delim, char **saveptr)
    //
    //! \brief   Description:  To be used in place of strtok. This tries to wrap thread safe versions of strtok when possible.
    //!                        If a thread safe version is not available, then it uses the strtok() function
    //!                        It is recommended that any string parsed by this function is a duplicate of the original (strdup) so
    //!                        to ensure the original string is not modified by any of the functions called within this function.
    //
    //  Entry:
    //!   \param[in] str = pointer string to tokenize/parse
    //!   \param[in] strmax = used by C11 annex K to track remaining characters to parse. Size of the ORIGINAL string to tokenize. Required for all implementations (Will emulate behavior as much as possible)
    //!   \param[in] delim = list of delimiters to use when tokenizing.
    //!   \param[in] saveptr = used by the thread-safe strtok functions internally to track their state. This must be non NULL. Does not need to be freed!
    //!
    //  Exit:
    //!   \return pointer to destination
    //
    //-----------------------------------------------------------------------------
    #if !defined (RSIZE_MAX)
        typedef size_t rsize_t;
        #define RSIZE_MAX (SIZE_MAX >> 1)
    #endif //!RSIZE_MAX
    char *common_String_Token(char * M_RESTRICT str, rsize_t * M_RESTRICT strmax, const char * M_RESTRICT delim, char ** M_RESTRICT saveptr);

    //-----------------------------------------------------------------------------
    //
    //  void* explicit_zeroes(void* dest, size_t count)
    //
    //! \brief   Description:  Write zeroes to memory pointer that will not be optimized out by the compiler. 
    //
    //  Entry:
    //!   \param[in] dest = pointer to memory to write with zeroes. Must be non-NULL
    //!   \param[in] count = number of bytes to write to zero
    //!
    //  Exit:
    //!   \return NULL = error occurred otherwise returns pointer to dest
    //
    //-----------------------------------------------------------------------------
    void* explicit_zeroes(void* dest, size_t count);

    size_t int8_to_sizet(int8_t val);

    size_t uint8_to_sizet(uint8_t val);

    size_t int16_to_sizet(int16_t val);

    size_t uint16_to_sizet(uint16_t val);

    size_t int32_to_sizet(int32_t val);

    size_t uint32_to_sizet(uint32_t val);

    size_t int64_to_sizet(int64_t val);

    size_t uint64_to_sizet(uint64_t val);

    size_t char_to_sizet(char val);
    size_t uchar_to_sizet(unsigned char val);

    size_t short_to_sizet(short val);
    size_t ushort_to_sizet(unsigned short val);

    size_t int_to_sizet(int val);
    size_t uint_to_sizet(unsigned int val);

    size_t long_to_sizet(long val);
    size_t ulong_to_sizet(unsigned long val);

    size_t longlong_to_sizet(long long val);
    size_t ulonglong_to_sizet(unsigned long long val);

    #if defined(USING_C11)
    //This is a generic selection macro.
    //so based on the type of X, it will call the appropriate function for that type.
    //similar to an overloaded function in C++ for different types.
    //NOTE: Not using intX_t or uintX_t since these are type defs to one of the types in the macro below.
    //NOTE: No default case so we can get a warning when this doesn't expand correctly.
    #define to_sizet(X) _Generic((X), \
                        char: char_to_sizet,\
                        unsigned char: uchar_to_sizet,\
                        short: short_to_sizet,\
                        unsigned short: ushort_to_sizet,\
                        int: int_to_sizet,\
                        unsigned int: uint_to_sizet,\
                        long: long_to_sizet,\
                        unsigned long: ulong_to_sizet,\
                        long long: longlong_to_sizet,\
                        unsigned long long: ulonglong_to_sizet\
                        )(X)
    #endif //C11

    bool is_size_t_max(size_t val);

    //returns number of milliseconds since Jan 1, 1970 UTC
    //If an error occurs, returns zero
    uint64_t get_Milliseconds_Since_Unix_Epoch(void);

    #if !defined (SSIZE_MAX) && !defined (SSIZE_T_DEFINED)
        //assume ssize_t is not defined
        #if defined (_MSC_VER) && defined (MAXSSIZE_T)
            //MSFT defined this as SSIZE_T, so just make it lowercase to match POSIX
            //NOTE: reviewing MSFT's headers shows this is the same as intptr_t definitions as well
            typedef SSIZE_T ssize_t;
            #define SSIZE_MAX MAXSSIZE_T
        #else
            //using intptr_t since it has the same range.
            typedef intptr_t ssize_t;
            #define SSIZE_MAX INTPTR_MAX
        #endif
        #define SSIZE_T_DEFINED
    #endif  //SSIZE_MAX && _MSC_VER


#if !defined (__STDC_ALLOC_LIB__) && !defined(POSIX_2008) && !defined (USING_C23)
    //define strndup
    //NOTE: Not defining strdup since this may be available in the OS's that need this definition already (Windows currently)
    char* strndup(const char* src, size_t size);

    //Need getline and getdelim functions since they are not available.
    ssize_t getline(char** lineptr, size_t* n, FILE* stream);

    ssize_t getdelim(char** M_RESTRICT lineptr, size_t* M_RESTRICT n, int delimiter, FILE* stream);
#endif //!__STDC_ALLOC_LIB__ && (POSIX < 2008)


    //-----------------------------------------------------------------------------
    //
    // bool os_Directory_Exits (const char * const pathToCheck)
    //
    // \brief   Description: Platform independent helper to check if path exists. 
    //                       WARNING: May not work with UNICODE path. 
    //
    // Entry:
    //      \param[in] pathToCheck The path that needs to be evaluated. 
    //
    // Exit:
    //      \return true if exists, false if it does not. 
    //
    //-----------------------------------------------------------------------------
    bool os_Directory_Exists(const char * const pathToCheck);

    //-----------------------------------------------------------------------------
    //
    // bool os_File_Exists (const char * const filetoCheck)
    //
    // \brief   Description: Platform independent helper to check if file exists. 
    //                       WARNING: May not work with UNICODE path. 
    //
    // Entry:
    //      \param[in] filetoCheck The file that needs to be evaluated. 
    //
    // Exit:
    //      \return true if exists, false if it does not. 
    //
    //-----------------------------------------------------------------------------
    bool os_File_Exists(const char * const filetoCheck);

    bool get_Bytes_To_16(uint8_t *dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint16_t *out);

    bool get_Bytes_To_32(uint8_t *dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint32_t *out);

    bool get_Bytes_To_64(uint8_t *dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint64_t *out);

    //This is available in Windows and linux/unix-like systems
#include <sys/types.h>

#if defined (_WIN32)
    //Windows has stat/stat64 and uses shorts for these members.
    typedef short uid_t;
    typedef short gid_t;
    typedef unsigned short mode_t;
    typedef short nlink_t;
    //using our own "type" to make sure we can get as much of the filesize as possible..and workaround Windows issues with off_t -TJE
    typedef __int64 offset_t;

    //Windows does not supply the macros to test the file, so we are defining them ourselves so that we can use them when we want to.
    //NOTE: This is only for the mode_t compatibility provided by Windows.
    #if !defined (S_ISREG)
        #define S_ISREG(m) (m & _S_IFMT) == _S_IFREG
    #endif //S_ISREG

    #if !defined (S_ISDIR)
        #define S_ISDIR(m) (m & _S_IFMT) == _S_IFDIR
    #endif //S_ISDIR

    #if !defined (S_ISCHR)
        #define S_ISCHR(m) (m & _S_IFMT) == _S_IFCHR
    #endif //S_ISCHR

    #if !defined (S_ISBLK)
        #define S_ISBLK(m) (0)  //Windows doesn't have a flag for this
    #endif //S_ISBLK

    #if !defined (S_ISFIFO)
        #define S_ISFIFO(m) (m & _S_IFMT) == _S_IFIFO
    #endif //S_ISFIFO

    #if !defined (S_ISLNK)
        #define S_ISLNK(m) (0) //Windows doesn't have a flag for this
    #endif //S_ISLNK

    #if !defined (S_ISSOCK)
        #define S_ISSOCK(m) (0) //Windows doesn't have a flag for this
    #endif //S_ISSOCK

    #if !defined (S_TYPEISMQ)
        #define S_TYPEISMQ(buf) (0) //Windows doesn't have a flag for this
    #endif //S_TYPEISMQ

    #if !defined (S_TYPEISSEM)
        #define S_TYPEISSEM(buf) (0) //Windows doesn't have a flag for this
    #endif //S_TYPEISSEM

    #if !defined (S_TYPEISSHM)
        #define S_TYPEISSHM(buf) (0) //Windows doesn't have a flag for this
    #endif //S_TYPEISSHM

#else
    //using our own "type" to make sure we can get as much of the filesize as possible..and workaround Windows issues with off_t -TJE
    typedef off_t offset_t;//to deal with windows differences in off_t definitions in stat
#endif //_WIN32

    #define FILE_UNIQUE_ID_ARR_MAX (16)
    typedef struct _fileUniqueIDInfo
    {
        //Windows uses unsigned long for vol SN
        //Windows uses 2 unsigned longs for index high/low. NOTE: REFS needs a 128bit identifier, need to use ex version to read full identifier
        //Linux has ino_t and dev_t to check for
        //Idea: Both linux and windows recommend combing things to create a unique ID (Windows: Vol SN + file index, Linux: ino_t + dev_t)
        //      use a very generic thing to compare that can hold this many bytes of data...an array.
        //      low-level OS function to do the comparison???
        union {
            uint64_t inode;
            uint64_t volsn;
        };
        union {
            uint64_t deviceid;//dev_t from struct stat in posix
            uint8_t fileid[FILE_UNIQUE_ID_ARR_MAX];//Windows may use 8 or 16 bytes depending on filesystem. Fat/NTFS can use only 64bits, but REFS needs at least 128 bits to identify this uniquely.
        };
    }fileUniqueIDInfo;

    M_NODISCARD fileUniqueIDInfo* os_Get_File_Unique_Identifying_Information(FILE* file);

    //Most members of this strucuture match the stat structure. There are some differences which is why we define it without that strucure.
    //Main reason to NOT use struct stat is that Windows has a version, but to get the 64 version would make this a mess to define.
    //So defining it in a way that is best for cross-platform use.
    //Some fields may not be set to anything other than 0 on Windows dues to not have the same concepts as on Unix/Unix-like systems
    typedef struct _fileAttributes
    {
        dev_t deviceID;
        ino_t inode;
        mode_t filemode;
        nlink_t numberOfLinks;
        uid_t userID;
        gid_t groupID;
        dev_t representedDeviceID;
        offset_t filesize;
        int64_t fileLastAccessTime;//milliseconds since Unix epoch (in Windows, converted from Windows file epoch to this value)
        int64_t fileModificationTime;//milliseconds since Unix epoch (in Windows, converted from Windows file epoch to this value)
        int64_t fileStatusChangeTime;//milliseconds since Unix epoch (in Windows, converted from Windows file epoch to this value)
        //Windows only below here for Windows specific info that may differ from above
        uint32_t fileFlags;
        uint16_t securityControlFlags;
#if defined (_WIN32)
        //Windows user and group SID?
        unsigned long securityDescriptorStringLength;
        char* winSecurityDescriptor;//Allocated by malloc. Call memset_explicit on this to zero it out before freeing.
#endif//_WIN32
    }fileAttributes;
    //DO NOT delete this structure manually. The windows SIDs are allocated and require deletion of their own.
    //this will be cleaned up when calling the function to delete this for you!

    //This can be used on files or directories.
    M_NODISCARD fileAttributes* os_Get_File_Attributes_By_Name(const char* const filetoCheck);
    //This is the preferred method to check attributes are the same once a file has been opened to make sure
    //that the attributes are read from exactly the same file as expected
    M_NODISCARD fileAttributes* os_Get_File_Attributes_By_File(FILE* file);

    void free_File_Attributes(fileAttributes** attributes);

    //For terminating value in this list, set ext to NULL
    typedef struct _fileExt
    {
        const char* ext;
        bool caseInsensitive;//by default to a case sensitive comparison. Only change this if you want .bin and .BIN and .bIN and .Bin...for example.
    }fileExt;

    M_DECLARE_ENUM(eSecureFileError,
        SEC_FILE_SUCCESS,
        SEC_FILE_INVALID_FILE,
        SEC_FILE_INVALID_PATH,
        SEC_FILE_FILE_ALREADY_EXISTS, /*attempting to write and create a file that already exists*/
        SEC_FILE_INVALID_FILE_EXTENSION,
        SEC_FILE_INVALID_FILE_ATTRIBTUES,
        SEC_FILE_INVALID_FILE_UNIQUE_ID,
        SEC_FILE_INSECURE_PATH,
        SEC_FILE_INVALID_MODE,/*mode string contains invalid options*/
        SEC_FILE_INVALID_SECURE_FILE, /*passed in NULL secureFileInfo structure to a function that requires this.*/
        SEC_FILE_FAILURE_CLOSING_FILE, /*a failure occured while trying to close the file*/
        SEC_FILE_BUFFER_TOO_SMALL, /*provided buffer is too small for read/write*/
        SEC_FILE_INVALID_PARAMETER, /*this can be returned if a function (like fread/fwrite) require a parameter that was not provided*/
        SEC_FILE_READ_WRITE_ERROR, /*did not read or write as many bytes as requested, cannot determine a more specific reason*/
        SEC_FILE_END_OF_FILE_REACHED, /*reached the end of the file...can be a case of success if this is expected*/
        SEC_FILE_WRITE_DISK_FULL, /*cannot write any more data due to an error from running out of space*/
        SEC_FILE_SEEK_FAILURE, /*Cannot seek to the specified offset in the file*/
        SEC_FILE_FLUSH_FAILURE,
        SEC_FILE_FAILURE /*generic undefinable error*/
    );

    typedef struct _secureFileInfo
    {
        eSecureFileError error;
        bool isValid;
        FILE* file;
        const char fullpath[4096];//contains the full canonicalized path and filename.
        const char* filename;//pointer to just the name section in the fullpath above
        int fileno;//POSIX fileno. Translated using fileno(FILE *file);
        size_t fileSize;//converted from the file attributes file size. May be smaller if size_t cannot represent whole file size. Ex: 4gig file on 32bit OS.
        fileAttributes *attributes;
        fileUniqueIDInfo* uniqueID;
    }secureFileInfo; //do not free this manually. call the free_Secure_File_Info function!

    void free_Secure_File_Info(secureFileInfo** fileInfo);

    //The purpose of this function is to perform the security validation necessary to make sure this is a valid file
    //on the system and minimize path traversal and validate permissions as much as reasonably possible.
    //The goal is mitigation of https://cwe.mitre.org/data/definitions/22.html
    //Will be using recommendations from https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=87151932
    //     as much as possible to accomplish this.-TJE
    //expectedFileInfo can be NULL for the first time opening a file.
    // If reopening a file used earlier, it is recommended to provide this info so it can be validated as the same file
    // It is recommended to not reopen files, but that may not always be possible. So this exists to help validate
    // that a file has not changed in some unexpected way.-TJE
    M_NODISCARD secureFileInfo* secure_Open_File(const char* filename, const char* mode, const fileExt* extList /*optional*/, fileAttributes* expectedFileInfo /*optional*/, fileUniqueIDInfo* uniqueIdInfo /*optional*/);

    //M_NODISCARD secureFileInfo* secure_Reopen_File(secureFileInfo* fileInfo);

    //matching close for the open.
    M_NODISCARD eSecureFileError secure_Close_File(secureFileInfo* fileInfo);

    M_DECLARE_ENUM(eSecureFileRename,
        SEC_RENAME_DO_NOT_REPLACE_EXISTING,
        SEC_RENAME_REPLACE_EXISTING
    );

    //NOTE: This will convert the filename into a canonical path internally to ensure a valid path is provided.
    //      Since the low-level APIs need a file name rather than FILE *, this takes the same kind of input.
    //eSecureFileError secure_Rename_File_By_Name(const char* filename, eSecureFileRename renameInfo);

    //NOTE: This will convert the filename into a canonical path internally to ensure a valid path is provided.
    //      Low-level APIs need the filename to do this, so using the same type of input.
    //eSecureFileError secure_Delete_File_By_Name(const char* filename);

    M_NODISCARD eSecureFileError secure_Read_File(secureFileInfo* fileInfo, void* M_RESTRICT buffer, size_t buffersize, size_t elementsize, size_t count, size_t* numberread/*optional*/);
    M_NODISCARD eSecureFileError secure_Write_File(secureFileInfo* fileInfo, void* M_RESTRICT buffer, size_t buffersize, size_t elementsize, size_t count, size_t* numberwritten/*optional*/);
    M_NODISCARD eSecureFileError secure_Seek_File(secureFileInfo* fileInfo, offset_t offset, int initialPosition);
    M_NODISCARD eSecureFileError secure_Rewind_File(secureFileInfo* fileInfo);
    M_NODISCARD offset_t secure_Tell_File(secureFileInfo* fileInfo);

    eSecureFileError secure_Flush_File(secureFileInfo* fileInfo);

    eSecureFileError secure_GetPos_File(secureFileInfo* M_RESTRICT fileInfo, fpos_t* M_RESTRICT pos);

    eSecureFileError secure_SetPos_File(secureFileInfo* fileInfo, const fpos_t* pos);

    //need secure_fprintf_File to write formatted data to a file

#if defined (__cplusplus)
} //extern "C"
#endif
