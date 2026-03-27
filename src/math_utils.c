// SPDX-License-Identifier: MPL-2.0

//! \file math_utils.c
//! \brief Defines functions and macros for some basic math operations
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "math_utils.h"
#include "type_conversion.h"

double raise_to_power(double number, double power)
{
    double  result     = 1.0;
    int64_t localPower = C_CAST(int64_t, power);

    if (localPower == INT64_C(0))
    {
        return 1.0;
    }

    if (localPower > INT64_C(0))
    {
        // Positive exponent: multiply number by itself localPower times
        for (int64_t i = INT64_C(0); i < localPower; i++)
        {
            result = result * number;
        }
    }
    else
    {
        // Negative exponent: multiply (1.0/number) by itself |localPower| times
        if (number == 0.0)
        {
            return 0.0 / 0.0;  // Division by zero: return NaN
        }
        for (int64_t i = INT64_C(0); i < -localPower; i++)
        {
            result = result * (1.0 / number);
        }
    }

    return result;
}
