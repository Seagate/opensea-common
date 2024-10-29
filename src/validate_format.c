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
// \file validate_format.c
// \brief Performs validation for format strings according to MSFT _s or C11
// Annex-K requirements
//

#include "code_attributes.h"
#include "common_types.h"
#include "io_utils.h"
#include "memory_safety.h"
#include "string_utils.h"
#include "type_conversion.h"

#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h> //Testing for conversion failures

typedef enum eValidateFormatResultEnum
{
    VALIDATE_FORMAT_SUCCESS,
    VALIDATE_FORMAT_CONTINUE,
    VALIDATE_FORMAT_COMPLETE,
    VALIDATE_FORMAT_INVALID_FORMAT
    // Should we add a separate status for failed wchar_t conversions?
} eValidateFormatResult;

#if defined(USING_C99)
#define C_STR_LITERAL_LIMIT 4095U
#else
#define C_STR_LITERAL_LIMIT 509U
#endif

static M_INLINE eValidateFormatResult update_Format_Offset(const char* format,
                                                           char*       offsetToSpecifier,
                                                           size_t*     formatoffset,
                                                           size_t      formatLength)
{
    if (format && offsetToSpecifier && formatoffset)
    {
        *formatoffset = C_CAST(uintptr_t, offsetToSpecifier) - C_CAST(uintptr_t, format);
        if (*formatoffset >= formatLength || offsetToSpecifier[0] == '\0')
        {
            return VALIDATE_FORMAT_COMPLETE;
        }
        return VALIDATE_FORMAT_SUCCESS;
    }
    return VALIDATE_FORMAT_INVALID_FORMAT;
}

