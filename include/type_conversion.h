// SPDX-License-Identifier: MPL-2.0

//! \file type_conversion.h
//! \brief Defines cast macros and conversion functions for safely converting between types.
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common_types.h"
#include "predef_env_detect.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//! \def C_CAST
//! \brief Macro to perform a C style cast. Makes it easier to find and search for
//! casts when using this macro.
//! \param[in] type the type to cast to
//! \param[in] val the original data you want to convert with a cast
//! \note For C++ it recommended to use static_cast, reinterpret_cast, dynamic_cast
//! instead of this macro whenever possible.
#define C_CAST(type, val) ((type)(val))

//! \def M_STATIC_CAST
//! \brief Macro to perform a static cast. Makes it easier to find and search for
//! casts when using this macro.
//! \param[in] type the type to cast to
//! \param[in] val the original data you want to convert with a cast
//! \note For C++ it uses static_cast. In C it uses a C style cast
#if defined(USING_CPP98)
#    define M_STATIC_CAST(type, val) static_cast<type>(val)
#else // C
#    define M_STATIC_CAST(type, val) ((type)(val))
#endif

//! \def M_REINTERPRET_CAST
//! \brief Macro to perform a reinterpret cast. Makes it easier to find and search for
//! casts when using this macro.
//! \param[in] type the type to cast to
//! \param[in] val the original data you want to convert with a cast
//! \note For C++ it uses reinterpret_cast. In C it uses a C style cast
#if defined(USING_CPP98)
#    define M_REINTERPRET_CAST(type, ptr) reinterpret_cast<type>(ptr)
#else // C
#    define M_REINTERPRET_CAST(type, ptr) ((type)(ptr))
#endif

//! \def M_CONST_CAST
//! \brief Macro to perform a const cast.
//! This can also be used a a type qualifier cast, such as on types marked "volatile"
//! \param[in] type the type to cast to
//! \param[in] val the original data you want to convert with a cast
//! \note For C++ it uses const_cast. In C it uses a C style cast to uintptr_t then to the final type
#if defined(USING_CPP98)
#    define M_CONST_CAST(type, val) const_cast<type>(val)
#else // C
#    define M_CONST_CAST(type, val) ((type)((uintptr_t)(val)))
#endif

//! \def M_ToBool
//! \brief Takes the output of the provided expression and converts it to "true" or "false"
//! \param[in] expression an expression to use for evaluation of the condition.
//! Expression returns > 0 for true, otherwise false
//! \note This is useful to silence conversion warnings in C++ when assigning the output to a bool
#define M_ToBool(expression) ((expression) > 0 ? true : false)

    //! \fn size_t int8_to_sizet(int8_t val)
    //! \brief Converts an int8_t value to size_t.
    //! \param[in] val The int8_t value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t int8_to_sizet(int8_t val);

    //! \fn size_t uint8_to_sizet(uint8_t val)
    //! \brief Converts a uint8_t value to size_t.
    //! \param[in] val The uint8_t value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t uint8_to_sizet(uint8_t val);

    //! \fn size_t int16_to_sizet(int16_t val)
    //! \brief Converts an int16_t value to size_t.
    //! \param[in] val The int16_t value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t int16_to_sizet(int16_t val);

    //! \fn size_t uint16_to_sizet(uint16_t val)
    //! \brief Converts a uint16_t value to size_t.
    //! \param[in] val The uint16_t value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t uint16_to_sizet(uint16_t val);

    //! \fn size_t int32_to_sizet(int32_t val)
    //! \brief Converts an int32_t value to size_t.
    //! \param[in] val The int32_t value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t int32_to_sizet(int32_t val);

    //! \fn size_t uint32_to_sizet(uint32_t val)
    //! \brief Converts a uint32_t value to size_t.
    //! \param[in] val The uint32_t value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t uint32_to_sizet(uint32_t val);

    //! \fn size_t int64_to_sizet(int64_t val)
    //! \brief Converts an int64_t value to size_t.
    //! \param[in] val The int64_t value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t int64_to_sizet(int64_t val);

    //! \fn size_t uint64_to_sizet(uint64_t val)
    //! \brief Converts a uint64_t value to size_t.
    //! \param[in] val The uint64_t value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t uint64_to_sizet(uint64_t val);

    //! \fn size_t char_to_sizet(char val)
    //! \brief Converts a char value to size_t.
    //! \param[in] val The char value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t char_to_sizet(char val);

    //! \fn size_t schar_to_sizet(signed char val)
    //! \brief Converts a signed char value to size_t.
    //! \param[in] val The signed char value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t schar_to_sizet(signed char val);

    //! \fn size_t uchar_to_sizet(unsigned char val)
    //! \brief Converts an unsigned char value to size_t.
    //! \param[in] val The unsigned char value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t uchar_to_sizet(unsigned char val);

    //! \fn size_t short_to_sizet(short val)
    //! \brief Converts a short value to size_t.
    //! \param[in] val The short value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t short_to_sizet(short val);

    //! \fn size_t ushort_to_sizet(unsigned short val)
    //! \brief Converts an unsigned short value to size_t.
    //! \param[in] val The unsigned short value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t ushort_to_sizet(unsigned short val);

    //! \fn size_t int_to_sizet(int val)
    //! \brief Converts an int value to size_t.
    //! \param[in] val The int value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t int_to_sizet(int val);

    //! \fn size_t uint_to_sizet(unsigned int val)
    //! \brief Converts an unsigned int value to size_t.
    //! \param[in] val The unsigned int value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t uint_to_sizet(unsigned int val);

    //! \fn size_t long_to_sizet(long val)
    //! \brief Converts a long value to size_t.
    //! \param[in] val The long value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t long_to_sizet(long val);

    //! \fn size_t ulong_to_sizet(unsigned long val)
    //! \brief Converts an unsigned long value to size_t.
    //! \param[in] val The unsigned long value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t ulong_to_sizet(unsigned long val);

    //! \fn size_t longlong_to_sizet(long long val)
    //! \brief Converts a long long value to size_t.
    //! \param[in] val The long long value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t longlong_to_sizet(long long val);

    //! \fn size_t ulonglong_to_sizet(unsigned long long val)
    //! \brief Converts an unsigned long long value to size_t.
    //! \param[in] val The unsigned long long value to convert.
    //! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater
    //! than SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it
    //! simply returns the same value as a size_t.
    size_t ulonglong_to_sizet(unsigned long long val);

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
    // clang-format off
