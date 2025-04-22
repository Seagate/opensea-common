// SPDX-License-Identifier: MPL-2.0

//! \file precision_timer.h
//! \brief Defines functions to time operations using a high resolution timer in
//! a cross-platform way
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "memory_safety.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \struct seatimer_t
    //! \brief structure used to track timer start and stop times
    //! \details The values in this structure may be system specific.
    //! Use the functions to translate this to a timeunit to
    //! properly understand the output
    typedef struct sseatimer_t
    {
        uint64_t timerStart; //!< system specific count value.
        uint64_t timerStop;  //!< system specific count value.
    } seatimer_t;

//! \def DECLARE_SEATIMER
//! \brief helper macro to create a seatimer_t on the stack and initialize it to zero
#define DECLARE_SEATIMER(name) seatimer_t name = {UINT64_C(0), UINT64_C(0)}

//! \def NEW_SEATIMER
//! \brief helper macro to create a seatimer_t on the heap and initialize it to zero
//! \note You much check that your timer is non-null after using this
#define NEW_SEATIMER(name) seatimer_t* name = M_REINTERPRET_CAST(seatimer_t*, safe_calloc(1, sizeof(seatimer_t)))

    //! \fn void safe_free_seatimer(seatimer_t** timer)
    //! \brief Helper function to safely free a seatimer from the heap
    static M_INLINE void safe_free_seatimer(seatimer_t** timer)
    {
        safe_free_core(M_REINTERPRET_CAST(void**, timer));
    }

    //! \fn void start_Timer(seatimer_t* timer)
    //! \brief start the timer
    //! \param[in,out] timer pointer to seatimer to start
    M_NONNULL_PARAM_LIST(1) M_PARAM_WO(1) void start_Timer(seatimer_t* timer);

    //! \fn void stop_Timer(seatimer_t* timer)
    //! \brief stop the timer
    //! \param[in,out] timer pointer to seatimer to stop
    M_NONNULL_PARAM_LIST(1) M_PARAM_WO(1) void stop_Timer(seatimer_t* timer);

    //! \fn uint64_t get_Nano_Seconds(seatimer_t timer)
    //! \brief get the number of nanoseconds elapsed between timer start and stop
    //! \param[in] timer timer with valid start and stop times to get the number of nanoseconds from
    //! \return uint64_t number of elapsed nanoseconds
    uint64_t get_Nano_Seconds(seatimer_t timer);

    //! \fn double get_Micro_Seconds(seatimer_t timer)
    //! \brief get the number of microseconds elapsed between timer start and stop
    //! \param[in] timer timer with valid start and stop times to get the number of microseconds from
    //! \return double number of elapsed microseconds
    double get_Micro_Seconds(seatimer_t timer);

    //! \fn double get_Milli_Seconds(seatimer_t timer)
    //! \brief get the number of milliseconds elapsed between timer start and stop
    //! \param[in] timer timer with valid start and stop times to get the number of milliseconds from
    //! \return double number of elapsed milliseconds
    double get_Milli_Seconds(seatimer_t timer);

    //! \fn double get_Seconds(seatimer_t timer)
    //! \brief get the number of sseconds elapsed between timer start and stop
    //! \param[in] timer timer with valid start and stop times to get the number of seconds from
    //! \return double number of elapsed seconds
    double get_Seconds(seatimer_t timer);

#if defined(__cplusplus)
}
#endif
