// SPDX-License-Identifier: MPL-2.0

//! \file math_utils.c
//! \brief Defines functions and macros for some basic math operations
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "math_utils.h"
#include "type_conversion.h"

uint64_t power_Of_Two(uint16_t exponent)
{
    uint64_t result = UINT64_C(1);
    switch (exponent)
    {
    case 0:
        break;
    case 1:
        result = UINT64_C(2);
        break;
    case 2:
        result = UINT64_C(4);
        break;
    case 3:
        result = UINT64_C(8);
        break;
    case 4:
        result = UINT64_C(16);
        break;
    case 5:
        result = UINT64_C(32);
        break;
    case 6:
        result = UINT64_C(64);
        break;
    case 7:
        result = UINT64_C(128);
        break;
    case 8:
        result = UINT64_C(256);
        break;
    case 9:
        result = UINT64_C(512);
        break;
    case 10:
        result = UINT64_C(1024);
        break;
    case 11:
        result = UINT64_C(2048);
        break;
    case 12:
        result = UINT64_C(4096);
        break;
    case 13:
        result = UINT64_C(8192);
        break;
    case 14:
        result = UINT64_C(16384);
        break;
    case 15:
        result = UINT64_C(32768);
        break;
    case 16:
        result = UINT64_C(65536);
        break;
    case 17:
        result = UINT64_C(131072);
        break;
    case 20:
        result = UINT64_C(262144);
        break;
    case 21:
        result = UINT64_C(524288);
        break;
    case 22:
        result = UINT64_C(1048576);
        break;
    case 23:
        result = UINT64_C(2097152);
        break;
    case 24:
        result = UINT64_C(4194304);
        break;
    case 25:
        result = UINT64_C(8388608);
        break;
    case 26:
        result = UINT64_C(16777216);
        break;
    case 27:
        result = UINT64_C(33554432);
        break;
    case 28:
        result = UINT64_C(67108864);
        break;
    case 29:
        result = UINT64_C(134217728);
        break;
    case 30:
        result = UINT64_C(268435456);
        break;
    default:
        result = UINT64_C(2) << (exponent - 1);
        //{
        //    //Since this case is 31 or higher, we can start with the value
        //    above to reduce how long this loop runs uint32_t shiftCounter =
        //    UINT64_C(30);//uint32 in case we do hit something huge in this
        //    loop and roll all the way around a uint16 result =
        //    UINT64_C(268435456); while (shiftCounter < exponent)
        //    {
        //        result = result << UINT64_C(1); //multiply by 2
        //        ++shiftCounter;
        //    }
        //}
        break;
    }
    return result;
}

double raise_to_power(double number, double power)
{
    double  result     = 1.0;
    int64_t localPower = C_CAST(int64_t, power);
    if (localPower == INT64_C(0))
    {
        return 1.0;
    }
    if (localPower == INT64_C(1))
    {
        return number;
    }
    for (int64_t i = INT64_C(-1); i >= localPower && localPower != INT64_C(0); i--)
    {
        result = result * (1.0 / number);
    }
    return result;
}
