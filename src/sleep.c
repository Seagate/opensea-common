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
// \file sleep.c
// \brief Defines functions to sleep/delay in cross-platform mannor
//

#include "sleep.h"

#include "predef_env_detect.h"
#include "type_conversion.h"

#if defined(_WIN32)
DISABLE_WARNING_4255
#    include <windows.h>
RESTORE_WARNING_4255
#else
#    include <time.h>
#    include <unistd.h> //needed for usleep()
#endif

void delay_Milliseconds(uint32_t milliseconds)
{
#if defined(_WIN32) && !defined(UEFI_C_SOURCE)
    Sleep(milliseconds);
#else
// according to this link: http://linux.die.net/man/3/usleep
// usleep is obsolete starting in POSIX 2001 and removed entirely in POSIX 2008
// and nanosleep is supposed to be used instead. the usleep code is left in just
// in case it is needed for any reason, but nanosleep works as expected
#    if defined POSIX_1993 && defined _POSIX_TIMERS
    struct timespec delayTime;
// NOTE: tv_sec is long in C11, but time_t prior.
//       tv_nsec is implementation defined until C23 where it is long long
//       Because of this change due to standardization, there are extra ifdef's
//       for the casts to fix conversion warnings.-TJE
// Try using typeof for GCC and __typeof__ for clang, unless in C23 where typeof
// is standard.
//
#        if defined(USING_C23)
    // C23, so use definitions from C23
    delayTime.tv_sec  = C_CAST(long, milliseconds / UINT32_C(1000));
    delayTime.tv_nsec = C_CAST(long long, UINT32_C(1000000) * (milliseconds % UINT32_C(1000)));
#        elif !defined(NO_TYPEOF)
    delayTime.tv_sec  = C_CAST(M_TYPEOF(delayTime.tv_sec), milliseconds / UINT32_C(1000));
    delayTime.tv_nsec = C_CAST(M_TYPEOF(delayTime.tv_nsec), UINT32_C(1000000) * (milliseconds % UINT32_C(1000)));
#        elif defined(USING_C11)
    // Use long and long int as a best guess or most likely correct case
    delayTime.tv_sec  = C_CAST(long, milliseconds / UINT32_C(1000));
    delayTime.tv_nsec = C_CAST(long int, UINT32_C(1000000) * (milliseconds % UINT32_C(1000)));
#        else
    // use time_t and long int
    delayTime.tv_sec  = C_CAST(time_t, milliseconds / UINT32_C(1000));
    delayTime.tv_nsec = C_CAST(long int, UINT32_C(1000000) * (milliseconds % UINT32_C(1000)));
#        endif
    nanosleep(&delayTime, M_NULLPTR);
#    else  //! POSIX 1993...try usleep instead
    usleep(C_CAST(useconds_t, milliseconds * UINT32_C(1000)));
#    endif // POSIX check
#endif
}
// sleep first appeared in Version 7 AT&T unix - conforms to POSIX 1990
// usleep first appeared in 4.3BSD and is in POSIX 2001
// select first appeared in 4.2BSD and SUSv2 <-posix 2001 uses sys/select,
// otherwise sys/time, sys/types, and unistd

void delay_Seconds(uint32_t seconds)
{
    delay_Milliseconds(UINT32_C(1000) * seconds);
}
