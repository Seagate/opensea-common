// SPDX-License-Identifier: MPL-2.0

//! \file common_types.h
//! \brief Defines common types to use throughout cross platform code.
//!
//! If a type does not exist on one system, but does on another and it
//! is useful for library functions, it will be defined in here.
//! Other various helper macros to initialize a type will also be found here.
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#if defined(__cplusplus)
//! \def __STDC_FORMAT_MACROS
//! \brief Enables the definition of format macros for C++.
//!
//! This macro is defined to make older C++ compilers compatible with newer C++ standards by enabling the definition of
//! format macros.
#    ifndef __STDC_FORMAT_MACROS
#        define __STDC_FORMAT_MACROS
#    endif

//! \def __STDC_LIMIT_MACROS
//! \brief Enables the definition of limit macros for C++.
//!
//! This macro is defined to make older C++ compilers compatible with newer C++ standards by enabling the definition of
//! limit macros.
#    ifndef __STDC_LIMIT_MACROS
#        define __STDC_LIMIT_MACROS
#    endif

//! \def __STDC_CONSTANT_MACROS
//! \brief Enables the definition of constant macros for C++.
//!
//! This macro is defined to make older C++ compilers compatible with newer C++ standards by enabling the definition of
//! constant macros.
#    ifndef __STDC_CONSTANT_MACROS
#        define __STDC_CONSTANT_MACROS
#    endif
#endif // __cplusplus

#include "code_attributes.h"
#include "predef_env_detect.h"

#if defined(UEFI_C_SOURCE)
#    include <sys/syslimits.h>
#endif

//! \def _FILE_OFFSET_BITS
//! \brief Enables large file support on 32-bit systems.
//!
//! This macro is defined to allow reading larger files on 32-bit operating systems without limitations. It sets the
//! file offset bits to 64. \note If _FILE_OFFSET_BITS is already defined and is less than 64, it is redefined to 64.
#if !defined(_FILE_OFFSET_BITS)
#    define _FILE_OFFSET_BITS 64
#elif _FILE_OFFSET_BITS < 64
#    undef _FILE_OFFSET_BITS
#    define _FILE_OFFSET_BITS 64
#endif

#include <sys/stat.h>
#include <sys/types.h> //This is available in Windows and linux/unix-like systems
#if defined(_WIN32)
#    include <BaseTsd.h> //for MAXSSIZE_T
DISABLE_WARNING_4255
#    include <windows.h> //Includes various Windows headers for types and functions. Used for MAX_PATH below
RESTORE_WARNING_4255
#else
#    include <sys/param.h>
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
    //! \typedef uid_t
    //! \brief Defines the user ID type for Windows.
    //!
    //! This type is defined as a short to match the Windows stat/stat64 structure.
    typedef short uid_t;

    //! \typedef gid_t
    //! \brief Defines the group ID type for Windows.
    //!
    //! This type is defined as a short to match the Windows stat/stat64 structure.
    typedef short gid_t;

    //! \typedef mode_t
    //! \brief Defines the mode type for Windows.
    //!
    //! This type is defined as an unsigned short to match the Windows stat/stat64 structure.
    typedef unsigned short mode_t;

    //! \typedef nlink_t
    //! \brief Defines the link count type for Windows.
    //!
    //! This type is defined as a short to match the Windows stat/stat64 structure.
    typedef short nlink_t;

    //! \typedef oscoffset_t
    //! \brief Defines the offset type for file sizes on Windows.
    //!
    //! This type is defined as int64_t to ensure compatibility with large file sizes and to work around Windows issues
    //! with off_t. \note WinAPI uses __int64_t, which is the same as long long, and int64_t from stdint.h is defined as
    //! long long.
    typedef int64_t oscoffset_t;

    //! \typedef winsyserror_t
    //! \brief Defines the system error type for Windows.
    //!
    //! This type is defined as an unsigned long to match the Windows system error type.
    typedef unsigned long winsyserror_t;
#else
//! \typedef oscoffset_t
//! \brief Defines the offset type for file sizes on non-Windows systems.
//!
//! This type is defined as off_t to ensure compatibility with large file sizes and to work around differences in off_t
//! definitions in stat.
typedef off_t oscoffset_t;
#endif

#if !defined(HAVE_C11_ANNEX_K) && !defined(HAVE_MSFT_SECURE_LIB)
    //! \typedef errno_t
    //! \brief Defines the error number type.
    //!
    //! This type is defined as an int to represent error numbers when C11 Annex K or Microsoft secure library is not
    //! available.
    typedef int errno_t;
#endif //! HAVE_C11_ANNEX_K && !HAVE_MSFT_SECURE_LIB

#if defined(USING_C23) || defined(USING_CPP23)
//! \def SIZE_T_C
//! \brief Defines a macro for size_t constants in C23 or C++23.
//!
//! This macro appends the 'ZU' suffix to a constant to define it as a size_t constant.
//! \param c The constant to be defined as a size_t constant.
#    if !defined(SIZE_T_C)
#        define SIZE_T_C(c) (c##ZU)
#    endif // SIZE_T_C

//! \def SSIZE_T_C
//! \brief Defines a macro for ssize_t constants in C23 or C++23.
//!
//! This macro appends the 'Z' suffix to a constant to define it as a ssize_t constant.
//! \param c The constant to be defined as a ssize_t constant.
#    if !defined(SSIZE_T_C)
#        define SSIZE_T_C(c) (c##Z)
#    endif // SSIZE_T_C
#endif     // C23 or C++23

