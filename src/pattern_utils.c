// SPDX-License-Identifier: MPL-2.0

//! \file pattern_utils.c
//! \brief Implements various functions for filling patterns into user provided
//! buffers
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "pattern_utils.h"
#include "math_utils.h"
#include "memory_safety.h"
#include "prng.h"
#include "string_utils.h"
#include "type_conversion.h"

#include <stdlib.h>
#include <string.h>

eReturnValues fill_Random_Pattern_In_Buffer(uint8_t* ptrData, uint32_t dataLength)
{
    size_t localPtrDataLen =
        ((dataLength + sizeof(uint32_t)) - 1) / sizeof(uint32_t); // round up to nearest uint32 amount
    uint32_t* localPtr = M_REINTERPRET_CAST(uint32_t*, safe_calloc(localPtrDataLen, sizeof(uint32_t)));
    if (localPtr == M_NULLPTR)
    {
        return MEMORY_FAILURE;
    }
    seed_32(C_CAST(uint32_t, time(M_NULLPTR)));
    for (uint32_t iter = UINT32_C(0); iter < (dataLength / sizeof(uint32_t)); ++iter)
    {
        localPtr[iter] = xorshiftplus32();
    }
    safe_memcpy(ptrData, dataLength, localPtr,
                dataLength); // copy only the length of the original pointer since we
                             // may have overallocated and rounded up earlier.
    safe_free(&localPtr);
    return SUCCESS;
}

eReturnValues fill_Hex_Pattern_In_Buffer(uint32_t hexPattern, uint8_t* ptrData, uint32_t dataLength)
{
    size_t localPtrDataLen =
        ((dataLength + sizeof(uint32_t)) - 1) / sizeof(uint32_t); // round up to nearest uint32 amount
    uint32_t* localPtr = M_REINTERPRET_CAST(uint32_t*, safe_calloc(localPtrDataLen, sizeof(uint32_t)));
    if (localPtr == M_NULLPTR)
    {
        return MEMORY_FAILURE;
    }
    for (uint32_t iter = UINT32_C(0); iter < (dataLength / sizeof(uint32_t)); ++iter)
    {
        localPtr[iter] = hexPattern;
    }
    safe_memcpy(ptrData, dataLength, localPtr,
                dataLength); // copy only the length of the original pointer since we
                             // may have overallocated and rounded up earlier.
    safe_free(&localPtr);
    return SUCCESS;
}

eReturnValues fill_Incrementing_Pattern_In_Buffer(uint8_t incrementStartValue, uint8_t* ptrData, uint32_t dataLength)
{
    DISABLE_NONNULL_COMPARE
    if (ptrData == M_NULLPTR)
    {
        return BAD_PARAMETER;
    }
    RESTORE_NONNULL_COMPARE
    for (uint32_t iter = UINT32_C(0); iter < dataLength; ++iter)
    {
        ptrData[iter] = incrementStartValue++;
    }
    return SUCCESS;
}

eReturnValues fill_ASCII_Pattern_In_Buffer(const char* asciiPattern,
                                           uint32_t    patternLength,
                                           uint8_t*    ptrData,
                                           uint32_t    dataLength)
{
    DISABLE_NONNULL_COMPARE
    if (ptrData == M_NULLPTR || patternLength == 0 || asciiPattern == M_NULLPTR)
    {
        return BAD_PARAMETER;
    }
    RESTORE_NONNULL_COMPARE
    for (uint32_t iter = UINT32_C(0); iter < dataLength; iter += patternLength)
    {
        safe_memcpy(&ptrData[iter], dataLength, asciiPattern, M_Min(patternLength, dataLength - iter));
    }
    return SUCCESS;
}

eReturnValues fill_Pattern_Buffer_Into_Another_Buffer(uint8_t* inPattern,
                                                      uint32_t inpatternLength,
                                                      uint8_t* ptrData,
                                                      uint32_t dataLength)
{
    DISABLE_NONNULL_COMPARE
    if (ptrData == M_NULLPTR || inpatternLength == 0 || inPattern == M_NULLPTR || dataLength == 0)
    {
        return BAD_PARAMETER;
    }
    RESTORE_NONNULL_COMPARE
    for (uint32_t iter = UINT32_C(0); iter < dataLength; iter += inpatternLength)
    {
        safe_memcpy(&ptrData[iter], dataLength, inPattern, M_Min(inpatternLength, dataLength - iter));
    }
    return SUCCESS;
}
