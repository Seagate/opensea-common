// SPDX-License-Identifier: MPL-2.0

//! \file precision_timer.c
//! \brief Defines functions to time operations using a high resolution timer in
//! a cross-platform way
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "precision_timer.h"
#include "env_detect.h"
#include "memory_safety.h"
#include "string_utils.h"
#include "type_conversion.h"

#include <string.h>

#if defined(_WIN32)
DISABLE_WARNING_4255
#    include <windows.h>
RESTORE_WARNING_4255
void start_Timer(seatimer_t* timer)
{
    DISABLE_NONNULL_COMPARE
    if (timer != M_NULLPTR)
    {
        LARGE_INTEGER tempLargeInt;
        tempLargeInt.QuadPart = 0;
        if (TRUE == QueryPerformanceCounter(&tempLargeInt)) // according to MSDN this will always return
                                                            // success on XP and later systems
        {
            if (tempLargeInt.QuadPart >= 0)
            {
                timer->timerStart = C_CAST(uint64_t, tempLargeInt.QuadPart);
            }
        }
    }
    RESTORE_NONNULL_COMPARE
}

void stop_Timer(seatimer_t* timer)
{
    DISABLE_NONNULL_COMPARE
    if (timer != M_NULLPTR)
    {
        LARGE_INTEGER tempLargeInt;
        safe_memset(&tempLargeInt, sizeof(LARGE_INTEGER), 0, sizeof(LARGE_INTEGER));
        if (TRUE == QueryPerformanceCounter(&tempLargeInt)) // according to MSDN this will always return
                                                            // success on XP and later systems
        {
            if (tempLargeInt.QuadPart >= 0)
            {
                timer->timerStop = C_CAST(uint64_t, tempLargeInt.QuadPart);
            }
        }
    }
    RESTORE_NONNULL_COMPARE
}

uint64_t get_Nano_Seconds(seatimer_t timer)
{
    LARGE_INTEGER frequency;                                 // clock ticks per second
    uint64_t      ticksPerNanosecond = UINT64_C(1000000000); // start with a count of nanoseconds per second
    uint64_t      seconds            = UINT64_C(0);
    uint64_t      nanoSeconds        = UINT64_C(0);
    safe_memset(&frequency, sizeof(LARGE_INTEGER), 0, sizeof(LARGE_INTEGER));
    if (TRUE == QueryPerformanceFrequency(&frequency))
    {
        if (frequency.QuadPart > 0) // no equals since this is used in division
                                    // and don't want to divide by zero
        {
            ticksPerNanosecond /= C_CAST(uint64_t, frequency.QuadPart);
            seconds =
                (timer.timerStop - timer.timerStart) / C_CAST(uint64_t,
                                                              frequency.QuadPart); // converted to nanoseconds later
            nanoSeconds =
                ((timer.timerStop - timer.timerStart) % C_CAST(uint64_t, frequency.QuadPart)) * ticksPerNanosecond;
            return ((seconds * UINT64_C(1000000000)) + nanoSeconds);
        }
        else
        {
            return UINT64_C(0);
        }
    }
    else
    {
        return UINT64_C(0);
    }
}
#else //!_WIN32
#    include <sys/time.h>
#    include <time.h>
#    include <unistd.h>
// Always prefers CLOCK_MONOTONIC
// may want to consider CLOCK_REALTIME as an alternative when not supported
// (this will be affected just like gettimeofday function) other clocks that may
// work: CLOCK_TAI (linux),  CLOCK_MONOTONIC_RAW  (linux), CLOCK_BOOTTIME
// (linux) https://www.man7.org/linux/man-pages/man3/clock_gettime.3.html
void start_Timer(seatimer_t* timer)
{
    DISABLE_NONNULL_COMPARE
    if (timer != M_NULLPTR)
    {
        struct timespec startTimespec;
        int             ret = 0;
        safe_memset(&startTimespec, sizeof(struct timespec), 0, sizeof(struct timespec));
#    if !defined(UEFI_C_SOURCE) && defined(POSIX_1993) && defined(_POSIX_TIMERS) && defined _POSIX_MONOTONIC_CLOCK
        ret = clock_gettime(CLOCK_MONOTONIC, &startTimespec);
#    else // this function is older and more likely available if we don't have
          // the support we need/want
        ret = gettimeofday(&startTimespec, M_NULLPTR);
#    endif
        if (0 == ret) // hopefully this always works...-TJE
        {
            //        printf("Start Time:  %lu\n", startTimespec.tv_nsec);
            timer->timerStart = (C_CAST(uint64_t, startTimespec.tv_sec) * UINT64_C(1000000000)) +
                                C_CAST(uint64_t, startTimespec.tv_nsec);
        }
        //    else
        //    {
        //       printf("Bad start_timer Ret:  %d\n",ret);
        //    }
    }
    RESTORE_NONNULL_COMPARE
}

void stop_Timer(seatimer_t* timer)
{
    DISABLE_NONNULL_COMPARE
    if (timer != M_NULLPTR)
    {
        struct timespec stopTimespec;
        int             ret = 0;
        safe_memset(&stopTimespec, sizeof(struct timespec), 0, sizeof(struct timespec));
#    if !defined(UEFI_C_SOURCE) && defined(POSIX_1993) && defined(_POSIX_TIMERS) && defined _POSIX_MONOTONIC_CLOCK
        ret = clock_gettime(CLOCK_MONOTONIC, &stopTimespec);
#    else // this function is older and more likely available if we don't have
          // the support we need/want
        ret = gettimeofday(&stopTimespec, M_NULLPTR);
#    endif
        if (0 == ret) // hopefully this always works...-TJE
        {
            //        printf("Stop Time:  %lu\n", stopTimespec.tv_nsec);
            timer->timerStop =
                (C_CAST(uint64_t, stopTimespec.tv_sec) * UINT64_C(1000000000)) + C_CAST(uint64_t, stopTimespec.tv_nsec);
        }
        //    else
        //    {
        //       printf("Bad stop_timer Ret:  %d\n",ret);
        //    }
    }
    RESTORE_NONNULL_COMPARE
}

uint64_t get_Nano_Seconds(seatimer_t timer)
{
    return timer.timerStop - timer.timerStart;
}
#endif // platform check

double get_Micro_Seconds(seatimer_t timer)
{
    uint64_t nanoseconds = get_Nano_Seconds(timer);
    return (C_CAST(double, nanoseconds) / 1000.00);
}

double get_Milli_Seconds(seatimer_t timer)
{
    return (get_Micro_Seconds(timer) / 1000.00);
}

double get_Seconds(seatimer_t timer)
{
    return (get_Milli_Seconds(timer) / 1000.00);
}
