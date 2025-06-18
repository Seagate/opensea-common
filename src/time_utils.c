// SPDX-License-Identifier: MPL-2.0

//! \file time_utils.c
//! \brief Implements best practices for working with time functions in stdc library
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "time_utils.h"
#include "bit_manip.h"
#include "common_types.h"
#include "io_utils.h"
#include "memory_safety.h"
#include "string_utils.h"
#include "type_conversion.h"

#if defined(_WIN32)
#    include "windows_version_detect.h"
#    if IS_MSVC_VERSION(MSVC_2003)
#        include <sys/timeb.h>
#        include <sys/types.h>
#    endif // VS2003 version check
#endif

#include <locale.h> //used when getting time to replace ctime and asctime function to try and replicate the format exactly-TJE
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

time_t CURRENT_TIME = M_STATIC_CAST(time_t, 0);
// NOTE: Do not use the DECLARE_ZERO_INIT_ARRAY here as it will not compile
// correctly for a global variable like this.
//       Instead we are manually setting all bytes to zero the old fashioned way
//       - TJE
char CURRENT_TIME_STRING[CURRENT_TIME_STRING_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

M_STATIC_ASSERT(SIZE_OF_STACK_ARRAY(CURRENT_TIME_STRING) >= 26, current_time_string_length_too_short);

// cannot support 32bit time_t properly!
M_STATIC_ASSERT(sizeof(time_t) >= 8, time_t_is_not_64_bits);

// Leap years occur every 4 years.
// Every 100 years you skip a leap year, unless it has been 400 years in which case you do not skip the leap year
static M_INLINE bool is_Year_A_Leap_Year(int year)
{
    bool isLeapYear = false;
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
    {
        isLeapYear = true;
    }
    return isLeapYear;
}

bool get_current_timestamp(void)
{
    bool retStatus = true;
    if (strcmp(CURRENT_TIME_STRING, "") == 0 || strcmp(CURRENT_TIME_STRING, "CAL_TIME_NOT_AVAILABLE") == 0)
    {
        struct tm logTime;
        safe_memset(&logTime, sizeof(struct tm), 0, sizeof(struct tm));
        CURRENT_TIME = time(M_NULLPTR);
        if (CURRENT_TIME == TIME_T_ERROR)
        {
            safe_memset(CURRENT_TIME_STRING, SIZE_OF_STACK_ARRAY(CURRENT_TIME_STRING), 0,
                        SIZE_OF_STACK_ARRAY(CURRENT_TIME_STRING));
            retStatus = false;
            snprintf_err_handle(CURRENT_TIME_STRING, CURRENT_TIME_STRING_LENGTH, "CAL_TIME_NOT_AVAILABLE");
        }
        else
        {
            safe_memset(CURRENT_TIME_STRING, SIZE_OF_STACK_ARRAY(CURRENT_TIME_STRING), 0,
                        SIZE_OF_STACK_ARRAY(CURRENT_TIME_STRING));
            retStatus = M_ToBool(strftime(CURRENT_TIME_STRING, CURRENT_TIME_STRING_LENGTH, "%Y%m%dT%H%M%S",
                                          get_Localtime(&CURRENT_TIME, &logTime)) > 0);
        }
    }
    return retStatus;
}

#if defined(NEED_TIMESPEC_GET)
#    if defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_2000
static M_INLINE int win_filetime_to_struct_timespec(struct timespec* ts, int base)
{
    int res = 0;
    // Use the function in the link below, but MSFT also documents another
    // way to do this, which is what we've implemented -TJE
    // https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-rtltimetosecondssince1970
    FILETIME       ftnow;
    FILETIME       epochfile;
    SYSTEMTIME     epoch;
    ULARGE_INTEGER nowAsInt;
    ULARGE_INTEGER epochAsInt;
    safe_memset(&ftnow, sizeof(FILETIME), 0, sizeof(FILETIME));
    safe_memset(&epochfile, sizeof(FILETIME), 0, sizeof(FILETIME));
    safe_memset(&epoch, sizeof(SYSTEMTIME), 0, sizeof(SYSTEMTIME));
    safe_memset(&nowAsInt, sizeof(ULARGE_INTEGER), 0, sizeof(ULARGE_INTEGER));
    safe_memset(&epochAsInt, sizeof(ULARGE_INTEGER), 0, sizeof(ULARGE_INTEGER));
    epoch.wYear      = WORD_C(1970);
    epoch.wMonth     = WORD_C(1);
    epoch.wSecond    = WORD_C(0); // MSFT says to set to first second.
    epoch.wDayOfWeek = WORD_C(4); // Thursday
    epoch.wDay       = WORD_C(1);
    GetSystemTimeAsFileTime(&ftnow);
    safe_memcpy(&nowAsInt, sizeof(ULARGE_INTEGER), &ftnow,
                sizeof(FILETIME)); // these are both 8 bytes in size, so it SHOULD
                                   // be fine and MSFT says to do this-TJE
    SystemTimeToFileTime(&epoch, &epochfile);
    safe_memcpy(&epochAsInt, sizeof(ULARGE_INTEGER), &epochfile,
                sizeof(FILETIME));                // these are both 8 bytes in size, so it SHOULD
                                                  // be fine and MSFT says to do this-TJE
    if (nowAsInt.QuadPart >= epochAsInt.QuadPart) // this should always be true, but seems like
                                                  // microsoft is likely checking this, so
                                                  // checking this as well-TJE
    {
        uint64_t nsSinceJan1970 = C_CAST(uint64_t, (nowAsInt.QuadPart - epochAsInt.QuadPart));
        // subtracting the 2 large integers gives 100
        // nanosecond intervals since jan 1, 1970
        ts->tv_sec  = C_CAST(time_t, nsSinceJan1970 / UINT64_C(1000000000));
        ts->tv_nsec = C_CAST(int32_t, nsSinceJan1970 % UINT64_C(1000000000));
        res         = base;
    }
    return res;
}
#    endif // WIN_API_TARGET_VERSION >= WIN_API_TARGET_2000

#    if IS_MSVC_VERSION(MSVC_2003)
static M_INLINE int win_ftime64_to_struct_timespec(struct timespec* ts, int base)
{
    // Microsoft also has ftime, ftime64 and ftime_s
    // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/ftime-s-ftime32-s-ftime64-s?view=msvc-170
    struct __timeb64 timenow;
    int              res       = 0;
    int              memsetret = safe_memset(&timenow, sizeof(struct __timeb64), 0, sizeof(struct __timeb64));
#        if IS_MSVC_VERSION(MSVC_2005)
    if (0 == memsetret && 0 == _ftime64_s(&timenow)) // Can use _s version
    {
        ts->tv_sec  = timenow.time;
        ts->tv_nsec = C_CAST(int32_t, C_CAST(uint64_t, timenow.millitm) * UINT64_C(1000000));
        res         = base;
    }
#        else  // VS2003
    if (0 == memsetret)
    {
        _ftime64(&timenow); // Can use _ftime64 back to Win98
        ts->tv_sec  = timenow.time;
        ts->tv_nsec = C_CAST(int32_t, C_CAST(uint64_t, timenow.millitm) * UINT64_C(1000000));
        res         = base;
    }
#        endif // VS2005 check
    return res;
}
#    endif // IS_MSVC_VERSION(MSVC_2003)

static M_INLINE int struct_tm_to_struct_timespec(struct timespec* ts, int base)
{
    int res = 0;
    // time_t is not guaranteed to always be a specific number of units.
    // This can be system specific/library specific. so to convert using
    // old standards, take a current time_t, convert it to struct tm,
    // then calculate it. If time_t is 32bits, then this will not work
    // past 2038. To detect this, we just need to check if the current
    // year is less than 1970. If it is, it could be a roll over problem
    // or some other clock issue. in that case, just return 0. NOTE:
    // This code seems to work until approximately the year 275705. The
    // following year I've noticed can be off by a day. There is
    // probably a missing
    //       adjustment factor somewhere else, but this is close
    //       enough-TJE
    // NOTE: To take into account leap seconds you need a table, or you
    // can average 1 every 1.5 years:
    // https://www.nist.gov/pml/time-and-frequency-division/leap-seconds-faqs#often
    //       This is not currently taken into account -TJE
    struct tm nowstruct;
    time_t    currentTime = time(M_NULLPTR);
    if (currentTime == TIME_T_ERROR)
    {
        return UINT64_C(0);
    }
    safe_memset(&nowstruct, sizeof(struct tm), 0, sizeof(struct tm));
    get_UTCtime(&currentTime, &nowstruct);
    uint64_t currentyear = (M_STATIC_CAST(uint64_t, nowstruct.tm_year) + UINT64_C(1900));
    if (currentyear >= UINT64_C(1970))
    {
        // need to account for leap years before calculating below. This
        // will get how many leap years there are between the current
        // year and 1970 to add to number of days for adjustment
        uint64_t leapyears = (currentyear - UINT64_C(1970)) / UINT64_C(4);
        // now subtract the number of skipped leap years (every 100
        // years unless divible by 400). This needs to be number since
        // 1970
        uint64_t skippedleapyears = UINT64_C(0);
        // years 2000 and 2400 are leap years, but 1700, 1800, 1900,
        // 2100, 2200, and 2300 are not. To get skipped leap years, need
        // to calculate based on number of centuries that have passed
        // since 1970, then check how many have skipped leap year. No
        // need to worry about this until at least 2100, however if you
        // skip the year 2000, this causes a problem in 24606 where it
        // will be off by 1 day.-TJE
        if (currentyear >= UINT64_C(2000)) // start at 2000 since it is the first
                                           // century after 1970.
        {
            // at least year 2100, so need to handle skipped leap years
            // and how many of these have occurred
            uint64_t centuriesSince1970 = ((currentyear - UINT64_C(1970)) / UINT64_C(100)) - UINT64_C(1);
            // use a loop to check each century, starting in 2100 to see
            // if it was a skipped leap year or not.
            uint64_t checkCentury = UINT64_C(2100);
            while (checkCentury <= (UINT64_C(2100) + (centuriesSince1970 * UINT64_C(100))))
            {
                if (checkCentury % UINT64_C(400) != UINT64_C(0))
                {
                    // This century does not have a leap year
                    skippedleapyears += UINT64_C(1);
                }
                checkCentury += UINT64_C(100);
            }
        }
        leapyears -= skippedleapyears;
        // Need to detect if the leap year is in the current year and if
        // it has occured already or not.
        if (currentyear % UINT64_C(4) == UINT64_C(0))
        {
            bool adjustforcurrentleapyear = true;
            // leap year is in the current year, unless it is a year to
            // skip a leap year (every 100 years, unless divisible by
            // 400)
            if (currentyear % UINT64_C(100) == UINT64_C(0) && currentyear % UINT64_C(400) != UINT64_C(0))
            {
                // currently in a century year without a leap year. No
                // need for the adjustment below
                adjustforcurrentleapyear = false;
            }
            if (adjustforcurrentleapyear)
            {
                // Check if it is past february 29th already or not.
                if (nowstruct.tm_mon < 2)
                {
                    // february or january
                    if (nowstruct.tm_mon == 0 || (nowstruct.tm_mon == 1 && nowstruct.tm_mday <= 29))
                    {
                        // subtract 1 from leapyears since it has not
                        // yet occurred for the current year
                        leapyears -= UINT64_C(1);
                    }
                }
            }
        }
        // use tm_sec, tm_min, tm_hour, tm_year, and tm_yday to convert
        // this.-TJE
        // NOTE: This was moved here to simplify the get time since unix epoch code and is left as is since it otherwise
        // works fine.-TJE
        uint64_t msSinceJan1970 = (C_CAST(uint64_t, nowstruct.tm_sec) * UINT64_C(1000)) +
                                  (C_CAST(uint64_t, nowstruct.tm_min) * UINT64_C(60000)) +
                                  (C_CAST(uint64_t, nowstruct.tm_hour) * UINT64_C(3600000)) +
                                  ((C_CAST(uint64_t, nowstruct.tm_yday) + leapyears) * UINT64_C(86400000)) +
                                  ((currentyear - UINT64_C(1970)) * UINT64_C(31536000000));
        ts->tv_sec  = C_CAST(time_t, msSinceJan1970 / UINT64_C(1000));
        ts->tv_nsec = C_CAST(int32_t, msSinceJan1970 %
                                          UINT64_C(1000)); // should set zero, but this is the correct way to set this
        res         = base;
    }
    return res;
}

int timespec_get(struct timespec* ts, int base)
{
    int res = 0;
    if (base != TIME_UTC)
    {
        return res;
    }
#    if defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_2000
    res = win_filetime_to_struct_timespec(ts, base);
    if (res == base)
    {
        return res;
    }
#    elif IS_MSVC_VERSION(MSVC_2003)
    res = win_ftime64_to_struct_timespec(ts, base);
    if (res == base)
    {
        return res;
    }
#    elif defined(POSIX_1993) && defined _POSIX_MONOTONIC_CLOCK
    // This uses the same structure and is best to use when available
    if (0 == clock_gettime(CLOCK_REALTIME, ts))
    {
        res = base;
        return res;
    }
#    elif defined(BSD4_2) || defined(POSIX_2001) || defined(SVR4_ENV) || defined(USING_SUS2)
    // using gettimeofday function as a backup
    // NOTE: ftime and ftime64 are another possible API but that has been long obsolete...longer than this method.
    // https://man.freebsd.org/cgi/man.cgi?query=ftime&sektion=3&n=1
    // https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-ftime-ftime64-set-date-time
    struct timeval timenow;
    safe_memset(&timenow, sizeof(struct timeval), 0, sizeof(struct timeval));
    if (0 == gettimeofday(&timenow, M_NULLPTR))
    {
        ts->tv_sec  = timenow.tv_sec;
        ts->tv_nsec = C_CAST(int32_t, timenow.tv_usec) * INT32_C(1000) res = base;
        return res;
    }
#    endif
    res = struct_tm_to_struct_timespec(ts, base);
    return res;
}
#endif // NEED_TIMESPEC_GET

#define JAN_1_1970_DAY_OF_WEEK   4 // Thursday
#define UNIX_EPOCH_STARTING_YEAR 1970
#define C_STRUCT_TM_YEARS_START  1900 // struct tm defines tm_year as a counter since 1900
#define DAYS_IN_WEEK             7
#define C_STRUCT_TM_MONTH_JAN    0
#define C_STRUCT_TM_MONTH_FEB    1
#define C_STRUCT_TM_MONTH_DEC    11

struct tm* milliseconds_Since_Unix_Epoch_To_Struct_TM(uint64_t milliseconds, struct tm* time)
{
    DISABLE_NONNULL_COMPARE
    if (time != M_NULLPTR)
    {
        const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        int       year            = 0;
        int       month           = 0;
        int       day             = 0;
        int       yday            = 0;
        const int secPerMin       = 60;
        const int secPerHour      = 3600;
        const int secPerDay       = 86400;
        const int daysPerYear     = 365;
        const int daysPerLeapYear = 366;
        const int secPerYear      = secPerDay * daysPerYear;
        const int secPerLeapYear  = secPerDay * daysPerLeapYear;

        int64_t seconds = M_STATIC_CAST(int64_t, milliseconds / UINT64_C(1000));
        int64_t remsec  = seconds;

        while (remsec >= (is_Year_A_Leap_Year(year) ? secPerLeapYear : secPerYear))
        {
            remsec -= is_Year_A_Leap_Year(year) ? secPerLeapYear : secPerYear;
            year += UINT16_C(1);
        }

        for (month = C_STRUCT_TM_MONTH_JAN; month <= C_STRUCT_TM_MONTH_DEC; month += 1)
        {
            int daysInMonth = days_in_month[month];
            if (month == C_STRUCT_TM_MONTH_FEB && is_Year_A_Leap_Year(year))
            {
                daysInMonth += 1;
            }
            if (remsec < (M_STATIC_CAST(int64_t, secPerDay) * M_STATIC_CAST(int64_t, daysInMonth)))
            {
                day = M_STATIC_CAST(int, (remsec / M_STATIC_CAST(int64_t, secPerDay)) + INT64_C(1)); // 1 based index
                yday += day - 1;                                                                     // 0 based index
                remsec %= secPerDay;
                break;
            }
            remsec -= (M_STATIC_CAST(int64_t, secPerDay) * M_STATIC_CAST(int64_t, daysInMonth));
            yday += daysInMonth;
        }

        int hour = M_STATIC_CAST(int, (remsec / M_STATIC_CAST(int64_t, secPerHour)));
        remsec %= M_STATIC_CAST(int64_t, secPerHour);
        int minute = M_STATIC_CAST(int, (remsec / M_STATIC_CAST(int64_t, secPerMin)));
        remsec %= M_STATIC_CAST(int64_t, secPerMin);
        int second = M_STATIC_CAST(int, remsec);

        time->tm_year  = (year + UNIX_EPOCH_STARTING_YEAR) - C_STRUCT_TM_YEARS_START;
        time->tm_mon   = month;
        time->tm_mday  = day;
        time->tm_yday  = yday;
        time->tm_hour  = hour;
        time->tm_min   = minute;
        time->tm_sec   = second;
        time->tm_isdst = -1;
        time->tm_wday  = M_STATIC_CAST(int, ((seconds / M_STATIC_CAST(int64_t, secPerDay)) + M_STATIC_CAST(int64_t, JAN_1_1970_DAY_OF_WEEK)) % M_STATIC_CAST(int64_t, DAYS_IN_WEEK));
    }
    RESTORE_NONNULL_COMPARE
    return time;
}

// returns number of milliseconds since 1970 UTC
uint64_t get_Milliseconds_Since_Unix_Epoch(void)
{
    uint64_t msSinceJan1970 = UINT64_C(0);
    // Check for C11's standardized API call.
    struct timespec now;
    safe_memset(&now, sizeof(struct timespec), 0, sizeof(struct timespec));
    if (TIME_UTC == timespec_get(&now, TIME_UTC))
    {
        // NOTE: Technically this is a time since the system's epoch as C
        // standard does not set an epoch.
        //       I'm not sure there is a system we have encountered where it is
        //       not the Unix epoch, so this is fine for now - TJE
        msSinceJan1970 =
            (C_CAST(uint64_t, now.tv_sec) * UINT64_C(1000)) + (C_CAST(uint64_t, now.tv_nsec) / UINT64_C(1000000));
    }
    return msSinceJan1970;
}

void convert_Seconds_To_Displayable_Time_Double(double    secondsToConvert,
                                                uint8_t*  years,
                                                uint16_t* days,
                                                uint8_t*  hours,
                                                uint8_t*  minutes,
                                                uint8_t*  seconds)
{
    double tempCalcValue = secondsToConvert;
    // get seconds up to a maximum of 60
    if (seconds != M_NULLPTR)
    {
        *seconds = C_CAST(uint8_t, fmod(tempCalcValue, 60.0));
    }
    tempCalcValue /= 60.0;
    // get minutes up to a maximum of 60
    if (minutes != M_NULLPTR)
    {
        *minutes = C_CAST(uint8_t, fmod(tempCalcValue, 60.0));
    }
    tempCalcValue /= 60.0;
    // get hours up to a maximum of 24
    if (hours != M_NULLPTR)
    {
        *hours = C_CAST(uint8_t, fmod(tempCalcValue, 24.0));
    }
    tempCalcValue /= 24.0;
    // get days up to 365
    if (days != M_NULLPTR)
    {
        *days = C_CAST(uint16_t, fmod(tempCalcValue, 365.0));
    }
    tempCalcValue /= 365.0;
    // get years
    if (years != M_NULLPTR)
    {
        *years = C_CAST(uint8_t, tempCalcValue);
    }
}

void convert_Seconds_To_Displayable_Time(uint64_t  secondsToConvert,
                                         uint8_t*  years,
                                         uint16_t* days,
                                         uint8_t*  hours,
                                         uint8_t*  minutes,
                                         uint8_t*  seconds)
{
    uint64_t tempCalcValue = secondsToConvert;
    // get seconds up to a maximum of 60
    if (seconds != M_NULLPTR)
    {
        *seconds = C_CAST(uint8_t, tempCalcValue % UINT64_C(60));
    }
    tempCalcValue /= UINT64_C(60);
    // get minutes up to a maximum of 60
    if (minutes != M_NULLPTR)
    {
        *minutes = C_CAST(uint8_t, tempCalcValue % UINT64_C(60));
    }
    tempCalcValue /= UINT64_C(60);
    // get hours up to a maximum of 24
    if (hours != M_NULLPTR)
    {
        *hours = C_CAST(uint8_t, tempCalcValue % UINT64_C(24));
    }
    tempCalcValue /= UINT64_C(24);
    // get days up to 365
    if (days != M_NULLPTR)
    {
        *days = C_CAST(uint16_t, tempCalcValue % UINT64_C(365));
    }
    tempCalcValue /= UINT64_C(365);
    // get years
    if (years != M_NULLPTR)
    {
        *years = C_CAST(uint8_t, tempCalcValue);
    }
}

void print_Time_To_Screen(const uint8_t*  years,
                          const uint16_t* days,
                          const uint8_t*  hours,
                          const uint8_t*  minutes,
                          const uint8_t*  seconds)
{
    if (years && *years > 0)
    {
        printf(" %" PRIu8 " year", *years);
        if (*years > 1)
        {
            printf("s");
        }
    }
    if (days && *days > 0)
    {
        printf(" %" PRIu16 " day", *days);
        if (*days > 1)
        {
            printf("s");
        }
    }
    if (hours && *hours > 0)
    {
        printf(" %" PRIu8 " hour", *hours);
        if (*hours > 1)
        {
            printf("s");
        }
    }
    if (minutes && *minutes > 0)
    {
        printf(" %" PRIu8 " minute", *minutes);
        if (*minutes > 1)
        {
            printf("s");
        }
    }
    if (seconds && *seconds > 0)
    {
        printf(" %" PRIu8 " second", *seconds);
        if (*seconds > 1)
        {
            printf("s");
        }
    }
    if (years || days || hours || minutes || seconds)
    {
        printf(" ");
    }
}

// helper functions for time.h functions to be safer depending on what is
// supported on a given system and compiler.
//__STDC_LIB_EXT1__ can be checked for the C11 _s functions. Otherwise, check
// posix definitions or C standard versions,
// or possible compiler specific definitions depending on what is available.

struct tm* impl_safe_gmtime(const time_t* M_RESTRICT timer,
                            struct tm* M_RESTRICT    buf,
                            const char*              file,
                            const char*              function,
                            int                      line,
                            const char*              expression)
{
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (timer == M_NULLPTR)
    {
        errno = EINVAL;
        invoke_Constraint_Handler("safe_gmtime: timer = NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  errno);
        return M_NULLPTR;
    }
    else if (buf == M_NULLPTR)
    {
        errno = EINVAL;
        invoke_Constraint_Handler("safe_gmtime: buf = NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  errno);
        return M_NULLPTR;
    }
    else
    {
#if (defined(POSIX_2001) && defined _POSIX_THREAD_SAFE_FUNCTIONS) || defined(USING_C23)
        // POSIX or C2x (C23 right now) have gmtime_r to use
        return gmtime_r(timer, buf);
#elif defined(HAVE_C11_ANNEX_K) || (defined(HAVE_MSFT_SECURE_LIB) && defined(_CRT_USE_CONFORMING_ANNEX_K_TIME) &&      \
                                    _CRT_USE_CONFORMING_ANNEX_K_TIME != 0)
        return gmtime_s(timer, buf);
#elif defined(HAVE_MSFT_SECURE_LIB)
        // If MSFT CRT available, use microsoft gmtime_s which is incompatible
        // with the standard
        if (0 != gmtime_s(buf, timer))
        {
            return M_NULLPTR;
        }
#else
        // There may be better options beyond what is done below, but it may be
        // a per-system implementation or something like that
        //       So there is room for improvement in this fallback function
        // Last thing we can do is use the unsafe version and copy it
        // immediately to our own buffer if nothing else is possible
        safe_memcpy(buf, sizeof(struct tm), gmtime(timer), sizeof(struct tm));
#endif
    }
    RESTORE_NONNULL_COMPARE
    return buf;
}

struct tm* impl_safe_localtime(const time_t* M_RESTRICT timer,
                               struct tm* M_RESTRICT    buf,
                               const char*              file,
                               const char*              function,
                               int                      line,
                               const char*              expression)
{
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (timer == M_NULLPTR)
    {
        errno = EINVAL;
        invoke_Constraint_Handler("safe_localtime: timer = NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), errno);
        return M_NULLPTR;
    }
    else if (buf == M_NULLPTR)
    {
        errno = EINVAL;
        invoke_Constraint_Handler("safe_localtime: buf = NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), errno);
        return M_NULLPTR;
    }
    else
    {
#if (defined(POSIX_2001) && defined _POSIX_THREAD_SAFE_FUNCTIONS) || defined(USING_C23)
        // POSIX or C2x (C23 right now) have localtime_r to use
        return localtime_r(timer, buf);
#elif defined(HAVE_C11_ANNEX_K) || (defined(HAVE_MSFT_SECURE_LIB) && defined(_CRT_USE_CONFORMING_ANNEX_K_TIME) &&      \
                                    _CRT_USE_CONFORMING_ANNEX_K_TIME != 0)
        return localtime_s(timer, buf);
#elif defined(HAVE_MSFT_SECURE_LIB)
        // If MSFT CRT available, use microsoft localtime_s which is
        // incompatible with the standard
        if (0 != localtime_s(buf, timer))
        {
            return M_NULLPTR;
        }
#else
        // There may be better options beyond what is done below, but it may be
        // a per-system implementation or something like that
        //       So there is room for improvement in this fallback function
        // Last thing we can do is use the unsafe version and copy it
        // immediately to our own buffer if nothing else is possible
        safe_memcpy(buf, sizeof(struct tm), localtime(timer), sizeof(struct tm));
#endif
    }
    RESTORE_NONNULL_COMPARE
    return buf;
}

errno_t impl_safe_asctime(char*            buf,
                          rsize_t          bufsz,
                          const struct tm* time_ptr,
                          bool             ctime,
                          const char*      file,
                          const char*      function,
                          int              line,
                          const char*      expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (buf == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: buf = NULL" : "safe_asctime: buf = NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (time_ptr == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: time_ptr = NULL" : "safe_asctime: time_ptr = NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (bufsz < 26)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: bufsz < 26" : "safe_asctime: bufsz < 26",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (bufsz > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: bufsz > RSIZE_MAX" : "safe_asctime: bufsz > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (time_ptr->tm_sec < 0 ||
#if defined(USING_C99)
             time_ptr->tm_sec > 60
#else
             time_ptr->tm_sec > 61
#endif
    )
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: time_ptr->tm_sec out of range"
                                        : "safe_asctime: time_ptr->tm_sec out of range",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (time_ptr->tm_min < 0 || time_ptr->tm_min > 59)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: time_ptr->tm_min out of range"
                                        : "safe_asctime: time_ptr->tm_min out of range",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (time_ptr->tm_hour < 0 || time_ptr->tm_hour > 23)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: time_ptr->tm_hour out of range"
                                        : "safe_asctime: time_ptr->tm_hour out of range",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (time_ptr->tm_mday < 1 || time_ptr->tm_mday > 31)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: time_ptr->tm_mday out of range"
                                        : "safe_asctime: time_ptr->tm_mday out of range",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (time_ptr->tm_mon < 0 || time_ptr->tm_mon > 11)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: time_ptr->tm_mon out of range"
                                        : "safe_asctime: time_ptr->tm_mon out of range",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (time_ptr->tm_wday < 0 || time_ptr->tm_wday > 6)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: time_ptr->tm_wday out of range"
                                        : "safe_asctime: time_ptr->tm_wday out of range",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (time_ptr->tm_yday < 0 || time_ptr->tm_yday > 365)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: time_ptr->tm_yday out of range"
                                        : "safe_asctime: time_ptr->tm_yday out of range",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (time_ptr->tm_year < 0 || time_ptr->tm_year > 9999)
    {
        error = EINVAL;
        invoke_Constraint_Handler(ctime ? "safe_ctime: time_ptr->tm_year out of range"
                                        : "safe_asctime: time_ptr->tm_year out of range",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
        // Perform the conversion
        // start with a known zeroed buffer
        safe_memset(buf, bufsz, 0, bufsz);
#if defined(HAVE_MSFT_SECURE_LIB) || defined(HAVE_C11_ANNEX_K)
        if (0 != asctime_s(buf, bufsz, time_ptr))
        {
            // error
            safe_memset(buf, bufsz, 0, bufsz);
            error = EINVAL;
            errno = error;
            return error;
        }
        else
        {
            return error;
        }
#else
        //  strftime is recommended to be used. Using format %c will return the
        //  matching output for this function asctime (which this replaces uses
        //  the format: Www Mmm dd hh:mm:ss yyyy\n) NOTE: %e is C99. C89's closest
        //  is %d which has a leading zero. To be technically correct in replacing
        //  this, need to set locale to english instead of localized locale first.
        //  After that is done, it can be reverted back.
        bool  restoreLocale = false;
        char* currentLocale = M_NULLPTR;
        // Turn off constraint handling since it is possible we will receive NULL and want to continue execution -TJE
        eConstraintHandler currentHandler = set_Constraint_Handler(ERR_IGNORE);
        error                             = safe_strdup(&currentLocale, setlocale(LC_TIME, M_NULLPTR));
        // Restore previous constraint handler
        set_Constraint_Handler(currentHandler);
        if (error == 0 && currentLocale != M_NULLPTR && setlocale(LC_TIME, "en-us.utf8"))
        {
            restoreLocale = true;
        }
#    if (defined(USING_C99) && !defined(__MINGW32__)) // MINGW in msys2 is warning it does not know %e, so
                                                      // this is catching that. This will likely need a
                                                      // version check whenever it gets fixed-TJE
        if (0 == strftime(buf, bufsz, "%a %b %e %H:%M:%S %Y", time_ptr))
#    else
        if (0 == strftime(buf, bufsz, "%a %b %d %H:%M:%S %Y", time_ptr))
#    endif
        {
            // This means the array was too small...which shouldn't happen...but
            // in case it does, zero out the memory
            safe_memset(buf, bufsz, 0, bufsz);
            error = EINVAL;
            errno = error;
        }
        if (restoreLocale)
        {
            // Casting to void because we are simply restoring the original
            // locale which should work without an issue at this point
            M_STATIC_CAST(void, setlocale(LC_TIME, currentLocale));
        }
        safe_free(&currentLocale);
#endif // HAVE_MSFT_SECURE_LIB || HAVE_C11_ANNEX_K
    }
    RESTORE_NONNULL_COMPARE
    return error;
}

errno_t impl_safe_ctime(char*         buf,
                        rsize_t       bufsz,
                        const time_t* timer,
                        const char*   file,
                        const char*   function,
                        int           line,
                        const char*   expression)
{
    errno_t error;
    DISABLE_NONNULL_COMPARE
    if (timer == M_NULLPTR)
    {
        constraintEnvInfo envInfo;
        error = EINVAL;
        invoke_Constraint_Handler("safe_ctime: timer = NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else
    {
        struct tm cTimeBuf;
        safe_memset(&cTimeBuf, sizeof(struct tm), 0, sizeof(struct tm));
        error = impl_safe_asctime(buf, bufsz, get_Localtime(timer, &cTimeBuf), true, file, function, line, expression);
    }
    RESTORE_NONNULL_COMPARE
    return error;
}

time_t get_Future_Date_And_Time(time_t inputTime, uint64_t secondsInTheFuture)
{
    uint16_t  days    = UINT16_C(0);
    uint8_t   years   = UINT8_C(0);
    uint8_t   months  = UINT8_C(0);
    uint8_t   hours   = UINT8_C(0);
    uint8_t   minutes = UINT8_C(0);
    uint8_t   seconds = UINT8_C(0);
    struct tm futureTime;
    safe_memset(&futureTime, sizeof(struct tm), 0, sizeof(struct tm));
    get_Localtime(C_CAST(const time_t*, &inputTime), &futureTime);
    // first break the input time into seperate units
    convert_Seconds_To_Displayable_Time(secondsInTheFuture, &years, &days, &hours, &minutes, &seconds);
    // now start setting the future time struct and checking when incrementing
    // things for overflow start with seconds
    if (seconds > 0)
    {
        if (futureTime.tm_sec + seconds > 59)
        {
            // increment minutes since we will adjust that next
            minutes++;
            futureTime.tm_sec = futureTime.tm_sec + seconds - 60;
        }
        else
        {
            futureTime.tm_sec += seconds;
        }
    }
    // now minutes
    if (minutes > 0)
    {
        if (futureTime.tm_min + minutes > 59)
        {
            hours++;
            futureTime.tm_min = futureTime.tm_min + minutes - 60;
        }
        else
        {
            futureTime.tm_min += minutes;
        }
    }
    // now hours
    if (hours > 0)
    {
        if (futureTime.tm_hour + hours > 23)
        {
            days++;
            futureTime.tm_hour = futureTime.tm_hour + hours - 24;
        }
        else
        {
            futureTime.tm_hour += hours;
        }
    }
    // now days (including week day information)
    if (days > 0)
    {
        // day of the year
        if (futureTime.tm_yday + days > 365)
        {
            years++;
            futureTime.tm_yday = futureTime.tm_yday + days - 366;
        }
        else
        {
            futureTime.tm_yday += days;
        }
        // day of the week
        if (futureTime.tm_wday + days > 6)
        {
            futureTime.tm_wday = futureTime.tm_wday + days - 7;
        }
        else
        {
            futureTime.tm_wday += days;
        }
        // day of the month (depends on the month!)
        uint8_t numberOfDaysInMonth = UINT8_C(31); // assume the most common
        switch (futureTime.tm_mon)
        {
        case 0:  // january
        case 2:  // march
        case 4:  // may
        case 6:  // july
        case 7:  // august
        case 9:  // october
        case 11: // december
            // 31 days a month
            numberOfDaysInMonth = 31;
            break;
        case 3:  // april
        case 5:  // june
        case 8:  // september
        case 10: // november
            // 30 days a month
            numberOfDaysInMonth = 30;
            break;
        case 1: // february
            // 28 days a month
            // NOLINTBEGIN(bugprone-branch-clone)
            //  disable clang-tidy here because while some conditions are
            //  duplicates, it is designed this way for both readability and the
            //  order in which the checks are performed. leap year detection has
            //  some specific rules and this was a more clear way to write
            //  them-TJE
            if (futureTime.tm_year % 400 == 0)
            {
                // leap year
                numberOfDaysInMonth = 29;
            }
            else if (futureTime.tm_year % 100 == 0)
            {
                // not a leap year
                numberOfDaysInMonth = 28;
            }
            else if (futureTime.tm_year % 4 == 0)
            {
                // leap year
                numberOfDaysInMonth = 29;
            }
            else
            {
                // not a leap year
                numberOfDaysInMonth = 28;
            }
            // NOLINTEND(bugprone-branch-clone)
            break;
        default: // do nothing
            break;
        }
        if (futureTime.tm_mday + days > numberOfDaysInMonth)
        {
            months++;
            futureTime.tm_mday = futureTime.tm_mday + days - (numberOfDaysInMonth + 1);
        }
        else
        {
            futureTime.tm_mday += days;
        }
    }
    // now months
    if (months > 0)
    {
        if (futureTime.tm_mon + months > 11)
        {
            years++;
            futureTime.tm_mon = futureTime.tm_mon + months - 12;
        }
        else
        {
            futureTime.tm_mon += months;
        }
    }
    // now years
    if (years > 0)
    {
        // since 1900...just increment since no range applies today
        futureTime.tm_year += years;
    }
    return mktime(&futureTime);
}