#if defined(ENV_64BIT)
#    if !defined(UINTPTR_MAX)
    //! \typedef uintptr_t
    //! \brief Defines the unsigned integer type for pointer arithmetic on 64-bit systems.
    //!
    //! This type is defined as uint64_t to ensure compatibility with 64-bit pointers.
    typedef uint64_t uintptr_t;

//! \def UINTPTR_MAX
//! \brief Defines the maximum value for uintptr_t on 64-bit systems.
#        define UINTPTR_MAX UINT64_MAX
#    endif //! UINTPTR_MAX

#    if !defined(INTPTR_MAX)
    //! \typedef intptr_t
    //! \brief Defines the signed integer type for pointer arithmetic on 64-bit systems.
    //!
    //! This type is defined as int64_t to ensure compatibility with 64-bit pointers.
    typedef int64_t intptr_t;

//! \def INTPTR_MAX
//! \brief Defines the maximum value for intptr_t on 64-bit systems.
#        define INTPTR_MAX INT64_MAX

//! \def INTPTR_MIN
//! \brief Defines the minimum value for intptr_t on 64-bit systems.
#        define INTPTR_MIN INT64_MIN
#    endif //! INTPTR_MAX

//! \def SIZE_T_C
//! \brief Defines a macro for size_t constants on 64-bit systems.
//!
//! This macro appends the 'ULL' suffix to a constant to define it as a size_t constant.
#    define SIZE_T_C(c) UINT64_C(c)

//! \def RSIZE_T_C
//! \brief Defines a macro for rsize_t constants on 64-bit systems.
//!
//! This macro appends the 'ULL' suffix to a constant to define it as a rsize_t constant.
#    define RSIZE_T_C(c) UINT64_C(c)

//! \def SSIZE_T_C
//! \brief Defines a macro for ssize_t constants on 64-bit systems.
//!
//! This macro appends the 'LL' suffix to a constant to define it as a ssize_t constant.
#    define SSIZE_T_C(c) INT64_C(c)
#else // ENV_32BIT
#    if !defined(UINTPTR_MAX)
//! \typedef uintptr_t
//! \brief Defines the unsigned integer type for pointer arithmetic on 32-bit systems.
//!
//! This type is defined as uint32_t to ensure compatibility with 32-bit pointers.
typedef uint32_t uintptr_t;

//! \def UINTPTR_MAX
//! \brief Defines the maximum value for uintptr_t on 32-bit systems.
#        define UINTPTR_MAX UINT32_MAX
#    endif //! UINTPTR_MAX

#    if !defined(INTPTR_MAX)
//! \typedef intptr_t
//! \brief Defines the signed integer type for pointer arithmetic on 32-bit systems.
//!
//! This type is defined as int32_t to ensure compatibility with 32-bit pointers.
typedef int32_t intptr_t;

//! \def INTPTR_MAX
//! \brief Defines the maximum value for intptr_t on 32-bit systems.
#        define INTPTR_MAX INT32_MAX

//! \def INTPTR_MIN
//! \brief Defines the minimum value for intptr_t on 32-bit systems.
#        define INTPTR_MIN INT32_MIN
#    endif // INTPTR_MAX

//! \def SIZE_T_C
//! \brief Defines a macro for size_t constants on 32-bit systems.
//!
//! This macro appends the 'U' suffix to a constant to define it as a size_t constant.
#    define SIZE_T_C(c)  UINT32_C(c)

//! \def RSIZE_T_C
//! \brief Defines a macro for rsize_t constants on 32-bit systems.
//!
//! This macro appends the 'U' suffix to a constant to define it as a rsize_t constant.
#    define RSIZE_T_C(c) UINT32_C(c)

//! \def SSIZE_T_C
//! \brief Defines a macro for ssize_t constants on 32-bit systems.
//!
//! This macro appends the 'L' suffix to a constant to define it as a ssize_t constant.
#    define SSIZE_T_C(c) INT32_C(c)
#endif

#if defined(_WIN32)
//! \def ULONGLONG_C
//! \brief Defines a macro for unsigned long long constants.
//!
//! This macro appends the 'ULL' suffix to a constant to define it as an unsigned long long constant.
#    define ULONGLONG_C(c) (c##ULL)

//! \def LONGLONG_C
//! \brief Defines a macro for long long constants.
//!
//! This macro appends the 'LL' suffix to a constant to define it as an long long constant.
#    define LONGLONG_C(c) (c##LL)

//! \def ULONG_C
//! \brief Defines a macro for unsigned long constants.
//!
//! This macro appends the 'UL' suffix to a constant to define it as an unsigned long constant.
#    define ULONG_C(c) (c##UL)

//! \def LONG_C
//! \brief Defines a macro for long constants.
//!
//! This macro appends the 'L' suffix to a constant to define it as an long constant.
#    define LONG_C(c) (c##L)

//! \def DWORD_C
//! \brief Defines a macro for unsigned long constants.
//!
//! This macro appends the 'UL' suffix to a constant to define it as an unsigned long constant.
#    define DWORD_C(c) (c##UL)

//! \def WORD_C
//! \brief Defines a macro for unsigned short constants.
//!
//! This macro defines the constant as an unsigned short.
#    define WORD_C(c) (c)

//! \def BYTE_C
//! \brief Defines a macro for unsigned char constants.
//!
//! This macro defines the constant as an unsigned char.
#    define BYTE_C(c) (c)

