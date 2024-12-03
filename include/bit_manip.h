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
// \file bit_manip.h
// \brief Implements various functions and macros to manipulate bit fields,
// manipulate bytes, swap bytes, etc

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "env_detect.h"
#include "memory_safety.h"
#include "type_conversion.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    // get a specific double word
    static M_INLINE uint32_t get_DWord0(uint64_t value)
    {
        return M_STATIC_CAST(uint32_t, value & UINT64_C(0x00000000FFFFFFFF));
    }

#define M_DoubleWord0(l) get_DWord0(l)

    static M_INLINE uint32_t get_DWord1(uint64_t value)
    {
        return M_STATIC_CAST(uint32_t, (value & UINT64_C(0xFFFFFFFF00000000)) >> 32);
    }

#define M_DoubleWord1(l) get_DWord1(l)

// get a specific double word
#define M_DoubleWordInt0(l) (M_STATIC_CAST(int32_t, (((l)&UINT64_C(0x00000000FFFFFFFF)) >> 0)))
#define M_DoubleWordInt1(l) (M_STATIC_CAST(int32_t, (((l)&UINT64_C(0xFFFFFFFF00000000)) >> 32)))

// get a specific word
#define M_Word0(l) (M_STATIC_CAST(uint16_t, (((l)&UINT64_C(0x000000000000FFFF)) >> 0)))
#define M_Word1(l) (M_STATIC_CAST(uint16_t, (((l)&UINT64_C(0x00000000FFFF0000)) >> 16)))
#define M_Word2(l) (M_STATIC_CAST(uint16_t, (((l)&UINT64_C(0x0000FFFF00000000)) >> 32)))
#define M_Word3(l) (M_STATIC_CAST(uint16_t, (((l)&UINT64_C(0xFFFF000000000000)) >> 48)))

// get a specific word as int's
#define M_WordInt0(l) (M_STATIC_CAST(int16_t, (((l)&UINT64_C(0x000000000000FFFF)) >> 0)))
#define M_WordInt1(l) (M_STATIC_CAST(int16_t, (((l)&UINT64_C(0x00000000FFFF0000)) >> 16)))
#define M_WordInt2(l) (M_STATIC_CAST(int16_t, (((l)&UINT64_C(0x0000FFFF00000000)) >> 32)))
#define M_WordInt3(l) (M_STATIC_CAST(int16_t, (((l)&UINT64_C(0xFFFF000000000000)) >> 48)))

// need to validate that this macro sets the correct bits on 32bit and 64bit
#define BITSPERBYTE UINT8_C(8)
#define M_ByteN(n)  ((UINT8_MAX << ((n)*BITSPERBYTE)))

// Get a specific byte
#define M_Byte0(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x00000000000000FF)) >> 0)))
#define M_Byte1(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x000000000000FF00)) >> 8)))
#define M_Byte2(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x0000000000FF0000)) >> 16)))
#define M_Byte3(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x00000000FF000000)) >> 24)))
#define M_Byte4(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x000000FF00000000)) >> 32)))
#define M_Byte5(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x0000FF0000000000)) >> 40)))
#define M_Byte6(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x00FF000000000000)) >> 48)))
#define M_Byte7(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0xFF00000000000000)) >> 56)))

// Get a specific byte int
#define M_ByteInt0(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x00000000000000FF)) >> 0)))
#define M_ByteInt1(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x000000000000FF00)) >> 8)))
#define M_ByteInt2(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x0000000000FF0000)) >> 16)))
#define M_ByteInt3(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x00000000FF000000)) >> 24)))
#define M_ByteInt4(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x000000FF00000000)) >> 32)))
#define M_ByteInt5(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x0000FF0000000000)) >> 40)))
#define M_ByteInt6(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x00FF000000000000)) >> 48)))
#define M_ByteInt7(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0xFF00000000000000)) >> 56)))

