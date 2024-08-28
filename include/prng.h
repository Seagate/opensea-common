// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file prng.h
// \brief Implements psuedo random number generation functionality. Currently it is implemented using XOR shift+

#pragma once

#include <time.h>
#include "common_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

    extern uint32_t seed32Array[2];
    extern uint64_t seed64Array[2];
    //-----------------------------------------------------------------------------
    //
    //  seed_32()
    //
    //! \brief   Description:  Takes a 32bit unsigned integer for the seed start and plugs it into the seed32Array in 0. 1 is filled with seed>>1, then the random number generator is run twice replacing each seed value in the array
    //
    //  Entry:
    //!   \param[in] seed = value to use as a seed for the random number generator
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void seed_32(uint32_t seed);//start the seed for random number generation

    //-----------------------------------------------------------------------------
    //
    //  seed_64()
    //
    //! \brief   Description:  Takes a 64bit unsigned integer for the seed start and plugs it into the seed64Array in 0. 1 is filled with seed>>2, then the random number generator is run twice replacing each seed value in the array
    //
    //  Entry:
    //!   \param[in] seed = value to use as a seed for the random number generator
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void seed_64(uint64_t seed);//start the seed for random number generation

    //-----------------------------------------------------------------------------
    //
    //  xorshiftplus32()
    //
    //! \brief   Description:  Generate a 32bit (psuedo) random number using the xorshiftplus alorithm http://en.wikipedia.org/wiki/Xorshift. This was chosen for implemtnation for being fast and light weight
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return random number
    //
    //-----------------------------------------------------------------------------
    uint32_t xorshiftplus32(void);

    //-----------------------------------------------------------------------------
    //
    //  xorshiftplus64()
    //
    //! \brief   Description:  Generate a 64bit (psuedo) random number using the xorshiftplus alorithm http://en.wikipedia.org/wiki/Xorshift. This was chosen for implemtnation for being fast and light weight
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return random number
    //
    //-----------------------------------------------------------------------------
    uint64_t xorshiftplus64(void);

    //-----------------------------------------------------------------------------
    //
    //  random_Range_32()
    //
    //! \brief   Description:  Generate a 32bit (psuedo) random number using the xorshiftplus alorithm within the specified range (slightly biased)
    //
    //  Entry:
    //!   \param[in] rangeMin = value to use for minimum value of range
    //!   \param[in] rangeMax = value to use for maximum value of range
    //!
    //  Exit:
    //!   \return random number
    //
    //-----------------------------------------------------------------------------
    uint32_t random_Range_32(uint32_t rangeMin, uint32_t rangeMax);

    //-----------------------------------------------------------------------------
    //
    //  random_Range_64()
    //
    //! \brief   Description:  Generate a 64bit (psuedo) random number using the xorshiftplus alorithm within the specified range (slightly biased)
    //
    //  Entry:
    //!   \param[in] rangeMin = value to use for minimum value of range
    //!   \param[in] rangeMax = value to use for maximum value of range
    //!
    //  Exit:
    //!   \return random number
    //
    //-----------------------------------------------------------------------------
    uint64_t random_Range_64(uint64_t rangeMin, uint64_t rangeMax);

#if defined (__cplusplus)
}
#endif
