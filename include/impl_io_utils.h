// SPDX-License-Identifier: MPL-2.0

//! \file impl_io_utils.h
//! \brief Internal header for handling internal definitions for io_utils.h.
//!
//! This is an internal header to handle the internal definitions for io_utils.h.
//! Do NOT include this file directly or use these functions directly. Instead, include io_utils.h and use the calls
//! from there. The functions with "impl" in the name expect __FILE__, __func__, __LINE__, and an expression when
//! called, which get wrapped to make this easier for you to use.
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

#include <stdio.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \fn errno_t safe_fopen_impl(FILE* M_RESTRICT* M_RESTRICT streamptr,
    //!                             const char* M_RESTRICT       filename,
    //!                             const char* M_RESTRICT       mode,
    //!                             const char*                  file,
    //!                             const char*                  function,
    //!                             int                          line,
    //!                             const char*                  expression)
    //! \brief Opens a file with bounds checking.
    //!
    //! This function opens a file with bounds checking, similar to the C11 Annex K `fopen_s` function.
    //!
    //! \param[out] streamptr Pointer to the FILE pointer that will be set to the opened file.
    //! \param[in] filename The name of the file to open.
    //! \param[in] mode The mode in which to open the file.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a streamptr is a null pointer
    //!
    //! - \a filename is a null pointer
    //!
    //! - \a mode is a null pointer
    M_NONNULL_PARAM_LIST(1, 2, 3)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_PARAM_RO(3)
    M_NULL_TERM_STRING(1)
    M_NULL_TERM_STRING(2)
    errno_t safe_fopen_impl(FILE* M_RESTRICT* M_RESTRICT streamptr,
                            const char* M_RESTRICT       filename,
                            const char* M_RESTRICT       mode,
                            const char*                  file,
                            const char*                  function,
                            int                          line,
                            const char*                  expression);

    //! \fn errno_t safe_freopen_impl(FILE* M_RESTRICT* M_RESTRICT newstreamptr,
    //!                               const char* M_RESTRICT       filename,
    //!                               const char* M_RESTRICT       mode,
    //!                               FILE* M_RESTRICT             stream,
    //!                               const char*                  file,
    //!                               const char*                  function,
    //!                               int                          line,
    //!                               const char*                  expression)
    //! \brief Reopens a file with bounds checking.
    //!
    //! This function reopens a file with bounds checking, similar to the C11 Annex K `freopen_s` function.
    //!
    //! \param[out] newstreamptr Pointer to the FILE pointer that will be set to the reopened file.
    //! \param[in] filename The name of the file to reopen.
    //! \param[in] mode The mode in which to reopen the file.
    //! \param[in] stream The original file stream to be reopened.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a newstreamptr is a null pointer
    //!
    //! - \a stream is a null pointer
    //!
    //! - \a mode is a null pointer
    M_NONNULL_PARAM_LIST(1, 2, 3, 4)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_PARAM_RO(3)
    M_PARAM_RW(4)
    M_NULL_TERM_STRING(1)
    M_NULL_TERM_STRING(2)
    errno_t safe_freopen_impl(FILE* M_RESTRICT* M_RESTRICT newstreamptr,
                              const char* M_RESTRICT       filename,
                              const char* M_RESTRICT       mode,
                              FILE* M_RESTRICT             stream,
                              const char*                  file,
                              const char*                  function,
                              int                          line,
                              const char*                  expression);

#if defined(WANT_SAFE_TMPNAM)
    //! \fn errno_t safe_tmpnam_impl(char*       filename_s,
    //!                              rsize_t     maxsize,
    //!                              const char* file,
    //!                              const char* function,
    //!                              int         line,
    //!                              const char* expression)
    //! \brief Generates a temporary file name with bounds checking.
    //!
    //! This function generates a temporary file name with bounds checking, similar to the C11 Annex K `tmpnam_s`
    //! function.
    //!
    //! \param[out] filename_s The buffer to store the generated temporary file name.
    //! \param[in] maxsize The maximum size of the buffer.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a filename_s is a null pointer
    //!
    //! - \a maxsize is a greater than RSIZE_MAX
    //!
    //! - \a maxsize is less than the generated file name string
    M_NONNULL_PARAM_LIST(1)
    M_PARAM_RW_SIZE(1, 2)
    errno_t safe_tmpnam_impl(char*       filename_s,
                             rsize_t     maxsize,
                             const char* file,
                             const char* function,
                             int         line,
                             const char* expression);
