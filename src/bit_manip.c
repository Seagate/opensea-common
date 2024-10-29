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
// \file bit_manip.c
// \brief Implements various functions and macros to manipulate bit fields,
// manipulate bytes, swap bytes, etc

#include "bit_manip.h"
#include "type_conversion.h"
#include <math.h>
#include <stdlib.h>

// MSVC has built-in byte swapping which may be more efficient here:
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/byteswap-uint64-byteswap-ulong-byteswap-ushort?view=msvc-170
// GCC 4.8 and Clang 3.2 and later have some built-ins:
// https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html Otherwise rely on our
// own bit shifting.

#if defined(_MSC_VER) && !defined(HAVE_WIN_BSWAP)
// Unknown if added to specific _MSC_VER.
// Will need to add this check if we ever run into an issue with this -TJE
#define HAVE_WIN_BSWAP
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)) && !defined(HAVE_BUILTIN_BSWAP)
#define HAVE_BUILTIN_BSWAP
#elif !defined(HAVE_BUILTIN_BSWAP) && defined(__has_builtin)
#if __has_builtin(__builtin_bswap64)
#define HAVE_BUILTIN_BSWAP
#endif
#endif

void nibble_Swap(uint8_t* byteToSwap)
{
    if (byteToSwap)
    {
        *byteToSwap = C_CAST(uint8_t, ((*byteToSwap & UINT8_C(0x0F)) << 4)) |
                      C_CAST(uint8_t, ((*byteToSwap & UINT8_C(0xF0)) >> 4));
    }
}

static uint16_t byteswap_uint16(uint16_t word)
{
#if defined(HAVE_BUILTIN_BSWAP)
    return __builtin_bswap16(word);
#elif defined(HAVE_WIN_BSWAP)
    return _byteswap_ushort(word);
#else
    return (((word & UINT16_C(0x00FF)) << 8) | ((word & UINT16_C(0xFF00)) >> 8));
#endif
}

void byte_Swap_16(uint16_t* wordToSwap)
{
    if (*wordToSwap)
    {
        *wordToSwap = byteswap_uint16(*wordToSwap);
    }
}

void byte_Swap_Int16(int16_t* signedWordToSwap)
{
    if (signedWordToSwap)
    {
        *signedWordToSwap = C_CAST(int16_t, ((*signedWordToSwap & UINT16_C(0x00FF)) << 8)) |
                            C_CAST(int16_t, ((*signedWordToSwap & UINT16_C(0xFF00)) >> 8));
    }
}

void big_To_Little_Endian_16(uint16_t* wordToSwap)
{
    if (get_Compiled_Endianness() == OPENSEA_LITTLE_ENDIAN)
    {
        byte_Swap_16(wordToSwap);
    }
}

static uint32_t byteswap_uint32(uint32_t dword)
{
#if defined(HAVE_BUILTIN_BSWAP)
    return __builtin_bswap32(dword);
#elif defined(HAVE_WIN_BSWAP) && defined(UINT_MAX) && defined(ULONG_MAX) && UINT_MAX == ULONG_MAX
    // Windows defines uint32 as unsigned int, not unsigned long as this
    // function expects. So we have a more elaborate check in order to make sure
    // these have the same range before casting and returning that result It is
    // unlikely this will change in Windows, but it does not hurt to be safe and
    // verify everything before casting types that can de defined as different
    // widths-TJE
    return C_CAST(uint32_t, _byteswap_ulong(C_CAST(unsigned long, dword)));
#else
    return ((dword & UINT32_C(0xFF000000) >> 24) | (dword & UINT32_C(0x00FF0000) >> 8) |
            (dword & UINT32_C(0x0000FF00) << 8) | (dword & UINT32_C(0x000000FF) << 24));
#endif
}

void byte_Swap_32(uint32_t* doubleWordToSwap)
{
    if (doubleWordToSwap)
    {
        *doubleWordToSwap = byteswap_uint32(*doubleWordToSwap);
    }
}

void byte_Swap_Int32(int32_t* signedDWord)
{
    if (signedDWord)
    {
        // uint32_t temp = C_CAST(uint32_t, *signedDWord);
        // byte_Swap_32(&temp);
        // *signedDWord = C_CAST(int32_t, temp);

        *signedDWord = C_CAST(int32_t, ((*signedDWord & C_CAST(int32_t, INT32_C(0x0000FFFF))) << 16)) |
                       C_CAST(int32_t, ((*signedDWord & C_CAST(int32_t, INT32_C(0xFFFF0000))) >> 16));
        *signedDWord = C_CAST(int32_t, ((*signedDWord & C_CAST(int32_t, INT32_C(0x00FF00FF))) << 8)) |
                       C_CAST(int32_t, ((*signedDWord & C_CAST(int32_t, INT32_C(0xFF00FF00))) >> 8));
    }
}
void big_To_Little_Endian_32(uint32_t* doubleWordToSwap)
{
    if (get_Compiled_Endianness() == OPENSEA_LITTLE_ENDIAN)
    {
        byte_Swap_32(doubleWordToSwap);
    }
}

