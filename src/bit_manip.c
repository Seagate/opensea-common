// SPDX-License-Identifier: MPL-2.0

//! \file bit_manip.c
//! \brief Implements various functions and macros to manipulate bit fields,
//! manipulate bytes, swap bytes, etc
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "bit_manip.h"
#include "predef_env_detect.h"
#include "type_conversion.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//! \fn static M_INLINE size_t get_Bytes_Abs_Range(size_t msb, size_t lsb)
//! \brief gets the maximum byte range between msb and lsb
//!
//! \param[in] msb most significant bit value
//! \param[in] lsb least  significant bit value
//! \return returns range between msb and lsb
static M_INLINE size_t get_Bytes_Abs_Range(size_t msb, size_t lsb)
{
    if (msb > lsb)
    {
        return msb - lsb;
    }
    else
    {
        return lsb - msb;
    }
}

bool get_Bytes_To_64(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint64_t* out)
{
    if (dataPtrBeginning == M_NULLPTR || out == M_NULLPTR || msb > fullDataLen || lsb > fullDataLen ||
        get_Bytes_Abs_Range(msb, lsb) > sizeof(uint64_t))
    {
        return false;
    }
    *out = UINT64_C(0); // clear to zero before filling in the bytes
    if (lsb <= msb)     // allowing equals for single bytes
    {
        for (size_t iter = msb, counter = 0; counter < fullDataLen && counter < SIZE_MAX && iter >= lsb;
             --iter, ++counter)
        {
            (*out) <<= 8;
            (*out) |= dataPtrBeginning[iter];
            if (iter == 0)
            {
                // exit the loop to make sure there is no undefined behavior
                break;
            }
        }
    }
    else
    {
        // opposite byte ordering from above
        for (size_t iter = msb, counter = 0; counter < fullDataLen && counter < SIZE_MAX && iter <= lsb;
             ++iter, ++counter)
        {
            (*out) <<= 8;
            (*out) |= dataPtrBeginning[iter];
        }
    }
    return true;
}

bool get_Bytes_To_32(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint32_t* out)
{
    if (out && get_Bytes_Abs_Range(msb, lsb) <= sizeof(uint32_t))
    {
        uint64_t temp   = UINT64_C(0);
        bool     result = get_Bytes_To_64(dataPtrBeginning, fullDataLen, msb, lsb, &temp);
        if (result)
        {
            *out = C_CAST(uint32_t, temp);
        }
        return result;
    }
    else
    {
        return false;
    }
}

bool get_Bytes_To_16(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint16_t* out)
{
    if (out && get_Bytes_Abs_Range(msb, lsb) <= sizeof(uint16_t))
    {
        uint64_t temp   = UINT64_C(0);
        bool     result = get_Bytes_To_64(dataPtrBeginning, fullDataLen, msb, lsb, &temp);
        if (result)
        {
            *out = C_CAST(uint16_t, temp);
        }
        return result;
    }
    else
    {
        return false;
    }
}

enum eGenericIntMaxBits
{
    GENERIC_INT_8BIT_MAX  = 7,
    GENERIC_INT_16BIT_MAX = 15,
    GENERIC_INT_32BIT_MAX = 31,
    GENERIC_INT_64BIT_MAX = 63,
    GENERIC_INT_MAX_BIT   = 63
};

/* Width constants for masks and extraction operations. Use explicit
 * named constants instead of magic numbers to make intent clear and
 * to satisfy static analyzers.
 */
enum
{
    GENERIC_WIDTH_0  = 0,
    GENERIC_WIDTH_8  = 8,
    GENERIC_WIDTH_16 = 16,
    GENERIC_WIDTH_32 = 32,
    GENERIC_WIDTH_64 = 64
};

/* Macro to assign output fields concisely.
 * Usage: GEN_ASSIGN_OUT(uint8_t, u8, gen_extract_u8, input.u8)
 * Expands to set out.sizeoftype, out.uX = casted extract, and out.issigned.
 */
#define GEN_ASSIGN_OUT(TARGET_TYPE, OUT_FIELD, EXTRACT_FN, IN_FIELD)                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        out.sizeoftype = sizeof(TARGET_TYPE);                                                                          \
        out.OUT_FIELD  = M_STATIC_CAST(TARGET_TYPE, EXTRACT_FN(IN_FIELD, msb, lsb));                                   \
        out.issigned   = input.issigned;                                                                               \
    } while (0)

