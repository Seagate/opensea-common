// SPDX-License-Identifier: MPL-2.0

//! \file io_utils.c
//! \brief Implements various functions to work with IO (printf, snprintf, etc)
//!
//!        This file also has functionality for changing output colors and
//!        reading user input. If working with files, it is recommended
//!        secure_file.h is used instead.
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "io_utils.h"
#include "bit_manip.h"
#include "common_types.h"
#include "env_detect.h"
#include "memory_safety.h"
#include "string_utils.h"
#include "type_conversion.h"
#include <ctype.h>
#include <math.h>   //HUGE_VALF, HUGE_VAL, HUGE_VALL
#include <stdarg.h> //asprintf/vasprintf
#include <string.h>

#if defined(UEFI_C_SOURCE)
#    include <Library/PrintLib.h> //to convert CHAR16 string to CHAR8. may also be able to use stdlib in someway, but this seems to work
#    include <Library/UefiBootServicesTableLib.h> //to get global boot services pointer. This pointer should be checked before use, but any app using stdlib will have this set.
#    include <Protocol/SimpleTextOut.h>           //for colors
#    include <Uefi.h>
#endif // UEFI_C_SOURCE

#if defined(UEFI_C_SOURCE)
eReturnValues get_Simple_Text_Output_Protocol_Ptr(void** pOutput)
{
    eReturnValues ret        = SUCCESS;
    EFI_STATUS    uefiStatus = EFI_SUCCESS;
    EFI_HANDLE*   handle     = M_NULLPTR;
    EFI_GUID      outputGuid = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
    UINTN         nodeCount  = 0;

    if (gBS == M_NULLPTR) // make sure global boot services pointer is valid before
                          // accessing it.
    {
        return MEMORY_FAILURE;
    }

    uefiStatus = gBS->LocateHandleBuffer(ByProtocol, &outputGuid, M_NULLPTR, &nodeCount, &handle);
    if (EFI_ERROR(uefiStatus))
    {
        return FAILURE;
    }
    // NOTE: This code below assumes that we only care to change color output on
    // node 0. This seems to work from a quick test, but may not be correct. Not
    // sure what the other 2 nodes are for...serial?
    uefiStatus = gBS->OpenProtocol(handle[0], &outputGuid, pOutput, gImageHandle, M_NULLPTR,
                                   EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    // TODO: based on the error code, rather than assuming failure, check for
    // supported/not supported.
    if (EFI_ERROR(uefiStatus))
    {
        ret = FAILURE;
    }
    return ret;
}

void close_Simple_Text_Output_Protocol_Ptr(void** pOutput)
{
    EFI_STATUS  uefiStatus = EFI_SUCCESS;
    EFI_HANDLE* handle     = M_NULLPTR;
    EFI_GUID    outputGuid = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
    UINTN       nodeCount  = 0;

    if (gBS == M_NULLPTR) // make sure global boot services pointer is valid before
                          // accessing it.
    {
        return;
    }

    uefiStatus = gBS->LocateHandleBuffer(ByProtocol, &outputGuid, M_NULLPTR, &nodeCount, &handle);
    if (EFI_ERROR(uefiStatus))
    {
        return;
    }
    // NOTE: This code below assumes that we only care to change color output on
    // node 0. This seems to work from a quick test, but may not be correct. Not
    // sure what the other 2 nodes are for...serial?
    uefiStatus = gBS->CloseProtocol(handle[0], &outputGuid, gImageHandle, M_NULLPTR);
    // TODO: based on the error code, rather than assuming failure, check for
    // supported/not supported.
    if (EFI_ERROR(uefiStatus))
    {
        perror("Failed to close simple text output protocol\n");
    }
}

static int32_t get_Default_Console_Colors()
{
    static int32_t defaultAttributes = INT32_MAX;
    if (defaultAttributes == INT32_MAX)
    {
        EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* outputProtocol;
        if (SUCCESS == get_Simple_Text_Output_Protocol_Ptr((void**)&outputProtocol))
        {
            defaultAttributes = outputProtocol->Mode->Attribute;
            // printf("Default text output attributes are set to %" PRIX32 "\n",
            // defaultAttributes);
            close_Simple_Text_Output_Protocol_Ptr((void**)&outputProtocol);
        }
    }
    return defaultAttributes;
}

// Comment from simpletextout.h describes acceptable colors....we will try
// setting whatever we can, but it may not actually work!
//  For Foreground, and EFI_* value is valid from EFI_BLACK(0x00) to
//  EFI_WHITE (0x0F).
//  For Background, only EFI_BLACK, EFI_BLUE, EFI_GREEN, EFI_CYAN,
//  EFI_RED, EFI_MAGENTA, EFI_BROWN, and EFI_LIGHTGRAY are acceptable
void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    static bool                      defaultsSet = false;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* outputProtocol;
    if (!defaultsSet)
    {
        // First time we are setting colors backup the default settings so they
        // can be restored properly later.
        get_Default_Console_Colors();
        defaultsSet = true;
    }
    if (SUCCESS == get_Simple_Text_Output_Protocol_Ptr((void**)&outputProtocol))
    {
        if (foregroundBackground) // change foreground color
        {
            // save current background color
            uint8_t currentBackground = M_Nibble1(C_CAST(unsigned long long, outputProtocol->Mode->Attribute));
            switch (consoleColor)
            {
            case DARK_BLUE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_BLUE, currentBackground));
                break;
            case CONSOLE_COLOR_BLUE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTBLUE, currentBackground));
                break;
            case DARK_GREEN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_GREEN, currentBackground));
                break;
            case CONSOLE_COLOR_GREEN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTGREEN, currentBackground));
                break;
            case DARK_RED:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_RED, currentBackground));
                break;
            case CONSOLE_COLOR_RED:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTRED, currentBackground));
                break;
            case BLACK:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_BLACK, currentBackground));
                break;
            case BROWN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_BROWN, currentBackground));
                break;
            case YELLOW:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_YELLOW, currentBackground));
                break;
            case TEAL:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_CYAN, currentBackground));
                break;
            case CYAN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTCYAN, currentBackground));
                break;
            case PURPLE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_MAGENTA, currentBackground));
                break;
            case MAGENTA:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTMAGENTA, currentBackground));
                break;
            case WHITE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_WHITE, currentBackground));
                break;
            case DARK_GRAY:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_DARKGRAY, currentBackground));
                break;
            case CONSOLE_COLOR_GRAY:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTGRAY, currentBackground));
                break;
            case CONSOLE_COLOR_DEFAULT:
            default:
                outputProtocol->SetAttribute(
                    outputProtocol, EFI_TEXT_ATTR(M_Nibble0(C_CAST(unsigned long long, get_Default_Console_Colors())),
                                                  currentBackground));
                break;
            }
        }
        else // change background color
        {
            uint8_t currentForeground = M_Nibble0(C_CAST(unsigned long long, outputProtocol->Mode->Attribute));
            switch (consoleColor)
            {
            case DARK_BLUE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_BLUE));
                break;
            case CONSOLE_COLOR_BLUE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTBLUE));
                break;
            case DARK_GREEN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_GREEN));
                break;
            case CONSOLE_COLOR_GREEN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTGREEN));
                break;
            case DARK_RED:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_RED));
                break;
            case CONSOLE_COLOR_RED:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTRED));
                break;
            case BLACK:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_BLACK));
                break;
            case BROWN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_BROWN));
                break;
            case YELLOW:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_YELLOW));
                break;
            case TEAL:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_CYAN));
                break;
            case CYAN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTCYAN));
                break;
            case PURPLE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_MAGENTA));
                break;
            case MAGENTA:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTMAGENTA));
                break;
            case WHITE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_WHITE));
                break;
            case DARK_GRAY:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_DARKGRAY));
                break;
            case CONSOLE_COLOR_GRAY:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTGRAY));
                break;
            case CONSOLE_COLOR_DEFAULT:
            default:
                outputProtocol->SetAttribute(
                    outputProtocol, EFI_TEXT_ATTR(currentForeground,
                                                  M_Nibble1(C_CAST(unsigned long long, get_Default_Console_Colors()))));
                break;
            }
        }
        // close the protocol since we are done for now.
        close_Simple_Text_Output_Protocol_Ptr((void**)&outputProtocol);
    }
}
#elif defined(_WIN32)
static uint16_t get_Console_Default_Color(void)
{
    static uint16_t defaultConsoleAttributes = UINT16_MAX;
    if (defaultConsoleAttributes == UINT16_MAX)
    {
        CONSOLE_SCREEN_BUFFER_INFO defaultInfo;
        safe_memset(&defaultInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFO), 0, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &defaultInfo))
        {
            defaultConsoleAttributes = defaultInfo.wAttributes;
        }
        else
        {
            // set defaultColorVal to zero
            defaultConsoleAttributes = 0;
        }
    }
    return defaultConsoleAttributes;
}

static uint16_t get_Console_Current_Color(void)
{
    uint16_t                   currentConsoleAttributes = UINT16_MAX;
    CONSOLE_SCREEN_BUFFER_INFO currentInfo;
    safe_memset(&currentInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFO), 0, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &currentInfo))
    {
        currentConsoleAttributes = currentInfo.wAttributes;
    }
    else
    {
        // set defaultColorVal to zero
        currentConsoleAttributes = 0;
    }
    return currentConsoleAttributes;
}

void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    if (foregroundBackground)
    {
        set_Console_Foreground_Background_Colors(consoleColor, CONSOLE_COLOR_CURRENT);
    }
    else
    {
        set_Console_Foreground_Background_Colors(CONSOLE_COLOR_CURRENT, consoleColor);
    }
}

void set_Console_Foreground_Background_Colors(eConsoleColors foregroundColor, eConsoleColors backgroundColor)
{
    static bool defaultsSet       = false;
    static WORD defaultColorValue = WORD_C(0);
    HANDLE      consoleHandle     = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD        theColor          = WORD_C(0);
    if (!defaultsSet)
    {
        // First time we are setting colors backup the default settings so they
        // can be restored properly later.
        defaultColorValue = get_Console_Default_Color();
        defaultsSet       = true;
    }
    theColor = get_Console_Current_Color(); // get current colors after defaults
                                            // are setup.
    // now change what is requested
    if (foregroundColor != CONSOLE_COLOR_CURRENT)
    {
        // clear out foreground bits, then set the requested color
        theColor &= 0xFFF0; // foreground are lowest 4 bits
        switch (foregroundColor)
        {
        case CONSOLE_COLOR_BLUE:
            theColor |= FOREGROUND_BLUE;
            break;
        case CONSOLE_COLOR_BRIGHT_BLUE:
            theColor |= FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_GREEN:
            theColor |= FOREGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_GREEN:
            theColor |= FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_RED:
            theColor |= FOREGROUND_RED;
            break;
        case CONSOLE_COLOR_BRIGHT_RED:
            theColor |= FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_BLACK:
            theColor |= 0; // this should mean no colors or black
            break;
        case CONSOLE_COLOR_YELLOW:
            theColor |= FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_YELLOW:
            theColor |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_CYAN:
            theColor |= FOREGROUND_BLUE | FOREGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_CYAN:
            theColor |= FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_MAGENTA:
            theColor |= FOREGROUND_BLUE | FOREGROUND_RED;
            break;
        case CONSOLE_COLOR_BRIGHT_MAGENTA:
            theColor |= FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_BRIGHT_WHITE:
            theColor |= FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_GRAY:
            theColor |= FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_WHITE:
            theColor |= FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
            break;
        case CONSOLE_COLOR_DEFAULT: // fall through to default
        default:
            theColor |= (defaultColorValue & 0x000F);
            break;
        }
    }
    if (backgroundColor != CONSOLE_COLOR_CURRENT)
    {
        // clear out background bits, then set the requested color
        theColor &= 0xFF0F; // foreground are middle 4 bits
        switch (backgroundColor)
        {
        case CONSOLE_COLOR_BLUE:
            theColor |= BACKGROUND_BLUE;
            break;
        case CONSOLE_COLOR_BRIGHT_BLUE:
            theColor |= BACKGROUND_BLUE | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_GREEN:
            theColor |= BACKGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_GREEN:
            theColor |= BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_RED:
            theColor |= BACKGROUND_RED;
            break;
        case CONSOLE_COLOR_BRIGHT_RED:
            theColor |= BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_BLACK:
            theColor |= 0; // this should mean no colors or black
            break;
        case CONSOLE_COLOR_YELLOW:
            theColor |= BACKGROUND_RED | BACKGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_YELLOW:
            theColor |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_CYAN:
            theColor |= BACKGROUND_BLUE | BACKGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_CYAN:
            theColor |= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_MAGENTA:
            theColor |= BACKGROUND_BLUE | BACKGROUND_RED;
            break;
        case CONSOLE_COLOR_BRIGHT_MAGENTA:
            theColor |= BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_BRIGHT_WHITE:
            theColor |= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_GRAY:
            theColor |= BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_WHITE:
            theColor |= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
            break;
        case CONSOLE_COLOR_DEFAULT: // fall through to default
        default:
            theColor |= (defaultColorValue & 0x00F0);
            break;
        }
    }
    SetConsoleTextAttribute(consoleHandle, theColor);
}

