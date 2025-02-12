// SPDX-License-Identifier: MPL-2.0

//! \file pattern_utils.h
//! \brief Implements various functions for filling patterns into user provided
//! buffers
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \fn eReturnValues fill_Random_Pattern_In_Buffer(uint8_t* ptrData, uint32_t dataLength)
    //! \brief Fills user provided buffer with random data
    //! \param[out] ptrData pointer to the databuffer to fill with random data
    //! \param[in] dataLength size of buffer pointer to by \a ptrData
    //! \return SUCCESS is successful. BAD_PARAMETER is invalid parameter value.
    M_NONNULL_PARAM_LIST(1)
    M_PARAM_WO_SIZE(1, 2) eReturnValues fill_Random_Pattern_In_Buffer(uint8_t* ptrData, uint32_t dataLength);

    //! \fn eReturnValues fill_Hex_Pattern_In_Buffer(uint32_t hexPattern, uint8_t* ptrData, uint32_t dataLength)
    //! \brief Fills user provided buffer with provided hexadecimal pattern
    //! to the entire buffer
    //! \param[in] hexPattern pattern to write to \a ptrData
    //! \param[out] ptrData pointer to the databuffer to fill
    //! \param[in] dataLength size of buffer pointer to by \a ptrData
    //! \return SUCCESS is successful. BAD_PARAMETER is invalid parameter value.
    M_NONNULL_PARAM_LIST(2)
    M_PARAM_WO_SIZE(2, 3)
    eReturnValues fill_Hex_Pattern_In_Buffer(uint32_t hexPattern, uint8_t* ptrData, uint32_t dataLength);

    //! \fn eReturnValues fill_Incrementing_Pattern_In_Buffer(uint8_t incrementStartValue, uint8_t* ptrData, uint32_t
    //! dataLength)
    //! \brief Fills user provided buffer with an incrementing pattern from a given start point
    //! \param[in] incrementStartValue starting value to use for incrementing pattern
    //! \param[out] ptrData pointer to the databuffer to fill
    //! \param[in] dataLength size of buffer pointer to by \a ptrData
    //! \return SUCCESS is successful.
    //! BAD_PARAMETER is invalid parameter value.
    M_NONNULL_PARAM_LIST(2)
    M_PARAM_WO_SIZE(2, 3)
    eReturnValues fill_Incrementing_Pattern_In_Buffer(uint8_t  incrementStartValue,
                                                      uint8_t* ptrData,
                                                      uint32_t dataLength);

    //! \fn eReturnValues fill_ASCII_Pattern_In_Buffer(const char* asciiPattern, uint32_t patternLength,
    //!                                                uint8_t* ptrData, uint32_t dataLength)
    //! \brief Fills user provided buffer with an ASCII pattern (string)
    //! \param[in] asciiPattern ASCII pattern to use when filling buffer
    //! \param[in] patternLength length of ASCII string to use. Allows for truncating input string if desired
    //! \param[out] ptrData pointer to the databuffer to fill
    //! \param[in] dataLength size of buffer pointer to by \a ptrData
    //! \return SUCCESS is successful. BAD_PARAMETER is invalid parameter value.
    M_NONNULL_PARAM_LIST(1, 3)
    M_PARAM_RO_SIZE(1, 2)
    M_PARAM_WO_SIZE(3, 4)
    eReturnValues fill_ASCII_Pattern_In_Buffer(const char* asciiPattern,
                                               uint32_t    patternLength,
                                               uint8_t*    ptrData,
                                               uint32_t    dataLength);

    //! \fn eReturnValues fill_Pattern_Buffer_Into_Another_Buffer(uint8_t* inPattern,
    //!                                                           uint32_t inpatternLength,
    //!                                                           uint8_t* ptrData,
    //!                                                           uint32_t dataLength)
    //! \brief   Fills a buffer from a pattern memory buffer and will
    //! repeat the pattern to fill the out buffer.
    //!
    //! This function allows for using a smaller buffer to fill a larger one.
    //! For example, if you have one buffer of 512B that you want to use as
    //! the source and a destination of 4096B, the source will be repeated
    //! 8 times to repeat the pattern and fill out the larger buffer
    //!
    //! \param[in] inPattern = pointer to a uint8 buffer
    //! \param[in] inpatternLength = length of the incoming pattern
    //! \param[out] ptrData = pointer to the data buffer to fill
    //! \param[in] dataLength = size of the data buffer in bytes.
    //! \return SUCCESS = successfully filled buffer. BAD_PARAMETER = error in
    //!   function parameters
    M_NONNULL_PARAM_LIST(1, 3)
    M_PARAM_RO_SIZE(1, 2)
    M_PARAM_WO_SIZE(3, 4)
    eReturnValues fill_Pattern_Buffer_Into_Another_Buffer(uint8_t* inPattern,
                                                          uint32_t inpatternLength,
                                                          uint8_t* ptrData,
                                                          uint32_t dataLength);

#if defined(__cplusplus)
}
#endif