//! \def UCHAR_C
//! \brief Defines a macro for unsigned char constants.
//!
//! This macro defines the constant as an unsigned char.
#    define UCHAR_C(c) (c)

//! \def INT_C
//! \brief Defines a macro for int constants.
//!
//! This macro defines the constant as an int.
#    define INT_C(c) (c)

//! \def UINT_C
//! \brief Defines a macro for unsigned int constants.
//!
//! This macro appends the 'U' suffix to a constant to define it as an unsigned int constant.
#    define UINT_C(c) (c##U)

//! \def MSFT_BOOL_TRUE
//! \brief Macro to wrap check for true. Since BOOL is an int, it's better to check != FALSE.
//! The reason for this is while TRUE is defined as 1 an API may return any non-zero value if it doesn't
//! explicitly return TRUE. So this is safer to use since it's either zero or not zero.
#    define MSFT_BOOL_TRUE(value) ((value) != FALSE)

//! \def MSFT_BOOL_FALSE
//! \brief Macro to wrap check for true. Since BOOL is an int, it's better to check == FALSE
//! The reason for this is while TRUE is defined as 1 an API may return any non-zero value if it doesn't
//! explicitly return TRUE. So this is safer to use since it's either zero or not zero.
#    define MSFT_BOOL_FALSE(value) ((value) == FALSE)
#endif

#if !defined(SSIZE_MAX) && !defined(SSIZE_MIN) && !defined(SSIZE_T_DEFINED)
#    if defined(_MSC_VER) && defined(MAXSSIZE_T) && defined(MINSSIZE_T)
    //! \typedef ssize_t
    //! \brief Defines the signed size type for Microsoft compilers.
    //!
    //! This type is defined as SSIZE_T to match POSIX standards.
    //! \note This is the same as intptr_t in Microsoft headers.
    typedef SSIZE_T ssize_t;

//! \def SSIZE_MAX
//! \brief Defines the maximum value for ssize_t on Microsoft compilers.
#        define SSIZE_MAX MAXSSIZE_T

//! \def SSIZE_MIN
//! \brief Defines the minimum value for ssize_t on Microsoft compilers.
#        define SSIZE_MIN MINSSIZE_T
#    else //!_MSCVER && !MAXSSIZE_T && !MINSSIZE_T
    //! \typedef ssize_t
    //! \brief Defines the signed size type.
    //!
    //! This type is defined as intptr_t to ensure compatibility with the same range.
    typedef intptr_t ssize_t;

//! \def SSIZE_MAX
//! \brief Defines the maximum value for ssize_t.
#        define SSIZE_MAX INTPTR_MAX

//! \def SSIZE_MIN
//! \brief Defines the minimum value for ssize_t.
#        define SSIZE_MIN INTPTR_MIN
#    endif //_MSC_VER && MAXSSIZE_T && MINSSIZE_T
#    define SSIZE_T_DEFINED
#endif // SSIZE_MAX && _MSC_VER

// Bit widths. Standardized in C23, but if the def is missing, we will define it -TJE
// NOTE: Using the table on this page as a reference https://en.cppreference.com/w/c/language/arithmetic_types
//       Some types may have a common width, otherwise we will do our best to set this based off the data model

//! \def CHAR_WIDTH
//! \brief Defines the width of a char type in bits.
//! \details Typically, this is defined as CHAR_BIT, which is usually 8 bits.
#if !defined(CHAR_WIDTH)
#    define CHAR_WIDTH CHAR_BIT
#endif

//! \def CHAR_MSB
//! \brief Defines a value with the most significant bit of char set to 1
#define CHAR_MSB (1 << (CHAR_WIDTH - 1))

//! \def SCHAR_WIDTH
//! \brief Defines the width of a signed char type in bits.
//! \details Standard width for signed char is 8 bits.
#if !defined(SCHAR_WIDTH)
#    define SCHAR_WIDTH 8
#endif

//! \def SCHAR_MSB
//! \brief Defines a value with the most significant bit of signed char set to 1
#define SCHAR_MSB (1 << (SCHAR_WIDTH - 1))

//! \def SHRT_WIDTH
//! \brief Defines the width of a short type in bits.
//! \details Standard width for short is 16 bits.
#if !defined(SHRT_WIDTH)
#    define SHRT_WIDTH 16
#endif

//! \def SHRT_MSB
//! \brief Defines a value with the most significant bit of short set to 1
#define SHRT_MSB (1 << (SHRT_WIDTH - 1))

//! \def INT_WIDTH
//! \brief Defines the width of an int type in bits.
//! \details Width varies based on data model:
//! - LP32: 16 bits
//! - ILP64: 64 bits
//! - Others: 32 bits
#if !defined(INT_WIDTH)
#    if defined(LP32_DATA_MODEL)
#        define INT_WIDTH 16
#    elif defined(ILP64_DATA_MODEL)
#        define INT_WIDTH 64
#    else
#        define INT_WIDTH 32
#    endif
#endif

//! \def INT_MSB
//! \brief Defines a value with the most significant bit of int set to 1
#define INT_MSB (1 << (INT_WIDTH - 1))

//! \def LONG_WIDTH
//! \brief Defines the width of a long type in bits.
//! \details Width varies based on data model:
//! - LP64 and ILP64: 64 bits
//! - Others: 32 bits
#if !defined(LONG_WIDTH)
#    if defined(LP64_DATA_MODEL) || defined(ILP64_DATA_MODEL)
#        define LONG_WIDTH 64
#    else
#        define LONG_WIDTH 32
#    endif
#endif