static DWORD get_Input_Console_Default_Mode(void)
{
    static DWORD defaultConsoleMode = UINT32_MAX;
    if (defaultConsoleMode == UINT32_MAX)
    {
        if (FALSE == GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &defaultConsoleMode))
        {
            defaultConsoleMode = 0;
            // From MSFT documentation:
            // https://learn.microsoft.com/en-us/windows/console/setconsolemode?redirectedfrom=MSDN
            // "When a console is created, all input modes except
            // ENABLE_WINDOW_INPUT and ENABLE_VIRTUAL_TERMINAL_INPUT are enabled
            // by default."
            defaultConsoleMode = ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE | ENABLE_LINE_INPUT | ENABLE_MOUSE_INPUT |
                                 ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE;
        }
    }
    return defaultConsoleMode;
}

static bool set_Input_Console_Mode(DWORD mode)
{
    return M_ToBool(SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode));
}

eReturnValues get_Secure_User_Input(const char* prompt, char** userInput, size_t* inputDataLen)
{
    eReturnValues ret            = SUCCESS;
    DWORD         defaultConMode = get_Input_Console_Default_Mode();
    DWORD         conMode        = defaultConMode;
    conMode &= C_CAST(DWORD, ~(ENABLE_ECHO_INPUT));
    printf("%s", prompt);
    flush_stdout();
    // disable echoing typed characters so that
    if (set_Input_Console_Mode(conMode))
    {
        if (getline(userInput, inputDataLen, stdin) <= 0)
        {
            ret = FAILURE;
        }
        else
        {
            // remove newline from the end...convert to a null.
            size_t endofinput = safe_strlen(*userInput);
            if ((*userInput)[endofinput - 1] == '\n')
            {
                (*userInput)[endofinput - 1] = '\0';
            }
        }
    }
    else
    {
        ret = FAILURE;
    }
    set_Input_Console_Mode(defaultConMode);
    printf("\n");
    return ret;
}

#else
#    include "secured_env_vars.h"
#    if IS_FREEBSD_VERSION(4, 6, 0) || (defined(__OpenBSD__) && defined OpenBSD2_9)
#        include <readpassphrase.h>
#    endif // FreeBSD 4.6+ or OpenBSD 2.9+
#    include <termios.h>
void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    // use the new behavior
    if (foregroundBackground)
    {
        set_Console_Foreground_Background_Colors(consoleColor, CONSOLE_COLOR_CURRENT);
    }
    else
    {
        set_Console_Foreground_Background_Colors(CONSOLE_COLOR_CURRENT, consoleColor);
    }
}

typedef enum eKnownTERMEnum
{
    TERM_UNKNOWN,
    TERM_XTERM, // no mention of 256 color output support. Can assume 16 colors
                // for now-TJE
    TERM_XTERM_256COLOR,
    TERM_GENERIC_256COLOR,
    TERM_SUN_COLOR,
    TERM_AIXTERM,
    TERM_GNOME_256COLOR,
    TERM_GNOME_COLOR,
    TERM_TRUECOLOR_256COLOR,
    TERM_GENERIC_COLOR,  // unknown level of support for colors...we just know it
                         // has some color capabilities
    TERM_LINUX_COLOR,    // 16 colors
    TERM_LINUX_256COLOR, // 16 colors
    // Add more terminal types to use for various color output formats
} eKnownTERM;