// get a specific nibble
#define M_Nibble0(l)  M_STATIC_CAST(uint8_t, M_STATIC_CAST(uint8_t, l) & M_STATIC_CAST(uint8_t, UINT8_C(0x0F)))
#define M_Nibble1(l)  M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint8_t, l) & UINT8_C(0xF0)) >> 4))
#define M_Nibble2(l)  M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint16_t, l) & UINT16_C(0x0F00)) >> 8))
#define M_Nibble3(l)  M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint16_t, l) & UINT16_C(0xF000)) >> 12))
#define M_Nibble4(l)  M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint32_t, l) & UINT32_C(0x000F0000)) >> 16))
#define M_Nibble5(l)  M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint32_t, l) & UINT32_C(0x00F00000)) >> 20))
#define M_Nibble6(l)  M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint32_t, l) & UINT32_C(0x0F000000)) >> 24))
#define M_Nibble7(l)  M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint32_t, l) & UINT32_C(0xF0000000)) >> 28))
#define M_Nibble8(l)  M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x0000000F00000000)) >> 32))
#define M_Nibble9(l)  M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x000000F000000000)) >> 36))
#define M_Nibble10(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x00000F0000000000)) >> 40))
#define M_Nibble11(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x0000F00000000000)) >> 44))
#define M_Nibble12(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x000F000000000000)) >> 48))
#define M_Nibble13(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x00F0000000000000)) >> 52))
#define M_Nibble14(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x0F00000000000000)) >> 56))
#define M_Nibble15(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0xF000000000000000)) >> 60))

    static M_INLINE uint8_t nibbles_To_Byte(uint8_t upperNibble, uint8_t lowerNibble)
    {
        return M_STATIC_CAST(uint8_t, (((upperNibble)&M_STATIC_CAST(uint8_t, 0x0F)) << 4) |
                                          (((lowerNibble)&M_STATIC_CAST(uint8_t, 0x0F)) << 0));
    }

#define M_NibblesTo1ByteValue(n1, n0) nibbles_To_Byte(n1, n0)

    static M_INLINE uint16_t bytes_To_Uint16(uint8_t msb, uint8_t lsb)
    {
        return M_STATIC_CAST(uint16_t, (M_STATIC_CAST(uint16_t, msb) << 8) | (M_STATIC_CAST(uint16_t, lsb) << 0));
    }

#define M_BytesTo2ByteValue(b1, b0) bytes_To_Uint16(b1, b0)

    static M_INLINE uint32_t bytes_To_Uint32(uint8_t msb, uint8_t byte2, uint8_t byte1, uint8_t lsb)
    {
        return (M_STATIC_CAST(uint32_t, msb) << 24) | (M_STATIC_CAST(uint32_t, byte2) << 16) |
               (M_STATIC_CAST(uint32_t, byte1) << 8) | (M_STATIC_CAST(uint32_t, lsb) << 0);
    }

#define M_BytesTo4ByteValue(b3, b2, b1, b0) bytes_To_Uint32(b3, b2, b1, b0)

    static M_INLINE uint64_t bytes_To_Uint64(uint8_t msb,
                                             uint8_t byte6,
                                             uint8_t byte5,
                                             uint8_t byte4,
                                             uint8_t byte3,
                                             uint8_t byte2,
                                             uint8_t byte1,
                                             uint8_t lsb)
    {
        return (M_STATIC_CAST(uint64_t, msb) << 56) | (M_STATIC_CAST(uint64_t, byte6) << 48) |
               (M_STATIC_CAST(uint64_t, byte5) << 40) | (M_STATIC_CAST(uint64_t, byte4) << 32) |
               (M_STATIC_CAST(uint64_t, byte3) << 24) | (M_STATIC_CAST(uint64_t, byte2) << 16) |
               (M_STATIC_CAST(uint64_t, byte1) << 8) | (M_STATIC_CAST(uint64_t, lsb) << 0);
    }

#define M_BytesTo8ByteValue(b7, b6, b5, b4, b3, b2, b1, b0) bytes_To_Uint64(b7, b6, b5, b4, b3, b2, b1, b0)

    static M_INLINE uint32_t words_To_Uint32(uint16_t msw, uint16_t lsw)
    {
        return (M_STATIC_CAST(uint32_t, msw) << 16) | (M_STATIC_CAST(uint32_t, lsw) << 0);
    }

#define M_WordsTo4ByteValue(w1, w0) words_To_Uint32(w0, w1)

    static M_INLINE uint64_t words_To_Uint64(uint16_t msw, uint16_t word2, uint16_t word1, uint16_t lsw)
    {
        return (M_STATIC_CAST(uint64_t, msw) << 48) | (M_STATIC_CAST(uint64_t, word2) << 32) |
               (M_STATIC_CAST(uint64_t, word1) << 16) | (M_STATIC_CAST(uint64_t, lsw) << 0);
    }

