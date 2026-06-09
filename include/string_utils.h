// SPDX-License-Identifier: MPL-2.0

//! \file string_utils.h
//! \brief Implements various functions to work with C style strings.
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
#include "env_detect.h"
#include "impl_string_utils.h"
#include "predef_env_detect.h"
#include "type_conversion.h"

#include <stdio.h>

#if defined(__cplusplus)
extern "C"
{
#endif

//! \def STRCPY_IS_STRCPY_NOT_STRMOVE
//! \brief Controls the default behavior of safe_strcpy, safe_strcat, safe_strncat, and safe_strncpy functions.
//!
//! When STRCPY_IS_STRCPY_NOT_STRMOVE is defined:
//!   - safe_strcpy, safe_strcat, safe_strncat, safe_strncpy behave strictly like the C11 annex K functions
//!   - They do not allow overlapping source and destination buffers
//!   - Overlapping ranges are detected as an error at runtime
//!
//! When STRCPY_IS_STRCPY_NOT_STRMOVE is NOT defined (default):
//!   - safe_strcpy routes to safe_strmove (memmove-like behavior)
//!   - safe_strcat routes to safe_strcatmove (allows overlaps)
//!   - safe_strncat routes to safe_strncatmove (allows overlaps)
//!   - safe_strncpy routes to safe_strnmove (allows overlaps)
//!   - Overlapping source and destination buffers are allowed and handled safely
//!
//! This flag follows the same pattern as MEMCPY_IS_MEMCPY_NOT_MEMMOVE for memory functions.
//! See also: safe_strcpy_no_overlap, safe_strcat_no_overlap, safe_strncat_no_overlap, safe_strncpy_no_overlap for
//! explicit variants.
// #define STRCPY_IS_STRCPY_NOT_STRMOVE

//! \def ALLOW_NO_OVERLAP_SUGGESTIONS
//! \brief Controls whether compile-time diagnostics suggest performance optimization using _no_overlap variants.
//!
//! When ALLOW_NO_OVERLAP_SUGGESTIONS is defined:
//!   - M_DIAG_WARNING messages are emitted for memory and string functions when compile-time analysis
//!     proves that no overlap exists between source and destination buffers
//!   - These warnings suggest using the _no_overlap variant for better performance
//!   - Example: "No overlap detected; consider safe_strcpy_no_overlap for better performance"
//!
//! When ALLOW_NO_OVERLAP_SUGGESTIONS is NOT defined (default):
//!   - No performance optimization suggestions are emitted
//!   - Only errors (M_DIAG_ERROR) for actual problems are reported
//!
//! This flag is useful during performance profiling and optimization phases to identify
//! opportunities for optimization where non-overlapping variants could be safely used.
// #define ALLOW_NO_OVERLAP_SUGGESTIONS

// Including strings.h to have string case compare functionality and working
// around Windows.
// TODO: improve this check as needed for other systems.
// TODO: make definitions for other functions in strings.h
#if defined(__unix__) || defined(POSIX_2001)
#    include <strings.h>
#elif defined(_WIN32)
#    if !defined(strcasecmp)
//! \def strcasecmp(s1, s2)
//! \brief compares two strings; case insensitive
//! \param[in] s1 pointer to first null terminated string to compare
//! \param[in] s2 pointer to second null terminated string to compare
//! \return 0 if equal. Negative if \a s1 is less than \a s2 or positive if
//! \a s1 is greater than \a s2
#        define strcasecmp(s1, s2) _stricmp(s1, s2)
#    endif // strcasecmp
#    if !defined(strncasecmp)
//! \def strncasecmp(s1, s2, n)
//! \brief compares two strings up to NULL termination or length \a n; case insensitive
//! \param[in] s1 pointer to first null terminated string to compare
//! \param[in] s2 pointer to second null terminated string to compare
//! \param[in] n maximum number of characters to compare between strings
//! \return 0 if equal. Negative if \a s1 is less than \a s2 or positive if
//! \a s1 is greater than \a s2
#        define strncasecmp(s1, s2, n) _strnicmp(s1, s2, n)
#    endif // strncasecmp
#else
#    error "Need string case compare definition."
#endif // __unix__, POSIX, WIN32

    //! \fn int is_ASCII(int c)
    //! \brief Checks if the provided character is part of the standard ASCII character set.
    //! \param[in] c Character to check if it is an ASCII character.
    //! \return 0 if not an ASCII character, 1 if it is an ASCII character.
    int is_ASCII(int c)
        // clang-format off
    M_DIAG_WARN((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_isascii(int c)
    //! \brief Safe version of isascii().
    //! \details Checks if the provided character is an ASCII character, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not an ASCII character, non-zero if it is an ASCII character.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isascii(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_isalnum(int c)
    //! \brief Safe version of isalnum().
    //! \details Checks if the provided character is alphanumeric, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not alphanumeric, non-zero if it is alphanumeric.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isalnum(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_isalpha(int c)
    //! \brief Safe version of isalpha().
    //! \details Checks if the provided character is alphabetic, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not alphabetic, non-zero if it is alphabetic.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isalpha(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_islower(int c)
    //! \brief Safe version of islower().
    //! \details Checks if the provided character is a lowercase letter, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not lowercase, non-zero if it is lowercase.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_islower(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_isupper(int c)
    //! \brief Safe version of isupper().
    //! \details Checks if the provided character is an uppercase letter, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not uppercase, non-zero if it is uppercase.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isupper(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_isdigit(int c)
    //! \brief Safe version of isdigit().
    //! \details Checks if the provided character is a digit, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not a digit, non-zero if it is a digit.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isdigit(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_isxdigit(int c)
    //! \brief Safe version of isxdigit().
    //! \details Checks if the provided character is a hexadecimal digit, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not a hexadecimal digit, non-zero if it is a hexadecimal digit.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isxdigit(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_iscntrl(int c)
    //! \brief Safe version of iscntrl().
    //! \details Checks if the provided character is a control character, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not a control character, non-zero if it is a control character.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_iscntrl(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_isgraph(int c)
    //! \brief Safe version of isgraph().
    //! \details Checks if the provided character has a graphical representation, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not graphical, non-zero if it is graphical.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isgraph(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_isspace(int c)
    //! \brief Safe version of isspace().
    //! \details Checks if the provided character is a whitespace character, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not whitespace, non-zero if it is whitespace.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isspace(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_isblank(int c)
    //! \brief Safe version of isblank().
    //! \details Checks if the provided character is a blank character, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not blank, non-zero if it is blank.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isblank(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_isprint(int c)
    //! \brief Safe version of isprint().
    //! \details Checks if the provided character is printable, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not printable, non-zero if it is printable.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isprint(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_ispunct(int c)
    //! \brief Safe version of ispunct().
    //! \details Checks if the provided character is a punctuation character, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not punctuation, non-zero if it is punctuation.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_ispunct(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_tolower(int c)
    //! \brief Safe version of tolower().
    //! \details Converts the provided character to lowercase, guarding against undefined behavior.
    //! \param[in] c Character to convert.
    //! \return The lowercase equivalent of \a c, or \a c if it is not an uppercase letter.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_tolower(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

    //! \fn int safe_toupper(int c)
    //! \brief Safe version of toupper().
    //! \details Converts the provided character to uppercase, guarding against undefined behavior.
    //! \param[in] c Character to convert.
    //! \return The uppercase equivalent of \a c, or \a c if it is not a lowercase letter.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_toupper(int c)
        // clang-format off
    M_DIAG_ERROR((c < 0 || c > UCHAR_MAX) && c != EOF, "Character value is out of range (0-UCHAR_MAX)")
        // clang-format on
        ;

#if defined(DEV_ENVIRONMENT)
    //! \fn size_t safe_strnlen(const char* string, size_t n)
    //! \brief Returns length of string or \a n if null terminator not found
    //! \param[in] string pointer to string to find the length of.
    //! \param[in] n maximum number of characters to scan for null terminator
    //! \return 0 if string is null. length of string if null terminator found. \a n if null terminator not found
    //! after scanning \a n characters
    // M_PARAM_RO_SIZE(1, 2) //Do not use this right now. Need to revisit how we want to do this since
    // doing this causes some weird behavior with builtin obj size and RSIZE_MAX as a backup maximum length-TJE
    M_NODISCARD M_INLINE size_t safe_strnlen(const char* M_NULLABLE string, size_t n)
    {
        return safe_strnlen_impl(string, n);
    }
#else
//! \def safe_strnlen(string, n)
//! \brief Returns length of string or \a n if null terminator not found
//! \param[in] string pointer to string to find the length of.
//! \param[in] n maximum number of characters to scan for null terminator
//! \return 0 if string is null. length of string if null terminator found. \a n if null terminator not found
//! after scanning \a n characters
// M_PARAM_RO_SIZE(1, 2) //Do not use this right now. Need to revisit how we want to do this since
// doing this causes some weird behavior with builtin obj size and RSIZE_MAX as a backup maximum length-TJE
#    define safe_strnlen(string, n) safe_strnlen_impl(string, n)
#endif

// if str is a null pointer, returns 0. Internally calls safe_strnlen with size
// set to RSIZE_MAX If __builtin_object_size can determine the amount of memory
// allocated for the string, the limit is set to this limit, otherwise RSIZE_MAX
//! \fn size_t safe_strlen(const char* string)
//! \brief Returns length of string
//! \param[in] string pointer to string to find the length of.
//! \return 0 if string is null. length of string if null terminator found.
//! Will scan up to RSIZE_MAX characters and may return RSIZE_MAX if null is not found
#if defined(_MSC_VER) && !defined(__clang__)
    M_NODISCARD M_PARAM_RO(1) M_NULL_TERM_STRING(1) M_INLINE size_t safe_strlen(const char* M_NULLABLE string)
#else
M_NODISCARD M_PARAM_RO(1) M_NULL_TERM_STRING(1) M_FORCEINLINE size_t safe_strlen(const char* M_NULLABLE string)
#endif
    {
#if defined(HAVE_BUILT_IN_OBJ_SIZE)
        return safe_strnlen(string, __builtin_object_size(string, 0) != SIZE_MAX ? __builtin_object_size(string, 0)
                                                                                 : RSIZE_MAX);
#else
    // NOTE: MSVC has _msize, but it only works on malloc'd memory, not constant
    // strings which may be sent to this function as well! no built-in way to
    // limit length based on memory size so limit to RSIZE_MAX MSFT also has the
    // heapapi with the HeapSize function, however I cannot get it to work. It
    // is always crashing internally, so we are not going to use it to try and
    // make sure the maximum is restricted at this time -TJE
    return safe_strnlen(string, RSIZE_MAX);
#endif
    }

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strcpy(char* M_RESTRICT       dest,
    //!                         rsize_t                destsz,
    //!                         const char* M_RESTRICT src)
    //! \brief safe_strcpy that works like C11 annex K's strcpy_s
    //!
    //! This copys a null terminated byte string pointed to by \a src to the
    //! character array whose first element is pointed to by \a dest.
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \return Zero on success, or an error code on failure.
    //! \note By default, this function allows overlapping source and destination buffers (memmove-like behavior).
    //! When STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlapping buffers are detected as an error.
    //! Use safe_strcpy_no_overlap when you need to explicitly require non-overlapping buffers for performance.
    //!
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
    //! - when STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlap would occur between the source and destination strings.
#    if defined(STRCPY_IS_STRCPY_NOT_STRMOVE)
    M_INLINE errno_t safe_strcpy(char* M_RESTRICT M_NONNULL dest, rsize_t destsz, const char* M_RESTRICT M_NONNULL src)
    {
        return safe_strcpy_impl(dest, destsz, src, __FILE__, __func__, __LINE__, "safe_strcpy(dest, destsz, src)");
    }
#    else
    M_INLINE errno_t safe_strcpy(char* M_NONNULL dest, rsize_t destsz, const char* M_NONNULL src)
    {
        return safe_strmove_impl(dest, destsz, src, __FILE__, __func__, __LINE__, "safe_strcpy(dest, destsz, src)");
    }
#    endif
#else
//! \def safe_strcpy
//! \brief safe_strcpy that works like C11 annex K's strcpy_s
//!
//! This copys a null terminated byte string pointed to by \a src to the
//! character array whose first element is pointed to by \a dest.
//! This version performs the bounds checking described in C11 annex K.
//! \param[in] dest pointer to the character array to write to
//! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
//! \param[in] src pointer to the null-terminated byte string to copy from
//! \return Zero on success, or an error code on failure.
//! \note By default, this function allows overlapping source and destination buffers (memmove-like behavior).
//! When STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlapping buffers are detected as an error.
//! Use safe_strcpy_no_overlap when you need to explicitly require non-overlapping buffers for performance.
//!
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
//! - when STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlap would occur between the source and destination strings.
#    if defined(STRCPY_IS_STRCPY_NOT_STRMOVE)
#        define safe_strcpy(dest, destsz, src)                                                                         \
            safe_strcpy_impl(dest, destsz, src, __FILE__, __func__, __LINE__,                                          \
                             "safe_strcpy(" #dest ", " #destsz ", " #src ")")
#    else
#        define safe_strcpy(dest, destsz, src)                                                                         \
            safe_strmove_impl(dest, destsz, src, __FILE__, __func__, __LINE__,                                         \
                              "safe_strcpy(" #dest ", " #destsz ", " #src ")")
#    endif
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strmove(char* M_RESTRICT      dest,
    //!                          rsize_t                destsz,
    //!                          const char* M_RESTRICT src)
    //! \brief safe_strmove works similar to safe_strcpy but allows overlapping range
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
    M_INLINE errno_t safe_strmove(char* M_NONNULL dest, rsize_t destsz, const char* M_NONNULL src)
    {
        return safe_strmove_impl(dest, destsz, src, __FILE__, __func__, __LINE__, "safe_strmove(dest, destsz, src)");
    }
#else
//! \def safe_strmove
//! \brief safe_strmove works similar to safe_strcpy but allows overlapping range
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
#    define safe_strmove(dest, destsz, src)                                                                            \
        safe_strmove_impl(dest, destsz, src, __FILE__, __func__, __LINE__,                                             \
                          "safe_strmove(" #dest ", " #destsz ", " #src ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strncpy(char* M_RESTRICT       dest,
    //!                          rsize_t                destsz,
    //!                          const char* M_RESTRICT src,
    //!                          rsize_t                count)
    //! \brief safe_strncpy works like C11 annex K's strncpy_s
    //!
    //! This copys certain number of characters from a null terminated byte string pointed to by \a src to the
    //! character array whose first element is pointed to by \a dest.
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \param[in] count maximum number of characters to copy
    //! \return Zero on success, or an error code on failure.
    //! \note By default, this function allows overlapping source and destination buffers (memmove-like behavior).
    //! When STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlapping buffers are detected as an error.
    //! Use safe_strncpy_no_overlap when you need to explicitly require non-overlapping buffers for performance.
    //!
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
    //!   strnlen_s(src, count); truncation would occur.
    //!
    //! - when STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlap would occur between the source and destination strings.
#    if defined(STRCPY_IS_STRCPY_NOT_STRMOVE)
    M_INLINE errno_t safe_strncpy(char* M_RESTRICT M_NONNULL       dest,
                                  rsize_t                          destsz,
                                  const char* M_RESTRICT M_NONNULL src,
                                  rsize_t                          count)
    {
        return safe_strncpy_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                 "safe_strncpy(dest, destsz, src, count)");
    }
#    else
    M_INLINE errno_t safe_strncpy(char* M_NONNULL dest, rsize_t destsz, const char* M_NONNULL src, rsize_t count)
    {
        return safe_strnmove_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                  "safe_strncpy(dest, destsz, src, count)");
    }
#    endif
#else
//! \def safe_strncpy
//! \brief safe_strncpy works like C11 annex K's strncpy_s
//!
//! This copys certain number of characters from a null terminated byte string pointed to by \a src to the
//! character array whose first element is pointed to by \a dest.
//! This version performs the bounds checking described in C11 annex K.
//! \param[in] dest pointer to the character array to write to
//! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
//! \param[in] src pointer to the null-terminated byte string to copy from
//! \param[in] count maximum number of characters to copy
//! \return Zero on success, or an error code on failure.
//! \note By default, this function allows overlapping source and destination buffers (memmove-like behavior).
//! When STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlapping buffers are detected as an error.
//! Use safe_strncpy_no_overlap when you need to explicitly require non-overlapping buffers for performance.
//!
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
//!   strnlen_s(src, count); truncation would occur.
//!
//! - when STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlap would occur between the source and destination strings.
#    if defined(STRCPY_IS_STRCPY_NOT_STRMOVE)
#        define safe_strncpy(dest, destsz, src, count)                                                                 \
            safe_strncpy_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                                  \
                              "safe_strncpy(" #dest ", " #destsz ", " #src ", " #count ")")
#    else
#        define safe_strncpy(dest, destsz, src, count)                                                                 \
            safe_strnmove_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                                 \
                               "safe_strncpy(" #dest ", " #destsz ", " #src ", " #count ")")
#    endif
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strnmove(char* M_RESTRICT       dest,
    //!                           rsize_t                destsz,
    //!                           const char* M_RESTRICT src,
    //!                           rsize_t                count)
    //! \brief safe_strnmove is similar to safe_strncpy but allows overlapping range
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
    //!   strnlen_s(src, count); truncation would occur.
    M_INLINE errno_t safe_strnmove(char* M_NONNULL dest, rsize_t destsz, const char* M_NONNULL src, rsize_t count)
    {
        return safe_strnmove_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                  "safe_strnmove(dest, destsz, src, count)");
    }
#else
//! \def safe_strnmove
//! \brief safe_strnmove is similar to safe_strncpy but allows overlapping range
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
//!   strnlen_s(src, count); truncation would occur.
#    define safe_strnmove(dest, destsz, src, count)                                                                    \
        safe_strnmove_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                                     \
                           "safe_strnmove(" #dest ", " #destsz ", " #src ", " #count ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strcat(char* M_RESTRICT       dest,
    //!                         rsize_t                destsz,
    //!                         const char* M_RESTRICT src)
    //! \brief safe_strcat works like C11 annex K's strcat_s
    //!
    //! Appends a copy of the null-terminated byte string pointed to by \a src to the end of the null-terminated
    //! byte string pointed to by \a dest. The character \a src[0] replaces the null terminator at the end of \a dest.
    //! The resulting byte string is null-terminated.
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \return Zero on success, or an error code on failure.
    //! \note By default, this function allows overlapping source and destination buffers (memmove-like behavior).
    //! When STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlapping buffers are detected as an error.
    //! Use safe_strcat_no_overlap when you need to explicitly require non-overlapping buffers for performance.
    //!
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
    //! - when STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlap would occur between \a src and \a dest strings
#    if defined(STRCPY_IS_STRCPY_NOT_STRMOVE)
    M_INLINE errno_t safe_strcat(char* M_RESTRICT M_NONNULL dest, rsize_t destsz, const char* M_RESTRICT M_NONNULL src)
    {
        return safe_strcat_impl(dest, destsz, src, __FILE__, __func__, __LINE__, "safe_strcat(dest, destsz, src)");
    }
#    else
    M_INLINE errno_t safe_strcat(char* M_NONNULL dest, rsize_t destsz, const char* M_NONNULL src)
    {
        return safe_strcatmove_impl(dest, destsz, src, __FILE__, __func__, __LINE__, "safe_strcat(dest, destsz, src)");
    }
#    endif
#else
//! \def safe_strcat
//! \brief safe_strcat works like C11 annex K's strcat_s
//!
//! Appends a copy of the null-terminated byte string pointed to by \a src to the end of the null-terminated
//! byte string pointed to by \a dest. The character \a src[0] replaces the null terminator at the end of \a dest.
//! The resulting byte string is null-terminated.
//! This version performs the bounds checking described in C11 annex K.
//! \param[in] dest pointer to the character array to write to
//! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
//! \param[in] src pointer to the null-terminated byte string to copy from
//! \return Zero on success, or an error code on failure.
//! \note By default, this function allows overlapping source and destination buffers (memmove-like behavior).
//! When STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlapping buffers are detected as an error.
//! Use safe_strcat_no_overlap when you need to explicitly require non-overlapping buffers for performance.
//!
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
//! - when STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlap would occur between \a src and \a dest strings
#    if defined(STRCPY_IS_STRCPY_NOT_STRMOVE)
#        define safe_strcat(dest, destsz, src)                                                                         \
            safe_strcat_impl(dest, destsz, src, __FILE__, __func__, __LINE__,                                          \
                             "safe_strcat(" #dest ", " #destsz ", " #src ")")
#    else
#        define safe_strcat(dest, destsz, src)                                                                         \
            safe_strcatmove_impl(dest, destsz, src, __FILE__, __func__, __LINE__,                                      \
                                 "safe_strcat(" #dest ", " #destsz ", " #src ")")
#    endif
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strncat(char* M_RESTRICT       dest,
    //!                          rsize_t                destsz,
    //!                          const char* M_RESTRICT src,
    //!                          rsize_t                count)
    //! \brief safe_strncat works like C11 annex K's strncat_s
    //!
    //! Appends at most count characters from the character array pointed to by \a src, stopping if the null character
    //! is found, to the end of the null-terminated byte string pointed to by \a dest. The character \a src[0]
    //! replaces the null terminator at the end of \a dest. The terminating null character is always appended in the
    //! end (so the maximum number of bytes the function may write is \a count + 1)
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \param[in] count maximum number of characters to copy
    //! \return Zero on success, or an error code on failure.
    //! \note By default, this function allows overlapping source and destination buffers (memmove-like behavior).
    //! When STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlapping buffers are detected as an error.
    //! Use safe_strncat_no_overlap when you need to explicitly require non-overlapping buffers for performance.
    //!
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
    //! - truncation would occur due to not enough space in \a dest to concatenate \a src or \a count bytes of \a src
    //!
    //! - when STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlap would occur between \a src and \a dest strings
#    if defined(STRCPY_IS_STRCPY_NOT_STRMOVE)
    M_INLINE errno_t safe_strncat(char* M_RESTRICT M_NONNULL       dest,
                                  rsize_t                          destsz,
                                  const char* M_RESTRICT M_NONNULL src,
                                  rsize_t                          count)
    {
        return safe_strncat_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                 "safe_strncat(dest, destsz, src, count)");
    }
#    else
    M_INLINE errno_t safe_strncat(char* M_NONNULL dest, rsize_t destsz, const char* M_NONNULL src, rsize_t count)
    {
        return safe_strncatmove_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                     "safe_strncat(dest, destsz, src, count)");
    }
#    endif
#else
//! \def safe_strncat
//! \brief safe_strncat works like C11 annex K's strncat_s
//!
//! Appends at most count characters from the character array pointed to by \a src, stopping if the null character
//! is found, to the end of the null-terminated byte string pointed to by \a dest. The character \a src[0]
//! replaces the null terminator at the end of \a dest. The terminating null character is always appended in the
//! end (so the maximum number of bytes the function may write is \a count + 1)
//! This version performs the bounds checking described in C11 annex K.
//! \param[in] dest pointer to the character array to write to
//! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
//! \param[in] src pointer to the null-terminated byte string to copy from
//! \param[in] count maximum number of characters to copy
//! \return Zero on success, or an error code on failure.
//! \note By default, this function allows overlapping source and destination buffers (memmove-like behavior).
//! When STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlapping buffers are detected as an error.
//! Use safe_strncat_no_overlap when you need to explicitly require non-overlapping buffers for performance.
//!
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
//! - truncation would occur due to not enough space in \a dest to concatenate \a src or \a count bytes of \a src
//!
//! - when STRCPY_IS_STRCPY_NOT_STRMOVE is defined, overlap would occur between \a src and \a dest strings
#    if defined(STRCPY_IS_STRCPY_NOT_STRMOVE)
#        define safe_strncat(dest, destsz, src, count)                                                                 \
            safe_strncat_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                                  \
                              "safe_strncat(" #dest ", " #destsz ", " #src ", " #count ")")
#    else
#        define safe_strncat(dest, destsz, src, count)                                                                 \
            safe_strncatmove_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                              \
                                  "safe_strncat(" #dest ", " #destsz ", " #src ", " #count ")")
#    endif
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strcpy_no_overlap(char* M_RESTRICT       dest,
    //!                                    rsize_t                destsz,
    //!                                    const char* M_RESTRICT src)
    //! \brief safe_strcpy variant for non-overlapping buffers with explicit performance intent.
    //!
    //! This function copies a null-terminated string from \a src to \a dest,
    //! with the explicit requirement that source and destination must not overlap.
    //! Use this when you know for certain that buffers don't overlap and need the performance benefit.
    //!
    //! This is semantically equivalent to safe_strcpy when STRCPY_IS_STRCPY_NOT_STRMOVE is defined,
    //! but makes the intent clear when called explicitly.
    //!
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \return Zero on success, or an error code on failure.
    //! \note Overlapping buffers will be detected and reported as an error.
    M_INLINE errno_t safe_strcpy_no_overlap(char* M_RESTRICT M_NONNULL       dest,
                                            rsize_t                          destsz,
                                            const char* M_RESTRICT M_NONNULL src)
    {
        return safe_strcpy_impl(dest, destsz, src, __FILE__, __func__, __LINE__,
                                "safe_strcpy_no_overlap(dest, destsz, src)");
    }
#else
//! \def safe_strcpy_no_overlap
//! \brief safe_strcpy variant for non-overlapping buffers with explicit performance intent.
//!
//! This function copies a null-terminated string from \a src to \a dest,
//! with the explicit requirement that source and destination must not overlap.
//! Use this when you know for certain that buffers don't overlap and need the performance benefit.
//!
//! This is semantically equivalent to safe_strcpy when STRCPY_IS_STRCPY_NOT_STRMOVE is defined,
//! but makes the intent clear when called explicitly.
//!
//! \param[in] dest pointer to the character array to write to
//! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
//! \param[in] src pointer to the null-terminated byte string to copy from
//! \return Zero on success, or an error code on failure.
//! \note Overlapping buffers will be detected and reported as an error.
#    define safe_strcpy_no_overlap(dest, destsz, src)                                                                  \
        safe_strcpy_impl(dest, destsz, src, __FILE__, __func__, __LINE__,                                              \
                         "safe_strcpy_no_overlap(" #dest ", " #destsz ", " #src ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strcat_no_overlap(char* M_RESTRICT       dest,
    //!                                    rsize_t                destsz,
    //!                                    const char* M_RESTRICT src)
    //! \brief safe_strcat variant for non-overlapping buffers with explicit performance intent.
    //!
    //! This function concatenates a null-terminated string from \a src to the end of \a dest,
    //! with the explicit requirement that source and destination must not overlap.
    //! Use this when you know for certain that buffers don't overlap and need the performance benefit.
    //!
    //! This is semantically equivalent to safe_strcat when STRCPY_IS_STRCPY_NOT_STRMOVE is defined,
    //! but makes the intent clear when called explicitly.
    //!
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \return Zero on success, or an error code on failure.
    //! \note Overlapping buffers will be detected and reported as an error.
    M_INLINE errno_t safe_strcat_no_overlap(char* M_RESTRICT M_NONNULL       dest,
                                            rsize_t                          destsz,
                                            const char* M_RESTRICT M_NONNULL src)
    {
        return safe_strcat_impl(dest, destsz, src, __FILE__, __func__, __LINE__,
                                "safe_strcat_no_overlap(dest, destsz, src)");
    }
#else
//! \def safe_strcat_no_overlap
//! \brief safe_strcat variant for non-overlapping buffers with explicit performance intent.
//!
//! This function concatenates a null-terminated string from \a src to the end of \a dest,
//! with the explicit requirement that source and destination must not overlap.
//! Use this when you know for certain that buffers don't overlap and need the performance benefit.
//!
//! This is semantically equivalent to safe_strcat when STRCPY_IS_STRCPY_NOT_STRMOVE is defined,
//! but makes the intent clear when called explicitly.
//!
//! \param[in] dest pointer to the character array to write to
//! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
//! \param[in] src pointer to the null-terminated byte string to copy from
//! \return Zero on success, or an error code on failure.
//! \note Overlapping buffers will be detected and reported as an error.
#    define safe_strcat_no_overlap(dest, destsz, src)                                                                  \
        safe_strcat_impl(dest, destsz, src, __FILE__, __func__, __LINE__,                                              \
                         "safe_strcat_no_overlap(" #dest ", " #destsz ", " #src ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strncpy_no_overlap(char* M_RESTRICT       dest,
    //!                                     rsize_t                destsz,
    //!                                     const char* M_RESTRICT src,
    //!                                     rsize_t                count)
    //! \brief safe_strncpy variant for non-overlapping buffers with explicit performance intent.
    //!
    //! This function copies at most \a count characters from \a src to \a dest,
    //! with the explicit requirement that source and destination must not overlap.
    //! Use this when you know for certain that buffers don't overlap and need the performance benefit.
    //!
    //! This is semantically equivalent to safe_strncpy when STRCPY_IS_STRCPY_NOT_STRMOVE is defined,
    //! but makes the intent clear when called explicitly.
    //!
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \param[in] count maximum number of characters to copy
    //! \return Zero on success, or an error code on failure.
    //! \note Overlapping buffers will be detected and reported as an error.
    M_INLINE errno_t safe_strncpy_no_overlap(char* M_RESTRICT M_NONNULL       dest,
                                             rsize_t                          destsz,
                                             const char* M_RESTRICT M_NONNULL src,
                                             rsize_t                          count)
    {
        return safe_strncpy_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                 "safe_strncpy_no_overlap(dest, destsz, src, count)");
    }
#else
//! \def safe_strncpy_no_overlap
//! \brief safe_strncpy variant for non-overlapping buffers with explicit performance intent.
//!
//! This function copies at most \a count characters from \a src to \a dest,
//! with the explicit requirement that source and destination must not overlap.
//! Use this when you know for certain that buffers don't overlap and need the performance benefit.
//!
//! This is semantically equivalent to safe_strncpy when STRCPY_IS_STRCPY_NOT_STRMOVE is defined,
//! but makes the intent clear when called explicitly.
//!
//! \param[in] dest pointer to the character array to write to
//! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
//! \param[in] src pointer to the null-terminated byte string to copy from
//! \param[in] count maximum number of characters to copy
//! \return Zero on success, or an error code on failure.
//! \note Overlapping buffers will be detected and reported as an error.
#    define safe_strncpy_no_overlap(dest, destsz, src, count)                                                          \
        safe_strncpy_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                                      \
                          "safe_strncpy_no_overlap(" #dest ", " #destsz ", " #src ", " #count ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strncat_no_overlap(char* M_RESTRICT       dest,
    //!                                     rsize_t                destsz,
    //!                                     const char* M_RESTRICT src,
    //!                                     rsize_t                count)
    //! \brief safe_strncat variant for non-overlapping buffers with explicit performance intent.
    //!
    //! This function concatenates at most \a count characters from \a src to the end of \a dest,
    //! with the explicit requirement that source and destination must not overlap.
    //! Use this when you know for certain that buffers don't overlap and need the performance benefit.
    //!
    //! This is semantically equivalent to safe_strncat when STRCPY_IS_STRCPY_NOT_STRMOVE is defined,
    //! but makes the intent clear when called explicitly.
    //!
    //! \param[in] dest pointer to the character array to write to
    //! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
    //! \param[in] src pointer to the null-terminated byte string to copy from
    //! \param[in] count maximum number of characters to copy
    //! \return Zero on success, or an error code on failure.
    //! \note Overlapping buffers will be detected and reported as an error.
    M_INLINE errno_t safe_strncat_no_overlap(char* M_RESTRICT M_NONNULL       dest,
                                             rsize_t                          destsz,
                                             const char* M_RESTRICT M_NONNULL src,
                                             rsize_t                          count)
    {
        return safe_strncat_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                 "safe_strncat_no_overlap(dest, destsz, src, count)");
    }
#else
//! \def safe_strncat_no_overlap
//! \brief safe_strncat variant for non-overlapping buffers with explicit performance intent.
//!
//! This function concatenates at most \a count characters from \a src to the end of \a dest,
//! with the explicit requirement that source and destination must not overlap.
//! Use this when you know for certain that buffers don't overlap and need the performance benefit.
//!
//! This is semantically equivalent to safe_strncat when STRCPY_IS_STRCPY_NOT_STRMOVE is defined,
//! but makes the intent clear when called explicitly.
//!
//! \param[in] dest pointer to the character array to write to
//! \param[in] destsz maximum number of characters to write, typically the size of the destination buffer
//! \param[in] src pointer to the null-terminated byte string to copy from
//! \param[in] count maximum number of characters to copy
//! \return Zero on success, or an error code on failure.
//! \note Overlapping buffers will be detected and reported as an error.
#    define safe_strncat_no_overlap(dest, destsz, src, count)                                                          \
        safe_strncat_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                                      \
                          "safe_strncat_no_overlap(" #dest ", " #destsz ", " #src ", " #count ")")
#endif

    //! \fn char* common_String_Concat_Len(char* destination, size_t
    //! destinationSizeBytes, const char* source, int sourceLength);
    //! \brief Convenience wrapper around safe_strcat that returns destination on success
    //! \param[in] destination pointer to string to concatenate another string to
    //! \param[in] destinationSizeBytes number of bytes pointed to by destination
    //! \param[in] source pointer to source string to concatenate onto \a destination.
    //! \return pointer to destination on success, null pointer on error
    M_PARAM_RW_SIZE(1, 2)
    M_PARAM_RO(3)
    M_NULL_TERM_STRING(3)
    static M_INLINE char* M_NULLABLE common_String_Concat(char* M_RESTRICT M_NONNULL       destination,
                                                          size_t                           destinationSizeBytes,
                                                          const char* M_RESTRICT M_NONNULL source)
        // clang-format off
                                                          M_DIAG_ERROR(destinationSizeBytes == 0, "destinationSizeBytes is zero")
                                                          M_DIAG_ERROR(destination == source, "source and destination buffers are equivalent")
    // clang-format on
    {
        if (0 == safe_strcat(destination, destinationSizeBytes, source))
        {
            return destination;
        }
        else
        {
            return M_NULLPTR;
        }
    }

    //! \fn char* common_String_Concat_Len(char* destination, size_t
    //! destinationSizeBytes, const char* source, int sourceLength);
    //! \brief Convenience wrapper around safe_strncat that returns destination on success
    //! \param[in] destination pointer to string to concatenate another string to
    //! \param[in] destinationSizeBytes number of bytes pointed to by destination
    //! \param[in] source pointer to source string to concatenate onto \a destination.
    //! \param[in] sourceLength number of bytes to use from source for concatenation
    //! \return pointer to destination on success, null pointer on error
    M_PARAM_RW_SIZE(1, 2)
    M_PARAM_RO_SIZE(3, 4)
    M_NULL_TERM_STRING(3)
    static M_INLINE char* M_NULLABLE common_String_Concat_Len(char* M_RESTRICT M_NONNULL       destination,
                                                              size_t                           destinationSizeBytes,
                                                              const char* M_RESTRICT M_NONNULL source,
                                                              int                              sourceLength)
        // clang-format off
                                                                M_DIAG_ERROR(destinationSizeBytes == 0, "destinationSizeBytes is zero")
                                                                M_DIAG_ERROR(destination == source, "source and destination buffers are equivalent")
                                                                M_DIAG_ERROR(sourceLength <= 0, "sourceLength is negative or zero")
    // clang-format on
    {
        if (0 == safe_strncat(destination, destinationSizeBytes, source, int_to_sizet(sourceLength)))
        {
            return destination;
        }
        else
        {
            return M_NULLPTR;
        }
    }

    //! \fn char* safe_strtok(char* M_RESTRICT       str,
    //!                             rsize_t* M_RESTRICT    strmax,
    //!                             const char* M_RESTRICT delim,
    //!                             char** M_RESTRICT      saveptr)
    //! \brief safe_strtok works like C11 annex K's strtok_s
    //!
    //! Finds the next token in a null-terminated byte string pointed to by \a str.
    //! The separator characters are identified by null-terminated byte string pointed to by \a delim.
    //! This function is designed to be called multiple times to obtain successive tokens from the same string.
    //! on every step, writes the number of characters left to see in str into \a *strmax and writes the tokenizer's
    //! internal state to \a *saveptr. Repeat calls (with null \a str) must pass \a strmax and \a saveptr
    //! with the values stored by the previous call.
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] str pointer to the null-terminated byte string to tokenize
    //! \param[in] strmax ointer to an object which initially holds the size of \a str: safe_strtok stores
    //! the number of characters that remain to be examined
    //! \param[in] delim pointer to the null-terminated byte string identifying delimiters
    //! \param[in] saveptr 	pointer to an object of type char*, which is used by safe_strtok to store its
    //! internal state
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
    //! - truncation would occur due to not enough space in \a dest to concatenate \a src or \a count bytes of \a src
    //!
    //! - overlap would occur between \a src and \a dest strings
#if defined(DEV_ENVIRONMENT)
    M_INLINE char* M_NULLABLE safe_strtok(char* M_RESTRICT M_NULLABLE            str,
                                          rsize_t* M_RESTRICT M_NONNULL          strmax,
                                          const char* M_RESTRICT M_NONNULL       delim,
                                          char* M_NONNULL* M_RESTRICT M_NULLABLE saveptr)
    {
        return safe_strtok_impl(str, strmax, delim, saveptr, __FILE__, __func__, __LINE__,
                                "safe_strtok(str, strmax, delim, saveptr)");
    }
#else
//! \def safe_strtok
//! \brief safe_strtok works like C11 annex K's strtok_s
//!
//! Finds the next token in a null-terminated byte string pointed to by \a str.
//! The separator characters are identified by null-terminated byte string pointed to by \a delim.
//! This function is designed to be called multiple times to obtain successive tokens from the same string.
//! on every step, writes the number of characters left to see in str into \a *strmax and writes the tokenizer's
//! internal state to \a *saveptr. Repeat calls (with null \a str) must pass \a strmax and \a saveptr
//! with the values stored by the previous call.
//! This version performs the bounds checking described in C11 annex K.
//! \param[in] str pointer to the null-terminated byte string to tokenize
//! \param[in] strmax ointer to an object which initially holds the size of \a str: safe_strtok stores
//! the number of characters that remain to be examined
//! \param[in] delim pointer to the null-terminated byte string identifying delimiters
//! \param[in] saveptr 	pointer to an object of type char*, which is used by safe_strtok to store its
//! internal state
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
//! - truncation would occur due to not enough space in \a dest to concatenate \a src or \a count bytes of \a src
//!
//! - overlap would occur between \a src and \a dest strings
#    define safe_strtok(str, strmax, delim, saveptr)                                                                   \
        safe_strtok_impl(str, strmax, delim, saveptr, __FILE__, __func__, __LINE__,                                    \
                         "safe_strtok(" #str ", " #strmax ", " #delim ", " #saveptr ")")
#endif

#define safe_String_Token(str, strmax, delim, saveptr) safe_strtok(str, strmax, delim, saveptr)

    //! \def common_String_Token
    //! \brief backwards compatible wrapper around safe_String_Token
    //! \ref safe_String_Token
#define common_String_Token(str, strmax, delim, saveptr) safe_String_Token(str, strmax, delim, saveptr)

#if !defined(__STDC_ALLOC_LIB__) && !defined(POSIX_2008) && !defined(USING_C23)
    // define strndup
    // NOTE: Not defining strdup since this may be available in the OS's that need
    // this definition already (Windows currently)

    //! \fn char* strndup(const char* src, size_t size)
    //! \brief implementation of POSIX strndup for systems without it.
    //!
    //! Recommend using safe_strndup instead.
    //! \param[in] src pointer to an null-terminated string to duplicate
    //! \param[in] size number of bytes to duplicate from \a src
    //! \return pointer to duplicated string on success, null pointer on error.
    M_NODISCARD M_FUNC_ATTR_MALLOC char* M_NULLABLE strndup(const char* M_NONNULL src, size_t size);
#endif // checks for when to add strndup functionality

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strdup_impl(char**      dup,
    //!                              const char* src)
    //! \brief safe_strdup works like strdup, but with security enhancements to prevent crashes.
    //!
    //! Checks that a \a src is non-null before duplicating, \a src is not length zero and not greater
    //! than \a RSIZE_MAX.
    //! The caller must free \a dup with a call to \a free() or \a safe_free().
    //! \param[out] dup pointer to use for duplicating \a src to. Must be non-NULL
    //! \param[in] src pointer to an null-terminated string to duplicate
    //! \return Zero on success, or an error code on failure.
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a dup is a null pointer
    //!
    //! - \a src is a null pointer
    //!
    //! - \a src is length 0 or greater than RSIZE_MAX
    M_INLINE errno_t safe_strdup(char* M_NONNULL* M_NULLABLE dup, const char* M_NONNULL src)
    {
        return safe_strdup_impl(dup, src, __FILE__, __func__, __LINE__, "safe_strdup(dup, src)");
    }
#else
//! \def safe_strdup
//! \brief safe_strdup works like strdup, but with security enhancements to prevent crashes.
//!
//! Checks that a \a src is non-null before duplicating, \a src is not length zero and not greater
//! than \a RSIZE_MAX.
//! The caller must free \a dup with a call to \a free() or \a safe_free().
//! \param[out] dup pointer to use for duplicating \a src to. Must be non-NULL
//! \param[in] src pointer to an null-terminated string to duplicate
//! \return Zero on success, or an error code on failure.
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a dup is a null pointer
//!
//! - \a src is a null pointer
//!
//! - \a src is length 0 or greater than RSIZE_MAX
#    define safe_strdup(dup, src)                                                                                      \
        safe_strdup_impl(dup, src, __FILE__, __func__, __LINE__, "safe_strdup(" #dup ", " #src ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strndup_impl(char**      dup,
    //!                               const char* src,
    //!                               rsize_t     size)
    //! \brief safe_strndup works like strndup, but with security enhancements to prevent crashes.
    //!
    //! Checks that a \a src is non-null before duplicating, \a src is not length zero and not greater
    //! than \a RSIZE_MAX, \a size is not zero and not greater than \a RSIZE_MAX.
    //! The caller must free \a dup with a call to \a free() or \a safe_free().
    //! \param[out] dup pointer to use for duplicating \a src to. Must be non-NULL
    //! \param[in] src pointer to an null-terminated string to duplicate
    //! \param[in] size number of bytes to duplicate from \a src
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
    M_INLINE errno_t safe_strndup(char* M_NONNULL* M_NULLABLE dup, const char* M_NONNULL src, rsize_t size)
    {
        return safe_strndup_impl(dup, src, size, __FILE__, __func__, __LINE__, "safe_strndup(dup, src, size)");
    }
#else
//! \def safe_strndup
//! \brief safe_strndup works like strndup, but with security enhancements to prevent crashes.
//!
//! Checks that a \a src is non-null before duplicating, \a src is not length zero and not greater
//! than \a RSIZE_MAX, \a size is not zero and not greater than \a RSIZE_MAX.
//! The caller must free \a dup with a call to \a free() or \a safe_free().
//! \param[out] dup pointer to use for duplicating \a src to. Must be non-NULL
//! \param[in] src pointer to an null-terminated string to duplicate
//! \param[in] size number of bytes to duplicate from \a src
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
#    define safe_strndup(dup, src, size)                                                                               \
        safe_strndup_impl(dup, src, size, __FILE__, __func__, __LINE__, "safe_strndup(" #dup ", " #src ", " #size ")")
#endif

    //! \fn void byte_Swap_String(char* stringToChange)
    //! \brief swap the bytes in a string. This is useful when interpreting ATA strings
    //! \param[out] stringToChange a pointer to the data containing a string
    //!   that needs to have the bytes swapped. Must be NULL terminated
    M_PARAM_RW(1) M_NULL_TERM_STRING(1) void byte_Swap_String(char* M_NONNULL stringToChange);

    //! \fn void byte_Swap_String_Len(char* stringToChange, size_t stringlen)
    //! \brief swap the bytes in a string up to specified length. This is useful when interpreting ATA strings
    //! \param[out] stringToChange a pointer to the data containing a string
    //!   that needs to have the bytes swapped
    //! \param[in] stringlen length to byteswap the string
    M_PARAM_RW_SIZE(1, 2)
    void byte_Swap_String_Len(char* M_NONNULL, size_t stringlen)
        // clang-format off
    M_DIAG_ERROR(stringlen % 2 != 0, "stringlen is not even")
    M_DIAG_ERROR(stringlen == 0, "stringlen is zero")
        // clang-format on
        ;

    //! \fn void remove_Whitespace_Left(char* stringToChange)
    //! \brief remove the whitespace at the beginning of a string with no repeating first char in string
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    M_PARAM_RW(1) M_NULL_TERM_STRING(1) void remove_Whitespace_Left(char* M_NONNULL stringToChange);

    //! \fn void remove_Trailing_Whitespace(char* stringToChange)
    //! \brief remove the whitespace at the end of a string
    //! \param[out] stringToChange a pointer to the data containing a string
    //!   that needs to have the ending whitespace removed. Must be NULL terminated
    M_PARAM_RW(1) M_NULL_TERM_STRING(1) void remove_Trailing_Whitespace(char* M_NONNULL stringToChange);

    //! \fn void remove_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen)
    //! \brief remove the whitespace at the end of a string of a specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //!   that needs to have the ending whitespace removed
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    M_PARAM_RW_SIZE(1, 2)
    void remove_Trailing_Whitespace_Len(char* M_NONNULL stringToChange, size_t stringlen)
        // clang-format off
    M_DIAG_ERROR(stringlen == 0, "stringlen is zero")
        // clang-format on
        ;

    //! \fn void remove_Leading_Whitespace(char* stringToChange)
    //! \brief remove the whitespace at the beginning of a string
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    M_PARAM_RW(1) M_NULL_TERM_STRING(1) void remove_Leading_Whitespace(char* M_NONNULL stringToChange);

    //! \fn void remove_Leading_Whitespace_Len(char* stringToChange, size_t stringlen)
    //! \brief remove the whitespace at the beginning of a string of a specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    M_PARAM_RW_SIZE(1, 2)
    void remove_Leading_Whitespace_Len(char* M_NONNULL stringToChange, size_t stringlen)
        // clang-format off
    M_DIAG_ERROR(stringlen == 0, "stringlen is zero")
        // clang-format on
        ;

    //! \fn void remove_Leading_And_Trailing_Whitespace(char* stringToChange)
    //! \brief remove the whitespace at the beginning and end of a string
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    M_PARAM_RW(1) M_NULL_TERM_STRING(1) void remove_Leading_And_Trailing_Whitespace(char* M_NONNULL stringToChange);

    //! \fn void remove_Leading_And_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen)
    //! \brief remove the whitespace at the beginning and end of a string of a specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange. This will be accessed as
    //! stringToChange[stringlen-1], so stringlen must be greater than 0.
    M_PARAM_RW_SIZE(1, 2)
    void remove_Leading_And_Trailing_Whitespace_Len(char* M_NONNULL stringToChange, size_t stringlen)
        // clang-format off
    M_DIAG_ERROR(stringlen == 0, "stringlen is zero")
        // clang-format on
        ;

    //! \fn void remove_Leading_And_Trailing_Control_Char(char* stringToChange)
    //! \brief remove the control char at the beginning and end of a string
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning and trailing control char removed. Must be NULL terminated
    M_PARAM_RW(1) M_NULL_TERM_STRING(1) void remove_Leading_And_Trailing_Control_Char(char* M_NONNULL stringToChange);

    //! \fn void remove_Leading_And_Trailing_Control_Char_Len(char* stringToChange, size_t stringlen)
    //! \brief remove the control char at the beginning and end of a string of a specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning and trailing control char removed
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    M_PARAM_RW_SIZE(1, 2)
    void remove_Leading_And_Trailing_Control_Char_Len(char* M_NONNULL stringToChange, size_t stringlen)
        // clang-format off
    M_DIAG_ERROR(stringlen == 0, "stringlen is zero")
        // clang-format on
        ;

    //! \fn void convert_String_To_Upper_Case(char* stringToChange)
    //! \brief Converts entire string to UPPER CASE
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    M_PARAM_RW(1) M_NULL_TERM_STRING(1) void convert_String_To_Upper_Case(char* M_NONNULL stringToChange);

    //! \fn void convert_String_To_Upper_Case(char* stringToChange)
    //! \brief Converts string to UPPER CASE for specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed.
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    M_PARAM_RW_SIZE(1, 2)
    void convert_String_To_Upper_Case_Len(char* M_NONNULL stringToChange, size_t stringlen)
        // clang-format off
    M_DIAG_ERROR(stringlen == 0, "stringlen is zero")
        // clang-format on
        ;

    //! \fn void convert_String_To_Lower_Case(char* stringToChange)
    //! \brief Converts entire string to lower case
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    M_PARAM_RW(1) M_NULL_TERM_STRING(1) void convert_String_To_Lower_Case(char* M_NONNULL stringToChange);

    //! \fn void convert_String_To_Lower_Case_Len(char* stringToChange, size_t stringlen)
    //! \brief Converts string to lower case for specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed.
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    M_PARAM_RW_SIZE(1, 2)
    void convert_String_To_Lower_Case_Len(char* M_NONNULL stringToChange, size_t stringlen)
        // clang-format off
    M_DIAG_ERROR(stringlen == 0, "stringlen is zero")
        // clang-format on
        ;

    //! \fn void convert_String_To_Inverse_Case(char* stringToChange)
    //! \brief Converts entire string to opposite case. Lower->Upper and Upper->Lower
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    M_PARAM_RW(1) M_NULL_TERM_STRING(1) void convert_String_To_Inverse_Case(char* M_NONNULL stringToChange);

    //! \fn void convert_String_To_Inverse_Case(char* stringToChange)
    //! \brief Converts string to opposite case up to specified length. Lower->Upper and Upper->Lower
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    M_PARAM_RW_SIZE(1, 2)
    void convert_String_To_Inverse_Case_Len(char* M_NONNULL stringToChange, size_t stringlen)
        // clang-format off
    M_DIAG_ERROR(stringlen == 0, "stringlen is zero")
        // clang-format on
        ;

    //! \fn size_t find_last_occurrence_in_string(const char* originalString, const char* stringToFind)
    //! \brief Find the last occurrence of one string within another string
    //! \param[in] originalString pointer to the data containing a string that will be searched
    //! \param[in] stringToFind a pointer to the data containing a string that is to be found within \a originalString
    //! \return offset to last occurrence of \a stringToFind in \a originalString. The offset will be from the end of
    //! the string
    M_PARAM_RO(1)
    M_PARAM_RO(2)
    size_t find_last_occurrence_in_string(const char* M_NONNULL originalString, const char* M_NONNULL stringToFind);

    //! \fn size_t find_first_occurrence_in_string(const char* originalString, const char* stringToFind)
    //! \brief Find the first occurrence of one string within another string
    //! \param[in] originalString pointer to the data containing a string that will be searched
    //! \param[in] stringToFind a pointer to the data containing a string that is to be found within \a originalString
    //! \return offset to first occurrence of \a stringToFind in \a originalString
    M_PARAM_RO(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(1)
    M_NULL_TERM_STRING(2)
    size_t find_first_occurrence_in_string(const char* M_NONNULL originalString, const char* M_NONNULL stringToFind);

    //! \fn bool wildcard_Match(const char* pattern, const char* data)
    //! \brief match the string with pattern consisting of wildcard chars.
    //! Character matching is case sensitive
    //!
    //! A single char wildcard match is '?'
    //! A multi char wildcard match is '*'
    //! \param[in] pattern a pointer to the pattern consisting wildcard chars. Null terminated
    //! \param[in] data a pointer to the data to search. Null terminated.
    //! \return true = found match, false = no match found
    M_PARAM_RO(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(1)
    M_NULL_TERM_STRING(2) bool wildcard_match(const char* M_NONNULL pattern, const char* M_NONNULL data);

//! \def wildcard_Match
//! \brief definition to match uppercase M in function name to lowercase named function.
//! This is for backwards compatibility.
#define wildcard_Match(pattern, data) wildcard_match(pattern, data)

    //! \fn bool wildcard_case_match(const char* pattern, const char* data)
    //! \brief match the string with pattern consisting of wildcard chars.
    //! Character matching is case insensitive
    //!
    //! A single char wildcard match is '?'
    //! A multi char wildcard match is '*'
    //! \param[in] pattern a pointer to the pattern consisting wildcard chars. Null terminated
    //! \param[in] data a pointer to the data to search. Null terminated.
    //! \return true = found match, false = no match found
    M_PARAM_RO(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(1)
    M_NULL_TERM_STRING(2) bool wildcard_case_match(const char* M_NONNULL pattern, const char* M_NONNULL data);

    //! \fn int string_version_compare(const char* string1, const char* string2)
    //! \brief Works like GNU's strvercmp function to compare two strings.
    //! \details Compares two strings taking into account numerical substrings as numbers
    //! rather than as text. For example, "file9.txt" is less than "file10.txt".
    //! Both strings must be null-terminated ASCII strings.
    //! This resolves issues where jan1, jan10, jan2 would come out from alphacompare when
    //! the caller wants jan1, jan2, jan10 instead.
    //! \param[in] string1 pointer to the first null-terminated string to compare
    //! \param[in] string2 pointer to the second null-terminated string to compare
    //! \return negative value if \a string1 < \a string2, zero if they are equal,
    //! positive value if \a string1 > \a string2
    //! \note Performance not quite as good as GNU version.
    M_PARAM_RO(1)
    M_PARAM_RO(2)
    M_NULL_TERM_STRING(1)
    M_NULL_TERM_STRING(2)
    int string_version_compare(const char* M_NONNULL string1, const char* M_NONNULL string2);

#if defined(__cplusplus)
}
#endif