//! \def LONG_MSB
//! \brief Defines a value with the most significant bit of long set to 1
#define LONG_MSB (1L << (LONG_WIDTH - 1))

//! \def LLONG_WIDTH
//! \brief Defines the width of a long long type in bits.
//! \details Standard width for long long is 64 bits.
#if !defined(LLONG_WIDTH)
#    define LLONG_WIDTH 64
#endif

//! \def LLONG_MSB
//! \brief Defines a value with the most significant bit of long long set to 1
#define LLONG_MSB (1LL << (LLONG_WIDTH - 1))

//! \def UCHAR_WIDTH
//! \brief Defines the width of an unsigned char type in bits.
//! \details Standard width for unsigned char is 8 bits.
#if !defined(UCHAR_WIDTH)
#    define UCHAR_WIDTH 8
#endif

//! \def UCHAR_MSB
//! \brief Defines a value with the most significant bit of unsigned char set to 1
#define UCHAR_MSB (1 << (UCHAR_WIDTH - 1))

//! \def USHRT_WIDTH
//! \brief Defines the width of an unsigned short type in bits.
//! \details Standard width for unsigned short is 16 bits.
#if !defined(USHRT_WIDTH)
#    define USHRT_WIDTH 16
#endif

//! \def USHRT_MSB
//! \brief Defines a value with the most significant bit of unsigned short set to 1
#define USHRT_MSB (1 << (USHRT_WIDTH - 1))

//! \def UINT_WIDTH
//! \brief Defines the width of an unsigned int type in bits.
//! \details Width varies based on data model:
//! - LP32: 16 bits
//! - ILP64: 64 bits
//! - Others: 32 bits
#if !defined(UINT_WIDTH)
#    if defined(LP32_DATA_MODEL)
#        define UINT_WIDTH 16
#    elif defined(ILP64_DATA_MODEL)
#        define UINT_WIDTH 64
#    else
#        define UINT_WIDTH 32
#    endif
#endif

//! \def UINT_MSB
//! \brief Defines a value with the most significant bit of unsigned int set to 1
#define UINT_MSB (1U << (UINT_WIDTH - 1))

//! \def ULONG_WIDTH
//! \brief Defines the width of an unsigned long type in bits.
//! \details Width varies based on data model:
//! - LP64 and ILP64: 64 bits
//! - Others: 32 bits
#if !defined(ULONG_WIDTH)
#    if defined(LP64_DATA_MODEL) || defined(ILP64_DATA_MODEL)
#        define ULONG_WIDTH 64
#    else
#        define ULONG_WIDTH 32
#    endif
#endif

//! \def ULONG_MSB
//! \brief Defines a value with the most significant bit of unsigned long set to 1
#define ULONG_MSB (1UL << (ULONG_WIDTH - 1))

//! \def ULLONG_WIDTH
//! \brief Defines the width of an unsigned long long type in bits.
//! \details Standard width for unsigned long long is 64 bits.
#if !defined(ULLONG_WIDTH)
#    define ULLONG_WIDTH 64
#endif

//! \def ULLONG_MSB
//! \brief Defines a value with the most significant bit of unsigned long long set to 1
#define ULLONG_MSB (1ULL << (ULLONG_WIDTH - 1))

//! \def RESERVED
//! \brief Defines a macro for reserved bytes.
//!
//! This macro defines a reserved byte with a value of 0.
//! This macro can be used within UINTwidth_C type macros as needed.
#define RESERVED 0

//! \def OBSOLETE
//! \brief Defines a macro for obsolete bytes.
//!
//! This macro defines an obsolete byte with a value of 0.
//! This macro can be used within UINTwidth_C type macros as needed.
#define OBSOLETE 0

//! \def M_NULLPTR
//! \brief Defines a macro for nullptr to handle different standards and environments.
//!
//! This macro defines M_NULLPTR to handle different standards and environments, ensuring compatibility when checking
//! for valid pointers.
//! - For C++98, it is declared at the bottom of the file outside of extern "C" to avoid warnings/errors.
//! - For C23, it is defined as nullptr.
//! - If NULL is defined, it uses NULL.
//! - Otherwise, it is defined as ((void*)0).
#if defined(USING_CPP98)
// NOTE: This is declared at the bottom of the file outside of the extern "C" to avoid warnings/errors.
#else // C
#    if defined(USING_C23)
#        define M_NULLPTR nullptr
#    elif defined(NULL)
#        define M_NULLPTR NULL
#    else
#        define M_NULLPTR ((void*)0)
#    endif // C23
#endif     // C & C++ M_NULLPTR PTR definitions

#if !defined(SIZE_MAX)
//! \def SIZE_MAX
//! \brief Defines the maximum value for size_t.
//!
//! This macro defines the maximum value for size_t, which is used to represent the maximum size of an object.
//! If UINTPTR_MAX is defined, it uses that value; otherwise, it calculates the maximum value by casting -1 to size_t.
#    if defined(UINTPTR_MAX)
#        define SIZE_MAX UINTPTR_MAX
#    else //! UINTPTR_MAX
#        define SIZE_MAX (size_t)(~((size_t)0))
#    endif // UINTPTR_MAX
#endif     //! SIZE_MAX

#if !defined(RSIZE_MAX)
    //! \typedef rsize_t
    //! \brief Defines the restricted size type.
    //!
    //! This type is defined as size_t to represent restricted sizes.
    typedef size_t rsize_t;

