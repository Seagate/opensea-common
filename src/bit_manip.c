// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file bit_manip.c
// \brief Implements various functions and macros to manipulate bit fields, manipulate bytes, swap bytes, etc

#include "bit_manip.h"
#include "type_conversion.h"
#include <math.h>

//MSVC has built-in byte swapping which may be more efficient here: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/byteswap-uint64-byteswap-ulong-byteswap-ushort?view=msvc-170
//GCC 4.8 and Clang 3.2 and later have some built-ins: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
//Otherwise rely on our own bit shifting.


void nibble_Swap(uint8_t* byteToSwap)
{
    *byteToSwap = C_CAST(uint8_t, ((*byteToSwap & UINT8_C(0x0F)) << 4)) | C_CAST(uint8_t, ((*byteToSwap & UINT8_C(0xF0)) >> 4));
}

void byte_Swap_16(uint16_t* wordToSwap)
{
    *wordToSwap = C_CAST(uint16_t, ((*wordToSwap & UINT16_C(0x00FF)) << 8)) | C_CAST(uint16_t, ((*wordToSwap & UINT16_C(0xFF00)) >> 8));
}

void byte_Swap_Int16(int16_t* signedWordToSwap)
{
    *signedWordToSwap = C_CAST(int16_t, ((*signedWordToSwap & UINT16_C(0x00FF)) << 8)) | C_CAST(int16_t, ((*signedWordToSwap & UINT16_C(0xFF00)) >> 8));
}

void big_To_Little_Endian_16(uint16_t* wordToSwap)
{
    if (get_Compiled_Endianness() == OPENSEA_LITTLE_ENDIAN)
    {
        byte_Swap_16(wordToSwap);
    }
}

void byte_Swap_32(uint32_t* doubleWordToSwap)
{
    *doubleWordToSwap = C_CAST(uint32_t, ((*doubleWordToSwap & UINT32_C(0x0000FFFF)) << 16)) | C_CAST(uint32_t, ((*doubleWordToSwap & UINT32_C(0xFFFF0000)) >> 16));
    *doubleWordToSwap = C_CAST(uint32_t, ((*doubleWordToSwap & UINT32_C(0x00FF00FF)) << 8)) | C_CAST(uint32_t, ((*doubleWordToSwap & UINT32_C(0xFF00FF00)) >> 8));
}

void byte_Swap_Int32(int32_t* signedDWord)
{
    // uint32_t temp = C_CAST(uint32_t, *signedDWord);
    // byte_Swap_32(&temp);
    // *signedDWord = C_CAST(int32_t, temp);

    *signedDWord = C_CAST(int32_t, ((*signedDWord & C_CAST(int32_t, INT32_C(0x0000FFFF))) << 16)) | C_CAST(int32_t, ((*signedDWord & C_CAST(int32_t, INT32_C(0xFFFF0000))) >> 16));
    *signedDWord = C_CAST(int32_t, ((*signedDWord & C_CAST(int32_t, INT32_C(0x00FF00FF))) << 8)) | C_CAST(int32_t, ((*signedDWord & C_CAST(int32_t, INT32_C(0xFF00FF00))) >> 8));
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
    *doubleWordToSwap = ((*doubleWordToSwap & UINT32_C(0x0000FFFF)) << 16) | ((*doubleWordToSwap & UINT32_C(0xFFFF0000)) >> 16);
}

void byte_Swap_64(uint64_t* quadWordToSwap)
{
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00000000FFFFFFFF)) << 32) | ((*quadWordToSwap & UINT64_C(0xFFFFFFFF00000000)) >> 32);
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x0000FFFF0000FFFF)) << 16) | ((*quadWordToSwap & UINT64_C(0xFFFF0000FFFF0000)) >> 16);
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00FF00FF00FF00FF)) << 8) | ((*quadWordToSwap & UINT64_C(0xFF00FF00FF00FF00)) >> 8);
}

void word_Swap_64(uint64_t* quadWordToSwap)
{
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00000000FFFFFFFF)) << 32) | ((*quadWordToSwap & UINT64_C(0xFFFFFFFF00000000)) >> 32);
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x0000FFFF0000FFFF)) << 16) | ((*quadWordToSwap & UINT64_C(0xFFFF0000FFFF0000)) >> 16);
}

void double_Word_Swap_64(uint64_t* quadWordToSwap)
{
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00000000FFFFFFFF)) << 32) | ((*quadWordToSwap & UINT64_C(0xFFFFFFFF00000000)) >> 32);
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
    if (dataPtrBeginning == M_NULLPTR || out == M_NULLPTR || msb > fullDataLen || lsb > fullDataLen || get_Bytes_Abs_Range(msb, lsb) > sizeof(uint64_t))
    {
        return false;
    }
    if (lsb <= msb)//allowing equals for single bytes
    {
        for (size_t iter = msb, counter = 0; counter < fullDataLen && counter < SIZE_MAX && iter >= lsb; --iter, ++counter)
        {
            (*out) <<= 8;
            (*out) |= dataPtrBeginning[iter];
            if (iter == 0)
            {
                //exit the loop to make sure there is no undefined behavior
                break;
            }
        }
    }
    else
    {
        //opposite byte ordering from above
        for (size_t iter = msb, counter = 0; counter < fullDataLen && counter < SIZE_MAX && iter <= lsb; ++iter, ++counter)
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
        uint64_t temp = 0;
        bool result = get_Bytes_To_64(dataPtrBeginning, fullDataLen, msb, lsb, &temp);
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
        uint64_t temp = 0;
        bool result = get_Bytes_To_64(dataPtrBeginning, fullDataLen, msb, lsb, &temp);
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
