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
// \file pattern_utils.h
// \brief Implements various functions for filling patterns into user provided buffers

#pragma once

#include "common_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

    //-----------------------------------------------------------------------------
    //
    //  fill_Random_Pattern_In_Buffer(uint8_t *ptrData, uint32_t dataLength)
    //
    //! \brief   Description:  Fills a buffer with random data. Data length must be at least sizeof(uint32_t) in size (4 bytes)
    //
    //  Entry:
    //!   \param[out] ptrData = pointer to the data buffer to fill
    //!   \param[in] dataLength = size of the data buffer in bytes (must be at least 4 bytes in size)
    //!
    //  Exit:
    //!   \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in function parameters
    //
    //-----------------------------------------------------------------------------
    eReturnValues fill_Random_Pattern_In_Buffer(uint8_t* ptrData, uint32_t dataLength);

    //-----------------------------------------------------------------------------
    //
    //  fill_Hex_Pattern_In_Buffer(uint32_t hexPattern, uint8_t *ptrData, uint32_t dataLength)
    //
    //! \brief   Description:  Fills a buffer with a hexadecimal pattern. The pattern is 32bits in length.
    //
    //  Entry:
    //!   \param[in] hexPattern = 32bit hexadecimal pattern to put into buffer.
    //!   \param[out] ptrData = pointer to the data buffer to fill
    //!   \param[in] dataLength = size of the data buffer in bytes (must be at least 4 bytes in size)
    //!
    //  Exit:
    //!   \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in function parameters
    //
    //-----------------------------------------------------------------------------
    eReturnValues fill_Hex_Pattern_In_Buffer(uint32_t hexPattern, uint8_t* ptrData, uint32_t dataLength);

    //-----------------------------------------------------------------------------
    //
    //  fill_Incrementing_Pattern_In_Buffer(uint8_t incrementStartValue, uint8_t *ptrData, uint32_t dataLength)
    //
    //! \brief   Description:  Fills a buffer with a incrementing pattern from a starting point
    //
    //  Entry:
    //!   \param[in] incrementStartValue = value to start incrementing from.
    //!   \param[out] ptrData = pointer to the data buffer to fill
    //!   \param[in] dataLength = size of the data buffer in bytes.
    //!
    //  Exit:
    //!   \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in function parameters
    //
    //-----------------------------------------------------------------------------
    eReturnValues fill_Incrementing_Pattern_In_Buffer(uint8_t incrementStartValue, uint8_t* ptrData, uint32_t dataLength);

    //-----------------------------------------------------------------------------
    //
    //  fill_ASCII_Pattern_In_Buffer(const char *asciiPattern, uint32_t patternLength, uint8_t *ptrData, uint32_t dataLength)
    //
    //! \brief   Description:  Fills a buffer with an ASCII pattern (string).
    //
    //  Entry:
    //!   \param[in] asciiPattern = pointer to ASCII pattern (string)
    //!   \param[in] patternLength = length of the ASCII pattern
    //!   \param[out] ptrData = pointer to the data buffer to fill
    //!   \param[in] dataLength = size of the data buffer in bytes.
    //!
    //  Exit:
    //!   \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in function parameters
    //
    //-----------------------------------------------------------------------------
    eReturnValues fill_ASCII_Pattern_In_Buffer(const char* asciiPattern, uint32_t patternLength, uint8_t* ptrData, uint32_t dataLength);

    //-----------------------------------------------------------------------------
    //
    //  fill_Pattern_Buffer_Into_Another_Buffer(uint8_t *inPattern, uint32_t inpatternLength, uint8_t *ptrData, uint32_t dataLength)
    //
    //! \brief   Description:  Fills a buffer from a pattern memory buffer and will repeat the pattern to fill the out buffer. (i.e., 1 sector in gets copied and put into all 4 sectors out)
    //
    //  Entry:
    //!   \param[in] inPattern = pointer to a uint8 buffer
    //!   \param[in] inpatternLength = length of the incomming pattern
    //!   \param[out] ptrData = pointer to the data buffer to fill
    //!   \param[in] dataLength = size of the data buffer in bytes.
    //!
    //  Exit:
    //!   \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in function parameters
    //
    //-----------------------------------------------------------------------------
    eReturnValues fill_Pattern_Buffer_Into_Another_Buffer(uint8_t* inPattern, uint32_t inpatternLength, uint8_t* ptrData, uint32_t dataLength);

#if defined (__cplusplus)
}
#endif
