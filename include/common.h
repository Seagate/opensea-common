//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012-2021 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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
/*
//This was an attempt to fix annoying warnings from MinGW rather than completely disable them, but it doesn't actually work, so instead we have to disable otherwise helpful warnings!
#if defined (__MINGW32__) || defined (__MINGW64__)
#define __USE_MINGW_ANSI_STDIO 1
#endif*/
    #include <stdio.h>
    #include <time.h>
    #include <string.h>
    #include <stdlib.h>
    #include <inttypes.h>
    #include <errno.h> //for printing std errors to the screen...more useful for 'nix OSs, but useful everywhere since it is at least standard functions
    #define __STDC_WANT_LIB_EXT1__ 1
    #include <time.h>

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
        
    #if defined (OPENSEA_COMMON_BOOLS) //THIS FLAG IS NOT ENABLED BY DEFAULT
    //I'm doing this because 
    //1) some compilers may not support stdbool
    //2) using booleans with getopt can cause stack corruption depending on how bools are defined by the compiler
        #define OPENSEA_COMMON_BOOLS
        #define bool int32_t
        #define false 0
        #define true !false
    //size of bool in this case is 32bits (4 bytes)
    #else //default definition of bool comes from this standard header
        //size of bool in this case is compiler dependent, but it is usually 8bits (1 byte)
        #include <stdbool.h>
    #endif

    #include "common_platform.h"

    //Microsoft doesn't have snprintf...it has _snprintf...at least until VS2015 according to my web search - TJE
    #if _MSC_VER <= 1800 && defined _WIN32
    #define snprintf _snprintf
    #endif

    //get a specific double word
    #define M_DoubleWord0(l) ( (uint32_t) ( ( (l) & 0x00000000FFFFFFFFULL ) >>  0 ) )
    #define M_DoubleWord1(l) ( (uint32_t) ( ( (l) & 0xFFFFFFFF00000000ULL ) >> 32 ) )

    //get a specific double word
    #define M_DoubleWordInt0(l) ( (int32_t) ( ( (l) & 0x00000000FFFFFFFFULL ) >>  0 ) )
    #define M_DoubleWordInt1(l) ( (int32_t) ( ( (l) & 0xFFFFFFFF00000000ULL ) >> 32 ) )

    //get a specific word
    #define M_Word0(l) ( (uint16_t) ( ( (l) & 0x000000000000FFFFULL ) >>  0 ) )
    #define M_Word1(l) ( (uint16_t) ( ( (l) & 0x00000000FFFF0000ULL ) >> 16 ) )
    #define M_Word2(l) ( (uint16_t) ( ( (l) & 0x0000FFFF00000000ULL ) >> 32 ) )
    #define M_Word3(l) ( (uint16_t) ( ( (l) & 0xFFFF000000000000ULL ) >> 48 ) )

    //get a specific word as int's
    #define M_WordInt0(l) ( (int16_t) ( ( (l) & 0x000000000000FFFFULL ) >>  0 ) )
    #define M_WordInt1(l) ( (int16_t) ( ( (l) & 0x00000000FFFF0000ULL ) >> 16 ) )
    #define M_WordInt2(l) ( (int16_t) ( ( (l) & 0x0000FFFF00000000ULL ) >> 32 ) )
    #define M_WordInt3(l) ( (int16_t) ( ( (l) & 0xFFFF000000000000ULL ) >> 48 ) )

    //need to validate that this macro sets the correct bits on 32bit and 64bit
    #define BITSPERBYTE UINT8_C(8)
    #define M_ByteN(n) ((UINT8_MAX << (n * BITSPERBYTE)))

    //Get a specific byte
    #define M_Byte0(l) ( (uint8_t) ( ( (l) & 0x00000000000000FFULL ) >>  0 ) )
    #define M_Byte1(l) ( (uint8_t) ( ( (l) & 0x000000000000FF00ULL ) >>  8 ) )
    #define M_Byte2(l) ( (uint8_t) ( ( (l) & 0x0000000000FF0000ULL ) >> 16 ) )
    #define M_Byte3(l) ( (uint8_t) ( ( (l) & 0x00000000FF000000ULL ) >> 24 ) )
    #define M_Byte4(l) ( (uint8_t) ( ( (l) & 0x000000FF00000000ULL ) >> 32 ) )
    #define M_Byte5(l) ( (uint8_t) ( ( (l) & 0x0000FF0000000000ULL ) >> 40 ) )
    #define M_Byte6(l) ( (uint8_t) ( ( (l) & 0x00FF000000000000ULL ) >> 48 ) )
    #define M_Byte7(l) ( (uint8_t) ( ( (l) & 0xFF00000000000000ULL ) >> 56 ) )

    //Get a specific byte int
    #define M_ByteInt0(l) ( (uint8_t) ( ( (l) & 0x00000000000000FFULL ) >>  0 ) )
    #define M_ByteInt1(l) ( (uint8_t) ( ( (l) & 0x000000000000FF00ULL ) >>  8 ) )
    #define M_ByteInt2(l) ( (uint8_t) ( ( (l) & 0x0000000000FF0000ULL ) >> 16 ) )
    #define M_ByteInt3(l) ( (uint8_t) ( ( (l) & 0x00000000FF000000ULL ) >> 24 ) )
    #define M_ByteInt4(l) ( (uint8_t) ( ( (l) & 0x000000FF00000000ULL ) >> 32 ) )
    #define M_ByteInt5(l) ( (uint8_t) ( ( (l) & 0x0000FF0000000000ULL ) >> 40 ) )
    #define M_ByteInt6(l) ( (uint8_t) ( ( (l) & 0x00FF000000000000ULL ) >> 48 ) )
    #define M_ByteInt7(l) ( (uint8_t) ( ( (l) & 0xFF00000000000000ULL ) >> 56 ) )

    //get a specific nibble
    #define M_Nibble0(l)  ( (uint8_t) ( ( (l) & 0x000000000000000FULL ) >>  0 ) )
    #define M_Nibble1(l)  ( (uint8_t) ( ( (l) & 0x00000000000000F0ULL ) >>  4 ) )
    #define M_Nibble2(l)  ( (uint8_t) ( ( (l) & 0x0000000000000F00ULL ) >>  8 ) )
    #define M_Nibble3(l)  ( (uint8_t) ( ( (l) & 0x000000000000F000ULL ) >> 12 ) )
    #define M_Nibble4(l)  ( (uint8_t) ( ( (l) & 0x00000000000F0000ULL ) >> 16 ) )
    #define M_Nibble5(l)  ( (uint8_t) ( ( (l) & 0x0000000000F00000ULL ) >> 20 ) )
    #define M_Nibble6(l)  ( (uint8_t) ( ( (l) & 0x000000000F000000ULL ) >> 24 ) )
    #define M_Nibble7(l)  ( (uint8_t) ( ( (l) & 0x00000000F0000000ULL ) >> 28 ) )
    #define M_Nibble8(l)  ( (uint8_t) ( ( (l) & 0x0000000F00000000ULL ) >> 32 ) )
    #define M_Nibble9(l)  ( (uint8_t) ( ( (l) & 0x000000F000000000ULL ) >> 36 ) )
    #define M_Nibble10(l) ( (uint8_t) ( ( (l) & 0x00000F0000000000ULL ) >> 40 ) )
    #define M_Nibble11(l) ( (uint8_t) ( ( (l) & 0x0000F00000000000ULL ) >> 44 ) )
    #define M_Nibble12(l) ( (uint8_t) ( ( (l) & 0x000F000000000000ULL ) >> 48 ) )
    #define M_Nibble13(l) ( (uint8_t) ( ( (l) & 0x00F0000000000000ULL ) >> 52 ) )
    #define M_Nibble14(l) ( (uint8_t) ( ( (l) & 0x0F00000000000000ULL ) >> 56 ) )
    #define M_Nibble15(l) ( (uint8_t) ( ( (l) & 0xF000000000000000ULL ) >> 60 ) )

    // Bit access macros

    #define M_BitN(n)   ((uint64_t)1 << n)

