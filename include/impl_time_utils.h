// SPDX-License-Identifier: MPL-2.0

//! \file impl_time_utils.h
//! \brief This is an internal header to handle the internal definitions for time_utils.h
//!        Do NOT include this and use these functions directly. Include time_utils.h and use the calls from there
//!        The functions with "impl" in the name expect __FILE__, __func__, __LINE__, and an expression when called
//!        which get wrapped by time_utils.h to make this easier for you to use.
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \fn struct tm* impl_safe_gmtime(const time_t* M_RESTRICT timer,
    //!                                 struct tm* M_RESTRICT    buf,
    //!                                 const char*              file,
    //!                                 const char*              function,
    //!                                 int                      line,
    //!                                 const char*              expression)
    //! \brief Internal implementation of safe_gmtime.
    //!
    //! Use in place of gmtime(). Calls gmtime_r or gmtime_s internally.
    //! gmtime() uses an internal buffer and is not thread safe. This uses thread safe
    //! versions to read this time and handles platform differences.
    //! \param[in] timer pointer to time_t object to convert
    //! \param[in] buf pointer to user allocated struct tm to store the conversion result
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return NULL on error or pointer to user provided \a buf
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a timer is a null pointer
    //!
    //! - \a buf is a null pointer
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RO(1)
    M_PARAM_RW(2) struct tm* impl_safe_gmtime(const time_t* M_RESTRICT timer,
                                              struct tm* M_RESTRICT    buf,
                                              const char*              file,
                                              const char*              function,
                                              int                      line,
                                              const char*              expression);

    //! \fn struct tm* impl_safe_localtime(const time_t* M_RESTRICT timer,
    //!                                   struct tm* M_RESTRICT    buf,
    //!                                   const char*              file,
    //!                                   const char*              function,
    //!                                   int                      line,
    //!                                   const char*              expression)
    //! \brief Internal implementation of safe_localtime.
    //!
    //! Use in place of localtime(). Calls localtime_r or localtime_s internally.
    //! localtime() uses an internal buffer and is not thread safe. This uses thread safe
    //! versions to read this time and handles platform differences.
    //! \param[in] timer pointer to time_t object to convert
    //! \param[in] buf pointer to user allocated struct tm to store the conversion result
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return NULL on error or pointer to user provided \a buf
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a timer is a null pointer
    //!
    //! - \a buf is a null pointer
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RO(1)
    M_PARAM_RW(2) struct tm* impl_safe_localtime(const time_t* M_RESTRICT timer,
                                                 struct tm* M_RESTRICT    buf,
                                                 const char*              file,
                                                 const char*              function,
                                                 int                      line,
                                                 const char*              expression);

    //! \fn errno_t impl_safe_asctime(char*            buf,
    //!                               rsize_t          bufsz,
    //!                               const struct tm* time_ptr,
    //!                               bool             ctime,
    //!                               const char*      file,
    //!                               const char*      function,
    //!                               int              line,
    //!                               const char*      expression)
    //! \brief Internal implementation of safe_asctime with flag for when used as safe_ctime
    //!
    //! Use in place of asctime(). Performs conversion using asctime_s or strftime to behave
    //! as close as possible to the same as the standard.
    //! \param[in] buf pointer to char buffer to hold output string. Must be at least 26 characters in size
    //! \param[in] bufsz size of \a buf. Must be at least 26 and less than RSIZE_MAX
    //! \param[in] time_ptr pointer to struct tm to use for conversion
    //! \param[in] ctime true when calling this from safe_ctime, false for safe_asctime
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
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
    M_NONNULL_PARAM_LIST(1, 3)
    M_PARAM_RW_SIZE(1, 2)
    M_PARAM_RO(3) errno_t impl_safe_asctime(char*            buf,
                                            rsize_t          bufsz,
                                            const struct tm* time_ptr,
                                            bool             ctime,
                                            const char*      file,
                                            const char*      function,
                                            int              line,
                                            const char*      expression);

    //! \fn errno_t impl_safe_ctime(char*            buf,
    //!                               rsize_t          bufsz,
    //!                               const time_t* timer,
    //!                               const char*      file,
    //!                               const char*      function,
    //!                               int              line,
    //!                               const char*      expression)
    //! \brief Internal implementation of safe_ctime that calls impl_safe_asctime
    //!
    //! Use in place of ctime(). Performs conversion using asctime_s or strftime to behave
    //! as close as possible to the same as the standard.
    //! \param[in] buf pointer to char buffer to hold output string. Must be at least 26 characters in size
    //! \param[in] bufsz size of \a buf. Must be at least 26 and less than RSIZE_MAX
    //! \param[in] timer pointer to time_t to use for conversion
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
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
    M_NONNULL_PARAM_LIST(1, 3)
    M_PARAM_RW_SIZE(1, 2)
    M_PARAM_RO(3) errno_t impl_safe_ctime(char*         buf,
                                          rsize_t       bufsz,
                                          const time_t* timer,
                                          const char*   file,
                                          const char*   function,
                                          int           line,
                                          const char*   expression);

#if defined(__cplusplus)
}
#endif
