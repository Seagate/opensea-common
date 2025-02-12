// SPDX-License-Identifier: MPL-2.0

//! \file math_utils.h
//! \brief Defines functions and macros for some basic math operations
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "predef_env_detect.h"
#include "type_conversion.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//! \def M_Min
//! \brief get the minimum of two values
//! \param[in] a first value for comparison
//! \param[in] b second value for comparison
//! \return returns the minimum of two values
#define M_Min(a, b) (((a) < (b)) ? (a) : (b))

//! \def M_Max
//! \brief get the maximum of two values
//! \param[in] a first value for comparison
//! \param[in] b second value for comparison
//! \return returns the maximum of two values
#define M_Max(a, b) (((a) > (b)) ? (a) : (b))

// clang-format off
#if defined(USING_C11) && defined (HAVE_C11_GENERIC_SELECTION)
// NOTE: On the int rounding macros for unsigned char and unsigned short, the 1 is cast to those types to avoid
//       making the type wider and generating a truncation error at assignment.
//       Since there is not a standard way in C to note a value as a char or short like there is with long and long long
//       this cast is required.

//! \def INT_ROUND_UP
//! \brief rounds a value up to a nearest multiple of another value.
//! For example 10 for nearest multiple of 10, 5 for nearest multiple of 5, etc
//! \param[in] value the integer value to round up
//! \param[in] roundto value specifying nearest multiple to round to
//! \return \a value rounded up to nearest multiple of \a roundto
#define INT_ROUND_UP(value, roundto)                                                                                   \
    _Generic((value),                                                                                                  \
     unsigned char:                                                                                                    \
            (((value) + (roundto) - M_STATIC_CAST(unsigned char, 1)) & ~((roundto) - M_STATIC_CAST(unsigned char, 1))),\
     unsigned short:                                                                                                   \
          (((value) + (roundto) - M_STATIC_CAST(unsigned short, 1)) & ~((roundto) - M_STATIC_CAST(unsigned short, 1))),\
     unsigned int: (((value) + (roundto) - 1U) & ~((roundto) - 1U)),                                                   \
     unsigned long: (((value) + (roundto) - 1UL) & ~((roundto) - 1UL)),                                                \
     unsigned long long: (((value) + (roundto) - 1ULL) & ~((roundto) - 1ULL)),                                         \
     default: (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value)))

//! \def INT_ROUND_DOWN
//! \brief rounds a value down to a nearest multiple of another value.
//! For example 10 for nearest multiple of 10, 5 for nearest multiple of 5, etc
//! \param[in] value the integer value to round down
//! \param[in] roundto value specifying nearest multiple to round to
//! \return \a value rounded down to nearest multiple of \a roundto
#define INT_ROUND_DOWN(value, roundto)                                                                                 \
    _Generic((value),                                                                                                  \
     unsigned char: ((value) & ~((roundto) - M_STATIC_CAST(unsigned char, 1))),                                        \
     unsigned short: ((value) & ~((roundto) - M_STATIC_CAST(unsigned short, 1))),                                      \
     unsigned int: ((value) & ~((roundto) - 1U)),                                                                      \
     unsigned long: ((value) & ~((roundto) - 1UL)),                                                                    \
     unsigned long long: ((value) & ~((roundto) - 1ULL)),                                                              \
     default: (((value) % (roundto)) ? ((value) - ((value) % (roundto))) : (value)))

#else

//! \def INT_ROUND_UP
//! \brief rounds a value up to a nearest multiple of another value.
//! For example 10 for nearest multiple of 10, 5 for nearest multiple of 5, etc
//! \param[in] value the integer value to round up
//! \param[in] roundto value specifying nearest multiple to round to
//! \return \a value rounded up to nearest multiple of \a roundto
#define INT_ROUND_UP(value, roundto) (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value) )

//! \def INT_ROUND_DOWN
//! \brief rounds a value down to a nearest multiple of another value.
//! For example 10 for nearest multiple of 10, 5 for nearest multiple of 5, etc
//! \param[in] value the integer value to round down
//! \param[in] roundto value specifying nearest multiple to round to
//! \return \a value rounded down to nearest multiple of \a roundto
#define INT_ROUND_DOWN(value, roundto) (((value) % (roundto)) ? ((value) - ((value) % (roundto))) : (value) )
#endif //C11 generic selection detection

    // clang-format on