#endif // WANT_SAFE_TMPNAM

    //! \fn errno_t safe_tmpfile_impl(FILE* M_RESTRICT* M_RESTRICT streamptr,
    //!                               const char*                  file,
    //!                               const char*                  function,
    //!                               int                          line,
    //!                               const char*                  expression)
    //! \brief Creates a temporary file with bounds checking.
    //!
    //! This function creates a temporary file with bounds checking, similar to the C11 Annex K `tmpfile_s` function.
    //!
    //! \param[out] streamptr Pointer to the FILE pointer that will be set to the created temporary file.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a streamptr is a null pointer
    M_NONNULL_PARAM_LIST(1)
    M_PARAM_RW(1)
    errno_t safe_tmpfile_impl(FILE* M_RESTRICT* M_RESTRICT streamptr,
                              const char*                  file,
                              const char*                  function,
                              int                          line,
                              const char*                  expression);

    //! \fn char* safe_gets_impl(char*       str,
    //!                          rsize_t     n,
    //!                          const char* file,
    //!                          const char* function,
    //!                          int         line,
    //!                          const char* expression)
    //! \brief Reads a string from stdin with bounds checking.
    //!
    //! This function reads a string from stdin with bounds checking, similar to the C11 Annex K `gets_s` function.
    //!
    //! \param[out] str The buffer to store the read string.
    //! \param[in] n The maximum number of characters to read.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return A pointer to the read string, or NULL if an error occurs.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a n is zero
    //!
    //! - \a n is a greater than RSIZE_MAX
    //!
    //! - \a str is a null pointer
    //!
    //! - endline or eof not encountered after storing `n - 1` characters to the buffer
    M_NONNULL_PARAM_LIST(1)
    M_PARAM_RW_SIZE(1, 2)
    char* safe_gets_impl(char*       str,
                         rsize_t     n,
                         const char* file,
                         const char* function,
                         int         line,
                         const char* expression);

    //! \fn errno_t safe_strtol_impl(long*                  value,
    //!                              const char* M_RESTRICT str,
    //!                              char** M_RESTRICT      endp,
    //!                              int                    base,
    //!                              const char*            file,
    //!                              const char*            function,
    //!                              int                    line,
    //!                              const char*            expression)
    //! \brief Converts a string to a long integer with bounds checking.
    //!
    //! This function converts a string to a long integer with bounds checking, following ISO C secure coding practices.
    //!
    //! \param[out] value Pointer to the long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is a greater than 36
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtol_impl(long*                  value,
                             const char* M_RESTRICT str,
                             char** M_RESTRICT      endp,
                             int                    base,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    //! \fn errno_t safe_strtoll_impl(long long*             value,
    //!                               const char* M_RESTRICT str,
    //!                               char** M_RESTRICT      endp,
    //!                               int                    base,
    //!                               const char*            file,
    //!                               const char*            function,
    //!                               int                    line,
    //!                               const char*            expression)
    //! \brief Converts a string to a long long integer with bounds checking.
    //!
    //! This function converts a string to a long long integer with bounds checking, following ISO C secure coding
    //! practices.
    //!
    //! \param[out] value Pointer to the long long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is a greater than 36
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtoll_impl(long long*             value,
                              const char* M_RESTRICT str,
                              char** M_RESTRICT      endp,
                              int                    base,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    //! \fn errno_t safe_strtoul_impl(unsigned long*         value,
    //!                               const char* M_RESTRICT str,
    //!                               char** M_RESTRICT      endp,
    //!                               int                    base,
    //!                               const char*            file,
    //!                               const char*            function,
    //!                               int                    line,
    //!                               const char*            expression)
    //! \brief Converts a string to an unsigned long integer with bounds checking.
    //!
    //! This function converts a string to an unsigned long integer with bounds checking, following ISO C secure coding
    //! practices.
    //!
    //! \param[out] value Pointer to the unsigned long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is a greater than 36
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtoul_impl(unsigned long*         value,
                              const char* M_RESTRICT str,
                              char** M_RESTRICT      endp,
                              int                    base,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    //! \fn errno_t safe_strtoull_impl(unsigned long long*    value,
    //!                                const char* M_RESTRICT str,
    //!                                char** M_RESTRICT      endp,
    //!                                int                    base,
    //!                                const char*            file,
    //!                                const char*            function,
    //!                                int                    line,
    //!                                const char*            expression)
    //! \brief Converts a string to an unsigned long long integer with bounds checking.
    //!
    //! This function converts a string to an unsigned long long integer with bounds checking, following ISO C secure
    //! coding practices.
    //!
    //! \param[out] value Pointer to the unsigned long long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is a greater than 36
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtoull_impl(unsigned long long*    value,
                               const char* M_RESTRICT str,
                               char** M_RESTRICT      endp,
                               int                    base,
                               const char*            file,
                               const char*            function,
                               int                    line,
                               const char*            expression);

    //! \fn errno_t safe_strtoimax_impl(intmax_t*              value,
    //!                                 const char* M_RESTRICT str,
    //!                                 char** M_RESTRICT      endp,
    //!                                 int                    base,
    //!                                 const char*            file,
    //!                                 const char*            function,
    //!                                 int                    line,
    //!                                 const char*            expression)
    //! \brief Converts a string to an intmax_t integer with bounds checking.
    //!
    //! This function converts a string to an intmax_t integer with bounds checking, following ISO C secure coding
    //! practices.
    //!
    //! \param[out] value Pointer to the intmax_t integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is a greater than 36
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtoimax_impl(intmax_t*              value,
                                const char* M_RESTRICT str,
                                char** M_RESTRICT      endp,
                                int                    base,
                                const char*            file,
                                const char*            function,
                                int                    line,
                                const char*            expression);

    //! \fn errno_t safe_strtoumax_impl(uintmax_t*             value,
    //!                                 const char* M_RESTRICT str,
    //!                                 char** M_RESTRICT      endp,
    //!                                 int                    base,
    //!                                 const char*            file,
    //!                                 const char*            function,
    //!                                 int                    line,
    //!                                 const char*            expression)
    //! \brief Converts a string to a uintmax_t integer with bounds checking.
    //!
    //! This function converts a string to a uintmax_t integer with bounds checking, following ISO C secure coding
    //! practices.
    //!
    //! \param[out] value Pointer to the uintmax_t integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is a greater than 36
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtoumax_impl(uintmax_t*             value,
                                const char* M_RESTRICT str,
                                char** M_RESTRICT      endp,
                                int                    base,
                                const char*            file,
                                const char*            function,
                                int                    line,
                                const char*            expression);

    //! \fn errno_t safe_strtof_impl(float*                 value,
    //!                              const char* M_RESTRICT str,
    //!                              char** M_RESTRICT      endp,
    //!                              const char*            file,
    //!                              const char*            function,
    //!                              int                    line,
    //!                              const char*            expression)
    //! \brief Converts a string to a float with bounds checking.
    //!
    //! This function converts a string to a float with bounds checking, following ISO C secure coding practices.
    //!
    //! \param[out] value Pointer to the float that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtof_impl(float*                 value,
                             const char* M_RESTRICT str,
                             char** M_RESTRICT      endp,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    //! \fn errno_t safe_strtod_impl(double*                value,
    //!                              const char* M_RESTRICT str,
    //!                              char** M_RESTRICT      endp,
    //!                              const char*            file,
    //!                              const char*            function,
    //!                              int                    line,
    //!                              const char*            expression)
    //! \brief Converts a string to a double with bounds checking.
    //!
    //! This function converts a string to a double with bounds checking, following ISO C secure coding practices.
    //!
    //! \param[out] value Pointer to the double that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtod_impl(double*                value,
                             const char* M_RESTRICT str,
                             char** M_RESTRICT      endp,
                             const char*            file,
                             const char*            function,
                             int                    line,
                             const char*            expression);

    //! \fn errno_t safe_strtold_impl(long double*           value,
    //!                               const char* M_RESTRICT str,
    //!                               char** M_RESTRICT      endp,
    //!                               const char*            file,
    //!                               const char*            function,
    //!                               int                    line,
    //!                               const char*            expression)
    //! \brief Converts a string to a long double with bounds checking.
    //!
    //! This function converts a string to a long double with bounds checking, following ISO C secure coding practices.
    //!
    //! \param[out] value Pointer to the long double that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtold_impl(long double*           value,
                              const char* M_RESTRICT str,
                              char** M_RESTRICT      endp,
                              const char*            file,
                              const char*            function,
                              int                    line,
                              const char*            expression);

    //! \fn errno_t safe_atoi_impl(int*                   value,
    //!                            const char* M_RESTRICT str,
    //!                            const char*            file,
    //!                            const char*            function,
    //!                            int                    line,
    //!                            const char*            expression)
    //! \brief Converts a string to an integer with bounds checking.
    //!
    //! This function converts a string to an integer with bounds checking, following ISO C secure coding practices. It
    //! calls the constraint handlers for similar issues as safe_strtol. The only additional error it detects is if
    //! there is text still present after performing a conversion.
    //!
    //! \param[out] value Pointer to the integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - There is text still present after performing the conversion
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_atoi_impl(int*                   value,
                           const char* M_RESTRICT str,
                           const char*            file,
                           const char*            function,
                           int                    line,
                           const char*            expression);

    //! \fn errno_t safe_atol_impl(long*                  value,
    //!                            const char* M_RESTRICT str,
    //!                            const char*            file,
    //!                            const char*            function,
    //!                            int                    line,
    //!                            const char*            expression)
    //! \brief Converts a string to a long integer with bounds checking.
    //!
    //! This function converts a string to a long integer with bounds checking, following ISO C secure coding practices.
    //! It calls the constraint handlers for similar issues as safe_strtol. The only additional error it detects is if
    //! there is text still present after performing a conversion.
    //!
    //! \param[out] value Pointer to the long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - There is text still present after performing the conversion
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_atol_impl(long*                  value,
                           const char* M_RESTRICT str,
                           const char*            file,
                           const char*            function,
                           int                    line,
                           const char*            expression);

    //! \fn errno_t safe_atoll_impl(long long*             value,
    //!                             const char* M_RESTRICT str,
    //!                             const char*            file,
    //!                             const char*            function,
    //!                             int                    line,
    //!                             const char*            expression)
    //! \brief Converts a string to a long long integer with bounds checking.
    //!
    //! This function converts a string to a long long integer with bounds checking, following ISO C secure coding
    //! practices. It calls the constraint handlers for similar issues as safe_strtol. The only additional error it
    //! detects is if there is text still present after performing a conversion.
    //!
    //! \param[out] value Pointer to the long long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - There is text still present after performing the conversion
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_atoll_impl(long long*             value,
                            const char* M_RESTRICT str,
                            const char*            file,
                            const char*            function,
                            int                    line,
                            const char*            expression);

    //! \fn errno_t safe_atof_impl(double*                value,
    //!                            const char* M_RESTRICT str,
    //!                            const char*            file,
    //!                            const char*            function,
    //!                            int                    line,
    //!                            const char*            expression)
    //! \brief Converts a string to a double with bounds checking.
    //!
    //! This function converts a string to a double with bounds checking, following ISO C secure coding practices. It
    //! calls the constraint handlers for similar issues as safe_strtof. The only additional error it detects is if
    //! there is text still present after performing a conversion.
    //!
    //! \param[out] value Pointer to the double that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - There is text still present after performing the conversion
    M_NONNULL_PARAM_LIST(1, 2)
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_atof_impl(double*                value,
                           const char* M_RESTRICT str,
                           const char*            file,
                           const char*            function,
                           int                    line,
                           const char*            expression);

    //! \fn FUNC_ATTR_PRINTF(3, 4) int impl_snprintf_err_handle(const char* file,
    //!                                                         const char* function,
    //!                                                         int         line,
    //!                                                         const char* expression,
    //!                                                         char*       buf, 
    //!                                                         size_t      bufsize,
    //!                                                         const char* format, 
    //!                                                         ...)
    //! \brief Not exactly the same as snprintf_s, just checking that the return code is as expected when using this
    //! function to detect errors or truncation.
    //! \details This function does NOT do bounds checking.
    //! \see https://wiki.sei.cmu.edu/confluence/display/c/ERR33-C.+Detect+and+handle+standard+library+errors
    //! \todo Implement a safe_snprintf that works as C11 annex k specifies.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \param[out] buf TThe buffer to write the formatted string to.
    //! \param[in] bufsize The size of the buffer.
    //! \param[in] format The format string.
    //! \param[in] ... Additional arguments for the format string.
    //! \return The number of characters that would have been written if the buffer had been sufficiently large, not
    //! including the null terminator. On error, this will always add a null terminator at the end of the buffer.
    //! \note Invokes the constraint handler on error.
    M_NONNULL_PARAM_LIST(7)
    M_NONNULL_IF_NONZERO_PARAM(5, 6)
    M_NULL_TERM_STRING(7)
    M_PARAM_RW(5)
    M_PARAM_RO(7)
    FUNC_ATTR_PRINTF(7, 8) int impl_snprintf_err_handle(const char* file,
                             const char* function,
                             int         line,
                             const char* expression,
                             char*       buf, 
                             size_t      bufsize, 
                             const char* format, 
                             ...);

#if defined(__cplusplus)
}
#endif
