// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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

#include "env_detect.h"
#include "common_types.h"
#include "string_utils.h"
#include "memory_safety.h"
#include "type_conversion.h"
#include "math_utils.h"

#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

//making it look similar to a std lib function like isPrint.
int is_ASCII(int c)
{
    //this should be even faster than the if/else approach in CPU cycles
    //instead of branching, this bit manipulation will operate quicker.
    //Basically it will check for any bits outside of 7F being set to 1 returning a positive value if they are-TJE
    return !(c & ~0x7F);
}

//This is good for checks on "is<>", not for "to<>"
static int handle_eof(int result)
{
    if (result == EOF)
    {
        errno = 0;
        return 0;
    }
    return result;
}

static int is_valid_unsigned_char_range(int c)
{
    return (c >= 0 && c <= UCHAR_MAX);
}

int safe_isascii(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(is_ASCII(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isalnum(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isalnum(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isalpha(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isalpha(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_islower(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(islower(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_tolower(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return tolower(C_CAST(unsigned char, c));
    }
    else if (c == EOF)
    {
        errno = 0;
        return EOF;
    }
    else
    {
        errno = ERANGE;
        return c;
    }
}

int safe_isupper(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isupper(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_toupper(int c)
{
    if (c >= 0 && c <= UCHAR_MAX)
    {
        errno = 0;
        return toupper(C_CAST(unsigned char, c));
    }
    else if (c == EOF)
    {
        errno = 0;
        return EOF;
    }
    else
    {
        errno = ERANGE;
        return c;
    }
}

int safe_isdigit(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isdigit(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isxdigit(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isxdigit(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_iscntrl(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(iscntrl(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isgraph(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isgraph(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isspace(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isspace(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

//isblank added in C99
//May need version check if we run into an environment that does not support this check-TJE
int safe_isblank(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isblank(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_isprint(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(isprint(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

int safe_ispunct(int c)
{
    if (is_valid_unsigned_char_range(c))
    {
        errno = 0;
        return handle_eof(ispunct(C_CAST(unsigned char, c)));
    }
    else
    {
        errno = ERANGE;
        return 0;
    }
}

//these string concatenation functions currently use snprintf for portability.
//We can look into using strlcat (BSD, glibc 2.38+) or strcpy_s (Annex_k, MSVC) as well to improve performance
// when these other calls are available.-TJE
//https://sourceware.org/glibc/wiki/FAQ#Why_no_strlcpy_.2F_strlcat.3F
// Using memccpy as it is available nearly everywhere and is significantly faster
//https://developers.redhat.com/blog/2019/08/12/efficient-string-copying-and-concatenation-in-c#
char* common_String_Concat(char* destination, size_t destinationSizeBytes, const char* source)
{
    //TODO: Overlapping range detection
    if (destination && source && destinationSizeBytes > 0)
    {
#if defined (POSIX_2001) || defined (_MSC_VER) || defined (__MINGW32__) || defined (USING_C23) || defined (BSD4_4)
        char* p = C_CAST(char*, destination + safe_strnlen(destination, destinationSizeBytes));
        size_t destinationSizeBytesAvailable = destinationSizeBytes - (C_CAST(uintptr_t, p) - C_CAST(uintptr_t, destination));
        if (M_NULLPTR == memccpy(p, source, '\0', destinationSizeBytesAvailable))
        {
            //add null terminator to the destination, overwriting last byte written to stay in bounds -TJE
            destination[destinationSizeBytes - int_to_sizet(1)] = '\0';
        }
        return destination;
#elif (defined (__FreeBSD__) && __FreeBSD__ >= 4) || (defined (__OpenBSD__) && defined(OpenBSD2_4)) || (defined (__NetBSD__) && defined (__NetBSD_Version__) && __NetBSD_Version >= 1040000300L)
        //use strlcat
        //FreeBSD 3.3 and later
        //openBSD 2.4 and later
        //netbsd  1.4.3
        strlcat(destination, source, destinationSizeBytes);
        return destination;
#elif defined (__STDC_SECURE_LIB__) || defined (HAVE_C11_ANNEX_K)
        if (0 == strcat_s(destination, destinationSizeBytes, source))
        {
            return destination;
        }
        else
        {
            return M_NULLPTR;
        }
#else //memccpy, strlcat/strcpy not available
        size_t duplen = safe_strlen(destination);
        char* dup = C_CAST(char*, safe_calloc(duplen + 1, sizeof(char)));
        if (dup)
        {
            memcpy(dup, destination, duplen + 1);
#if defined (HAVE_C11_ANNEX_K)
            snprintf_s(destination, destinationSizeBytes, "%s%s", dup, source);
#elif defined (__STDC_SECURE_LIB__)//microsoft _snprintf_s has DIFFERENT parameters...we should be using a method above when possible.-TJE
            _snprintf_s(destination, destinationSizeBytes, _TRUNCATE, "%s%s", dup, source);
#else
            snprintf(destination, destinationSizeBytes, "%s%s", dup, source);
#endif
            safe_Free(C_CAST(void**, &dup));
            return destination;
        }
#endif   
    }
    return M_NULLPTR;
}

char* common_String_Concat_Len(char* destination, size_t destinationSizeBytes, const char* source, int sourceLength)
{
    if (destination && source && destinationSizeBytes > 0 && sourceLength > 0)
    {
#if defined (POSIX_2001) || defined (_MSC_VER) || defined (__MINGW32__) || defined (USING_C23)
        char* p = C_CAST(char*, destination + safe_strnlen(destination, destinationSizeBytes));
        size_t destinationSizeBytesAvailable = destinationSizeBytes - (C_CAST(uintptr_t, p) - C_CAST(uintptr_t, destination));
        if (M_NULLPTR == memccpy(p, source, '\0', M_Min(destinationSizeBytesAvailable, int_to_sizet(sourceLength))))
        {
            if (int_to_sizet(sourceLength) >= destinationSizeBytesAvailable)
            {
                //add null terminator to the destination, overwriting last byte written to stay in bounds -TJE
                destination[destinationSizeBytes - int_to_sizet(1)] = '\0';
            }
            else
            {
                //append a null terminator after the last written byte of the string - TJE
                destination[destinationSizeBytes - destinationSizeBytesAvailable + int_to_sizet(sourceLength)] = '\0';
            }
        }
        return destination;
#elif (defined (__FreeBSD__) && __FreeBSD__ >= 4) || (defined (__OpenBSD__) && defined(OpenBSD2_4)) || (defined (__NetBSD__) && defined (__NetBSD_Version__) && __NetBSD_Version >= 1040000300L)
        //use strlcat
        //FreeBSD 3.3 and later
        //openBSD 2.4 and later
        //netbsd  1.4.3
        strlcat(destination, source, M_Min(destinationSizeBytesAvailable, int_to_sizet(sourceLength)));
        return destination;
#elif defined (__STDC_SECURE_LIB__) || defined (HAVE_C11_ANNEX_K)
        if (0 == strncat_s(destination, destinationSizeBytes, source, sourceLength))
        {
            return destination;
        }
        else
        {
            return M_NULLPTR;
        }
#else //memccpy, strlcat/strcpy not available
        size_t duplen = safe_strlen(destination);
        char* dup = C_CAST(char*, safe_calloc(duplen + 1, sizeof(char)));
        if (dup)
        {
            memcpy(dup, destination, duplen + 1);
#if defined (HAVE_C11_ANNEX_K)
            snprintf_s(destination, destinationSizeBytes, "%s%.*s", dup, sourceLength, source);
#elif defined (__STDC_SECURE_LIB__)//microsoft _snprintf_s has DIFFERENT parameters...we should be using a method above when possible.-TJE
            _snprintf_s(destination, destinationSizeBytes, _TRUNCATE, "%s%.*s", dup, sourceLength, source);
#else
            snprintf(destination, destinationSizeBytes, "%s%.*s", dup, sourceLength, source);
#endif
            safe_Free(C_CAST(void**, &dup));
            return destination;
        }
#endif
    }
    return M_NULLPTR;
}

char* safe_String_Token(char* M_RESTRICT str, rsize_t* M_RESTRICT strmax, const char* M_RESTRICT delim, char** M_RESTRICT saveptr)
{
#if defined (HAVE_C11_ANNEX_K)
    return strtok_s(str, strmax, delim, saveptr);
#elif defined (POSIX_2001) || defined (__STDC_SECURE_LIB__)
    char* token = M_NULLPTR;
    if (str != M_NULLPTR)
    {
        //Initial call of the function. Perform some validation
        if (saveptr == M_NULLPTR || strmax == M_NULLPTR)
        {
            errno = EINVAL;
            return M_NULLPTR;
        }
        else if (*strmax == 0 || *strmax > RSIZE_MAX)
        {
            errno = ERANGE;
            return M_NULLPTR;
        }
    }
    if (strmax == M_NULLPTR)//letting strtok_r and strtok_s validate delim and saveptr
    {
        errno = EINVAL;
        return M_NULLPTR;
    }
    if (*strmax == 0)
    {
        errno = EINVAL;
        return M_NULLPTR;
    }
#if defined (POSIX_2001)
    token = strtok_r(str, delim, saveptr);
#elif defined (__STDC_SECURE_LIB__)
    token = strtok_s(str, delim, saveptr);
#else
    #error "Missing strtok_r equivalent function for emulation of C11 strtok_s behavior"
#endif
    if (token)
    {
        if (*saveptr)
        {
            *strmax -= (C_CAST(uintptr_t, (*saveptr)) - C_CAST(uintptr_t, token));
        }
        else
        {
            *strmax -= safe_strlen(token);
        }
    }
    return token;
#else
    //Do not have a system provided strtok_s, strtok_r implementation, so using our own that works as closely as posisble
    //to C11 annex K strtok_s
    char* token = M_NULLPTR;
    char* end = M_NULLPTR;
    if (strmax == M_NULLPTR || delim == M_NULLPTR || saveptr == M_NULLPTR)
    {
        errno = EINVAL;
        return M_NULLPTR;
    }
    else if (*strmax == 0 || *strmax > RSIZE_MAX)
    {
        errno = ERANGE;
        return M_NULLPTR;
    }
    if (str != M_NULLPTR)
    {
        //Initial call of the function. Perform some validation
        if (saveptr == M_NULLPTR || strmax == M_NULLPTR)
        {
            errno = EINVAL;
            return M_NULLPTR;
        }
        *saveptr = str;
        *strmax = safe_strlen(str);
    }
    token = *saveptr;
    end = *saveptr + *strmax;
    if (*end == '\0')
    {
        *saveptr = end;
        *strmax = C_CAST(uintptr_t, end) - C_CAST(uintptr_t, str);
        return str;
    }
    while (*strmax > 0 && *token && !strchr(delim, *token))
    {
        token++;
        (*strmax)--;
    }
    if (*token)
    {
        *token = '\0';
        *saveptr = token + 1;
        (*strmax)--;
    }
    else
    {
        *saveptr = end;
        token = M_NULLPTR;
    }
    return token;

#endif
}

size_t safe_strnlen(const char* string, size_t n)
{
#if defined (HAVE_C11_ANNEX_K) || defined (__STDC_SECURE_LIB__)
    return strnlen_s(string, n);
#elif defined (POSIX_2008) || defined (USING_SUS4) || defined (HAVE_STRNLEN) /*also glibc 2.0, openbsd 4.8*/
    if (string != M_NULLPTR)
    {
        return strnlen(string, n);
    }
    else
    {
        return 0;
    }
#else
    //implement this ourselves with memchr after making sure string is not a null pointer
    if (string != M_NULLPTR)
    {
        const char* found = memchr(string, '\0', n);
        if (found != M_NULLPTR)
        {
            return C_CAST(size_t, C_CAST(uintptr_t, found) - C_CAST(uintptr_t, string));
        }
        else
        {
            return n;
        }
    }
    return 0;
#endif
}

#if !defined (__STDC_ALLOC_LIB__) && !defined (POSIX_2008) && !defined (USING_C23)
M_FUNC_ATTR_MALLOC char* strndup(const char* src, size_t size)
{
    size_t length = safe_strnlen(src, size);
    if (length > 0)
    {
        char* dupstr = C_CAST(char*, safe_malloc(length + 1));
        if (dupstr == M_NULLPTR)
        {
            errno = ENOMEM;
            return M_NULLPTR;
        }
        dupstr[length] = '\0';
        return C_CAST(char*, memcpy(dupstr, src, length));
    }
    else
    {
        return M_NULLPTR;
    }
}
#endif //checks for strndup

void byte_Swap_String_Len(char* stringToChange, size_t stringlen)
{
    if (stringlen > 1) // Check if the string has more than one character
    {
        for (size_t stringIter = 0; stringIter < stringlen - 1; stringIter += 2)
        {
            // Swap the characters
            char temp = stringToChange[stringIter];
            stringToChange[stringIter] = stringToChange[stringIter + 1];
            stringToChange[stringIter + 1] = temp;
        }
    }
}

//use this to swap the bytes in a string...useful for ATA strings
void byte_Swap_String(char* stringToChange)
{
    size_t stringlen = safe_strlen(stringToChange);
    if (stringlen > 1) // Check if the string has more than one character
    {
        for (size_t stringIter = 0; stringIter < stringlen - 1; stringIter += 2)
        {
            // Swap the characters
            char temp = stringToChange[stringIter];
            stringToChange[stringIter] = stringToChange[stringIter + 1];
            stringToChange[stringIter + 1] = temp;
        }
    }
}

void remove_Whitespace_Left(char* stringToChange)
{
    size_t iter = 0;
    size_t len = 0;
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    len = strspn(stringToChange, " \t\n\v\f"); //only touch spaces at the beginning of the string, not the whole string
    if (len == 0)
    {
        return;
    }

    while ((iter < (safe_strlen(stringToChange) - 1) && stringToChange[iter]))  // having issues with the isspace command leaving extra chars in the string
    {
        stringToChange[iter] = stringToChange[iter + len];
        iter++;
    }
}

void remove_Trailing_Whitespace(char* stringToChange)
{
    size_t iter = 0;
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    iter = (safe_strlen(stringToChange));
    if (iter == 0)
    {
        return;
    }
    while (iter > 0 && safe_isascii(stringToChange[iter - 1]) && safe_isspace(stringToChange[iter - 1]))
    {
        stringToChange[iter - 1] = '\0'; //replace spaces with null terminators
        iter--;
    }
}

void remove_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR || stringlen == 0)
    {
        return;
    }

    size_t iter = stringlen;
    while (iter > 0 && safe_isascii(stringToChange[iter - 1]) && safe_isspace(stringToChange[iter - 1]))
    {
        stringToChange[iter - 1] = '\0'; // Replace spaces with null terminators
        iter--;
    }
}

void remove_Leading_Whitespace(char* stringToChange)
{
    size_t iter = 0;
    size_t stringToChangeLen = 0;
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    stringToChangeLen = safe_strlen(stringToChange);
    while (safe_isascii(stringToChange[iter]) && safe_isspace(stringToChange[iter]) && iter < stringToChangeLen)
    {
        iter++;
    }
    if (iter > 0)
    {
        safe_memmove(&stringToChange[0], stringToChangeLen, &stringToChange[iter], stringToChangeLen - iter);
        memset(&stringToChange[stringToChangeLen - iter], 0, iter);//should this be a null? Or a space? Leaving as null for now since it seems to work...
    }
}

void remove_Leading_Whitespace_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR || stringlen == 0)
    {
        return;
    }

    size_t iter = 0;
    while (iter < stringlen && safe_isascii(stringToChange[iter]) && safe_isspace(stringToChange[iter]))
    {
        iter++;
    }

    if (iter > 0)
    {
        safe_memmove(stringToChange, stringlen, &stringToChange[iter], stringlen - iter);
        memset(&stringToChange[stringlen - iter], 0, iter); // Null-terminate the shifted string
    }
}

void remove_Leading_And_Trailing_Whitespace(char* stringToChange)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }

    size_t stringlen = safe_strlen(stringToChange);
    if (stringlen == 0)
    {
        return;
    }

    // Remove leading whitespace (calculate for memmove later)
    size_t start = 0;
    while (start < stringlen && safe_isascii(stringToChange[start]) && safe_isspace(stringToChange[start]))
    {
        start++;
    }

    // Remove trailing whitespace
    size_t end = stringlen;
    while (end > start && safe_isascii(stringToChange[end - 1]) && safe_isspace(stringToChange[end - 1]))
    {
        end--;
    }

    // Calculate new length after removing whitespace
    size_t newlen = end - start;

    // If there's leading whitespace, shift the string to the start
    if (start > 0)
    {
        safe_memmove(stringToChange, stringlen, &stringToChange[start], newlen);
    }

    // Null-terminate the string after the last non-whitespace character
    stringToChange[newlen] = '\0';
}

void remove_Leading_And_Trailing_Whitespace_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR || stringlen == 0)
    {
        return;
    }

    // Remove leading whitespace (calculate for memmove later)
    size_t start = 0;
    while (start < stringlen && safe_isascii(stringToChange[start]) && safe_isspace(stringToChange[start]))
    {
        start++;
    }

    // Remove trailing whitespace
    size_t end = stringlen;
    while (end > start && safe_isascii(stringToChange[end - 1]) && safe_isspace(stringToChange[end - 1]))
    {
        end--;
    }

    // Calculate new length after removing whitespace
    size_t newlen = end - start;

    // If there's leading whitespace, shift the string to the start
    if (start > 0)
    {
        safe_memmove(stringToChange, stringlen, &stringToChange[start], newlen);
    }

    // Null-terminate the string after the last non-whitespace character
    stringToChange[newlen] = '\0';
}

void convert_String_To_Upper_Case(char* stringToChange)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = 0; stringToChange[iter] != '\0'; iter++)
    {
        stringToChange[iter] = C_CAST(char, safe_toupper(stringToChange[iter]));
    }
}

void convert_String_To_Upper_Case_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = 0; iter < stringlen; iter++)
    {
        stringToChange[iter] = C_CAST(char, safe_toupper(stringToChange[iter]));
    }
}

void convert_String_To_Lower_Case(char* stringToChange)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = 0; stringToChange[iter] != '\0'; iter++)
    {
        stringToChange[iter] = C_CAST(char, safe_tolower(stringToChange[iter]));
    }
}

void convert_String_To_Lower_Case_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = 0; iter < stringlen; iter++)
    {
        stringToChange[iter] = C_CAST(char, safe_tolower(stringToChange[iter]));
    }
}

void convert_String_To_Inverse_Case(char* stringToChange)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = 0; stringToChange[iter] != '\0'; iter++)
    {
        if (safe_islower(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, safe_toupper(stringToChange[iter]));
        }
        else if (safe_isupper(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, safe_tolower(stringToChange[iter]));
        }
    }
}

void convert_String_To_Inverse_Case_Len(char* stringToChange, size_t stringlen)
{
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    for (size_t iter = 0; iter < stringlen; iter++)
    {
        if (safe_islower(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, safe_toupper(stringToChange[iter]));
        }
        else if (safe_isupper(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, safe_tolower(stringToChange[iter]));
        }
    }
}

size_t find_last_occurrence_in_string(const char* originalString, const char* stringToFind)
{
    if (originalString == M_NULLPTR || stringToFind == M_NULLPTR)
    {
        return SIZE_MAX;
    }

    size_t last_occurrence = SIZE_MAX;
    size_t stringToFindLen = safe_strlen(stringToFind);
    if (stringToFindLen == 0)
    {
        return SIZE_MAX; // Searching for an empty string is undefined
    }

    const char* stringToCompare = originalString;
    while ((stringToCompare = strstr(stringToCompare, stringToFind)) != M_NULLPTR)
    {
        last_occurrence = C_CAST(uintptr_t, stringToCompare) - C_CAST(uintptr_t, originalString);
        stringToCompare += stringToFindLen; // Move past the current found substring
    }

    return (last_occurrence != SIZE_MAX) ? last_occurrence : safe_strlen(originalString);
}

size_t find_first_occurrence_in_string(const char* originalString, const char* stringToFind)
{
    if (originalString == M_NULLPTR || stringToFind == M_NULLPTR)
    {
        return SIZE_MAX;
    }

    const char* partialString = strstr(originalString, stringToFind);
    return (partialString != M_NULLPTR) ? (C_CAST(uintptr_t, partialString) - C_CAST(uintptr_t, originalString)) : SIZE_MAX;
}

bool wildcard_Match(const char* pattern, const char* data)
{
    if (pattern == M_NULLPTR || data == M_NULLPTR)
    {
        return false;
    }
    if (*pattern == '\0' && *data == '\0')
    {
        return true;
    }
    if (*pattern == *data || *pattern == '?')
    {
        return *data != '\0' && wildcard_Match(pattern + 1, data + 1);
    }
    if (*pattern == '*')
    {
        return wildcard_Match(pattern + 1, data) || (*data != '\0' && wildcard_Match(pattern, data + 1));
    }
    return false;
}
