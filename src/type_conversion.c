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
// \file type_conversion.c
// \brief Defines cast macros and conversion functions for more safely
// converting between types.
//
#include "type_conversion.h"
#include "bit_manip.h"
#include <math.h>

#if !defined(SIZE_MAX) // this should come from the system or at least defined
                       // with all the types
#    error "SIZE_MAX not defined! Please add a definition for this system!"
#endif

size_t int8_to_sizet(int8_t val)
{
    if (val < 0)
    {
        return 0;
    }
#if defined(SIZE_MAX) && defined(INT8_MAX) && SIZE_MAX >= INT8_MAX
    else
    {
        return C_CAST(size_t, val);
    }
#else // SIZE_MAX < INT8_MAX
    else if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t uint8_to_sizet(uint8_t val)
{
#if defined(SIZE_MAX) && defined(UINT8_MAX) && SIZE_MAX >= UINT8_MAX
    return C_CAST(size_t, val);
#else // SIZE_MAX < UINT8_MAX
    if (val <= SIZE_MAX)
    {
        return val;
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t int16_to_sizet(int16_t val)
{
    if (val < 0)
    {
        return 0;
    }
#if defined(SIZE_MAX) && defined(INT16_MAX) && SIZE_MAX >= INT16_MAX
    else
    {
        return C_CAST(size_t, val);
    }
#else // SIZE_MAX < INT8_MAX
    else if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t uint16_to_sizet(uint16_t val)
{
#if defined(SIZE_MAX) && defined(UINT16_MAX) && SIZE_MAX >= UINT16_MAX
    return C_CAST(size_t, val);
#else // SIZE_MAX < UINT16_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t int32_to_sizet(int32_t val)
{
    if (val < 0)
    {
        return 0;
    }
#if defined(SIZE_MAX) && defined(INT32_MAX) && SIZE_MAX >= INT32_MAX
    else
    {
        return C_CAST(size_t, val);
    }
#else // SIZE_MAX < INT32_MAX
    else if (C_CAST(uint32_t, val) <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t uint32_to_sizet(uint32_t val)
{
#if defined(SIZE_MAX) && defined(UINT32_MAX) && SIZE_MAX >= UINT32_MAX
    return C_CAST(size_t, val);
#else // SIZE_MAX < UINT32_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t int64_to_sizet(int64_t val)
{
    if (val < 0)
    {
        return 0;
    }
#if defined(SIZE_MAX) && defined(INT64_MAX) && SIZE_MAX >= INT64_MAX
    else
    {
        return C_CAST(size_t, val);
    }
#else // SIZE_MAX < INT64_MAX
    else if (C_CAST(uint64_t, val) <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t uint64_to_sizet(uint64_t val)
{
#if defined(SIZE_MAX) && defined(UINT64_MAX) && SIZE_MAX >= UINT64_MAX
    return C_CAST(size_t, val);
#else // SIZE_MAX < UINT64_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t char_to_sizet(char val)
{
#if defined(CHAR_MIN) && CHAR_MIN < 0
    if (val < 0)
    {
        return 0;
    }
#endif // CHAR_MIN < 0
#if defined(SIZE_MAX) && defined(CHAR_MAX) && SIZE_MAX >= CHAR_MAX
    {
        return C_CAST(size_t, val);
    }
#else // SIZE_MAX < CHAR_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t schar_to_sizet(signed char val)
{
#if defined(SCHAR_MIN) && SCHAR_MIN < 0
    if (val < 0)
    {
        return 0;
    }
#endif // CHAR_MIN < 0
#if defined(SIZE_MAX) && defined(SCHAR_MAX) && SIZE_MAX >= SCHAR_MAX
    {
        return C_CAST(size_t, val);
    }
#else // SIZE_MAX < CHAR_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t uchar_to_sizet(unsigned char val)
{
#if defined(SIZE_MAX) && defined(UCHAR_MAX) && SIZE_MAX >= UCHAR_MAX
    return C_CAST(size_t, val);
#else // SIZE_MAX < UCHAR_MAX
    if (val <= SIZE_MAX)
    {
        return val;
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t short_to_sizet(short val)
{
    if (val < 0)
    {
        return 0;
    }
#if defined(SIZE_MAX) && defined(SHRT_MAX) && SIZE_MAX >= SHRT_MAX
    else
    {
        return C_CAST(size_t, val);
    }
#else // SIZE_MAX < SHRT_MAX
    else if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t ushort_to_sizet(unsigned short val)
{
#if defined(SIZE_MAX) && defined(USHRT_MAX) && SIZE_MAX >= USHRT_MAX
    return C_CAST(size_t, val);
#else // SIZE_MAX < USHRT_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t int_to_sizet(int val)
{
    if (val < 0)
    {
        return 0;
    }
#if defined(SIZE_MAX) && defined(INT_MAX) && SIZE_MAX >= INT_MAX
    else
    {
        return C_CAST(size_t, val);
    }
#else // SIZE_MAX < INT_MAX
    else if (C_CAST(unsigned int, val) <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t uint_to_sizet(unsigned int val)
{
#if defined(SIZE_MAX) && defined(UINT_MAX) && SIZE_MAX >= UINT_MAX
    return C_CAST(size_t, val);
#else // SIZE_MAX < UINT_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t long_to_sizet(long val)
{
    if (val < 0)
    {
        return 0;
    }
#if defined(SIZE_MAX) && defined(LONG_MAX) && SIZE_MAX >= LONG_MAX
    else
    {
        return C_CAST(size_t, val);
    }
#else // SIZE_MAX < LONG_MAX
    else if (C_CAST(unsigned long, val) <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t ulong_to_sizet(unsigned long val)
{
#if defined(SIZE_MAX) && defined(ULONG_MAX) && SIZE_MAX >= ULONG_MAX
    return C_CAST(size_t, val);
#else // SIZE_MAX < ULONG_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t longlong_to_sizet(long long val)
{
    if (val < 0)
    {
        return 0;
    }
#if defined(SIZE_MAX) && defined(LLONG_MAX) && SIZE_MAX >= LLONG_MAX
    else
    {
        return C_CAST(size_t, val);
    }
#else // SIZE_MAX < LLONG_MAX
    else if (C_CAST(unsigned long long, val) <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

size_t ulonglong_to_sizet(unsigned long long val)
{
#if defined(SIZE_MAX) && defined(ULLONG_MAX) && SIZE_MAX >= ULLONG_MAX
    return C_CAST(size_t, val);
#else // SIZE_MAX < ULLONG_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
#endif
}

bool is_size_t_max(size_t val)
{
    if (val == SIZE_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void get_Decimal_From_4_byte_Float(uint32_t floatValue, double* decimalValue)
{
    int32_t exponent = M_STATIC_CAST(int32_t, get_8bit_range_uint32(floatValue, 30, 23) - 127);
    double  sign =
        pow(-1.0, M_STATIC_CAST(double, floatValue >> 31)); // shift sign bit to bit 0. Same as getbitrange 31, 31

    int8_t power   = -23;
    double mantisa = 1.0;
    for (uint8_t i = UINT8_C(0); i < 23; i++)
    {
        mantisa += M_STATIC_CAST(double, floatValue& BIT0) * pow(2.0, power);
        power++;
        floatValue >>= 1; // right shift to look at next bit next time through the loop
    }

    *decimalValue = sign * pow(2.0, exponent) * mantisa;
}

double convert_128bit_to_double(const uint8_t* pData)
{
    double result = 0.0;
    int    i      = 0;
    for (i = 0; i < 16; i++)
    {
        result *= 256.0;
        result += pData[15 - i];
    }
    return result;
}