//! \def RSIZE_MAX
//! \brief Defines the maximum value for rsize_t.
//!
//! This macro defines the maximum value for rsize_t as half of SIZE_MAX.
#    define RSIZE_MAX (SIZE_MAX >> 1)
#endif //! RSIZE_MAX

//! \def TIME_T_ERROR
//! \brief The error value for an invalid time_t, such as on output from time()
#if defined(USING_CPP98)
#    define TIME_T_ERROR static_cast<time_t>(-1)
#else // C
#    define TIME_T_ERROR ((time_t)(-1))
#endif

//! \def M_DECLARE_ENUM
//! \brief Declares an enum or enum class based on the detected C++ standard.
//!
//! This macro declares an enum or enum class based on the detected C++ standard to reduce warnings and ensure
//! compatibility.
//! \param name The name of the enum.
//! \param ... The enumerator list.
#if defined(USING_CPP11)
#    define M_DECLARE_ENUM(name, ...)                                                                                  \
        enum class name                                                                                                \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        }
#elif defined(USING_CPP98)
#    define M_DECLARE_ENUM(name, ...)                                                                                  \
        enum name                                                                                                      \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        }
#else //! CPP11
#    define M_DECLARE_ENUM(name, ...) typedef enum e_##name{__VA_ARGS__} name
#endif // CPP11

//! \def M_DECLARE_ENUM_TYPE
//! \brief Declares an enum or enum class with a specific underlying type based on the detected C++ standard.
//!
//! This macro declares an enum or enum class with a specific underlying type based on the detected C++ standard to
//! reduce warnings and ensure compatibility.
//! \param name The name of the enum.
//! \param type The underlying type of the enum.
//! \param ... The enumerator list.
#if defined(USING_CPP11)
#    define M_DECLARE_ENUM_TYPE(name, type, ...)                                                                       \
        enum class name : type                                                                                         \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        }
#elif defined(USING_CPP98)
#    define M_DECLARE_ENUM_TYPE(name, type, ...)                                                                       \
        enum name                                                                                                      \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        }
#else //! CPP11...old CPP or C
#    if defined(USING_C23)
#        define M_DECLARE_ENUM_TYPE(name, type, ...) typedef enum e_##name : type{__VA_ARGS__} name
#    else
/*cannot specify the type, so just ignore the input for now-TJE*/
#        define M_DECLARE_ENUM_TYPE(name, type, ...) typedef enum e_##name{__VA_ARGS__} name
#    endif
#endif // CPP11

//! \def M_ACCESS_ENUM
//! \brief Accesses an enum value for comparison or assignment based on the detected C++ standard.
//!
//! This macro accesses an enum value for comparison or assignment based on the detected C++ standard to ensure
//! compatibility.
//! \param type The enum type.
//! \param val The enum value.
#if defined(USING_CPP11)
#    define M_ACCESS_ENUM(type, val) type::val
#else
#    define M_ACCESS_ENUM(type, val) val
#endif

//! \def M_PACK_ALIGN_STRUCT
//! \brief Defines a packed and aligned structure.
//!
//! This macro defines a packed and aligned structure to ensure the structure is packed and aligned according to the
//! specified alignment value. It attempts to provide a compatible definition for various compilers.
//! \param name The name of the structure.
//! \param alignmentval The alignment value for the structure.
//! \param ... The structure members.
#if IS_GCC_VERSION(3, 0) || IS_CLANG_VERSION(1, 0)
#    define M_PACK_ALIGN_STRUCT(name, alignmentval, ...)                                                               \
        typedef struct s_##name                                                                                        \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        } __attribute__((packed, aligned(alignmentval))) name
#elif IS_MSVC_VERSION(MSVC_2005) && !defined(__clang__)
#    define M_PACK_ALIGN_STRUCT(name, alignmentval, ...)                                                               \
        __pragma(pack(push, alignmentval));                                                                            \
        typedef struct s_##name                                                                                        \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        } name;                                                                                                        \
        __pragma(pack(pop))
#else
#    define M_PACK_ALIGN_STRUCT(name, alignmentval, ...)                                                               \
        typedef struct s_##name                                                                                        \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        } name
#endif

//! \def M_PACKED_STRUCT
//! \brief Defines a packed structure.
//!
//! This macro defines a packed structure to ensure the structure is packed as small as possible.
//! It attempts to provide a compatible definition for various compilers.
//! \param name The name of the structure.
//! \param ... The structure members.
#if IS_GCC_VERSION(3, 0) || IS_CLANG_VERSION(1, 0)
#    define M_PACKED_STRUCT(name, ...)                                                                                 \
        typedef struct s_##name                                                                                        \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        } __attribute__((packed)) name
#elif IS_MSVC_VERSION(MSVC_2005) && !defined(__clang__)
#    define M_PACKED_STRUCT(name, ...)                                                                                 \
        __pragma(pack(push, 1));                                                                                       \
        typedef struct s_##name                                                                                        \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        } name;                                                                                                        \
        __pragma(pack(pop))
#else
#    define M_PACKED_STRUCT(name, ...)                                                                                 \
        typedef struct s_##name                                                                                        \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        } name
#endif

