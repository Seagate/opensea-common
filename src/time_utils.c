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
// \file common_time.c
// \brief Implements best practices for working with time functions in stdc
// library

#include "time_utils.h"
#include "common_types.h"
#include "io_utils.h"
#include "memory_safety.h"
#include "type_conversion.h"

#include <locale.h> //used when getting time to replace ctime and asctime function to try and replicate the format exactly-TJE
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

time_t CURRENT_TIME = 0;
// NOTE: Do not use the DECLARE_ZERO_INIT_ARRAY here as it will not compile
// correctly for a global variable like this.
//       Instead we are manually setting all bytes to zero the old fashioned way
//       - TJE
char CURRENT_TIME_STRING[CURRENT_TIME_STRING_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

bool get_current_timestamp(void)
{
    bool retStatus = true;
    if (strcmp(CURRENT_TIME_STRING, "") == 0)
    {
        struct tm logTime;
        safe_memset(&logTime, sizeof(struct tm), 0, sizeof(struct tm));
        CURRENT_TIME = time(M_NULLPTR);
        safe_memset(CURRENT_TIME_STRING, sizeof(CURRENT_TIME_STRING) / sizeof(*CURRENT_TIME_STRING), 0,
                    sizeof(CURRENT_TIME_STRING) / sizeof(*CURRENT_TIME_STRING));
        retStatus = strftime(CURRENT_TIME_STRING, CURRENT_TIME_STRING_LENGTH, "%Y%m%dT%H%M%S",
                             get_Localtime(&CURRENT_TIME, &logTime));
    }
    return retStatus;
}

// returns number of milliseconds since 1970 UTC
uint64_t get_Milliseconds_Since_Unix_Epoch(void)
{
    uint64_t msSinceJan1970 = UINT64_C(0);
    // Check for C11's standardized API call.
    // Also check that TIME_UTC is defined, because if it is not, then this will
    // not work. This apparently causes an error in mingww64 environment in
    // msys2 since this function is missing, but C11 is defined.
#if defined(USING_C11) && defined(TIME_UTC)
    struct timespec now;
    safe_memset(&now, sizeof(struct timespec), 0, sizeof(struct timespec));
    if (0 != timespec_get(&now, TIME_UTC))
    {
        // NOTE: Technically this is a time since the system's epoch as C
        // standard does not set an epoch.
        //       I'm not sure there is a system we have encountered where it is
        //       not the Unix epoch, so this is fine for now - TJE
        msSinceJan1970 =
            (C_CAST(uint64_t, now.tv_sec) * UINT64_C(1000)) + (C_CAST(uint64_t, now.tv_nsec) / UINT64_C(1000000));
    }
    else
#endif // C11
    {
#if defined(POSIX_1993) && defined _POSIX_TIMERS
        // POSIX gettimeofday() or clock_gettime(CLOCK_REALTIME, ts)
        // https://pubs.opengroup.org/onlinepubs/9699919799/functions/clock_getres.html
        // NOTE: Using clock_gettime since it's more accurate and not affected
        // by time-skew
        struct timespec posixnow;
        safe_memset(&posixnow, sizeof(struct timespec), 0, sizeof(struct timespec));
        if (0 == clock_gettime(CLOCK_REALTIME, &posixnow))
        {
            msSinceJan1970 = (C_CAST(uint64_t, posixnow.tv_sec) * UINT64_C(1000)) +
                             (C_CAST(uint64_t, posixnow.tv_nsec) / UINT64_C(1000000));
        }
        else
#elif defined(_MSC_VER)
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
        epoch.wYear      = 1970;
        epoch.wMonth     = 1;
        epoch.wSecond    = 0; // MSFT says to set to first second.
        epoch.wDayOfWeek = 4; // Thursday
        epoch.wDay       = 1;
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
            msSinceJan1970 = C_CAST(uint64_t, (nowAsInt.QuadPart - epochAsInt.QuadPart)) /
                             UINT64_C(10000); // subtracting the 2 large integers gives 100
                                              // nanosecond intervals since jan 1, 1970, so
                                              // divide by 10000 to get milliseconds (divide
                                              // by 10000000 to get seconds)
        }
        else
#endif // OS unique methods to get this info
        {
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
            safe_memset(&nowstruct, sizeof(struct tm), 0, sizeof(struct tm));
            // to get the milliseconds, need to convert this to struct tm, then
            // calculate this.
            get_UTCtime(&currentTime, &nowstruct);
            uint64_t currentyear = (C_CAST(uint64_t, nowstruct.tm_year) + UINT64_C(1900));
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
                msSinceJan1970 = (C_CAST(uint64_t, nowstruct.tm_sec) * UINT64_C(1000)) +
                                 (C_CAST(uint64_t, nowstruct.tm_min) * UINT64_C(60000)) +
                                 (C_CAST(uint64_t, nowstruct.tm_hour) * UINT64_C(3600000)) +
                                 ((C_CAST(uint64_t, nowstruct.tm_yday) + leapyears) * UINT64_C(86400000)) +
                                 ((currentyear - UINT64_C(1970)) * UINT64_C(31536000000));
            }
            else
            {
                // some weird time skew issue or 32bit time_t 2038 issue, so
                // just return zero for an invalid value.-TJE NOTE: with a 32bit
                // time_t you could probably implement a clever workaround to
                // adjust it to a correct year, but that is not worth
                //       implmenting in this code. I don't even know which
                //       modern OS is using a 32bit time_t in 2024. -TJE If a
                //       workaround to adjust the year was implemented, it would
                //       likely only work until some future time where the same
                //       32bit problem happens again.-TJE
                msSinceJan1970 = UINT64_C(0);
            }
        }
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
    if (seconds)
    {
        *seconds = C_CAST(uint8_t, fmod(tempCalcValue, 60.0));
    }
    tempCalcValue /= 60.0;
    // get minutes up to a maximum of 60
    if (minutes)
    {
        *minutes = C_CAST(uint8_t, fmod(tempCalcValue, 60.0));
    }
    tempCalcValue /= 60.0;
    // get hours up to a maximum of 24
    if (hours)
    {
        *hours = C_CAST(uint8_t, fmod(tempCalcValue, 24.0));
    }
    tempCalcValue /= 24.0;
    // get days up to 365
    if (days)
    {
        *days = C_CAST(uint16_t, fmod(tempCalcValue, 365.0));
    }
    tempCalcValue /= 365.0;
    // get years
    if (years)
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
    if (seconds)
    {
        *seconds = C_CAST(uint8_t, tempCalcValue % UINT64_C(60));
    }
    tempCalcValue /= UINT64_C(60);
    // get minutes up to a maximum of 60
    if (minutes)
    {
        *minutes = C_CAST(uint8_t, tempCalcValue % UINT64_C(60));
    }
    tempCalcValue /= UINT64_C(60);
    // get hours up to a maximum of 24
    if (hours)
    {
        *hours = C_CAST(uint8_t, tempCalcValue % UINT64_C(24));
    }
    tempCalcValue /= UINT64_C(24);
    // get days up to 365
    if (days)
    {
        *days = C_CAST(uint16_t, tempCalcValue % UINT64_C(365));
    }
    tempCalcValue /= UINT64_C(365);
    // get years
    if (years)
    {
        *years = C_CAST(uint8_t, tempCalcValue);
    }
}