#define M_WordsTo8ByteValue(w3, w2, w1, w0) words_To_Uint64(w3, w2, w1, w0)

    static M_INLINE uint64_t dwords_To_Uint64(uint32_t msdw, uint32_t lsdw)
    {
        return (M_STATIC_CAST(uint64_t, msdw) << 32) | (M_STATIC_CAST(uint64_t, lsdw) << 0);
    }

#define M_DWordsTo8ByteValue(d1, d0) dwords_To_Uint64(d1, d0)

// MACRO to round the number of x so that it will not round up when formating
// the float
#define ROUNDF(f, c) M_STATIC_CAST(float, (M_STATIC_CAST(int, (f) * (c))) / (c))

    typedef struct sgenericint_t
    {
        bool   issigned;   /*is this from a signed integer or not*/
        size_t sizeoftype; /*set to sizzeof(var) when using this structure*/
        union
        {
            uint8_t  u8;
            uint16_t u16;
            uint32_t u32;
            uint64_t u64;
            int8_t   i8;
            int16_t  i16;
            int32_t  i32;
            int64_t  i64;
        };
    } genericint_t;

    static M_INLINE bool is_generic_int_valid(genericint_t genint)
    {
        bool good = false;
        switch (genint.sizeoftype)
        {
        case sizeof(uint8_t):
        case sizeof(uint16_t):
        case sizeof(uint32_t):
        case sizeof(uint64_t):
            good = true;
            break;
        }
        return good;
    }

    genericint_t generic_Get_Bit_Range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb);

    static M_INLINE uint8_t get_bit_range_uint8(uint8_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = false;
        genint.u8         = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint8_t), msb, lsb);
        return genint.u8;
    }

    static M_INLINE uint16_t get_bit_range_uint16(uint16_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = false;
        genint.u16        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint16_t), msb, lsb);
        return genint.u16;
    }

    static M_INLINE uint8_t get_8bit_range_uint16(uint16_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = false;
        genint.u16        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint8_t), msb, lsb);
        return genint.u8;
    }

    static M_INLINE uint32_t get_bit_range_uint32(uint32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = false;
        genint.u32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint32_t), msb, lsb);
        return genint.u32;
    }

    static M_INLINE uint8_t get_8bit_range_uint32(uint32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = false;
        genint.u32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint8_t), msb, lsb);
        return genint.u8;
    }

    static M_INLINE uint16_t get_16bit_range_uint32(uint32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = false;
        genint.u32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint16_t), msb, lsb);
        return genint.u16;
    }

    static M_INLINE uint64_t get_bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = false;
        genint.u64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint64_t), msb, lsb);
        return genint.u64;
    }

    static M_INLINE uint8_t get_8bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = false;
        genint.u64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint8_t), msb, lsb);
        return genint.u8;
    }

    static M_INLINE uint16_t get_16bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = false;
        genint.u64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint16_t), msb, lsb);
        return genint.u16;
    }

    static M_INLINE uint32_t get_32bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = false;
        genint.u64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint32_t), msb, lsb);
        return genint.u32;
    }

    static M_INLINE int8_t get_bit_range_int8(int8_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = true;
        genint.i8         = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int8_t), msb, lsb);
        return genint.i8;
    }

    static M_INLINE int16_t get_bit_range_int16(int16_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = true;
        genint.i16        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int16_t), msb, lsb);
        return genint.i16;
    }

    static M_INLINE int8_t get_8bit_range_int16(int16_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = true;
        genint.i16        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int8_t), msb, lsb);
        return genint.i8;
    }

    static M_INLINE int32_t get_bit_range_int32(int32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = true;
        genint.i32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int32_t), msb, lsb);
        return genint.i32;
    }

    static M_INLINE int8_t get_8bit_range_int32(int32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = true;
        genint.i32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int8_t), msb, lsb);
        return genint.i8;
    }

    static M_INLINE int16_t get_16bit_range_int32(int32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = true;
        genint.i32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int16_t), msb, lsb);
        return genint.i16;
    }

    static M_INLINE int64_t get_bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = true;
        genint.i64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int64_t), msb, lsb);
        return genint.i64;
    }

    static M_INLINE int8_t get_8bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = true;
        genint.i64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int8_t), msb, lsb);
        return genint.i8;
    }

    static M_INLINE int16_t get_16bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = true;
        genint.i64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int16_t), msb, lsb);
        return genint.i16;
    }

    static M_INLINE int32_t get_32bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        safe_memset(&genint, sizeof(genericint_t), 0, sizeof(genericint_t));
        genint.issigned   = true;
        genint.i64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int32_t), msb, lsb);
        return genint.i32;
    }

