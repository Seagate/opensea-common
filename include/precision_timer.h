// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All
// Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file highrestimer.c
// \brief Defines functions to time operations using a high resolution timer in
// a cross-platform way
//

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "memory_safety.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    typedef struct sseatimer_t
    {
        uint64_t timerStart; // system specific count value. Do not use directly. May be
                             // NS, CPU ticks, or some other system specific value
        uint64_t timerStop;  // system specific count value. Do not use directly. May
                             // be NS, CPU ticks, or some other system specific value
    } seatimer_t;

// This macro is helpful if declaring seatimer on the stack to initialize it to
// zero
#define DECLARE_SEATIMER(name) seatimer_t name = {UINT64_C(0), UINT64_C(0)}

// This macro is helpful if declaring seatimer on the heap to initialize it to
// zero. You must check if this is null after this macro!
#define NEW_SEATIMER(name) seatimer_t* name = M_REINTERPRET_CAST(seatimer_t*, safe_calloc(1, sizeof(seatimer_t)))

    static M_INLINE void safe_free_seatimer(seatimer_t** timer)
    {
        safe_Free(M_REINTERPRET_CAST(void**, timer));
    }

    //-----------------------------------------------------------------------------
    //
    // void start_Timer(seatimer_t *timer)
    //
    // \brief   Description: set's timerStart in a seatimer_t structure that will be
    // used later when calculating how long the timer ran for.
    //
    // Entry:
    //      \param[in,out] timer - pointer to a seatimer_t structure that will be
    //      used for the timer.
    //
    // Exit:
    //      \return VOID
    //
    //-----------------------------------------------------------------------------
    void start_Timer(seatimer_t* timer);

    //-----------------------------------------------------------------------------
    //
    // void stop_Timer(seatimer_t *timer)
    //
    // \brief   Description: set's timerStop in a seatimer_t structure that will be
    // used later when calculating how long the timer ran for.
    //
    // Entry:
    //      \param[in,out] timer - pointer to a seatimer_t structure that will be
    //      used for the timer.
    //
    // Exit:
    //      \return VOID
    //
    //-----------------------------------------------------------------------------
    void stop_Timer(seatimer_t* timer);

    //-----------------------------------------------------------------------------
    //
    // uint64_t get_Nano_Seconds(seatimer_t timer)
    //
    // \brief   Description: Gets the number of nano seconds elapsed between
    // timerStart and timerStop
    //
    // Entry:
    //      \param[in] timer - pointer to a seatimer_t structure that will be used
    //      for the timer.
    //
    // Exit:
    //      \return uint64_t value representing the number of nanoseconds elapsed.
    //
    //-----------------------------------------------------------------------------
    uint64_t get_Nano_Seconds(seatimer_t timer);

    //-----------------------------------------------------------------------------
    //
    // double get_Micro_Seconds(seatimer_t timer)
    //
    // \brief   Description: Gets the number of micro seconds elapsed between
    // timerStart and timerStop
    //
    // Entry:
    //      \param[in] timer - pointer to a seatimer_t structure that will be used
    //      for the timer.
    //
    // Exit:
    //      \return double value representing the number of microseconds elapsed.
    //
    //-----------------------------------------------------------------------------
    double get_Micro_Seconds(seatimer_t timer);

    //-----------------------------------------------------------------------------
    //
    // double get_Milli_Seconds(seatimer_t timer)
    //
    // \brief   Description: Gets the number of milli seconds elapsed between
    // timerStart and timerStop
    //
    // Entry:
    //      \param[in] timer - pointer to a seatimer_t structure that will be used
    //      for the timer.
    //
    // Exit:
    //      \return double value representing the number of milliseconds elapsed.
    //
    //-----------------------------------------------------------------------------
    double get_Milli_Seconds(seatimer_t timer);

    //-----------------------------------------------------------------------------
    //
    // double get_Seconds(seatimer_t timer)
    //
    // \brief   Description: Gets the number of seconds elapsed between timerStart
    // and timerStop
    //
    // Entry:
    //      \param[in] timer - pointer to a seatimer_t structure that will be used
    //      for the timer.
    //
    // Exit:
    //      \return double value representing the number of seconds elapsed.
    //
    //-----------------------------------------------------------------------------
    double get_Seconds(seatimer_t timer);

#if defined(__cplusplus)
}
#endif
