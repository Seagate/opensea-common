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
// \file common_types.h
// \brief Defines common types to use throughout cross platform code. If a type
// does not exist on one system, but does on another and it
//        is useful for library functions, it will be defined in here.
//
#pragma once

#if defined(__cplusplus)
// defining these macros for C++ to make older C++ compilers happy and work like
// the newer C++ compilers
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#endif //__cplusplus

#include "predef_env_detect.h"

#if defined(UEFI_C_SOURCE)
#include <sys/syslimits.h>
#endif

// Define this to allow reading larger files in 32bit OSs without a limitation.
// https://www.gnu.org/software/libc/manual/html_node/Feature-Test-Macros.html
#if !defined(_FILE_OFFSET_BITS)
#define _FILE_OFFSET_BITS 64
#elif _FILE_OFFSET_BITS < 64
#undef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include <sys/stat.h>
#include <sys/types.h> //This is available in Windows and linux/unix-like systems
#if defined(_WIN32)
#include <BaseTsd.h> //for MAXSSIZE_T
#include <windows.h> //Includes various Windows headers for types and functions. Used for MAX_PATH below
#else
#include <sys/param.h>
#endif             //_WIN32
#include <errno.h> //for errno_t if it is available
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h> //memset

#if defined(__cplusplus)
extern "C"
{
#endif

#if defined(_WIN32)
    // Windows has stat/stat64 and uses shorts for these members.
    typedef short          uid_t;
    typedef short          gid_t;
    typedef unsigned short mode_t;
    typedef short          nlink_t;
    // using our own "type" to make sure we can get as much of the filesize as
    // possible..and workaround Windows issues with off_t -TJE
    typedef int64_t offset_t; // NOTE: WinAPI uses __int64_t which is the same as long long
                              // which is also what int64_t from stdint.h is defined as
    typedef unsigned long winsyserror_t;
#else
// using our own "type" to make sure we can get as much of the filesize as
// possible..and workaround Windows issues with off_t -TJE
typedef off_t offset_t; // to deal with windows differences in off_t definitions
                        // in stat
#endif

#if !defined(HAVE_C11_ANNEX_K) && !defined(__STDC_SECURE_LIB__)
    typedef int errno_t;
#endif //! HAVE_C11_ANNEX_K && !__STDC_SECURE_LIB__

#if !defined(UINTPTR_MAX)
// need uintptr_t type for NVMe capabilities to prevent warnings/errors
// TODO: if C11, _Static_assert can be used to check against sizeof(void*) to
// make sure this is defined in a way that should work.
#if defined(_WIN64) || defined(_M_IA64) || defined(_M_ALPHA) || defined(_M_X64) || defined(_M_AMD64) ||                \
    defined(__alpha__) || defined(__amd64__) || defined(__x86_64__) || defined(__aarch64__) || defined(__ia64__) ||    \
    defined(__IA64__) || defined(__powerpc64__) || defined(__PPC64__) || defined(__ppc64__) ||                         \
    defined(_ARCH_PPC64) // 64bit
    typedef uint64_t uintptr_t;
#define UINTPTR_MAX UINT64_MAX
#else // assuming 32bit
    typedef uint32_t uintptr_t;
#define UINTPTR_MAX UINT32_MAX
#endif // Checking for 64bit CPUs
#endif //! UINTPTR_MAX

#if !defined(INTPTR_MAX)
// need uintptr_t type for NVMe capabilities to prevent warnings/errors
// TODO: if C11, _Static_assert can be used to check against sizeof(void*) to
// make sure this is defined in a way that should work.
#if defined(_WIN64) || defined(_M_IA64) || defined(_M_ALPHA) || defined(_M_X64) || defined(_M_AMD64) ||                \
    defined(__alpha__) || defined(__amd64__) || defined(__x86_64__) || defined(__aarch64__) || defined(__ia64__) ||    \
    defined(__IA64__) || defined(__powerpc64__) || defined(__PPC64__) || defined(__ppc64__) ||                         \
    defined(_ARCH_PPC64) // 64bit
    typedef int64_t intptr_t;
#define INTPTR_MAX INT64_MAX
#define INTMAX_MIN INT64_MIN
#else // assuming 32bit
    typedef int32_t  intptr_t;
#define INTPTR_MAX INT32_MAX
#define INTMAX_MIN INT32_MIN
#endif // Checking for 64bit CPUs
#endif //! INTPTR_MAX

#if !defined(SSIZE_MAX) && !defined(SSIZE_MIN) && !defined(SSIZE_T_DEFINED)
// assume ssize_t is not defined
#if defined(_MSC_VER) && defined(MAXSSIZE_T) && defined(MINSSIZE_T)
    // MSFT defined this as SSIZE_T, so just make it lowercase to match POSIX
    // NOTE: reviewing MSFT's headers shows this is the same as intptr_t definitions
    // as well
    typedef SSIZE_T ssize_t;
#define SSIZE_MAX MAXSSIZE_T
#define SSIZE_MIN MINSSIZE_T
#else //!_MSCVER && !MAXSSIZE_T && !MINSSIZE_T
    // using intptr_t since it has the same range.
    typedef intptr_t ssize_t;
#define SSIZE_MAX INTPTR_MAX
#define SSIZE_MIN INTPTR_MIN
#endif //_MSC_VER && MAXSSIZE_T && MINSSIZE_T
#define SSIZE_T_DEFINED
#endif // SSIZE_MAX && _MSC_VER

// define something called reserved that has a value of zero. Use it to set
// reserved bytes to 0
#define RESERVED 0
#define OBSOLETE 0

// M_NULLPTR vs nullptr can be an issue with overloads in C++ and _GENERIC
// selection in C While M_NULLPTR should be defined as nullptr in C23, adding
// this definition to remove ambiguity when we are checking for valid pointers
// in our code. This will use a proper type depending on the standard detected
// at compile time
// https://stackoverflow.com/questions/1282295/what-is-the-nullptr-keyword-and-why-is-it-better-than-null
#if defined(USING_CPP98)
// NOTE: This is declared at the bottom of the file outside of the extern C to
// avoid warnings/errors
#else // C
#if defined(USING_C23)
#define M_NULLPTR nullptr
#elif defined(NULL)
                        // use NULL since this is commonly available and likely
                        // to be safe for the environment we are
                        // in.
// NOTE: If this is ever an issue, consider improving this check to fall into
// the #else below
//       instead where it is defined as a void* of 0 which should work most of
//       the time unless the platform is doing something funky.
//       https://stackoverflow.com/questions/2597142/when-was-the-null-macro-not-0
#define M_NULLPTR NULL
#else
                        // for unknown reasons M_NULLPTR was not defined so
                        // define it the most common way we can to be
                        // safe
#define M_NULLPTR ((void*)0)
#endif // C23
#endif // C & C++ M_NULLPTR PTR definitions

// define SIZE_MAX if not defined...probably not needed since we are expecting
// at least C99
#if !defined(SIZE_MAX)
#if defined(UINTPTR_MAX)
#define SIZE_MAX UINTPTR_MAX
#else //! UINTPTR_MAX
#define SIZE_MAX (size_t)(~((size_t)0))
#endif // UINTPTR_MAX
#endif //! SIZE_MAX

#if !defined(RSIZE_MAX)
    typedef size_t rsize_t;
#define RSIZE_MAX (SIZE_MAX >> 1)
#endif //! RSIZE_MAX

// Use this macro to declare enums for C and enum classes for C++11 and later.
// Without enum classes there are a lot of excess warnings and this will make it
// easier to resolve all warnings.
#if defined(USING_CPP11)
#define M_DECLARE_ENUM(name, ...)                                                                                      \
    enum class name                                                                                                    \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    }
