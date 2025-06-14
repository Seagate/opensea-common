// SPDX-License-Identifier: MPL-2.0

//! \file time_utils.h
//! \brief Implements best practices for working with time functions in stdc library
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "common_types.h"
#include "impl_time_utils.h"

#include <time.h>

#if defined(__cplusplus)
extern "C"
{
#endif

//! \def TIME_STRING_LENGTH
//! \brief number of bytes to allocate for a string to hold the a time string including null terminator.
#define TIME_STRING_LENGTH 26

//! \def CURRENT_TIME_STRING_LENGTH
//! \brief number of bytes to allocate for a string to hold the current time including null terminator.
#define CURRENT_TIME_STRING_LENGTH TIME_STRING_LENGTH

    //! \var CURRENT_TIME
    //! \brief Global variable of time_t. Set by get_current_timestamp.
    extern time_t CURRENT_TIME;
    //! \var CURRENT_TIME_STRING
    //! \brief Global variable of char array. Set by get_current_timestamp as a human readable time
    extern char CURRENT_TIME_STRING[CURRENT_TIME_STRING_LENGTH];

    //! \fn bool get_current_timestamp(void)
    //! \brief sets the global CURRENT_TIME and CURRENT_TIME_STRING variables
    //!
    //! This is used so that it can be called at the start of execution and reuse the same timestamp
    //! until the utility has exited.
    //! \return true = successfully set CURRENT_TIME_STRING, false = failure to write formatted time string
    bool get_current_timestamp(void);

#if defined(DEV_ENVIRONMENT)
    //! \fn struct tm* safe_gmtime(const time_t* M_RESTRICT timer, struct tm* M_RESTRICT buf)
    //! \brief Get the current UTC time as struct tm
    //! \details This function wraps impl_safe_gmtime to provide the current UTC time.
    //! \param[in] timer pointer to time_t object to convert
    //! \param[in] buf pointer to user allocated struct tm to store the conversion result
    //! \return NULL on error or pointer to user provided \a buf
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a timer is a null pointer
    //!
    //! - \a buf is a null pointer
    static M_INLINE struct tm* safe_gmtime(const time_t* M_RESTRICT timer, struct tm* M_RESTRICT buf)
    {
        return impl_safe_gmtime(timer, buf, __FILE__, __func__, __LINE__, "safe_gmtime(timer, buf)");
    }
#else
//! \def safe_gmtime
//! \brief Get the current UTC time as struct tm
//! \details This definition wraps impl_safe_gmtime to provide the current UTC time.
//! \param[in] timer pointer to time_t object to convert
//! \param[in] buf pointer to user allocated struct tm to store the conversion result
//! \return NULL on error or pointer to user provided \a buf
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a timer is a null pointer
//!
//! - \a buf is a null pointer
#    define safe_gmtime(timer, buf)                                                                                    \
        impl_safe_gmtime(timer, buf, __FILE__, __func__, __LINE__, "safe_gmtime(" #timer ", " #buf ")")
#endif

//! \def get_UTCtime(timer, buf)
//! \brief Convenience macro that calls safe_gmtime.
//! \sa safe_gmtime
#define get_UTCtime(timer, buf) safe_gmtime(timer, buf)

#if defined(DEV_ENVIRONMENT)
    //! \fn struct tm* safe_localtime(const time_t* M_RESTRICT timer, struct tm* M_RESTRICT buf)
    //! \brief Get the current local time as struct tm
    //! \details This function wraps impl_safe_localtime to provide the current Local time.
    //! \param[in] timer pointer to time_t object to convert
    //! \param[in] buf pointer to user allocated struct tm to store the conversion result
    //! \return NULL on error or pointer to user provided \a buf
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a timer is a null pointer
    //!
    //! - \a buf is a null pointer
    static M_INLINE struct tm* safe_localtime(const time_t* M_RESTRICT timer, struct tm* M_RESTRICT buf)
    {
        return impl_safe_localtime(timer, buf, __FILE__, __func__, __LINE__, "safe_localtime(timer, buf)");
    }
#else
//! \def safe_localtime
//! \brief Get the current local time as struct tm
//! \details This function wraps impl_safe_localtime to provide the current Local time.
//! \param[in] timer pointer to time_t object to convert
//! \param[in] buf pointer to user allocated struct tm to store the conversion result
//! \return NULL on error or pointer to user provided \a buf
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a timer is a null pointer
//!
//! - \a buf is a null pointer
#    define safe_localtime(timer, buf)                                                                                 \
        impl_safe_localtime(timer, buf, __FILE__, __func__, __LINE__, "safe_localtime(" #timer ", " #buf ")")
#endif

#define get_Localtime(timer, buf) safe_localtime(timer, buf)

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_asctime(char* buf, rsize_t bufsz, const struct tm* time_ptr)
    //! \brief convert struct tm to textual representation like asctime_s
    //!
    //! Use in place of asctime(). Performs conversion using asctime_s or strftime to behave
    //! as close as possible to the same as the standard.
    //! \param[in] buf pointer to char buffer to hold output string. Must be at least 26 characters in size
    //! \param[in] bufsz size of \a buf. Must be at least 26 and less than RSIZE_MAX
    //! \param[in] time_ptr pointer to struct tm to use for conversion
    //! \return error code describing the failure or zero for no error.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a buf is a null pointer
    //!
    //! - \a time_ptr is a null pointer
    //!
    //! - \a bufsz < 26
    //!
    //! - \a bufsz > RSIZE_MAX
    //!
    //! - member of \a time_ptr is out of normal range
    static M_INLINE errno_t safe_asctime(char* buf, rsize_t bufsz, const struct tm* time_ptr)
    {
        return impl_safe_asctime(buf, bufsz, time_ptr, false, __FILE__, __func__, __LINE__,
                                 "safe_asctime(buf, bufsz, time_ptr)");
    }
#else

//! \def safe_asctime
//! \brief convert struct tm to textual representation like asctime_s
//!
//! Use in place of asctime(). Performs conversion using asctime_s or strftime to behave
//! as close as possible to the same as the standard.
//! \param[in] buf pointer to char buffer to hold output string. Must be at least 26 characters in size
//! \param[in] bufsz size of \a buf. Must be at least 26 and less than RSIZE_MAX
//! \param[in] time_ptr pointer to struct tm to use for conversion
//! \return error code describing the failure or zero for no error.
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a buf is a null pointer
//!
//! - \a time_ptr is a null pointer
//!
//! - \a bufsz < 26
//!
//! - \a bufsz > RSIZE_MAX
//!
//! - member of \a time_ptr is out of normal range
#    define safe_asctime(buf, bufsz, time_ptr)                                                                         \
        impl_safe_asctime(buf, bufsz, time_ptr, false, __FILE__, __func__, __LINE__,                                   \
                          "safe_asctime(" #buf ", " #bufsz ", " #time_ptr ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_ctime(char* buf, rsize_t bufsz, const time_t* timer)
    //! \brief convert struct tm to textual representation like ctime_s
    //!
    //! Use in place of ctime(). Performs conversion using ctime_s or strftime to behave
    //! as close as possible to the same as the standard.
    //! \param[in] buf pointer to char buffer to hold output string. Must be at least 26 characters in size
    //! \param[in] bufsz size of \a buf. Must be at least 26 and less than RSIZE_MAX
    //! \param[in] timer pointer to time_t to use for conversion.
    //! \return error code describing the failure or zero for no error.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a buf is a null pointer
    //!
    //! - \a timer is a null pointer
    //!
    //! - \a bufsz < 26
    //!
    //! - \a bufsz > RSIZE_MAX
    static M_INLINE errno_t safe_ctime(char* buf, rsize_t bufsz, const time_t* timer)
    {
        return impl_safe_ctime(buf, bufsz, timer, __FILE__, __func__, __LINE__, "safe_ctime(buf, bufsz, timer)");
    }
#else
//! \def safe_ctime
//! \brief convert struct tm to textual representation like ctime_s
//!
//! Use in place of ctime(). Performs conversion using ctime_s or strftime to behave
//! as close as possible to the same as the standard.
//! \param[in] buf pointer to char buffer to hold output string. Must be at least 26 characters in size
//! \param[in] bufsz size of \a buf. Must be at least 26 and less than RSIZE_MAX
//! \param[in] timer pointer to time_t to use for conversion.
//! \return error code describing the failure or zero for no error.
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a buf is a null pointer
//!
//! - \a timer is a null pointer
//!
//! - \a bufsz < 26
//!
//! - \a bufsz > RSIZE_MAX
#    define safe_ctime(buf, bufsz, timer)                                                                              \
        impl_safe_ctime(buf, bufsz, timer, __FILE__, __func__, __LINE__, "safe_ctime(" #buf ", " #bufsz ", " #timer ")")
#endif

    //! \fn char* get_Time_String_From_TM_Structure(const struct tm* timeptr, char* buffer, size_t bufferSize)
    //! \brief helper function wrapping safe_asctime that returns buffer on success
    //!
    //! \param[in] timeptr pointer to struct tm to use for conversion.
    //! \param[in] buffer pointer to char buffer to hold output string. Must be at least 26 characters in size
    //! \param[in] bufferSize size of \a buffer. Must be at least 26 and less than RSIZE_MAX
    //! \return pointer to converted string on success. Null pointer on failure.
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RO(1)
    M_PARAM_WO_SIZE(2, 3)
    static M_INLINE char* get_Time_String_From_TM_Structure(const struct tm* timeptr, char* buffer, size_t bufferSize)
    {
        if (0 == safe_asctime(buffer, bufferSize, timeptr))
        {
            return buffer;
        }
        else
        {
            return M_NULLPTR;
        }
    }

    //! \fn char* get_Current_Time_String(const time_t* timer, char* buffer, size_t bufferSize)
    //! \brief helper function wrapping safe_ctime that returns buffer on success
    //!
    //! \param[in] timer pointer to time_t to use for conversion.
    //! \param[in] buffer pointer to char buffer to hold output string. Must be at least 26 characters in size
    //! \param[in] bufferSize size of \a buffer. Must be at least 26 and less than RSIZE_MAX
    //! \return pointer to converted string on success. Null pointer on failure.
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RO(1)
    M_PARAM_WO_SIZE(2, 3)
    static M_INLINE char* get_Current_Time_String(const time_t* timer, char* buffer, size_t bufferSize)
    {
        if (0 == safe_ctime(buffer, bufferSize, timer))
        {
            return buffer;
        }
        else
        {
            return M_NULLPTR;
        }
    }

    //! \fn time_t get_Future_Date_And_Time(time_t inputTime, uint64_t secondsInTheFuture)
    //! \brief   Takes an input time, and an amount of time in the
    //! future, and returns it in a time_t type.
    //!
    //! This function exists because C standard does not specify how time_t is stored and may vary by implementation.
    //! POSIX specifies it's a number of seconds since Jan 1,
    //! 1970 UTC, but that is not portable which is why this
    //! function uses ctime to try to be portable regardless
    //! of how time_t is stored internally by the compiler.
    //! \param[in] inputTime time_t value representing a time to use as the
    //! input for the future time calculation
    //! \param[in] secondsInTheFuture = the number of seconds in the future.
    //! \return time_t value representing a time in the future
    time_t get_Future_Date_And_Time(time_t inputTime, uint64_t secondsInTheFuture);

    //! \fn void convert_Seconds_To_Displayable_Time(uint64_t  secondsToConvert,
    //!                                              uint8_t*  years,
    //!                                              uint16_t* days,
    //!                                              uint8_t*  hours,
    //!                                              uint8_t*  minutes,
    //!                                              uint8_t*  seconds)
    //! \brief Takes a time in seconds and converts it to something
    //! more friendly to display. Any input M_NULLPTR pointers will be skipped.
    //! \param[in] secondsToConvert - a value specifying the number of seconds to convert
    //! \param[out] years = this is a pointer to a value to hold a number representing years. Can be M_NULLPTR.
    //! \param[out] days = this is a pointer to a value to hold a number representing days. Can be M_NULLPTR.
    //! \param[out] hours = this is a pointer to a value to hold a number representing hours. Can be M_NULLPTR.
    //! \param[out] minutes = this is a pointer to a value to hold a number representing minutes. Can be M_NULLPTR.
    //! \param[out] seconds = this is a pointer to a value to hold a number representing seconds. Can be M_NULLPTR.
    M_PARAM_WO(2)
    M_PARAM_WO(3)
    M_PARAM_WO(4)
    M_PARAM_WO(5)
    M_PARAM_WO(6)
    void convert_Seconds_To_Displayable_Time(uint64_t  secondsToConvert,
                                             uint8_t*  years,
                                             uint16_t* days,
                                             uint8_t*  hours,
                                             uint8_t*  minutes,
                                             uint8_t*  seconds);

    //! \fn void convert_Seconds_To_Displayable_Time_Double(double  secondsToConvert,
    //!                                              uint8_t*  years,
    //!                                              uint16_t* days,
    //!                                              uint8_t*  hours,
    //!                                              uint8_t*  minutes,
    //!                                              uint8_t*  seconds)
    //! \brief Takes a time in seconds and converts it to something
    //! more friendly to display. Any input M_NULLPTR pointers will be skipped.
    //! \param[in] secondsToConvert - a value specifying the number of seconds to convert
    //! \param[out] years = this is a pointer to a value to hold a number representing years. Can be M_NULLPTR.
    //! \param[out] days = this is a pointer to a value to hold a number representing days. Can be M_NULLPTR.
    //! \param[out] hours = this is a pointer to a value to hold a number representing hours. Can be M_NULLPTR.
    //! \param[out] minutes = this is a pointer to a value to hold a number representing minutes. Can be M_NULLPTR.
    //! \param[out] seconds = this is a pointer to a value to hold a number representing seconds. Can be M_NULLPTR.
    M_PARAM_WO(2)
    M_PARAM_WO(3)
    M_PARAM_WO(4)
    M_PARAM_WO(5)
    M_PARAM_WO(6)
    void convert_Seconds_To_Displayable_Time_Double(double    secondsToConvert,
                                                    uint8_t*  years,
                                                    uint16_t* days,
                                                    uint8_t*  hours,
                                                    uint8_t*  minutes,
                                                    uint8_t*  seconds);

    //! \fn void print_Time_To_Screen(const uint8_t*  years,
    //!                               const uint16_t* days,
    //!                               const uint8_t*  hours,
    //!                               const uint8_t*  minutes,
    //!                               const uint8_t*  seconds)
    //! \brief Takes a time represented by pointers and prints it to
    //! the screen in a friendly way. It will print a space before and after the
    //! time for you. If you do not wish to show one of the parameters, it may be
    //! null as it will be skipped. Values of zero are also skipped.
    //! \param[in] years = this is a pointer to a value to hold a number representing years. Can be M_NULLPTR.
    //! \param[in] days = this is a pointer to a value to hold a number representing days. Can be M_NULLPTR.
    //! \param[in] hours = this is a pointer to a value to hold a number representing hours. Can be M_NULLPTR.
    //! \param[in] minutes = this is a pointer to a value to hold a number representing minutes. Can be M_NULLPTR.
    //! \param[in] seconds = this is a pointer to a value to hold a number representing seconds. Can be M_NULLPTR.
    M_PARAM_RO(1)
    M_PARAM_RO(2)
    M_PARAM_RO(3)
    M_PARAM_RO(4)
    M_PARAM_RO(5)
    void print_Time_To_Screen(const uint8_t*  years,
                              const uint16_t* days,
                              const uint8_t*  hours,
                              const uint8_t*  minutes,
                              const uint8_t*  seconds);

    //! \fn uint64_t get_Milliseconds_Since_Unix_Epoch(void)
    //! \brief returns the number of milliseconds since January 1, 1970 UTC
    //! \return number of milliseconds. 0 if an error occurs
    uint64_t get_Milliseconds_Since_Unix_Epoch(void);

    //! \fn struct tm* milliseconds_Since_Unix_Epoch_To_Struct_TM(uint64_t milliseconds, struct tm *time)
    //! \brief converts a number of milliseconds representing time since the unix epoch of
    //! January 1, 1970 UTC to struct tm representing this value
    //! \param[in] milliseconds number of milliseconds since Jan 1, 1970 UTC to convert
    //! \param[in] time pointer to user allocated struct tm to fill in.
    //! \return pointer to user provided \a time parameter
    M_NONNULL_PARAM_LIST(2)
    M_PARAM_WO(2)
    struct tm* milliseconds_Since_Unix_Epoch_To_Struct_TM(uint64_t milliseconds, struct tm* time);

#if !defined(TIME_UTC)
// Checking for TIME_UTC definition in order to figure out if the C11 API for timespec_get is available

//! \def TIME_UTC
//! \brief C11 implementation specific value to use as base for timespec_get function
#    define TIME_UTC 1

#    if !defined(POSIX_1993) && !defined _POSIX_MONOTONIC_CLOCK
    //! \struct timespec
    //! \brief Structure representing a time value with seconds and nanoseconds.
    //!
    //! This structure is used to represent time with a resolution of seconds and nanoseconds.
    //! It is commonly used in functions like timespec_get to obtain high-resolution time values.
    //! This definition is provided for systems where timespec_get is not otherwise available.
    //!
    //! \var time_t timespec::tv_sec
    //! The number of seconds.
    //!
    //! \var int32_t timespec::tv_nsec
    //! The number of nanoseconds. Note that Windows uses long, but int32_t is used here to ensure 32-bit compatibility.
    struct timespec
    {
        time_t  tv_sec;  //!< Seconds
        int32_t tv_nsec; //!< Nanoseconds
    };

#    endif

#    define NEED_TIMESPEC_GET

    //! \fn int timespec_get(struct timespec *ts, int base)
    //! \brief   Sets the current calendar time in a timespec structure.
    //!
    //! This function sets the current calendar time in the given timespec structure
    //! based on the specified time base. It is useful for obtaining high-resolution
    //! time values.
    //! \param[in] ts Pointer to a timespec structure to be filled with the current time.
    //! \param[in] base The time base to use. Typically, TIME_UTC is used to specify
    //! Coordinated Universal Time (UTC).
    //! \return The value of \a base to indicate success (non-zero) or failure (zero).
    //! \see https://en.cppreference.com/w/c/chrono/timespec_get
    int timespec_get(struct timespec* ts, int base);
#endif // TIME_UTC

#if defined(__cplusplus)
}
#endif