// other things to potentially look for:
//     "GNOME_TERMINAL_SERVICE"
//     "GNOME_TERMINAL_SCREEN"
//     "TERM_PROGRAM"         <---git bash and msys2 in windows use this and set
//     it to "mintty". TERM is set to xterm "TERM_PROGRAM_VERSION" <---git bash
//     and msys2 in windows use this for mintty version number
// See this for more info: https://github.com/termstandard/colors
static eKnownTERM get_Terminal_Type(void)
{
    eKnownTERM terminalType      = TERM_UNKNOWN;
    char*      termEnv           = M_NULLPTR;
    bool       haveCompleteMatch = false;
    if (get_Environment_Variable("TERM", &termEnv) == ENV_VAR_SUCCESS && termEnv != M_NULLPTR)
    {
        // do exact matches up top, then search for substrings
        if (strcmp(termEnv, "xterm-256color") == 0)
        {
            terminalType      = TERM_XTERM_256COLOR;
            haveCompleteMatch = true;
        }
        else if (strcmp(termEnv, "aixterm") == 0)
        {
            terminalType      = TERM_AIXTERM;
            haveCompleteMatch = true;
        }
        else if (strcmp(termEnv, "sun-color") == 0)
        {
            terminalType      = TERM_SUN_COLOR;
            haveCompleteMatch = true;
        }
        else if (strcmp(termEnv, "xterm") == 0)
        {
            terminalType = TERM_XTERM;
        }
        else if (strcmp(termEnv, "linux") == 0)
        {
            // alpine linux does not set COLORTERM or anything else, so this
            // would be complete there, but let the other checks run too linux
            // kernel 3.16 and earlier do not support "truecolor" and will be
            // more limited, but this may be ok since we are not using rgb
            // format. Only 16 different colors-TJE
            OSVersionNumber linVer;
            safe_memset(&linVer, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
            if (SUCCESS == get_Operating_System_Version_And_Name(&linVer, M_NULLPTR))
            {
                if (linVer.versionType.linuxVersion.majorVersion >= 4 ||
                    (linVer.versionType.linuxVersion.majorVersion >= 3 &&
                     linVer.versionType.linuxVersion.minorVersion > 16))
                {
                    terminalType      = TERM_LINUX_256COLOR;
                    haveCompleteMatch = true;
                }
                else
                {
                    // limited to 16 colors...which is fine
                    terminalType      = TERM_LINUX_COLOR;
                    haveCompleteMatch = true;
                }
            }
            else
            {
                // assuming this as it seems to still support 16 colors, enough
                // for us today
                terminalType = TERM_LINUX_COLOR;
            }
        }
        else if (strstr(termEnv, "256color"))
        {
            terminalType = TERM_GENERIC_256COLOR;
        }
        else if (strstr(termEnv, "color"))
        {
            terminalType = TERM_GENERIC_COLOR;
        }
    }
    if (!haveCompleteMatch)
    {
        if (get_Environment_Variable("COLORTERM", &termEnv) == ENV_VAR_SUCCESS && termEnv != M_NULLPTR)
        {
            // check what this is set to.
            // truecolor seems to mean supports 256 colors
            if (strcmp(termEnv, "gnome-terminal") == 0)
            {
                // centos6 reports "gnome-terminal" which seems to support 256
                // colors output mode
                if (terminalType == TERM_XTERM)
                {
                    terminalType      = TERM_GNOME_256COLOR;
                    haveCompleteMatch = true;
                }
                else
                {
                    terminalType      = TERM_GNOME_COLOR;
                    haveCompleteMatch = true;
                }
            }
            else if (strstr(termEnv, "truecolor") || strncmp(termEnv, "vte", 3) == 0)
            {
                terminalType      = TERM_TRUECOLOR_256COLOR;
                haveCompleteMatch = true;
            }
            else // this case is generic and a "last" effort in this part. If
                 // "COLORTERM" is set, assume it has some color output
                 // support
            {
                terminalType = TERM_GENERIC_COLOR;
            }
        }
    }
    if (!haveCompleteMatch)
    {
        if (get_Environment_Variable("COLORFGBR", &termEnv) == ENV_VAR_SUCCESS && termEnv != M_NULLPTR)
        {
            // this environment variable is found in kde for forground and
            // background terminal colors This is probably good enough to say
            // "it has some color" support at this point, but no further
            // indication of what color support is available
            terminalType = TERM_GENERIC_COLOR;
        }
    }
    safe_free(&termEnv);
    return terminalType;
}

typedef struct sconsoleColorCap
{
    bool colorSupport;
    bool eightColorsOnly;
    bool eightBackgroundColorsOnly;
    bool use256ColorFormat;
    bool useIntensityBitFormat;
    bool useInvertFormatForBackgroundColors;
    // bool useColonInsteadOfSemiColon; //TODO: Implement support for the very
    // few consoles that need a colon instead of semicolon
} consoleColorCap, *ptrConsoleColorCap;

// Future var we might need is whether the reset to defaults (39m & 49m) work or
// if the complete reset is needed (0m)
static void get_Console_Color_Capabilities(ptrConsoleColorCap colorCapabilities)
{
    if (colorCapabilities != M_NULLPTR)
    {
        eKnownTERM term = get_Terminal_Type();
        safe_memset(colorCapabilities, sizeof(consoleColorCap), 0, sizeof(consoleColorCap));
        switch (term)
        {
        case TERM_LINUX_256COLOR:
            colorCapabilities->useIntensityBitFormat     = true;
            colorCapabilities->eightBackgroundColorsOnly = true;
            M_FALLTHROUGH;
        case TERM_XTERM_256COLOR:
        case TERM_GENERIC_256COLOR:
        case TERM_SUN_COLOR:
        case TERM_GNOME_256COLOR:
        case TERM_TRUECOLOR_256COLOR:
            colorCapabilities->colorSupport      = true;
            colorCapabilities->use256ColorFormat = true;
            break;
        case TERM_LINUX_COLOR:
            colorCapabilities->useIntensityBitFormat     = true;
            colorCapabilities->eightBackgroundColorsOnly = true;
            M_FALLTHROUGH;
        case TERM_XTERM:
        case TERM_AIXTERM:
        case TERM_GNOME_COLOR:
        case TERM_GENERIC_COLOR:
            colorCapabilities->colorSupport = true;
            // these should support 16 colors
            break;
        case TERM_UNKNOWN:
            // Assuming color is supported for now until we run into a terminal
            // that does not support any color code changes When this is found
            // we need it in a different case to return this variable
            colorCapabilities->colorSupport = true;
            // if a terminal does not have any way of doing bright color output,
            // set the 8colorsOnly to true. Ex: red/bright red will be the same
            // in these cases
            colorCapabilities->useIntensityBitFormat = true;
            break;
        }
    }
}

void set_Console_Foreground_Background_Colors(eConsoleColors foregroundColor, eConsoleColors backgroundColor)
{
    static consoleColorCap consoleCap;
    static bool            haveTermCapabilities = false;
    if (!haveTermCapabilities)
    {
        get_Console_Color_Capabilities(&consoleCap);
        haveTermCapabilities = true;
    }
    if (consoleCap.colorSupport) // if color is not supported, do not do
                                 // anything as the escape sequences will just
                                 // print to the output and make it a mess
    {
        if (foregroundColor == CONSOLE_COLOR_CURRENT && backgroundColor == CONSOLE_COLOR_CURRENT)
        {
            // nothing to do
            return;
        }
        if (foregroundColor == CONSOLE_COLOR_DEFAULT && backgroundColor == CONSOLE_COLOR_DEFAULT)
        {
            // reset or normal
            printf("\033[0m");
        }
        else
        {
            // written according to
            // https://en.wikipedia.org/wiki/ANSI_escape_code
            //   There are possible needs to adjust color output if this does
            //   not work for an OS we are supporting. Can ifdef or check for
            //   other things
            // set the string for each color that needs to be set.
            uint8_t foregroundColorInt = UINT8_MAX;
            uint8_t backgroundColorInt = UINT8_MAX;
            // Checking for env variable COLORTERM is one method, or COLORFGBG,
            // or if TERM is set to sun-color, xterm-256color, true-color, or
            // gnome-terminal will work for 256color when debugging a unix-like
            // system, try printenv | grep "color" and printenv | grep "COLOR"
            // to see what you can find out about the terminal testing on CentOS
            // 6 through latest Ubuntu the bright colors seem to be supported
            // (codes 90-97 and 100-107) as well as 256color codes In CentOS 6 &
            // 7 the "intensity" field does change to bright colors, but in
            // Ubuntu 22.04 this only changes to a bold font If the extended
            // colors are not available, will need to use the "intensity" method
            // of \033[1:##m to set it instead Sometimes the "intensity" just
            // makes things bold though FreeBSD 11 through 13 claim
            // xterm-256color so they are also likely supported by 256color and
            // extra bright color codes. omniOS r151038 seems to support 256
            // color codes properly too (sun-color terminal) aixterm also
            // appears to support some color:
            // https://www.ibm.com/docs/en/aix/7.2?topic=aixterm-command, but
            // 256 color is not listed reading terminfo or termcap may be the
            // best way to change formats or ignore color changes if not capable
            // at all, but that is much more complicated to
            //   implement in here right now.
            // http://jdebp.uk./Softwares/nosh/guide/commands/TerminalCapabilities.xml
            switch (backgroundColor)
            {
            case CONSOLE_COLOR_CURRENT:
                break;
            case CONSOLE_COLOR_BLACK:
                backgroundColorInt = 40;
                break;
            case CONSOLE_COLOR_RED:
                backgroundColorInt = 41;
                break;
            case CONSOLE_COLOR_GREEN:
                backgroundColorInt = 42;
                break;
            case CONSOLE_COLOR_YELLOW:
                backgroundColorInt = 43;
                break;
            case CONSOLE_COLOR_BLUE:
                backgroundColorInt = 44;
                break;
            case CONSOLE_COLOR_MAGENTA:
                backgroundColorInt = 45;
                break;
            case CONSOLE_COLOR_CYAN:
                backgroundColorInt = 46;
                break;
            case CONSOLE_COLOR_WHITE:
                backgroundColorInt = 47;
                break;
            case CONSOLE_COLOR_GRAY:
                backgroundColorInt = 100;
                break;
            case CONSOLE_COLOR_BRIGHT_RED:
                backgroundColorInt = 101;
                break;
            case CONSOLE_COLOR_BRIGHT_GREEN:
                backgroundColorInt = 102;
                break;
            case CONSOLE_COLOR_BRIGHT_YELLOW:
                backgroundColorInt = 103;
                break;
            case CONSOLE_COLOR_BRIGHT_BLUE:
                backgroundColorInt = 104;
                break;
            case CONSOLE_COLOR_BRIGHT_MAGENTA:
                backgroundColorInt = 105;
                break;
            case CONSOLE_COLOR_BRIGHT_CYAN:
                backgroundColorInt = 106;
                break;
            case CONSOLE_COLOR_BRIGHT_WHITE:
                backgroundColorInt = 107;
                break;
            case CONSOLE_COLOR_DEFAULT:
            default:
                // aixterm does not list this, so will need to test it!
                // otherwise reset with 0m will be as close as we get
                backgroundColorInt = 49;
                break;
            }
            // if background colors do not work, may need to try the "invert"
            // trick to make it happen using a format like \033[7;nm or
            // \033[7;1;nm
            if (backgroundColorInt != UINT8_MAX)
            {
                uint8_t back256Color = UINT8_MAX;
                if (backgroundColorInt < 100)
                {
                    back256Color = backgroundColorInt - 40; // 256 colors start at 0
                }
                else
                {
                    back256Color = backgroundColorInt - 100 + 8; // 256 bright colors start at 8
                }
                if (backgroundColorInt == 49 || !consoleCap.use256ColorFormat ||
                    (consoleCap.use256ColorFormat && consoleCap.eightBackgroundColorsOnly))
                {
                    // use the inversion method with 7;intensity;colorm
                    if (consoleCap.useInvertFormatForBackgroundColors)
                    {
                        // more background colors are available using the
                        // inversion method (maybe) convert the color to a
                        // foreground color first
                        backgroundColorInt -= 10;
                        if (consoleCap.useIntensityBitFormat && backgroundColorInt >= 90)
                        {
                            printf("\033[7;1;%" PRIu8 "m", backgroundColorInt - 60);
                        }
                        else if (consoleCap.eightColorsOnly && backgroundColorInt >= 90)
                        {
                            printf("\033[7;%" PRIu8 "m", backgroundColorInt - 60);
                        }
                        else
                        {
                            if (consoleCap.useIntensityBitFormat)
                            {
                                printf("\033[7;0;%" PRIu8 "m", backgroundColorInt);
                            }
                            else
                            {
                                printf("\033[7;%" PRIu8 "m", backgroundColorInt);
                            }
                        }
                    }
                    else
                    {
                        // print the background request
                        if (consoleCap.useIntensityBitFormat && backgroundColorInt >= 100)
                        {
                            printf("\033[1;%" PRIu8 "m", backgroundColorInt - 60);
                        }
                        else if (consoleCap.eightColorsOnly && backgroundColorInt >= 100)
                        {
                            printf("\033[%" PRIu8 "m", backgroundColorInt - 60);
                        }
                        else
                        {
                            if (consoleCap.useIntensityBitFormat)
                            {
                                printf("\033[0;%" PRIu8 "m", backgroundColorInt);
                            }
                            else
                            {
                                printf("\033[%" PRIu8 "m", backgroundColorInt);
                            }
                        }
                    }
                }
                else
                {
                    printf("\033[48;5;%" PRIu8 "m", back256Color);
                }
            }

            switch (foregroundColor)
            {
            case CONSOLE_COLOR_CURRENT:
                break;
            case CONSOLE_COLOR_BLACK:
                foregroundColorInt = 30;
                break;
            case CONSOLE_COLOR_RED:
                foregroundColorInt = 31;
                break;
            case CONSOLE_COLOR_GREEN:
                foregroundColorInt = 32;
                break;
            case CONSOLE_COLOR_YELLOW:
                foregroundColorInt = 33;
                break;
            case CONSOLE_COLOR_BLUE:
                foregroundColorInt = 34;
                break;
            case CONSOLE_COLOR_MAGENTA:
                foregroundColorInt = 35;
                break;
            case CONSOLE_COLOR_CYAN:
                foregroundColorInt = 36;
                break;
            case CONSOLE_COLOR_WHITE:
                foregroundColorInt = 37;
                break;
            case CONSOLE_COLOR_GRAY:
                foregroundColorInt = 90;
                break;
            case CONSOLE_COLOR_BRIGHT_RED:
                foregroundColorInt = 91;
                break;
            case CONSOLE_COLOR_BRIGHT_GREEN:
                foregroundColorInt = 92;
                break;
            case CONSOLE_COLOR_BRIGHT_YELLOW:
                foregroundColorInt = 93;
                break;
            case CONSOLE_COLOR_BRIGHT_BLUE:
                foregroundColorInt = 94;
                break;
            case CONSOLE_COLOR_BRIGHT_MAGENTA:
                foregroundColorInt = 95;
                break;
            case CONSOLE_COLOR_BRIGHT_CYAN:
                foregroundColorInt = 96;
                break;
            case CONSOLE_COLOR_BRIGHT_WHITE:
                foregroundColorInt = 97;
                break;
            case CONSOLE_COLOR_DEFAULT:
            default:
                // aixterm does not list this, so will need to test it!
                // otherwise reset with 0m will be as close as we get
                foregroundColorInt = 39;
                break;
            }
            if (foregroundColorInt != UINT8_MAX)
            {
                uint8_t fore256Color = UINT8_MAX;
                if (foregroundColorInt < 90)
                {
                    fore256Color = foregroundColorInt - 30; // 256 colors start at 0
                }
                else
                {
                    fore256Color = foregroundColorInt - 90 + 8; // 256 bright colors start at 8
                }
                if (foregroundColorInt == 39 || !consoleCap.use256ColorFormat)
                {
                    // print the foreground request
                    if (consoleCap.useInvertFormatForBackgroundColors)
                    {
                        // more background colors are available using the
                        // inversion method (maybe)
                        if (consoleCap.useIntensityBitFormat && foregroundColorInt >= 90)
                        {
                            printf("\033[27;1;%" PRIu8 "m", foregroundColorInt - 60);
                        }
                        else if (consoleCap.eightColorsOnly && foregroundColorInt >= 90)
                        {
                            printf("\033[27;%" PRIu8 "m", foregroundColorInt - 60);
                        }
                        else
                        {
                            if (consoleCap.useIntensityBitFormat)
                            {
                                printf("\033[27;0;%" PRIu8 "m", foregroundColorInt);
                            }
                            else
                            {
                                printf("\033[27;%" PRIu8 "m", foregroundColorInt);
                            }
                        }
                    }
                    else
                    {
                        if (consoleCap.useIntensityBitFormat && foregroundColorInt >= 90)
                        {
                            printf("\033[1;%" PRIu8 "m", foregroundColorInt - 60);
                        }
                        else if (consoleCap.eightColorsOnly && foregroundColorInt >= 90)
                        {
                            printf("\033[%" PRIu8 "m", foregroundColorInt - 60);
                        }
                        else
                        {
                            if (consoleCap.useIntensityBitFormat)
                            {
                                printf("\033[0;%" PRIu8 "m", foregroundColorInt);
                            }
                            else
                            {
                                printf("\033[%" PRIu8 "m", foregroundColorInt);
                            }
                        }
                    }
                }
                else
                {
                    printf("\033[38;5;%" PRIu8 "m", fore256Color);
                }
            }
        }
    }
}

static M_INLINE void fclose_term(FILE* term)
{
    M_STATIC_CAST(void, fclose(term));
}

// if not POSIX.1-2001, getpass exists, but has issues and is not recommended.
// https://www.man7.org/linux/man-pages/man3/getpass.3.html getpassphrase exists
// in BSD
// https://man.freebsd.org/cgi/man.cgi?query=readpassphrase&apropos=0&sektion=0&manpath=FreeBSD+14.0-RELEASE+and+Ports&arch=default&format=html
// getpass_r is available in NetBSD 7..same with getpassfd
// getpassphrase is in sunos and accepts up to 256 chars. In sun 5.19 getpass
// says it reads up to 9 characters getpassphrase starts in sun 5.6 Unix 7 says
// 8 characters freebsd, netbsd, older linux, etc usually say getpass can accept
// up to 128 chars From one of the BSD manpages on getpass:
//   Historically getpass accepted and returned a password if	it  could  not
//   modify  the terminal settings to	turn echo off(or if the input was not
//   a terminal).
// So if necessary, for "compatibility" this could be implemented without echo,
// but avoiding that for now-TJE
eReturnValues get_Secure_User_Input(const char* prompt, char** userInput, size_t* inputDataLen)
{
    eReturnValues ret = SUCCESS;
#    if defined(POSIX_2001) && defined(_POSIX_JOB_CONTROL) // https://linux.die.net/man/7/posixoptions
    struct termios defaultterm;
    struct termios currentterm;
    FILE*          term   = M_NULLPTR;
    bool           devtty = true;
    safe_memset(&defaultterm, sizeof(struct termios), 0, sizeof(struct termios));
    safe_memset(&currentterm, sizeof(struct termios), 0, sizeof(struct termios));
    errno_t openresult = safe_fopen(&term, "/dev/tty", "r"); // use /dev/tty instead of stdin to get the
                                                             // terminal controlling the process.
    if (openresult != 0 || !term)
    {
        term   = stdin; // fallback to stdin I guess...
        devtty = false;
    }
    // get the default terminal flags
    if (tcgetattr(fileno(term), &defaultterm))
    {
        if (devtty)
        {
            fclose_term(term);
        }
        return FAILURE;
    }
    safe_memcpy(&currentterm, sizeof(struct termios), &defaultterm, sizeof(struct termios));
    // print the prompt
    printf("%s", prompt);
    flush_stdout();
    // disable echo for now.
    currentterm.c_lflag &= C_CAST(tcflag_t, ~ECHO);
    if (tcsetattr(fileno(term), TCSAFLUSH, &currentterm) != 0)
    {
        if (devtty)
        {
            fclose_term(term);
        }
        printf("\n");
        return FAILURE;
    }
    // now read the input with getline
    if (getline(userInput, inputDataLen, term) <= 0)
    {
        ret = FAILURE;
    }
    else
    {
        // check if newline was read (it likely will be there) and remove it
        // remove newline from the end...convert to a null.
        size_t endofinput = safe_strlen(*userInput);
        if ((*userInput)[endofinput - 1] == '\n')
        {
            (*userInput)[endofinput - 1] = '\0';
        }
    }
    // restore echo/default flags
    if (tcsetattr(fileno(term), TCSAFLUSH, &defaultterm) != 0)
    {
        if (devtty)
        {
            fclose_term(term);
        }
        printf("\n");
        return FAILURE;
    }
    if (devtty)
    {
        fclose_term(term);
    }
    printf("\n");
#    elif IS_FREEBSD_VERSION(4, 6, 0) || (defined(__OpenBSD__) && defined OpenBSD2_9)
    // use readpassphrase instead
    // use BUFSIZ buffer as that should be more than enough to read this
    // NOTE: Linux's libbsd also provides this, but termios method above is
    // still preferred-TJE
    *inputDataLen = BUFSIZ;
    *userInput    = M_REINTERPRET_CAST(char*, safe_calloc(*inputDataLen, sizeof(char)));
    if (*userInput)
    {
        if (!readpassphrase(prompt, *userInput, *inputDataLen, 0))
        {
            ret = FAILURE;
        }
    }
    else
    {
        ret = MEMORY_FAILURE;
    }
#    elif IS_NETBSD_VERSION(7, 0, 0)
    // Use getpass_r
    // this will dynamically allocate memory for use when the buffer is set to
    // M_NULLPTR
    *userInput = getpass_r(prompt, M_NULLPTR, 0);
    if ((*userInput) == M_NULLPTR)
    {
        ret = FAILURE;
    }
    else
    {
        *inputDataLen = safe_strlen(*userInput) + 1; // add one since this adds to the buffer size and that is what we
                                                     // are returning in all other cases-TJE
    }
#    elif defined(__sun) && defined(__SunOS_5_6)
    // use getpassphrase since it can return longer passwords than getpass
    char* eraseme = getpassphrase(prompt);
    if (eraseme != M_NULLPTR)
    {
        errno_t erasedup = safe_strdup(userInput, eraseme);
        // immediately clear the original buffer to make sure it cannot be
        // accessed again
        explicit_zeroes(eraseme, safe_strlen(eraseme));
        if (erasedup != 0 || *userInput == M_NULLPTR)
        {
            ret = FAILURE;
        }
        else
        {
            *inputDataLen =
                safe_strlen(*userInput) + SIZE_T_C(1); // add one since this adds to the buffer size and that is
                                                       // what we are returning in all other cases-TJE
        }
    }
    else
    {
        ret = FAILURE;
    }
#    else  // POSIX & OS MACRO CHECKS
    // Last resort is to use getpass. This is the least desirable thing to use
    // which is why it only gets used in this else case as a backup in case the
    // OS we are supporting doesn't support any other available method.-TJE
    char* eraseme = getpass(prompt);
    if (eraseme != M_NULLPTR)
    {
        errno_t erasedup = safe_strdup(userInput, eraseme);
        // immediately clear the original buffer to make sure it cannot be
        // accessed again
        explicit_zeroes(eraseme, safe_strlen(eraseme));
        if (erasedup != 0 || *userInput == M_NULLPTR)
        {
            ret = FAILURE;
        }
        else
        {
            *inputDataLen = safe_strlen(*userInput) + 1; // add one since this adds to the buffer size and that is
                                                         // what we are returning in all other cases-TJE
        }
    }
    else
    {
        ret = FAILURE;
    }
#    endif // POSIX & OS MACRO CHECKS
    return ret;
}
#endif

static M_INLINE bool is_Allowed_Datasize_Unit(const char* unit)
{
    bool allowed = false;
    // allowed units must match exactly at the end of the string!
    if (strcmp(unit, "B") == 0 || strcmp(unit, "KB") == 0 || strcmp(unit, "KiB") == 0 || strcmp(unit, "MB") == 0 ||
        strcmp(unit, "MiB") == 0 || strcmp(unit, "GB") == 0 || strcmp(unit, "GiB") == 0 || strcmp(unit, "TB") == 0 ||
        strcmp(unit, "TiB") == 0 || strcmp(unit, "BLOCKS") == 0 || strcmp(unit, "SECTORS") == 0)
    {
        allowed = true;
    }
    return allowed;
}

static M_INLINE bool is_Allowed_Sector_Size_Unit(const char* unit)
{
    bool allowed = false;
    if (strcmp(unit, "l") == 0 // used by some utilities to indicate a count is in
                               // logical sectors instead of physical sectors
        || strcmp(unit, "p") == 0 || strcmp(unit, "logical") == 0 || strcmp(unit, "physical") == 0)
    {
        allowed = true;
    }
    return allowed;
}

static M_INLINE bool is_Allowed_Time_Unit(const char* unit)
{
    bool allowed = false;
    if (strcmp(unit, "ns") == 0    // nanoseconds
        || strcmp(unit, "us") == 0 // microseconds
        || strcmp(unit, "ms") == 0 // milliseconds
        || strcmp(unit, "s") == 0  // seconds
        || strcmp(unit, "m") == 0  // minutes
        || strcmp(unit, "h") == 0  // hours
    )
    {
        allowed = true;
    }
    return allowed;
}

static M_INLINE bool is_Allowed_Power_Unit(const char* unit)
{
    bool allowed = false;
    if (strcmp(unit, "w") == 0     // watts
        || strcmp(unit, "mw") == 0 // milliwatts
    )
    {
        allowed = true;
    }
    return allowed;
}

static M_INLINE bool is_Allowed_Volts_Unit(const char* unit)
{
    bool allowed = false;
    if (strcmp(unit, "v") == 0     // volts
        || strcmp(unit, "mv") == 0 // millivolts
    )
    {
        allowed = true;
    }
    return allowed;
}

static M_INLINE bool is_Allowed_Amps_Unit(const char* unit)
{
    bool allowed = false;
    if (strcmp(unit, "a") == 0     // amps
        || strcmp(unit, "ma") == 0 // milliamps
    )
    {
        allowed = true;
    }
    return allowed;
}

static M_INLINE bool is_Allowed_Temperature_Unit(const char* unit)
{
    bool allowed = false;
    if (strcmp(unit, "c") == 0    // celsius
        || strcmp(unit, "f") == 0 // fahrenheit
        || strcmp(unit, "k") == 0 // kelvin
    )
    {
        allowed = true;
    }
    return allowed;
}

// This will only return true when this is at the end of a
// string meaning the user provided 64KB or something like that,
// so this matched to KB This allows for the utilities calling
// this to multiply the output integer into a value that makes
// sense
static bool is_Allowed_Unit_For_Get_And_Validate_Input(const char* unit, eAllowedUnitInput unittype)
{
    bool allowed = false;
    if (unit != M_NULLPTR)
    {
        switch (unittype)
        {
        case ALLOW_UNIT_NONE:
            if (strcmp(unit, "") == 0)
            {
                allowed = true;
            }
            else
            {
                allowed = false;
            }
            break;
        case ALLOW_UNIT_DATASIZE:
            allowed = is_Allowed_Datasize_Unit(unit);
            break;
        case ALLOW_UNIT_SECTOR_TYPE:
            allowed = is_Allowed_Sector_Size_Unit(unit);
            break;
        case ALLOW_UNIT_TIME:
            allowed = is_Allowed_Time_Unit(unit);
            break;
        case ALLOW_UNIT_POWER:
            allowed = is_Allowed_Power_Unit(unit);
            break;
        case ALLOW_UNIT_VOLTS:
            allowed = is_Allowed_Volts_Unit(unit);
            break;
        case ALLOW_UNIT_AMPS:
            allowed = is_Allowed_Amps_Unit(unit);
            break;
        case ALLOW_UNIT_TEMPERATURE:
            allowed = is_Allowed_Temperature_Unit(unit);
            break;
        }
    }
    else if (unittype == ALLOW_UNIT_NONE && unit == M_NULLPTR)
    {
        allowed = true;
    }
    return allowed;
}

typedef enum integerInputStrTypeEnum
{
    INT_INPUT_INVALID,
    INT_INPUT_DECIMAL = 10,
    INT_INPUT_HEX     = 16
} eintergetInputStrType;

static M_INLINE eintergetInputStrType get_Input_Str_Type(const char* str)
{
    eintergetInputStrType type = INT_INPUT_DECIMAL;
    if (str != M_NULLPTR)
    {
        const char* temp = str;
        while (*temp != '\0')
        {
            if ((!safe_isxdigit(*temp)) && (*temp != 'x') && (*temp != 'h'))
            {
                type = INT_INPUT_INVALID;
                break;
            }
            else if (!safe_isdigit(*temp))
            {
                type = INT_INPUT_HEX;
            }
            ++temp;
        }
    }
    else
    {
        type = INT_INPUT_INVALID;
    }
    return type;
}

M_NODISCARD bool get_And_Validate_Integer_Input_ULL(const char*         strToConvert,
                                                    char**              unit,
                                                    eAllowedUnitInput   unittype,
                                                    unsigned long long* outputInteger)
{
    bool result = false;
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputInteger != M_NULLPTR)
    {
        eintergetInputStrType strType = get_Input_Str_Type(strToConvert);
        // If everything is a valid hex digit.
        if (strType != INT_INPUT_INVALID)
        {
            if (0 != safe_strtoull(outputInteger, strToConvert, unit, strType))
            {
                result = false;
            }
            else if (unit && !is_Allowed_Unit_For_Get_And_Validate_Input(*unit, unittype))
            {
                result = false;
            }
            else if (unit == M_NULLPTR && unittype != ALLOW_UNIT_NONE)
            {
                result = false;
            }
            else
            {
                result = true;
            }
        }
        else
        {
            result = false;
        }
    }
    RESTORE_NONNULL_COMPARE
    return result;
}