#if !defined(USING_CPP98)
//! \def DECLARE_ZERO_INIT_ARRAY
//! \brief Declares and zero-initializes an array.
//!
//! This macro declares and zero-initializes an array to ensure all elements are set to zero.
//! It attempts to provide a compatible definition for various compilers and standards.
//! \param type_name The type of the array elements.
//! \param array_name The name of the array.
//! \param size The size of the array.
#    if IS_GCC_VERSION(4, 0) || IS_CLANG_VERSION(1, 0)
#        define DECLARE_ZERO_INIT_ARRAY(type_name, array_name, size)                                                   \
            type_name array_name[size] = {[0 ...((size) - 1)] = 0}
#    elif USING_C23
#        define DECLARE_ZERO_INIT_ARRAY(type_name, array_name, size) type_name array_name[size] = {}
#    else
#        if defined(USING_C99)
    static M_INLINE void zero_init_array(void* array, size_t element_size, size_t element_count)
    {
#            if defined(USING_C23) || defined(HAVE_MEMSET_EXPLICIT)
        memset_explicit(array, 0, element_size * element_count);
#            elif defined(HAVE_C11_ANNEX_K) || defined(HAVE_MEMSET_S)
        memset_s(array, element_size * element_count, 0, element_size * element_count);
#            elif (defined(_WIN32) && defined(_MSC_VER)) || defined(HAVE_MSFT_SECURE_ZERO_MEMORY) ||                   \
                defined(HAVE_MSFT_SECURE_ZERO_MEMORY2)
#                if !defined(NO_HAVE_MSFT_SECURE_ZERO_MEMORY2) &&                                                      \
                    (defined(HAVE_MSFT_SECURE_ZERO_MEMORY2) ||                                                         \
                     (defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN11_26100))
        SecureZeroMemory2(array, element_size * element_count);
#                else
        SecureZeroMemory(array, element_size * element_count);
#                endif
#            else
        memset(array, 0, element_size * element_count);
#            endif
    }

#            define DECLARE_ZERO_INIT_ARRAY(type_name, array_name, size)                                               \
                type_name array_name[size];                                                                            \
                zero_init_array(array_name, sizeof(type_name), size)
#        else
#            define DECLARE_ZERO_INIT_ARRAY(type_name, array_name, size) type_name array_name[size] = {0}
#        endif
#    endif
#endif

//! \def M_STATIC_ASSERT
//! \brief Compile time assert macro working across standards and compilers
//!
//! This macro defines a way to do a static assert across various standards.
//! The message should NOT be a quoted string and have no spaces for backwards compatibility
//! \param condition Condition to check at compile time
//! \param message Message to display upon compiler error
//!
//! \code
//! M_STATIC_ASSET(condition, this_is_my_example_error)
//! \endcode
#if defined(USING_CPP11) && defined(__cpp_static_assert)
#    define M_STATIC_ASSERT(condition, message) static_assert(condition, #message)
#elif defined(USING_C23)
#    define M_STATIC_ASSERT(condition, message) static_assert(condition, #message)
#elif defined(USING_C11)
#    if IS_MSVC_VERSION(MSVC_2019_16_8) /* Need new enough Windows SDK for this to be available - TJE */
#        if defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_WIN10_20348
#            include <assert.h>
#            define M_STATIC_ASSERT(condition, message) _Static_assert(condition, #message)
#        endif
#    elif !defined(_MSC_VER)
#        include <assert.h>
#        define M_STATIC_ASSERT(condition, message) _Static_assert(condition, #message)
#    endif
#endif
#if !defined(M_STATIC_ASSERT)
// Generic way to do this. Not as good messaging but should work about the same
#    define M_STATIC_ASSERT(condition, message) typedef char static_assertion_##message[(condition) ? 1 : -1]
#endif

//! \def OPENSEA_PATH_MAX
//! \brief Defines the maximum path length for different systems.
//! \details
//! - For Windows systems, it uses MAX_PATH. More info:
//! https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=cmd
//! - For systems defining PATH_MAX, it uses PATH_MAX.
//! - For 4.4BSD and Solaris systems, it uses MAXPATHLEN (found in <sys/param.h>).
//! - If none of the above are defined, it defaults to 4096. If needed, this default case can be updated based on
//! system-specific definitions.
#if defined(MAX_PATH)
#    define OPENSEA_PATH_MAX MAX_PATH
#elif defined(PATH_MAX)
#    define OPENSEA_PATH_MAX PATH_MAX
#elif defined(MAXPATHLEN)
#    define OPENSEA_PATH_MAX MAXPATHLEN
#else
#    define OPENSEA_PATH_MAX (4096)
#endif

    //! \def SYSTEM_PATH_SEPARATOR
    //! \def SYSTEM_PATH_SEPARATOR_STR
    //! \brief Defines the path separator for different systems.
    //! \details
    //! - For Windows systems (excluding UEFI C source), it uses '\\' and "\\".
    //! - For other systems, it uses '/' and "/".
#if defined(_WIN32) && !defined(UEFI_C_SOURCE)
#    define SYSTEM_PATH_SEPARATOR     '\\'
#    define SYSTEM_PATH_SEPARATOR_STR "\\"
#else
#    define SYSTEM_PATH_SEPARATOR     '/'
#    define SYSTEM_PATH_SEPARATOR_STR "/"
#endif

