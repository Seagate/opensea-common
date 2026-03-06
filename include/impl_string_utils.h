// SPDX-License-Identifier: MPL-2.0

//! \file impl_string_utils.h
//! \brief This is an internal header to handle the internal definitions for string_utils.h
//!
//!        Do NOT include this and use these functions directly. Include string_utils.h and use the calls from there
//!        The functions with "impl" in the name expect __FILE__, __func__, __LINE__, and an expression when called
//!        which get wrapped to make this easier for you to use.
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "memory_safety.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \fn size_t safe_strnlen(const char* string, size_t n)
    //! \brief Returns length of string or \a n if null terminator not found
    //! \param[in] string pointer to string to find the length of.
    //! \param[in] n maximum number of characters to scan for null terminator
    //! \return 0 if string is null. length of string if null terminator found. \a n if null terminator not found
    //! after scanning \a n characters
    // M_PARAM_RO_SIZE(1, 2) //Do not use this right now. Need to revisit how we want to do this since
    // doing this causes some weird behavior with builtin obj size and RSIZE_MAX as a backup maximum length-TJE
    size_t safe_strnlen_impl(const char* M_NULLABLE string, size_t n);

// Methods to detect errors at compile time for clang with diagnose_if attributes.
// This is not perfect, but having constexpr is far more helpful and truthful for these comparisons
// than the workaround. diagnose_if does not allow casts of any kinds so trying to compare directly
// to a NULL or M_NULLPTR is not allowed. !ptr is allowed, but may not work in some systems/libraries
// so using the constexpr is preferred since it is a more direct match for the error condition in all
// scenarios. - TJE
#if defined(HAVE_CONSTEXPR)
    M_ATTR_UNUSED constexpr char* nullstr = M_NULLPTR;
#    define M_NULL_STR_CHECK(ptr) ((ptr) == nullstr)
    M_ATTR_UNUSED constexpr char** nulldblstr = M_NULLPTR;
#    define M_NULL_DBL_STR_CHECK(ptr) ((ptr) == nulldblstr)

    M_ATTR_UNUSED constexpr const char* nullconststr = M_NULLPTR;
#    define M_NULL_CONST_STR_CHECK(ptr) ((ptr) == nullconststr)
    M_ATTR_UNUSED constexpr const char** nullconstdblstr = M_NULLPTR;
#    define M_NULL_CONST_DBL_STR_CHECK(ptr) ((ptr) == nullconstdblstr)

    M_ATTR_UNUSED constexpr rsize_t* nullrsizet = M_NULLPTR;
#    define M_NULL_RSIZET_CHECK(ptr) ((ptr) == nullrsizet)
#else
#    define M_NULL_STR_CHECK(ptr)           (!(ptr))
#    define M_NULL_DBL_STR_CHECK(ptr)       M_NULL_STR_CHECK(ptr)
#    define M_NULL_CONST_STR_CHECK(ptr)     M_NULL_STR_CHECK(ptr)
#    define M_NULL_CONST_DBL_STR_CHECK(ptr) M_NULL_STR_CHECK(ptr)
#    define M_NULL_RSIZET_CHECK(ptr)        (!(ptr))
#endif

    //! \def M_STRING_REGIONS_OVERLAP_COMPILE_TIME(ptr1, size1, ptr2, size2)
    //! \brief Compile-time constant check for potentially overlapping string regions.
    //!
    //! This macro can be used with M_DIAG_ERROR to detect overlapping string memory regions using
    //! pointer arithmetic. The check determines if region 1 [ptr1, ptr1+size1) overlaps with
    //! region 2 [ptr2, ptr2+size2) by verifying that:
    //!   - ptr1 starts before region 2 ends (ptr1 < ptr2 + size2), AND
    //!   - ptr2 starts before region 1 ends (ptr2 < ptr1 + size1)
    //!
    //! This uses natural pointer subtraction (no explicit casts), which results in ptrdiff_t
    //! and is suitable for use in constant expressions with diagnose_if.
    //!
    //! \param[in] ptr1 First string/memory region pointer
    //! \param[in] size1 Size of first region
    //! \param[in] ptr2 Second string/memory region pointer
    //! \param[in] size2 Size of second region
    //! \return True (non-zero) if regions overlap, false (zero) otherwise
    //!
    //! \note This macro uses pointer arithmetic and will work for compile-time constant pointers.
    //!       For runtime cases with dynamic pointers, use the memory_regions_overlap() function
    //!       from memory_safety.h
    //!
    //! \example
    //! M_DIAG_ERROR(M_STRING_REGIONS_OVERLAP_COMPILE_TIME(dest, destsz, src, srclen), \
    //!              "source and destination regions overlap")