M_NODISCARD bool get_And_Validate_Integer_Input_UL(const char*       strToConvert,
                                                   char**            unit,
                                                   eAllowedUnitInput unittype,
                                                   unsigned long*    outputInteger)
{
    bool result = false;
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputInteger != M_NULLPTR)
    {
        eintergetInputStrType strType = get_Input_Str_Type(strToConvert);
        // If everything is a valid hex digit.
        if (strType != INT_INPUT_INVALID)
        {
            if (0 != safe_strtoul(outputInteger, strToConvert, unit, strType))
            {
                result = false;
            }
            else if (unit && !is_Allowed_Unit_For_Get_And_Validate_Input(*unit, unittype))
            {
                result = false;
            }
            else if (unit == M_NULLPTR && unittype != ALLOW_UNIT_NONE)
            {
                result = false;
            }
            else
            {
                result = true;
            }
        }
        else
        {
            result = false;
        }
    }
    RESTORE_NONNULL_COMPARE
    return result;
}

M_NODISCARD bool get_And_Validate_Integer_Input_UI(const char*       strToConvert,
                                                   char**            unit,
                                                   eAllowedUnitInput unittype,
                                                   unsigned int*     outputInteger)
{
    unsigned long temp = 0UL;
    bool          ret  = get_And_Validate_Integer_Input_UL(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined(UINT_MAX) && defined(ULONG_MAX) && ULONG_MAX > UINT_MAX
        if (temp > UINT_MAX)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = UINT_MAX;
        }
        else
#endif // ULONG_MAX !> UINT_MAX
        {
            *outputInteger = C_CAST(unsigned int, temp);
        }
    }
    return ret;
}

M_NODISCARD bool get_And_Validate_Integer_Input_US(const char*       strToConvert,
                                                   char**            unit,
                                                   eAllowedUnitInput unittype,
                                                   unsigned short*   outputInteger)
{
    unsigned long temp = 0UL;
    bool          ret  = get_And_Validate_Integer_Input_UL(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined(USHORT_MAX) && defined(ULONG_MAX) && ULONG_MAX > USHORT_MAX
        if (temp > USHORT_MAX)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = USHORT_MAX;
        }
        else
#endif // ULONG_MAX !> USHORT_MAX
        {
            *outputInteger = C_CAST(unsigned short, temp);
        }
    }
    return ret;
}

M_NODISCARD bool get_And_Validate_Integer_Input_UC(const char*       strToConvert,
                                                   char**            unit,
                                                   eAllowedUnitInput unittype,
                                                   unsigned char*    outputInteger)
{
    unsigned long temp = 0UL;
    bool          ret  = get_And_Validate_Integer_Input_UL(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined(UCHAR_MAX) && defined(ULONG_MAX) && ULONG_MAX > UCHAR_MAX
        if (temp > UCHAR_MAX)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = UCHAR_MAX;
        }
        else
#endif // ULONG_MAX !> UCHAR_MAX
        {
            *outputInteger = C_CAST(unsigned char, temp);
        }
    }
    return ret;
}

M_NODISCARD bool get_And_Validate_Integer_Input_LL(const char*       strToConvert,
                                                   char**            unit,
                                                   eAllowedUnitInput unittype,
                                                   long long*        outputInteger)
{
    bool result = false;
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputInteger != M_NULLPTR)
    {
        eintergetInputStrType strType = get_Input_Str_Type(strToConvert);
        // If everything is a valid hex digit.
        if (strType != INT_INPUT_INVALID)
        {
            if (0 != safe_strtoll(outputInteger, strToConvert, unit, strType))
            {
                result = false;
            }
            else if (unit && !is_Allowed_Unit_For_Get_And_Validate_Input(*unit, unittype))
            {
                result = false;
            }
            else if (unit == M_NULLPTR && unittype != ALLOW_UNIT_NONE)
            {
                result = false;
            }
            else
            {
                result = true;
            }
        }
        else
        {
            result = false;
        }
    }
    RESTORE_NONNULL_COMPARE
    return result;
}

M_NODISCARD bool get_And_Validate_Integer_Input_L(const char*       strToConvert,
                                                  char**            unit,
                                                  eAllowedUnitInput unittype,
                                                  long*             outputInteger)
{
    bool result = false;
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputInteger != M_NULLPTR)
    {
        eintergetInputStrType strType = get_Input_Str_Type(strToConvert);
        // If everything is a valid hex digit.
        if (strType != INT_INPUT_INVALID)
        {
            if (0 != safe_strtol(outputInteger, strToConvert, unit, strType))
            {
                result = false;
            }
            else if (unit && !is_Allowed_Unit_For_Get_And_Validate_Input(*unit, unittype))
            {
                result = false;
            }
            else if (unit == M_NULLPTR && unittype != ALLOW_UNIT_NONE)
            {
                result = false;
            }
            else
            {
                result = true;
            }
        }
        else
        {
            result = false;
        }
    }
    RESTORE_NONNULL_COMPARE
    return result;
}

M_NODISCARD bool get_And_Validate_Integer_Input_I(const char*       strToConvert,
                                                  char**            unit,
                                                  eAllowedUnitInput unittype,
                                                  int*              outputInteger)
{
    long temp = 0L;
    bool ret  = get_And_Validate_Integer_Input_L(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined(INT_MAX) && defined(LONG_MAX) && defined(INT_MIN) && LONG_MAX > INT_MAX && LONG_MIN < INT_MIN
        if (temp > INT_MAX)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = INT_MAX;
        }
        else if (temp < INT_MIN)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = INT_MIN;
        }
        else