#elif defined(USING_CPP98)
#define M_DECLARE_ENUM(name, ...)                                                                                      \
    enum name                                                                                                          \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    }
#else //! CPP11
#define M_DECLARE_ENUM(name, ...) typedef enum _##name{__VA_ARGS__} name
#endif // CPP11

// If you want a specific type underlying the enum, use M_DECLAR_ENUM_TYPE to
// specify it. This can be something like M_DECLARE_ENUM_TYPE(myEnum, uint32_t,
// ...) If you set type to "int" then it's the same as M_DECLARE_ENUM
#if defined(USING_CPP11)
#define M_DECLARE_ENUM_TYPE(name, type, ...)                                                                           \
    enum class name : type                                                                                             \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    }
#elif defined(USING_CPP98)
#define M_DECLARE_ENUM_TYPE(name, type, ...)                                                                           \
    enum name                                                                                                          \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    }
#else //! CPP11...old CPP or C
#if defined(USING_C23)
#define M_DECLARE_ENUM_TYPE(name, type, ...) typedef enum _##name : type{__VA_ARGS__} name
#else
/*cannot specify the type, so just ignore the input for now-TJE*/
#define M_DECLARE_ENUM_TYPE(name, type, ...) typedef enum _##name{__VA_ARGS__} name
#endif
#endif // CPP11

