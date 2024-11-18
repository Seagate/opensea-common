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
// \file io_utils.h
// \brief Implements various functions to work with IO (printf, snprintf, etc)
//        This file also has functionality for changing output colors and
//        reading user input. If working with files, it is recommended
//        secure_file.h is used instead.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "constraint_handling.h"
#include "env_detect.h"
#include "type_conversion.h"
#include <stdarg.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    //-----------------------------------------------------------------------------
    //
    //  get_And_Validate_Integer_Input()
    //
    //! \brief   Description:   Validates an input as unsigned integer & converts it
    //! to unsigned type.
    //!                         This function supports hex values such as 0xFF AEh
    //!                         etc.
    //
    //  Entry:
    //!   \param[in] strToConvert - const char * buffer to convert to integer
    //!   \param[out] outputInteger - pointer to integer to store the output
    //!
    //  Exit:
    //!   \return true if able to read in an integer number, false if invalid
    //!   format.
    //
    //-----------------------------------------------------------------------------
    M_DEPRECATED /*user the bit width specific versions instead!*/
        bool
        get_And_Validate_Integer_Input(const char* strToConvert, uint64_t* outputInteger);

    // This enum specifies which units are allowed to be at the end of the user's
    // input it must match exactly the units described in the comments. If we need
    // to expand the list of allowed units in a given input type, that is OK. All
    // callers should be using the output unit as needed and any unknown unit must
    // be treated as an error that way if these lists get expanded, a new unit does
    // not cause unexpected behavior.
    M_DECLARE_ENUM(eAllowedUnitInput,
                   ALLOW_UNIT_NONE,        /*no units allowed.*/
                   ALLOW_UNIT_DATASIZE,    /*BLOCKS, SECTORS, B, KB, KiB, MB, MiB, GB,
                                              GiB, TB, TiB*/
                   ALLOW_UNIT_SECTOR_TYPE, /*l, p, logical, physical*/
                   ALLOW_UNIT_TIME,        /*h, m, s, ms, us, ns*/
                   ALLOW_UNIT_POWER,       /*w, mw*/
                   ALLOW_UNIT_VOLTS,       /*v, mv*/
                   ALLOW_UNIT_AMPS,        /*a, ma*/
                   ALLOW_UNIT_TEMPERATURE  /*f, c, k*/
    );

    M_NODISCARD bool get_And_Validate_Integer_Input_Uint64(const char*       strToConvert,
                                                           char**            unit,
                                                           eAllowedUnitInput unittype,
                                                           uint64_t*         outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Uint32(const char*       strToConvert,
                                                           char**            unit,
                                                           eAllowedUnitInput unittype,
                                                           uint32_t*         outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Uint16(const char*       strToConvert,
                                                           char**            unit,
                                                           eAllowedUnitInput unittype,
                                                           uint16_t*         outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Uint8(const char*       strToConvert,
                                                          char**            unit,
                                                          eAllowedUnitInput unittype,
                                                          uint8_t*          outputInteger);

    M_NODISCARD bool get_And_Validate_Integer_Input_Int64(const char*       strToConvert,
                                                          char**            unit,
                                                          eAllowedUnitInput unittype,
                                                          int64_t*          outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Int32(const char*       strToConvert,
                                                          char**            unit,
                                                          eAllowedUnitInput unittype,
                                                          int32_t*          outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Int16(const char*       strToConvert,
                                                          char**            unit,
                                                          eAllowedUnitInput unittype,
                                                          int16_t*          outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_Int8(const char*       strToConvert,
                                                         char**            unit,
                                                         eAllowedUnitInput unittype,
                                                         int8_t*           outputInteger);

    M_NODISCARD bool get_And_Validate_Integer_Input_ULL(const char*         strToConvert,
                                                        char**              unit,
                                                        eAllowedUnitInput   unittype,
                                                        unsigned long long* outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_UL(const char*       strToConvert,
                                                       char**            unit,
                                                       eAllowedUnitInput unittype,
                                                       unsigned long*    outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_UI(const char*       strToConvert,
                                                       char**            unit,
                                                       eAllowedUnitInput unittype,
                                                       unsigned int*     outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_US(const char*       strToConvert,
                                                       char**            unit,
                                                       eAllowedUnitInput unittype,
                                                       unsigned short*   outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_UC(const char*       strToConvert,
                                                       char**            unit,
                                                       eAllowedUnitInput unittype,
                                                       unsigned char*    outputInteger);

    M_NODISCARD bool get_And_Validate_Integer_Input_LL(const char*       strToConvert,
                                                       char**            unit,
                                                       eAllowedUnitInput unittype,
                                                       long long*        outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_L(const char*       strToConvert,
                                                      char**            unit,
                                                      eAllowedUnitInput unittype,
                                                      long*             outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_I(const char*       strToConvert,
                                                      char**            unit,
                                                      eAllowedUnitInput unittype,
                                                      int*              outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_S(const char*       strToConvert,
                                                      char**            unit,
                                                      eAllowedUnitInput unittype,
                                                      short*            outputInteger);
    M_NODISCARD bool get_And_Validate_Integer_Input_C(const char*       strToConvert,
                                                      char**            unit,
                                                      eAllowedUnitInput unittype,
                                                      char*             outputInteger);

    M_NODISCARD bool get_And_Validate_Float_Input(const char*       strToConvert,
                                                  char**            unit,
                                                  eAllowedUnitInput unittype,
                                                  float*            outputFloat);
    M_NODISCARD bool get_And_Validate_Double_Input(const char*       strToConvert,
                                                   char**            unit,
                                                   eAllowedUnitInput unittype,
                                                   double*           outputFloat);
    M_NODISCARD bool get_And_Validate_LDouble_Input(const char*       strToConvert,
                                                    char**            unit,
                                                    eAllowedUnitInput unittype,
                                                    long double*      outputFloat);

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
// This is a generic selection macro.
// so based on the type of X, it will call the appropriate function for that
// type. similar to an overloaded function in C++ for different types. NOTE: Not
// using intX_t or uintX_t since these are type defs to one of the types in the
// macro below. NOTE: No default case so we can get a warning when this doesn't
// expand correctly.
// clang-format off
#define get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger)                                           \
    _Generic((outputInteger),                                                                                          \
             char*                  : get_And_Validate_Integer_Input_C,                                                \
             unsigned char*         : get_And_Validate_Integer_Input_UC,                                               \
             short*                 : get_And_Validate_Integer_Input_S,                                                \
             unsigned short*        : get_And_Validate_Integer_Input_US,                                               \
             int*                   : get_And_Validate_Integer_Input_I,                                                \
             unsigned int*          : get_And_Validate_Integer_Input_UI,                                               \
             long*                  : get_And_Validate_Integer_Input_L,                                                \
             unsigned long*         : get_And_Validate_Integer_Input_UL,                                               \
             long long*             : get_And_Validate_Integer_Input_LL,                                               \
             unsigned long long*    : get_And_Validate_Integer_Input_ULL,                                              \
             float*                 : get_And_Validate_Float_Input,                                                    \
             double*                : get_And_Validate_Double_Input,                                                   \
             long double*           : get_And_Validate_LDouble_Input                                                   \
            )(strToConvert, unit, unittype, outputInteger)
// clang-format on
#endif // C11

#if !defined(__STDC_ALLOC_LIB__) && !defined(POSIX_2008) && !defined(USING_C23)
    // Need getline and getdelim functions since they are not available.
    ssize_t getline(char** lineptr, size_t* n, FILE* stream);

    ssize_t getdelim(char** M_RESTRICT lineptr, size_t* M_RESTRICT n, int delimiter, FILE* stream);
#endif //!__STDC_ALLOC_LIB__ && (POSIX < 2008)

// Defining asprintf ans vasprintf for all systems that do not have these
// already asprintf and vaspritnf were in GNU C library, added in FreeBSD 2.2,
// and OpenBSD 2.3
#if !defined(__STDC_ALLOC_LIB__) && !defined(_GNU_SOURCE) && !(defined(__FreeBSD__) && __FreeBSD__ > 3) &&             \
    !(defined(__OpenBSD__) && defined(OpenBSD2_3)) && !defined(HAVE_VASPRINTF)

    M_NODISCARD FUNC_ATTR_PRINTF(2, 3) int asprintf(char** M_RESTRICT strp, const char* M_RESTRICT fmt, ...);

    M_NODISCARD FUNC_ATTR_PRINTF(2, 0) int vasprintf(char** M_RESTRICT strp, const char* M_RESTRICT fmt, va_list arg);

#endif // asprintf, vasprintf

// Microsoft doesn't have snprintf compliant with C99 until VS2015, so make our
// own definition using what microsoft does have available.
#if defined(_MSC_VER) && _MSC_VER <= 1800 && defined _WIN32
    int snprintf(char* buffer, size_t bufsz, const char* format, ...);

    int vsnprintf(char* buffer, size_t bufsz, const char* format, va_list args);
#endif

    // Not exactly the same as snprintf_s, just checking that the return code is as
    // expected when using this function to detect errors or truncation.
    // https://wiki.sei.cmu.edu/confluence/display/c/ERR33-C.+Detect+and+handle+standard+library+errors
    // This does NOT do bounds checking
    // TODO: a safe_snprintf that works as C11 annex k specifies.
    FUNC_ATTR_PRINTF(3, 4) static M_INLINE int snprintf_err_handle(char* buf, size_t bufsize, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        // Disabling this warning in GCC and Clang for now. It only seems to show in Windows at the moment-TJE
#if defined __clang__ && defined(__clang_major__) && __clang_major__ >= 3
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wformat-nonliteral"
#elif defined __GNUC__ && __GNUC__ >= 4 /*technically 4.1*/
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
        // NOLINTBEGIN(clang-analyzer-valist.Uninitialized,clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        // - false positive
        int n = vsnprintf(buf, bufsize, format, args);
        // NOLINTEND(clang-analyzer-valist.Uninitialized,clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        // - false positive
#if defined __clang__ && defined(__clang_major__) && __clang_major__ >= 3
#    pragma clang diagnostic pop
#elif defined __GNUC__ && __GNUC__ >= 4 /*technically 4.1*/
#    pragma GCC diagnostic pop
#endif

        va_end(args);

        if (n < 0 || int_to_sizet(n) >= bufsize)
        {
            errno                         = EINVAL;
            constraintErrorInfo errorInfo = {"n < 0 || n >= (bufsize)", __FILE__, __func__, __LINE__};
            invoke_Constraint_Handler("snprintf_error_handler_macro: error in snprintf", &errorInfo, errno);
        }
        return n;
    }

    int verify_Format_String_And_Args(const char* M_RESTRICT format, va_list formatargs);

    // This should be used after prompting the user to enter data.
    // The echo of the typed characters is disabled while reading input with this
    // function. Once the reading is complete, the echo is restored to normal. If
    // you wish to confirm the user input, call this again after asking for
    // confirmation and compare the strings. this will allocate memory to be free'd
    // with free(). NOTE: Recommend calling explicit_zeroes before freeing to ensure
    // data is securely removed. NOTE: inputDataLen is the allocated buffer size,
    // not necessarily how many characters were typed.
    //       this can vary depending on the low-level OS's implementation of getting
    //       a password without echoing.
    // NOTE: On really old systems, this may be limited to 8 characters. Most modern
    // systems should allow at least 128 characters
    //       As long as POSIX2001 is supported, there should be no limitation to
    //       number of characters read from this function-TJE
    eReturnValues get_Secure_User_Input(const char* prompt, char** userInput, size_t* inputDataLen);

    // NOTE: This is taken from this list:
    // https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit
    //       These may appear slightly different in each console, but will be close
    //       to the requested color
    M_DECLARE_ENUM(eConsoleColors,
                   CONSOLE_COLOR_DEFAULT,
                   CONSOLE_COLOR_BLACK,
                   CONSOLE_COLOR_RED,
                   CONSOLE_COLOR_GREEN,
                   CONSOLE_COLOR_YELLOW,
                   CONSOLE_COLOR_BLUE,
                   CONSOLE_COLOR_MAGENTA,
                   CONSOLE_COLOR_CYAN,
                   CONSOLE_COLOR_WHITE,
                   CONSOLE_COLOR_GRAY,
                   CONSOLE_COLOR_BRIGHT_BLACK = CONSOLE_COLOR_GRAY,
                   CONSOLE_COLOR_BRIGHT_RED,
                   CONSOLE_COLOR_BRIGHT_GREEN,
                   CONSOLE_COLOR_BRIGHT_YELLOW,
                   CONSOLE_COLOR_BRIGHT_BLUE,
                   CONSOLE_COLOR_BRIGHT_MAGENTA,
                   CONSOLE_COLOR_BRIGHT_CYAN,
                   CONSOLE_COLOR_BRIGHT_WHITE,
                   CONSOLE_COLOR_CURRENT);

    //-----------------------------------------------------------------------------
    //
    //  set_Console_Colors() OBSOLETE
    //
    //! \brief   Description:  Recommend using
    //! set_Console_Foreground_Background_Colors() instead!
    //!                        Set the foreground or background color in the console
    //!                        output
    //
    //  Entry:
    //!   \param[in] foregroundBackground - true = foreground, false = background
    //!   \param[in] consoleColor = one of the enum values defined in this header
    //!   for which color you want.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor);

    //-----------------------------------------------------------------------------
    //
    //  set_Console_Foreground_Background_Colors(eConsoleColors foregroundColor,
    //  eConsoleColors backgroundColor)
    //
    //! \brief   Description:  Set the foreground and background color in the
    //! console output. MUST USE CONSOLE_COLOR_... types!
    //
    //  Entry:
    //!   \param[in] foregroundColor = requested foreground color.
    //!   \param[in] backgroundColor = requested background color.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void set_Console_Foreground_Background_Colors(eConsoleColors foregroundColor, eConsoleColors backgroundColor);

    //-----------------------------------------------------------------------------
    //
    //  print_Data_Buffer()
    //
    //! \brief   Description:  print out a data buffer to the screen
    //
    //  Entry:
    //!   \param[in] dataBuffer = a pointer to the data buffer you want to print out
    //!   \param[in] bufferLen = the length that you want to print out. This can be
    //!   the length of the buffer, or anything less than that \param[in] showPrint
    //!   = set to true to show printable characters on the side of the hex output
    //!   for the buffer. Non-printable characters will be represented as dots.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Data_Buffer(uint8_t* dataBuffer, uint32_t bufferLen, bool showPrint);

    //-----------------------------------------------------------------------------
    //
    //  print_Pipe_Data()
    //
    //! \brief   Description:  print out a data buffer for piping to the next
    //! executable to the screen
    //
    //  Entry:
    //!   \param[in] dataBuffer = a pointer to the data buffer you want to print out
    //!   \param[in] bufferLen = the length that you want to print out. This can be
    //!   the length of the buffer, or anything less than that
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Pipe_Data(uint8_t* dataBuffer, uint32_t bufferLen);

    //-----------------------------------------------------------------------------
    //
    //  print_Return_Enum()
    //
    //! \brief   Description:  Prints humanized eReturnValue for a given return
    //! value
    //
    //  Entry:
    //!   \param[in] funcName = Name of function value returning from.
    //!   \param[in] ret = value to humanize
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Return_Enum(const char* funcName, eReturnValues ret);

    static M_INLINE void flush_stdout(void)
    {
        M_STATIC_CAST(void, fflush(stdout));
    }

    static M_INLINE void flush_stderr(void)
    {
        M_STATIC_CAST(void, fflush(stderr));
    }

    errno_t safe_fopen(FILE* M_RESTRICT* M_RESTRICT streamptr,
                       const char* M_RESTRICT       filename,
                       const char* M_RESTRICT       mode);

    errno_t safe_freopen(FILE* M_RESTRICT* M_RESTRICT newstreamptr,
                         const char* M_RESTRICT       filename,
                         const char* M_RESTRICT       mode,
                         FILE* M_RESTRICT             stream);

    errno_t safe_tmpfile(FILE* M_RESTRICT* M_RESTRICT streamptr);

    // Recommend using getline implementation instead as it will dynamically allocate the string for you.-TJE
    // NOTE: Max size of n is limited to INT_MAX in many cases. Windows with secure lib can support RSIZE_MAX
    char* safe_gets(char* str, rsize_t n);

// These are for specifying the base for conversion in strto(u)l(l) functions
#define BASE_0_AUTO     (0)
#define BASE_2_BINARY   (2)
#define BASE_8_OCTAL    (8)
#define BASE_10_DECIMAL (10)
#define BASE_16_HEX     (16)
#define BASE_36_MAX     (36) // this is the max base required by the standards for strtol type functions

    // These safe string to long conversion functions check for NULL ptr on str and value.
    // They properly check errno for range errors, and detect invalid conversions too
    errno_t safe_strtol(long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base);

    errno_t safe_strtoll(long long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base);

    errno_t safe_strtoul(unsigned long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base);

    errno_t safe_strtoull(unsigned long long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base);

    errno_t safe_strtoimax(intmax_t* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base);

    errno_t safe_strtoumax(uintmax_t* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base);

    errno_t safe_strtof(float* value, const char* M_RESTRICT str, char** M_RESTRICT endp);

    errno_t safe_strtod(double* value, const char* M_RESTRICT str, char** M_RESTRICT endp);

    errno_t safe_strtold(long double* value, const char* M_RESTRICT str, char** M_RESTRICT endp);

#if defined(__cplusplus)
}
#endif
