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
// \file prng.c
// \brief Implements psuedo random number generation functionality. Currently it is implemented using XOR shift+

#include "prng.h"
#include "common_types.h"
#include "type_conversion.h"

uint32_t seed32Array[2] = { UINT32_C(0x05EAF00D), UINT32_C(0x05EA51DE) };
uint64_t seed64Array[2] = { UINT64_C(0x05EAF00D05EA51DE), UINT64_C(0x05EA51DE05EAF00D) };

void seed_32(uint32_t seed)
{
    //first initialize
    seed32Array[0] = seed;
    seed32Array[1] = C_CAST(uint32_t, C_CAST(int32_t, seed) >> 1);//converting to signed int to perform arithmetic shift, then back for the seed value
    //using that initialization, run the random number generator for a more random seed...may or may not be needed, but I'm doing this anyways - Tyler
    seed32Array[0] = xorshiftplus32();
    seed32Array[0] = xorshiftplus32();
}
void seed_64(uint64_t seed)
{
    //first initialize
    seed64Array[0] = seed;
    seed64Array[1] = C_CAST(uint64_t, C_CAST(int64_t, seed) >> 2);//converting to signed int to perform arithmetic shift, then back for the seed value
    //using that initialization, run the random number generator for a more random seed...may or may not be needed, but I'm doing this anyways - Tyler
    seed64Array[0] = xorshiftplus64();
    seed64Array[0] = xorshiftplus64();
}

//shift values chosen randomly
uint32_t xorshiftplus32(void)
{
    uint32_t x = seed32Array[0];
    uint32_t const y = seed32Array[1];
    seed32Array[0] = y;
    x ^= x << 13;//a
    seed32Array[1] = x ^ y ^ (x >> 17) ^ (y >> 7); //b, c
    return (seed32Array[1] + y);
}

//shift values chosen randomly
uint64_t xorshiftplus64(void)
{
    uint64_t x = seed64Array[0];
    uint64_t const y = seed64Array[1];
    seed64Array[0] = y;
    x ^= x << 27;//a
    seed64Array[1] = x ^ y ^ (x >> 13) ^ (y >> 32); //b, c
    return (seed64Array[1] + y);
}

uint32_t random_Range_32(uint32_t rangeMin, uint32_t rangeMax)
{
    //doing this to prevent a possible overflow
    if (rangeMax == UINT32_MAX)
    {
        rangeMax -= 1;
    }
    //this will slightly bias the randomness.
    //return (xorshiftplus32() % (rangeMax + 1 - rangeMin) + rangeMin);

    //This method below should return unbiased results. see http://c-faq.com/lib/randrange.html
    uint32_t d = (UINT32_MAX / (rangeMax - rangeMin + 1) + 1);
    if (d > 0)
    {
        return (rangeMin + xorshiftplus32() / d);
    }
    else
    {
        return 0;
    }
}

uint64_t random_Range_64(uint64_t rangeMin, uint64_t rangeMax)
{
    //doing this to prevent a possible overflow
    if (rangeMax == UINT64_MAX)
    {
        rangeMax -= 1;
    }
    //this will slightly bias the randomness.
    //return (xorshiftplus64() % (rangeMax + 1 - rangeMin) + rangeMin);

    //This method below should return unbiased results. see http://c-faq.com/lib/randrange.html
    uint64_t d = (UINT64_MAX / (rangeMax - rangeMin + 1) + 1);
    if (d > 0)
    {
        return (rangeMin + xorshiftplus64() / d);
    }
    else
    {
        return 0;
    }
}