// recommend using the inlines above when possible, but this will work as the
// macro always did in the past -TJE
#define M_GETBITRANGE(input, msb, lsb)                                                                                 \
    (((msb) - (lsb) + 1) <= 8                                                                                          \
         ? get_8bit_range_uint64(input, msb, lsb)                                                                      \
         : (((msb) - (lsb) + 1) <= 16                                                                                  \
                ? get_16bit_range_uint64(input, msb, lsb)                                                              \
                : (((msb) - (lsb) + 1) <= 32                                                                           \
                       ? get_32bit_range_uint64(input, msb, lsb)                                                       \
                       : (((msb) - (lsb) + 1) <= 64 ? get_bit_range_uint64(input, msb, lsb) : UINTMAX_MAX))))

// get bit range for signed values
#define M_IGETBITRANGE(input, msb, lsb)                                                                                \
    (((msb) - (lsb) + 1) <= 8                                                                                          \
         ? get_8bit_range_int64(input, msb, lsb)                                                                       \
         : (((msb) - (lsb) + 1) <= 16                                                                                  \
                ? get_16bit_range_int64(input, msb, lsb)                                                               \
                : (((msb) - (lsb) + 1) <= 32                                                                           \
                       ? get_32bit_range_int64(input, msb, lsb)                                                        \
                       : (((msb) - (lsb) + 1) <= 64 ? get_bit_range_int64(input, msb, lsb) : INTMAX_MAX))))

    // Bit access macros

#define M_BitN(n)   (UINT64_C(1) << (n))

#define M_BitN8(n)  (UINT8_C(1) << (n))
#define M_BitN16(n) (UINT16_C(1) << (n))
#define M_BitN32(n) (UINT32_C(1) << (n))
#define M_BitN64(n) (UINT64_C(1) << (n))

// defined in EDK2 MdePkg and causes conflicts, so checking this define for now
// to avoid conflicts
#if !defined(UEFI_C_SOURCE)

#    define BIT0  (M_BitN8(0))
#    define BIT1  (M_BitN8(1))
#    define BIT2  (M_BitN8(2))
#    define BIT3  (M_BitN8(3))
#    define BIT4  (M_BitN8(4))
#    define BIT5  (M_BitN8(5))
#    define BIT6  (M_BitN8(6))
#    define BIT7  (M_BitN8(7))
#    define BIT8  (M_BitN16(8))
#    define BIT9  (M_BitN16(9))
#    define BIT10 (M_BitN16(10))
#    define BIT11 (M_BitN16(11))
#    define BIT12 (M_BitN16(12))
#    define BIT13 (M_BitN16(13))
#    define BIT14 (M_BitN16(14))
#    define BIT15 (M_BitN16(15))
#    define BIT16 (M_BitN32(16))
#    define BIT17 (M_BitN32(17))
#    define BIT18 (M_BitN32(18))
#    define BIT19 (M_BitN32(19))
#    define BIT20 (M_BitN32(20))
#    define BIT21 (M_BitN32(21))
#    define BIT22 (M_BitN32(22))
#    define BIT23 (M_BitN32(23))
#    define BIT24 (M_BitN32(24))
#    define BIT25 (M_BitN32(25))
#    define BIT26 (M_BitN32(26))
#    define BIT27 (M_BitN32(27))
#    define BIT28 (M_BitN32(28))
#    define BIT29 (M_BitN32(29))
#    define BIT30 (M_BitN32(30))
#    define BIT31 (M_BitN32(31))
#    define BIT32 (M_BitN64(32))
#    define BIT33 (M_BitN64(33))
#    define BIT34 (M_BitN64(34))
#    define BIT35 (M_BitN64(35))
#    define BIT36 (M_BitN64(36))
#    define BIT37 (M_BitN64(37))
#    define BIT38 (M_BitN64(38))
#    define BIT39 (M_BitN64(39))
#    define BIT40 (M_BitN64(40))
#    define BIT41 (M_BitN64(41))
#    define BIT42 (M_BitN64(42))
#    define BIT43 (M_BitN64(43))
#    define BIT44 (M_BitN64(44))
#    define BIT45 (M_BitN64(45))
#    define BIT46 (M_BitN64(46))
#    define BIT47 (M_BitN64(47))
#    define BIT48 (M_BitN64(48))
#    define BIT49 (M_BitN64(49))
#    define BIT50 (M_BitN64(50))
#    define BIT51 (M_BitN64(51))
#    define BIT52 (M_BitN64(52))
#    define BIT53 (M_BitN64(53))
#    define BIT54 (M_BitN64(54))
#    define BIT55 (M_BitN64(55))
#    define BIT56 (M_BitN64(56))
#    define BIT57 (M_BitN64(57))
#    define BIT58 (M_BitN64(58))
#    define BIT59 (M_BitN64(59))
#    define BIT60 (M_BitN64(60))
#    define BIT61 (M_BitN64(61))
#    define BIT62 (M_BitN64(62))
#    define BIT63 (M_BitN64(63))