void print_Time_To_Screen(uint8_t* years, uint16_t* days, uint8_t* hours, uint8_t* minutes, uint8_t* seconds)
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

struct tm* get_UTCtime(const time_t* timer, struct tm* buf)
{
    // There are a few different things we can do in here to be more safe
    // depending on what is supported by the system and compiler, so there are
    // ifdefs to help with that... Microsoft CRT uses different parameter order!
    // May need to detect this if using the _s version in here
    if (timer && buf)
    {
#if (defined(POSIX_2001) && defined _POSIX_THREAD_SAFE_FUNCTIONS) || defined(USING_C23)
        // POSIX or C2x (C23 right now) have gmtime_r to use
        return gmtime_r(timer, buf);
#elif defined(HAVE_C11_ANNEX_K)
        return gmtime_s(timer, buf);
#elif defined(__STDC_SECURE_LIB__)
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
    return buf;
}

struct tm* get_Localtime(const time_t* timer, struct tm* buf)
{
    // There are a few different things we can do in here to be more safe
    // depending on what is supported by the system and compiler, so there are
    // ifdefs to help with that... Microsoft CRT uses different parameter order!
    // May need to detect this if using the _s version in here
    if (timer && buf)
    {
#if (defined(POSIX_2001) && defined _POSIX_THREAD_SAFE_FUNCTIONS) || defined(USING_C23)
        // POSIX or C2x (C23 right now) have localtime_r to use
        return localtime_r(timer, buf);
#elif defined(HAVE_C11_ANNEX_K)
        return localtime_s(timer, buf);
#elif defined(__STDC_SECURE_LIB__)
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
    return buf;
}

char* get_Time_String_From_TM_Structure(const struct tm* timeptr, char* buffer, size_t bufferSize)
{
    if (timeptr && buffer && bufferSize >= TIME_STRING_LENGTH)
    {
        // start with a known zeroed buffer
        safe_memset(buffer, bufferSize, 0, bufferSize);
#if defined(__STDC_SECURE_LIB__) || defined(HAVE_C11_ANNEX_K)
        if (0 != asctime_s(buffer, bufferSize, timeptr))
        {
            // error
            safe_memset(buffer, bufferSize, 0, bufferSize);
        }
        else
        {
            return buffer;
        }
#endif //__STDC_SECURE_LIB__ || HAVE_C11_ANNEX_K
       // strftime is recommended to be used. Using format %c will return the
       // matching output for this function asctime (which this replaces uses
       // the format: Www Mmm dd hh:mm:ss yyyy\n) NOTE: %e is C99. C89's closest
       // is %d which has a leading zero. To be technically correct in replacing
       // this, need to set locale to english instead of localized locale first.
       // After that is done, it can be reverted back.
        bool  restoreLocale = false;
        char* currentLocale = strdup(setlocale(LC_TIME, M_NULLPTR));
        if (setlocale(LC_TIME, "en-us.utf8"))
        {
            restoreLocale = true;
        }
#if (defined(USING_C99) && !defined(__MINGW32__)) // MINGW in msys2 is warning it does not know %e, so
                                                  // this is catching that. This will likely need a
                                                  // version check whenever it gets fixed-TJE
        if (0 == strftime(buffer, bufferSize, "%a %b %e %H:%M:%S %Y", timeptr))
#else
        if (0 == strftime(buffer, bufferSize, "%a %b %d %H:%M:%S %Y", timeptr))
#endif
        {
            // This means the array was too small...which shouldn't happen...but
            // in case it does, zero out the memory
            safe_memset(buffer, bufferSize, 0, bufferSize);
        }
        if (restoreLocale)
        {
            // Casting to void because we are simply restoring the original
            // locale which should work without an issue at this point
            M_STATIC_CAST(void, setlocale(LC_TIME, currentLocale));
        }
        safe_free(&currentLocale);
    }
    return buffer;
}

char* get_Current_Time_String(const time_t* timer, char* buffer, size_t bufferSize)
{
    if (timer && buffer && bufferSize >= CURRENT_TIME_STRING_LENGTH)
    {
        struct tm cTimeBuf;
        safe_memset(&cTimeBuf, sizeof(struct tm), 0, sizeof(struct tm));
        get_Time_String_From_TM_Structure(get_Localtime(timer, &cTimeBuf), buffer, bufferSize);
    }
    return buffer;
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