static M_INLINE eValidateFormatResult check_For_Litteral_Precent(const char* format,
                                                                 char**      offsetToSpecifier,
                                                                 size_t*     formatoffset,
                                                                 size_t      formatLength)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset && safe_strnlen(*offsetToSpecifier, 2) >= 2)
    {
        if (strncmp(*offsetToSpecifier, "%%", 2) == 0)
        {
            *offsetToSpecifier += 2;
            result = update_Format_Offset(format, *offsetToSpecifier, formatoffset, formatLength);
            if (result == VALIDATE_FORMAT_SUCCESS)
            {
                result = VALIDATE_FORMAT_CONTINUE;
            }
        }
        else
        {
            *offsetToSpecifier += 1;
            result = update_Format_Offset(format, *offsetToSpecifier, formatoffset, formatLength);
        }
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

static M_INLINE eValidateFormatResult validate_Format_Flags(const char* format,
                                                            char**      offsetToSpecifier,
                                                            size_t*     formatoffset,
                                                            size_t      formatLength)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset)
    {
        switch ((*offsetToSpecifier)[0])
        {
        case '-':
        case '+':
        case ' ':
        case '#':
        case '0':
            *offsetToSpecifier += 1;
            result = update_Format_Offset(format, *offsetToSpecifier, formatoffset, formatLength);
        }
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

static M_INLINE eValidateFormatResult validate_Format_Width(const char* format,
                                                            char**      offsetToSpecifier,
                                                            size_t*     formatoffset,
                                                            int*        width,
                                                            va_list*    args,
                                                            size_t      formatLength)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset && width && args)
    {
        if ((*offsetToSpecifier)[0] == '*')
        {
            *width = va_arg(*args,
                            int); // read this to move to next thing in arg list
            *offsetToSpecifier += 1;
            result = update_Format_Offset(format, *offsetToSpecifier, formatoffset, formatLength);
        }
        else
        {
            errno = 0; // ISO secure coding standard recommends this to ensure
                       // errno is interpretted correctly after this call
            char* endptr = M_NULLPTR;
            long  lwidth = strtol(*offsetToSpecifier, &endptr, 10);
            if (lwidth == 0 && endptr == M_NULLPTR)
            {
                // no width is present
                // leave ptr as-is
            }
            else if (((lwidth == LONG_MAX || lwidth == LONG_MIN) && errno == ERANGE) ||
                     (lwidth > INT_MAX || lwidth < INT_MIN))
            {
                // something went wrong reading the width specifier, so treat
                // this as an error
                result = VALIDATE_FORMAT_INVALID_FORMAT;
            }
            else
            {
                *width = C_CAST(int, lwidth);
                // read the width, endptr should be pointing to the next part we
                // need to look at
                *offsetToSpecifier = endptr;
                result             = update_Format_Offset(format, *offsetToSpecifier, formatoffset, formatLength);
            }
        }
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

static M_INLINE eValidateFormatResult validate_Format_Precision(const char* format,
                                                                char**      offsetToSpecifier,
                                                                size_t*     formatoffset,
                                                                int*        precision,
                                                                va_list*    args,
                                                                size_t      formatLength)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset && precision && args)
    {
        if ((*offsetToSpecifier)[0] == '.')
        {
            *offsetToSpecifier += 1;
            result = update_Format_Offset(format, *offsetToSpecifier, formatoffset, formatLength);
            if (result == VALIDATE_FORMAT_COMPLETE)
            {
                return result;
            }
            // get precision value as int or * again
            if ((*offsetToSpecifier)[0] == '*')
            {
                *precision = va_arg(*args, int); // read this to move to next thing in arg list
                *offsetToSpecifier += 1;
                result = update_Format_Offset(format, *offsetToSpecifier, formatoffset, formatLength);
                if (result == VALIDATE_FORMAT_COMPLETE)
                {
                    return result;
                }
            }
            else
            {
                errno = 0; // ISO secure coding standard recommends this to ensure
                           // errno is interpretted correctly after this call
                char* endptr     = M_NULLPTR;
                long  lprecision = strtol(*offsetToSpecifier, &endptr, 10);
                if (lprecision == 0 && endptr == M_NULLPTR)
                {
                    // no width is present
                    // leave ptr as-is
                }
                else if (((lprecision == LONG_MAX || lprecision == LONG_MIN) && errno == ERANGE) ||
                         (lprecision > INT_MAX || lprecision < INT_MIN))
                {
                    // something went wrong reading the width specifier, so
                    // treat this as an error
                    return VALIDATE_FORMAT_INVALID_FORMAT;
                }
                else
                {
                    *precision = C_CAST(int, lprecision);
                    // read the width, endptr should be pointing to the next
                    // part we need to look at
                    *offsetToSpecifier = endptr;
                    result             = update_Format_Offset(format, *offsetToSpecifier, formatoffset, formatLength);
                    if (result == VALIDATE_FORMAT_COMPLETE)
                    {
                        return result;
                    }
                }
            }
        }
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

typedef struct sverifyFormatLengthModifiers
{
    bool hh;
    bool h;
    bool l;
    bool ll;
    bool j;
    bool z;
    bool t;
    bool L;
} verifyFormatLengthModifiers, *ptrVerifyFormatLengthModifiers;

static M_INLINE eValidateFormatResult validate_Format_Length_Modifier(const char*                    format,
                                                                      char**                         offsetToSpecifier,
                                                                      size_t*                        formatoffset,
                                                                      ptrVerifyFormatLengthModifiers lmods,
                                                                      size_t                         formatLength)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset && lmods)
    {
        switch ((*offsetToSpecifier)[0])
        {
        case 'h':
            lmods->h = true;
            *offsetToSpecifier += 1;
            if ((*offsetToSpecifier)[0] == 'h')
            {
                lmods->hh = true;
                *offsetToSpecifier += 1;
            }
            break;
        case 'l':
            lmods->l = true;
            *offsetToSpecifier += 1;
            if ((*offsetToSpecifier)[0] == 'l')
            {
                lmods->ll = true;
                *offsetToSpecifier += 1;
            }
            break;
        case 'j':
            // check if long or long long and set ll to true/false based on this
            // check
            lmods->j = true;
#if defined(INTMAX_MAX) && defined(LONG_MAX) && INTMAX_MAX == LONG_MAX
            lmods->l  = true;
            lmods->ll = false;
#else
            lmods->l  = false;
            lmods->ll = true;
#endif
            break;
        case 'z':
            // check if long or long long and set ll to true/false based on this
            // check
            lmods->z = true;
#if defined(SIZE_MAX) && defined(ULONG_MAX) && SIZE_MAX == ULONG_MAX
            lmods->l  = true;
            lmods->ll = false;
#else
            lmods->l  = false;
            lmods->ll = true;
#endif
            break;
        case 't':
            // check if long or long long and set ll to true/false based on this
            // check
            lmods->t = true;
#if defined(PTRDIFF_MAX) && defined(LONG_MAX) && PTRDIFF_MAX == LONG_MAX
            lmods->l  = true;
            lmods->ll = false;
#else
            lmods->l  = false;
            lmods->ll = true;
#endif
            break;
        case 'L':
            lmods->L = true;
            break;
        }
        result = update_Format_Offset(format, *offsetToSpecifier, formatoffset, formatLength);
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

static M_INLINE eValidateFormatResult validate_Format_Integer(const char*                    format,
                                                              char**                         offsetToSpecifier,
                                                              size_t*                        formatoffset,
                                                              ptrVerifyFormatLengthModifiers lenmods,
                                                              va_list*                       args)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset && lenmods && args)
    {
        // various integer types.
        if ((*offsetToSpecifier)[0] == 'd' || (*offsetToSpecifier)[0] == 'i')
        {
            if (lenmods->hh == true)
            {
                signed char value = C_CAST(signed char, va_arg(*args, int)); // type promoted to int in
                                                                             // va_args, so access as int
                M_USE_UNUSED(value);
            }
            else if (lenmods->h == true)
            {
                signed short value = C_CAST(signed short, va_arg(*args, int)); // type promoted to int in
                                                                               // va_args, so access as int
                M_USE_UNUSED(value);
            }
            else if (lenmods->ll == true)
            {
                signed long long value = va_arg(*args, signed long long);
                M_USE_UNUSED(value);
            }
            else if (lenmods->l == true)
            {
                signed long value = va_arg(*args, signed long);
                M_USE_UNUSED(value);
            }
            else if (lenmods->j == true)
            {
                intmax_t value = va_arg(*args, intmax_t);
                M_USE_UNUSED(value);
            }
            else if (lenmods->z == true)
            {
                ssize_t value = va_arg(*args, ssize_t);
                M_USE_UNUSED(value);
            }
            else if (lenmods->t == true)
            {
                ptrdiff_t value = va_arg(*args, ptrdiff_t);
                M_USE_UNUSED(value);
            }
            else
            {
                int value = va_arg(*args, int);
                M_USE_UNUSED(value);
            }
        }
        else // unsigned
        {
            if (lenmods->hh == true)
            {
                unsigned char value = C_CAST(unsigned char, va_arg(*args,
                                                                   unsigned int)); // type promoted to unsigned int in
                                                                                   // va_args, so access as unsigned int
                M_USE_UNUSED(value);
            }
            else if (lenmods->h == true)
            {
                unsigned short value =
                    C_CAST(unsigned short, va_arg(*args,
                                                  unsigned int)); // type promoted to unsigned int in
                                                                  // va_args, so access as unsigned int
                M_USE_UNUSED(value);
            }
            else if (lenmods->ll == true)
            {
                unsigned long long value = va_arg(*args, unsigned long long);
                M_USE_UNUSED(value);
            }
            else if (lenmods->l == true)
            {
                unsigned long value = va_arg(*args, unsigned long);
                M_USE_UNUSED(value);
            }
            else if (lenmods->j == true)
            {
                uintmax_t value = va_arg(*args, uintmax_t);
                M_USE_UNUSED(value);
            }
            else if (lenmods->z == true)
            {
                size_t value = va_arg(*args, size_t);
                M_USE_UNUSED(value);
            }
            else if (lenmods->t == true)
            {
                // unsigned version of this....but this should be ok since we
                // aren't doing anything else with this right now
                ptrdiff_t value = va_arg(*args, ptrdiff_t);
                M_USE_UNUSED(value);
            }
            else
            {
                unsigned int value = va_arg(*args, unsigned int);
                M_USE_UNUSED(value);
            }
        }
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

static M_INLINE eValidateFormatResult validate_Format_Float(const char*                    format,
                                                            char**                         offsetToSpecifier,
                                                            size_t*                        formatoffset,
                                                            ptrVerifyFormatLengthModifiers lenmods,
                                                            va_list*                       args)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset && lenmods && args)
    {
        if (lenmods->L == true)
        {
            long double floatval = va_arg(*args, long double);
            M_USE_UNUSED(floatval);
        }
        else if (lenmods->l == true)
        {
            double floatval = va_arg(*args, double);
            M_USE_UNUSED(floatval);
        }
        else
        {
            double floatval = va_arg(*args, double);
            M_USE_UNUSED(floatval);
        }
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

static M_INLINE eValidateFormatResult validate_Wchar_Conversion(wint_t widechar)
{
    eValidateFormatResult result           = VALIDATE_FORMAT_SUCCESS;
    wchar_t               character        = C_CAST(wchar_t, widechar);
    char*                 convertedChar    = M_NULLPTR;
    size_t                charArrayLen     = 0;
    size_t                conversionresult = 0;
    mbstate_t             state;
    safe_memset(&state, sizeof(mbstate_t), 0, sizeof(mbstate_t));
    errno = 0; // ISO C security standard does not say this is necessary, but
               // going to make sure it is zero before moving on anyways - TJE
#if defined(__STDC_SECURE_LIB__) || defined(HAVE_C11_ANNEX_K)
    // wcrtombs_s
    if (wcrtomb_s(&charArrayLen, M_NULLPTR, 0, character, &state) != 0)
    {
        return VALIDATE_FORMAT_INVALID_FORMAT;
    }
#else
    // wcrtomb
    charArrayLen = wcrtomb(M_NULLPTR, character, &state);
#endif
    if ((charArrayLen == SIZE_MAX && errno == EILSEQ) || charArrayLen == 0)
    {
        return VALIDATE_FORMAT_INVALID_FORMAT; // Should this be a different
                                               // error for encoding failure???
    }
    convertedChar = C_CAST(char*, safe_calloc(charArrayLen + 1, sizeof(char)));
    if (convertedChar == M_NULLPTR)
    {
        return VALIDATE_FORMAT_INVALID_FORMAT; // Should this be a different
                                               // error for encoding failure???
    }
    errno = 0; // ISO C does not show this as necessary, but doing to set this
               // to zero anyways
#if defined(__STDC_SECURE_LIB__) || defined(HAVE_C11_ANNEX_K)
    // wcrtombs_s
    if (wcrtomb_s(&conversionresult, convertedChar, charArrayLen, character, &state) != 0)
    {
        safe_free(&convertedChar);
        return VALIDATE_FORMAT_INVALID_FORMAT;
    }
#else
    // wcrtomb
    conversionresult = wcrtomb(convertedChar, character, &state);
#endif
    safe_free(&convertedChar);
    if (conversionresult == SIZE_MAX || conversionresult == 0)
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

static M_INLINE eValidateFormatResult validate_Format_Char(const char*                    format,
                                                           char**                         offsetToSpecifier,
                                                           size_t*                        formatoffset,
                                                           ptrVerifyFormatLengthModifiers lenmods,
                                                           va_list*                       args)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset && lenmods && args)
    {
        if (lenmods->l == true)
        {
            wint_t character = 0;
            // wint_t's size it platform dependent
            // because of this, we need to check how to read the value from the
            // va_args based on what size it is. va_args promote smaller types
            // to int, but wint_t may be smaller or the same size which is why
            // this check is here.
#if defined(WINT_MAX) && defined(INT_MAX) && WINT_MAX < INT_MAX
            character = C_CAST(wint_t, va_arg(*args, int));
#else
            character = va_arg(*args, wint_t);
#endif
            result = validate_Wchar_Conversion(character);
        }
        else
        {
            int character = va_arg(*args, int);
            M_USE_UNUSED(character);
        }
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

static M_INLINE eValidateFormatResult validate_Format_Pointer(const char*                    format,
                                                              char**                         offsetToSpecifier,
                                                              size_t*                        formatoffset,
                                                              ptrVerifyFormatLengthModifiers lenmods,
                                                              va_list*                       args)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset && lenmods && args)
    {
        uintptr_t pointerval = C_CAST(uintptr_t, va_arg(*args, void*));
        M_USE_UNUSED(pointerval);
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

// NOTE: Conversion uses wcrtombs to make sure we avoid any potential issues
// with the conversion state across threads.
static M_INLINE eValidateFormatResult validate_WStr_Conversion(const wchar_t* string)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (string)
    {
        char*     charStr          = M_NULLPTR;
        size_t    charStrSize      = 0;
        size_t    conversionResult = 0;
        mbstate_t state;
        safe_memset(&state, sizeof(mbstate_t), 0, sizeof(mbstate_t));
        errno = 0; // ISO C does not show this as necessary, but doing to set
                   // this to zero anyways get allocation size first
#if defined(__STDC_SECURE_LIB__) || defined(HAVE_C11_ANNEX_K)
        // wcsrtombs_s
        if (wcsrtombs_s(&charStrSize, M_NULLPTR, 0, &string, 0, &state) != 0)
        {
            return VALIDATE_FORMAT_INVALID_FORMAT;
        }
#else
        // wcsrtombs
        charStrSize = wcsrtombs(M_NULLPTR, &string, 0, &state);
#endif
        if ((charStrSize == SIZE_MAX && errno == EILSEQ) || charStrSize == 0)
        {
            return VALIDATE_FORMAT_INVALID_FORMAT; // Should this be a different
                                                   // error for encoding
                                                   // failure???
        }
        charStr = C_CAST(char*, safe_calloc(charStrSize + 1,
                                            sizeof(char))); // add room for M_NULLPTR terminator
        if (charStr == M_NULLPTR)
        {
            return VALIDATE_FORMAT_INVALID_FORMAT; // Should this be a different
                                                   // error for encoding
                                                   // failure???
        }
        errno = 0; // ISO C does not show this as necessary, but doing to set
                   // this to zero anyways
#if defined(__STDC_SECURE_LIB__) || defined(HAVE_C11_ANNEX_K)
        // wcsrtombs_s
        if (wcsrtombs_s(&conversionResult, charStr, charStrSize, &string, 0, &state) != 0)
        {
            safe_free(&charStr);
            return VALIDATE_FORMAT_INVALID_FORMAT; // Should this be a different
                                                   // error for encoding
                                                   // failure???
        }
#else
        // wcsrtombs
        conversionResult = wcsrtombs(charStr, &string, charStrSize, &state);
#endif
        if ((conversionResult == SIZE_MAX && errno == EILSEQ) || conversionResult == 0)
        {
            result = VALIDATE_FORMAT_INVALID_FORMAT;
        }
        safe_free(&charStr);
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

static M_INLINE eValidateFormatResult validate_Format_String(const char*                    format,
                                                             char**                         offsetToSpecifier,
                                                             size_t*                        formatoffset,
                                                             ptrVerifyFormatLengthModifiers lenmods,
                                                             va_list*                       args)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset && lenmods && args)
    {
        if (lenmods->l == true)
        {
            wchar_t* string = va_arg(*args, wchar_t*);
            if (string == M_NULLPTR)
            {
                result = VALIDATE_FORMAT_INVALID_FORMAT;
            }
            else
            {
                result = validate_WStr_Conversion(string);
            }
        }
        else
        {
            char* string = va_arg(*args, char*);
            if (string == M_NULLPTR)
            {
                result = VALIDATE_FORMAT_INVALID_FORMAT;
            }
        }
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

static M_INLINE eValidateFormatResult validate_Format_Specifier(const char*                    format,
                                                                char**                         offsetToSpecifier,
                                                                size_t*                        formatoffset,
                                                                size_t                         formatlength,
                                                                ptrVerifyFormatLengthModifiers lenmods,
                                                                va_list*                       args)
{
    eValidateFormatResult result = VALIDATE_FORMAT_SUCCESS;
    if (format && offsetToSpecifier && formatoffset && lenmods && args)
    {
        switch ((*offsetToSpecifier)[0])
        {
        case 'c':
            result = validate_Format_Char(format, offsetToSpecifier, formatoffset, lenmods, args);
            break;
        case 'p':
            result = validate_Format_Pointer(format, offsetToSpecifier, formatoffset, lenmods, args);
            break;
        case 's':
            result = validate_Format_String(format, offsetToSpecifier, formatoffset, lenmods, args);
            break;
        case 'd':
        case 'i':
        case 'u':
        case 'x':
        case 'X':
        case 'o':
        case 'b':
            result = validate_Format_Integer(format, offsetToSpecifier, formatoffset, lenmods, args);
            break;
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'a':
        case 'A':
        case 'g':
        case 'G':
            result = validate_Format_Float(format, offsetToSpecifier, formatoffset, lenmods, args);
            break;
        case 'n':
        case '%':
        default:
            // unknown format specifier.
            // We can add handling for platform unique formats, but they should
            // only be allowed on that platform. For example, if it is Windows
            // unique, it should only be allowed in Windows, POSIX unique = only
            // POSIX, etc
            result = VALIDATE_FORMAT_INVALID_FORMAT;
            break;
        }
        if (result != VALIDATE_FORMAT_INVALID_FORMAT)
        {
            *offsetToSpecifier += 1;
            result = update_Format_Offset(format, *offsetToSpecifier, formatoffset, formatlength);
        }
    }
    else
    {
        result = VALIDATE_FORMAT_INVALID_FORMAT;
    }
    return result;
}

#define M_VALIDATE_FORMAT_RETURN_VAL(func)                                                                             \
    switch (func)                                                                                                      \
    {                                                                                                                  \
    case VALIDATE_FORMAT_SUCCESS:                                                                                      \
        break;                                                                                                         \
    case VALIDATE_FORMAT_COMPLETE:                                                                                     \
        exitloop = true;                                                                                               \
        M_FALLTHROUGH;                                                                                                 \
    case VALIDATE_FORMAT_CONTINUE:                                                                                     \
        continue;                                                                                                      \
    case VALIDATE_FORMAT_INVALID_FORMAT:                                                                               \
        return -1;                                                                                                     \
    }

// This function attempts to verify a format string for valid formatting as C11
// annex K and Microsoft _s functions do. Format must be non-M_NULLPTR, %n is
// considered insecure and not allowed (we don't use it anyways), and all
// arguments to strings must be non-M_NULLPTR. Checking for encoding errors
// would also be good, but not sure the best way to do this right now. -TJE
int verify_Format_String_And_Args(const char* M_RESTRICT format, va_list formatargs)
{
    if (format)
    {
        char* offsetToSpecifier = strstr(format, "%"); // if there are no formatting specifiers, just
                                                       // skip all the checks to return zero
        size_t formatoffset = 0;
        if (offsetToSpecifier != M_NULLPTR)
        {
            va_list args;
            size_t  formatLength = 0;
            // removed early %n detection since it would incorrectly catch %%n
            // which is a valid sequence. The rest of the code does catch %n and
            // modifiers to it, so it is safe to let it proceed. see answer from
            // S.S. Anne here for why it is necessary to make a copy of the
            // args:
            // https://stackoverflow.com/questions/3369588/pass-va-list-or-pointer-to-va-list
#if defined(va_copy)
            va_copy(args, formatargs);
#elif defined(__va_copy)
            __va_copy(args, formatargs);
#else
            // known var arg copy macro not available.
            // The only other option we have is to try a simple assignment which
            // may or may not work as expected This is what MSVC defines the
            // va_copy macro as, but other compilers may handle this some other
            // way. If we need to define other compiler specific versions to
            // make this work, we can! -TJE
            args = formatargs;
#endif
            formatoffset = C_CAST(uintptr_t, offsetToSpecifier) - C_CAST(uintptr_t, format);
            formatLength = safe_strnlen(format, C_STR_LITERAL_LIMIT);
            // check at least the strings to make sure they are non-M_NULLPTR.
            // checking for other encoding errors may be more difficult to
            // detect in here -TJE
            bool exitloop = false;
            while (offsetToSpecifier != M_NULLPTR && formatoffset < formatLength && exitloop == false)
            {
                int                         width     = 0;
                int                         precision = 0;
                verifyFormatLengthModifiers lenmods   = {false, false, false, false, false, false, false, false};
                //%% should not ever have other flags to parse or validate.
                // rather than go through parsing all of this, update the offset
                // and continue the loop
                M_VALIDATE_FORMAT_RETURN_VAL(
                    check_For_Litteral_Precent(format, &offsetToSpecifier, &formatoffset, formatLength))

                // check flags first
                M_VALIDATE_FORMAT_RETURN_VAL(
                    validate_Format_Flags(format, &offsetToSpecifier, &formatoffset, formatLength))

                // check for integer value or * for width
                M_VALIDATE_FORMAT_RETURN_VAL(
                    validate_Format_Width(format, &offsetToSpecifier, &formatoffset, &width, &args, formatLength))

                // precision
                M_VALIDATE_FORMAT_RETURN_VAL(validate_Format_Precision(format, &offsetToSpecifier, &formatoffset,
                                                                       &precision, &args, formatLength))

                // length modifier (Ex: lu, llu, ld, lld, hs, ls, etc)
                M_VALIDATE_FORMAT_RETURN_VAL(
                    validate_Format_Length_Modifier(format, &offsetToSpecifier, &formatoffset, &lenmods, formatLength))

                // finally the specifier to check the arg for
                // NOTE: Using if-else instead of a switch because of how the
                // M_VALIDATE_FORMAT_RETURN_VAL works it may not exit a
                // switch-case
                //       the way we want. It will process as expected for
                //       if-else though which is why we are doing this.-TJE
                M_VALIDATE_FORMAT_RETURN_VAL(
                    validate_Format_Specifier(format, &offsetToSpecifier, &formatoffset, formatLength, &lenmods, &args))

                // get next specifier, if any
                offsetToSpecifier = strstr(offsetToSpecifier, "%");
                if (offsetToSpecifier)
                {
                    M_VALIDATE_FORMAT_RETURN_VAL(
                        update_Format_Offset(format, offsetToSpecifier, &formatoffset, formatLength))
                }
            }
            va_end(args);
        }
        return C_CAST(int, formatoffset); // This should be safe as the format string
                                          // evaluation is limited to C_STR_LITERAL_LIMIT
                                          // which is way below SIZE_MAX
    }
    return -1;
}