#endif // ULONG_MAX !> INT_MAX
        {
            *outputInteger = C_CAST(short, temp);
        }
    }
    return ret;
}

M_NODISCARD bool get_And_Validate_Integer_Input_S(const char*       strToConvert,
                                                  char**            unit,
                                                  eAllowedUnitInput unittype,
                                                  short*            outputInteger)
{
    long temp = 0L;
    bool ret  = get_And_Validate_Integer_Input_L(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined(SHORT_MAX) && defined(LONG_MAX) && defined(SHORT_MIN) && LONG_MAX > SHORT_MAX && LONG_MIN < SHORT_MIN
        if (temp > SHORT_MAX)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = SHORT_MAX;
        }
        else if (temp < SHORT_MIN)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = SHORT_MIN;
        }
        else
#endif // ULONG_MAX !> SHORT_MAX
        {
            *outputInteger = C_CAST(short, temp);
        }
    }
    return ret;
}

M_NODISCARD bool get_And_Validate_Integer_Input_C(const char*       strToConvert,
                                                  char**            unit,
                                                  eAllowedUnitInput unittype,
                                                  char*             outputInteger)
{
    long temp = 0L;
    bool ret  = get_And_Validate_Integer_Input_L(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined(CHAR_MAX) && defined(LONG_MAX) && defined(CHAR_MIN) && LONG_MAX > CHAR_MAX && LONG_MIN < CHAR_MIN
        if (temp > CHAR_MAX)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = CHAR_MAX;
        }
        else if (temp < CHAR_MIN)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = CHAR_MIN;
        }
        else
#endif // ULONG_MAX !> CHAR_MAX
        {
            *outputInteger = C_CAST(char, temp);
        }
    }
    return ret;
}

M_NODISCARD bool get_And_Validate_Integer_Input_Uint64(const char*       strToConvert,
                                                       char**            unit,
                                                       eAllowedUnitInput unittype,
                                                       uint64_t*         outputInteger)
{
#if defined(USING_C11) && defined(get_Valid_Integer_Input)
    // let the generic selection macro do this
    return get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger);
#elif defined(LP64_DATA_MODEL) || defined(ILP64_DATA_MODEL)
    unsigned long temp = 0UL;
    bool          ret  = get_And_Validate_Integer_Input_UL(strToConvert, unit, unittype, &temp);
    if (ret && temp > UINT64_MAX)
    {
        ret            = false;
        errno          = ERANGE;
        *outputInteger = UINT64_MAX;
    }
    else
    {
        *outputInteger = C_CAST(uint64_t, temp);
    }
    return ret;
#else
    unsigned long long temp = 0ULL;
    bool               ret  = get_And_Validate_Integer_Input_ULL(strToConvert, unit, unittype, &temp);
    if (ret && temp > UINT64_MAX)
    {
        ret            = false;
        errno          = ERANGE;
        *outputInteger = UINT64_MAX;
    }
    else
    {
        *outputInteger = C_CAST(uint64_t, temp);
    }
    return ret;
#endif
}

M_NODISCARD bool get_And_Validate_Float_Input(const char*       strToConvert,
                                              char**            unit,
                                              eAllowedUnitInput unittype,
                                              float*            outputFloat)
{
    bool result = false;
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputFloat != M_NULLPTR)
    {
        if (0 != safe_strtof(outputFloat, strToConvert, unit))
        {
            result = false;
        }
        else if (unit && !is_Allowed_Unit_For_Get_And_Validate_Input(*unit, unittype))
        {
            result = false;
        }
        else if (unit == M_NULLPTR && unittype != ALLOW_UNIT_NONE)
        {
            result = false;
        }
        else
        {
            result = true;
        }
    }
    RESTORE_NONNULL_COMPARE
    return result;
}

M_NODISCARD bool get_And_Validate_Double_Input(const char*       strToConvert,
                                               char**            unit,
                                               eAllowedUnitInput unittype,
                                               double*           outputFloat)
{
    bool result = false;
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputFloat != M_NULLPTR)
    {
        if (0 != safe_strtod(outputFloat, strToConvert, unit))
        {
            result = false;
        }
        else if (unit && !is_Allowed_Unit_For_Get_And_Validate_Input(*unit, unittype))
        {
            result = false;
        }
        else if (unit == M_NULLPTR && unittype != ALLOW_UNIT_NONE)
        {
            result = false;
        }
        else
        {
            result = true;
        }
    }
    RESTORE_NONNULL_COMPARE
    return result;
}

M_NODISCARD bool get_And_Validate_LDouble_Input(const char*       strToConvert,
                                                char**            unit,
                                                eAllowedUnitInput unittype,
                                                long double*      outputFloat)
{
    bool result = false;
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputFloat != M_NULLPTR)
    {
        if (0 != safe_strtold(outputFloat, strToConvert, unit))
        {
            result = false;
        }
        else if (unit && !is_Allowed_Unit_For_Get_And_Validate_Input(*unit, unittype))
        {
            result = false;
        }
        else if (unit == M_NULLPTR && unittype != ALLOW_UNIT_NONE)
        {
            result = false;
        }
        else
        {
            result = true;
        }
    }
    RESTORE_NONNULL_COMPARE
    return result;
}

// NOTE: This function is deprecated as you should use the one that matches your
// integer type instead for best error handling.
M_DEPRECATED bool get_And_Validate_Integer_Input(const char* strToConvert, uint64_t* outputInteger)
{
    return get_And_Validate_Integer_Input_Uint64(strToConvert, M_NULLPTR, ALLOW_UNIT_NONE, outputInteger);
}

M_NODISCARD bool get_And_Validate_Integer_Input_Uint32(const char*       strToConvert,
                                                       char**            unit,
                                                       eAllowedUnitInput unittype,
                                                       uint32_t*         outputInteger)
{
#if defined(USING_C11) && defined(get_Valid_Integer_Input)
    // let the generic selection macro do this
    return get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger);
#else
    unsigned long temp = 0UL;
    bool          ret  = get_And_Validate_Integer_Input_UL(strToConvert, unit, unittype, &temp);
    if (ret && temp > UINT32_MAX)
    {
        ret            = false;
        errno          = ERANGE;
        *outputInteger = UINT32_MAX;
    }
    else
    {
        *outputInteger = C_CAST(uint32_t, temp);
    }
    return ret;
#endif
}

M_NODISCARD bool get_And_Validate_Integer_Input_Uint16(const char*       strToConvert,
                                                       char**            unit,
                                                       eAllowedUnitInput unittype,
                                                       uint16_t*         outputInteger)
{
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputInteger != M_NULLPTR)
    {
        uint32_t temp = UINT32_C(0);
        bool     ret  = get_And_Validate_Integer_Input_Uint32(strToConvert, unit, unittype, &temp);
        if (ret && temp > UINT16_MAX)
        {
            ret   = false;
            errno = ERANGE; // manually set a range error since this is outside
                            // of what is expected for this function
            *outputInteger = UINT16_MAX;
        }
        else if (ret)
        {
            *outputInteger = C_CAST(uint16_t, temp);
        }
        return ret;
    }
    else
    {
        return false;
    }
    RESTORE_NONNULL_COMPARE
}

M_NODISCARD bool get_And_Validate_Integer_Input_Uint8(const char*       strToConvert,
                                                      char**            unit,
                                                      eAllowedUnitInput unittype,
                                                      uint8_t*          outputInteger)
{
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputInteger != M_NULLPTR)
    {
        uint32_t temp = UINT32_C(0);
        bool     ret  = get_And_Validate_Integer_Input_Uint32(strToConvert, unit, unittype, &temp);
        if (ret && temp > UINT8_MAX)
        {
            ret   = false;
            errno = ERANGE; // manually set a range error since this is outside
                            // of what is expected for this function
            *outputInteger = UINT8_MAX;
        }
        else if (ret)
        {
            *outputInteger = C_CAST(uint8_t, temp);
        }
        return ret;
    }
    else
    {
        return false;
    }
    RESTORE_NONNULL_COMPARE
}

M_NODISCARD bool get_And_Validate_Integer_Input_Int64(const char*       strToConvert,
                                                      char**            unit,
                                                      eAllowedUnitInput unittype,
                                                      int64_t*          outputInteger)
{
#if defined(USING_C11) && defined(get_Valid_Integer_Input)
    // let the generic selection macro do this
    return get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger);
#elif defined(LP64_DATA_MODEL) || defined(ILP64_DATA_MODEL)
    long temp = 0L;
    bool ret  = get_And_Validate_Integer_Input_L(strToConvert, unit, unittype, &temp);
    if (ret)
    {
        if (temp > INT64_MAX)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = INT64_MAX;
        }
        else if (temp < INT64_MIN)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = INT64_MIN;
        }
        else
        {
            *outputInteger = C_CAST(int64_t, temp);
        }
    }
    return ret;
#else
    long long temp = 0LL;
    bool      ret  = get_And_Validate_Integer_Input_LL(strToConvert, unit, unittype, &temp);
    if (ret)
    {
        if (temp > INT64_MAX)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = INT64_MAX;
        }
        else if (temp < INT64_MIN)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = INT64_MIN;
        }
        else
        {
            *outputInteger = C_CAST(int64_t, temp);
        }
    }
    return ret;
#endif
}

M_NODISCARD bool get_And_Validate_Integer_Input_Int32(const char*       strToConvert,
                                                      char**            unit,
                                                      eAllowedUnitInput unittype,
                                                      int32_t*          outputInteger)
{
#if defined(USING_C11) && defined(get_Valid_Integer_Input)
    // let the generic selection macro do this
    return get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger);
#else
    long temp = 0L;
    bool ret  = get_And_Validate_Integer_Input_L(strToConvert, unit, unittype, &temp);
    if (ret)
    {
        if (temp > INT32_MAX)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = INT32_MAX;
        }
        else if (temp < INT32_MIN)
        {
            ret            = false;
            errno          = ERANGE;
            *outputInteger = INT32_MIN;
        }
        else
        {
            *outputInteger = C_CAST(int32_t, temp);
        }
    }
    return ret;
#endif
}

M_NODISCARD bool get_And_Validate_Integer_Input_Int16(const char*       strToConvert,
                                                      char**            unit,
                                                      eAllowedUnitInput unittype,
                                                      int16_t*          outputInteger)
{
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputInteger != M_NULLPTR)
    {
        int32_t temp = INT32_C(0);
        bool    ret  = get_And_Validate_Integer_Input_Int32(strToConvert, unit, unittype, &temp);
        if (ret && (temp > INT16_MAX || temp < INT16_MIN))
        {
            ret   = false;
            errno = ERANGE; // manually set a range error since this is outside
                            // of what is expected for this function
        }
        else if (ret)
        {
            *outputInteger = C_CAST(int16_t, temp);
        }
        return ret;
    }
    else
    {
        return false;
    }
    RESTORE_NONNULL_COMPARE
}

M_NODISCARD bool get_And_Validate_Integer_Input_Int8(const char*       strToConvert,
                                                     char**            unit,
                                                     eAllowedUnitInput unittype,
                                                     int8_t*           outputInteger)
{
    DISABLE_NONNULL_COMPARE
    if (strToConvert != M_NULLPTR && outputInteger != M_NULLPTR)
    {
        int32_t temp = INT32_C(0);
        bool    ret  = get_And_Validate_Integer_Input_Int32(strToConvert, unit, unittype, &temp);
        if (ret && (temp > INT8_MAX || temp < INT8_MIN))
        {
            ret   = false;
            errno = ERANGE; // manually set a range error since this is outside
                            // of what is expected for this function
        }
        else if (ret)
        {
            *outputInteger = C_CAST(int8_t, temp);
        }
        return ret;
    }
    else
    {
        return false;
    }
    RESTORE_NONNULL_COMPARE
}

#if !defined(__STDC_ALLOC_LIB__) && !defined(POSIX_2008) && !defined(USING_C23)
// getdelim and getline are not available, so define them ourselves for our own
// use

