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

//making it look similar to a std lib function like isPrint.
int is_ASCII(int c)
{
    if (c >= 0x7F || c < 0)
    {
        return 0;//false
    }
    else
    {
        return 1;//true
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
    if (destination && source && destinationSizeBytes > 0)
    {
#if defined (POSIX_2001) || defined (_MSC_VER) || defined (__MINGW32__) || defined (USING_C23) || defined (BSD4_4)
        char* dup = strdup(destination);
        if (dup)
        {
            char* p = C_CAST(char*, memccpy(destination, dup, '\0', destinationSizeBytes));
            size_t destinationSizeBytesAvailable = destinationSizeBytes - (C_CAST(uintptr_t, p) - C_CAST(uintptr_t, destination) - int_to_sizet(1));
            if (M_NULLPTR == memccpy(p - 1, source, '\0', destinationSizeBytesAvailable))
            {
                //add null terminator to the destination, overwriting last byte written to stay in bounds -TJE
                destination[destinationSizeBytes - int_to_sizet(1)] = '\0';
            }
            safe_Free(C_CAST(void**, &dup));
            return destination;
        }
#elif (defined (__FreeBSD__) && __FreeBSD__ >= 4) || (defined (__OpenBSD__) && defined(OpenBSD2_4)) || (defined (__NetBSD__) && defined (__NetBSD_Version__) && __NetBSD_Version >= 1040000300L)
        //use strlcat
        //FreeBSD 3.3 and later
        //openBSD 2.4 and later
        //netbsd  1.4.3
        strlcat(destination, source, destinationSizeBytes);
        return destination;
#else //memccpy, strlcat/strcpy not available
        size_t duplen = strlen(destination);
        char* dup = C_CAST(char*, calloc(duplen + 1, sizeof(char)));
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
        char* dup = strdup(destination);
        if (dup)
        {
            char* p = C_CAST(char*, memccpy(destination, dup, '\0', destinationSizeBytes));
            size_t destinationSizeBytesAvailable = destinationSizeBytes - (C_CAST(uintptr_t, p) - C_CAST(uintptr_t, destination) - int_to_sizet(1));
            if (M_NULLPTR == memccpy(p - 1, source, '\0', M_Min(destinationSizeBytesAvailable, int_to_sizet(sourceLength))))
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
            safe_Free(C_CAST(void**, &dup));
            return destination;
        }
#elif (defined (__FreeBSD__) && __FreeBSD__ >= 4) || (defined (__OpenBSD__) && defined(OpenBSD2_4)) || (defined (__NetBSD__) && defined (__NetBSD_Version__) && __NetBSD_Version >= 1040000300L)
        //use strlcat
        //FreeBSD 3.3 and later
        //openBSD 2.4 and later
        //netbsd  1.4.3
        char* dup = C_CAST(char*, calloc(sourceLength + 1, sizeof(char)));
        if (dup)
        {
            strlcpy(dup, source, sourceLength);
            strlcat(destination, dup, destinationSizeBytes);
            safe_Free(C_CAST(void**, &dup));
            return destination;
        }
#else //memccpy, strlcat/strcpy not available
        size_t duplen = strlen(destination);
        char* dup = C_CAST(char*, calloc(duplen + 1, sizeof(char)));
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

char* common_String_Token(char* M_RESTRICT str, rsize_t* M_RESTRICT strmax, const char* M_RESTRICT delim, char** M_RESTRICT saveptr)
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
            *strmax -= strlen(token);
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
        *strmax = strlen(str);
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

size_t string_n_length(const char* string, size_t n)
{
#if defined (HAVE_C11_ANNEX_K) || defined (__STDC_SECURE_LIB__)
    return strnlen_s(string, n);
#elif defined (POSIX_2008) || defined (USING_SUS4) || defined (HAVE_STRNLEN) //also glibc 2.0, openbsd 4.8
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
            return C_CAST(size_t, found - string);
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
char* strndup(const char* src, size_t size)
{
    size_t length = string_n_length(src, size);//NOTE: Windows has this so not defining it. POSIX defines this in 2008. May need to define this if not available for an os that needs strndup
    if (length > 0)
    {
        char* dupstr = C_CAST(char*, malloc(length + 1));
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

//use this to swap the bytes in a string...useful for ATA strings
void byte_Swap_String(char* stringToChange)
{
    size_t stringlen = strlen(stringToChange) + 1;
    if (stringlen > 1)//greater than 1 since we append 1 for a null
    {
        char* swappedString = C_CAST(char*, calloc(stringlen, sizeof(char)));
        if (swappedString == M_NULLPTR)
        {
            return;
        }

        for (size_t stringIter = 0; stringIter < (stringlen - 1); stringIter += 2)//strlen - 1 to make sure M_NULLPTR terminator will not be touched with other changes made in this function -TJE
        {
            swappedString[stringIter] = stringToChange[stringIter + 1];
            if (stringIter + 1 < stringlen)
            {
                swappedString[stringIter + 1] = stringToChange[stringIter];
            }
        }
        memset(stringToChange, 0, stringlen);
        memcpy(stringToChange, swappedString, stringlen);
        safe_Free(C_CAST(void**, &swappedString));
    }
}
void remove_Whitespace_Left(char* stringToChange)
{
    size_t iter = 0, len = 0;
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    len = strspn(stringToChange, " \t\n\v\f"); //only touch spaces at the beginning of the string, not the whole string
    if (len == 0)
    {
        return;
    }

    while ((iter < (strlen(stringToChange) - 1) && stringToChange[iter]))  // having issues with the isspace command leaving extra chars in the string
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
    iter = (strlen(stringToChange));
    if (iter == 0)
    {
        return;
    }
    while (iter > 0 && is_ASCII(stringToChange[iter - 1]) && isspace(stringToChange[iter - 1]))
    {
        stringToChange[iter - 1] = '\0'; //replace spaces with M_NULLPTR terminators
        iter--;
    }
}

void remove_Leading_Whitespace(char* stringToChange)
{
    size_t iter = 0, stringToChangeLen = 0;
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    stringToChangeLen = strlen(stringToChange);
    while (is_ASCII(stringToChange[iter]) && isspace(stringToChange[iter]) && iter < stringToChangeLen)
    {
        iter++;
    }
    if (iter > 0)
    {
        memmove(&stringToChange[0], &stringToChange[iter], stringToChangeLen - iter);
        memset(&stringToChange[stringToChangeLen - iter], 0, iter);//should this be a null? Or a space? Leaving as null for now since it seems to work...
    }
}

void remove_Leading_And_Trailing_Whitespace(char* stringToChange)
{
    remove_Leading_Whitespace(stringToChange);
    remove_Trailing_Whitespace(stringToChange);
}

void convert_String_To_Upper_Case(char* stringToChange)
{
    size_t stringLen = 0, iter = 0;
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    stringLen = strlen(stringToChange);
    if (stringLen == 0)
    {
        return;
    }
    while (iter <= stringLen)
    {
        stringToChange[iter] = C_CAST(char, toupper(stringToChange[iter]));
        iter++;
    }
}

void convert_String_To_Lower_Case(char* stringToChange)
{
    size_t stringLen = 0, iter = 0;
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    stringLen = strlen(stringToChange);
    if (stringLen == 0)
    {
        return;
    }
    while (iter <= stringLen)
    {
        stringToChange[iter] = C_CAST(char, tolower(stringToChange[iter]));
        iter++;
    }
}

void convert_String_To_Inverse_Case(char* stringToChange)
{
    size_t stringLen = 0, iter = 0;
    if (stringToChange == M_NULLPTR)
    {
        return;
    }
    stringLen = strlen(stringToChange);
    if (stringLen == 0)
    {
        return;
    }
    while (iter <= stringLen)
    {
        if (islower(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, tolower(stringToChange[iter]));
        }
        else if (isupper(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, toupper(stringToChange[iter]));
        }
        iter++;
    }
}

size_t find_last_occurrence_in_string(const char* originalString, const char* stringToFind)
{
    char* stringToCompare = M_CONST_CAST(char*, originalString);//need to start at the beginning of this, but update stringToCompare as we work through the string, hence disregarding const-TJE
    size_t last_occurrence = strlen(originalString);

    while (stringToCompare != M_NULLPTR)
    {
        char* partialString = strstr(stringToCompare, stringToFind);
        if (partialString != M_NULLPTR)
        {
            last_occurrence = strlen(partialString);
            partialString += strlen(stringToFind);
            stringToCompare = strstr(partialString, stringToFind);
        }
        else
        {
            break;
        }
    }

    return last_occurrence;
}

size_t find_first_occurrence_in_string(const char* originalString, const char* stringToFind)
{
    char* partialString = strstr(originalString, stringToFind);
    if (partialString != M_NULLPTR)
    {
        return C_CAST(size_t, partialString - originalString);
    }

    return strlen(originalString);
}

bool wildcard_Match(const char* pattern, const char* data)
{
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
