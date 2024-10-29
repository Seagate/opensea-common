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
// \file type_conversion.h
// \brief Defines cast macros and conversion functions for more safely
// converting between types.
//
#pragma once

#include "common_types.h"
#include "predef_env_detect.h"

#if defined(__cplusplus)
extern "C"
{
#endif

// Macro to help make casts more clear and searchable. Can be very helpful while
// debugging. If using C++, use static_cast, reinterpret_cast, dynamic_cast
// before trying a C_CAST.
#define C_CAST(type, val) ((type)(val))

// Defining macros for static/reinterpret cast. These are mostly meant to be
// used in inline functions in this library to make it easy to import correct
// code into C and C++. They can be used in the library code or C++ code if you
// wish, but you are better off using static_cast or reinterpret_cast in C++
// over this macro. -TJE
#if defined(USING_CPP98)
#define M_STATIC_CAST(type, val) static_cast<type>(val)
#else // C
#define M_STATIC_CAST(type, val) C_CAST(type, val)
#endif

#if defined(USING_CPP98)
#define M_REINTERPRET_CAST(type, ptr) reinterpret_cast<type>(ptr)
#else // C
#define M_REINTERPRET_CAST(type, ptr) C_CAST(type, ptr)
#endif

#if defined(USING_CPP98)
#define M_CONST_CAST(type, val) const_cast<type>(val)
#else // C
#define M_CONST_CAST(type, val) ((type)((uintptr_t)(val)))
#endif

// Convert the result to a boolean true or false (ternary operator)
// This is especially good for C++ since you get conversion warnings and is a
// way to remove warnings in C++ when converting to a bool
#define M_ToBool(expression) ((expression) > 0 ? true : false)

    size_t int8_to_sizet(int8_t val);

    size_t uint8_to_sizet(uint8_t val);

    size_t int16_to_sizet(int16_t val);

    size_t uint16_to_sizet(uint16_t val);

    size_t int32_to_sizet(int32_t val);

    size_t uint32_to_sizet(uint32_t val);

    size_t int64_to_sizet(int64_t val);

    size_t uint64_to_sizet(uint64_t val);

    // There are technically 3 kinds of char, however char can be defined as signed
    // or unsigned. If you are not sure, call the char version. If you know you are
    // using signed or unsigned char, then use those versions.
    size_t char_to_sizet(char val);
    size_t schar_to_sizet(signed char val);
    size_t uchar_to_sizet(unsigned char val);

    size_t short_to_sizet(short val);
    size_t ushort_to_sizet(unsigned short val);

    size_t int_to_sizet(int val);
    size_t uint_to_sizet(unsigned int val);

    size_t long_to_sizet(long val);
    size_t ulong_to_sizet(unsigned long val);

    size_t longlong_to_sizet(long long val);
    size_t ulonglong_to_sizet(unsigned long long val);

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
// This is a generic selection macro.
// so based on the type of X, it will call the appropriate function for that
// type. similar to an overloaded function in C++ for different types. NOTE: Not
// using intX_t or uintX_t since these are type defs to one of the types in the
// macro below. NOTE: No default case so we can get a warning when this doesn't
// expand correctly.
#define to_sizet(X)                                                                                                    \
    _Generic((X), signed char                                                                                          \
             : schar_to_sizet, unsigned char                                                                           \
             : uchar_to_sizet, short                                                                                   \
             : short_to_sizet, unsigned short                                                                          \
             : ushort_to_sizet, int                                                                                    \
             : int_to_sizet, unsigned int                                                                              \
             : uint_to_sizet, long                                                                                     \
             : long_to_sizet, unsigned long                                                                            \
             : ulong_to_sizet, long long                                                                               \
             : longlong_to_sizet, unsigned long long                                                                   \
             : ulonglong_to_sizet)(X)
#endif // C11

    bool is_size_t_max(size_t val);

    double convert_128bit_to_double(uint8_t* pData);

    //-----------------------------------------------------------------------------
    //
    //  void get_Decimal_From_4_byte_Float(uint32_t floatValue, double
    //  *decimalValue))
    //
    //! \brief   Description:  This function converts 4 Byte Representation of a
    //! Floating Point Number to Decimal Format
    //
    //  Entry:
    //!   \param[in] floatValue = 4 byte format value
    //!   \param[out] decimalValue = corresponding decimal format value
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void get_Decimal_From_4_byte_Float(uint32_t floatValue, double* decimalValue);

#if defined(__cplusplus)
}
#endif