ssize_t getdelim(char** M_RESTRICT lineptr, size_t* M_RESTRICT n, int delimiter, FILE* stream)
{
    char* currentptr = M_NULLPTR;
    char* endptr     = M_NULLPTR;
    DISABLE_NONNULL_COMPARE
    if (lineptr == M_NULLPTR || n == M_NULLPTR || stream == M_NULLPTR)
    {
        errno = EINVAL;
        return SSIZE_T_C(-1);
    }
    if (*lineptr == M_NULLPTR || *n == SIZE_T_C(0))
    {
        *n       = BUFSIZ;
        *lineptr = M_REINTERPRET_CAST(char*, malloc(*n));
        if (M_NULLPTR == *lineptr)
        {
            errno = ENOMEM;
            return SSIZE_T_C(-1);
        }
    }
    RESTORE_NONNULL_COMPARE
    currentptr = *lineptr;
    endptr     = *lineptr + *n;
    // read using fgetc until delimiter is encountered in the stream or end of
    // the stream is reached
    do
    {
        int c = fgetc(stream);
        if (c == EOF)
        {
            // hit end of the stream.
            if (feof(stream))
            {
                ssize_t numchars = C_CAST(ssize_t, C_CAST(intptr_t, currentptr) - C_CAST(intptr_t, *lineptr));
                if (numchars > SSIZE_T_C(0))
                {
                    // read all the characters in the stream to the end.
                    // set M_NULLPTR terminator and return how many chars were
                    // written
                    currentptr += 1;
                    *currentptr = '\0';
                    return numchars;
                }
            }
            // errno value???
            return SSIZE_T_C(-1);
        }
        // add to the stream
        *currentptr = C_CAST(char, c); // This cast is necessary to stop a warning. C is
                                       // an int so that EOF can be defined.
        currentptr += 1;
        // check if we got the delimiter
        if (c == delimiter)
        {
            *currentptr = '\0';
            // calculate how may characters were read and return that value
            return C_CAST(ssize_t, C_CAST(intptr_t, currentptr) - C_CAST(intptr_t, *lineptr));
        }
        // check if writing next two characters will overflow the buffer (next
        // char + null terminator if needed)
        if (C_CAST(intptr_t, currentptr) + 2 >= C_CAST(intptr_t, endptr))
        {
            // reallocate. Simple method is to double the current buffer size
            char*   temp     = M_NULLPTR;
            size_t  newsize  = *n * SIZE_T_C(2);
            ssize_t numchars = C_CAST(ssize_t, C_CAST(intptr_t, currentptr) - C_CAST(intptr_t, *lineptr));
#    if defined(SSIZE_MAX)
            if (newsize > SSIZE_MAX)
            {
                errno = EOVERFLOW;
                return SSIZE_T_C(-1);
            }
#    endif // SSIZE_MAX
            temp = safe_reallocf(C_CAST(void**, lineptr), newsize);
            if (temp == M_NULLPTR)
            {
                errno = ENOMEM;
                return SSIZE_T_C(-1);
            }
            *lineptr   = temp;
            *n         = newsize;
            currentptr = *lineptr + numchars;
            endptr     = *lineptr + *n;
        }
    } while (1);
}

ssize_t getline(char** lineptr, size_t* n, FILE* stream)
{
    return getdelim(lineptr, n, '\n', stream);
}

#endif //__STDC_ALLOC_LIB__

#if !defined(__STDC_ALLOC_LIB__) && !defined(_GNU_SOURCE) && !IS_FREEBSD_VERSION(2, 2, 0) &&                           \
    !(defined(__OpenBSD__) && defined(OpenBSD2_3)) && !defined(HAVE_VASPRINTF)

M_NODISCARD FUNC_ATTR_PRINTF(2, 3) int asprintf(char** M_RESTRICT strp, const char* M_RESTRICT fmt, ...)
{
    // call vasprintf
    va_list args;
    va_start(args, fmt);
    int result = vasprintf(strp, fmt, args);
    va_end(args);
    return result;
}

M_NODISCARD FUNC_ATTR_PRINTF(2, 0) int vasprintf(char** M_RESTRICT strp, const char* M_RESTRICT fmt, va_list arg)
{
    va_list copyarg;
#    if defined(va_copy)
    va_copy(copyarg, arg);
#    elif defined(__va_copy)
    __va_copy(copyarg, arg);
#    else
    copyarg = arg;
#    endif
//_vscprintf existed in Windows to get format string len before vsnprintf
#    if defined(USING_C99) || defined(BSD4_4) /*or have VSNPRINTF?*/
    int len = vsnprintf(M_NULLPTR, 0, fmt, copyarg);
#    elif defined(_WIN32)
    int len = _vscprintf(fmt, copyarg);
#    else
    int len = -1; // error, cannot get count
#    endif
    va_end(copyarg);

    if (len < 0)
    {
        *strp = M_NULLPTR;
        return -1;
    }

    *strp = M_REINTERPRET_CAST(char*, malloc(int_to_sizet(len) + SIZE_T_C(1)));
    if (*strp == M_NULLPTR)
    {
        return -1;
    }

#    if defined(USING_C99) || defined(BSD4_4) /*or have VSNPRINTF?*/
    vsnprintf(*strp, int_to_sizet(len) + 1, fmt, arg);
#    else /*don't have vsnprintf, but we allocated a buffer big enough for                                             \
             this and a NULL terminator, so vsprintf will be safe*/
    vsprintf(*strp, fmt, arg);
#    endif

    return len;
}

#endif // asprintf, vasprintf

#if defined(_MSC_VER) && _MSC_VER <= MSVC_2013 && defined _WIN32
int snprintf(char* buffer, size_t bufsz, const char* format, ...)
{
    int     charCount = -1;
    va_list args;
    va_list countargs;
    va_start(args, format);
#    if defined(va_copy)
    va_copy(countargs,
            args); // C99, but available in VS2013 which is the oldest VS
                   // compiler we expect to possibly work with this code.
#    else
    countargs = args; // this is what microsoft's va_copy expands to
#    endif
    if (bufsz > SIZE_T_C(0)) // Allow calling only when bufsz > 0. Let _vsnprintf evaluate
                             // if buffer is M_NULLPTR in here.
    {
        errno = 0;
#    if defined(HAVE_MSFT_SECURE_LIB)
        charCount = _vsnprintf_s(buffer, bufsz, _TRUNCATE, format, args);
#    else
        charCount = _vsnprintf(buffer, bufsz, format, args);
#    endif
    }
    if (charCount == -1)
    {
        if (errno == EINVAL || errno == EILSEQ)
        {
            // do not change the return value or attempt any other actions.
        }
        else
        {
            // out of space or some error occurred, so need to null terminate
            // and calculate how long the buffer should have been for this
            // format
            if (buffer && bufsz > SIZE_T_C(0))
            {
                // null terminate at bufsz since there was no more room, so we
                // are at the end of the buffer already.
                buffer[bufsz - SIZE_T_C(1)] = '\0';
            }
            charCount = _vscprintf(format, countargs); // gets the count of the number of args
        }
    }
    va_end(args);
    va_end(countargs);
    return charCount;
}

int vsnprintf(char* buffer, size_t bufsz, const char* format, va_list args)
{
    int     charCount = -1;
    va_list countargs;
#    if defined(va_copy)
    va_copy(countargs,
            args); // C99, but available in VS2013 which is the oldest VS
                   // compiler we expect to possibly work with this code.
#    else
    countargs = args; // this is what microsoft's va_copy expands to
#    endif
    if (bufsz > SIZE_T_C(0)) // Allow calling only when bufsz > 0. Let _vsnprintf evaluate
                             // if buffer is M_NULLPTR in here.
    {
        errno = 0;
#    if defined(HAVE_MSFT_SECURE_LIB)
        charCount = _vsnprintf_s(buffer, bufsz, _TRUNCATE, format, countargs);
#    else
        charCount = _vsnprintf(buffer, bufsz, format, countargs);
#    endif
    }
    if (charCount == -1)
    {
        if (errno == EINVAL || errno == EILSEQ)
        {
            // do not change the return value or attempt any other actions.
        }
        else
        {
            // out of space or some error occurred, so need to null terminate
            // and calculate how long the buffer should have been for this
            // format
            if (buffer && bufsz > SIZE_T_C(0))
            {
                // null terminate at bufsz since there was no more room, so we
                // are at the end of the buffer already.
                buffer[bufsz - SIZE_T_C(1)] = '\0';
            }
            charCount = _vscprintf(format, countargs); // gets the count of the number of args
        }
    }
    return charcount;
}
#endif // defined (_MSC_VER) && _MSC_VER <= MSVC_2013 && defined _WIN32

void print_Return_Enum(const char* funcName, eReturnValues ret)
{
    DISABLE_NONNULL_COMPARE
    if (M_NULLPTR == funcName)
    {
        printf("Unknown function returning: ");
    }
    else
    {
        printf("%s returning: ", funcName);
    }
    RESTORE_NONNULL_COMPARE

    switch (ret)
    {
    case SUCCESS:
        printf("SUCCESS\n");
        break;
    case FAILURE:
        printf("FAILURE\n");
        break;
    case NOT_SUPPORTED:
        printf("NOT SUPPORTED\n");
        break;
    case COMMAND_FAILURE:
        printf("COMMAND FAILURE\n");
        break;
    case IN_PROGRESS:
        printf("IN PROGRESS\n");
        break;
    case ABORTED:
        printf("ABORTED\n");
        break;
    case BAD_PARAMETER:
        printf("BAD PARAMETER\n");
        break;
    case MEMORY_FAILURE:
        printf("MEMORY FAILURE\n");
        break;
    case OS_PASSTHROUGH_FAILURE:
        printf("OS PASSTHROUGH FAILURE\n");
        break;
    case LIBRARY_MISMATCH:
        printf("LIBRARY MISMATCH\n");
        break;
    case FROZEN:
        printf("FROZEN\n");
        break;
    case PERMISSION_DENIED:
        printf("PERMISSION DENIED\n");
        break;
    case FILE_OPEN_ERROR:
        printf("FILE OPEN ERROR\n");
        break;
    case WARN_INCOMPLETE_RFTRS:
        printf("WARNING INCOMPLETE RTFRS\n");
        break;
    case OS_COMMAND_TIMEOUT:
        printf("COMMAND TIMEOUT\n");
        break;
    case WARN_NOT_ALL_DEVICES_ENUMERATED:
        printf("WARNING NOT ALL DEVICES ENUMERATED\n");
        break;
    case WARN_INVALID_CHECKSUM:
        printf("WARN INVALID CHECKSUM\n");
        break;
    case OS_COMMAND_NOT_AVAILABLE:
        printf("OS COMMAND NOT AVAILABLE\n");
        break;
    case OS_COMMAND_BLOCKED:
        printf("OS COMMAND BLOCKED\n");
        break;
    case COMMAND_INTERRUPTED:
        printf("COMMAND INTERRUPTED\n");
        break;
    case VALIDATION_FAILURE:
        printf("VALIDATION FAILURE\n");
        break;
    case STRIP_HDR_FOOTER_FAILURE:
        printf("STRIP HDR FOOTER FAILURE\n");
        break;
    case PARSE_FAILURE:
        printf("PARSE FAILURE\n");
        break;
    case INVALID_LENGTH:
        printf("INVALID LENGTH\n");
        break;
    case ERROR_WRITING_FILE:
        printf("ERROR WRITING FILE\n");
        break;
    case TIMEOUT:
        printf("TIMEOUT\n");
        break;
    case OS_TIMEOUT_TOO_LARGE:
        printf("OS TIMEOUT TOO LARGE\n");
        break;
    case PARSING_EXCEPTION_FAILURE:
        printf("PARSING EXCEPTION FAILURE\n");
        break;
    case POWER_CYCLE_REQUIRED:
        printf("POWER CYCLE REQUIRED\n");
        break;
    case DIR_CREATION_FAILED:
        printf("DIR CREATION FAILED\n");
        break;
    case FILE_READ_ERROR:
        printf("FILE READ ERROR\n");
        break;
    case DEVICE_ACCESS_DENIED:
        printf("DEVICE ACCESS DENIED\n");
        break;
    case NOT_PARSED:
        printf("NOT PARSED\n");
        break;
    case MISSING_INFORMATION:
        printf("MISSING INFORMATION\n");
        break;
    case TRUNCATED_FILE:
        printf("TRUNCATED FILE\n");
        break;
    case INSECURE_PATH:
        printf("INSECURE PATH\n");
        break;
    case UNKNOWN:
        printf("UNKNOWN\n");
        break;
        // NO DEFAULT CASE! This will cause warnings when an enum value is not
        // in this switch-case so that it is never out of date!
    }
    printf("\n");
}

#define DATA_LINE_BUFFER_LENGTH (70)
#define PRINTABLE_DATA_OFFSET   (50)
#define LINE_WIDTH              UINT32_C(16)
#define CHARS_PER_BUF_VAL       (3)
// This creates the output for a SINGLE line with optional printable characters and returns it.
// The pointer to the buffer and remaining length should be passed into this function!
static char* create_data_line_output(char* line, const uint8_t* dataBuffer, uint32_t bufferLen, bool showPrint)
{
    safe_memset(line, DATA_LINE_BUFFER_LENGTH, ' ', DATA_LINE_BUFFER_LENGTH - 1);
    line[DATA_LINE_BUFFER_LENGTH - 1] = '\0';
    const char hexDigits[]            = "0123456789ABCDEF";
    for (uint32_t bufferIter = UINT32_C(0), hexLineIter = UINT32_C(0), printLineIter = PRINTABLE_DATA_OFFSET;
         bufferIter < bufferLen && bufferIter < LINE_WIDTH;
         ++bufferIter, hexLineIter += CHARS_PER_BUF_VAL, printLineIter += 1)
    {
        line[hexLineIter]     = hexDigits[M_Nibble1(dataBuffer[bufferIter])];
        line[hexLineIter + 1] = hexDigits[M_Nibble0(dataBuffer[bufferIter])];
        line[hexLineIter + 2] = ' ';
        if (showPrint)
        {
            if (safe_isascii(dataBuffer[bufferIter]) && safe_isprint(C_CAST(int, dataBuffer[bufferIter])))
            {
                line[printLineIter] = C_CAST(char, dataBuffer[bufferIter]);
            }
            else
            {
                line[printLineIter] = '.';
            }
        }
    }
    return line;
}