void word_Swap_32(uint32_t* doubleWordToSwap)
{
    if (doubleWordToSwap)
    {
        *doubleWordToSwap =
            ((*doubleWordToSwap & UINT32_C(0x0000FFFF)) << 16) | ((*doubleWordToSwap & UINT32_C(0xFFFF0000)) >> 16);
    }
}

static uint64_t byteswap_uint64(uint64_t qword)
{
#if defined(HAVE_BUILTIN_BSWAP)
    return __builtin_bswap64(qword);
#elif defined(HAVE_WIN_BSWAP)
    return _byteswap_uint64(qword);
#else
    return ((qword & UINT64_C(0xFF00000000000000) >> 56) | (qword & UINT64_C(0x00FF000000000000) >> 40) |
            (qword & UINT64_C(0x0000FF0000000000) >> 24) | (qword & UINT64_C(0x000000FF00000000) >> 8) |
            (qword & UINT64_C(0x00000000FF000000) << 8) | (qword & UINT64_C(0x0000000000FF0000) << 24) |
            (qword & UINT64_C(0x000000000000FF00) << 40) | (qword & UINT64_C(0x00000000000000FF) << 56));
#endif
}

void byte_Swap_64(uint64_t* quadWordToSwap)
{
    if (quadWordToSwap)
    {
        *quadWordToSwap = byteswap_uint64(*quadWordToSwap);
    }
}

void word_Swap_64(uint64_t* quadWordToSwap)
{
    if (quadWordToSwap)
    {
        *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00000000FFFFFFFF)) << 32) |
                          ((*quadWordToSwap & UINT64_C(0xFFFFFFFF00000000)) >> 32);
        *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x0000FFFF0000FFFF)) << 16) |
                          ((*quadWordToSwap & UINT64_C(0xFFFF0000FFFF0000)) >> 16);
    }
}

void double_Word_Swap_64(uint64_t* quadWordToSwap)
{
    if (quadWordToSwap)
    {
        *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00000000FFFFFFFF)) << 32) |
                          ((*quadWordToSwap & UINT64_C(0xFFFFFFFF00000000)) >> 32);
    }
}

static size_t get_Bytes_Abs_Range(size_t msb, size_t lsb)
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

bool get_Bytes_To_64(uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint64_t* out)
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

bool get_Bytes_To_32(uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint32_t* out)
{
    if (out && get_Bytes_Abs_Range(msb, lsb) <= sizeof(uint32_t))
    {
        uint64_t temp   = 0;
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

bool get_Bytes_To_16(uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint16_t* out)
{
    if (out && get_Bytes_Abs_Range(msb, lsb) <= sizeof(uint16_t))
    {
        uint64_t temp   = 0;
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

genericint_t generic_Get_Bit_Range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb)
{
    genericint_t out;
    safe_memset(&out, sizeof(genericint_t), 0, sizeof(genericint_t));
    errno = 0; // clear out any errors first
    if (msb > 63 || lsb > 63)
    {
        errno = ERANGE;
    }
    else if (!is_generic_int_valid(input))
    {
        errno = EINVAL;
    }
    else
    {
        switch (input.sizeoftype)
        {
        case sizeof(uint8_t):
            if (msb > 7 || lsb > 7)
            {
                errno = ERANGE;
            }
            else
            {
                // output size is not used in this case since a uint8_t can only
                // ever output a uint8_t
                out.sizeoftype = sizeof(uint8_t);
                out.u8         = ((input.u8) >> (lsb)) &
                         ~(~UINT32_C(0) << ((msb) - (lsb) + UINT8_C(1))); // starting with UINT32_C to
                                                                          // resolve warning about signed int
                                                                          // since UINT8_C() does not apply U
                out.issigned = input.issigned;
            }
            break;
        case sizeof(uint16_t):
            if (msb > 15 || lsb > 15)
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
            break;
        case sizeof(uint32_t):
            if (msb > 31 || lsb > 31)
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
                    out.u16 =
                        C_CAST(uint16_t,
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
            break;
        case sizeof(uint64_t):
            if (msb > 63 || lsb > 63)
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
            break;
        }
    }
    return out;
}