#define M_STRING_REGIONS_OVERLAP_COMPILE_TIME(ptr1, size1, ptr2, size2) \
    (((ptr1) < (ptr2) + (size2)) && ((ptr2) < (ptr1) + (size1)))

    //! \fn errno_t safe_strcpy_impl(char* M_RESTRICT       dest,
    //!                              rsize_t                destsz,
    //!                              const char* M_RESTRICT src,
    //!                              const char*            file,
    //!                              const char*            function,
    //!                              int                    line,
    //!                              const char*            expression)
    //! \brief internal implementation of safe_strcpy
    //!
    //! Works like C11 annex K's strcpy_s
    //! This copys a null terminated byte string pointed to by \a src to the
    //! character array whose first element is pointed to by \a dest.
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a src is a null pointer
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a destsz is zero or greater than \a RSIZE_MAX
    //!
    //! - \a destsz is less than or equal to safe_strnlen(src, destsz); truncation would occur
    //!
    //! - overlap would occur between the source and destination strings.
    M_PARAM_RW_SIZE(1, 2)
    M_PARAM_RO(3)
    M_NULL_TERM_STRING(3)
    errno_t safe_strcpy_impl(char* M_RESTRICT M_NONNULL       dest,
                             rsize_t                          destsz,
                             const char* M_RESTRICT M_NONNULL src,
                             const char* M_NULLABLE           file,
                             const char* M_NULLABLE           function,
                             int                              line,
                             const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_STR_CHECK(dest), "dest is NULL")
        M_DIAG_ERROR(destsz == 0, "destsz is zero")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz is > RSIZE_MAX")
        M_DIAG_ERROR(M_NULL_CONST_STR_CHECK(src), "src is NULL")
        M_DIAG_ERROR(M_STRING_REGIONS_OVERLAP_COMPILE_TIME(dest, destsz, src, destsz), "source and destination regions overlap. Use safe_strmove instead.")
        // clang-format on
        ;

    //! \fn errno_t safe_strmove_impl(char* M_RESTRICT      dest,
    //!                              rsize_t                destsz,
    //!                              const char* M_RESTRICT src,
    //!                              const char*            file,
    //!                              const char*            function,
    //!                              int                    line,
    //!                              const char*            expression)
    //! \brief internal implementation of safe_strmove
    //!
    //! This copys a null terminated byte string pointed to by \a src to the
    //! character array whose first element is pointed to by \a dest.
    //! This version performs the bounds checking described in C11 annex K.
    //! Unlike safe_strcpy this allows for overlapping source and destination memory.
    //! This version always appends a null terminator after completing the move.
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a src is a null pointer
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a destsz is zero or greater than \a RSIZE_MAX
    //!
    //! - \a destsz is less than or equal to safe_strnlen(src, destsz); truncation would occur
    M_PARAM_RW_SIZE(1, 2)
    M_PARAM_RO(3)
    M_NULL_TERM_STRING(3)
    errno_t safe_strmove_impl(char* M_NONNULL        dest,
                              rsize_t                destsz,
                              const char* M_NONNULL  src,
                              const char* M_NULLABLE file,
                              const char* M_NULLABLE function,
                              int                    line,
                              const char* M_NULLABLE expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_STR_CHECK(dest), "dest is NULL")
        M_DIAG_ERROR(destsz == 0, "destsz is zero")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz is > RSIZE_MAX")
        M_DIAG_ERROR(M_NULL_CONST_STR_CHECK(src), "src is NULL")
        // clang-format on
        ;

    //! \fn errno_t safe_strncpy_impl(char* M_RESTRICT       dest,
    //!                               rsize_t                destsz,
    //!                               const char* M_RESTRICT src,
    //!                               rsize_t                count,
    //!                               const char*            file,
    //!                               const char*            function,
    //!                               int                    line,
    //!                               const char*            expression)
    //! \brief internal implementation of safe_strncpy
    //!
    //! Works like C11 annex K's strncpy_s
    //! This copys certain number of characters from a null terminated byte string pointed to by \a src to the
    //! character array whose first element is pointed to by \a dest.
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \param[in] count maximum number of characters to copy
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a src is a null pointer
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a destsz is zero or greater than \a RSIZE_MAX
    //!
    //! - \a destsz is less than or equal to safe_strnlen(src, destsz); truncation would occur
    //!
    //! - \a count is greater than \a RSIZE_MAX
    //!
    //! - \a count is greater than or equal to \a destsz, but \a destsz is less than or equal to
    //!   safe_strnlen(src, count); truncation would occur.
    //!
    //! - overlap would occur between the source and destination strings.
    M_PARAM_RW_SIZE(1, 2)
    M_PARAM_RO_SIZE(3, 4)
    errno_t safe_strncpy_impl(char* M_RESTRICT M_NONNULL       dest,
                              rsize_t                          destsz,
                              const char* M_RESTRICT M_NONNULL src,
                              rsize_t                          count,
                              const char* M_NULLABLE           file,
                              const char* M_NULLABLE           function,
                              int                              line,
                              const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_STR_CHECK(dest), "dest is NULL")
        M_DIAG_ERROR(destsz == 0, "destsz is zero")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz is > RSIZE_MAX")
        M_DIAG_ERROR(M_NULL_CONST_STR_CHECK(src), "src is NULL")
        M_DIAG_ERROR(count > RSIZE_MAX, "count is > RSIZE_MAX")
        M_DIAG_ERROR(M_STRING_REGIONS_OVERLAP_COMPILE_TIME(dest, destsz, src, count), "source and destination regions overlap. Use safe_strnmove instead.")
        // clang-format on
        ;

    //! \fn errno_t safe_strnmove_impl(char* M_RESTRICT       dest,
    //!                                rsize_t                destsz,
    //!                                const char* M_RESTRICT src,
    //!                                rsize_t                count,
    //!                                const char*            file,
    //!                                const char*            function,
    //!                                int                    line,
    //!                                const char*            expression)
    //! \brief internal implementation of safe_strnmove
    //!
    //! This copys certain number of characters from a null terminated byte string pointed to by \a src to the
    //! character array whose first element is pointed to by \a dest.
    //! This version performs the bounds checking described in C11 annex K.
    //! This differs from safe_strncpy as it allows for overlapping ranges and always null terminates
    //! after moving the data.
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \param[in] count maximum number of characters to copy
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a src is a null pointer
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a destsz is zero or greater than \a RSIZE_MAX
    //!
    //! - \a destsz is less than or equal to safe_strnlen(src, destsz); truncation would occur
    //!
    //! - \a count is greater than \a RSIZE_MAX
    //!
    //! - \a count is greater than or equal to \a destsz, but \a destsz is less than or equal to
    //!   safe_strnlen(src, count); truncation would occur.
    M_PARAM_RW_SIZE(1, 2)
    M_PARAM_RO_SIZE(3, 4)
    errno_t safe_strnmove_impl(char* M_NONNULL        dest,
                               rsize_t                destsz,
                               const char* M_NONNULL  src,
                               rsize_t                count,
                               const char* M_NULLABLE file,
                               const char* M_NULLABLE function,
                               int                    line,
                               const char* M_NULLABLE expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_STR_CHECK(dest), "dest is NULL")
        M_DIAG_ERROR(destsz == 0, "destsz is zero")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz is > RSIZE_MAX")
        M_DIAG_ERROR(M_NULL_CONST_STR_CHECK(src), "src is NULL")
        M_DIAG_ERROR(count > RSIZE_MAX, "count is > RSIZE_MAX")
        // clang-format on
        ;

    //! \fn errno_t safe_strcat_impl(char* M_RESTRICT       dest,
    //!                              rsize_t                destsz,
    //!                              const char* M_RESTRICT src,
    //!                              const char*            file,
    //!                              const char*            function,
    //!                              int                    line,
    //!                              const char*            expression)
    //! \brief internal implementation of safe_strcat
    //!
    //! Works like C11 annex K's strcat_s
    //! Appends a copy of the null-terminated byte string pointed to by \a src to the end of the null-terminated
    //! byte string pointed to by \a dest. The character \a src[0] replaces the null terminator at the end of \a dest.
    //! The resulting byte string is null-terminated.
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a src is a null pointer
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a destsz is zero or greater than \a RSIZE_MAX
    //!
    //! - there is no null terminator in the first \a destsz bytes of \a dest
    //!
    //! - truncation would occur due to not enough space in \a dest to concatenate \a src
    //!
    //! - overlap would occur between \a src and \a dest strings
    M_PARAM_RW_SIZE(1, 2)
    M_PARAM_RO(3)
    M_NULL_TERM_STRING(1)
    M_NULL_TERM_STRING(3)
    errno_t safe_strcat_impl(char* M_RESTRICT M_NONNULL       dest,
                             rsize_t                          destsz,
                             const char* M_RESTRICT M_NONNULL src,
                             const char* M_NULLABLE           file,
                             const char* M_NULLABLE           function,
                             int                              line,
                             const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_STR_CHECK(dest), "dest is NULL")
        M_DIAG_ERROR(destsz == 0, "destsz is zero")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz is > RSIZE_MAX")
        M_DIAG_ERROR(M_NULL_CONST_STR_CHECK(src), "src is NULL")
        M_DIAG_ERROR(M_STRING_REGIONS_OVERLAP_COMPILE_TIME(dest, destsz, src, destsz), "source and destination regions overlap")
        // TODO Add diagnostic for not enough space in dest to concatenate source
        // clang-format on
        ;

    //! \fn errno_t safe_strncat_impl(char* M_RESTRICT       dest,
    //!                               rsize_t                destsz,
    //!                               const char* M_RESTRICT src,
    //!                               rsize_t                count,
    //!                               const char*            file,
    //!                               const char*            function,
    //!                               int                    line,
    //!                               const char*            expression)
    //! \brief internal implementation of safe_strncat
    //!
    //! Works like C11 annex K's strncat_s
    //! Appends at most count characters from the character array pointed to by \a src, stopping if the null character
    //! is found, to the end of the null-terminated byte string pointed to by \a dest. The character \a src[0]
    //! replaces the null terminator at the end of \a dest. The terminating null character is always appended in the
    //! end (so the maximum number of bytes the function may write is \a count + 1)
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \param[in] count maximum number of characters to copy
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a src is a null pointer
    //!
    //! - \a dest is a null pointer
    //!
    //! - \a destsz is zero or greater than \a RSIZE_MAX
    //!
    //! - \a count is zero or greater than \a RSIZE_MAX
    //!
    //! - there is no null terminator in the first \a destsz bytes of \a dest
    //!
    //! - truncation would occur due to not enough space in \a dest to concatenate \a src or \a count bytes of \a src
    //!
    //! - overlap would occur between \a src and \a dest strings
    M_PARAM_RW_SIZE(1, 2)
    M_PARAM_RO_SIZE(3, 4)
    M_NULL_TERM_STRING(1)
    errno_t safe_strncat_impl(char* M_RESTRICT M_NONNULL       dest,
                              rsize_t                          destsz,
                              const char* M_RESTRICT M_NONNULL src,
                              rsize_t                          count,
                              const char* M_NULLABLE           file,
                              const char* M_NULLABLE           function,
                              int                              line,
                              const char* M_NULLABLE           expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_STR_CHECK(dest), "dest is NULL")
        M_DIAG_ERROR(destsz == 0, "destsz is zero")
        M_DIAG_ERROR(destsz > RSIZE_MAX, "destsz is > RSIZE_MAX")
        M_DIAG_ERROR(count == 0, "destsz is zero")
        M_DIAG_ERROR(count > RSIZE_MAX, "destsz is > RSIZE_MAX")
        M_DIAG_ERROR(M_NULL_CONST_STR_CHECK(src), "src is NULL")
        M_DIAG_ERROR(M_STRING_REGIONS_OVERLAP_COMPILE_TIME(dest, destsz, src, count), "source and destination regions overlap")
        // clang-format on
        ;

    //! \fn char* safe_strtok_impl(char* M_RESTRICT       str,
    //!                            rsize_t* M_RESTRICT    strmax,
    //!                            const char* M_RESTRICT delim,
    //!                            char** M_RESTRICT      saveptr,
    //!                            const char*            file,
    //!                            const char*            function,
    //!                            int                    line,
    //!                            const char*            expression)
    //! \brief internal implementation of safe_strtok
    //!
    //! Works like C11 annex K's strtok_s
    //! Finds the next token in a null-terminated byte string pointed to by \a str.
    //! The separator characters are identified by null-terminated byte string pointed to by \a delim.
    //! This function is designed to be called multiple times to obtain successive tokens from the same string.
    //! on every step, writes the number of characters left to see in str into \a *strmax and writes the tokenizer's
    //! internal state to \a *saveptr. Repeat calls (with null \a str) must pass \a strmax and \a saveptr
    //! with the values stored by the previous call.
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] str pointer to the null-terminated byte string to tokenize
    //! \param[in] strmax pointer to an object which initially holds the size of \a str: safe_strtok stores
    //! the number of characters that remain to be examined
    //! \param[in] delim pointer to the null-terminated byte string identifying delimiters
    //! \param[in] saveptr 	pointer to an object of type char*, which is used by safe_strtok to store its
    //! internal state
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return pointer to token on success, null pointer on error
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a strmax is a null pointer
    //!
    //! - \a delim is a null pointer
    //!
    //! - \a saveptr is a null pointer
    //!
    //! - on a non-initial call (null \a str), \a *saveptr is a null pointer
    //!
    //! - search for the end of a token reaches the end of the source string (as measured by the initial value of
    //! *strmax) without encountering the null terminator
    //!
    //! - \a strmax > RSIZE_MAX
    M_PARAM_RW(1)
    M_PARAM_RW(2)
    M_PARAM_RO(3)
    M_PARAM_RW(4)
    M_NULL_TERM_STRING(1)
    M_NULL_TERM_STRING(3)
    char* M_NULLABLE safe_strtok_impl(char* M_RESTRICT M_NULLABLE            str,
                                      rsize_t* M_RESTRICT M_NONNULL          strmax,
                                      const char* M_RESTRICT M_NONNULL       delim,
                                      char* M_NONNULL* M_RESTRICT M_NULLABLE saveptr,
                                      const char* M_NULLABLE                 file,
                                      const char* M_NULLABLE                 function,
                                      int                                    line,
                                      const char* M_NULLABLE                 expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_RSIZET_CHECK(strmax), "strmax is NULL")
        M_DIAG_ERROR(*strmax > RSIZE_MAX, "strmax > RSIZE_MAX")
        M_DIAG_ERROR(M_NULL_CONST_STR_CHECK(delim), "delim is NULL")
        M_DIAG_ERROR(M_NULL_DBL_STR_CHECK(saveptr), "saveptr is NULL")
        // clang-format on
        ;

    //! \fn errno_t safe_strdup_impl(char**      dup,
    //!                              const char* src,
    //!                              const char* file,
    //!                              const char* function,
    //!                              int         line,
    //!                              const char* expression)
    //! \brief internal implementation of safe_strdup
    //!
    //! Works like strdup, but with security enhancements to prevent crashes.
    //! Checks that a \a src is non-null before duplicating, \a src is not length zero and not greater
    //! than \a RSIZE_MAX.
    //! The caller must free \a dup with a call to \a free() or \a safe_free().
    //! \param[out] dup pointer to use for duplicating \a src to. Must be non-NULL
    //! \param[in] src pointer to an null-terminated string to duplicate
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a dup is a null pointer
    //!
    //! - \a src is a null pointer
    //!
    //! - \a src is length 0 or greater than RSIZE_MAX
    M_PARAM_RW(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(2)
    errno_t safe_strdup_impl(char* M_NONNULL* M_NULLABLE dup,
                             const char* M_NONNULL       src,
                             const char* M_NULLABLE      file,
                             const char* M_NULLABLE      function,
                             int                         line,
                             const char* M_NULLABLE      expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_DBL_STR_CHECK(dup), "output parameter dup cannot be NULL")
        M_DIAG_ERROR(M_NULL_CONST_STR_CHECK(src), "src is NULL")
        // clang-format on
        ;

    //! \fn errno_t safe_strndup_impl(char**      dup,
    //!                               const char* src,
    //!                               rsize_t     size,
    //!                               const char* file,
    //!                               const char* function,
    //!                               int         line,
    //!                               const char* expression)
    //! \brief internal implementation of safe_strndup
    //!
    //! Works like strndup, but with security enhancements to prevent crashes.
    //! Checks that a \a src is non-null before duplicating, \a src is not length zero and not greater
    //! than \a RSIZE_MAX, \a size is not zero and not greater than \a RSIZE_MAX.
    //! The caller must free \a dup with a call to \a free() or \a safe_free().
    //! \param[out] dup pointer to use for duplicating \a src to. Must be non-NULL
    //! \param[in] src pointer to an null-terminated string to duplicate
    //! \param[in] size number of bytes to duplicate from \a src
    //! \param[in] file The source file name where this function is called.
    //! \param[in] function The function name where this function is called.
    //! \param[in] line The line number where this function is called.
    //! \param[in] expression The expression being evaluated.
    //! \return Zero on success, or an error code on failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a dup is a null pointer
    //!
    //! - \a src is a null pointer
    //!
    //! - \a src is length 0 or greater than RSIZE_MAX
    //!
    //! - \a size is length 0 or greater than RSIZE_MAX
    M_PARAM_RW(1)
    M_PARAM_RO_SIZE(2, 3)
    errno_t safe_strndup_impl(char* M_NONNULL* M_NULLABLE dup,
                              const char* M_NONNULL       src,
                              rsize_t                     size,
                              const char* M_NULLABLE      file,
                              const char* M_NULLABLE      function,
                              int                         line,
                              const char* M_NULLABLE      expression)
        // clang-format off
        M_DIAG_ERROR(M_NULL_DBL_STR_CHECK(dup), "output parameter dup cannot be NULL")
        M_DIAG_ERROR(M_NULL_CONST_STR_CHECK(src), "src is NULL")
        M_DIAG_ERROR(size > RSIZE_MAX, "size > RSIZE_MAX")
        // clang-format on
        ;

#if defined(__cplusplus)
}
#endif
