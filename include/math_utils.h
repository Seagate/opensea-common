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
// \file math_utils.h
// \brief Defines functions and macros for some basic math operations
//
#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "predef_env_detect.h"
#include "type_conversion.h"

#if defined(__cplusplus)
extern "C"
{
#endif

// clang-format off
#if defined(USING_C11) && defined (HAVE_C11_GENERIC_SELECTION)
// Using generic selection to change behavior on when to use
// a specific way to evaluate these macros
// This allows turnary for signed types to avoid undefined behavior
// and bitwise operations on unsigned types for fewer branch conditions.
#define M_Min(a, b) _Generic((a), \
    unsigned char: _Generic((b), \
        unsigned char: M_STATIC_CAST(unsigned char, M_STATIC_CAST(unsigned char, b) ^ ((M_STATIC_CAST(unsigned char, a) ^ M_STATIC_CAST(unsigned char, b)) & -M_STATIC_CAST(unsigned char, (a) < (b)))), \
        unsigned short: M_STATIC_CAST(unsigned short, M_STATIC_CAST(unsigned short, b) ^ ((M_STATIC_CAST(unsigned short, a) ^ M_STATIC_CAST(unsigned short, b)) & -M_STATIC_CAST(unsigned short, (a) < (b)))), \
        unsigned int: (M_STATIC_CAST(unsigned int, b) ^ ((M_STATIC_CAST(unsigned int, a) ^ M_STATIC_CAST(unsigned int, b)) & -M_STATIC_CAST(unsigned int, (a) < (b)))), \
        unsigned long: (M_STATIC_CAST(unsigned long, b) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned long long: (M_STATIC_CAST(unsigned long long, b) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        default: (((a) < (b)) ? (a) : (b))), \
    unsigned short: _Generic((b), \
        unsigned char: M_STATIC_CAST(unsigned short, M_STATIC_CAST(unsigned short, b) ^ ((M_STATIC_CAST(unsigned short, a) ^ M_STATIC_CAST(unsigned short, b)) & -M_STATIC_CAST(unsigned short, (a) < (b)))), \
        unsigned short: M_STATIC_CAST(unsigned short, M_STATIC_CAST(unsigned short, b) ^ ((M_STATIC_CAST(unsigned short, a) ^ M_STATIC_CAST(unsigned short, b)) & -M_STATIC_CAST(unsigned short, (a) < (b)))), \
        unsigned int: (M_STATIC_CAST(unsigned int, b) ^ ((M_STATIC_CAST(unsigned int, a) ^ M_STATIC_CAST(unsigned int, b)) & -M_STATIC_CAST(unsigned int, (a) < (b)))), \
        unsigned long: (M_STATIC_CAST(unsigned long, b) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned long long: (M_STATIC_CAST(unsigned long long, b) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        default: (((a) < (b)) ? (a) : (b))), \
    unsigned int: _Generic((b), \
        unsigned char: (M_STATIC_CAST(unsigned int, b) ^ ((M_STATIC_CAST(unsigned int, a) ^ M_STATIC_CAST(unsigned int, b)) & -M_STATIC_CAST(unsigned int, (a) < (b)))), \
        unsigned short: (M_STATIC_CAST(unsigned int, b) ^ ((M_STATIC_CAST(unsigned int, a) ^ M_STATIC_CAST(unsigned int, b)) & -M_STATIC_CAST(unsigned int, (a) < (b)))), \
        unsigned int: (M_STATIC_CAST(unsigned int, b) ^ ((M_STATIC_CAST(unsigned int, a) ^ M_STATIC_CAST(unsigned int, b)) & -M_STATIC_CAST(unsigned int, (a) < (b)))), \
        unsigned long: (M_STATIC_CAST(unsigned long, b) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned long long: (M_STATIC_CAST(unsigned long long, b) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        default: (((a) < (b)) ? (a) : (b))), \
    unsigned long: _Generic((b), \
        unsigned char: (M_STATIC_CAST(unsigned long, b) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned short: (M_STATIC_CAST(unsigned long, b) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned int: (M_STATIC_CAST(unsigned long, b) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned long: (M_STATIC_CAST(unsigned long, b) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned long long: (M_STATIC_CAST(unsigned long long, b) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        default: (((a) < (b)) ? (a) : (b))), \
    unsigned long long: _Generic((b), \
        unsigned char: (M_STATIC_CAST(unsigned long long, b) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        unsigned short: (M_STATIC_CAST(unsigned long long, b) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        unsigned int: (M_STATIC_CAST(unsigned long long, b) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        unsigned long: (M_STATIC_CAST(unsigned long long, b) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        unsigned long long: (M_STATIC_CAST(unsigned long long, b) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        default: (((a) < (b)) ? (a) : (b))), \
    default: (((a) < (b)) ? (a) : (b)))

#define M_Max(a, b) _Generic((a), \
    unsigned char: _Generic((b), \
        unsigned char: M_STATIC_CAST(unsigned char, M_STATIC_CAST(unsigned char, a) ^ ((M_STATIC_CAST(unsigned char, a) ^ M_STATIC_CAST(unsigned char, b)) & -M_STATIC_CAST(unsigned char, (a) < (b)))), \
        unsigned short: M_STATIC_CAST(unsigned short, M_STATIC_CAST(unsigned short, a) ^ ((M_STATIC_CAST(unsigned short, a) ^ M_STATIC_CAST(unsigned short, b)) & -M_STATIC_CAST(unsigned short, (a) < (b)))), \
        unsigned int: (M_STATIC_CAST(unsigned int, a) ^ ((M_STATIC_CAST(unsigned int, a) ^ M_STATIC_CAST(unsigned int, b)) & -M_STATIC_CAST(unsigned int, (a) < (b)))), \
        unsigned long: (M_STATIC_CAST(unsigned long, a) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned long long: (M_STATIC_CAST(unsigned long long, a) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        default: (((a) > (b)) ? (a) : (b))), \
    unsigned short: _Generic((b), \
        unsigned char: M_STATIC_CAST(unsigned short, M_STATIC_CAST(unsigned short, a) ^ ((M_STATIC_CAST(unsigned short, a) ^ M_STATIC_CAST(unsigned short, b)) & -M_STATIC_CAST(unsigned short, (a) < (b)))), \
        unsigned short: M_STATIC_CAST(unsigned short, M_STATIC_CAST(unsigned short, a) ^ ((M_STATIC_CAST(unsigned short, a) ^ M_STATIC_CAST(unsigned short, b)) & -M_STATIC_CAST(unsigned short, (a) < (b)))), \
        unsigned int: (M_STATIC_CAST(unsigned int, a) ^ ((M_STATIC_CAST(unsigned int, a) ^ M_STATIC_CAST(unsigned int, b)) & -M_STATIC_CAST(unsigned int, (a) < (b)))), \
        unsigned long: (M_STATIC_CAST(unsigned long, a) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned long long: (M_STATIC_CAST(unsigned long long, a) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        default: (((a) > (b)) ? (a) : (b))), \
    unsigned int: _Generic((b), \
        unsigned char: (M_STATIC_CAST(unsigned int, a) ^ ((M_STATIC_CAST(unsigned int, a) ^ M_STATIC_CAST(unsigned int, b)) & -M_STATIC_CAST(unsigned int, (a) < (b)))), \
        unsigned short: (M_STATIC_CAST(unsigned int, a) ^ ((M_STATIC_CAST(unsigned int, a) ^ M_STATIC_CAST(unsigned int, b)) & -M_STATIC_CAST(unsigned int, (a) < (b)))), \
        unsigned int: (M_STATIC_CAST(unsigned int, a) ^ ((M_STATIC_CAST(unsigned int, a) ^ M_STATIC_CAST(unsigned int, b)) & -M_STATIC_CAST(unsigned int, (a) < (b)))), \
        unsigned long: (M_STATIC_CAST(unsigned long, a) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned long long: (M_STATIC_CAST(unsigned long long, a) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        default: (((a) > (b)) ? (a) : (b))), \
    unsigned long: _Generic((b), \
        unsigned char: (M_STATIC_CAST(unsigned long, a) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned short: (M_STATIC_CAST(unsigned long, a) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned int: (M_STATIC_CAST(unsigned long, a) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned long: (M_STATIC_CAST(unsigned long, a) ^ ((M_STATIC_CAST(unsigned long, a) ^ M_STATIC_CAST(unsigned long, b)) & -M_STATIC_CAST(unsigned long, (a) < (b)))), \
        unsigned long long: (M_STATIC_CAST(unsigned long long, a) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        default: (((a) > (b)) ? (a) : (b))), \
    unsigned long long: _Generic((b), \
        unsigned char: (M_STATIC_CAST(unsigned long long, a) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        unsigned short: (M_STATIC_CAST(unsigned long long, a) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        unsigned int: (M_STATIC_CAST(unsigned long long, a) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        unsigned long: (M_STATIC_CAST(unsigned long long, a) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        unsigned long long: (M_STATIC_CAST(unsigned long long, a) ^ ((M_STATIC_CAST(unsigned long long, a) ^ M_STATIC_CAST(unsigned long long, b)) & -M_STATIC_CAST(unsigned long long, (a) < (b)))), \
        default: (((a) > (b)) ? (a) : (b))), \
    default: (((a) > (b)) ? (a) : (b)))

//NOTE: On the int rounding macros for unsigned char and unsigned short, the 1 is cast to those types to avoid
//      making the type wider and generating a truncation error at assignment.
//      Since there is not a standard way in C to note a value as a char or short like there is with long and long long
//      this cast is required.

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

#define INT_ROUND_DOWN(value, roundto)                                                                                 \
    _Generic((value),                                                                                                  \
     unsigned char: ((value) & ~((roundto) - M_STATIC_CAST(unsigned char, 1))),                                        \
     unsigned short: ((value) & ~((roundto) - M_STATIC_CAST(unsigned short, 1))),                                      \
     unsigned int: ((value) & ~((roundto) - 1U)),                                                                      \
     unsigned long: ((value) & ~((roundto) - 1UL)),                                                                    \
     unsigned long long: ((value) & ~((roundto) - 1ULL)),                                                              \
     default: (((value) % (roundto)) ? ((value) - ((value) % (roundto))) : (value)))

#else
// Max & Min Helpers (signed and unsigned)
#define  M_Min(a,b)    (((a)<(b))?(a):(b))
#define  M_Max(a,b)    (((a)>(b))?(a):(b))

//round an integer up to nearest multiple
//NOTE: It is recommended to use the type specific inline versions below for type enforcement by the compiler
//Example: Need to round up to nearest 512B for a data transfer
#define INT_ROUND_UP(value, roundto) (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value) )

//round an integer down to nearest multiple
#define INT_ROUND_DOWN(value, roundto) (((value) % (roundto)) ? ((value) - ((value) % (roundto))) : (value) )
#endif //C11 generic selection detection

    // clang-format on

#define M_2sCOMPLEMENT(val) (~(val) + 1)

    // Generic round up for any value of roundto
    static M_INLINE uint8_t uint8_round_up_generic(uint8_t value, uint8_t roundto)
    {
        return (((value) + (roundto)-UINT8_C(1)) & ~((roundto)-UINT8_C(1)));
    }

    static M_INLINE int8_t int8_round_up_generic(int8_t value, int8_t roundto)
    {
        return M_STATIC_CAST(int8_t, ((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value));
    }

    static M_INLINE uint16_t uint16_round_up_generic(uint16_t value, uint16_t roundto)
    {
        return (((value) + (roundto)-UINT16_C(1)) & ~((roundto)-UINT16_C(1)));
    }

    static M_INLINE int16_t int16_round_up_generic(int16_t value, int16_t roundto)
    {
        return M_STATIC_CAST(int16_t, ((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value));
    }

    static M_INLINE uint32_t uint32_round_up_generic(uint32_t value, uint32_t roundto)
    {
        return (((value) + (roundto)-UINT32_C(1)) & ~((roundto)-UINT32_C(1)));
    }

    static M_INLINE int32_t int32_round_up_generic(int32_t value, int32_t roundto)
    {
        return (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value));
    }

    static M_INLINE uint64_t uint64_round_up_generic(uint64_t value, uint64_t roundto)
    {
        return (((value) + (roundto)-UINT64_C(1)) & ~((roundto)-UINT64_C(1)));
    }

    static M_INLINE int64_t int64_round_up_generic(int64_t value, int64_t roundto)
    {
        return (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value));
    }

    // Generic round down for any value of roundto
    static M_INLINE uint8_t uint8_round_down_generic(uint8_t value, uint8_t roundto)
    {
        return ((value) & ~((roundto)-UINT8_C(1)));
    }

    static M_INLINE int8_t int8_round_down_generic(int8_t value, int8_t roundto)
    {
        return M_STATIC_CAST(int8_t, (value % roundto) ? ((value - (value % roundto))) : (value));
    }

    static M_INLINE uint16_t uint16_round_down_generic(uint16_t value, uint16_t roundto)
    {
        return ((value) & ~((roundto)-UINT16_C(1)));
    }

    static M_INLINE int16_t int16_round_down_generic(int16_t value, int16_t roundto)
    {
        return M_STATIC_CAST(int16_t, (value % roundto) ? ((value - (value % roundto))) : (value));
    }

    static M_INLINE uint32_t uint32_round_down_generic(uint32_t value, uint32_t roundto)
    {
        return ((value) & ~((roundto)-UINT32_C(1)));
    }

    static M_INLINE int32_t int32_round_down_generic(int32_t value, int32_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    static M_INLINE uint64_t uint64_round_down_generic(uint64_t value, uint64_t roundto)
    {
        return ((value) & ~((roundto)-UINT64_C(1)));
    }

    static M_INLINE int64_t int64_round_down_generic(int64_t value, int64_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    // Optimized round up for powers of two
    static M_INLINE uint8_t uint8_round_up_power2(uint8_t value, uint8_t roundto)
    {
        return (value + roundto - UINT8_C(1)) & ~(roundto - UINT8_C(1));
    }

    static M_INLINE int8_t int8_round_up_power2(int8_t value, int8_t roundto)
    {
        return M_STATIC_CAST(int8_t, (value + roundto - INT8_C(1)) & ~(roundto - INT8_C(1)));
    }

    static M_INLINE uint16_t uint16_round_up_power2(uint16_t value, uint16_t roundto)
    {
        return (value + roundto - UINT16_C(1)) & ~(roundto - UINT16_C(1));
    }

    static M_INLINE int16_t int16_round_up_power2(int16_t value, int16_t roundto)
    {
        return M_STATIC_CAST(int16_t, (value + roundto - INT16_C(1)) & ~(roundto - INT16_C(1)));
    }

    static M_INLINE uint32_t uint32_round_up_power2(uint32_t value, uint32_t roundto)
    {
        return (value + roundto - UINT32_C(1)) & ~(roundto - UINT32_C(1));
    }

    static M_INLINE int32_t int32_round_up_power2(int32_t value, int32_t roundto)
    {
        return (value + roundto - INT32_C(1)) & ~(roundto - INT32_C(1));
    }

    static M_INLINE uint64_t uint64_round_up_power2(uint64_t value, uint64_t roundto)
    {
        return (value + roundto - UINT64_C(1)) & ~(roundto - UINT64_C(1));
    }

    static M_INLINE int64_t int64_round_up_power2(int64_t value, int64_t roundto)
    {
        return (value + roundto - INT64_C(1)) & ~(roundto - INT64_C(1));
    }

    // Optimized round down for powers of two
    static M_INLINE uint8_t uint8_round_down_power2(uint8_t value, uint8_t roundto)
    {
        return value & ~(roundto - UINT8_C(1));
    }

    static M_INLINE int8_t int8_round_down_power2(int8_t value, int8_t roundto)
    {
        return M_STATIC_CAST(int8_t, value & ~(roundto - INT8_C(1)));
    }

    static M_INLINE uint16_t uint16_round_down_power2(uint16_t value, uint16_t roundto)
    {
        return value & ~(roundto - UINT16_C(1));
    }

    static M_INLINE int16_t int16_round_down_power2(int16_t value, int16_t roundto)
    {
        return M_STATIC_CAST(int16_t, value & ~(roundto - INT16_C(1)));
    }

    static M_INLINE uint32_t uint32_round_down_power2(uint32_t value, uint32_t roundto)
    {
        return value & ~(roundto - UINT32_C(1));
    }

    static M_INLINE int32_t int32_round_down_power2(int32_t value, int32_t roundto)
    {
        return value & ~(roundto - INT32_C(1));
    }

    static M_INLINE uint64_t uint64_round_down_power2(uint64_t value, uint64_t roundto)
    {
        return value & ~(roundto - UINT64_C(1));
    }

    static M_INLINE int64_t int64_round_down_power2(int64_t value, int64_t roundto)
    {
        return value & ~(roundto - INT64_C(1));
    }

    uint64_t power_Of_Two(uint16_t exponent);

    double raise_to_power(double number, double power);

#if defined(__cplusplus)
}
#endif
