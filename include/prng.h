// SPDX-License-Identifier: MPL-2.0

//! \file prng.h
//! \brief Implements psuedo random number generation functionality.
//! Currently implemented using XOR shift+
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common_types.h"
#include <time.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    extern uint32_t seed32Array[2];
    extern uint64_t seed64Array[2];

    //! \fn void seed_32(uint32_t seed)
    //! \brief set the seed for the 32bit pseudo random number generator
    //! \param[in] seed value to set as the initial value for the random number generator
    void seed_32(uint32_t seed);

    //! \fn void seed_64(uint64_t seed)
    //! \brief set the seed for the 64bit pseudo random number generator
    //! \param[in] seed value to set as the initial value for the random number generator
    void seed_64(uint64_t seed);

    //! \fn uint32_t xorshiftplus32(void)
    //! \brief XOR shift + 32bit random number generator function
    //! \return random 32bit unsigned integer
    uint32_t xorshiftplus32(void);

    //! \fn uint64_t xorshiftplus64(void)
    //! \brief XOR shift + 64bit random number generator function
    //! \return random 64bit unsigned integer
    uint64_t xorshiftplus64(void);

    //! \fn uint32_t random_Range_32(uint32_t rangeMin, uint32_t rangeMax)
    //! \brief Generate a random 32bit number within a range
    //! \param[in] rangeMin minimum output value
    //! \param[in] rangeMax maximum output value
    //! \return random 32bit unsigned integer within specified range
    uint32_t random_Range_32(uint32_t rangeMin, uint32_t rangeMax);

    //! \fn uint64_t random_Range_64(uint64_t rangeMin, uint64_t rangeMax)
    //! \brief Generate a random 64bit number within a range
    //! \param[in] rangeMin minimum output value
    //! \param[in] rangeMax maximum output value
    //! \return random 64bit unsigned integer within specified range
    uint64_t random_Range_64(uint64_t rangeMin, uint64_t rangeMax);

#if defined(__cplusplus)
}
#endif
