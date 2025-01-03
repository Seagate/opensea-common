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

//! \file string_utils.h
//! \brief Implements various functions to work with C style strings.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "env_detect.h"
#include "impl_string_utils.h"
#include "predef_env_detect.h"
#include "type_conversion.h"

#if defined(__cplusplus)
extern "C"
{
#endif

// Including strings.h to have string case compare functionality and working
// around Windows.
// TODO: improve this check as needed for other systems.
// TODO: make definitions for other functions in strings.h
#if defined(__unix__) || defined(POSIX_2001)
#    include <strings.h>
#elif defined(_WIN32)
#    if !defined(strcasecmp)
#        define strcasecmp(s1, s2) _stricmp(s1, s2)
#    endif // strcasecmp
#    if !defined(strncasecmp)
#        define strncasecmp(s1, s2, n) _strnicmp(s1, s2, n)
#    endif // strncasecmp
#else
#    error "Need string case compare definition."
#endif // __unix__, POSIX, WIN32

    //! \fn int is_ASCII(int c)
    //! \brief Checks if the provided character is part of the standard ASCII character set.
    //! \param[in] c Character to check if it is an ASCII character.
    //! \return 0 if not an ASCII character, 1 if it is an ASCII character.
    int is_ASCII(int c);

    //! \fn int safe_isascii(int c)
    //! \brief Safe version of isascii().
    //! \details Checks if the provided character is an ASCII character, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not an ASCII character, non-zero if it is an ASCII character.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isascii(int c);

    //! \fn int safe_isalnum(int c)
    //! \brief Safe version of isalnum().
    //! \details Checks if the provided character is alphanumeric, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not alphanumeric, non-zero if it is alphanumeric.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isalnum(int c);

    //! \fn int safe_isalpha(int c)
    //! \brief Safe version of isalpha().
    //! \details Checks if the provided character is alphabetic, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not alphabetic, non-zero if it is alphabetic.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isalpha(int c);

    //! \fn int safe_islower(int c)
    //! \brief Safe version of islower().
    //! \details Checks if the provided character is a lowercase letter, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not lowercase, non-zero if it is lowercase.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_islower(int c);

    //! \fn int safe_isupper(int c)
    //! \brief Safe version of isupper().
    //! \details Checks if the provided character is an uppercase letter, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not uppercase, non-zero if it is uppercase.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isupper(int c);

    //! \fn int safe_isdigit(int c)
    //! \brief Safe version of isdigit().
    //! \details Checks if the provided character is a digit, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not a digit, non-zero if it is a digit.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isdigit(int c);

    //! \fn int safe_isxdigit(int c)
    //! \brief Safe version of isxdigit().
    //! \details Checks if the provided character is a hexadecimal digit, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not a hexadecimal digit, non-zero if it is a hexadecimal digit.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isxdigit(int c);

    //! \fn int safe_iscntrl(int c)
    //! \brief Safe version of iscntrl().
    //! \details Checks if the provided character is a control character, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not a control character, non-zero if it is a control character.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_iscntrl(int c);

    //! \fn int safe_isgraph(int c)
    //! \brief Safe version of isgraph().
    //! \details Checks if the provided character has a graphical representation, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not graphical, non-zero if it is graphical.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isgraph(int c);

    //! \fn int safe_isspace(int c)
    //! \brief Safe version of isspace().
    //! \details Checks if the provided character is a whitespace character, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not whitespace, non-zero if it is whitespace.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isspace(int c);

    //! \fn int safe_isblank(int c)
    //! \brief Safe version of isblank().
    //! \details Checks if the provided character is a blank character, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not blank, non-zero if it is blank.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isblank(int c);

    //! \fn int safe_isprint(int c)
    //! \brief Safe version of isprint().
    //! \details Checks if the provided character is printable, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not printable, non-zero if it is printable.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_isprint(int c);

    //! \fn int safe_ispunct(int c)
    //! \brief Safe version of ispunct().
    //! \details Checks if the provided character is a punctuation character, guarding against undefined behavior.
    //! \param[in] c Character to check.
    //! \return 0 if not punctuation, non-zero if it is punctuation.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_ispunct(int c);

    //! \fn int safe_tolower(int c)
    //! \brief Safe version of tolower().
    //! \details Converts the provided character to lowercase, guarding against undefined behavior.
    //! \param[in] c Character to convert.
    //! \return The lowercase equivalent of \a c, or \a c if it is not an uppercase letter.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_tolower(int c);

    //! \fn int safe_toupper(int c)
    //! \brief Safe version of toupper().
    //! \details Converts the provided character to uppercase, guarding against undefined behavior.
    //! \param[in] c Character to convert.
    //! \return The uppercase equivalent of \a c, or \a c if it is not a lowercase letter.
    //! \note Sets errno to ERANGE if \a c is not in the range of unsigned char and is not EOF.
    int safe_toupper(int c);

    //! \fn size_t safe_strnlen(const char* string, size_t n)
    //! \brief Returns length of string or \a n if null terminator not found
    //! \param[in] string pointer to string to find the length of.
    //! \param[in] n maximum number of characters to scan for null terminator
    //! \return 0 if string is null. length of string if null terminator found. \a n if null terminator not found
    //! after scanning \a n characters
    size_t safe_strnlen(const char* string, size_t n);

    // if str is a null pointer, returns 0. Internally calls safe_strnlen with size
    // set to RSIZE_MAX If __builtin_object_size can determine the amount of memory
    // allocated for the string, the limit is set to this limit, otherwise RSIZE_MAX
    //! \fn ssize_t safe_strlen(const char* string)
    //! \brief Returns length of string
    //! \param[in] string pointer to string to find the length of.
    //! \return 0 if string is null. length of string if null terminator found.
    //! Will scan up to RSIZE_MAX characters and may return RSIZE_MAX if null is not found
    M_FORCEINLINE size_t safe_strlen(const char* string)
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
    M_INLINE errno_t safe_strcpy(char* M_RESTRICT dest, rsize_t destsz, const char* M_RESTRICT src)
    {
        return safe_strcpy_impl(dest, destsz, src, __FILE__, __func__, __LINE__, "safe_strcpy(dest, destsz, src)");
    }
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
#    define safe_strcpy(dest, destsz, src)                                                                             \
        safe_strcpy_impl(dest, destsz, src, __FILE__, __func__, __LINE__,                                              \
                         "safe_strcpy(" #dest ", " #destsz ", " #src ")")
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
    M_INLINE errno_t safe_strmove(char* dest, rsize_t destsz, const char* src)
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
    //! - overlap would occur between the source and destination strings.
    M_INLINE errno_t safe_strncpy(char* M_RESTRICT dest, rsize_t destsz, const char* M_RESTRICT src, rsize_t count)
    {
        return safe_strncpy_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                 "safe_strncpy(dest, destsz, src, count)");
    }
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
//! - overlap would occur between the source and destination strings.
#    define safe_strncpy(dest, destsz, src, count)                                                                     \
        safe_strncpy_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                                      \
                          "safe_strncpy(" #dest ", " #destsz ", " #src ", " #count ")")
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
    M_INLINE errno_t safe_strnmove(char* dest, rsize_t destsz, const char* src, rsize_t count)
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
    M_INLINE errno_t safe_strcat(char* M_RESTRICT dest, rsize_t destsz, const char* M_RESTRICT src)
    {
        return safe_strcat_impl(dest, destsz, src, __FILE__, __func__, __LINE__, "safe_strcat(dest, destsz, src)");
    }
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
#    define safe_strcat(dest, destsz, src)                                                                             \
        safe_strcat_impl(dest, destsz, src, __FILE__, __func__, __LINE__,                                              \
                         "safe_strcat(" #dest ", " #destsz ", " #src ")")
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
    //! - overlap would occur between \a src and \a dest strings
    M_INLINE errno_t safe_strncat(char* M_RESTRICT dest, rsize_t destsz, const char* M_RESTRICT src, rsize_t count)
    {
        return safe_strncat_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,
                                 "safe_strncat(dest, destsz, src, count)");
    }
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
//! - overlap would occur between \a src and \a dest strings
#    define safe_strncat(dest, destsz, src, count)                                                                     \
        safe_strncat_impl(dest, destsz, src, count, __FILE__, __func__, __LINE__,                                      \
                          "safe_strncat(" #dest ", " #destsz ", " #src ", " #count ")")
#endif

    //! \fn char* common_String_Concat_Len(char* destination, size_t
    //! destinationSizeBytes, const char* source, int sourceLength);
    //! \brief Convenience wrapper around safe_strcat that returns destination on success
    //! \param[in] destination pointer to string to concatenate another string to
    //! \param[in] destinationSizeBytes number of bytes pointed to by destination
    //! \param[in] source pointer to source string to concatenate onto \a destination.
    //! \return pointer to destination on success, null pointer on error
    static M_INLINE char* common_String_Concat(char* M_RESTRICT       destination,
                                               size_t                 destinationSizeBytes,
                                               const char* M_RESTRICT source)
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
    static M_INLINE char* common_String_Concat_Len(char* M_RESTRICT       destination,
                                                   size_t                 destinationSizeBytes,
                                                   const char* M_RESTRICT source,
                                                   int                    sourceLength)
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

    //! \fn char* safe_String_Token(char* M_RESTRICT       str,
    //!                             rsize_t* M_RESTRICT    strmax,
    //!                             const char* M_RESTRICT delim,
    //!                             char** M_RESTRICT      saveptr)
    //! \brief safe_String_Token works like C11 annex K's strtok_s
    //!
    //! Finds the next token in a null-terminated byte string pointed to by \a str.
    //! The separator characters are identified by null-terminated byte string pointed to by \a delim.
    //! This function is designed to be called multiple times to obtain successive tokens from the same string.
    //! on every step, writes the number of characters left to see in str into \a *strmax and writes the tokenizer's
    //! internal state to \a *saveptr. Repeat calls (with null \a str) must pass \a strmax and \a saveptr
    //! with the values stored by the previous call.
    //! This version performs the bounds checking described in C11 annex K.
    //! \param[in] str pointer to the null-terminated byte string to tokenize
    //! \param[in] strmax ointer to an object which initially holds the size of \a str: safe_String_Token stores
    //! the number of characters that remain to be examined
    //! \param[in] delim pointer to the null-terminated byte string identifying delimiters
    //! \param[in] saveptr 	pointer to an object of type char*, which is used by safe_String_Token to store its
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
    M_INLINE char* safe_String_Token(char* M_RESTRICT       str,
                                     rsize_t* M_RESTRICT    strmax,
                                     const char* M_RESTRICT delim,
                                     char** M_RESTRICT      saveptr)
    {
        return safe_String_Token_impl(str, strmax, delim, saveptr, __FILE__, __func__, __LINE__,
                                      "safe_strncat(str, strmax, delim, saveptr)");
    }
#else
//! \def safe_String_Token
//! \brief safe_String_Token works like C11 annex K's strtok_s
//!
//! Finds the next token in a null-terminated byte string pointed to by \a str.
//! The separator characters are identified by null-terminated byte string pointed to by \a delim.
//! This function is designed to be called multiple times to obtain successive tokens from the same string.
//! on every step, writes the number of characters left to see in str into \a *strmax and writes the tokenizer's
//! internal state to \a *saveptr. Repeat calls (with null \a str) must pass \a strmax and \a saveptr
//! with the values stored by the previous call.
//! This version performs the bounds checking described in C11 annex K.
//! \param[in] str pointer to the null-terminated byte string to tokenize
//! \param[in] strmax ointer to an object which initially holds the size of \a str: safe_String_Token stores
//! the number of characters that remain to be examined
//! \param[in] delim pointer to the null-terminated byte string identifying delimiters
//! \param[in] saveptr 	pointer to an object of type char*, which is used by safe_String_Token to store its
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
#    define safe_String_Token(str, strmax, delim, saveptr)                                                             \
        safe_String_Token_impl(str, strmax, delim, saveptr, __FILE__, __func__, __LINE__,                              \
                               "safe_String_Token(" #str ", " #strmax ", " #delim ", " #saveptr ")")
#endif

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
    M_FUNC_ATTR_MALLOC char* strndup(const char* src, size_t size);
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
    M_INLINE errno_t safe_strdup(char** dup, const char* src)
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
    M_INLINE errno_t safe_strndup(char** dup, const char* src, rsize_t size)
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
    void byte_Swap_String(char* stringToChange);

    //! \fn void byte_Swap_String_Len(char* stringToChange, size_t stringlen)
    //! \brief swap the bytes in a string up to specified length. This is useful when interpreting ATA strings
    //! \param[out] stringToChange a pointer to the data containing a string
    //!   that needs to have the bytes swapped
    //! \param[in] stringlen length to byteswap the string
    void byte_Swap_String_Len(char* stringToChange, size_t stringlen);

    //! \fn void remove_Whitespace_Left(char* stringToChange)
    //! \brief remove the whitespace at the beginning of a string with no repeating first char in string
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    void remove_Whitespace_Left(char* stringToChange);

    //! \fn void remove_Trailing_Whitespace(char* stringToChange)
    //! \brief remove the whitespace at the end of a string
    //! \param[out] stringToChange a pointer to the data containing a string
    //!   that needs to have the ending whitespace removed. Must be NULL terminated
    void remove_Trailing_Whitespace(char* stringToChange);

    //! \fn void remove_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen)
    //! \brief remove the whitespace at the end of a string of a specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //!   that needs to have the ending whitespace removed
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    void remove_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen);

    //! \fn void remove_Leading_Whitespace(char* stringToChange)
    //! \brief remove the whitespace at the beginning of a string
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    void remove_Leading_Whitespace(char* stringToChange);

    //! \fn void remove_Leading_Whitespace_Len(char* stringToChange, size_t stringlen)
    //! \brief remove the whitespace at the beginning of a string of a specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    void remove_Leading_Whitespace_Len(char* stringToChange, size_t stringlen);

    //! \fn void remove_Leading_And_Trailing_Whitespace(char* stringToChange)
    //! \brief remove the whitespace at the beginning and end of a string
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    void remove_Leading_And_Trailing_Whitespace(char* stringToChange);

    //! \fn void remove_Leading_And_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen)
    //! \brief remove the whitespace at the beginning and end of a string of a specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    void remove_Leading_And_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen);

    //! \fn void convert_String_To_Upper_Case(char* stringToChange)
    //! \brief Converts entire string to UPPER CASE
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    void convert_String_To_Upper_Case(char* stringToChange);

    //! \fn void convert_String_To_Upper_Case(char* stringToChange)
    //! \brief Converts string to UPPER CASE for specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed.
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    void convert_String_To_Upper_Case_Len(char* stringToChange, size_t stringlen);

    //! \fn void convert_String_To_Lower_Case(char* stringToChange)
    //! \brief Converts entire string to lower case
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    void convert_String_To_Lower_Case(char* stringToChange);

    //! \fn void convert_String_To_Lower_Case_Len(char* stringToChange, size_t stringlen)
    //! \brief Converts string to lower case for specified length
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed.
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    void convert_String_To_Lower_Case_Len(char* stringToChange, size_t stringlen);

    //! \fn void convert_String_To_Inverse_Case(char* stringToChange)
    //! \brief Converts entire string to opposite case. Lower->Upper and Upper->Lower
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed. Must be NULL terminated
    void convert_String_To_Inverse_Case(char* stringToChange);

    //! \fn void convert_String_To_Inverse_Case(char* stringToChange)
    //! \brief Converts string to opposite case up to specified length. Lower->Upper and Upper->Lower
    //! \param[out] stringToChange a pointer to the data containing a string
    //! that needs to have the beginning whitespace removed
    //! \param[in] stringlen total length of the string pointed to by \a stringToChange
    void convert_String_To_Inverse_Case_Len(char* stringToChange, size_t stringlen);

    //! \fn size_t find_last_occurrence_in_string(const char* originalString, const char* stringToFind)
    //! \brief Find the last occurrence of one string within another string
    //! \param[in] originalString pointer to the data containing a string that will be searched
    //! \param[in] stringToFind a pointer to the data containing a string that is to be found within \a originalString
    //! \return offset to last occurrence of \a stringToFind in \a originalString
    size_t find_last_occurrence_in_string(const char* originalString, const char* stringToFind);

    //! \fn size_t find_first_occurrence_in_string(const char* originalString, const char* stringToFind)
    //! \brief Find the first occurrence of one string within another string
    //! \param[in] originalString pointer to the data containing a string that will be searched
    //! \param[in] stringToFind a pointer to the data containing a string that is to be found within \a originalString
    //! \return offset to first occurrence of \a stringToFind in \a originalString
    size_t find_first_occurrence_in_string(const char* originalString, const char* stringToFind);

    //! \fn bool wildcard_Match(const char* pattern, const char* data)
    //! \brief match the string with pattern consisting of wildcard chars.
    //!
    //! A single char wildcard match is '?'
    //! A multi char wildcard match is '*'
    //! \param[in] pattern a pointer to the pattern consisting wildcard chars. Null terminated
    //! \param[in] data a pointer to the data to search. Null terminated.
    //! \return true = found match, false = no match found
    bool wildcard_Match(const char* pattern, const char* data);

#if defined(__cplusplus)
}
#endif
