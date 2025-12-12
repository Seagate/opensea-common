// SPDX-License-Identifier: MPL-2.0

//! \file sleep.h
//! \brief Defines functions to sleep/delay that are cross-platform
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common_types.h"
#include "type_conversion.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \fn errno_t sleepns(uint64_t nanoseconds)
    //! \brief sleep for specified number of nanoseconds
    //! \param nanoseconds number of nanoseconds to sleep
    //! \return 0 for no error, ENOSYS if nanosecond sleep not supported,
    //! otherwise an errno value describing the issue
    //! \note Windows has 100ns resolution in this function
    //! \note Recommended to use the other sleep functions
    errno_t sleepns(uint64_t nanoseconds);

    //! \fn bool is_nanosleep_supported(void)
    //! \brief Check if support for nanosecond sleep is supported
    //! \return true = supported, false = not supported
    static M_INLINE bool is_nanosleep_supported(void)
    {
        bool supported = true;
        if (ENOSYS == sleepns(UINT64_C(100)))
        {
            supported = false;
        }
        return supported;
    }

#if defined(SYSTEM_WINDOWS) && defined(_MSC_VER) && defined(WIN_API_TARGET_VERSION) &&                                 \
    WIN_API_TARGET_VERSION >= WIN_API_TARGET_8_DOT_1
#    pragma comment(lib, "Ws2_32.lib")
#endif

    //! \fn errno_t sleepus(uint32_t microseconds)
    //! \brief sleep for specified number of microseconds
    //! \param microseconds number of microseconds to sleep
    //! \return 0 for no error, otherwise an errno value describing the issue
    errno_t sleepus(uint32_t microseconds);

    //! \fn errno_t sleepms(uint32_t milliseconds)
    //! \brief sleep for specified number of milliseconds
    //! \param milliseconds number of milliseconds to sleep
    //! \return 0 for no error, otherwise an errno value describing the issue
    errno_t sleepms(uint32_t milliseconds);

    //! \fn void delay_Milliseconds(uint32_t milliseconds)
    //! \brief delay/sleep for specified number of milliseconds
    //!
    //! Recommend using sleepms going forward for a better named function
    //!
    //! \param milliseconds number of milliseconds to delay/sleep
    static M_INLINE void delay_Milliseconds(uint32_t milliseconds)
    {
        M_STATIC_CAST(void, sleepms(milliseconds));
    }

    //! \fn errno_t sleepsec(uint32_t seconds)
    //! \brief sleep for specified number of seconds
    //! \param seconds number of seconds to sleep
    //! \return 0 for no error, otherwise an errno value describing the issue
    errno_t sleepsec(uint32_t seconds);

    //! \fn void delay_Seconds(uint32_t seconds)
    //! \brief delay/sleep for specified number of seconds
    //!
    //! Recommend using sleepsec going forward for a better named function
    //!
    //! \param milliseconds number of seconds to delay/sleep
    static M_INLINE void delay_Seconds(uint32_t seconds)
    {
        M_STATIC_CAST(void, sleepsec(seconds));
    }

#if defined(__cplusplus)
}
#endif
