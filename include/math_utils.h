// SPDX-License-Identifier: MPL-2.0

//! \file math_utils.h
//! \brief Defines functions and macros for some basic math operations
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "bit_manip.h"
#include "code_attributes.h"
#include "common_types.h"
#include "predef_env_detect.h"
#include "type_conversion.h"

#include <assert.h>

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
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN((value) > (UINT8_MAX - (roundto - UINT8_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using uint8_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        uint8_t rem = value % roundto;
        return M_STATIC_CAST(uint8_t, rem ? (value + roundto - rem) : value);
    }

    //! \fn int8_t int8_round_up_generic(int8_t value, int8_t roundto)
    //! \brief Rounds an int8_t value up to the nearest multiple of another value.
    //! \param[in] value The int8_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE int8_t int8_round_up_generic(int8_t value, int8_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) > (INT8_MAX - (roundto - INT8_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using int8_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        int8_t div = value / roundto;
        int8_t rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value > 0)
        {
            return M_STATIC_CAST(int8_t, (div + 1) * roundto);
        }
        return M_STATIC_CAST(int8_t, div* roundto);
    }

    /*
     * Base-type native implementations so _Generic can match plain types
     * (signed/unsigned char, short, int, long, long long) regardless of
     * typedef underlying types. These duplicate the fixed-width logic but
     * operate on the native base types to avoid relying on typedef equivalence.
     */
    //! \fn signed char signed_char_round_up_generic(signed char value, signed char roundto)
    //! \brief Rounds a signed char value up to the nearest multiple of another value.
    //! \param[in] value The signed char value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto (ceiling semantics for signed values).
    static M_INLINE signed char signed_char_round_up_generic(signed char value, signed char roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) > (SCHAR_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using signed_char_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        signed char div = value / roundto;
        signed char rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value > 0)
        {
            return M_STATIC_CAST(signed char, (div + 1) * roundto);
        }
        return M_STATIC_CAST(signed char, div* roundto);
    }

    //! \fn signed char signed_char_round_down_generic(signed char value, signed char roundto)
    //! \brief Rounds a signed char value down to the nearest multiple of another value.
    //! \param[in] value The signed char value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto (floor semantics for signed values).
    static M_INLINE signed char signed_char_round_down_generic(signed char value, signed char roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using signed_char_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        signed char rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value < 0)
        {
            return M_STATIC_CAST(signed char, (value - (roundto - rem)));
        }
        return M_STATIC_CAST(signed char, value - rem);
    }

    //! \fn unsigned char unsigned_char_round_up_generic(unsigned char value, unsigned char roundto)
    //! \brief Rounds an unsigned char value up to the nearest multiple of another value.
    //! \param[in] value The unsigned char value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE unsigned char unsigned_char_round_up_generic(unsigned char value, unsigned char roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN((value) > (UCHAR_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using unsigned_char_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        unsigned char rem = value % roundto;
        return M_STATIC_CAST(unsigned char, rem ? (value + roundto - rem) : value);
    }

    //! \fn unsigned char unsigned_char_round_down_generic(unsigned char value, unsigned char roundto)
    //! \brief Rounds an unsigned char value down to the nearest multiple of another value.
    //! \param[in] value The unsigned char value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE unsigned char unsigned_char_round_down_generic(unsigned char value, unsigned char roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using unsigned_char_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        unsigned char rem = value % roundto;
        return M_STATIC_CAST(unsigned char, rem ? (value - rem) : value);
    }

    //! \fn short signed_short_round_up_generic(short value, short roundto)
    //! \brief Rounds a short value up to the nearest multiple of another value.
    //! \param[in] value The short value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto (ceiling semantics for signed values).
    static M_INLINE short signed_short_round_up_generic(short value, short roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) > (SHRT_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using signed_short_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        short div = value / roundto;
        short rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value > 0)
        {
            return M_STATIC_CAST(short, (div + 1) * roundto);
        }
        return M_STATIC_CAST(short, div* roundto);
    }

    //! \fn short signed_short_round_down_generic(short value, short roundto)
    //! \brief Rounds a short value down to the nearest multiple of another value.
    //! \param[in] value The short value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto (floor semantics for signed values).
    static M_INLINE short signed_short_round_down_generic(short value, short roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using signed_short_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        short rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value < 0)
        {
            return M_STATIC_CAST(short, (value - (roundto - rem)));
        }
        return M_STATIC_CAST(short, value - rem);
    }

    //! \fn unsigned short unsigned_short_round_up_generic(unsigned short value, unsigned short roundto)
    //! \brief Rounds an unsigned short value up to the nearest multiple of another value.
    //! \param[in] value The unsigned short value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE unsigned short unsigned_short_round_up_generic(unsigned short value, unsigned short roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN((value) > (USHRT_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using unsigned_short_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        unsigned short rem = value % roundto;
        return M_STATIC_CAST(unsigned short, rem ? (value + roundto - rem) : value);
    }

    //! \fn unsigned short unsigned_short_round_down_generic(unsigned short value, unsigned short roundto)
    //! \brief Rounds an unsigned short value down to the nearest multiple of another value.
    //! \param[in] value The unsigned short value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE unsigned short unsigned_short_round_down_generic(unsigned short value, unsigned short roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using unsigned_short_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        unsigned short rem = value % roundto;
        return M_STATIC_CAST(unsigned short, rem ? (value - rem) : value);
    }

    //! \fn int signed_int_round_up_generic(int value, int roundto)
    //! \brief Rounds an int value up to the nearest multiple of another value.
    //! \param[in] value The int value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto (ceiling semantics for signed values).
    static M_INLINE int signed_int_round_up_generic(int value, int roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) > (INT_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using signed_int_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        int div = value / roundto;
        int rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value > 0)
        {
            return (div + 1) * roundto;
        }
        return div * roundto;
    }

    //! \fn int signed_int_round_down_generic(int value, int roundto)
    //! \brief Rounds an int value down to the nearest multiple of another value.
    //! \param[in] value The int value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto (floor semantics for signed values).
    static M_INLINE int signed_int_round_down_generic(int value, int roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using signed_int_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        int rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value < 0)
        {
            return (value - (roundto - rem));
        }
        return value - rem;
    }

    //! \fn unsigned int unsigned_int_round_up_generic(unsigned int value, unsigned int roundto)
    //! \brief Rounds an unsigned int value up to the nearest multiple of another value.
    //! \param[in] value The unsigned int value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE unsigned int unsigned_int_round_up_generic(unsigned int value, unsigned int roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN((value) > (UINT_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using unsigned_int_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        unsigned int rem = value % roundto;
        return rem ? (value + roundto - rem) : value;
    }

    //! \fn unsigned int unsigned_int_round_down_generic(unsigned int value, unsigned int roundto)
    //! \brief Rounds an unsigned int value down to the nearest multiple of another value.
    //! \param[in] value The unsigned int value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE unsigned int unsigned_int_round_down_generic(unsigned int value, unsigned int roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using unsigned_int_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        unsigned int rem = value % roundto;
        return rem ? (value - rem) : value;
    }

    //! \fn long signed_long_round_up_generic(long value, long roundto)
    //! \brief Rounds a long value up to the nearest multiple of another value.
    //! \param[in] value The long value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto (ceiling semantics for signed values).
    static M_INLINE long signed_long_round_up_generic(long value, long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) > (LONG_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using signed_long_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        long div = value / roundto;
        long rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value > 0)
        {
            return (div + 1) * roundto;
        }
        return div * roundto;
    }

    //! \fn long signed_long_round_down_generic(long value, long roundto)
    //! \brief Rounds a long value down to the nearest multiple of another value.
    //! \param[in] value The long value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto (floor semantics for signed values).
    static M_INLINE long signed_long_round_down_generic(long value, long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using signed_long_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        long rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value < 0)
        {
            return (value - (roundto - rem));
        }
        return value - rem;
    }

    //! \fn unsigned long unsigned_long_round_up_generic(unsigned long value, unsigned long roundto)
    //! \brief Rounds an unsigned long value up to the nearest multiple of another value.
    //! \param[in] value The unsigned long value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE unsigned long unsigned_long_round_up_generic(unsigned long value, unsigned long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN((value) > (ULONG_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using unsigned_long_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        unsigned long rem = value % roundto;
        return rem ? (value + roundto - rem) : value;
    }

    //! \fn unsigned long unsigned_long_round_down_generic(unsigned long value, unsigned long roundto)
    //! \brief Rounds an unsigned long value down to the nearest multiple of another value.
    //! \param[in] value The unsigned long value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE unsigned long unsigned_long_round_down_generic(unsigned long value, unsigned long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using unsigned_long_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        unsigned long rem = value % roundto;
        return rem ? (value - rem) : value;
    }

    //! \fn long long signed_long_long_round_up_generic(long long value, long long roundto)
    //! \brief Rounds a long long value up to the nearest multiple of another value.
    //! \param[in] value The long long value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto (ceiling semantics for signed values).
    static M_INLINE long long signed_long_long_round_up_generic(long long value, long long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) > (LLONG_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using signed_long_long_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        long long div = value / roundto;
        long long rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value > 0)
        {
            return (div + 1) * roundto;
        }
        return div * roundto;
    }

    //! \fn long long signed_long_long_round_down_generic(long long value, long long roundto)
    //! \brief Rounds a long long value down to the nearest multiple of another value.
    //! \param[in] value The long long value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto (floor semantics for signed values).
    static M_INLINE long long signed_long_long_round_down_generic(long long value, long long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using signed_long_long_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        long long rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value < 0)
        {
            return (value - (roundto - rem));
        }
        return value - rem;
    }

    //! \fn unsigned long long unsigned_long_long_round_up_generic(unsigned long long value, unsigned long long roundto)
    //! \brief Rounds an unsigned long long value up to the nearest multiple of another value.
    //! \param[in] value The unsigned long long value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE unsigned long long unsigned_long_long_round_up_generic(unsigned long long value,
                                                                           unsigned long long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN((value) > (ULLONG_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using unsigned_long_long_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        unsigned long long rem = value % roundto;
        return rem ? (value + roundto - rem) : value;
    }

    //! \fn unsigned long long unsigned_long_long_round_down_generic(unsigned long long value, unsigned long long
    //! roundto)
    //! \brief Rounds an unsigned long long value down to the nearest multiple of another value.
    //! \param[in] value The unsigned long long value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE unsigned long long unsigned_long_long_round_down_generic(unsigned long long value,
                                                                             unsigned long long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using unsigned_long_long_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        unsigned long long rem = value % roundto;
        return rem ? (value - rem) : value;
    }

    //! \fn uint16_t uint16_round_up_generic(uint16_t value, uint16_t roundto)
    //! \brief Rounds a uint16_t value up to the nearest multiple of another value.
    //! \param[in] value The uint16_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE uint16_t uint16_round_up_generic(uint16_t value, uint16_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN((value) > (UINT16_MAX - (roundto - UINT16_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using uint16_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        uint16_t rem = value % roundto;
        return M_STATIC_CAST(uint16_t, rem ? (value + roundto - rem) : value);
    }

    //! \fn int16_t int16_round_up_generic(int16_t value, int16_t roundto)
    //! \brief Rounds an int16_t value up to the nearest multiple of another value.
    //! \param[in] value The int16_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE int16_t int16_round_up_generic(int16_t value, int16_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) > (INT16_MAX - (roundto - INT16_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using int16_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        int16_t div = value / roundto;
        int16_t rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value > 0)
        {
            return M_STATIC_CAST(int16_t, (div + 1) * roundto);
        }
        return M_STATIC_CAST(int16_t, div* roundto);
    }

    //! \fn uint32_t uint32_round_up_generic(uint32_t value, uint32_t roundto)
    //! \brief Rounds a uint32_t value up to the nearest multiple of another value.
    //! \param[in] value The uint32_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE uint32_t uint32_round_up_generic(uint32_t value, uint32_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN((value) > (UINT32_MAX - (roundto - UINT32_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using uint32_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        uint32_t rem = value % roundto;
        return rem ? (value + roundto - rem) : value;
    }

    //! \fn int32_t int32_round_up_generic(int32_t value, int32_t roundto)
    //! \brief Rounds an int32_t value up to the nearest multiple of another value.
    //! \param[in] value The int32_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE int32_t int32_round_up_generic(int32_t value, int32_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) > (INT32_MAX - (roundto - INT32_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using int32_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        int32_t div = value / roundto;
        int32_t rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value > 0)
        {
            return (div + 1) * roundto;
        }
        return div * roundto;
    }

    //! \fn uint64_t uint64_round_up_generic(uint64_t value, uint64_t roundto)
    //! \brief Rounds a uint64_t value up to the nearest multiple of another value.
    //! \param[in] value The uint64_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE uint64_t uint64_round_up_generic(uint64_t value, uint64_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN((value) > (UINT64_MAX - (roundto - UINT64_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using uint64_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        uint64_t rem = value % roundto;
        return rem ? (value + roundto - rem) : value;
    }

    //! \fn int64_t int64_round_up_generic(int64_t value, int64_t roundto)
    //! \brief Rounds an int64_t value up to the nearest multiple of another value.
    //! \param[in] value The int64_t value to round up.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest multiple of roundto.
    static M_INLINE int64_t int64_round_up_generic(int64_t value, int64_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) > (INT64_MAX - (roundto - INT64_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using int64_round_up_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        int64_t div = value / roundto;
        int64_t rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value > 0)
        {
            return (div + 1) * roundto;
        }
        return div * roundto;
    }

    //! \fn uint8_t uint8_round_down_generic(uint8_t value, uint8_t roundto)
    //! \brief Rounds a uint8_t value down to the nearest multiple of another value.
    //! \param[in] value The uint8_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE uint8_t uint8_round_down_generic(uint8_t value, uint8_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using uint8_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        uint8_t rem = value % roundto;
        return M_STATIC_CAST(uint8_t, rem ? (value - rem) : value);
    }

    //! \fn int8_t int8_round_down_generic(int8_t value, int8_t roundto)
    //! \brief Rounds an int8_t value down to the nearest multiple of another value.
    //! \param[in] value The int8_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE int8_t int8_round_down_generic(int8_t value, int8_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) < (INT8_MIN + (roundto - INT8_C(1))), "possible underflow: cast to larger type to avoid underflow when rounding down")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using int8_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        int8_t div = value / roundto;
        int8_t rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value < 0)
        {
            return M_STATIC_CAST(int8_t, (div - 1) * roundto);
        }
        return M_STATIC_CAST(int8_t, div* roundto);
    }

    //! \fn uint16_t uint16_round_down_generic(uint16_t value, uint16_t roundto)
    //! \brief Rounds a uint16_t value down to the nearest multiple of another value.
    //! \param[in] value The uint16_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE uint16_t uint16_round_down_generic(uint16_t value, uint16_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using uint16_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        uint16_t rem = value % roundto;
        return M_STATIC_CAST(uint16_t, rem ? (value - rem) : value);
    }

    //! \fn int16_t int16_round_down_generic(int16_t value, int16_t roundto)
    //! \brief Rounds an int16_t value down to the nearest multiple of another value.
    //! \param[in] value The int16_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE int16_t int16_round_down_generic(int16_t value, int16_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) < (INT16_MIN + (roundto - INT16_C(1))), "possible underflow: cast to larger type to avoid underflow when rounding down")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using int16_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        int16_t div = value / roundto;
        int16_t rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value < 0)
        {
            return M_STATIC_CAST(int16_t, (div - 1) * roundto);
        }
        return M_STATIC_CAST(int16_t, div* roundto);
    }

    //! \fn uint32_t uint32_round_down_generic(uint32_t value, uint32_t roundto)
    //! \brief Rounds a uint32_t value down to the nearest multiple of another value.
    //! \param[in] value The uint32_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE uint32_t uint32_round_down_generic(uint32_t value, uint32_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using uint32_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        uint32_t rem = value % roundto;
        return rem ? (value - rem) : value;
    }

    //! \fn int32_t int32_round_down_generic(int32_t value, int32_t roundto)
    //! \brief Rounds an int32_t value down to the nearest multiple of another value.
    //! \param[in] value The int32_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE int32_t int32_round_down_generic(int32_t value, int32_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) < (INT32_MIN + (roundto - INT32_C(1))), "possible underflow: cast to larger type to avoid underflow when rounding down")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using int32_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        int32_t div = value / roundto;
        int32_t rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value < 0)
        {
            return (div - 1) * roundto;
        }
        return div * roundto;
    }

    //! \fn uint64_t uint64_round_down_generic(uint64_t value, uint64_t roundto)
    //! \brief Rounds a uint64_t value down to the nearest multiple of another value.
    //! \param[in] value The uint64_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE uint64_t uint64_round_down_generic(uint64_t value, uint64_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_WARN(((roundto) & ((roundto) - 1)) == 0, "roundto is a power of two; consider using uint64_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto == 0)
        {
            assert(roundto != 0);
            return value;
        }
        uint64_t rem = value % roundto;
        return rem ? (value - rem) : value;
    }

    //! \fn int64_t int64_round_down_generic(int64_t value, int64_t roundto)
    //! \brief Rounds an int64_t value down to the nearest multiple of another value.
    //! \param[in] value The int64_t value to round down.
    //! \param[in] roundto The value specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest multiple of roundto.
    static M_INLINE int64_t int64_round_down_generic(int64_t value, int64_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_WARN((value) < (INT64_MIN + (roundto - INT64_C(1))), "possible underflow: cast to larger type to avoid underflow when rounding down")
    M_DIAG_WARN(((roundto) > 0) && (((roundto) & ((roundto) - 1)) == 0), "roundto is a power of two; consider using int64_round_down_power2 for better performance")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        if (roundto <= 0)
        {
            assert(roundto > 0);
            return value;
        }
        int64_t div = value / roundto;
        int64_t rem = value % roundto;
        if (rem == 0)
        {
            return value;
        }
        if (value < 0)
        {
            return (div - 1) * roundto;
        }
        return div * roundto;
    }

    //! \fn uint8_t uint8_round_up_power2(uint8_t value, uint8_t roundto)
    //! \brief Rounds a uint8_t value up to the nearest power of two multiple.
    //! \param[in] value The uint8_t value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE uint8_t uint8_round_up_power2(uint8_t value, uint8_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use uint8_round_up_generic")
    M_DIAG_WARN((value) > (UINT8_MAX - (roundto - UINT8_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    // clang-format on
    {
        assert(roundto != 0);
        return (value + roundto - UINT8_C(1)) & ~(roundto - UINT8_C(1));
    }

    //! \fn int8_t int8_round_up_power2(int8_t value, int8_t roundto)
    //! \brief Rounds an int8_t value up to the nearest power of two multiple.
    //! \param[in] value The int8_t value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    static M_INLINE int8_t int8_round_up_power2(int8_t value, int8_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use int8_round_up_generic")
    M_DIAG_WARN((value) > (INT8_MAX - (roundto - INT8_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        uint8_t u   = M_STATIC_CAST(uint8_t, value);
        uint8_t r   = M_STATIC_CAST(uint8_t, roundto);
        uint8_t res = M_STATIC_CAST(uint8_t, (u + r - UINT8_C(1)) & ~(r - UINT8_C(1)));
        return M_STATIC_CAST(int8_t, res);
    }

    //! \fn uint16_t uint16_round_up_power2(uint16_t value, uint16_t roundto)
    //! \brief Rounds a uint16_t value up to the nearest power of two multiple.
    //! \param[in] value The uint16_t value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE uint16_t uint16_round_up_power2(uint16_t value, uint16_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use uint16_round_up_generic")
    M_DIAG_WARN((value) > (UINT16_MAX - (roundto - UINT16_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    // clang-format on
    {
        assert(roundto != 0);
        return (value + roundto - UINT16_C(1)) & ~(roundto - UINT16_C(1));
    }

    //! \fn int16_t int16_round_up_power2(int16_t value, int16_t roundto)
    //! \brief Rounds an int16_t value up to the nearest power of two multiple.
    //! \param[in] value The int16_t value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE int16_t int16_round_up_power2(int16_t value, int16_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use int16_round_up_generic")
    M_DIAG_WARN((value) > (INT16_MAX - (roundto - INT16_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        uint16_t u   = M_STATIC_CAST(uint16_t, value);
        uint16_t r   = M_STATIC_CAST(uint16_t, roundto);
        uint16_t res = M_STATIC_CAST(uint16_t, (u + r - UINT16_C(1)) & ~(r - UINT16_C(1)));
        return M_STATIC_CAST(int16_t, res);
    }

    //! \fn uint32_t uint32_round_up_power2(uint32_t value, uint32_t roundto)
    //! \brief Rounds a uint32_t value up to the nearest power of two multiple.
    //! \param[in] value The uint32_t value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE uint32_t uint32_round_up_power2(uint32_t value, uint32_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use uint32_round_up_generic")
    M_DIAG_WARN((value) > (UINT32_MAX - (roundto - UINT32_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    // clang-format on
    {
        assert(roundto != 0);
        return (value + roundto - UINT32_C(1)) & ~(roundto - UINT32_C(1));
    }

    //! \fn int32_t int32_round_up_power2(int32_t value, int32_t roundto)
    //! \brief Rounds an int32_t value up to the nearest power of two multiple.
    //! \param[in] value The int32_t value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE int32_t int32_round_up_power2(int32_t value, int32_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use int32_round_up_generic")
    M_DIAG_WARN((value) > (INT32_MAX - (roundto - INT32_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        uint32_t u   = M_STATIC_CAST(uint32_t, value);
        uint32_t r   = M_STATIC_CAST(uint32_t, roundto);
        uint32_t res = M_STATIC_CAST(uint32_t, (u + r - UINT32_C(1)) & ~(r - UINT32_C(1)));
        return M_STATIC_CAST(int32_t, res);
    }

    //! \fn uint64_t uint64_round_up_power2(uint64_t value, uint64_t roundto)
    //! \brief Rounds a uint64_t value up to the nearest power of two multiple.
    //! \param[in] value The uint64_t value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE uint64_t uint64_round_up_power2(uint64_t value, uint64_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use uint64_round_up_generic")
    M_DIAG_WARN((value) > (UINT64_MAX - (roundto - UINT64_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    // clang-format on
    {
        assert(roundto != 0);
        return (value + roundto - UINT64_C(1)) & ~(roundto - UINT64_C(1));
    }

    //! \fn int64_t int64_round_up_power2(int64_t value, int64_t roundto)
    //! \brief Rounds an int64_t value up to the nearest power of two multiple.
    //! \param[in] value The int64_t value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded up to the nearest power of two multiple of roundto.
    static M_INLINE int64_t int64_round_up_power2(int64_t value, int64_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use int64_round_up_generic")
    M_DIAG_WARN((value) > (INT64_MAX - (roundto - INT64_C(1))), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        uint64_t u   = M_STATIC_CAST(uint64_t, value);
        uint64_t r   = M_STATIC_CAST(uint64_t, roundto);
        uint64_t res = M_STATIC_CAST(uint64_t, (u + r - UINT64_C(1)) & ~(r - UINT64_C(1)));
        return M_STATIC_CAST(int64_t, res);
    }

    //! \fn uint8_t uint8_round_down_power2(uint8_t value, uint8_t roundto)
    //! \brief Rounds a uint8_t value down to the nearest power of two multiple.
    //! \param[in] value The uint8_t value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE uint8_t uint8_round_down_power2(uint8_t value, uint8_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use uint8_round_down_generic")
    // clang-format on
    {
        assert(roundto != 0);
        return value & ~(roundto - UINT8_C(1));
    }

    //! \fn int8_t int8_round_down_power2(int8_t value, int8_t roundto)
    //! \brief Rounds an int8_t value down to the nearest power of two multiple.
    //! \param[in] value The int8_t value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE int8_t int8_round_down_power2(int8_t value, int8_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use int8_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        uint8_t u   = M_STATIC_CAST(uint8_t, value);
        uint8_t r   = M_STATIC_CAST(uint8_t, roundto);
        uint8_t res = M_STATIC_CAST(uint8_t, (u & ~(r - UINT8_C(1))));
        return M_STATIC_CAST(int8_t, res);
    }

    //! \fn uint16_t uint16_round_down_power2(uint16_t value, uint16_t roundto)
    //! \brief Rounds a uint16_t value down to the nearest power of two multiple.
    //! \param[in] value The uint16_t value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE uint16_t uint16_round_down_power2(uint16_t value, uint16_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use uint16_round_down_generic")
    // clang-format on
    {
        assert(roundto != 0);
        return value & ~(roundto - UINT16_C(1));
    }

    //! \fn int16_t int16_round_down_power2(int16_t value, int16_t roundto)
    //! \brief Rounds an int16_t value down to the nearest power of two multiple.
    //! \param[in] value The int16_t value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE int16_t int16_round_down_power2(int16_t value, int16_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use int16_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        uint16_t u   = M_STATIC_CAST(uint16_t, value);
        uint16_t r   = M_STATIC_CAST(uint16_t, roundto);
        uint16_t res = M_STATIC_CAST(uint16_t, (u & ~(r - UINT16_C(1))));
        return M_STATIC_CAST(int16_t, res);
    }

    //! \fn uint32_t uint32_round_down_power2(uint32_t value, uint32_t roundto)
    //! \brief Rounds a uint32_t value down to the nearest power of two multiple.
    //! \param[in] value The uint32_t value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE uint32_t uint32_round_down_power2(uint32_t value, uint32_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use uint32_round_down_generic")
    // clang-format on
    {
        assert(roundto != 0);
        return value & ~(roundto - UINT32_C(1));
    }

    //! \fn int32_t int32_round_down_power2(int32_t value, int32_t roundto)
    //! \brief Rounds an int32_t value down to the nearest power of two multiple.
    //! \param[in] value The int32_t value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE int32_t int32_round_down_power2(int32_t value, int32_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use int32_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        uint32_t u   = M_STATIC_CAST(uint32_t, value);
        uint32_t r   = M_STATIC_CAST(uint32_t, roundto);
        uint32_t res = M_STATIC_CAST(uint32_t, (u & ~(r - UINT32_C(1))));
        return M_STATIC_CAST(int32_t, res);
    }

    //! \fn uint64_t uint64_round_down_power2(uint64_t value, uint64_t roundto)
    //! \brief Rounds a uint64_t value down to the nearest power of two multiple.
    //! \param[in] value The uint64_t value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE uint64_t uint64_round_down_power2(uint64_t value, uint64_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use uint64_round_down_generic")
    // clang-format on
    {
        assert(roundto != 0);
        return value & ~(roundto - UINT64_C(1));
    }

    //! \fn int64_t int64_round_down_power2(int64_t value, int64_t roundto)
    //! \brief Rounds an int64_t value down to the nearest power of two multiple.
    //! \param[in] value The int64_t value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note For signed variants this uses unsigned arithmetic internally to ensure defined
    //! bitwise behavior; results are cast back to the signed type.
    //! \return The value rounded down to the nearest power of two multiple of roundto.
    static M_INLINE int64_t int64_round_down_power2(int64_t value, int64_t roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR((roundto & (roundto - 1)) != 0, "roundto must be a power of two. Otherwise use int64_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        uint64_t u   = M_STATIC_CAST(uint64_t, value);
        uint64_t r   = M_STATIC_CAST(uint64_t, roundto);
        uint64_t res = M_STATIC_CAST(uint64_t, (u & ~(r - UINT64_C(1))));
        return M_STATIC_CAST(int64_t, res);
    }

    /*
     * Base-type power-of-two optimized helpers (native implementations).
     * Signed variants use the corresponding unsigned base type for defined
     * bitwise behavior and cast back before returning.
     */
    //! \fn signed char signed_char_round_up_power2(signed char value, signed char roundto)
    //! \brief Rounds a signed char value up to the nearest power-of-two multiple.
    //! \param[in] value The signed char value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note Uses unsigned arithmetic internally for defined bitwise behavior; results are cast back to the signed
    //! type.
    //! \return The value rounded up to the nearest power-of-two multiple of \a roundto.
    static M_INLINE signed char signed_char_round_up_power2(signed char value, signed char roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use signed_char_round_up_generic")
    M_DIAG_WARN((value) > (SCHAR_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        unsigned char u   = M_STATIC_CAST(unsigned char, value);
        unsigned char r   = M_STATIC_CAST(unsigned char, roundto);
        unsigned char res = M_STATIC_CAST(unsigned char, (u + r - 1) & ~(r - 1));
        return M_STATIC_CAST(signed char, res);
    }

    //! \fn signed char signed_char_round_down_power2(signed char value, signed char roundto)
    //! \brief Rounds a signed char value down to the nearest power-of-two multiple.
    //! \param[in] value The signed char value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note Uses unsigned arithmetic internally for defined bitwise behavior; results are cast back to the signed
    //! type.
    //! \return The value rounded down to the nearest power-of-two multiple of \a roundto.
    static M_INLINE signed char signed_char_round_down_power2(signed char value, signed char roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use signed_char_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        unsigned char u   = M_STATIC_CAST(unsigned char, value);
        unsigned char r   = M_STATIC_CAST(unsigned char, roundto);
        unsigned char res = M_STATIC_CAST(unsigned char, (u & ~(r - 1)));
        return M_STATIC_CAST(signed char, res);
    }

    //! \fn unsigned char unsigned_char_round_up_power2(unsigned char value, unsigned char roundto)
    //! \brief Rounds an unsigned char value up to the nearest power-of-two multiple.
    //! \param[in] value The unsigned char value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power-of-two multiple of \a roundto.
    static M_INLINE unsigned char unsigned_char_round_up_power2(unsigned char value, unsigned char roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use unsigned_char_round_up_generic")
    M_DIAG_WARN((value) > (UCHAR_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto != 0);
        return M_STATIC_CAST(unsigned char, (value + roundto - 1) & ~(roundto - 1));
    }

    //! \fn unsigned char unsigned_char_round_down_power2(unsigned char value, unsigned char roundto)
    //! \brief Rounds an unsigned char value down to the nearest power-of-two multiple.
    //! \param[in] value The unsigned char value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power-of-two multiple of \a roundto.
    static M_INLINE unsigned char unsigned_char_round_down_power2(unsigned char value, unsigned char roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use unsigned_char_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto != 0);
        return M_STATIC_CAST(unsigned char, (value & ~(roundto - 1)));
    }

    //! \fn short signed_short_round_up_power2(short value, short roundto)
    //! \brief Rounds a signed short value up to the nearest power-of-two multiple.
    //! \param[in] value The signed short value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note Uses unsigned arithmetic internally for defined bitwise behavior; results are cast back to the signed
    //! type.
    //! \return The value rounded up to the nearest power-of-two multiple of \a roundto.
    static M_INLINE short signed_short_round_up_power2(short value, short roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use signed_short_round_up_generic")
    M_DIAG_WARN((value) > (SHRT_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        unsigned short u   = M_STATIC_CAST(unsigned short, value);
        unsigned short r   = M_STATIC_CAST(unsigned short, roundto);
        unsigned short res = M_STATIC_CAST(unsigned short, (u + r - 1) & ~(r - 1));
        return M_STATIC_CAST(short, res);
    }

    //! \fn short signed_short_round_down_power2(short value, short roundto)
    //! \brief Rounds a signed short value down to the nearest power-of-two multiple.
    //! \param[in] value The signed short value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note Uses unsigned arithmetic internally for defined bitwise behavior; results are cast back to the signed
    //! type.
    //! \return The value rounded down to the nearest power-of-two multiple of \a roundto.
    static M_INLINE short signed_short_round_down_power2(short value, short roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use signed_short_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        unsigned short u   = M_STATIC_CAST(unsigned short, value);
        unsigned short r   = M_STATIC_CAST(unsigned short, roundto);
        unsigned short res = M_STATIC_CAST(unsigned short, (u & ~(r - 1)));
        return M_STATIC_CAST(short, res);
    }

    //! \fn unsigned short unsigned_short_round_up_power2(unsigned short value, unsigned short roundto)
    //! \brief Rounds an unsigned short value up to the nearest power-of-two multiple.
    //! \param[in] value The unsigned short value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power-of-two multiple of \a roundto.
    static M_INLINE unsigned short unsigned_short_round_up_power2(unsigned short value, unsigned short roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use unsigned_short_round_up_generic")
    M_DIAG_WARN((value) > (USHRT_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto != 0);
        return M_STATIC_CAST(unsigned short, (value + roundto - 1) & ~(roundto - 1));
    }

    //! \fn unsigned short unsigned_short_round_down_power2(unsigned short value, unsigned short roundto)
    //! \brief Rounds an unsigned short value down to the nearest power-of-two multiple.
    //! \param[in] value The unsigned short value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power-of-two multiple of \a roundto.
    static M_INLINE unsigned short unsigned_short_round_down_power2(unsigned short value, unsigned short roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use unsigned_short_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto != 0);
        return M_STATIC_CAST(unsigned short, (value & ~(roundto - 1)));
    }

    //! \fn int signed_int_round_up_power2(int value, int roundto)
    //! \brief Rounds a signed int value up to the nearest power-of-two multiple.
    //! \param[in] value The signed int value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note Uses unsigned arithmetic internally for defined bitwise behavior; results are cast back to the signed
    //! type.
    //! \return The value rounded up to the nearest power-of-two multiple of \a roundto.
    static M_INLINE int signed_int_round_up_power2(int value, int roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use signed_int_round_up_generic")
    M_DIAG_WARN((value) > (INT_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        unsigned int u   = M_STATIC_CAST(unsigned int, value);
        unsigned int r   = M_STATIC_CAST(unsigned int, roundto);
        unsigned int res = M_STATIC_CAST(unsigned int, (u + r - 1) & ~(r - 1));
        return M_STATIC_CAST(int, res);
    }

    //! \fn int signed_int_round_down_power2(int value, int roundto)
    //! \brief Rounds a signed int value down to the nearest power-of-two multiple.
    //! \param[in] value The signed int value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note Uses unsigned arithmetic internally for defined bitwise behavior; results are cast back to the signed
    //! type.
    //! \return The value rounded down to the nearest power-of-two multiple of \a roundto.
    static M_INLINE int signed_int_round_down_power2(int value, int roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use signed_int_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        unsigned int u   = M_STATIC_CAST(unsigned int, value);
        unsigned int r   = M_STATIC_CAST(unsigned int, roundto);
        unsigned int res = M_STATIC_CAST(unsigned int, (u & ~(r - 1)));
        return M_STATIC_CAST(int, res);
    }

    //! \fn unsigned int unsigned_int_round_up_power2(unsigned int value, unsigned int roundto)
    //! \brief Rounds an unsigned int value up to the nearest power-of-two multiple.
    //! \param[in] value The unsigned int value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power-of-two multiple of \a roundto.
    static M_INLINE unsigned int unsigned_int_round_up_power2(unsigned int value, unsigned int roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use unsigned_int_round_up_generic")
    M_DIAG_WARN((value) > (UINT_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto != 0);
        return (value + roundto - 1) & ~(roundto - 1);
    }

    //! \fn unsigned int unsigned_int_round_down_power2(unsigned int value, unsigned int roundto)
    //! \brief Rounds an unsigned int value down to the nearest power-of-two multiple.
    //! \param[in] value The unsigned int value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power-of-two multiple of \a roundto.
    static M_INLINE unsigned int unsigned_int_round_down_power2(unsigned int value, unsigned int roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use unsigned_int_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto != 0);
        return value & ~(roundto - 1);
    }

    //! \fn long signed_long_round_up_power2(long value, long roundto)
    //! \brief Rounds a signed long value up to the nearest power-of-two multiple.
    //! \param[in] value The signed long value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note Uses unsigned arithmetic internally for defined bitwise behavior; results are cast back to the signed
    //! type.
    //! \return The value rounded up to the nearest power-of-two multiple of \a roundto.
    static M_INLINE long signed_long_round_up_power2(long value, long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use signed_long_round_up_generic")
    M_DIAG_WARN((value) > (LONG_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        unsigned long u   = M_STATIC_CAST(unsigned long, value);
        unsigned long r   = M_STATIC_CAST(unsigned long, roundto);
        unsigned long res = M_STATIC_CAST(unsigned long, (u + r - 1) & ~(r - 1));
        return M_STATIC_CAST(long, res);
    }

    //! \fn long signed_long_round_down_power2(long value, long roundto)
    //! \brief Rounds a signed long value down to the nearest power-of-two multiple.
    //! \param[in] value The signed long value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note Uses unsigned arithmetic internally for defined bitwise behavior; results are cast back to the signed
    //! type.
    //! \return The value rounded down to the nearest power-of-two multiple of \a roundto.
    static M_INLINE long signed_long_round_down_power2(long value, long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use signed_long_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        unsigned long u   = M_STATIC_CAST(unsigned long, value);
        unsigned long r   = M_STATIC_CAST(unsigned long, roundto);
        unsigned long res = M_STATIC_CAST(unsigned long, (u & ~(r - 1)));
        return M_STATIC_CAST(long, res);
    }

    //! \fn unsigned long unsigned_long_round_up_power2(unsigned long value, unsigned long roundto)
    //! \brief Rounds an unsigned long value up to the nearest power-of-two multiple.
    //! \param[in] value The unsigned long value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power-of-two multiple of \a roundto.
    static M_INLINE unsigned long unsigned_long_round_up_power2(unsigned long value, unsigned long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use unsigned_long_round_up_generic")
    M_DIAG_WARN((value) > (ULONG_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto != 0);
        return (value + roundto - 1) & ~(roundto - 1);
    }

    //! \fn unsigned long unsigned_long_round_down_power2(unsigned long value, unsigned long roundto)
    //! \brief Rounds an unsigned long value down to the nearest power-of-two multiple.
    //! \param[in] value The unsigned long value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power-of-two multiple of \a roundto.
    static M_INLINE unsigned long unsigned_long_round_down_power2(unsigned long value, unsigned long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use unsigned_long_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto != 0);
        return value & ~(roundto - 1);
    }

    //! \fn long long signed_long_long_round_up_power2(long long value, long long roundto)
    //! \brief Rounds a signed long long value up to the nearest power-of-two multiple.
    //! \param[in] value The signed long long value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note Uses unsigned arithmetic internally for defined bitwise behavior; results are cast back to the signed
    //! type.
    //! \return The value rounded up to the nearest power-of-two multiple of \a roundto.
    static M_INLINE long long signed_long_long_round_up_power2(long long value, long long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use signed_long_long_round_up_generic")
    M_DIAG_WARN((value) > (LLONG_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        unsigned long long u   = M_STATIC_CAST(unsigned long long, value);
        unsigned long long r   = M_STATIC_CAST(unsigned long long, roundto);
        unsigned long long res = M_STATIC_CAST(unsigned long long, (u + r - 1) & ~(r - 1));
        return M_STATIC_CAST(long long, res);
    }

    //! \fn long long signed_long_long_round_down_power2(long long value, long long roundto)
    //! \brief Rounds a signed long long value down to the nearest power-of-two multiple.
    //! \param[in] value The signed long long value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \note Uses unsigned arithmetic internally for defined bitwise behavior; results are cast back to the signed
    //! type.
    //! \return The value rounded down to the nearest power-of-two multiple of \a roundto.
    static M_INLINE long long signed_long_long_round_down_power2(long long value, long long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) <= 0, "roundto must be positive non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use signed_long_long_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto > 0);
        unsigned long long u   = M_STATIC_CAST(unsigned long long, value);
        unsigned long long r   = M_STATIC_CAST(unsigned long long, roundto);
        unsigned long long res = M_STATIC_CAST(unsigned long long, (u & ~(r - 1)));
        return M_STATIC_CAST(long long, res);
    }

    //! \fn unsigned long long unsigned_long_long_round_up_power2(unsigned long long value, unsigned long long roundto)
    //! \brief Rounds an unsigned long long value up to the nearest power-of-two multiple.
    //! \param[in] value The unsigned long long value to round up.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \return The value rounded up to the nearest power-of-two multiple of \a roundto.
    static M_INLINE unsigned long long unsigned_long_long_round_up_power2(unsigned long long value,
                                                                          unsigned long long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use unsigned_long_long_round_up_generic")
    M_DIAG_WARN((value) > (ULLONG_MAX - (roundto - 1)), "possible overflow: cast to larger type to avoid overflow when rounding up")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto != 0);
        return (value + roundto - 1) & ~(roundto - 1);
    }

    //! \fn unsigned long long unsigned_long_long_round_down_power2(unsigned long long value, unsigned long long
    //! roundto)
    //! \brief Rounds an unsigned long long value down to the nearest power-of-two multiple.
    //! \param[in] value The unsigned long long value to round down.
    //! \param[in] roundto The power-of-two (positive, non-zero) specifying the nearest multiple to round to.
    //! \return The value rounded down to the nearest power-of-two multiple of \a roundto.
    static M_INLINE unsigned long long unsigned_long_long_round_down_power2(unsigned long long value,
                                                                            unsigned long long roundto)
        // clang-format off
    M_DIAG_ERROR((roundto) == 0, "roundto must be non-zero")
    M_DIAG_ERROR(((roundto) & ((roundto) - 1)) != 0, "roundto must be a power of two. Otherwise use unsigned_long_long_round_down_generic")
    M_DIAG_WARN((roundto) == 1, "roundto is 1; operation is a no-op")
    // clang-format on
    {
        assert(roundto != 0);
        return value & ~(roundto - 1);
    }

    //! \def INT_ROUND_UP
//! \brief rounds a value up to a nearest multiple of another value.
//! For example 10 for nearest multiple of 10, 5 for nearest multiple of 5, etc
//! \param[in] value the integer value to round up
//! \param[in] roundto value specifying nearest multiple to round to
//! \return \a value rounded up to nearest multiple of \a roundto
//!
//! \note Behavior and safety:
//! - When the compiler supports C11 `_Generic` this macro dispatches to
//!   a typed inline helper that preserves signed semantics (round-up = ceiling)
//!   and evaluates each argument exactly once when used via the `_T` variants.
//! - On older toolchains without `_Generic` there are two fallback modes:
//!   1. If `M_TYPEOF` is available the header uses a widest-type helper
//!      (casts to `unsigned long long`), which may change semantics for
//!      negative signed `value`s (unsigned conversion) and is therefore
//!      not equivalent for signed inputs.
//!   2. If neither `_Generic` nor `M_TYPEOF` are available the macro
//!      falls back to an arithmetic expression which may double-evaluate
//!      its arguments (unsafe with side-effects).
//! - Inputs that may trigger incorrect/undefined behavior with the
//!   pre-C11 fallbacks include:
//!   - signed negative `value` when the widest-unsigned cast path is used
//!     (results will be from unsigned arithmetic, not ceiling semantics).
//!   - `roundto == 0` or non-positive `roundto` (asserts or UB depending
//!     on the helper used).
//!   - `roundto == 1` (no-op — harmless but unnecessary).
//!   - expressions with side-effects passed as `value` or `roundto` when
//!     the arithmetic fallback is active (they may be evaluated twice).
//!   - overflow when `(value + roundto - 1)` exceeds the type range.
//! - Recommendation: when portability or strict signed semantics are
//!   required, call the typed helpers directly (e.g. `signed_int_round_up_generic`)
//!   or build with a compiler that provides `_Generic` and `M_TYPEOF`.
/* Prefer C11 _Generic dispatch to typed inline helpers when available. */
#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define INT_ROUND_UP(value, roundto)                                                                               \
        _Generic((value), /* base types */                                                                             \
            signed char: signed_char_round_up_generic,                                                                 \
            unsigned char: unsigned_char_round_up_generic,                                                             \
            short: signed_short_round_up_generic,                                                                      \
            unsigned short: unsigned_short_round_up_generic,                                                           \
            int: signed_int_round_up_generic,                                                                          \
            unsigned int: unsigned_int_round_up_generic,                                                               \
            long: signed_long_round_up_generic,                                                                        \
            unsigned long: unsigned_long_round_up_generic,                                                             \
            long long: signed_long_long_round_up_generic,                                                              \
            unsigned long long: unsigned_long_long_round_up_generic)((value), (roundto))
#else

#    if !defined(NO_TYPEOF)
#        define INT_ROUND_UP(value, roundto)                                                                           \
            M_STATIC_CAST(M_TYPEOF(value),                                                                             \
                          unsigned_long_long_round_up_generic(M_STATIC_CAST(unsigned long long, value),                \
                                                              M_STATIC_CAST(unsigned long long, roundto)))
#    else
#        define INT_ROUND_UP(value, roundto)                                                                           \
            (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value))
#    endif

#endif

//! \def INT_ROUND_DOWN
//! \brief rounds a value down to a nearest multiple of another value.
//! For example 10 for nearest multiple of 10, 5 for nearest multiple of 5, etc
//! \param[in] value the integer value to round down
//! \param[in] roundto value specifying nearest multiple to round to
//! \return \a value rounded down to nearest multiple of \a roundto
//!
//! \note Behavior and safety (pre-C11 fallbacks):
//! - With C11 `_Generic` the macro dispatches to typed helpers that
//!   preserve signed semantics (round-down = floor).
//! - Without `_Generic` the same widest-type cast or arithmetic fallbacks
//!   described for `INT_ROUND_UP` apply; these can change semantics for
//!   negative signed inputs or double-evaluate arguments.
//! - Problematic inputs for fallbacks:
//!   - signed negative `value` when cast to unsigned for the widest-type
//!     helper (may produce incorrect results).
//!   - `roundto == 0` or non-positive `roundto` (asserts/UB).
//!   - side-effecting expressions when the arithmetic fallback is used
//!     (they may be evaluated twice).
//! - Recommendation: call the typed helpers directly for deterministic
//!   behavior on older compilers.
/* Prefer C11 _Generic dispatch to typed inline helpers when available. */
#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define INT_ROUND_DOWN(value, roundto)                                                                             \
        _Generic((value), /* base types */                                                                             \
            signed char: signed_char_round_down_generic,                                                               \
            unsigned char: unsigned_char_round_down_generic,                                                           \
            short: signed_short_round_down_generic,                                                                    \
            unsigned short: unsigned_short_round_down_generic,                                                         \
            int: signed_int_round_down_generic,                                                                        \
            unsigned int: unsigned_int_round_down_generic,                                                             \
            long: signed_long_round_down_generic,                                                                      \
            unsigned long: unsigned_long_round_down_generic,                                                           \
            long long: signed_long_long_round_down_generic,                                                            \
            unsigned long long: unsigned_long_long_round_down_generic)((value), (roundto))
#else
#    if !defined(NO_TYPEOF)
#        define INT_ROUND_DOWN(value, roundto)                                                                         \
            M_STATIC_CAST(M_TYPEOF(value),                                                                             \
                          unsigned_long_long_round_down_generic(M_STATIC_CAST(unsigned long long, value),              \
                                                                M_STATIC_CAST(unsigned long long, roundto)))
#    else
#        define INT_ROUND_DOWN(value, roundto) (((value) % (roundto)) ? ((value) - ((value) % (roundto))) : (value))
#    endif
#endif

//!
//! Power-of-two optimized macros
//! These select power-of-two optimized typed helpers when available via C11
//! `_Generic`. Fallbacks use the arithmetic/bitmask expressions (may
//! double-evaluate arguments).
//!
//! \note Behavior and safety (pre-C11 fallbacks):
//! - The power-of-two helpers assume `roundto` is a power of two. When
//!   `_Generic` is not available the fallback may either call the `_T`
//!   single-eval dispatch (if `M_TYPEOF` is present) or fall back to the
//!   bitmask arithmetic which may double-evaluate arguments.
//! - Problematic inputs for pre-C11 fallbacks include:
//!   - `roundto` not a power of two (results are undefined/incorrect).
//!   - `roundto == 0` (undefined behavior / division-by-zero like issues).
//!   - signed negative `value` when the widest-unsigned cast path is used
//!     (unsigned conversion changes semantics).
//!   - side-effecting expressions passed to the macro when the arithmetic
//!     fallback is active (may be evaluated twice).
//! - Recommendation: call specific typed `*_round_up_power2` /
//!   `*_round_down_power2` helpers directly when portability or strict
//!   signed semantics are required.
//!
#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define INT_ROUND_UP_POWER2(value, roundto)                                                                        \
        _Generic((value),                                                                                              \
            signed char: signed_char_round_up_power2,                                                                  \
            unsigned char: unsigned_char_round_up_power2,                                                              \
            short: signed_short_round_up_power2,                                                                       \
            unsigned short: unsigned_short_round_up_power2,                                                            \
            int: signed_int_round_up_power2,                                                                           \
            unsigned int: unsigned_int_round_up_power2,                                                                \
            long: signed_long_round_up_power2,                                                                         \
            unsigned long: unsigned_long_round_up_power2,                                                              \
            long long: signed_long_long_round_up_power2,                                                               \
            unsigned long long: unsigned_long_long_round_up_power2)((value), (roundto))
#else

#    define INT_ROUND_UP_POWER2(value, roundto) (((value) + (roundto) - 1) & ~((roundto) - 1))
#endif

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define INT_ROUND_DOWN_POWER2(value, roundto)                                                                      \
        _Generic((value),                                                                                              \
            signed char: signed_char_round_down_power2,                                                                \
            unsigned char: unsigned_char_round_down_power2,                                                            \
            short: signed_short_round_down_power2,                                                                     \
            unsigned short: unsigned_short_round_down_power2,                                                          \
            int: signed_int_round_down_power2,                                                                         \
            unsigned int: unsigned_int_round_down_power2,                                                              \
            long: signed_long_round_down_power2,                                                                       \
            unsigned long: unsigned_long_round_down_power2,                                                            \
            long long: signed_long_long_round_down_power2,                                                             \
            unsigned long long: unsigned_long_long_round_down_power2)((value), (roundto))
#else

#    define INT_ROUND_DOWN_POWER2(value, roundto) ((value) & ~((roundto) - 1))
#endif

    //! \enum UINT*_MAX_POWER2
    //! \brief Maximum exponent for power-of-two rounding functions to avoid overflow for each unsigned integer type.
    //! For example, `UINT8_MAX_POWER2` is 8 because 2^8 = 256, which is the first power of two that exceeds the maximum
    //! value of an unsigned char (255). These constants can be used to validate inputs to the power-of-two rounding
    //! functions to prevent overflow.
    //! \code
    //! if (exponent >= UINT8_MAX_POWER2) {
    //!     // handle error: exponent too large for uint8_t power-of-two functions
    //! }
    //! \endcode

    enum
    {
        UINT8_MAX_POWER2  = 8,
        UINT16_MAX_POWER2 = 16,
        UINT32_MAX_POWER2 = 32,
        UINT64_MAX_POWER2 = 64
    };

    //! \fn uint64_t power_Of_Two(uint16_t exponent)
    //! \brief returns 2^exponent value as uint64_t. Maximum exponent value is 63 to avoid overflow (2^64 would overflow
    //! uint64_t).
    //! \param[in] exponent exponent value to raise 2 by
    //! \return result of 2^exponent calculation
    static M_INLINE uint64_t power_Of_Two(uint16_t exponent)
        // clang-format off
    M_DIAG_ERROR((exponent) >= UINT64_MAX_POWER2, "exponent must be less than 64 to avoid overflow")
    // clang-format on
    {
        assert(exponent < UINT64_MAX_POWER2 && "Exponent must be less than 64 to avoid overflow");
        return (UINT64_C(1) << exponent);
    }

    //! \fn uint64_t log2_power2(uint64_t p2val)
    //! \brief Calculates log2 for a value that is a power of 2
    //! Using this on values that are NOT a power of two will return an incorrect result
    //! \param[in] p2val power of 2 value to perform log2 on.
    //! \return result of log2(p2val)
    //! \note if passed zero, this returns UINT64_MAX as a way to indicate an error since log2(0) is undefined.
    static M_INLINE uint64_t log2_power2(uint64_t p2val)
        // clang-format off
    M_DIAG_ERROR((p2val) == 0, "Input must be non-zero")
    M_DIAG_ERROR(((p2val) & ((p2val) - 1)) != 0, "Input must be a power of two")
    // clang-format on
    {
        if (p2val == 0)
        {
            assert(false && "Input must be non-zero"); // just to help with debugging, but this is unlikely to happen
            return UINT64_MAX;                         // Sentinel value for invalid input
        }
        assert((p2val & (p2val - 1)) == 0 && "Input must be a power of two");
        return count_trailing_zeros(p2val);
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