#if !defined(UEFI_C_SOURCE)//defined in EDK2 MdePkg and causes conflicts, so checking this define for now to avoid conflicts

    #define BIT0      (M_BitN((uint64_t)0))
    #define BIT1      (M_BitN((uint64_t)1))
    #define BIT2      (M_BitN((uint64_t)2))
    #define BIT3      (M_BitN((uint64_t)3))
    #define BIT4      (M_BitN((uint64_t)4))
    #define BIT5      (M_BitN((uint64_t)5))
    #define BIT6      (M_BitN((uint64_t)6))
    #define BIT7      (M_BitN((uint64_t)7))
    #define BIT8      (M_BitN((uint64_t)8))
    #define BIT9      (M_BitN((uint64_t)9))
    #define BIT10     (M_BitN((uint64_t)10))
    #define BIT11     (M_BitN((uint64_t)11))
    #define BIT12     (M_BitN((uint64_t)12))
    #define BIT13     (M_BitN((uint64_t)13))
    #define BIT14     (M_BitN((uint64_t)14))
    #define BIT15     (M_BitN((uint64_t)15))
    #define BIT16     (M_BitN((uint64_t)16))
    #define BIT17     (M_BitN((uint64_t)17))
    #define BIT18     (M_BitN((uint64_t)18))
    #define BIT19     (M_BitN((uint64_t)19))
    #define BIT20     (M_BitN((uint64_t)20))
    #define BIT21     (M_BitN((uint64_t)21))
    #define BIT22     (M_BitN((uint64_t)22))
    #define BIT23     (M_BitN((uint64_t)23))
    #define BIT24     (M_BitN((uint64_t)24))
    #define BIT25     (M_BitN((uint64_t)25))
    #define BIT26     (M_BitN((uint64_t)26))
    #define BIT27     (M_BitN((uint64_t)27))
    #define BIT28     (M_BitN((uint64_t)28))
    #define BIT29     (M_BitN((uint64_t)29))
    #define BIT30     (M_BitN((uint64_t)30))
    #define BIT31     (M_BitN((uint64_t)31))
    #define BIT32     (M_BitN((uint64_t)32))
    #define BIT33     (M_BitN((uint64_t)33))
    #define BIT34     (M_BitN((uint64_t)34))
    #define BIT35     (M_BitN((uint64_t)35))
    #define BIT36     (M_BitN((uint64_t)36))
    #define BIT37     (M_BitN((uint64_t)37))
    #define BIT38     (M_BitN((uint64_t)38))
    #define BIT39     (M_BitN((uint64_t)39))
    #define BIT40     (M_BitN((uint64_t)40))
    #define BIT41     (M_BitN((uint64_t)41))
    #define BIT42     (M_BitN((uint64_t)42))
    #define BIT43     (M_BitN((uint64_t)43))
    #define BIT44     (M_BitN((uint64_t)44))
    #define BIT45     (M_BitN((uint64_t)45))
    #define BIT46     (M_BitN((uint64_t)46))
    #define BIT47     (M_BitN((uint64_t)47))
    #define BIT48     (M_BitN((uint64_t)48))
    #define BIT49     (M_BitN((uint64_t)49))
    #define BIT50     (M_BitN((uint64_t)50))
    #define BIT51     (M_BitN((uint64_t)51))
    #define BIT52     (M_BitN((uint64_t)52))
    #define BIT53     (M_BitN((uint64_t)53))
    #define BIT54     (M_BitN((uint64_t)54))
    #define BIT55     (M_BitN((uint64_t)55))
    #define BIT56     (M_BitN((uint64_t)56))
    #define BIT57     (M_BitN((uint64_t)57))
    #define BIT58     (M_BitN((uint64_t)58))
    #define BIT59     (M_BitN((uint64_t)59))
    #define BIT60     (M_BitN((uint64_t)60))
    #define BIT61     (M_BitN((uint64_t)61))
    #define BIT62     (M_BitN((uint64_t)62))
    #define BIT63     (M_BitN((uint64_t)63))

