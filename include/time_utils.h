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
// \file time_utils.h
// \brief Implements best practices for working with time functions in stdc library

#pragma once

#include <time.h>
#include "common_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

    #define TIME_STRING_LENGTH 26
    #define CURRENT_TIME_STRING_LENGTH TIME_STRING_LENGTH
    // global timestamp
    extern time_t CURRENT_TIME;
    extern char CURRENT_TIME_STRING[CURRENT_TIME_STRING_LENGTH];

    // get the current timestamp and use same for all log pulls and utility banner.
    bool get_current_timestamp(void);

    //meant to replace calling gmtime() function as this handles all the cross-platform weirdness calling the safest possible version
    struct tm* get_UTCtime(const time_t* timer, struct tm* buf);

    //meant to replace calling localtime() function as this handles all the cross-platform weirdness calling the safest possible version
    struct tm* get_Localtime(const time_t* timer, struct tm* buf);

    //meant to replace calling asctime() function as this handles all the cross-platform weirdness calling the safest possible version
    char* get_Time_String_From_TM_Structure(const struct tm* timeptr, char* buffer, size_t bufferSize);

    //meant to replace calling ctime() function as this handles all the cross-platform weirdness calling the safest possible version
    char* get_Current_Time_String(const time_t* timer, char* buffer, size_t bufferSize);

    //-----------------------------------------------------------------------------
    //
    //  get_Future_Date_And_Time()
    //
    //! \brief   Description:  Takes an input time, and an amount of time in the future, and returns it in a time_t type. This function exists because C standard does not specify how time_t is stored.
    //!                        POSIX specifies it's a number of seconds since Jan 1, 1970 UTC, but that is not portable which is why this function uses ctime to try to be portable regardless of how time_t is stored internally by the compiler.
    //
    //  Entry:
    //!   \param[in] inputTime = time_t value representing a time to use as the input for the future time calculation
    //!   \param[in] secondsInTheFuture = the number of seconds in the future. This must be a uint64_t and not a time_t value as a system may represent time_t oddly.
    //!
    //  Exit:
    //!   \return time_t value representing a time in the future
    //
    //-----------------------------------------------------------------------------
    time_t get_Future_Date_And_Time(time_t inputTime, uint64_t secondsInTheFuture);

    //-----------------------------------------------------------------------------
    //
    //  convert_Seconds_To_Displayable_Time()
    //
    //! \brief   Description:  Takes a time in seconds and converts it to something more friendly to display. Any input M_NULLPTR pointers will be skipped.
    //
    //  Entry:
    //!   \param[in] secondsToConvert - a value specifying the number of seconds to convert
    //!   \param[out] years = this is a pointer to a value to hold a number representing years. Can be M_NULLPTR.
    //!   \param[out] days = this is a pointer to a value to hold a number representing days. Can be M_NULLPTR.
    //!   \param[out] hours = this is a pointer to a value to hold a number representing hours. Can be M_NULLPTR.
    //!   \param[out] minutes = this is a pointer to a value to hold a number representing minutes. Can be M_NULLPTR.
    //!   \param[out] seconds = this is a pointer to a value to hold a number representing seconds. Can be M_NULLPTR.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void convert_Seconds_To_Displayable_Time(uint64_t secondsToConvert, uint8_t* years, uint16_t* days, uint8_t* hours, uint8_t* minutes, uint8_t* seconds);

    void convert_Seconds_To_Displayable_Time_Double(double secondsToConvert, uint8_t* years, uint16_t* days, uint8_t* hours, uint8_t* minutes, uint8_t* seconds);

    //-----------------------------------------------------------------------------
    //
    //  print_Time_To_Screen()
    //
    //! \brief   Description:  Takes a time represented by pointers and prints it to the screen in a friendly way. It will print a space before and after the time for you. If you do not wish to show one of the parameters, it may be null as it will be skipped. Values of zero are also skipped.
    //
    //  Entry:
    //!   \param[in] years = this is a pointer to a value holding a number representing years. Can be M_NULLPTR.
    //!   \param[in] days = this is a pointer to a value holding a number representing days. Can be M_NULLPTR.
    //!   \param[in] hours = this is a pointer to a value holding a number representing hours. Can be M_NULLPTR.
    //!   \param[in] minutes = this is a pointer to a value holding a number representing minutes. Can be M_NULLPTR.
    //!   \param[in] seconds = this is a pointer to a value holding a number representing seconds. Can be M_NULLPTR.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Time_To_Screen(uint8_t* years, uint16_t* days, uint8_t* hours, uint8_t* minutes, uint8_t* seconds);

    //returns number of milliseconds since Jan 1, 1970 UTC
    //If an error occurs, returns zero
    uint64_t get_Milliseconds_Since_Unix_Epoch(void);

#if defined (__cplusplus)
}
#endif