static void internal_Print_Data_Buffer(const uint8_t* dataBuffer, uint32_t bufferLen, bool showPrint, bool showOffset)
{
    uint32_t    printIter    = UINT32_C(0);
    uint32_t    offset       = UINT32_C(0);
    const char* offsetFmtStr = "\n  0x%02" PRIX32 " ";

    DISABLE_NONNULL_COMPARE
    if (dataBuffer == M_NULLPTR || bufferLen == UINT32_C(0))
    {
        return;
    }
    RESTORE_NONNULL_COMPARE

    if (showOffset)
    {

        const char* spacePad = "\n        ";
        if (bufferLen <= UINT8_MAX)
        {
            // vars already set
        }
        else if (bufferLen <= UINT16_MAX)
        {
            offsetFmtStr = "\n  0x%04" PRIX32 " ";
            spacePad     = "\n          ";
        }
        else if (bufferLen <= UINT32_C(0xFFFFFF))
        {
            offsetFmtStr = "\n  0x%06" PRIX32 " ";
            spacePad     = "\n            ";
        }
        else // 32bit width, don't care about 64bit since max size if 32bit
        {
            offsetFmtStr = "\n  0x%08" PRIX32 " ";
            spacePad     = "\n              ";
        }
        // we print out 2 (0x) + printf formatting width + 2 (spaces) then the
        // offsets
        fputs(spacePad, stdout);
        switch (bufferLen)
        {
        case 0:
            break;
        case 1:
            fputs("0", stdout);
            break;
        case 2:
            fputs("0  1", stdout);
            break;
        case 3:
            fputs("0  1  2", stdout);
            break;
        case 4:
            fputs("0  1  2  3", stdout);
            break;
        case 5:
            fputs("0  1  2  3  4", stdout);
            break;
        case 6:
            fputs("0  1  2  3  4  5", stdout);
            break;
        case 7:
            fputs("0  1  2  3  4  5  6", stdout);
            break;
        case 8:
            fputs("0  1  2  3  4  5  6  7", stdout);
            break;
        case 9:
            fputs("0  1  2  3  4  5  6  7  8", stdout);
            break;
        case 0xA:
            fputs("0  1  2  3  4  5  6  7  8  9", stdout);
            break;
        case 0xB:
            fputs("0  1  2  3  4  5  6  7  8  9  A", stdout);
            break;
        case 0xC:
            fputs("0  1  2  3  4  5  6  7  8  9  A  B", stdout);
            break;
        case 0xD:
            fputs("0  1  2  3  4  5  6  7  8  9  A  B  C", stdout);
            break;
        case 0xE:
            fputs("0  1  2  3  4  5  6  7  8  9  A  B  C  D", stdout);
            break;
        case 0xF:
            fputs("0  1  2  3  4  5  6  7  8  9  A  B  C  D  E", stdout);
            break;
        default:
            fputs("0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F", stdout);
        }
    }

    volatile uint32_t buffLen         = bufferLen;
    uint32_t          buffLenModifier = LINE_WIDTH;

    for (printIter = UINT32_C(0), offset = UINT32_C(0); printIter < buffLen && offset < bufferLen;
         buffLen -= buffLenModifier, offset += LINE_WIDTH)
    {
        DECLARE_ZERO_INIT_ARRAY(char, line, DATA_LINE_BUFFER_LENGTH);
        if (showOffset)
        {
            // We set the correct string literal above to a variable.
            // This is 1 millisecond faster than a switch case again here for a small buffer (512B) and can be even more
            // for larger buffers This is the reason to disable the warning in this case, then re-enable it again. Since
            // this variable is only set within this function and cannot be manipulated externally, this should be safe
            // enough to disable this warning -TJE
            DISABLE_WARNING_FORMAT_NONLITERAL
            printf(offsetFmtStr, offset);
            RESTORE_WARNING_FORMAT_NONLITERAL
        }
        else
        {
            fputs("\n  ", stdout);
        }
        if (buffLen < buffLenModifier)
        {
            buffLenModifier = buffLen;
        }
        fputs(create_data_line_output(line, &dataBuffer[offset], buffLenModifier, showPrint), stdout);
    }

    fputs("\n\n", stdout);
}

void print_Data_Buffer(const uint8_t* dataBuffer, uint32_t bufferLen, bool showPrint)
{
    internal_Print_Data_Buffer(dataBuffer, bufferLen, showPrint, true);
}

void print_Pipe_Data(const uint8_t* dataBuffer, uint32_t bufferLen)
{
    internal_Print_Data_Buffer(dataBuffer, bufferLen, false, false);
}