//! \def to_sizet
//! \brief C11 generic selection macro to call the appropriate x_to_sizet function based on the typeof \a X
//! \param[in] X the value to convert. It's type is used to call the correct function for the conversion.
//! \return The converted size_t value. If the value is less than zero, it returns zero. If the value is greater than 
//! SIZE_MAX, it sets SIZE_MAX. If less than zero or above SIZE_MAX, errno is also set to ERANGE. Otherwise, it simply
//! returns the same value as a size_t.
#define to_sizet(X)                                                                                                    \
    _Generic((X),                                                                                                      \
             signed char        : schar_to_sizet,                                                                      \
             unsigned char      : uchar_to_sizet,                                                                      \
             short              : short_to_sizet,                                                                      \
             unsigned short     : ushort_to_sizet,                                                                     \
             int                : int_to_sizet,                                                                        \
             unsigned int       : uint_to_sizet,                                                                       \
             long               : long_to_sizet,                                                                       \
             unsigned long      : ulong_to_sizet,                                                                      \
             long long          : longlong_to_sizet,                                                                   \
             unsigned long long : ulonglong_to_sizet                                                                   \
            )(X)
#endif // C11
    // clang-format on

    //! \fn bool is_size_t_max(size_t val)
    //! \brief Checks if a value is equivalent to \a SIZE_MAX. Can be useful if \a SIZE_MAX is not
    //! known on a given system or need to know if the X_to_sizet functions converted to \a SIZE_MAX.
    //! For example, uint64_t to size_t on a 32bit system may return SIZE_MAX and this can be used to check it.
    //! \param[in] val The value to check
    //! \return true means equivalent to \a SIZE_MAX, false means not \a SIZE_MAX
    bool is_size_t_max(size_t val);

    //! \fn double convert_128bit_to_double(const uint8_t* pData)
    //! \brief Converts a 16 byte array to a double
    //! Since uint128_t is not a standard available type, but double is. Double can be used to store a
    //! very large number. IEEE 754 floating point can store a number much larger than a 128 bit integer.
    //! \param[in] pData pointer to the 16 byte array to use for conversion to a double
    //! \return 128bit value as a double
    M_NONNULL_PARAM_LIST(1) M_PARAM_RO(1) double convert_128bit_to_double(const uint8_t* pData);

    //! \fn void get_Decimal_From_4_byte_Float(uint32_t floatValue, double* decimalValue)
    //! \brief This function converts 4 Byte Representation of a Floating Point Number to a double
    //! \param[in] floatValue 4 byte format value
    //! \param[out] decimalValue corresponding double format value
    M_NONNULL_PARAM_LIST(2) M_PARAM_WO(2) void get_Decimal_From_4_byte_Float(uint32_t floatValue, double* decimalValue);

#if defined(__cplusplus)
}
#endif