/* Helper functions to centralize safe mask/shift logic and avoid
 * undefined behavior when shifting by full type width. These reduce
 * duplication and fix cases where constructing masks like
 * ~(~UINT64_C(0) << width) would shift by 64 (UB).
 */
static M_INLINE size_t gen_bit_width(uint8_t msb, uint8_t lsb)
{
    return uint8_to_sizet(msb) - uint8_to_sizet(lsb) + SIZE_T_C(1);
}

static M_INLINE uint64_t gen_safe_mask_u64(size_t width)
{
    if (width >= GENERIC_WIDTH_64)
    {
        return UINT64_MAX;
    }

    if (width == GENERIC_WIDTH_0)
    {
        return UINT64_C(0);
    }

    return (UINT64_C(1) << width) - UINT64_C(1);
}

static M_INLINE uint32_t gen_safe_mask_u32(size_t width)
{
    if (width >= GENERIC_WIDTH_32)
    {
        return UINT32_MAX;
    }

    if (width == GENERIC_WIDTH_0)
    {
        return UINT32_C(0);
    }

    return (UINT32_C(1) << width) - UINT32_C(1);
}

static M_INLINE uint16_t gen_safe_mask_u16(size_t width)
{
    if (width >= GENERIC_WIDTH_16)
    {
        return UINT16_MAX;
    }

    if (width == GENERIC_WIDTH_0)
    {
        return UINT16_C(0);
    }

    return M_STATIC_CAST(uint16_t, (UINT32_C(1) << width) - UINT32_C(1));
}

static M_INLINE uint8_t gen_safe_mask_u8(size_t width)
{
    if (width >= GENERIC_WIDTH_8)
    {
        return UINT8_MAX;
    }

    if (width == GENERIC_WIDTH_0)
    {
        return UINT8_C(0);
    }

    return M_STATIC_CAST(uint8_t, (UINT32_C(1) << width) - UINT32_C(1));
}

static M_INLINE uint64_t gen_extract_u64(uint64_t val, uint8_t msb, uint8_t lsb)
{
    size_t width = gen_bit_width(msb, lsb);

    if (width == GENERIC_WIDTH_0)
    {
        return UINT64_C(0);
    }

    if (width >= GENERIC_WIDTH_64 && lsb == 0)
    {
        return val;
    }

    if (lsb >= GENERIC_WIDTH_64)
    {
        return UINT64_C(0);
    }

    return M_STATIC_CAST(uint64_t, (val >> lsb) & gen_safe_mask_u64(width));
}

static M_INLINE uint32_t gen_extract_u32(uint32_t val, uint8_t msb, uint8_t lsb)
{
    size_t width = gen_bit_width(msb, lsb);

    if (width == GENERIC_WIDTH_0)
    {
        return UINT32_C(0);
    }

    if (width >= GENERIC_WIDTH_32 && lsb == 0)
    {
        return val;
    }

    if (lsb >= GENERIC_WIDTH_32)
    {
        return UINT32_C(0);
    }

    return M_STATIC_CAST(uint32_t, (val >> lsb) & gen_safe_mask_u32(width));
}

static M_INLINE uint16_t gen_extract_u16(uint16_t val, uint8_t msb, uint8_t lsb)
{
    size_t width = gen_bit_width(msb, lsb);

    if (width == GENERIC_WIDTH_0)
    {
        return M_STATIC_CAST(uint16_t, UINT32_C(0));
    }

    if (width >= GENERIC_WIDTH_16 && lsb == 0)
    {
        return val;
    }

    if (lsb >= GENERIC_WIDTH_16)
    {
        return M_STATIC_CAST(uint16_t, UINT32_C(0));
    }

    return M_STATIC_CAST(uint16_t, (val >> lsb) & gen_safe_mask_u16(width));
}

