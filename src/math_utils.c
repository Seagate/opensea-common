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
#include <math.h>

double raise_to_power(double number, double power)
{
    return pow(number, power);
}
