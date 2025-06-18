// SPDX-License-Identifier: MPL-2.0

//! \file bit_manip.c
//! \brief Implements various functions and macros to manipulate bit fields,
//! manipulate bytes, swap bytes, etc
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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
    if (lsb <= msb) // allowing equals for single bytes
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
        // output size is not used in this case since a uint8_t can only
        // ever output a uint8_t
        out.sizeoftype = sizeof(uint8_t);
        out.u8 =
            ((input.u8) >> (lsb)) & ~(~UINT32_C(0) << ((msb) - (lsb) + UINT8_C(1))); // starting with UINT32_C to
                                                                                     // resolve warning about signed int
                                                                                     // since UINT8_C() does not apply U
        out.issigned = input.issigned;
    }
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
    else if (outputsize == sizeof(uint8_t))
    {
        if ((msb - lsb + 1) > 8)
        {
            errno = ERANGE;
        }
        else
        {
            out.sizeoftype = sizeof(uint8_t);
            out.u8         = C_CAST(uint8_t,
                                    ((input.u16) >> (lsb)) &
                                        ~(~UINT32_C(0) << ((msb) - (lsb) + UINT8_C(1)))); // starting with UINT32_C to
                                                                                          // resolve warning about
                                                                                          // signed int since UINT16_C()
                                                                                          // does not apply U
            out.issigned = input.issigned;
        }
    }
    else // default case where outputsize is not less than uint16
    {
        out.sizeoftype = sizeof(uint16_t);
        out.u16        = ((input.u16) >> (lsb)) &
                  ~(~UINT32_C(0) << ((msb) - (lsb) + UINT8_C(1))); // starting with UINT32_C to resolve
                                                                   // warning about signed int since
                                                                   // UINT16_C() does not apply U
        out.issigned = input.issigned;
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
    else if (outputsize == sizeof(uint8_t))
    {
        if ((msb - lsb + 1) > 8)
        {
            errno = ERANGE;
        }
        else
        {
            out.sizeoftype = sizeof(uint8_t);
            out.u8         = C_CAST(uint8_t,
                                    ((input.u32) >> (lsb)) &
                                        ~(~UINT32_C(0) << ((msb) - (lsb) + UINT8_C(1)))); // starting with UINT32_C to
                                                                                          // resolve warning about
                                                                                          // signed int since UINT16_C()
                                                                                          // does not apply U
            out.issigned = input.issigned;
        }
    }
    else if (outputsize == sizeof(uint16_t))
    {
        if ((msb - lsb + 1) > 16)
        {
            errno = ERANGE;
        }
        else
        {
            out.sizeoftype = sizeof(uint16_t);
            out.u16        = C_CAST(uint16_t,
                                    ((input.u32) >> (lsb)) &
                                        ~(~UINT32_C(0) << ((msb) - (lsb) + UINT8_C(1)))); // starting with UINT32_C to
                                                                                   // resolve warning about signed
                                                                                   // int since UINT16_C() does
                                                                                   // not apply U
            out.issigned = input.issigned;
        }
    }
    else // default case where outputsize is not less than uint32
    {
        out.sizeoftype = sizeof(uint16_t);
        out.u32        = ((input.u32) >> (lsb)) & ~(~UINT32_C(0) << ((msb) - (lsb) + UINT8_C(1)));
        out.issigned   = input.issigned;
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
    else if (outputsize == sizeof(uint8_t))
    {
        if ((msb - lsb + 1) > 8)
        {
            errno = ERANGE;
        }
        else
        {
            out.sizeoftype = sizeof(uint8_t);
            out.u8         = ((input.u64) >> (lsb)) &
                     ~(~UINT64_C(0) << ((msb) - (lsb) + UINT8_C(1))); // starting with UINT32_C to resolve
                                                                      // warning about signed int since
                                                                      // UINT16_C() does not apply U
            out.issigned = input.issigned;
        }
    }
    else if (outputsize == sizeof(uint16_t))
    {
        if ((msb - lsb + 1) > 16)
        {
            errno = ERANGE;
        }
        else
        {
            out.sizeoftype = sizeof(uint16_t);
            out.u16        = ((input.u64) >> (lsb)) &
                      ~(~UINT64_C(0) << ((msb) - (lsb) + UINT8_C(1))); // starting with UINT32_C to resolve
                                                                       // warning about signed int since
                                                                       // UINT16_C() does not apply U
            out.issigned = input.issigned;
        }
    }
    else if (outputsize == sizeof(uint32_t))
    {
        if ((msb - lsb + 1) > 32)
        {
            errno = ERANGE;
        }
        else
        {
            out.sizeoftype = sizeof(uint16_t);
            out.u32        = ((input.u64) >> (lsb)) &
                      ~(~UINT64_C(0) << ((msb) - (lsb) + UINT8_C(1))); // starting with UINT32_C to resolve
                                                                       // warning about signed int since
                                                                       // UINT16_C() does not apply U
            out.issigned = input.issigned;
        }
    }
    else // default case where outputsize is not less than uint64
    {
        out.sizeoftype = sizeof(uint64_t);
        out.u64        = ((input.u64) >> (lsb)) & ~(~UINT64_C(0) << ((msb) - (lsb) + UINT8_C(1)));
        out.issigned   = input.issigned;
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