//! \def ROOT_UID_VAL
//! \brief Defines the root user ID value.
#define ROOT_UID_VAL (0)

    //! \def NO_TYPEOF
    //! \brief When defined then the M_TYPEOF macro is not available

    //! \def M_TYPEOF
    //! \brief Defines a macro for type inference based on compiler support.
    //! \details
    //!   Only available when NO_TYPEOF is not defined.
    //! - For C23 standard, it uses `typeof(var)`.
    //! - For C++11 standard, it uses `decltype(var)`.
    //! - For GCC 2.95+, Clang 1.0+, and MSVC 2022 17.9+, it uses `__typeof__(var)`.
    //! - If none of the above are available, it defines `NO_TYPEOF`.

#if defined(USING_C23)
#    define M_TYPEOF(var) typeof(var)
#elif defined(USING_CPP11)
#    define M_TYPEOF(var) decltype(var)
#elif IS_GCC_VERSION(2, 95) || IS_CLANG_VERSION(1, 0) || IS_MSVC_VERSION(MSVC_2022_17_9)
#    define M_TYPEOF(var) __typeof__(var)
#else
#    define NO_TYPEOF
#endif

    //! \enum eReturnValues
    //! \brief Enum representing various return values.
    //!
    //! This enum defines various return values used throughout the application.
    M_DECLARE_ENUM(
        eReturnValues,
        /*!< Operation completed successfully. */
        SUCCESS = 0,
        /*!< Operation failed. */
        FAILURE = 1,
        /*!< Operation not supported. */
        NOT_SUPPORTED = 2,
        /*!< Command execution failed. */
        COMMAND_FAILURE = 3,
        /*!< Another command is in progress, or a command has started and is now in progress in the background. */
        IN_PROGRESS = 4,
        /*!< Command was aborted. */
        ABORTED = 5,
        /*!< Bad parameter provided. This should not appear in a finished application. */
        BAD_PARAMETER = 6,
        /*!< Could not allocate memory. */
        MEMORY_FAILURE = 7,
        /*!< OS API call to issue the pass-through command failed. */
        OS_PASSTHROUGH_FAILURE = 8,
        /*!< Library mismatch detected. */
        LIBRARY_MISMATCH = 9,
        /*!< Device is in a frozen state for a command like sanitize or ATA security. */
        FROZEN = 10,
        /*!< OS returned Access/permission denied. */
        PERMISSION_DENIED = 11,
        /*!< Error opening file. */
        FILE_OPEN_ERROR = 12,
        /*!< Command was issued, and some RTFRs were received, but we were unable to get a complete RTFR result. */
        WARN_INCOMPLETE_RFTRS = 13,
        /*!< Command took longer than the timeout provided to the OS. */
        OS_COMMAND_TIMEOUT = 14,
        /*!< Not all devices were enumerated. */
        WARN_NOT_ALL_DEVICES_ENUMERATED = 15,
        /*!< The checksum on the data for a command didn't calculate correctly. */
        WARN_INVALID_CHECKSUM = 16,
        /*!< OS does not have a way to issue the requested command. */
        OS_COMMAND_NOT_AVAILABLE = 17,
        /*!< OS is blocking the command from being issued. */
        OS_COMMAND_BLOCKED = 18,
        /*!< Command was interrupted by another SCT command. */
        COMMAND_INTERRUPTED = 19,
        /*!< Validation check failed. */
        VALIDATION_FAILURE = 20,
        /*!< Failure in stripping header/footer for UDS. */
        STRIP_HDR_FOOTER_FAILURE = 21,
        /*!< Parsing failure. */
        PARSE_FAILURE = 22,
        /*!< Binary file has an invalid length or the parameters for the length don't match the size of the file. */
        INVALID_LENGTH = 23,
        /*!< Error writing to file. */
        ERROR_WRITING_FILE = 24,
        /*!< Operation timeout for SeaQueue. */
        TIMEOUT = 25,
        /*!< Requested timeout is larger than the OS can support in passthrough. */
        OS_TIMEOUT_TOO_LARGE = 26,
        /*!< C/C++ exception failure while parsing. */
        PARSING_EXCEPTION_FAILURE = 27,
        /*!< Creation of output folder failed. */
        DIR_CREATION_FAILED = 28,
        /*!< Reading logfile to buffer failed. */
        FILE_READ_ERROR = 29,
        /*!< Power cycle required to complete the update. */
        POWER_CYCLE_REQUIRED = 30,
        /*!< Device access denied by the drive. */
        DEVICE_ACCESS_DENIED = 31,
        /*!< File was not parsed. */
        NOT_PARSED = 32,
        /*!< File is missing information to be parsed correctly. */
        MISSING_INFORMATION = 33,
        /*!< File is truncated and might be missing data for parsing it correctly. */
        TRUNCATED_FILE = 34,
        /*!< Path is insecure. */
        INSECURE_PATH = 35,
        /*!< Device handle reported busy and cannot be opened. */
        DEVICE_BUSY = 36,
        /*!< invalid device handle specified (not connected or present on the system) */
        DEVICE_INVALID = 37,
        /*!< Device disconnected since being opened and is no longer available. */
        DEVICE_DISCONNECTED = 38,
        /*!< Unknown error. */
        UNKNOWN);

    //! \enum eDataTransferDirection
    //! \brief Enum representing data transfer directions.
    //!
    //! This enum defines the possible directions for data transfer.
    M_DECLARE_ENUM(eDataTransferDirection,
                   /*!< No data transfer. */
                   XFER_NO_DATA,
                   /*!< Transfer from target to host. */
                   XFER_DATA_IN,
                   /*!< Transfer from host to target. */
                   XFER_DATA_OUT,
                   /*!< Transfer from host to target, followed by target to host. */
                   XFER_DATA_OUT_IN,
                   /*!< Transfer from target to host, followed by host to target. */
                   XFER_DATA_IN_OUT);

    //! \enum eVerbosityLevels
    //! \brief Enum representing verbosity levels.
    //!
    //! This enum defines the different levels of verbosity for logging or output.
    M_DECLARE_ENUM(
        eVerbosityLevels,
        /*!< Quiet verbosity level. */
        VERBOSITY_QUIET = 0,
        /*!< Default verbosity level. */
        VERBOSITY_DEFAULT = 1,
        /*!< Verbosity level for command names. */
        VERBOSITY_COMMAND_NAMES = 2,
        /*!< Verbosity level for detailed command output. */
        VERBOSITY_COMMAND_VERBOSE = 3,
        /*!< Verbosity level for buffer contents. */
        VERBOSITY_BUFFERS = 4,
        /*!< Maximum verbosity level. This should always be at the end of this list to allow for bounds checking. */
        VERBOSITY_MAX);

    //! \enum eDataFormat
    //! \brief Enum representing data formats.
    //!
    //! This enum defines the possible formats for data.
    M_DECLARE_ENUM(eDataFormat,
                   /*!< Default JSON data format. */
                   JSON_DATA = 0,
                   /*!< Pre-Python data format. */
                   PREPYTHON_DATA = 1);

    //! \struct tDataPtr
    //! \brief Structure representing a data buffer pointer and length block.
    //!
    //! This structure defines a data buffer with its pointer, length of valid data, and allocated length.
    typedef struct stDataPtr
    {
        void*  pData;    /*!< Pointer to the data buffer. If pData is M_NULLPTR, AllocLen must be zero also. */
        size_t DataLen;  /*!< Number of valid bytes in the data buffer. */
        size_t AllocLen; /*!< Allocated length of the data buffer. If AllocLen is zero, pData must be M_NULLPTR also. */
    } tDataPtr;

    //! \typedef custom_Update
    //! \brief Provides a custom callback for updating important messages to a UI.
    //!
    //! This typedef defines a function pointer for a custom callback that updates important messages to a UI.
    //! For instance, messages like "Performing 5000 Random Reads" or "Sequential Read Test". This allows the UI to get
    //! occasional refreshes.
    //!
    //! \param[in,out] customData Custom data used to help the Updater function. Usually hidden to the calling function.
    //! \param[in,out] message Message to go to the UI.
    typedef void (*custom_Update)(void* customData, char* message);

