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
#include "code_attributes.h"

#if defined (__cplusplus)
extern "C"
{
#endif

    // Max & Min Helpers
    #define  M_Min(a,b)    (((a)<(b))?(a):(b))
    #define  M_Max(a,b)    (((a)>(b))?(a):(b))

    #define M_2sCOMPLEMENT(val) (~(val) + 1)

    //round an integer up to nearest multiple
    //NOTE: It is recommended to use the type specific inline versions below for type enforcement by the compiler
    //Example: Need to round up to nearest 512B for a data transfer
    #define INT_ROUND_UP(value, roundto) (((value) % (roundto)) ? ((value) + (roundto) - ((value) % (roundto))) : (value) )

    //round an integer down to nearest multiple
    #define INT_ROUND_DOWN(value, roundto) (((value) % (roundto)) ? ((value) - ((value) % (roundto))) : (value) )

    // Generic round up for any value of roundto
    static M_INLINE uint8_t uint8_round_up_generic(uint8_t value, uint8_t roundto)
    {
        return ((value % roundto) ? ((value + roundto - (value % roundto))) : (value));
    }

    static M_INLINE int8_t int8_round_up_generic(int8_t value, int8_t roundto)
    {
        return ((value % roundto) ? ((value + roundto - (value % roundto))) : (value));
    }

    static M_INLINE uint16_t uint16_round_up_generic(uint16_t value, uint16_t roundto)
    {
        return ((value % roundto) ? ((value + roundto - (value % roundto))) : (value));
    }

    static M_INLINE int16_t int16_round_up_generic(int16_t value, int16_t roundto)
    {
        return ((value % roundto) ? ((value + roundto - (value % roundto))) : (value));
    }

    static M_INLINE uint32_t uint32_round_up_generic(uint32_t value, uint32_t roundto)
    {
        return ((value % roundto) ? ((value + roundto - (value % roundto))) : (value));
    }

    static M_INLINE int32_t int32_round_up_generic(int32_t value, int32_t roundto)
    {
        return ((value % roundto) ? ((value + roundto - (value % roundto))) : (value));
    }

    static M_INLINE uint64_t uint64_round_up_generic(uint64_t value, uint64_t roundto)
    {
        return ((value % roundto) ? ((value + roundto - (value % roundto))) : (value));
    }

    static M_INLINE int64_t int64_round_up_generic(int64_t value, int64_t roundto)
    {
        return ((value % roundto) ? ((value + roundto - (value % roundto))) : (value));
    }

    // Generic round down for any value of roundto
    static M_INLINE uint8_t uint8_round_down_generic(uint8_t value, uint8_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    static M_INLINE int8_t int8_round_down_generic(int8_t value, int8_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    static M_INLINE uint16_t uint16_round_down_generic(uint16_t value, uint16_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    static M_INLINE int16_t int16_round_down_generic(int16_t value, int16_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    static M_INLINE uint32_t uint32_round_down_generic(uint32_t value, uint32_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    static M_INLINE int32_t int32_round_down_generic(int32_t value, int32_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    static M_INLINE uint64_t uint64_round_down_generic(uint64_t value, uint64_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    static M_INLINE int64_t int64_round_down_generic(int64_t value, int64_t roundto)
    {
        return ((value % roundto) ? ((value - (value % roundto))) : (value));
    }

    // Optimized round up for powers of two
    static M_INLINE uint8_t uint8_round_up_power2(uint8_t value, uint8_t roundto)
    {
        return (value + roundto - UINT8_C(1)) & ~(roundto - UINT8_C(1));
    }

    static M_INLINE int8_t int8_round_up_power2(int8_t value, int8_t roundto)
    {
        return (value + roundto - INT8_C(1)) & ~(roundto - INT8_C(1));
    }

    static M_INLINE uint16_t uint16_round_up_power2(uint16_t value, uint16_t roundto)
    {
        return (value + roundto - UINT16_C(1)) & ~(roundto - UINT16_C(1));
    }

    static M_INLINE int16_t int16_round_up_power2(int16_t value, int16_t roundto)
    {
        return (value + roundto - INT16_C(1)) & ~(roundto - INT16_C(1));
    }

    static M_INLINE uint32_t uint32_round_up_power2(uint32_t value, uint32_t roundto)
    {
        return (value + roundto - UINT32_C(1)) & ~(roundto - UINT32_C(1));
    }

    static M_INLINE int32_t int32_round_up_power2(int32_t value, int32_t roundto)
    {
        return (value + roundto - INT32_C(1)) & ~(roundto - INT32_C(1));
    }

    static M_INLINE uint64_t uint64_round_up_power2(uint64_t value, uint64_t roundto)
    {
        return (value + roundto - UINT64_C(1)) & ~(roundto - UINT64_C(1));
    }

    static M_INLINE int64_t int64_round_up_power2(int64_t value, int64_t roundto)
    {
        return (value + roundto - INT64_C(1)) & ~(roundto - INT64_C(1));
    }

    // Optimized round down for powers of two
    static M_INLINE uint8_t uint8_round_down_power2(uint8_t value, uint8_t roundto)
    {
        return value & ~(roundto - UINT8_C(1));
    }

    static M_INLINE int8_t int8_round_down_power2(int8_t value, int8_t roundto)
    {
        return value & ~(roundto - INT8_C(1));
    }

    static M_INLINE uint16_t uint16_round_down_power2(uint16_t value, uint16_t roundto)
    {
        return value & ~(roundto - UINT16_C(1));
    }

    static M_INLINE int16_t int16_round_down_power2(int16_t value, int16_t roundto)
    {
        return value & ~(roundto - INT16_C(1));
    }

    static M_INLINE uint32_t uint32_round_down_power2(uint32_t value, uint32_t roundto)
    {
        return value & ~(roundto - UINT32_C(1));
    }

    static M_INLINE int32_t int32_round_down_power2(int32_t value, int32_t roundto)
    {
        return value & ~(roundto - INT32_C(1));
    }

    static M_INLINE uint64_t uint64_round_down_power2(uint64_t value, uint64_t roundto)
    {
        return value & ~(roundto - UINT64_C(1));
    }

    static M_INLINE int64_t int64_round_down_power2(int64_t value, int64_t roundto)
    {
        return value & ~(roundto - INT64_C(1));
    }

    uint64_t power_Of_Two(uint16_t exponent);

    double raise_to_power(double number, double power);

#if defined (__cplusplus)
}
#endif
