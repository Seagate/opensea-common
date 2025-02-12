// SPDX-License-Identifier: MPL-2.0

//! \file sleep.c
//! \brief Defines functions to sleep/delay that are cross-platform
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "sleep.h"

#include "predef_env_detect.h"
#include "type_conversion.h"

#if defined(_WIN32)
DISABLE_WARNING_4255
#    include "windows_version_detect.h"
#    include <windows.h>
#    include <winsock.h>
RESTORE_WARNING_4255
#else
#    include <time.h>
#    include <unistd.h> //needed for usleep()
#endif

//! \def NANOSECONDS_PER_SECOND
//! \brief number of nanoseconds in a second
#define NANOSECONDS_PER_SECOND UINT64_C(1000000000)

errno_t sleepns(uint64_t nanoseconds)
{
    errno_t error = 0;
#if defined(SYSTEM_WINDOWS)
    HANDLE        nanotimer = INVALID_HANDLE_VALUE;
    LARGE_INTEGER timeval;
    timeval.QuadPart = M_STATIC_CAST(LONGLONG, nanoseconds) * -1LL;
    nanotimer        = CreateWaitableTimer(M_NULLPTR, TRUE, M_NULLPTR);
    if (nanotimer == INVALID_HANDLE_VALUE)
    {
        error = ENOSYS;
    }
    else
    {
        if (MSFT_BOOL_FALSE(SetWaitableTimer(nanotimer, &timeval, 0, M_NULLPTR, M_NULLPTR, FALSE)))
        {
            error = ENOSYS;
        }
        else
        {
            DWORD waitres = WaitForSingleObject(nanotimer, INFINITE);
            if (waitres == WAIT_FAILED)
            {
                switch (GetLastError())
                {
                case ERROR_INVALID_FUNCTION:
                    error = ENOSYS;
                    break;
                default:
                    error = EFAULT;
                    break;
                }
            }
            else if (waitres == WAIT_OBJECT_0)
            {
                error = 0;
            }
            else if (waitres == WAIT_TIMEOUT)
            {
                error = ETIMEDOUT;
            }
            else if (waitres == WAIT_ABANDONED)
            {
                error = EOWNERDEAD;
            }
            else
            {
                error = EFAULT;
            }
        }
        CloseHandle(nanotimer);
    }
#else
#    if (defined POSIX_1993 && defined _POSIX_TIMERS)
    struct timespec delayTime;
// NOTE: tv_sec is long in C11, but time_t prior.
//       tv_nsec is implementation defined until C23 where it is long long
//       Because of this change due to standardization, there are extra ifdef's
//       for the casts to fix conversion warnings.-TJE
// Try using typeof for GCC and __typeof__ for clang, unless in C23 where typeof
// is standard.
//
#        if !defined(NO_TYPEOF)
    delayTime.tv_sec  = M_STATIC_CAST(M_TYPEOF(delayTime.tv_sec), nanoseconds / NANOSECONDS_PER_SECOND);
    delayTime.tv_nsec = M_STATIC_CAST(M_TYPEOF(delayTime.tv_nsec), (nanoseconds % NANOSECONDS_PER_SECOND));
#        elif defined(USING_C23)
    // C23, so use definitions from C23
    delayTime.tv_sec  = M_STATIC_CAST(long, nanoseconds / NANOSECONDS_PER_SECOND);
    delayTime.tv_nsec = M_STATIC_CAST(long long, (nanoseconds % NANOSECONDS_PER_SECOND));
#        elif defined(USING_C11)
    // Use long and long int as a best guess or most likely correct case
    delayTime.tv_sec  = M_STATIC_CAST(long, nanoseconds / NANOSECONDS_PER_SECOND);
    delayTime.tv_nsec = M_STATIC_CAST(long int, (nanoseconds % NANOSECONDS_PER_SECOND));
#        else
    // use time_t and long int
    delayTime.tv_sec  = M_STATIC_CAST(time_t, nanoseconds / NANOSECONDS_PER_SECOND);
    delayTime.tv_nsec = M_STATIC_CAST(long int, (nanoseconds % NANOSECONDS_PER_SECOND));
#        endif
    int sleepres = 0;
    do
    {
        sleepres = nanosleep(&delayTime, &delayTime);
    } while (sleepres == -1 && errno == EINTR);
    if (sleepres != 0)
    {
        error = errno;
    }
#    else
    errno = ENOSYS;
#    endif
#endif
    return error;
}

//! \def MICROSECONDS_PER_SECOND
//! \brief number of microseconds within a second as uint32_t
#define MICROSECONDS_PER_SECOND UINT32_C(1000000)

errno_t sleepus(uint32_t microseconds)
{
    errno_t error = 0;
    // NOTE: This function attempts to call sleepns first before falling back to using this call to select.
    //       This will give the best overall experience when trying to perform this action.
    //       The reason for this is nanosleep is the most recommended method to use due to how it interacts with signals
    //       and how it can report errors.
    //       Select is a good fallback since it has been around forever BSD 4.2 (BSD4_2) also in SUSv2 (USING_SUS2)
    //           NOTE: There are differences on how some implementations modify the timeval. DO NOT ASSUME THIS IS
    //                 expected behavior! That only happens on certain systems to restart the timer.
    //       usleep may also be available, but it is obsolete in POSIX2008. It is likely a wrapper to this call.
    if (ENOSYS == sleepns(M_STATIC_CAST(uint64_t, microseconds) * UINT64_C(1000)))
    {
        struct timeval tv;
#if defined(SYSTEM_WINDOWS)
        int res = -1;
#    if defined(_MSC_VER) && defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_8_DOT_1
        if (is_Windows_8_One_Or_Higher())
        {
            tv.tv_sec  = M_STATIC_CAST(long, microseconds / MICROSECONDS_PER_SECOND);
            tv.tv_usec = M_STATIC_CAST(long, microseconds % MICROSECONDS_PER_SECOND);
            res        = select(0, M_NULLPTR, M_NULLPTR, M_NULLPTR, &tv);
        }
        else
        {
            M_USE_UNUSED(tv);
            errno = ENOSYS;
            error = ENOSYS;
        }
#    else
        // select function is not available in this version of the windows API
        M_USE_UNUSED(tv);
        errno = ENOSYS;
        error = ENOSYS;
#    endif
#else
        tv.tv_sec  = M_STATIC_CAST(time_t, microseconds / MICROSECONDS_PER_SECOND);
        tv.tv_usec = M_STATIC_CAST(suseconds_t, microseconds % MICROSECONDS_PER_SECOND);
        int res    = select(0, M_NULLPTR, M_NULLPTR, M_NULLPTR, &tv);
#endif

        if (res == -1)
        {
            error = errno;
        }
    }
    return error;
}

errno_t sleepms(uint32_t milliseconds)
{
    errno_t error = 0;
#if defined(_WIN32) && !defined(UEFI_C_SOURCE)
    Sleep(milliseconds);
#else
    error = sleepus(milliseconds * UINT32_C(1000));
#endif
    return error;
}
// sleep first appeared in Version 7 AT&T unix - conforms to POSIX 1990
// usleep first appeared in 4.3BSD and is in POSIX 2001
// select first appeared in 4.2BSD and SUSv2 <-posix 2001 uses sys/select,
// otherwise sys/time, sys/types, and unistd

errno_t sleepsec(uint32_t seconds)
{
    errno_t error = 0;
    if (seconds > (UINT32_MAX / UINT32_C(1000)))
    {
        error = EOVERFLOW;
    }
    else
    {
        error = sleepms(UINT32_C(1000) * seconds);
    }
    return error;
}
