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
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "type_conversion.h"

#include <stdarg.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C"
{
#endif

// Methods to detect errors at compile time for clang with diagnose_if attributes.
// This is not perfect, but having constexpr is far more helpful and truthful for these comparisons
// than the workaround. diagnose_if does not allow casts of any kinds so trying to compare directly
// to a NULL or M_NULLPTR is not allowed. !ptr is allowed, but may not work in some systems/libraries
// so using the constexpr is preferred since it is a more direct match for the error condition in all
// scenarios. - TJE

// Note: This can only be checked like this in both C23 with constexpr and without C23 due to the restrict qualifiers
// used on this type of check.
#define M_NULL_OUTSTREAM_CHECK(ptr) (!(ptr))

#if defined(HAVE_CONSTEXPR)
    M_ATTR_UNUSED constexpr FILE*               nullstreamptr   = M_NULLPTR;
    M_ATTR_UNUSED constexpr char*               nulliochar      = M_NULLPTR;
    M_ATTR_UNUSED constexpr const char*         nullioconstchar = M_NULLPTR;
    M_ATTR_UNUSED constexpr int*                nullint         = M_NULLPTR;
    M_ATTR_UNUSED constexpr long*               nulllong        = M_NULLPTR;
    M_ATTR_UNUSED constexpr long long*          nulllonglong    = M_NULLPTR;
    M_ATTR_UNUSED constexpr unsigned long*      nullulong       = M_NULLPTR;
    M_ATTR_UNUSED constexpr unsigned long long* nullulonglong   = M_NULLPTR;
    M_ATTR_UNUSED constexpr intmax_t*           nullintmax      = M_NULLPTR;
    M_ATTR_UNUSED constexpr uintmax_t*          nulluintmax     = M_NULLPTR;
    M_ATTR_UNUSED constexpr float*              nullfloat       = M_NULLPTR;
    M_ATTR_UNUSED constexpr double*             nulldouble      = M_NULLPTR;
    M_ATTR_UNUSED constexpr long double*        nullldouble     = M_NULLPTR;
#    define M_NULL_STREAM_CHECK(ptr)        ((ptr) == nullstreamptr)
#    define M_NULL_IO_CHAR_CHECK(ptr)       ((ptr) == nulliochar)
#    define M_NULL_IO_CONST_CHAR_CHECK(ptr) ((ptr) == nullioconstchar)
#    define M_NULL_IO_INT_CHECK(ptr)        ((ptr) == nullint)
#    define M_NULL_IO_LONG_CHECK(ptr)       ((ptr) == nulllong)
#    define M_NULL_IO_LONGLONG_CHECK(ptr)   ((ptr) == nulllonglong)
#    define M_NULL_IO_ULONG_CHECK(ptr)      ((ptr) == nullulong)
#    define M_NULL_IO_ULONGLONG_CHECK(ptr)  ((ptr) == nullulonglong)
#    define M_NULL_IO_INTMAX_CHECK(ptr)     ((ptr) == nullintmax)
#    define M_NULL_IO_UINTMAX_CHECK(ptr)    ((ptr) == nulluintmax)
#    define M_NULL_IO_FLOAT_CHECK(ptr)      ((ptr) == nullfloat)
#    define M_NULL_IO_DOUBLE_CHECK(ptr)     ((ptr) == nulldouble)
#    define M_NULL_IO_LDOUBLE_CHECK(ptr)    ((ptr) == nullldouble)
#else
#    define M_NULL_STREAM_CHECK(ptr)        (!(ptr))
#    define M_NULL_IO_CHAR_CHECK(ptr)       (!(ptr))
#    define M_NULL_IO_CONST_CHAR_CHECK(ptr) (!(ptr))
#    define M_NULL_IO_INT_CHECK(ptr)        (!(ptr))
#    define M_NULL_IO_LONG_CHECK(ptr)       (!(ptr))
#    define M_NULL_IO_LONGLONG_CHECK(ptr)   (!(ptr))
#    define M_NULL_IO_ULONG_CHECK(ptr)      (!(ptr))
#    define M_NULL_IO_ULONGLONG_CHECK(ptr)  (!(ptr))
#    define M_NULL_IO_INTMAX_CHECK(ptr)     (!(ptr))
#    define M_NULL_IO_UINTMAX_CHECK(ptr)    (!(ptr))
#    define M_NULL_IO_FLOAT_CHECK(ptr)      (!(ptr))
#    define M_NULL_IO_DOUBLE_CHECK(ptr)     (!(ptr))
#    define M_NULL_IO_LDOUBLE_CHECK(ptr)    (!(ptr))
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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_PARAM_RO(3)
    M_NULL_TERM_STRING(1)
    M_NULL_TERM_STRING(2)
    errno_t safe_fopen_impl(FILE* M_RESTRICT M_NONNULL* M_RESTRICT M_NULLABLE streamptr,
                            const char* M_RESTRICT M_NONNULL                  filename,
                            const char* M_RESTRICT M_NONNULL                  mode,
                            const char* M_NULLABLE                            file,
                            const char* M_NULLABLE                            function,
                            int                                               line,
                            const char* M_NULLABLE                            expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_OUTSTREAM_CHECK(streamptr), "streamptr is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(filename), "filename is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(mode), "mode is NULL")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_PARAM_RO(3)
    M_PARAM_RW(4)
    M_NULL_TERM_STRING(1)
    M_NULL_TERM_STRING(2)
    errno_t safe_freopen_impl(FILE* M_RESTRICT M_NONNULL* M_RESTRICT M_NULLABLE newstreamptr,
                              const char* M_RESTRICT M_NONNULL                  filename,
                              const char* M_RESTRICT M_NONNULL                  mode,
                              FILE* M_RESTRICT M_NONNULL                        stream,
                              const char* M_NULLABLE                            file,
                              const char* M_NULLABLE                            function,
                              int                                               line,
                              const char* M_NULLABLE                            expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_OUTSTREAM_CHECK(newstreamptr), "newstreamptr is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(filename), "filename is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(mode), "mode is NULL")
        M_DIAG_ERROR(M_NULL_STREAM_CHECK(stream), "stream is NULL")
        // clang-format on
        ;

#if defined(WANT_SAFE_TMPNAM)

#    if !defined(TMP_MAX_S)
#        define TMP_MAX_S TMP_MAX
#    endif
#    if !defined(L_tmpnam_s)
#        define L_tmpnam_s L_tmpnam
#    endif

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
    M_PARAM_RW_SIZE(1, 2)
    errno_t safe_tmpnam_impl(char* M_NONNULL        filename_s,
                             rsize_t                maxsize,
                             const char* M_NULLABLE file,
                             const char* M_NULLABLE function,
                             int                    line,
                             const char*            M_NULLABLE)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_CHAR_CHECK(filename_s), "filename_s is NULL")
        M_DIAG_ERROR(maxsize > RSIZE_MAX, "maxsize > RSIZE_MAX")
        M_DIAG_WARNING(maxsize < L_tmpnam_s, "maxsize < L_tmpname_s which may cause a failure in this function")
        // clang-format on
        ;
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
    M_PARAM_RW(1)
    errno_t safe_tmpfile_impl(FILE* M_RESTRICT M_NONNULL* M_RESTRICT M_NULLABLE streamptr,
                              const char* M_NULLABLE                            file,
                              const char* M_NULLABLE                            function,
                              int                                               line,
                              const char* M_NULLABLE                            expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_OUTSTREAM_CHECK(streamptr), "streamptr is NULL")
        // clang-format on
        ;

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
    M_PARAM_RW_SIZE(1, 2)
    char* M_NULLABLE safe_gets_impl(char* M_NONNULL        str,
                                    rsize_t                n,
                                    const char* M_NULLABLE file,
                                    const char* M_NULLABLE function,
                                    int                    line,
                                    const char* M_NULLABLE expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_CHAR_CHECK(str), "str is NULL")
        M_DIAG_ERROR(n == 0, "n is zero")
        M_DIAG_ERROR(n > RSIZE_MAX, "n > RSIZE_MAX")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtol_impl(long* M_NONNULL                         value,
                             const char* M_RESTRICT M_NONNULL        str,
                             char* M_NULLABLE* M_RESTRICT M_NULLABLE endp,
                             int                                     base,
                             const char* M_NULLABLE                  file,
                             const char* M_NULLABLE                  function,
                             int                                     line,
                             const char* M_NULLABLE                  expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_LONG_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        M_DIAG_ERROR(base > 36, "base > 36")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtoll_impl(long long* M_NONNULL                    value,
                              const char* M_RESTRICT M_NONNULL        str,
                              char* M_NULLABLE* M_RESTRICT M_NULLABLE endp,
                              int                                     base,
                              const char* M_NULLABLE                  file,
                              const char* M_NULLABLE                  function,
                              int                                     line,
                              const char* M_NULLABLE                  expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_LONGLONG_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        M_DIAG_ERROR(base > 36, "base > 36")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtoul_impl(unsigned long* M_NONNULL                value,
                              const char* M_RESTRICT M_NONNULL        str,
                              char* M_NULLABLE* M_RESTRICT M_NULLABLE endp,
                              int                                     base,
                              const char* M_NULLABLE                  file,
                              const char* M_NULLABLE                  function,
                              int                                     line,
                              const char* M_NULLABLE                  expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_ULONG_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        M_DIAG_ERROR(base > 36, "base > 36")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtoull_impl(unsigned long long* M_NONNULL           value,
                               const char* M_RESTRICT M_NONNULL        str,
                               char* M_NULLABLE* M_RESTRICT M_NULLABLE endp,
                               int                                     base,
                               const char* M_NULLABLE                  file,
                               const char* M_NULLABLE                  function,
                               int                                     line,
                               const char* M_NULLABLE                  expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_ULONGLONG_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        M_DIAG_ERROR(base > 36, "base > 36")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtoimax_impl(intmax_t* M_NONNULL                     value,
                                const char* M_RESTRICT M_NONNULL        str,
                                char* M_NULLABLE* M_RESTRICT M_NULLABLE endp,
                                int                                     base,
                                const char* M_NULLABLE                  file,
                                const char* M_NULLABLE                  function,
                                int                                     line,
                                const char* M_NULLABLE                  expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_INTMAX_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        M_DIAG_ERROR(base > 36, "base > 36")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtoumax_impl(uintmax_t* M_NONNULL                    value,
                                const char* M_RESTRICT M_NONNULL        str,
                                char* M_NULLABLE* M_RESTRICT M_NULLABLE endp,
                                int                                     base,
                                const char* M_NULLABLE                  file,
                                const char* M_NULLABLE                  function,
                                int                                     line,
                                const char* M_NULLABLE                  expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_UINTMAX_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        M_DIAG_ERROR(base > 36, "base > 36")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtof_impl(float* M_NONNULL                        value,
                             const char* M_RESTRICT M_NONNULL        str,
                             char* M_NULLABLE* M_RESTRICT M_NULLABLE endp,
                             const char* M_NULLABLE                  file,
                             const char* M_NULLABLE                  function,
                             int                                     line,
                             const char* M_NULLABLE                  expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_FLOAT_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtod_impl(double* M_NONNULL                       value,
                             const char* M_RESTRICT M_NONNULL        str,
                             char* M_NULLABLE* M_RESTRICT M_NULLABLE endp,
                             const char* M_NULLABLE                  file,
                             const char* M_NULLABLE                  function,
                             int                                     line,
                             const char* M_NULLABLE                  expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_DOUBLE_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strtold_impl(long double* M_NONNULL                  value,
                              const char* M_RESTRICT M_NONNULL        str,
                              char* M_NULLABLE* M_RESTRICT M_NULLABLE endp,
                              const char* M_NULLABLE                  file,
                              const char* M_NULLABLE                  function,
                              int                                     line,
                              const char* M_NULLABLE                  expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_LDOUBLE_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_atoi_impl(int* M_NONNULL                   value,
                           const char* M_RESTRICT M_NONNULL str,
                           const char* M_NULLABLE           file,
                           const char* M_NULLABLE           function,
                           int                              line,
                           const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_INT_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_atol_impl(long* M_NONNULL                  value,
                           const char* M_RESTRICT M_NONNULL str,
                           const char* M_NULLABLE           file,
                           const char* M_NULLABLE           function,
                           int                              line,
                           const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_LONG_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_atoll_impl(long long* M_NONNULL             value,
                            const char* M_RESTRICT M_NONNULL str,
                            const char* M_NULLABLE           file,
                            const char* M_NULLABLE           function,
                            int                              line,
                            const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_LONGLONG_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        // clang-format on
        ;

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
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_atof_impl(double* M_NONNULL                value,
                           const char* M_RESTRICT M_NONNULL str,
                           const char* M_NULLABLE           file,
                           const char* M_NULLABLE           function,
                           int                              line,
                           const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_DOUBLE_CHECK(value), "value is NULL")
        M_DIAG_ERROR(M_NULL_IO_CONST_CHAR_CHECK(str), "str is NULL")
        // clang-format on
        ;

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
    M_NONNULL_IF_NONZERO_PARAM(5, 6)
    M_NULL_TERM_STRING(7)
    M_PARAM_RW(5)
    M_PARAM_RO(7)
    FUNC_ATTR_PRINTF(7, 8)
    int impl_snprintf_err_handle(const char* M_NULLABLE file,
                                 const char* M_NULLABLE function,
                                 int                    line,
                                 const char* M_NULLABLE expression,
                                 char* M_NULLABLE       buf,
                                 size_t                 bufsize,
                                 const char* M_NONNULL  format,
                                 ...)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_CHAR_CHECK(buf) && bufsize != 0, "buf is NULL and bufsize != 0")
        M_DIAG_ERROR(!M_NULL_IO_CHAR_CHECK(buf) && (bufsize == 0 || bufsize > RSIZE_MAX), "bufsize is out of range (1-RSIZE_MAX allowed)")
        // clang-format on
        ;

    //! \fn FUNC_ATTR_PRINTF(3, 4) int impl_vsnprintf_err_handle(const char* file,
    //!                                                         const char* function,
    //!                                                         int         line,
    //!                                                         const char* expression,
    //!                                                         char*       buf,
    //!                                                         size_t      bufsize,
    //!                                                         const char* format,
    //!                                                         va_list     args)
    //! \brief Not exactly the same as vsnprintf_s, just checking that the return code is as expected when using this
    //! function to detect errors or truncation.
    //! \details This function does NOT do bounds checking.
    //! \see https://wiki.sei.cmu.edu/confluence/display/c/ERR33-C.+Detect+and+handle+standard+library+errors
    //! \todo Implement a safe_vsnprintf that works as C11 annex k specifies.
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \param[out] buf TThe buffer to write the formatted string to.
    //! \param[in] bufsize The size of the buffer.
    //! \param[in] format The format string.
    //! \param[in] args The arguments for the format string.
    //! \return The number of characters that would have been written if the buffer had been sufficiently large, not
    //! including the null terminator. On error, this will always add a null terminator at the end of the buffer.
    //! \note Invokes the constraint handler on error.
    M_NONNULL_IF_NONZERO_PARAM(5, 6)
    M_NULL_TERM_STRING(7)
    M_PARAM_RW(5)
    M_PARAM_RO(7)
    FUNC_ATTR_PRINTF(7, 0)
    int impl_vsnprintf_err_handle(const char* M_NULLABLE file,
                                  const char* M_NULLABLE function,
                                  int                    line,
                                  const char* M_NULLABLE expression,
                                  char* M_NULLABLE       buf,
                                  size_t                 bufsize,
                                  const char* M_NONNULL  format,
                                  va_list                args)
        // clang-format off
        M_DIAG_ERROR(M_NULL_IO_CHAR_CHECK(buf) && bufsize != 0, "buf is NULL and bufsize != 0")
        M_DIAG_ERROR(!M_NULL_IO_CHAR_CHECK(buf) && (bufsize == 0 || bufsize > RSIZE_MAX), "bufsize is out of range (1-RSIZE_MAX allowed)")
        // clang-format on
        ;

#if defined(__cplusplus)
}
#endif
