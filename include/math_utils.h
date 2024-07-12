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
// \file math_utils.h
// \brief Defines functions and macros for some basic math operations
//
#pragma once

#include "predef_env_detect.h"
#include "common_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

    // Max & Min Helpers
    #define  M_Min(a,b)    (((a)<(b))?(a):(b))
    #define  M_Max(a,b)    (((a)>(b))?(a):(b))

    #define M_2sCOMPLEMENT(val) (~(val) + 1)

    //round an integer up to nearest multiple
    //Example: Need to round up to nearest 512B for a data transfer
    #define INT_ROUND_UP(value, roundto) (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value) )

    //round an integer down to nearest multiple
    #define INT_ROUND_DOWN(value, roundto) (((value) % (roundto)) ? ((value) - ((value) % (roundto))) : (value) )

    uint64_t power_Of_Two(uint16_t exponent);

    double raise_to_power(double number, double power);

#if defined (__cplusplus)
}
#endif