static M_INLINE uint8_t gen_extract_u8(uint8_t val, uint8_t msb, uint8_t lsb)
{
    size_t width = gen_bit_width(msb, lsb);

    if (width == GENERIC_WIDTH_0)
    {
        return M_STATIC_CAST(uint8_t, UINT32_C(0));
    }

    if (width >= GENERIC_WIDTH_8 && lsb == 0)
    {
        return val;
    }

    if (lsb >= GENERIC_WIDTH_8)
    {
        return M_STATIC_CAST(uint8_t, UINT32_C(0));
    }

    return M_STATIC_CAST(uint8_t, (val >> lsb) & gen_safe_mask_u8(width));
}

//! \fn static M_INLINE genericint_t gen_8bit_range(genericint_t         input,
//!                                            M_ATTR_UNUSED size_t outputsize,
//!                                            uint8_t              msb,
//!                                            uint8_t              lsb)
//! \brief internal function to get bit range in a byte (uint8_t)
//!
//! \param[in] input input genericint_t to get byte range from
//! \param[in] outputsize unused
//! \param[in] msb most significant bit offset value
//! \param[in] lsb least  significant bit offset value
//! \return returns genericint_t in requested range
static M_INLINE genericint_t gen_8bit_range(genericint_t         input,
                                            M_ATTR_UNUSED size_t outputsize,
                                            uint8_t              msb,
                                            uint8_t              lsb)
{
    genericint_t out;
    safe_memset(&out, sizeof(genericint_t), 0, sizeof(genericint_t));
    if (msb > GENERIC_INT_8BIT_MAX || lsb > GENERIC_INT_8BIT_MAX)
    {
        errno = ERANGE;
    }
    else
    {
        size_t bit_count = gen_bit_width(msb, lsb);

        if (bit_count == GENERIC_WIDTH_0 || bit_count > GENERIC_WIDTH_8)
        {
            errno = ERANGE;
        }
        else
        {
            GEN_ASSIGN_OUT(uint8_t, u8, gen_extract_u8, input.u8);
        }
    }
    M_USE_UNUSED(outputsize);
    return out;
}

//! \fn genericint_t gen_16bit_range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb)
//! \brief internal function to get bit range in a word (uint16_t)
//!
//! \param[in] input input genericint_t to get byte range from
//! \param[in] outputsize requested output size (sizeof(uint8_t), sizeof(uint16_t), etc)
//! \param[in] msb most significant bit offset value
//! \param[in] lsb least significant bit offset value
//! \return returns genericint_t in requested range
static M_INLINE genericint_t gen_16bit_range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb)
{
    genericint_t out;
    safe_memset(&out, sizeof(genericint_t), 0, sizeof(genericint_t));
    if (msb > GENERIC_INT_16BIT_MAX || lsb > GENERIC_INT_16BIT_MAX)
    {
        errno = ERANGE;
    }
    else
    {
        size_t bit_count = gen_bit_width(msb, lsb);

        if (outputsize == sizeof(uint8_t))
        {
            if (bit_count == GENERIC_WIDTH_0 || bit_count > GENERIC_WIDTH_8)
            {
                errno = ERANGE;
            }
            else
            {
                GEN_ASSIGN_OUT(uint8_t, u8, gen_extract_u16, input.u16);
            }
        }
        else /* default case where outputsize is not less than uint16 */
        {
            if (bit_count == GENERIC_WIDTH_0 || bit_count > GENERIC_WIDTH_16)
            {
                errno = ERANGE;
            }
            else
            {
                GEN_ASSIGN_OUT(uint16_t, u16, gen_extract_u16, input.u16);
            }
        }
    }
    return out;
}