#if defined(__cplusplus)
}
#endif

// This has to live here to avoid errors about C linkage in older GCC in C++98 mode
#if defined(USING_CPP98) && !defined(M_NULLPTR)

//! \def M_NULLPTR
//! \brief Defines a macro for nullptr to handle different standards and environments.
//!
//! This macro defines M_NULLPTR to handle different standards and environments, ensuring compatibility when checking
//! for valid pointers. It provides a compatible definition for nullptr in C++98 and other environments where nullptr is
//! not available.
#    if defined(USING_CPP11)
#        define M_NULLPTR nullptr
#    else
DISABLE_WARNING_CPP11_COMPAT

//! \class nullptr_t
//! \brief A class representing a null pointer.
//!
//! This class provides a type-safe null pointer representation that can be used in place of nullptr in environments
//! where nullptr is not available. It supports conversion to any pointer type and member pointer type.
const class nullptr_t
{
  public:
    //! \brief Conversion operator to any pointer type.
    //! \tparam T The pointer type to convert to.
    //! \return A null pointer of type T*.
    template <class T> operator T*() const
    {
        return 0;
    }

    //! \brief Conversion operator to any member pointer type.
    //! \tparam C The class type.
    //! \tparam T The member pointer type.
    //! \return A null member pointer of type T C::*.
    template <class C, class T> operator T C::*() const
    {
        return 0;
    }

  private:
    //! \brief Deleted address-of operator to prevent taking the address of nullptr.
    void operator&() const;
} nullptr = {};

#        define M_NULLPTR nullptr
RESTORE_WARNING_CPP11_COMPAT
#    endif // C++11 check for nullptr
#endif     // C++98 and no M_NULLPTR

#if defined(USING_CPP98) && !defined(DECLARE_ZERO_INIT_ARRAY)

//! \brief Zero-initializes an array.
//!
//! This template function zero-initializes an array of any type and size.
//! It sets all elements of the array to zero.
//!
//! \tparam T The type of the array elements.
//! \tparam N The size of the array.
//! \param array The array to be zero-initialized.
template <typename T, size_t N> void zero_init_array(T (&array)[N])
{
    for (size_t i = SIZE_T_C(0); i < N; ++i)
    {
        array[i] = 0;
    }
}

//! \def DECLARE_ZERO_INIT_ARRAY
//! \brief Declares and zero-initializes an array.
//!
//! This macro declares and zero-initializes an array to ensure all elements are set to zero.
//! It attempts to provide a compatible definition for various compilers and standards.
//!
//! \param type_name The type of the array elements.
//! \param array_name The name of the array.
//! \param size The size of the array.
//! \note This does not use memset to handle non-trivial types.
//!       A compiler may optimize this to memset though if it detects that this
//!       is a zero initialization of the data.
#    define DECLARE_ZERO_INIT_ARRAY(type_name, array_name, size)                                                       \
        type_name array_name[size];                                                                                    \
        zero_init_array(array_name)
#endif // C++98 and no DECLARE_ZERO_INIT_ARRAY