#endif //UEFI_C_SOURCE

    //set a bit to 1 within a value
    #define M_SET_BIT(val, bitNum) (val |= M_BitN(bitNum))
    //clear a bit to 0 within a value
    #define M_CLEAR_BIT(val, bitNum) (val &= (~M_BitN(bitNum)))

    #define M_GETBITRANGE(input, msb, lsb) (((input) >> (lsb)) & ~(~UINT64_C(0) << ((msb) - (lsb) + 1)))
    // get bit range for signed values
    #define M_IGETBITRANGE(input, msb, lsb) (((input) >> (lsb)) & ~(~INT64_C(0) << ((msb) - (lsb) + 1)))

    #define M_2sCOMPLEMENT(val) (~(val) + 1)

    // MACRO to round the number of x so that it will not round up when formating the float
    #define ROUNDF(f, c) (((float)((int)((f) * (c))) / (c)))

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
            /* gnu::fallback check is not included as this breaks in other compilers that support some gcc-isms, and it's easier to comment this out for now than setup something complicated to support it. -TJE
            #elif __has_cpp_attribute(gnu::fallthrough)
                //This is the GNU method for C++11 and C++14
                #define M_FALLTHROUGH [[gnu::fallthrough]];
            */
            #elif defined __has_attribute
                //GCC type compiler check
                #if __has_attribute(fallthrough)
                    #define M_FALLTHROUGH __attribute__ ((fallthrough));
                #else
                    #define M_FALLTHROUGH /*FALLTHRU*/ \
                    
                #endif
            #else
                //Insert a comment instead since other methods were not detected.
                #define M_FALLTHROUGH /*FALLTHRU*/ \
                
            #endif
        #elif defined __has_attribute
            //GCC type compiler check
            #if __has_attribute(fallthrough)
                #define M_FALLTHROUGH __attribute__ ((fallthrough));
            #else
                #define M_FALLTHROUGH /*FALLTHRU*/ \
                
            #endif
        #else
            //Insert a comment instead since other methods were not detected.
            #define M_FALLTHROUGH /*FALLTHRU*/ \
            
        #endif
    #elif defined __has_c_attribute
        #if __has_c_attribute(fallthrough)
            //C2x style
            #define M_FALLTHROUGH [[fallthrough]];
        #elif defined __has_attribute
            //GCC type compiler check
            #if __has_attribute(fallthrough)
                #define M_FALLTHROUGH __attribute__ ((fallthrough));
            #else
                #define M_FALLTHROUGH /*FALLTHRU*/ \
                
            #endif
        #else
            //Insert a comment instead since other methods were not detected.
            #define M_FALLTHROUGH /*FALLTHRU*/ \
            
        #endif
    #else //not C++ and doesn't have __has_c_attribute so do something for older C code
        //Weird case: Clang 3.4.2 (CentOS7) doesn't like the attribute definition below and throws tons
        //of warnings for "declaration does not declare anything", but this is not a problem in newer versions
        //So currently solution to prevent excessive warnings that don't make sense is to define this differently for
        // clang 3 and earlier. This may need adjusting to specific versions of clang later, but this is what I've been able to do so far - TJE
        #if defined (__clang__) && __clang__ <= 3
            #define M_FALLTHROUGH /*FALLTHRU*/ \
                
        #elif defined __has_attribute
            //GCC type compiler check
            #if __has_attribute(fallthrough)
                #define M_FALLTHROUGH __attribute__ ((fallthrough));
            #else
                #define M_FALLTHROUGH /*FALLTHRU*/ \
                
            #endif
        #else
            //Insert a comment instead since other methods were not detected.
            #define M_FALLTHROUGH /*FALLTHRU*/ \
            
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
            #elif defined __has_attribute
                //GCC type compiler check
                #if __has_attribute(unused)
                    #define M_ATTR_UNUSED __attribute__ ((unused))
                #else
                    #define M_ATTR_UNUSED /*UNUSED*/ \
                    
                #endif
            #elif defined (_MSC_VER)
                #define M_ATTR_UNUSED __pragma(warning(suppress:4100 4101)) //4102?
            #else
                //Insert a comment instead since other methods were not detected.
                #define M_ATTR_UNUSED /*UNUSED*/ \
                
            #endif
        #elif defined __has_attribute
            //GCC type compiler check
            #if __has_attribute(unused)
                #define M_ATTR_UNUSED __attribute__ ((unused))
            #else
                #define M_ATTR_UNUSED /*UNUSED*/ \
                
            #endif
		#elif defined (__GNUC__) && __GNUC__ >= 3
			//GCC 3 & 4 support the unused attribute...you just don't have a convenient way to detect it otherwise
			#define M_ATTR_UNUSED __attribute__ ((unused))
        #elif defined (_MSC_VER)
            #define M_ATTR_UNUSED __pragma(warning(suppress:4100 4101)) //4102?
        #else
            //Insert a comment instead since other methods were not detected.
            #define M_ATTR_UNUSED /*UNUSED*/ \
            
        #endif
    #elif defined __has_c_attribute //C2x
        #if __has_c_attribute(maybe_unused)
            //C2x style
            #define M_ATTR_UNUSED [[maybe_unused]]
        #elif defined __has_attribute
            //GCC type compiler check
            #if __has_attribute(unused)
                #define M_ATTR_UNUSED __attribute__ ((unused))
            #else
                #define M_ATTR_UNUSED /*UNUSED*/ \
                
            #endif
        #elif defined (_MSC_VER)
            #define M_ATTR_UNUSED __pragma(warning(suppress:4100 4101)) //4102?
        #else
            //Insert a comment instead since other methods were not detected.
            #define M_ATTR_UNUSED /*UNUSED*/ \
            
        #endif
    #else //older C
        #if defined __has_attribute
            //GCC type compiler check
            #if __has_attribute(unused)
                #define M_ATTR_UNUSED __attribute__ ((unused))
            #else
                #define M_ATTR_UNUSED /*UNUSED*/ \
                
            #endif
		#elif defined (__GNUC__) && __GNUC__ >= 3
			//GCC 3 & 4 support the unused attribute...you just don't have a convenient way to detect it otherwise
			#define M_ATTR_UNUSED __attribute__ ((unused))
        #elif defined (_MSC_VER)
            #define M_ATTR_UNUSED __pragma(warning(suppress:4100 4101)) //4102?
        #else
            //Insert a comment instead since other methods were not detected.
            #define M_ATTR_UNUSED /*UNUSED*/ \
            
        #endif
    #endif

    //Macro to help make casts more clear and searchable. Can be very helpful while debugging.
    //If using C++, use static_cast, reinterpret_cast, dynamic_cast before trying a C_CAST.
    #define C_CAST(type, val) (type)(val)

    typedef enum _eReturnValues
    {
        SUCCESS                 = 0,
        FAILURE                 = 1,
        NOT_SUPPORTED           = 2,
        COMMAND_FAILURE         = 3,
        IN_PROGRESS             = 4, //another command is in progress, or a command has started and is now in progress in the background
        ABORTED                 = 5, //Command was aborted
        BAD_PARAMETER           = 6, //the only time this return code should be seen is when a developer is writing code to add something. This should not appear in a finished application
        MEMORY_FAILURE          = 7, //could not allocate memory
        OS_PASSTHROUGH_FAILURE  = 8, //For some unknown reason, the OS API call to issue the pass-through command failed.
        LIBRARY_MISMATCH        = 9,
        FROZEN                  = 10, //use this to communicate back when the device is in a frozen state for a commmand like sanitize or ata security
        PERMISSION_DENIED       = 11, //OS returned Access/permission denied
        FILE_OPEN_ERROR         = 12,
        WARN_INCOMPLETE_RFTRS   = 13, //command was issued, and some RTFRs were received, but we were unable to get a complete RTFR result. This is most likely due to a SATL limitation.
        COMMAND_TIMEOUT         = 14, //command took longer than the timeout that was provided to the OS
        WARN_NOT_ALL_DEVICES_ENUMERATED = 15,
        WARN_INVALID_CHECKSUM   = 16, //The checksum on the data for a command didn't calculate correctly (EX: Identify device, some ATA Logs)
        OS_COMMAND_NOT_AVAILABLE = 17, //This is returned when the OS does not have a way to issue the requested command. (EX: Trying to send an NVMe command without Win10, or trying a 32byte SCSI command pre-Win8)
        OS_COMMAND_BLOCKED      = 18, //This is returned when the OS is blocking the command from being issued (EX: TCG - linux, lib ATA......or Sanitize in Windos 8+)
        COMMAND_INTERRUPTED     = 19, //Nidhi - Added for SCT commands, if interrupted by some other SCT command.
		VALIDATION_FAILURE       =20, //For UDS/SM2 validation check
        STRIP_HDR_FOOTER_FAILURE = 21, //For UDS
        PARSE_FAILURE             =22,
        INVALID_LENGTH           = 23,  // Binary file has a invalid length or the parameters for the length don't match the size of the fiile
        ERROR_WRITING_FILE       = 24, //LookTan added for fwrite check on May20'20
		TIMEOUT					 = 25, //Pranali added for indicating operation timeout for SeaQueue
        OS_TIMEOUT_TOO_LARGE     = 26, //Tyler added for cases where a requested timeout is larger than the OS is capable of supporting in passthrough
        UNKNOWN
    }eReturnValues;

    typedef enum _eDataTransferDirection 
    {
        XFER_NO_DATA,
        XFER_DATA_IN,     // Transfer from target to host
        XFER_DATA_OUT,    // Transfer from host to target
        XFER_DATA_OUT_IN, // Transfer from host to target, followed by target to host
        XFER_DATA_IN_OUT, // Transfer from target to host, followed by host to target
     } eDataTransferDirection;

    typedef enum _eVerbosityLevels
    {
        VERBOSITY_QUIET           = 0,
        VERBOSITY_DEFAULT         = 1,
        VERBOSITY_COMMAND_NAMES   = 2,
        VERBOSITY_COMMAND_VERBOSE = 3,
        VERBOSITY_BUFFERS         = 4
    }eVerbosityLevels;

    #define M_NibblesTo1ByteValue(n1, n0) ( \
    (uint8_t)( ((uint8_t)((n1) & 0x0F) << 4) | ((uint8_t)((n0) & 0x0F) << 0)) \
                                           )

    // Big endian parameter order, little endian value
    #define M_BytesTo4ByteValue(b3, b2, b1, b0)                    (        \
    (uint32_t)(  ((uint32_t)(b3) << 24) | ((uint32_t)(b2) << 16) |          \
                 ((uint32_t)(b1) <<  8) | ((uint32_t)(b0) <<  0)  )         \
                                                                   )
    // Big endian parameter order, little endian value
    #define M_BytesTo8ByteValue(b7, b6, b5, b4, b3, b2, b1, b0)    (        \
    (uint64_t)( ((uint64_t)(b7) << 56) | ((uint64_t)(b6) << 48) |           \
                ((uint64_t)(b5) << 40) | ((uint64_t)(b4) << 32) |           \
                ((uint64_t)(b3) << 24) | ((uint64_t)(b2) << 16) |           \
                ((uint64_t)(b1) <<  8) | ((uint64_t)(b0) <<  0)  )          \
                                                                   )

    // Big endian parameter order, little endian value
    #define M_BytesTo2ByteValue(b1, b0)                            (        \
    (uint16_t)(  ((uint16_t)(b1) << 8) | ((uint16_t)(b0) <<  0)  )          \
                                                                   )

    // Big endian parameter order, little endian value
    #define M_WordsTo4ByteValue(w1, w0)                            (        \
    (uint32_t)(  ((uint32_t)(w1) << 16) | ((uint32_t)(w0) <<  0)  )         \
                                                                   )

    #define M_WordsTo8ByteValue(w3, w2, w1, w0)                    (   \
    (uint64_t)(  ((uint64_t)(w3) << 48) | ((uint64_t)(w2) << 32) |     \
                 ((uint64_t)(w1) << 16) | ((uint64_t)(w0) <<  0)  )    \
                                                                   )

    // Big endian parameter order, little endian value
    #define M_DWordsTo8ByteValue(d1, d0)                           (        \
    (uint64_t)(  ((uint64_t)(d1) << 32) | ((uint64_t)(d0) <<  0)  )         \
                                                                   )
    // Max & Min Helpers
    #define  M_Min(a,b)    (((a)<(b))?(a):(b))
    #define  M_Max(a,b)    (((a)>(b))?(a):(b))

    // Convert the result to a boolean true or false (ternary operator)
    // This is especially good for C++ since you get conversion warnings, but this is an acceptible method for C++ code
    #define M_ToBool(expression) ((expression) > 0 ? true : false)

    typedef enum _eOutputFormat
    {
        SEAC_OUTPUT_TEXT, //default way everything is output
        SEAC_OUTPUT_RAW,  //This will output the data as raw binary glob
        SEAC_OUTPUT_JSON,
        //TODO: add other output formats as we want to support them
        SEAC_OUTPUT_CSV, //just an idea...isn't implemented now
        SEAC_OUTPUT_XML, //just an idea...isn't implemented now
    }eOutputFormat;


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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    //!   \return VOID
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
    size_t find_last_occurrence_in_string(char *originalString, char *stringToFind);

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
    //!   \return VOID
    //
    //-----------------------------------------------------------------------------
    void print_Data_Buffer(uint8_t *dataBuffer, uint32_t bufferLen, bool showPrint);

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
    //!   \return VOID
    //
    //-----------------------------------------------------------------------------
    void print_Return_Enum(char *funcName, int ret);

    //-----------------------------------------------------------------------------
    //
    //  metric_Unit_Convert()
    //
    //! \brief   Description:  Takes a double value which is a a numberic value specifying a number of bytes, and 
    //!                        converts it to KB, MB, GB, TB, etc in a more easy to display manor. Sets the units for you for display purposes too.
    //
    //  Entry:
    //!   \param[in] byteValue = value specifying a number of bytes
    //!   \param[in] metricUnit = char ptr to hold the metric unit. This can be NULL
    //!
    //  Exit:
    //!   \return SUCCESS on successful completion, !SUCCESS if problems encountered
    //
    //-----------------------------------------------------------------------------
    int metric_Unit_Convert(double *byteValue, char** metricUnit);

    //-----------------------------------------------------------------------------
    //
    //  capacity_Unit_Convert()
    //
    //! \brief   Description:  Takes a double value which is a a numeric value specifying a number of bytes, and 
    //!                        converts it to KiB, MiB, GiB, TiB, etc in a more easy to display manor. Sets the units for you for display purposes too.
    //
    //  Entry:
    //!   \param[in] byteValue = value specifying a number of bytes
    //!   \param[in] capacityUnit = char ptr to hold the metric unit. This can be NULL
    //!
    //  Exit:
    //!   \return SUCCESS on successful completion, !SUCCESS if problems encountered
    //
    //-----------------------------------------------------------------------------
    int capacity_Unit_Convert(double *byteValue, char** capacityUnit);

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
    //!   \return void
    //
    //-----------------------------------------------------------------------------
    #define safe_Free(mem)  \
    if(mem)                 \
    {                       \
        free(mem);          \
        mem = NULL;         \
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
    //!   \return void
    //
    //-----------------------------------------------------------------------------
    void convert_Seconds_To_Displayable_Time(uint64_t secondsToConvert, uint8_t *years, uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

    void convert_Seconds_To_Displayable_Time_Double(double secondsToConvert, uint8_t *years, uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

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
    bool get_And_Validate_Integer_Input(const char * strToConvert, uint64_t * outputInteger);

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
    //!   \return void
    //
    //-----------------------------------------------------------------------------
    void print_Time_To_Screen(uint8_t *years, uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

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
    //!   \return void
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
    //!   \return void
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
    int fill_Random_Pattern_In_Buffer(uint8_t *ptrData, uint32_t dataLength);

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
    int fill_Hex_Pattern_In_Buffer(uint32_t hexPattern, uint8_t *ptrData, uint32_t dataLength);

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
    int fill_Incrementing_Pattern_In_Buffer(uint8_t incrementStartValue, uint8_t *ptrData, uint32_t dataLength);

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
    int fill_ASCII_Pattern_In_Buffer(const char *asciiPattern, uint32_t patternLength, uint8_t *ptrData, uint32_t dataLength);

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
    int fill_Pattern_Buffer_Into_Another_Buffer(uint8_t *inPattern, uint32_t inpatternLength, uint8_t *ptrData, uint32_t dataLength);

    double convert_128bit_to_double(uint8_t * pData);

    //meant to replace calling gmtime() function as this handles all the cross-platform weirdness calling the safest possible version
    struct tm * get_UTCtime(const time_t *timer, struct tm *buf);

    //meant to replace calling localtime() function as this handles all the cross-platform weirdness calling the safest possible version
    struct tm * get_Localtime(const time_t *timer, struct tm *buf);

    #define TIME_STRING_LENGTH 26
    //meant to replace calling asctime() function as this handles all the cross-platform weirdness calling the safest possible version
    char * get_Time_String_From_TM_Structure(const struct tm * timeptr, char *buffer, size_t bufferSize);

    #define CURRENT_TIME_STRING_LENGTH TIME_STRING_LENGTH
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

    typedef enum _eCompiler
    {
        OPENSEA_COMPILER_UNKNOWN,
        OPENSEA_COMPILER_MICROSOFT_VISUAL_C_CPP,
        OPENSEA_COMPILER_GCC,
        OPENSEA_COMPILER_CLANG,
        OPENSEA_COMPILER_MINGW,
        OPENSEA_COMPILER_INTEL_C_CPP,//not supported right now
        OPENSEA_COMPILER_SUNPRO_C_CPP,//not supported right now
        OPENSEA_COMPILER_IBM_XL_C_CPP,//not supported right now
        OPENSEA_COMPILER_IBM_SYSTEMZ_C_CPP,//very similar to XLC/C++ compiler but slightly different versioning - not supported right now
        OPENSEA_COMPILER_HP_A_CPP,//not supported right now
        //Add other compilers here if we ever add more than those above (which not all listed above are supported!)
        OPENSEA_COMPILER_RESERVED
    }eCompiler;

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
    int get_Compiler_Info(eCompiler *compilerUsed, ptrCompilerVersion compilerVersionInfo);

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
    //!   \return void
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
    //!   \return void
    //
    //-----------------------------------------------------------------------------
    void print_Compiler_Version_Info(ptrCompilerVersion compilerVersionInfo);

    //-----------------------------------------------------------------------------
    //
    //  get_File_Size(FILE *filePtr)
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
    //!   \return VOID
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
    //!   \return void
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
    //!   \return void
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
    //!   \return void
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
    //!   \return void
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
    //!   \return void
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

    //
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
    //!   \return void
    //
    //-----------------------------------------------------------------------------
    void get_Decimal_From_4_byte_Float(uint32_t floatValue, double *decimalValue);

#if defined (__cplusplus)
} //extern "C"
#endif