//! \fn genericint_t gen_32bit_range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb)
//! \brief internal function to get bit range in a dword (uint32_t)
//!
//! \param[in] input input genericint_t to get byte range from
//! \param[in] outputsize requested output size (sizeof(uint8_t), sizeof(uint16_t), etc)
//! \param[in] msb most significant bit offset value
//! \param[in] lsb least significant bit offset value
//! \return returns genericint_t in requested range
static M_INLINE genericint_t gen_32bit_range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb)
{
    genericint_t out;
    safe_memset(&out, sizeof(genericint_t), 0, sizeof(genericint_t));
    if (msb > GENERIC_INT_32BIT_MAX || lsb > GENERIC_INT_32BIT_MAX)
    {
        errno = ERANGE;
    }
    else
    {
        size_t bit_count = gen_bit_width(msb, lsb);

        if (outputsize == sizeof(uint8_t))
        {
            if (bit_count == GENERIC_WIDTH_0 || bit_count > GENERIC_WIDTH_8)
            {
                errno = ERANGE;
            }
            else
            {
                GEN_ASSIGN_OUT(uint8_t, u8, gen_extract_u32, input.u32);
            }
        }
        else if (outputsize == sizeof(uint16_t))
        {
            if (bit_count == GENERIC_WIDTH_0 || bit_count > GENERIC_WIDTH_16)
            {
                errno = ERANGE;
            }
            else
            {
                GEN_ASSIGN_OUT(uint16_t, u16, gen_extract_u32, input.u32);
            }
        }
        else /* default case where outputsize is not less than uint32 */
        {
            if (bit_count == GENERIC_WIDTH_0 || bit_count > GENERIC_WIDTH_32)
            {
                errno = ERANGE;
            }
            else
            {
                GEN_ASSIGN_OUT(uint32_t, u32, gen_extract_u32, input.u32);
            }
        }
    }
    return out;
}

//! \fn genericint_t gen_64bit_range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb)
//! \brief internal function to get bit range in a qword (uint64_t)
//!
//! \param[in] input input genericint_t to get byte range from
//! \param[in] outputsize requested output size (sizeof(uint8_t), sizeof(uint16_t), etc)
//! \param[in] msb most significant bit offset value
//! \param[in] lsb least significant bit offset value
//! \return returns genericint_t in requested range
static M_INLINE genericint_t gen_64bit_range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb)
{
    genericint_t out;
    safe_memset(&out, sizeof(genericint_t), 0, sizeof(genericint_t));
    if (msb > GENERIC_INT_64BIT_MAX || lsb > GENERIC_INT_64BIT_MAX)
    {
        errno = ERANGE;
    }
    else
    {
        size_t bit_count = gen_bit_width(msb, lsb);

        if (outputsize == sizeof(uint8_t))
        {
            if (bit_count == GENERIC_WIDTH_0 || bit_count > GENERIC_WIDTH_8)
            {
                errno = ERANGE;
            }
            else
            {
                GEN_ASSIGN_OUT(uint8_t, u8, gen_extract_u64, input.u64);
            }
        }
        else if (outputsize == sizeof(uint16_t))
        {
            if (bit_count == GENERIC_WIDTH_0 || bit_count > GENERIC_WIDTH_16)
            {
                errno = ERANGE;
            }
            else
            {
                GEN_ASSIGN_OUT(uint16_t, u16, gen_extract_u64, input.u64);
            }
        }
        else if (outputsize == sizeof(uint32_t))
        {
            if (bit_count == GENERIC_WIDTH_0 || bit_count > GENERIC_WIDTH_32)
            {
                errno = ERANGE;
            }
            else
            {
                GEN_ASSIGN_OUT(uint32_t, u32, gen_extract_u64, input.u64);
            }
        }
        else /* default case where outputsize is not less than uint64 */
        {
            if (bit_count == GENERIC_WIDTH_0 || bit_count > GENERIC_WIDTH_64)
            {
                errno = ERANGE;
            }
            else
            {
                GEN_ASSIGN_OUT(uint64_t, u64, gen_extract_u64, input.u64);
            }
        }
    }
    return out;
}

genericint_t generic_Get_Bit_Range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb)
{
    genericint_t out;
    safe_memset(&out, sizeof(genericint_t), 0, sizeof(genericint_t));
    errno = 0; // clear out any errors first
    if (!is_generic_int_valid(input))
    {
        errno = EINVAL;
    }
    else if (msb > GENERIC_INT_MAX_BIT || lsb > GENERIC_INT_MAX_BIT)
    {
        errno = ERANGE;
    }
    else
    {
        switch (input.sizeoftype)
        {
        case sizeof(uint8_t):
            out = gen_8bit_range(input, outputsize, msb, lsb);
            break;
        case sizeof(uint16_t):
            out = gen_16bit_range(input, outputsize, msb, lsb);
            break;
        case sizeof(uint32_t):
            out = gen_32bit_range(input, outputsize, msb, lsb);
            break;
        case sizeof(uint64_t):
            out = gen_64bit_range(input, outputsize, msb, lsb);
            break;
        default:
            perror("Error in generic get bit range function");
            break;
        }
    }
    return out;
}