//! \def M_2sCOMPLEMENT
//! \brief Performs 2's complement of the provided value.
//! 2's complement is the most common notation for representing signed integers
//! however this can be useful when doing a conversion on any representation of a
//! signed integer.
//! \param[in] value integer to perform two's complement on
//! \return 2's complement of \a value
#if defined(NO_TYPEOF)
#    define M_2sCOMPLEMENT(val) (~(val) + 1)
#else
#    define M_2sCOMPLEMENT(val) (~(val) + M_STATIC_CAST(M_TYPEOF(val), 1))
#endif

    //! \fn uint8_t uint8_round_up_generic(uint8_t value, uint8_t roundto)
    //! \brief Rounds a uint8_t value up to the nearest multiple of another value.
    //! \param[in] value The uint8_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE uint8_t uint8_round_up_generic(uint8_t value, uint8_t roundto)
    {
        return (((value) + (roundto)-UINT8_C(1)) & ~((roundto)-UINT8_C(1)));
    }

    //! \fn int8_t int8_round_up_generic(int8_t value, int8_t roundto)
    //! \brief Rounds an int8_t value up to the nearest multiple of another value.
    //! \param[in] value The int8_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE int8_t int8_round_up_generic(int8_t value, int8_t roundto)
    {
        return M_STATIC_CAST(int8_t, ((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value));
    }

    //! \fn uint16_t uint16_round_up_generic(uint16_t value, uint16_t roundto)
    //! \brief Rounds a uint16_t value up to the nearest multiple of another value.
    //! \param[in] value The uint16_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE uint16_t uint16_round_up_generic(uint16_t value, uint16_t roundto)
    {
        return (((value) + (roundto)-UINT16_C(1)) & ~((roundto)-UINT16_C(1)));
    }

    //! \fn int16_t int16_round_up_generic(int16_t value, int16_t roundto)
    //! \brief Rounds an int16_t value up to the nearest multiple of another value.
    //! \param[in] value The int16_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE int16_t int16_round_up_generic(int16_t value, int16_t roundto)
    {
        return M_STATIC_CAST(int16_t, ((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value));
    }

    //! \fn uint32_t uint32_round_up_generic(uint32_t value, uint32_t roundto)
    //! \brief Rounds a uint32_t value up to the nearest multiple of another value.
    //! \param[in] value The uint32_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE uint32_t uint32_round_up_generic(uint32_t value, uint32_t roundto)
    {
        return (((value) + (roundto)-UINT32_C(1)) & ~((roundto)-UINT32_C(1)));
    }

    //! \fn int32_t int32_round_up_generic(int32_t value, int32_t roundto)
    //! \brief Rounds an int32_t value up to the nearest multiple of another value.
    //! \param[in] value The int32_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE int32_t int32_round_up_generic(int32_t value, int32_t roundto)
    {
        return (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value));
    }

    //! \fn uint64_t uint64_round_up_generic(uint64_t value, uint64_t roundto)
    //! \brief Rounds a uint64_t value up to the nearest multiple of another value.
    //! \param[in] value The uint64_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE uint64_t uint64_round_up_generic(uint64_t value, uint64_t roundto)
    {
        return (((value) + (roundto)-UINT64_C(1)) & ~((roundto)-UINT64_C(1)));
    }

    //! \fn int64_t int64_round_up_generic(int64_t value, int64_t roundto)
    //! \brief Rounds an int64_t value up to the nearest multiple of another value.
    //! \param[in] value The int64_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE int64_t int64_round_up_generic(int64_t value, int64_t roundto)
    {
        return (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value));
    }

    //! \fn uint8_t uint8_round_down_generic(uint8_t value, uint8_t roundto)
    //! \brief Rounds a uint8_t value down to the nearest multiple of another value.
    //! \param[in] value The uint8_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE uint8_t uint8_round_down_generic(uint8_t value, uint8_t roundto)
    {
        return ((value) & ~((roundto)-UINT8_C(1)));
    }

    //! \fn int8_t int8_round_down_generic(int8_t value, int8_t roundto)
    //! \brief Rounds an int8_t value down to the nearest multiple of another value.
    //! \param[in] value The int8_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE int8_t int8_round_down_generic(int8_t value, int8_t roundto)
    {
        return M_STATIC_CAST(int8_t, (value % roundto) ? ((value - (value % roundto))) : (value));
    }

    //! \fn uint16_t uint16_round_down_generic(uint16_t value, uint16_t roundto)
    //! \brief Rounds a uint16_t value down to the nearest multiple of another value.
    //! \param[in] value The uint16_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE uint16_t uint16_round_down_generic(uint16_t value, uint16_t roundto)
    {
        return ((value) & ~((roundto)-UINT16_C(1)));
    }

    //! \fn int16_t int16_round_down_generic(int16_t value, int16_t roundto)
    //! \brief Rounds an int16_t value down to the nearest multiple of another value.
    //! \param[in] value The int16_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE int16_t int16_round_down_generic(int16_t value, int16_t roundto)
    {
        return M_STATIC_CAST(int16_t, (value % roundto) ? ((value - (value % roundto))) : (value));
    }

    //! \fn uint32_t uint32_round_down_generic(uint32_t value, uint32_t roundto)
    //! \brief Rounds a uint32_t value down to the nearest multiple of another value.
    //! \param[in] value The uint32_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE uint32_t uint32_round_down_generic(uint32_t value, uint32_t roundto)
    {
        return ((value) & ~((roundto)-UINT32_C(1)));
    }

    //! \fn int32_t int32_round_down_generic(int32_t value, int32_t roundto)
    //! \brief Rounds an int32_t value down to the nearest multiple of another value.
    //! \param[in] value The int32_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE int32_t int32_round_down_generic(int32_t value, int32_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    //! \fn uint64_t uint64_round_down_generic(uint64_t value, uint64_t roundto)
    //! \brief Rounds a uint64_t value down to the nearest multiple of another value.
    //! \param[in] value The uint64_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE uint64_t uint64_round_down_generic(uint64_t value, uint64_t roundto)
    {
        return ((value) & ~((roundto)-UINT64_C(1)));
    }

    //! \fn int64_t int64_round_down_generic(int64_t value, int64_t roundto)
    //! \brief Rounds an int64_t value down to the nearest multiple of another value.
    //! \param[in] value The int64_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE int64_t int64_round_down_generic(int64_t value, int64_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    //! \fn uint8_t uint8_round_up_power2(uint8_t value, uint8_t roundto)
    //! \brief Rounds a uint8_t value up to the nearest power of two multiple.
    //! \param[in] value The uint8_t value to round up.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE uint8_t uint8_round_up_power2(uint8_t value, uint8_t roundto)
    {
        return (value + roundto - UINT8_C(1)) & ~(roundto - UINT8_C(1));
    }

    //! \fn int8_t int8_round_up_power2(int8_t value, int8_t roundto)
    //! \brief Rounds an int8_t value up to the nearest power of two multiple.
    //! \param[in] value The int8_t value to round up.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE int8_t int8_round_up_power2(int8_t value, int8_t roundto)
    {
        return M_STATIC_CAST(int8_t, (value + roundto - INT8_C(1)) & ~(roundto - INT8_C(1)));
    }

    //! \fn uint16_t uint16_round_up_power2(uint16_t value, uint16_t roundto)
    //! \brief Rounds a uint16_t value up to the nearest power of two multiple.
    //! \param[in] value The uint16_t value to round up.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE uint16_t uint16_round_up_power2(uint16_t value, uint16_t roundto)
    {
        return (value + roundto - UINT16_C(1)) & ~(roundto - UINT16_C(1));
    }

    //! \fn int16_t int16_round_up_power2(int16_t value, int16_t roundto)
    //! \brief Rounds an int16_t value up to the nearest power of two multiple.
    //! \param[in] value The int16_t value to round up.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE int16_t int16_round_up_power2(int16_t value, int16_t roundto)
    {
        return M_STATIC_CAST(int16_t, (value + roundto - INT16_C(1)) & ~(roundto - INT16_C(1)));
    }

    //! \fn uint32_t uint32_round_up_power2(uint32_t value, uint32_t roundto)
    //! \brief Rounds a uint32_t value up to the nearest power of two multiple.
    //! \param[in] value The uint32_t value to round up.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE uint32_t uint32_round_up_power2(uint32_t value, uint32_t roundto)
    {
        return (value + roundto - UINT32_C(1)) & ~(roundto - UINT32_C(1));
    }

    //! \fn int32_t int32_round_up_power2(int32_t value, int32_t roundto)
    //! \brief Rounds an int32_t value up to the nearest power of two multiple.
    //! \param[in] value The int32_t value to round up.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE int32_t int32_round_up_power2(int32_t value, int32_t roundto)
    {
        return (value + roundto - INT32_C(1)) & ~(roundto - INT32_C(1));
    }

    //! \fn uint64_t uint64_round_up_power2(uint64_t value, uint64_t roundto)
    //! \brief Rounds a uint64_t value up to the nearest power of two multiple.
    //! \param[in] value The uint64_t value to round up.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE uint64_t uint64_round_up_power2(uint64_t value, uint64_t roundto)
    {
        return (value + roundto - UINT64_C(1)) & ~(roundto - UINT64_C(1));
    }

    //! \fn int64_t int64_round_up_power2(int64_t value, int64_t roundto)
    //! \brief Rounds an int64_t value up to the nearest power of two multiple.
    //! \param[in] value The int64_t value to round up.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE int64_t int64_round_up_power2(int64_t value, int64_t roundto)
    {
        return (value + roundto - INT64_C(1)) & ~(roundto - INT64_C(1));
    }

    //! \fn uint8_t uint8_round_down_power2(uint8_t value, uint8_t roundto)
    //! \brief Rounds a uint8_t value down to the nearest power of two multiple.
    //! \param[in] value The uint8_t value to round down.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE uint8_t uint8_round_down_power2(uint8_t value, uint8_t roundto)
    {
        return value & ~(roundto - UINT8_C(1));
    }

    //! \fn int8_t int8_round_down_power2(int8_t value, int8_t roundto)
    //! \brief Rounds an int8_t value down to the nearest power of two multiple.
    //! \param[in] value The int8_t value to round down.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE int8_t int8_round_down_power2(int8_t value, int8_t roundto)
    {
        return M_STATIC_CAST(int8_t, value & ~(roundto - INT8_C(1)));
    }

    //! \fn uint16_t uint16_round_down_power2(uint16_t value, uint16_t roundto)
    //! \brief Rounds a uint16_t value down to the nearest power of two multiple.
    //! \param[in] value The uint16_t value to round down.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE uint16_t uint16_round_down_power2(uint16_t value, uint16_t roundto)
    {
        return value & ~(roundto - UINT16_C(1));
    }

    //! \fn int16_t int16_round_down_power2(int16_t value, int16_t roundto)
    //! \brief Rounds an int16_t value down to the nearest power of two multiple.
    //! \param[in] value The int16_t value to round down.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE int16_t int16_round_down_power2(int16_t value, int16_t roundto)
    {
        return M_STATIC_CAST(int16_t, value & ~(roundto - INT16_C(1)));
    }

    //! \fn uint32_t uint32_round_down_power2(uint32_t value, uint32_t roundto)
    //! \brief Rounds a uint32_t value down to the nearest power of two multiple.
    //! \param[in] value The uint32_t value to round down.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE uint32_t uint32_round_down_power2(uint32_t value, uint32_t roundto)
    {
        return value & ~(roundto - UINT32_C(1));
    }

    //! \fn int32_t int32_round_down_power2(int32_t value, int32_t roundto)
    //! \brief Rounds an int32_t value down to the nearest power of two multiple.
    //! \param[in] value The int32_t value to round down.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE int32_t int32_round_down_power2(int32_t value, int32_t roundto)
    {
        return value & ~(roundto - INT32_C(1));
    }

    //! \fn uint64_t uint64_round_down_power2(uint64_t value, uint64_t roundto)
    //! \brief Rounds a uint64_t value down to the nearest power of two multiple.
    //! \param[in] value The uint64_t value to round down.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE uint64_t uint64_round_down_power2(uint64_t value, uint64_t roundto)
    {
        return value & ~(roundto - UINT64_C(1));
    }

    //! \fn int64_t int64_round_down_power2(int64_t value, int64_t roundto)
    //! \brief Rounds an int64_t value down to the nearest power of two multiple.
    //! \param[in] value The int64_t value to round down.
    //! \param[in] roundto The power of two value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE int64_t int64_round_down_power2(int64_t value, int64_t roundto)
    {
        return value & ~(roundto - INT64_C(1));
    }

    //! \fn uint64_t power_Of_Two(uint16_t exponent)
    //! \brief returns 2^exponent value as uint64_t
    //! \param[in] exponent exponent value to raise 2 by
    //! \return result of 2^exponent calculation
    uint64_t power_Of_Two(uint16_t exponent);

    //! \fn uint64_t log2_power2(uint64_t p2val)
    //! \brief Calculates log2 for a value that is a power of 2
    //! Using this on values that are NOT a power of two will return an incorrect result
    //! \param[in] p2val power of 2 value to perform log2 on.
    //! \return result of log2(p2val)
    static M_INLINE uint64_t log2_power2(uint64_t p2val)
    {
        // NOTE: Using a built in to count trailing zeroes would be even more efficient as a single instruction on some
        // CPUs
        uint64_t ret = UINT64_C(0);
        while (p2val > UINT64_C(1))
        {
            p2val >>= 1;
            ++ret;
        }
        return ret;
    }

    //! \fn double raise_to_power(double number, double power)
    //! \brief Raises a double value to the requested power
    //! \param[in] number value to raise to a power
    //! \param[in] power power to raise \a number to
    //! \return result of number^power
    double raise_to_power(double number, double power);

#if defined(__cplusplus)
}
#endif