// This macro is really for C++, but can be used in C as well. It helps with the
// differences between C++11 and C++98 when accessing an enum value for
// comparison or assignment.
#if defined(USING_CPP11)
#define M_ACCESS_ENUM(type, val) type::val
#else
#define M_ACCESS_ENUM(type, val) val
#endif

// A macro to help define packed structures in a way that best works with
// different compilers
#if defined(_MSC_VER)
#define M_PACK_ALIGN_STRUCT(name, alignmentval, ...)                                                                   \
    __pragma(pack(push, alignmentval));                                                                                \
    typedef struct _##name                                                                                             \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    } name;                                                                                                            \
    __pragma(pack(pop))
#elif defined(__GNUC__) || defined(__clang__) || defined(__MINGW32__) || defined(__MINGW64__)
#define M_PACK_ALIGN_STRUCT(name, alignmentval, ...)                                                                   \
    typedef struct _##name                                                                                             \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    } __attribute__((packed, aligned(alignmentval))) name
#else
#define M_PACK_ALIGN_STRUCT(name, alignmentval, ...)                                                                   \
    typedef struct _##name                                                                                             \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    } name
#endif

// same idea as above but without providing a specific alignment requirement,
// just to pack as small as "possible" This is not exactly the same as providing
// an alignment of 1 in GCC, but it is as close as MSVC can get to the same
// behavior
#if defined(_MSC_VER)
#define M_PACKED_STRUCT(name, ...)                                                                                     \
    __pragma(pack(push, 1));                                                                                           \
    typedef struct _##name                                                                                             \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    } name;                                                                                                            \
    __pragma(pack(pop))
#elif defined(__GNUC__) || defined(__clang__) || defined(__MINGW32__) || defined(__MINGW64__)
#define M_PACKED_STRUCT(name, ...)                                                                                     \
    typedef struct _##name                                                                                             \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    } __attribute__((packed)) name
#else
#define M_PACKED_STRUCT(name, ...)                                                                                     \
    typedef struct _##name                                                                                             \
    {                                                                                                                  \
        __VA_ARGS__                                                                                                    \
    } name
#endif

#if !defined(USING_CPP98)
// only use these methods in C
// The C++ version is at the end of this file outside of the extern "C"
#if defined(__GNUC__)
// This is a GNU extension to zero initialize the array
#define DECLARE_ZERO_INIT_ARRAY(type_name, array_name, size) type_name array_name[size] = {[0 ...((size)-1)] = 0}
#else
#if defined(USING_C99)
    // Define a function for zero-initialization to be compatible with other C99
    // compilers where we can call a function after defining the type without
    // generating compiler warnings or errors. NOTE: Not using M_INLINE macro here
    // because including that code_attributes.h will generate a circular include
    // issue Since we are already checking for C99 support here, we can use inline
    // since it's part of C99 NOTE: Cannot call safe_memset within opensea-common at
    // the moment because of circular include so this version has a lot of the same
    // ifdefs at the moment.
    // TODO: cleanup how we implement safe_memset to make that easier to call or
    // explicit_zeroes to reduce the amount of ifdefs in here.
    static inline void zero_init_array(void* array, size_t element_size, size_t element_count)
    {
#if defined(USING_C23) || defined(HAVE_MEMSET_EXPLICIT)
        memset_explicit(array, 0, element_size * element_count);
#elif defined(HAVE_C11_ANNEX_K) || defined(HAVE_MEMSET_S)
        memset_s(array, element_size * element_count, 0, element_size * element_count);
#elif (defined(_WIN32) && defined(_MSC_VER)) || defined(HAVE_MSFT_SECURE_ZERO_MEMORY) ||                               \
    defined(HAVE_MSFT_SECURE_ZERO_MEMORY2)
#if !defined(NO_HAVE_MSFT_SECURE_ZERO_MEMORY2) &&                                                                      \
    (defined(HAVE_MSFT_SECURE_ZERO_MEMORY2) ||                                                                         \
     (defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN11_26100))
        // use secure zero memory 2
        // Cast is to remove warning about different volatile qualifiers
        SecureZeroMemory2(array, element_size * element_count);
#else
        // use microsoft's SecureZeroMemory function
        SecureZeroMemory(array, element_size * element_count);
#endif
#else
        memset(array, 0, element_size * element_count);
#endif
    }

