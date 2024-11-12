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
// \file string_utils.h
// \brief Implements various functions to work with C style strings.
//        Many of these implement best practices for safety as well.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "env_detect.h"
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

    //-----------------------------------------------------------------------------
    //
    //  int is_ASCII(int c)
    //
    //! \brief   Description:  This function checks if the provided character is
    //! between 0 and 7F. A.K.A. part of the standard ascii character set.
    //
    //  Entry:
    //!   \param[in] c = character to check if is an ASCII character
    //!
    //  Exit:
    //!   \return 0 = not an ASCII character. 1 = is an ASCII character
    //
    //-----------------------------------------------------------------------------
    int is_ASCII(int c);

    // All of these functions work just like the standard, but guard from undefined
    // behavior. If a value (c) is not in range of unsigned char and is not EOF,
    // then it is returned and errno is set to ERANGE If a value (c) is equal to
    // EOF, 0 is returned (for is functions) or EOF is returned (for to functions)
    // and errno is not set. All of these functions will set errno to zero before
    // calling the standard librarie's implementation of these functions, only
    // setting errno to ERANGE when outside of unsigned char's range. Any other
    // errno value comes from the standard library.
    int safe_isascii(int c);
    int safe_isalnum(int c);
    int safe_isalpha(int c);
    int safe_islower(int c);
    int safe_isupper(int c);
    int safe_isdigit(int c);
    int safe_isxdigit(int c);
    int safe_iscntrl(int c);
    int safe_isgraph(int c);
    int safe_isspace(int c);
    int safe_isblank(int c);
    int safe_isprint(int c);
    int safe_ispunct(int c);
    int safe_tolower(int c);
    int safe_toupper(int c);

    size_t safe_strnlen(const char* string, size_t n);