#endif // UEFI_C_SOURCE

// set a bit to 1 within a value. NOTE: it is recommended to use the inline
// functions or type width specific macros instead
#define M_SET_BIT(val, bitNum) ((val) |= M_BitN(bitNum))

// clear a bit to 0 within a value. NOTE: it is recommended to use the inline
// functions or type width specific macros instead
#define M_CLEAR_BIT(val, bitNum) ((val) &= (~M_BitN(bitNum)))

    // Inline functions for setting a bit
    static M_INLINE uint8_t set_uint8_bit(uint8_t val, uint8_t bitNum)
    {
        return M_STATIC_CAST(uint8_t, val | M_STATIC_CAST(uint8_t, UINT8_C(1) << bitNum));
    }

    static M_INLINE uint16_t set_uint16_bit(uint16_t val, uint16_t bitNum)
    {
        return M_STATIC_CAST(uint16_t, val | M_STATIC_CAST(uint16_t, UINT16_C(1) << bitNum));
    }

    static M_INLINE uint32_t set_uint32_bit(uint32_t val, uint32_t bitNum)
    {
        return val | M_STATIC_CAST(uint32_t, UINT32_C(1) << bitNum);
    }

    static M_INLINE uint64_t set_uint64_bit(uint64_t val, uint64_t bitNum)
    {
        return val | M_STATIC_CAST(uint64_t, UINT64_C(1) << bitNum);
    }

    // Inline functions for clearing a bit
    static M_INLINE uint8_t clear_uint8_bit(uint8_t val, uint8_t bitNum)
    {
        return M_STATIC_CAST(uint8_t, val & M_STATIC_CAST(uint8_t, ~(UINT8_C(1) << bitNum)));
    }

    static M_INLINE uint16_t clear_uint16_bit(uint16_t val, uint16_t bitNum)
    {
        return M_STATIC_CAST(uint16_t, val & M_STATIC_CAST(uint16_t, ~(UINT16_C(1) << bitNum)));
    }

    static M_INLINE uint32_t clear_uint32_bit(uint32_t val, uint32_t bitNum)
    {
        return val & M_STATIC_CAST(uint32_t, ~(UINT32_C(1) << bitNum));
    }

    static M_INLINE uint64_t clear_uint64_bit(uint64_t val, uint64_t bitNum)
    {
        return val & M_STATIC_CAST(uint64_t, ~(UINT64_C(1) << bitNum));
    }

#define M_CLEAR_BIT8(val, bitNum)  ((val) = clear_uint8_bit(val, bitNum))
#define M_CLEAR_BIT16(val, bitNum) ((val) = clear_uint16_bit(val, bitNum))
#define M_CLEAR_BIT32(val, bitNum) ((val) = clear_uint32_bit(val, bitNum))
#define M_CLEAR_BIT64(val, bitNum) ((val) = clear_uint64_bit(val, bitNum))