#define DECLARE_ZERO_INIT_ARRAY(type_name, array_name, size)                                                           \
    type_name array_name[size];                                                                                        \
    zero_init_array(array_name, sizeof(type_name), size)
#else
// This is not exactly the same, but what we have done for years and usually
// works out ok... The only way to make this work for old standards 100% is a
// memset, but we don't want to do that because C89 requires variable
// declarations before functions are run, so defining like this. Some compilers
// will interpret this to mean zero initialize the array, but it is not
// guaranteed.
#define DECLARE_ZERO_INIT_ARRAY(type_name, array_name, size) type_name array_name[size] = {0}
#endif
#endif
#endif

// Setup a way to do static-assertions
// NOTE: Do not pass a quoted string for backwards compatibility with the
// fallback case. example: M_STATIC_ASSET(condition, this_is_my_example_error)
#if defined(USING_CPP11) && defined(__cpp_static_assert)
#define M_STATIC_ASSERT(condition, message) static_assert(condition, #message)
#elif defined(USING_C23)
#define M_STATIC_ASSERT(condition, message) static_assert(condition, #message)
#elif defined(USING_C11)
#include <assert.h>
#define M_STATIC_ASSERT(condition, message) _Static_assert(condition, #message)
#else
// Generic way to do this. Not as good messaging but should work about the same
#define M_STATIC_ASSERT(condition, message)                                                                            \
    typedef char static_assertion_##message[(condition) ? 1 : -1] /* M_ATTR_UNUSED                                     \
                                                                   */
#endif

#if defined(MAX_PATH)
// more info on max path in Windows
// https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=cmd
#define OPENSEA_PATH_MAX MAX_PATH
#elif defined(PATH_MAX)
#define OPENSEA_PATH_MAX PATH_MAX
#elif defined(MAXPATHLEN)
// In 4.4BSD and Solaris, the limit on the pathname length is MAXPATHLEN (found
// in <sys/param.h>).
#define OPENSEA_PATH_MAX MAXPATHLEN
#else
// a system is not defining the length. You can read it with pathconf or
// sysconf, but that does not do us any good for how we use this define in some
// of our code, so defaulting to 4096 If we need to change this default case on
// a system by system basis, we can check system unique definitions to update
// this.
#define OPENSEA_PATH_MAX (4096)
#endif

// NOTE: Technically UEFI shell uses Windows style seperators, but is a
// POSIX-like environment in many ways.
//       This is not currently set this way as / is accepted in UEFI shell apps
//       and functions There are other places in our code that will need updates
//       to handle converting to \\ style path seperators in UEFI shell - TJE
#if defined(_WIN32) && !defined(UEFI_C_SOURCE)
#define SYSTEM_PATH_SEPARATOR     '\\'
#define SYSTEM_PATH_SEPARATOR_STR "\\"
#else
#define SYSTEM_PATH_SEPARATOR     '/'
#define SYSTEM_PATH_SEPARATOR_STR "/"
#endif

#define ROOT_UID_VAL (0)

// Typeof or decltype can be helpful, but was added in newer standards.
// some compilers have it as an extension, so define a macro to make it usable
// when it is available if not available, add the definition NO_TYPEOF
#if defined(USING_C23)
#define M_TYPEOF(var) typeof(var)
#elif defined(USING_CPP11) one web comment mentions this is closer:
                        // std::decay<decltype((X))>::type but this may be
                        // situational. Not sure...-TJE