errno_t safe_fopen_impl(FILE* M_RESTRICT* M_RESTRICT streamptr,
                        const char* M_RESTRICT       filename,
                        const char* M_RESTRICT       mode,
                        const char*                  file,
                        const char*                  function,
                        int                          line,
                        const char*                  expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (streamptr == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_fopen: streamptr is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (filename == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_fopen: filename is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (mode == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_fopen: mode is NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else
    {
#if defined(HAVE_MSFT_SECURE_LIB)
        error = fopen_s(M_CONST_CAST(FILE**, streamptr), filename, mode);
#else
        errno      = 0;
        *streamptr = fopen(filename, mode);
        if (*streamptr == M_NULLPTR)
        {
            error = errno;
            if (error == 0)
            {
                // TODO: Find a better error to use in this case. Unlikely we will need is as errno will likely be set
                // correctly
                error = EINVAL;
            }
        }
#endif
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

errno_t safe_freopen_impl(FILE* M_RESTRICT* M_RESTRICT newstreamptr,
                          const char* M_RESTRICT       filename,
                          const char* M_RESTRICT       mode,
                          FILE* M_RESTRICT             stream,
                          const char*                  file,
                          const char*                  function,
                          int                          line,
                          const char*                  expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (newstreamptr == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_freopen: newstreamptr is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (stream == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_freopen: stream is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (mode == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_freopen: mode is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
#if defined(HAVE_MSFT_SECURE_LIB)
        error = freopen_s(M_CONST_CAST(FILE**, newstreamptr), filename, mode, stream);
#else
        errno         = 0;
        *newstreamptr = freopen(filename, mode, stream);
        if (*newstreamptr == M_NULLPTR)
        {
            error = errno;
            if (error == 0)
            {
                // TODO: Find a better error to use in this case. Unlikely we will need is as errno will likely be set
                // correctly
                error = EINVAL;
            }
        }
#endif
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

// NOTE: This implementation of safe_tmpnam matches C11 tmpnam_s
//       It is commented out as it makes a LOT more sense to use safe_tmpfile call instead and because
//       calling tmpnam generates warnings about being insecure to use.-TJE
#if defined(WANT_SAFE_TMPNAM)
errno_t safe_tmpnam_impl(char*       filename_s,
                         rsize_t     maxsize,
                         const char* file,
                         const char* function,
                         int         line,
                         const char* expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (filename_s == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_tmpnam: filename_s is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else if (maxsize == RSIZE_T_C(0))
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_tmpnam: maxsize == 0", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return error;
    }
    else if (maxsize > RSIZE_MAX)
    {
        error         = EINVAL;
        filename_s[0] = 0;
        invoke_Constraint_Handler("safe_tmpnam: maxsize > RSIZE_MAX",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
#    if defined(HAVE_MSFT_SECURE_LIB)
        error = tmpnam_s(filename_s, maxsize);
#    else
        DECLARE_ZERO_INIT_ARRAY(char, internaltempname, L_tmpnam);
        errno = 0;
        if (tmpnam(internaltempname) != M_NULLPTR)
        {
            size_t internaltempnamelen = safe_strlen(internaltempname);
            if (internaltempnamelen > maxsize)
            {
                error         = EINVAL;
                filename_s[0] = 0;
                invoke_Constraint_Handler("safe_tmpnam: maxsize < generated tmpnam size",
                                          set_Env_Info(&envInfo, file, function, expression, line), error);
                errno = error;
                return error;
            }
            else
            {
                error = safe_strcpy(filename_s, maxsize, internaltempname);
            }
        }
        else
        {
            error = errno;
            if (error == 0)
            {
                // TODO: Find a better error to use in this case. Unlikely we will need is as errno will likely be
                // set correctly
                error = EINVAL;
            }
        }
#    endif
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}
#endif // WANT_SAFE_TMPNAM

errno_t safe_tmpfile_impl(FILE* M_RESTRICT* M_RESTRICT streamptr,
                          const char*                  file,
                          const char*                  function,
                          int                          line,
                          const char*                  expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (streamptr == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_tmpfile: streamptr is NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return error;
    }
    else
    {
#if defined(HAVE_MSFT_SECURE_LIB)
        error = tmpfile_s(M_CONST_CAST(FILE**, streamptr));
#else
        errno      = 0;
        *streamptr = tmpfile();
        if (*streamptr == M_NULLPTR)
        {
            error = errno;
            if (error == 0)
            {
                // TODO: Find a better error to use in this case. Unlikely we will need is as errno will likely be set
                // correctly
                error = EINVAL;
            }
        }
#endif
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

char* safe_gets_impl(char* str, rsize_t n, const char* file, const char* function, int line, const char* expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (n == RSIZE_T_C(0))
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_gets: n == 0", set_Env_Info(&envInfo, file, function, expression, line), error);
        errno = error;
        return M_NULLPTR;
    }
#if defined(HAVE_MSFT_SECURE_LIB)
    else if (n > RSIZE_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_gets: n > RSIZE_MAX", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return M_NULLPTR;
    }
#else
    else if (n > INT_MAX)
    {
        // NOTE: fgets is limited to int for the count, so this is limited to INTMAX in this case unless we write a
        // different alternative
        //       to support up to RSIZE_MAX - TJE
        error = EINVAL;
        invoke_Constraint_Handler("safe_gets: n > INT_MAX", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return M_NULLPTR;
    }
#endif
    else if (str == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_gets: str == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
        errno = error;
        return M_NULLPTR;
    }
    else
    {
#if defined(HAVE_MSFT_SECURE_LIB)
        char* ret = gets_s(str, n);
        errno     = error;
        return ret;
#else
        errno = 0;
        if (n <= RSIZE_T_C(1))
        {
            // NOTE: Handling this special case for consistent behavior.
            //       Since this "string" is too small to hold anything but NULL,
            //       we will not consider this an error and also not read anything from the stream.
            //       Some implementation do this, some report errors, some store nothing, some set NULL.
            //       Setting NULL and reading nothing seems most reasonable way to get consistent behavior
            //       across libc's that we may interact with. - TJE
            str[0] = 0;
            error  = 0;
        }
        else if (M_NULLPTR == fgets(str, M_STATIC_CAST(int, n), stdin))
        {
            str[0] = 0;
            error  = EINVAL;
            invoke_Constraint_Handler("safe_gets: fgets(str, n, stdin) == NULL",
                                      set_Env_Info(&envInfo, file, function, expression, line), error);
            errno = error;
            return M_NULLPTR;
        }
        else if (ferror(stdin))
        {
            str[0] = 0;
            error  = EINVAL;
            invoke_Constraint_Handler("safe_gets: ferror(stdin)",
                                      set_Env_Info(&envInfo, file, function, expression, line), error);
            errno = error;
            return M_NULLPTR;
        }
        else
        {
            size_t len = safe_strnlen(str, n);
            if (!feof(stdin) || (len > SIZE_T_C(0) && str[len - SIZE_T_C(1)] != '\n'))
            {
                if (len == (n - RSIZE_T_C(1)))
                {
                    // there is an error in here
                    str[0] = 0;
                    invoke_Constraint_Handler(
                        "safe_gets: Did not find newline or eof after writing n-1 chars from stdin!",
                        set_Env_Info(&envInfo, file, function, expression, line), error);
                    errno = EINVAL;
                    return M_NULLPTR;
                }
            }
            // overwrite the newline character if in the stream after fgets
            if (str[len - SIZE_T_C(1)] == '\n')
            {
                str[len - SIZE_T_C(1)] = 0;
            }
        }
        errno = error;
        return str;
#endif
    }
    RESTORE_NONNULL_COMPARE
}

errno_t safe_strtol_impl(long*                  value,
                         const char* M_RESTRICT str,
                         char** M_RESTRICT      endp,
                         int                    base,
                         const char*            file,
                         const char*            function,
                         int                    line,
                         const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (value == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtol: value == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (str == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtol: str == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else if (base > BASE_36_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtol: base > 36", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else
    {
        // We need the end pointer to detect errors in conversion reliably.
        // since that is an optional parameter, we have a local one to use if the user
        // does not provide one
        char*  localend = M_NULLPTR;
        char** useend   = endp;
        if (useend == M_NULLPTR)
        {
            useend = &localend;
        }
        errno  = 0; // must clear to zero before calling strtol to properly detect errors
        *value = strtol(str, useend, base);
        if (((*value == LONG_MAX || *value == LONG_MIN) && errno == ERANGE) || (*value == 0L && *useend == str) ||
            errno != 0)
        {
            error = errno;
            if (error == 0)
            {
                error = EINVAL;
            }
        }
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

errno_t safe_strtoll_impl(long long*             value,
                          const char* M_RESTRICT str,
                          char** M_RESTRICT      endp,
                          int                    base,
                          const char*            file,
                          const char*            function,
                          int                    line,
                          const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (value == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoll: value == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (str == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoll: str == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else if (base > BASE_36_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoll: base > 36", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else
    {
        // We need the end pointer to detect errors in conversion reliably.
        // since that is an optional parameter, we have a local one to use if the user
        // does not provide one
        char*  localend = M_NULLPTR;
        char** useend   = endp;
        if (useend == M_NULLPTR)
        {
            useend = &localend;
        }
        errno  = 0; // must clear to zero before calling strtol to properly detect errors
        *value = strtoll(str, useend, base);
        if (((*value == LLONG_MAX || *value == LLONG_MIN) && errno == ERANGE) || (*value == 0LL && *useend == str) ||
            errno != 0)
        {
            error = errno;
            if (error == 0)
            {
                error = EINVAL;
            }
        }
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

errno_t safe_strtoul_impl(unsigned long*         value,
                          const char* M_RESTRICT str,
                          char** M_RESTRICT      endp,
                          int                    base,
                          const char*            file,
                          const char*            function,
                          int                    line,
                          const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (value == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoul: value == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (str == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoul: str == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else if (base > BASE_36_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoul: base > 36", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else
    {
        // We need the end pointer to detect errors in conversion reliably.
        // since that is an optional parameter, we have a local one to use if the user
        // does not provide one
        char*  localend = M_NULLPTR;
        char** useend   = endp;
        if (useend == M_NULLPTR)
        {
            useend = &localend;
        }
        errno  = 0; // must clear to zero before calling strtol to properly detect errors
        *value = strtoul(str, useend, base);
        if ((*value == ULONG_MAX && errno == ERANGE) || (*value == 0UL && *useend == str) || errno != 0)
        {
            error = errno;
            if (error == 0)
            {
                error = EINVAL;
            }
        }
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

errno_t safe_strtoull_impl(unsigned long long*    value,
                           const char* M_RESTRICT str,
                           char** M_RESTRICT      endp,
                           int                    base,
                           const char*            file,
                           const char*            function,
                           int                    line,
                           const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (value == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoull: value == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (str == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoull: str == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (base > BASE_36_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoull: base > 36", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else
    {
        // We need the end pointer to detect errors in conversion reliably.
        // since that is an optional parameter, we have a local one to use if the user
        // does not provide one
        char*  localend = M_NULLPTR;
        char** useend   = endp;
        if (useend == M_NULLPTR)
        {
            useend = &localend;
        }
        errno  = 0; // must clear to zero before calling strtol to properly detect errors
        *value = strtoull(str, useend, base);
        if ((*value == ULLONG_MAX && errno == ERANGE) || (*value == 0ULL && *useend == str) || errno != 0)
        {
            error = errno;
            if (error == 0)
            {
                error = EINVAL;
            }
        }
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

errno_t safe_strtoimax_impl(intmax_t*              value,
                            const char* M_RESTRICT str,
                            char** M_RESTRICT      endp,
                            int                    base,
                            const char*            file,
                            const char*            function,
                            int                    line,
                            const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (value == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoimax: value == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (str == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoimax: str == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (base > BASE_36_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoimax: base > 36", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else
    {
        // We need the end pointer to detect errors in conversion reliably.
        // since that is an optional parameter, we have a local one to use if the user
        // does not provide one
        char*  localend = M_NULLPTR;
        char** useend   = endp;
        if (useend == M_NULLPTR)
        {
            useend = &localend;
        }
        errno  = 0; // must clear to zero before calling strtol to properly detect errors
        *value = strtoimax(str, useend, base);
        if (((*value == INTMAX_MAX || *value == INTMAX_MIN) && errno == ERANGE) ||
            (*value == INTMAX_C(0) && *useend == str) || errno != 0)
        {
            error = errno;
            if (error == 0)
            {
                error = EINVAL;
            }
        }
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

errno_t safe_strtoumax_impl(uintmax_t*             value,
                            const char* M_RESTRICT str,
                            char** M_RESTRICT      endp,
                            int                    base,
                            const char*            file,
                            const char*            function,
                            int                    line,
                            const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (value == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoumax: value == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (str == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoumax: str == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (base > BASE_36_MAX)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtoumax: base > 36", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else
    {
        // We need the end pointer to detect errors in conversion reliably.
        // since that is an optional parameter, we have a local one to use if the user
        // does not provide one
        char*  localend = M_NULLPTR;
        char** useend   = endp;
        if (useend == M_NULLPTR)
        {
            useend = &localend;
        }
        errno  = 0; // must clear to zero before calling strtol to properly detect errors
        *value = strtoumax(str, useend, base);
        if ((*value == UINTMAX_MAX && errno == ERANGE) || (*value == UINTMAX_C(0) && *useend == str) || errno != 0)
        {
            error = errno;
            if (error == 0)
            {
                error = EINVAL;
            }
        }
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

errno_t safe_strtof_impl(float*                 value,
                         const char* M_RESTRICT str,
                         char** M_RESTRICT      endp,
                         const char*            file,
                         const char*            function,
                         int                    line,
                         const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (value == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtof: value == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (str == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtof: str == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else
    {
        // We need the end pointer to detect errors in conversion reliably.
        // since that is an optional parameter, we have a local one to use if the user
        // does not provide one
        char*  localend = M_NULLPTR;
        char** useend   = endp;
        if (useend == M_NULLPTR)
        {
            useend = &localend;
        }
        errno  = 0; // must clear to zero before calling strtol to properly detect errors
        *value = strtof(str, useend);
        // Disable float equal because this is necessary to check for conversion errors from this function.
        // This is one of the only times this warning should ever be disabled.
        DISABLE_WARNING_FLOAT_EQUAL
        if (((*value >= HUGE_VALF || *value <= -HUGE_VALF) && errno == ERANGE) || (*value == 0.0F && *useend == str) ||
            errno != 0)
        {
            error = errno;
            if (error == 0)
            {
                error = EINVAL;
            }
        }
        RESTORE_WARNING_FLOAT_EQUAL
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

errno_t safe_strtod_impl(double*                value,
                         const char* M_RESTRICT str,
                         char** M_RESTRICT      endp,
                         const char*            file,
                         const char*            function,
                         int                    line,
                         const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (value == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtod: value == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (str == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtod: str == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else
    {
        // We need the end pointer to detect errors in conversion reliably.
        // since that is an optional parameter, we have a local one to use if the user
        // does not provide one
        char*  localend = M_NULLPTR;
        char** useend   = endp;
        if (useend == M_NULLPTR)
        {
            useend = &localend;
        }
        errno  = 0; // must clear to zero before calling strtol to properly detect errors
        *value = strtod(str, useend);
        // Disable float equal because this is necessary to check for conversion errors from this function.
        // This is one of the only times this warning should ever be disabled.
        DISABLE_WARNING_FLOAT_EQUAL
        if (((*value >= HUGE_VAL || *value <= -HUGE_VAL) && errno == ERANGE) || (*value == 0.0 && *useend == str) ||
            errno != 0)
        {
            error = errno;
            if (error == 0)
            {
                error = EINVAL;
            }
        }
        RESTORE_WARNING_FLOAT_EQUAL
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

errno_t safe_strtold_impl(long double*           value,
                          const char* M_RESTRICT str,
                          char** M_RESTRICT      endp,
                          const char*            file,
                          const char*            function,
                          int                    line,
                          const char*            expression)
{
    errno_t           error = 0;
    constraintEnvInfo envInfo;
    DISABLE_NONNULL_COMPARE
    if (value == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtold: value == NULL",
                                  set_Env_Info(&envInfo, file, function, expression, line), error);
    }
    else if (str == M_NULLPTR)
    {
        error = EINVAL;
        invoke_Constraint_Handler("safe_strtold: str == NULL", set_Env_Info(&envInfo, file, function, expression, line),
                                  error);
    }
    else
    {
        // We need the end pointer to detect errors in conversion reliably.
        // since that is an optional parameter, we have a local one to use if the user
        // does not provide one
        char*  localend = M_NULLPTR;
        char** useend   = endp;
        if (useend == M_NULLPTR)
        {
            useend = &localend;
        }
        errno  = 0; // must clear to zero before calling strtol to properly detect errors
        *value = strtold(str, useend);
        // Disable float equal because this is necessary to check for conversion errors from this function.
        // This is one of the only times this warning should ever be disabled.
        DISABLE_WARNING_FLOAT_EQUAL
        if (((*value >= HUGE_VALL || *value <= -HUGE_VALL) && errno == ERANGE) || (*value == 0.0L && *useend == str) ||
            errno != 0)
        {
            error = errno;
            if (error == 0)
            {
                error = EINVAL;
            }
        }
        RESTORE_WARNING_FLOAT_EQUAL
    }
    RESTORE_NONNULL_COMPARE
    errno = error;
    return error;
}

errno_t safe_atoi_impl(int*                   value,
                       const char* M_RESTRICT str,
                       const char*            file,
                       const char*            function,
                       int                    line,
                       const char*            expression)
{
    DISABLE_NONNULL_COMPARE
    if (value == M_NULLPTR)
    {
        errno = EINVAL;
        return EINVAL;
    }
    RESTORE_NONNULL_COMPARE
    char*   endp  = M_NULLPTR;
    long    temp  = 0L;
    errno_t error = safe_strtol_impl(&temp, str, &endp, BASE_10_DECIMAL, file, function, line, expression);
    if (error == 0)
    {
        if (temp > INT_MAX || temp < INT_MIN)
        {
            error  = ERANGE;
            *value = 0;
        }
        else if (*endp != '\0')
        {
            error  = EINVAL;
            *value = 0;
        }
        else
        {
            *value = M_STATIC_CAST(int, temp);
        }
    }
    errno = error;
    return error;
}

errno_t safe_atol_impl(long*                  value,
                       const char* M_RESTRICT str,
                       const char*            file,
                       const char*            function,
                       int                    line,
                       const char*            expression)
{
    char*   endp  = M_NULLPTR;
    errno_t error = safe_strtol_impl(value, str, &endp, BASE_10_DECIMAL, file, function, line, expression);
    if (error == 0 && *endp != '\0')
    {
        error  = EINVAL;
        *value = 0L;
    }
    errno = error;
    return error;
}

errno_t safe_atoll_impl(long long*             value,
                        const char* M_RESTRICT str,
                        const char*            file,
                        const char*            function,
                        int                    line,
                        const char*            expression)
{
    char*   endp  = M_NULLPTR;
    errno_t error = safe_strtoll_impl(value, str, &endp, BASE_10_DECIMAL, file, function, line, expression);
    if (error == 0 && *endp != '\0')
    {
        error  = EINVAL;
        *value = 0LL;
    }
    errno = error;
    return error;
}

errno_t safe_atof_impl(double*                value,
                       const char* M_RESTRICT str,
                       const char*            file,
                       const char*            function,
                       int                    line,
                       const char*            expression)
{
    char*   endp  = M_NULLPTR;
    errno_t error = safe_strtod_impl(value, str, &endp, file, function, line, expression);
    if (error == 0 && *endp != '\0')
    {
        error  = EINVAL;
        *value = 0;
    }
    errno = error;
    return error;
}

int impl_snprintf_err_handle(const char* file,
                             const char* function,
                             int         line,
                             const char* expression,
                             char*       buf, 
                             size_t      bufsize, 
                             const char* format, 
                             ...)
{
    int     n = 0;
    va_list args;
    va_start(args, format);
    // Disabling this warning in GCC and Clang for now. It only seems to show in Windows at the moment-TJE
    DISABLE_WARNING_FORMAT_NONLITERAL
    // NOLINTBEGIN(clang-analyzer-valist.Uninitialized,clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    // - false positive
    n = vsnprintf(buf, bufsize, format, args);
    // NOLINTEND(clang-analyzer-valist.Uninitialized,clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    // - false positive
    RESTORE_WARNING_FORMAT_NONLITERAL
    va_end(args);

    if (n < 0 || (buf != M_NULLPTR && bufsize != 0 && int_to_sizet(n) >= bufsize))
    {
        if (buf != M_NULLPTR && bufsize > 0)
        {
            buf[bufsize - 1] = 0;
        }
        constraintEnvInfo envInfo;
        errno = EINVAL;
        invoke_Constraint_Handler("snprintf_error_handler_macro: error in snprintf",
                                    set_Env_Info(&envInfo, file, function, expression, line), EINVAL);
    }
    return n;
}