#define M_SET_BIT8(val, bitNum)    ((val) = set_uint8_bit(val, bitNum))
#define M_SET_BIT16(val, bitNum)   ((val) = set_uint16_bit(val, bitNum))
#define M_SET_BIT32(val, bitNum)   ((val) = set_uint32_bit(val, bitNum))
#define M_SET_BIT64(val, bitNum)   ((val) = set_uint64_bit(val, bitNum))

    //-----------------------------------------------------------------------------
    //
    //  nibble_Swap()
    //
    //! \brief   Description:  swap the nibbles in a byte
    //
    //  Entry:
    //!   \param[out] byteToSwap = a pointer to the byte containing the data in
    //!   which to have the nibbles swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void nibble_Swap(uint8_t* byteToSwap);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_16()
    //
    //! \brief   Description:  swap the bytes in a word
    //
    //  Entry:
    //!   \param[out] wordToSwap = a pointer to the word containing the data in
    //!   which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_16(uint16_t* wordToSwap);
    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_Int16()
    //
    //! \brief   Description:  swap the bytes in a singned word
    //
    //  Entry:
    //!   \param[out] signedWordToSwap = a pointer to the signed word containing the
    //!   data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_Int16(int16_t* signedWordToSwap);
    //-----------------------------------------------------------------------------
    //
    //  big_To_Little_Endian_16()
    //
    //! \brief   Description:  swap the bytes in a word only if on little endian
    //! system.
    //
    //  Entry:
    //!   \param[out] wordToSwap = a pointer to the word containing the data in
    //!   which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void big_To_Little_Endian_16(uint16_t* wordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_32()
    //
    //! \brief   Description:  swap the bytes in a double word - useful for
    //! converting between endianness
    //
    //  Entry:
    //!   \param[out] doubleWordToSwap = a pointer to the double word containing the
    //!   data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_32(uint32_t* doubleWordToSwap);
    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_Int32()
    //
    //! \brief   Description:  swap the bytes in a Signed double word - useful for
    //! converting between endianness
    //
    //  Entry:
    //!   \param[out] signedDWord = a pointer to the Signed double word containing
    //!   the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_Int32(int32_t* signedDWord);
    //-----------------------------------------------------------------------------
    //
    //  big_To_Little_Endian_32()
    //
    //! \brief   Description:  swap the bytes in a double word only if running on
    //! little endian system
    //
    //  Entry:
    //!   \param[out] doubleWordToSwap = a pointer to the double word containing the
    //!   data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void big_To_Little_Endian_32(uint32_t* doubleWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  word_Swap_32()
    //
    //! \brief   Description:  swap the words in a double word
    //
    //  Entry:
    //!   \param[out] doubleWordToSwap = a pointer to the double word containing the
    //!   data in which to have the words swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void word_Swap_32(uint32_t* doubleWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_64()
    //
    //! \brief   Description:  swap the bytes in a quad word - useful for conversion
    //! between endianness
    //
    //  Entry:
    //!   \param[out] quadWordToSwap = a pointer to the quad word containing the
    //!   data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_64(uint64_t* quadWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  word_Swap_64()
    //
    //! \brief   Description:  swap the words in a quad word
    //
    //  Entry:
    //!   \param[out] quadWordToSwap = a pointer to the quad word containing the
    //!   data in which to have the words swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void word_Swap_64(uint64_t* quadWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  double_Word_Swap_64()
    //
    //! \brief   Description:  swap the double words in a quad word
    //
    //  Entry:
    //!   \param[out] quadWordToSwap = a pointer to the quad word containing the
    //!   data in which to have the double words swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void double_Word_Swap_64(uint64_t* quadWordToSwap);

    bool get_Bytes_To_16(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint16_t* out);

    bool get_Bytes_To_32(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint32_t* out);

    bool get_Bytes_To_64(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint64_t* out);

    // These functions are similar to those in endian.h, named slightly differently to be common to this library code,
    // but work the same. Defiend ourselves since there are various names and different systems have the endian.h in
    // different locations. Rather than deal with that mess, we defined our own ourselves. - TJE
    M_NODISCARD uint16_t be16_to_host(uint16_t value);
    M_NODISCARD uint32_t be32_to_host(uint32_t value);
    M_NODISCARD uint64_t be64_to_host(uint64_t value);
    M_NODISCARD uint16_t host_to_be16(uint16_t value);
    M_NODISCARD uint32_t host_to_be32(uint32_t value);
    M_NODISCARD uint64_t host_to_be64(uint64_t value);
    M_NODISCARD uint16_t host_to_le16(uint16_t value);
    M_NODISCARD uint32_t host_to_le32(uint32_t value);
    M_NODISCARD uint64_t host_to_le64(uint64_t value);
    M_NODISCARD uint16_t le16_to_host(uint16_t value);
    M_NODISCARD uint32_t le32_to_host(uint32_t value);
    M_NODISCARD uint64_t le64_to_host(uint64_t value);

#if defined(__cplusplus)
}
#endif