#define M_TYPEOF(var) decltype(var)
#elif defined(_MSC_VER) && _MSC_VER >= 1939
// added in Visual Studio 2022 version 17.9 and later
#define M_TYPEOF(var) __typeof__(var)
#elif defined(__clang__) || defined(__GNUC__)
// GCC 2 and later have typeof support so not even checking the version in this
// case
#define M_TYPEOF(var) __typeof__(var)
#else
// This is not available in the current compiler/environment
// If this is defined then M_TYPEOF will not be available to use
#define NO_TYPEOF
#endif

    M_DECLARE_ENUM(eReturnValues,
                   SUCCESS         = 0,
                   FAILURE         = 1,
                   NOT_SUPPORTED   = 2,
                   COMMAND_FAILURE = 3,
                   IN_PROGRESS     = 4,        /* another command is in progress, or a command has started
                                                  and is now in progress in the background */
                   ABORTED       = 5,          /* Command was aborted */
                   BAD_PARAMETER = 6,          /* the only time this return code should be seen is when
                                                  a developer is writing code to add something. This
                                                  should not appear in a finished application */
                   MEMORY_FAILURE         = 7, /* could not allocate memory */
                   OS_PASSTHROUGH_FAILURE = 8, /* For some unknown reason, the OS API call to
                                                  issue the pass-through command failed. */
                   LIBRARY_MISMATCH = 9,
                   FROZEN           = 10,      /* use this to communicate back when the device is in a frozen
                                                  state for a commmand like sanitize or ata security */
                   PERMISSION_DENIED     = 11, /* OS returned Access/permission denied */
                   FILE_OPEN_ERROR       = 12,
                   WARN_INCOMPLETE_RFTRS = 13, /* command was issued, and some RTFRs were received,
                                                  but we were unable to get a complete RTFR result.
                                                  This is most likely due to a SATL limitation. */
                   OS_COMMAND_TIMEOUT = 14,    /* command took longer than the timeout that was
                                                  provided to the OS */
                   WARN_NOT_ALL_DEVICES_ENUMERATED = 15,
                   WARN_INVALID_CHECKSUM = 16,    /* The checksum on the data for a command didn't calculate correctly
                                                     (EX: Identify device, some ATA Logs) */
                   OS_COMMAND_NOT_AVAILABLE = 17, /* This is returned when the OS does not have a way to issue the
                                                     requested command. (EX: Trying to send an NVMe command without
                                                     Win10, or trying a 32byte SCSI command pre-Win8) */
                   OS_COMMAND_BLOCKED = 18,       /* This is returned when the OS is blocking the
                                                     command from being issued (EX: TCG -  linux, lib
                                                     ATA......or Sanitize in Windos 8+) */
                   COMMAND_INTERRUPTED = 19,      /* Added for SCT commands, if interrupted by some
                                                     other SCT command. */
                   VALIDATION_FAILURE       = 20, /* validation check*/
                   STRIP_HDR_FOOTER_FAILURE = 21, /* For UDS */
                   PARSE_FAILURE            = 22,
                   INVALID_LENGTH           = 23,  /* Binary file has a invalid length or the parameters
                                                      for the length don't match the size of the file */
                   ERROR_WRITING_FILE   = 24,      /*added for fwrite check on May20'20 */
                   TIMEOUT              = 25,      /*added for indicating operation timeout for SeaQueue */
                   OS_TIMEOUT_TOO_LARGE = 26,      /*added for cases where a requested timeout is larger than the OS is
                                                      capable of supporting in passthrough */
                   PARSING_EXCEPTION_FAILURE = 27, /*For C/C++ exception failure while parsing */
                   DIR_CREATION_FAILED       = 28, /*For Telemetry Log Parser, when the creation of
                                                      output folder fails */
                   FILE_READ_ERROR      = 29,      /*For Telemetry Log Parser, when reading logfile to buffer fails */
                   POWER_CYCLE_REQUIRED = 30,      /*For some firmware update scenarios, a power
                                                      cycle is required to complete the update. This
                                                      code is returned in these situations. */
                   DEVICE_ACCESS_DENIED = 31,      /*While similar to PERMISSION_DENIED, this is meant for a response
                                                      from the drive telling us it is not allowing something versus the
                                                      OS telling us something is not allowed.-TJE */
                   NOT_PARSED          = 32,       /* File was not parsed */
                   MISSING_INFORMATION = 33,       /* file is missing information inorder to be parsed correctly */
                   TRUNCATED_FILE      = 34,       /* file is truncated and might be missing data for
                                                      parsing it correctly */
                   INSECURE_PATH = 35,
                   UNKNOWN);

    M_DECLARE_ENUM(eDataTransferDirection,
                   XFER_NO_DATA,
                   XFER_DATA_IN,     /* Transfer from target to host */
                   XFER_DATA_OUT,    /* Transfer from host to target    */
                   XFER_DATA_OUT_IN, /* Transfer from host to target, followed by
                                        target to host */
                   XFER_DATA_IN_OUT, /* Transfer from target to host, followed by
                                        host to target */
    );

    M_DECLARE_ENUM(eVerbosityLevels,
                   VERBOSITY_QUIET           = 0,
                   VERBOSITY_DEFAULT         = 1,
                   VERBOSITY_COMMAND_NAMES   = 2,
                   VERBOSITY_COMMAND_VERBOSE = 3,
                   VERBOSITY_BUFFERS         = 4,
                   VERBOSITY_MAX /*This should always be at the end of this list. This allows
                                    for a function to use this to check bounds even if we add
                                    more levels ahead of this. -TJE*/
    );

    // json data type sets. used for formating data to a customer demands
    M_DECLARE_ENUM(eDataFormat,
                   JSON_DATA      = 0, /*default*/
                   PREPYTHON_DATA = 1, );

    // Data buffer pointer and length block.
    typedef struct stDataPtr
    {
        void*  pData;    // If pData is M_NULLPTR, AllocLen must be zero also.
        size_t DataLen;  // Number of valid bytes.
        size_t AllocLen; // If AllocLen is zero, pData must be M_NULLPTR also.
    } tDataPtr;

    //-----------------------------------------------------------------------------
    //
    //  custom_Update(void *customData, char *message)
    //
    //! \brief   Description:  Provides a custom callback for updating important
    //! messages to a UI.
    //!                        For instance "Performing 5000 Random Reads" or
    //!                        "Sequential Read Test". This allows the UI to get
    //!                        occasional refreshes
    //
    //  Entry:
    //!   \param[in] customData = Custom data used to help the Updater function.
    //!   Usually hidden to the calling function. \param[in] message    = Message to
    //!   go to the UI.
    //!
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    typedef void (*custom_Update)(void* customData, char* message);