#if defined(__GNUC__) && (__GNUC__ > 4 || (defined(__GNUC_MINOR__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 1))
//__builtin_object_size was added to GCC 4.1.0
#    define HAVE_BUILT_IN_OBJ_SIZE
#elif defined __has_builtin
// If the compiler does not defined __GNUC__ to 4.1.0 or higher, we can check if
// it has the built - in function with this macro instead. This was added to GCC
// 10, but other GCC compatible compilers may use this if not defining a
// compatible GCC versio
#    if __has_builtin(__builtin_object_size)
#        define HAVE_BUILT_IN_OBJ_SIZE
#    endif
#endif

    // if str is a null pointer, returns 0. Internally calls safe_strnlen with size
    // set to RSIZE_MAX If __builtin_object_size can determine the amount of memory
    // allocated for the string, the limit is set to this limit, otherwise RSIZE_MAX
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

    errno_t safe_strcpy(char* M_RESTRICT dest, rsize_t destsz, const char* M_RESTRICT src);

    errno_t safe_strmove(char* dest, rsize_t destsz, const char* src);

    errno_t safe_strncpy(char* M_RESTRICT dest, rsize_t destsz, const char* M_RESTRICT src, rsize_t count);

    errno_t safe_strnmove(char* dest, rsize_t destsz, const char* src, rsize_t count);

    errno_t safe_strcat(char* M_RESTRICT dest, rsize_t destsz, const char* M_RESTRICT src);

    errno_t safe_strncat(char* M_RESTRICT dest, rsize_t destsz, const char* M_RESTRICT src, rsize_t count);

    //-----------------------------------------------------------------------------
    //
    //  char* safe_strcat(char* destination, size_t destinationSizeBytes,
    //  const char* source);
    //
    //! \brief   Description:  To be used in place of strcat.
    //
    //  Entry:
    //!   \param[in] destination = pointer to memory to write with zeroes. Must be
    //!   non-M_NULLPTR \param[in] destinationSizeBytes = number of bytes pointed to
    //!   by destination \param[in] source = pointer to source string to concatenate
    //!   onto destination. Must be M_NULLPTR terminated.
    //!
    //  Exit:
    //!   \return pointer to destination
    //
    //-----------------------------------------------------------------------------
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

    //-----------------------------------------------------------------------------
    //
    //  char* common_String_Concat_Len(char* destination, size_t
    //  destinationSizeBytes, const char* source, int sourceLength);
    //
    //! \brief   Description:  To be used in place of strncat.
    //
    //  Entry:
    //!   \param[in] destination = pointer to memory to write with zeroes. Must be
    //!   non-M_NULLPTR \param[in] destinationSizeBytes = number of bytes pointed to
    //!   by destination \param[in] source = pointer to source string to concatenate
    //!   onto destination. null termination recommended \param[in] sourceLength =
    //!   number of bytes to use from source for concatenation
    //!
    //  Exit:
    //!   \return pointer to destination
    //
    //-----------------------------------------------------------------------------
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

    //-----------------------------------------------------------------------------
    //
    //  char *safe_String_Token(char *str, const char *delim, char **saveptr)
    //
    //! \brief   Description:  To be used in place of strtok. This tries to wrap
    //! thread safe versions of strtok when possible.
    //!                        If a thread safe version is not available, then it
    //!                        uses the strtok() function It is recommended that any
    //!                        string parsed by this function is a duplicate of the
    //!                        original (strdup) so to ensure the original string is
    //!                        not modified by any of the functions called within
    //!                        this function.
    //
    //  Entry:
    //!   \param[in] str = pointer string to tokenize/parse
    //!   \param[in] strmax = used by C11 annex K to track remaining characters to
    //!   parse. Size of the ORIGINAL string to tokenize. Required for all
    //!   implementations (Will emulate behavior as much as possible) \param[in]
    //!   delim = list of delimiters to use when tokenizing. \param[in] saveptr =
    //!   used by the thread-safe strtok functions internally to track their state.
    //!   This must be non M_NULLPTR. Does not need to be freed!
    //!
    //  Exit:
    //!   \return pointer to destination
    //
    //-----------------------------------------------------------------------------
    char* safe_String_Token(char* M_RESTRICT       str,
                            rsize_t* M_RESTRICT    strmax,
                            const char* M_RESTRICT delim,
                            char** M_RESTRICT      saveptr);

#define common_String_Token(str, strmax, delim, saveptr) safe_String_Token(str, strmax, delim, saveptr)

#if !defined(__STDC_ALLOC_LIB__) && !defined(POSIX_2008) && !defined(USING_C23)
    // define strndup
    // NOTE: Not defining strdup since this may be available in the OS's that need
    // this definition already (Windows currently)
    M_FUNC_ATTR_MALLOC char* strndup(const char* src, size_t size);
#endif // checks for when to add strndup functionality

    errno_t safe_strdup(char** dup, const char* src);

    errno_t safe_strndup(char** dup, const char* src, rsize_t size);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_String()
    //
    //! \brief   Description:  swap the bytes in a string. This is useful when
    //! interpretting ATA strings
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string
    //!   that needs to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_String(char* stringToChange);

    void byte_Swap_String_Len(char* stringToChange, size_t stringlen);

    //-----------------------------------------------------------------------------
    //
    //  remove_Whitespace_Left()
    //
    //! \brief   Description:  remove the whitespace at the beginning of a string
    //! with no repeating first char in string
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string
    //!   that needs to have the beginning whitespace removed
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void remove_Whitespace_Left(char* stringToChange);

    //-----------------------------------------------------------------------------
    //
    //  remove_Trailing_Whitespace()
    //
    //! \brief   Description:  remove the whitespace at the end of a string
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string
    //!   that needs to have the ending whitespace removed
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void remove_Trailing_Whitespace(char* stringToChange);

    void remove_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen);

    //-----------------------------------------------------------------------------
    //
    //  remove_Leading_Whitespace()
    //
    //! \brief   Description:  remove the whitespace at the beginning of a string
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string
    //!   that needs to have the beginning whitespace removed
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void remove_Leading_Whitespace(char* stringToChange);

    void remove_Leading_Whitespace_Len(char* stringToChange, size_t stringlen);

    //-----------------------------------------------------------------------------
    //
    //  remove_Leading_And_Trailing_Whitespace()
    //
    //! \brief   Description:  remove the whitespace at the beginning and end of a
    //! string
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string
    //!   that needs to have the beginning and ending whitespace removed
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void remove_Leading_And_Trailing_Whitespace(char* stringToChange);

    void remove_Leading_And_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen);

    //-----------------------------------------------------------------------------
    //
    //  convert_String_To_Upper_Case()
    //
    //! \brief   Description:  convert a string to all uppercase letters
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string
    //!   that needs to have all the characters converted to uppercase
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void convert_String_To_Upper_Case(char* stringToChange);

    void convert_String_To_Upper_Case_Len(char* stringToChange, size_t stringlen);

    //-----------------------------------------------------------------------------
    //
    //  convert_String_To_Lower_Case()
    //
    //! \brief   Description:  convert a string to all lowercase letters
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string
    //!   that needs to have all the characters converted to lowercase
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void convert_String_To_Lower_Case(char* stringToChange);

    void convert_String_To_Lower_Case_Len(char* stringToChange, size_t stringlen);

    //-----------------------------------------------------------------------------
    //
    //   convert_String_To_Inverse_Case(char *stringToChange)
    //
    //! \brief   Description:  convert uppercase characters to lowercase and
    //! lowercase characters to uppercase in a string.
    //
    //  Entry:
    //!   \param[out] stringToChange = a pointer to the data containing a string
    //!   that will be modified
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void convert_String_To_Inverse_Case(char* stringToChange);

    void convert_String_To_Inverse_Case_Len(char* stringToChange, size_t stringlen);

    //-----------------------------------------------------------------------------
    //
    //   find_last_occurrence_in_string(char *originalString, char *stringToFind)
    //
    //! \brief   Description:  convert uppercase characters to lowercase and
    //! lowercase characters to uppercase in a string.
    //
    //  Entry:
    //!   \param[in] originalString = a pointer to the data containing a string that
    //!   will be searched(superset) \param[in] stringToFind = a pointer to the data
    //!   containing a string that is to be searched(subset)
    //!
    //  Exit:
    //!   \return size_t = last occurence of 'stringToFind' in 'originalString'
    //
    //-----------------------------------------------------------------------------
    size_t find_last_occurrence_in_string(const char* originalString, const char* stringToFind);

    //-----------------------------------------------------------------------------
    //
    //   find_first_occurrence_in_string(char *originalString, char *stringToFind)
    //
    //! \brief   Description:  convert uppercase characters to lowercase and
    //! lowercase characters to uppercase in a string.
    //
    //  Entry:
    //!   \param[in] originalString = a pointer to the data containing a string that
    //!   will be searched(superset) \param[in] stringToFind = a pointer to the data
    //!   containing a string that is to be searched(subset)
    //!
    //  Exit:
    //!   \return size_t = first occurence of 'stringToFind' in 'originalString'
    //
    //-----------------------------------------------------------------------------
    size_t find_first_occurrence_in_string(const char* originalString, const char* stringToFind);

    //-----------------------------------------------------------------------------
    //
    //   wildcard_Match(char* pattern, char* data)
    //
    //! \brief   Description:  match the string with pattern consisting wildcard
    //! chars. A single char wildcard match is
    //! '?'
    //  and multi char wildcard match is '*'
    //
    //  Entry:
    //!   \param[in] pattern = a pointer to the pattern consisting wildcard chars
    //!   \param[in] data = a pointer to the data
    //!
    //  Exit:
    //!   \return bool = true is matches, false if not
    //
    //-----------------------------------------------------------------------------
    bool wildcard_Match(const char* pattern, const char* data);

#if defined(__cplusplus)
}
#endif