#if defined(__cplusplus)
}
#endif

// This has to live here to avoid errors about C linkage in older GCC in C++98
// mode
#if defined(USING_CPP98) && !defined(M_NULLPTR)
#if defined(USING_CPP11)
#define M_NULLPTR nullptr
#else
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++0x-compat" // treated as synonymn for Wc++11-compat, so
                                                  // use this for compatibility to old versions
// gcc 4.7.x - present calls it c++11-compat
// 4.3.6 - 4.6.4 calls it c++0x-compat
#elif defined(__GNUC__)
#if (defined(__GNUC__) && (__GNUC__ > 4) || (defined(__GNUC_MINOR__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 7))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++11-compat"
#elif (defined(__GNUC_MINOR__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++0x-compat"
#endif // no need to disable on older GCC as this warning didn't
       // exist
#endif // GCC or Clang
// NOTE: G++ defines M_NULLPTR as __null which should be safe
//       https://gcc.gnu.org/onlinedocs/gcc-4.9.2/libstdc++/manual/manual/support.html#std.support.types.null
//       Can add a special case to use M_NULLPTR macro instead if this template
//       class doesn't work as expected
const class nullptr_t
{
  public:
    template <class T> operator T*() const
    {
        return 0;
    }

    template <class C, class T> operator T C::*() const
    {
        return 0;
    }

  private:
    void operator&() const;
} nullptr = {};
#define M_NULLPTR nullptr
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
// the pop can be simplified
#if __GNUC__ > 4 || (defined(__GNUC_MINOR__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
#pragma GCC diagnostic pop
#endif
#endif
#endif // C++11 check for nullptr
#endif // c++98 and no M_NULLPTR

#if defined(USING_CPP98) && !defined(DECLARE_ZERO_INIT_ARRAY)
// NOTE: This does not use memset to handle non-trivial types.
//       A compiler may optimize this to memset though if it detects that this
//       is a zero initialization of the data -TJE
template <typename T, size_t N> void zero_init_array(T (&array)[N])
{
    for (size_t i = 0; i < N; ++i)
    {
        array[i] = 0;
    }
}
#define DECLARE_ZERO_INIT_ARRAY(type_name, array_name, size)                                                           \
    type_name array_name[size];                                                                                        \
    zero_init_array(array_name)
#endif // C++98 and no DECLARE_ZERO_INIT_ARRAY
