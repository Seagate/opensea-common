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
// \file io_utils.c
// \brief Implements various functions to work with IO (printf, snprintf, etc)
//        This file also has functionality for changing output colors and reading user input.
//        If working with files, it is recommended secure_file.h is used instead.

#include "env_detect.h"
#include "io_utils.h"
#include "memory_safety.h"
#include "common_types.h"
#include "type_conversion.h"
#include "string_utils.h"
#include <string.h>
#include <math.h> //HUGE_VALF, HUGE_VAL, HUGE_VALL
#include <ctype.h>

#if defined (UEFI_C_SOURCE)
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>//to get global boot services pointer. This pointer should be checked before use, but any app using stdlib will have this set.
#include <Library/PrintLib.h> //to convert CHAR16 string to CHAR8. may also be able to use stdlib in someway, but this seems to work
#include <Protocol/SimpleTextOut.h> //for colors
#endif //UEFI_C_SOURCE

#if defined (_MSC_VER) && _MSC_VER <= 1800 && defined _WIN32
int snprintf(char* buffer, size_t bufsz, const char* format, ...)
{
    int charCount = -1;
    va_list args;
    va_list countargs;
    va_start(args, format);
#if defined (va_copy)
    va_copy(countargs, args);//C99, but available in VS2013 which is the oldest VS compiler we expect to possibly work with this code.
#else
    countargs = args;//this is what microsoft's va_copy expands to
#endif
    if (bufsz > 0) //Allow calling only when bufsz > 0. Let _vsnprintf evaluate if buffer is M_NULLPTR in here.
    {
        errno = 0;
#if defined (__STDC_SECURE_LIB__)
        charCount = _vsnprintf_s(buffer, bufsz, _TRUNCATE, format, args);
#else
        charCount = _vsnprintf(buffer, bufsz, format, args);
#endif
    }
    if (charCount == -1)
    {
        if (errno == EINVAL || errno == EILSEQ)
        {
            //do not change the return value or attempt any other actions.
        }
        else
        {
            //out of space or some error occurred, so need to null terminate and calculate how long the buffer should have been for this format
            if (buffer && bufsz > 0)
            {
                //null terminate at bufsz since there was no more room, so we are at the end of the buffer already.
                buffer[bufsz - 1] = '\0';
            }
            charCount = _vscprintf(format, countargs);//gets the count of the number of args
        }
    }
    va_end(args);
    va_end(countargs);
    return charCount;
}
#endif

#if defined (UEFI_C_SOURCE)
eReturnValues get_Simple_Text_Output_Protocol_Ptr(void** pOutput)
{
    eReturnValues ret = SUCCESS;
    EFI_STATUS uefiStatus = EFI_SUCCESS;
    EFI_HANDLE* handle = M_NULLPTR;
    EFI_GUID outputGuid = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
    UINTN nodeCount = 0;

    if (!gBS) //make sure global boot services pointer is valid before accessing it.
    {
        return MEMORY_FAILURE;
    }

    uefiStatus = gBS->LocateHandleBuffer(ByProtocol, &outputGuid, M_NULLPTR, &nodeCount, &handle);
    if (EFI_ERROR(uefiStatus))
    {
        return FAILURE;
    }
    //NOTE: This code below assumes that we only care to change color output on node 0. This seems to work from a quick test, but may not be correct. Not sure what the other 2 nodes are for...serial?
    uefiStatus = gBS->OpenProtocol(handle[0], &outputGuid, pOutput, gImageHandle, M_NULLPTR, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    //TODO: based on the error code, rather than assuming failure, check for supported/not supported.
    if (EFI_ERROR(uefiStatus))
    {
        ret = FAILURE;
    }
    return ret;
}

void close_Simple_Text_Output_Protocol_Ptr(void** pOutput)
{
    EFI_STATUS uefiStatus = EFI_SUCCESS;
    EFI_HANDLE* handle = M_NULLPTR;
    EFI_GUID outputGuid = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
    UINTN nodeCount = 0;

    if (!gBS) //make sure global boot services pointer is valid before accessing it.
    {
        return;
    }

    uefiStatus = gBS->LocateHandleBuffer(ByProtocol, &outputGuid, M_NULLPTR, &nodeCount, &handle);
    if (EFI_ERROR(uefiStatus))
    {
        return;
    }
    //NOTE: This code below assumes that we only care to change color output on node 0. This seems to work from a quick test, but may not be correct. Not sure what the other 2 nodes are for...serial?
    uefiStatus = gBS->CloseProtocol(handle[0], &outputGuid, gImageHandle, M_NULLPTR);
    //TODO: based on the error code, rather than assuming failure, check for supported/not supported.
    if (EFI_ERROR(uefiStatus))
    {
        perror("Failed to close simple text output protocol\n");
    }
    return;
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
            //printf("Default text output attributes are set to %" PRIX32 "\n", defaultAttributes);
            close_Simple_Text_Output_Protocol_Ptr((void**)&outputProtocol);
        }
    }
    return defaultAttributes;
}

//Comment from simpletextout.h describes acceptable colors....we will try setting whatever we can, but it may not actually work!
// For Foreground, and EFI_* value is valid from EFI_BLACK(0x00) to
// EFI_WHITE (0x0F).
// For Background, only EFI_BLACK, EFI_BLUE, EFI_GREEN, EFI_CYAN,
// EFI_RED, EFI_MAGENTA, EFI_BROWN, and EFI_LIGHTGRAY are acceptable
void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    static bool defaultsSet = false;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* outputProtocol;
    if (!defaultsSet)
    {
        //First time we are setting colors backup the default settings so they can be restored properly later.
        get_Default_Console_Colors();
        defaultsSet = true;
    }
    if (SUCCESS == get_Simple_Text_Output_Protocol_Ptr((void**)&outputProtocol))
    {
        if (foregroundBackground)//change foreground color
        {
            //save current background color
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
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(M_Nibble0(C_CAST(unsigned long long, get_Default_Console_Colors())), currentBackground));
                break;
            }
        }
        else//change background color
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
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, M_Nibble1(C_CAST(unsigned long long, get_Default_Console_Colors()))));
                break;
            }
        }
        //close the protocol since we are done for now.
        close_Simple_Text_Output_Protocol_Ptr((void**)&outputProtocol);
    }
}
#elif defined (_WIN32)
static uint16_t get_Console_Default_Color(void)
{
    static uint16_t defaultConsoleAttributes = UINT16_MAX;
    if (defaultConsoleAttributes == UINT16_MAX)
    {
        CONSOLE_SCREEN_BUFFER_INFO defaultInfo;
        memset(&defaultInfo, 0, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &defaultInfo))
        {
            defaultConsoleAttributes = defaultInfo.wAttributes;
        }
        else
        {
            //set defaultColorVal to zero
            defaultConsoleAttributes = 0;
        }
    }
    return defaultConsoleAttributes;
}

static uint16_t get_Console_Current_Color(void)
{
    uint16_t currentConsoleAttributes = UINT16_MAX;
    CONSOLE_SCREEN_BUFFER_INFO currentInfo;
    memset(&currentInfo, 0, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &currentInfo))
    {
        currentConsoleAttributes = currentInfo.wAttributes;
    }
    else
    {
        //set defaultColorVal to zero
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
    static bool defaultsSet = false;
    static WORD defaultColorValue = 0;
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD theColor = 0;
    if (!defaultsSet)
    {
        //First time we are setting colors backup the default settings so they can be restored properly later.
        defaultColorValue = get_Console_Default_Color();
        defaultsSet = true;
    }
    theColor = get_Console_Current_Color();//get current colors after defaults are setup.
    //now change what is requested
    if (foregroundColor != CONSOLE_COLOR_CURRENT)
    {
        //clear out foreground bits, then set the requested color
        theColor &= 0xFFF0;//foreground are lowest 4 bits
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
            theColor |= 0;//this should mean no colors or black
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
        case CONSOLE_COLOR_DEFAULT://fall through to default
        default:
            theColor |= (defaultColorValue & 0x000F);
            break;
        }
    }
    if (backgroundColor != CONSOLE_COLOR_CURRENT)
    {
        //clear out background bits, then set the requested color
        theColor &= 0xFF0F;//foreground are middle 4 bits
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
            theColor |= 0;//this should mean no colors or black
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
        case CONSOLE_COLOR_DEFAULT://fall through to default
        default:
            theColor |= (defaultColorValue & 0x00F0);
            break;
        }
    }
    SetConsoleTextAttribute(consoleHandle, theColor);
    return;
}

static DWORD get_Input_Console_Default_Mode(void)
{
    static DWORD defaultConsoleMode = UINT32_MAX;
    if (defaultConsoleMode == UINT32_MAX)
    {
        if (FALSE == GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &defaultConsoleMode))
        {
            defaultConsoleMode = 0;
            //From MSFT documentation: https://learn.microsoft.com/en-us/windows/console/setconsolemode?redirectedfrom=MSDN
            //"When a console is created, all input modes except ENABLE_WINDOW_INPUT and ENABLE_VIRTUAL_TERMINAL_INPUT are enabled by default."
            defaultConsoleMode = ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE | ENABLE_LINE_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE;
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
    eReturnValues ret = SUCCESS;
    DWORD defaultConMode = get_Input_Console_Default_Mode();
    DWORD conMode = defaultConMode;
    conMode &= C_CAST(DWORD, ~(ENABLE_ECHO_INPUT));
    printf("%s", prompt);
    fflush(stdout);
    //disable echoing typed characters so that
    if (set_Input_Console_Mode(conMode))
    {
        if (getline(userInput, inputDataLen, stdin) <= 0)
        {
            ret = FAILURE;
        }
        else
        {
            //remove newline from the end...convert to a null.
            size_t endofinput = strlen(*userInput);
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
#include "secured_env_vars.h"
#if (defined(__FreeBSD__) && __FreeBSD__ >= 4 /*4.6 technically*/) || (defined(__OpenBSD__) && defined OpenBSD2_9)
#include <readpassphrase.h>
#endif //FreeBSD 4.6+ or OpenBSD 2.9+
#include <termios.h>
void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    //use the new behavior
    if (foregroundBackground)
    {
        set_Console_Foreground_Background_Colors(consoleColor, CONSOLE_COLOR_CURRENT);
    }
    else
    {
        set_Console_Foreground_Background_Colors(CONSOLE_COLOR_CURRENT, consoleColor);
    }
}

typedef enum _eKnownTERM
{
    TERM_UNKNOWN,
    TERM_XTERM,//no mention of 256 color output support. Can assume 16 colors for now-TJE
    TERM_XTERM_256COLOR,
    TERM_GENERIC_256COLOR,
    TERM_SUN_COLOR,
    TERM_AIXTERM,
    TERM_GNOME_256COLOR,
    TERM_GNOME_COLOR,
    TERM_TRUECOLOR_256COLOR,
    TERM_GENERIC_COLOR,//unknown level of support for colors...we just know it has some color capabilities
    TERM_LINUX_COLOR,//16 colors
    TERM_LINUX_256COLOR,//16 colors
    //Add more terminal types to use for various color output formats
}eKnownTERM;

//other things to potentially look for: 
//    "GNOME_TERMINAL_SERVICE"
//    "GNOME_TERMINAL_SCREEN"
//    "TERM_PROGRAM"         <---git bash and msys2 in windows use this and set it to "mintty". TERM is set to xterm
//    "TERM_PROGRAM_VERSION" <---git bash and msys2 in windows use this for mintty version number
//See this for more info: https://github.com/termstandard/colors
static eKnownTERM get_Terminal_Type(void)
{
    eKnownTERM terminalType = TERM_UNKNOWN;
    char* termEnv = M_NULLPTR;
    bool haveCompleteMatch = false;
    if (get_Environment_Variable("TERM", &termEnv) == ENV_VAR_SUCCESS && termEnv != M_NULLPTR)
    {
        //do exact matches up top, then search for substrings
        if (strcmp(termEnv, "xterm-256color") == 0)
        {
            terminalType = TERM_XTERM_256COLOR;
            haveCompleteMatch = true;
        }
        else if (strcmp(termEnv, "aixterm") == 0)
        {
            terminalType = TERM_AIXTERM;
            haveCompleteMatch = true;
        }
        else if (strcmp(termEnv, "sun-color") == 0)
        {
            terminalType = TERM_SUN_COLOR;
            haveCompleteMatch = true;
        }
        else if (strcmp(termEnv, "xterm") == 0)
        {
            terminalType = TERM_XTERM;
        }
        else if (strcmp(termEnv, "linux") == 0)
        {
            //alpine linux does not set COLORTERM or anything else, so this would be complete there, but let the other checks run too
            //linux kernel 3.16 and earlier do not support "truecolor" and will be more limited, but this may be ok since we are not using rgb format. Only 16 different colors-TJE
            OSVersionNumber linVer;
            memset(&linVer, 0, sizeof(OSVersionNumber));
            if (SUCCESS == get_Operating_System_Version_And_Name(&linVer, M_NULLPTR))
            {
                if (linVer.versionType.linuxVersion.majorVersion >= 4 || (linVer.versionType.linuxVersion.majorVersion >= 3 && linVer.versionType.linuxVersion.minorVersion > 16))
                {
                    terminalType = TERM_LINUX_256COLOR;
                    haveCompleteMatch = true;
                }
                else
                {
                    //limited to 16 colors...which is fine
                    terminalType = TERM_LINUX_COLOR;
                    haveCompleteMatch = true;
                }
            }
            else
            {
                //assuming this as it seems to still support 16 colors, enough for us today
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
            //check what this is set to.
            //truecolor seems to mean supports 256 colors
            if (strcmp(termEnv, "gnome-terminal") == 0)
            {
                //centos6 reports "gnome-terminal" which seems to support 256 colors output mode
                if (terminalType == TERM_XTERM)
                {
                    terminalType = TERM_GNOME_256COLOR;
                    haveCompleteMatch = true;
                }
                else
                {
                    terminalType = TERM_GNOME_COLOR;
                    haveCompleteMatch = true;
                }
            }
            else if (strstr(termEnv, "truecolor") == 0)
            {
                terminalType = TERM_TRUECOLOR_256COLOR;
                haveCompleteMatch = true;
            }
            else if (strncmp(termEnv, "vte", 3) == 0)
            {
                terminalType = TERM_TRUECOLOR_256COLOR;
                haveCompleteMatch = true;
            }
            else //this case is generic and a "last" effort in this part. If "COLORTERM" is set, assume it has some color output support
            {
                terminalType = TERM_GENERIC_COLOR;
            }
        }
    }
    if (!haveCompleteMatch)
    {
        if (get_Environment_Variable("COLORFGBR", &termEnv) == ENV_VAR_SUCCESS && termEnv != M_NULLPTR)
        {
            //this environment variable is found in kde for forground and background terminal colors
            //This is probably good enough to say "it has some color" support at this point, but no further indication of what color support is available
            terminalType = TERM_GENERIC_COLOR;
        }
    }
    safe_Free(C_CAST(void**, &termEnv));
    return terminalType;
}

typedef struct _consoleColorCap
{
    bool colorSupport;
    bool eightColorsOnly;
    bool eightBackgroundColorsOnly;
    bool use256ColorFormat;
    bool useIntensityBitFormat;
    bool useInvertFormatForBackgroundColors;
    //bool useColonInsteadOfSemiColon; //TODO: Implement support for the very few consoles that need a colon instead of semicolon
}consoleColorCap, * ptrConsoleColorCap;

//Future var we might need is whether the reset to defaults (39m & 49m) work or if the complete reset is needed (0m)
static void get_Console_Color_Capabilities(ptrConsoleColorCap colorCapabilities)
{
    if (colorCapabilities)
    {
        eKnownTERM term = get_Terminal_Type();
        memset(colorCapabilities, 0, sizeof(consoleColorCap));
        switch (term)
        {
        case TERM_LINUX_256COLOR:
            colorCapabilities->useIntensityBitFormat = true;
            colorCapabilities->eightBackgroundColorsOnly = true;
            M_FALLTHROUGH;
        case TERM_XTERM_256COLOR:
        case TERM_GENERIC_256COLOR:
        case TERM_SUN_COLOR:
        case TERM_GNOME_256COLOR:
        case TERM_TRUECOLOR_256COLOR:
            colorCapabilities->colorSupport = true;
            colorCapabilities->use256ColorFormat = true;
            break;
        case TERM_LINUX_COLOR:
            colorCapabilities->useIntensityBitFormat = true;
            colorCapabilities->eightBackgroundColorsOnly = true;
            M_FALLTHROUGH;
        case TERM_XTERM:
        case TERM_AIXTERM:
        case TERM_GNOME_COLOR:
        case TERM_GENERIC_COLOR:
            colorCapabilities->colorSupport = true;
            //these should support 16 colors
            break;
        case TERM_UNKNOWN:
            //Assuming color is supported for now until we run into a terminal that does not support any color code changes
            //When this is found we need it in a different case to return this variable
            colorCapabilities->colorSupport = true;
            //if a terminal does not have any way of doing bright color output, set the 8colorsOnly to true. Ex: red/bright red will be the same in these cases
            colorCapabilities->useIntensityBitFormat = true;
            break;
        }
    }
    return;
}

void set_Console_Foreground_Background_Colors(eConsoleColors foregroundColor, eConsoleColors backgroundColor)
{
    static consoleColorCap consoleCap;
    static bool haveTermCapabilities = false;
    if (!haveTermCapabilities)
    {
        get_Console_Color_Capabilities(&consoleCap);
        haveTermCapabilities = true;
    }
    if (consoleCap.colorSupport) //if color is not supported, do not do anything as the escape sequences will just print to the output and make it a mess
    {
        if (foregroundColor == CONSOLE_COLOR_CURRENT && backgroundColor == CONSOLE_COLOR_CURRENT)
        {
            //nothing to do
            return;
        }
        if (foregroundColor == CONSOLE_COLOR_DEFAULT && backgroundColor == CONSOLE_COLOR_DEFAULT)
        {
            //reset or normal
            printf("\033[0m");
        }
        else
        {
            //written according to https://en.wikipedia.org/wiki/ANSI_escape_code
            //  There are possible needs to adjust color output if this does not work for an OS we are supporting. Can ifdef or check for other things
            //set the string for each color that needs to be set.
            uint8_t foregroundColorInt = UINT8_MAX;
            uint8_t backgroundColorInt = UINT8_MAX;
            //Checking for env variable COLORTERM is one method, or COLORFGBG, or if TERM is set to sun-color, xterm-256color, true-color, or gnome-terminal will work for 256color
            //when debugging a unix-like system, try printenv | grep "color" and printenv | grep "COLOR" to see what you can find out about the terminal
            //testing on CentOS 6 through latest Ubuntu the bright colors seem to be supported (codes 90-97 and 100-107) as well as 256color codes
            //In CentOS 6 & 7 the "intensity" field does change to bright colors, but in Ubuntu 22.04 this only changes to a bold font
            //If the extended colors are not available, will need to use the "intensity" method of \033[1:##m to set it instead
            //Sometimes the "intensity" just makes things bold though
            //FreeBSD 11 through 13 claim xterm-256color so they are also likely supported by 256color and extra bright color codes.
            //omniOS r151038 seems to support 256 color codes properly too (sun-color terminal)
            //aixterm also appears to support some color: https://www.ibm.com/docs/en/aix/7.2?topic=aixterm-command, but 256 color is not listed
            //reading terminfo or termcap may be the best way to change formats or ignore color changes if not capable at all, but that is much more complicated to
            //  implement in here right now.
            //http://jdebp.uk./Softwares/nosh/guide/commands/TerminalCapabilities.xml
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
                //aixterm does not list this, so will need to test it! otherwise reset with 0m will be as close as we get
                backgroundColorInt = 49;
                break;
            }
            //if background colors do not work, may need to try the "invert" trick to make it happen using a format like \033[7;nm or \033[7;1;nm
            if (backgroundColorInt != UINT8_MAX)
            {
                uint8_t back256Color = UINT8_MAX;
                if (backgroundColorInt < 100)
                {
                    back256Color = backgroundColorInt - 40;//256 colors start at 0
                }
                else
                {
                    back256Color = backgroundColorInt - 100 + 8;//256 bright colors start at 8
                }
                if (backgroundColorInt == 49 || !consoleCap.use256ColorFormat || (consoleCap.use256ColorFormat && consoleCap.eightBackgroundColorsOnly))
                {
                    //use the inversion method with 7;intensity;colorm
                    if (consoleCap.useInvertFormatForBackgroundColors)
                    {
                        //more background colors are available using the inversion method (maybe)
                        //convert the color to a foreground color first
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
                        //print the background request
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
                //aixterm does not list this, so will need to test it! otherwise reset with 0m will be as close as we get
                foregroundColorInt = 39;
                break;
            }
            if (foregroundColorInt != UINT8_MAX)
            {
                uint8_t fore256Color = UINT8_MAX;
                if (foregroundColorInt < 90)
                {
                    fore256Color = foregroundColorInt - 30;//256 colors start at 0
                }
                else
                {
                    fore256Color = foregroundColorInt - 90 + 8;//256 bright colors start at 8
                }
                if (foregroundColorInt == 39 || !consoleCap.use256ColorFormat)
                {
                    //print the foreground request
                    if (consoleCap.useInvertFormatForBackgroundColors)
                    {
                        //more background colors are available using the inversion method (maybe)
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
    return;
}

//if not POSIX.1-2001, getpass exists, but has issues and is not recommended. https://www.man7.org/linux/man-pages/man3/getpass.3.html
//getpassphrase exists in BSD https://man.freebsd.org/cgi/man.cgi?query=readpassphrase&apropos=0&sektion=0&manpath=FreeBSD+14.0-RELEASE+and+Ports&arch=default&format=html
//getpass_r is available in NetBSD 7..same with getpassfd
//getpassphrase is in sunos and accepts up to 256 chars. In sun 5.19 getpass says it reads up to 9 characters getpassphrase starts in sun 5.6
//Unix 7 says 8 characters
//freebsd, netbsd, older linux, etc usually say getpass can accept up to 128 chars
//From one of the BSD manpages on getpass:
//  Historically getpass accepted and returned a password if	it  could  not
//  modify  the terminal settings to	turn echo off(or if the input was not
//  a terminal).
//So if necessary, for "compatibility" this could be implemented without echo, but avoiding that for now-TJE
eReturnValues get_Secure_User_Input(const char* prompt, char** userInput, size_t* inputDataLen)
{
    eReturnValues ret = SUCCESS;
#if defined (POSIX_2001)
    struct termios defaultterm, currentterm;
    FILE* term = fopen("/dev/tty", "r");//use /dev/tty instead of stdin to get the terminal controlling the process.
    bool devtty = true;
    memset(&defaultterm, 0, sizeof(struct termios));
    memset(&currentterm, 0, sizeof(struct termios));
    if (!term)
    {
        term = stdin;//fallback to stdin I guess...
        devtty = false;
    }
    //get the default terminal flags
    if (tcgetattr(fileno(term), &defaultterm))
    {
        if (devtty)
        {
            fclose(term);
        }
        return FAILURE;
    }
    memcpy(&currentterm, &defaultterm, sizeof(struct termios));
    //print the prompt
    printf("%s", prompt);
    fflush(stdout);
    //disable echo for now.
    currentterm.c_lflag &= C_CAST(tcflag_t, ~ECHO);
    if (tcsetattr(fileno(term), TCSAFLUSH, &currentterm) != 0)
    {
        if (devtty)
        {
            fclose(term);
        }
        printf("\n");
        return FAILURE;
    }
    //now read the input with getline
    if (getline(userInput, inputDataLen, term) <= 0)
    {
        ret = FAILURE;
    }
    else
    {
        //check if newline was read (it likely will be there) and remove it
        //remove newline from the end...convert to a null.
        size_t endofinput = strlen(*userInput);
        if ((*userInput)[endofinput - 1] == '\n')
        {
            (*userInput)[endofinput - 1] = '\0';
        }
    }
    //restore echo/default flags
    if (tcsetattr(fileno(term), TCSAFLUSH, &defaultterm) != 0)
    {
        if (devtty)
        {
            fclose(term);
        }
        printf("\n");
        return FAILURE;
    }
    if (devtty)
    {
        fclose(term);
    }
    printf("\n");
#elif (defined (__FreeBSD__) && __FreeBSD__ >= 4 /*4.6 technically*/) || (defined (__OpenBSD__) && defined OpenBSD2_9)
    //use readpassphrase instead
    //use BUFSIZ buffer as that should be more than enough to read this
    //NOTE: Linux's libbsd also provides this, but termios method above is still preferred-TJE
    *inputDataLen = BUFSIZ;
    *userInput = C_CAST(char*, safe_calloc(*inputDataLen, sizeof(char)));
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
#elif defined (__NetBSD__) && defined __NetBSD_Version__ && __NetBSD_Version__ >= 7000000000
    //Use getpass_r
    //this will dynamically allocate memory for use when the buffer is set to M_NULLPTR
    * userInput = getpass_r(prompt, M_NULLPTR, 0);
    if ((*userInput) == M_NULLPTR)
    {
        ret = FAILURE;
    }
    else
    {
        *inputDataLen = strlen(*userInput) + 1;//add one since this adds to the buffer size and that is what we are returning in all other cases-TJE
    }
#elif defined (__sun) && defined(__SunOS_5_6)
    //use getpassphrase since it can return longer passwords than getpass
    char* eraseme = getpassphrase(prompt);
    if (eraseme)
    {
        *userInput = strdup(eraseme);
        //immediately clear the original buffer to make sure it cannot be accessed again
        explicit_zeroes(eraseme, strlen(eraseme));
        if (*userInput == M_NULLPTR)
        {
            ret = FAILURE;
        }
        else
        {
            *inputDataLen = strlen(*userInput) + 1;//add one since this adds to the buffer size and that is what we are returning in all other cases-TJE
        }
    }
    else
    {
        ret = FAILURE;
    }
#else //POSIX & OS MACRO CHECKS
    //Last resort is to use getpass. This is the least desirable thing to use which is why it only gets used in this else case as a backup in case the OS we are supporting doesn't support any other available method.-TJE
    char* eraseme = getpass(prompt);
    if (eraseme)
    {
        *userInput = strdup(eraseme);
        //immediately clear the original buffer to make sure it cannot be accessed again
        explicit_zeroes(eraseme, strlen(eraseme));
        if (*userInput == M_NULLPTR)
        {
            ret = FAILURE;
        }
        else
        {
            *inputDataLen = strlen(*userInput) + 1;//add one since this adds to the buffer size and that is what we are returning in all other cases-TJE
        }
    }
    else
    {
        ret = FAILURE;
    }
#endif //POSIX & OS MACRO CHECKS
    return ret;
}
#endif

//Should these units be broken up into different types and the allowed type has to be passed in?
static bool is_Allowed_Unit_For_Get_And_Validate_Input(const char* unit, eAllowedUnitInput unittype)
{
    bool allowed = false;
    if (unit)
    {
        switch (unittype)
        {
        case ALLOW_UNIT_NONE:
            allowed = false;
            break;
        case ALLOW_UNIT_DATASIZE:
            //allowed units must match exactly at the end of the string!
            if (strcmp(unit, "B") == 0
                || strcmp(unit, "KB") == 0
                || strcmp(unit, "KiB") == 0
                || strcmp(unit, "MB") == 0
                || strcmp(unit, "MiB") == 0
                || strcmp(unit, "GB") == 0
                || strcmp(unit, "GiB") == 0
                || strcmp(unit, "TB") == 0
                || strcmp(unit, "TiB") == 0
                || strcmp(unit, "BLOCKS") == 0
                || strcmp(unit, "SECTORS") == 0
                )
            {
                allowed = true;
            }
            break;
        case ALLOW_UNIT_SECTOR_TYPE:
            if (strcmp(unit, "l") == 0 //used by some utilities to indicate a count is in logical sectors instead of physical sectors
                || strcmp(unit, "p") == 0
                || strcmp(unit, "logical") == 0
                || strcmp(unit, "physical") == 0
                )
            {
                allowed = true;
            }
            break;
        case ALLOW_UNIT_TIME:
            if (strcmp(unit, "ns") == 0 //nanoseconds
                || strcmp(unit, "us") == 0 //microseconds
                || strcmp(unit, "ms") == 0 //milliseconds
                || strcmp(unit, "s") == 0 //seconds
                || strcmp(unit, "m") == 0 //minutes
                || strcmp(unit, "h") == 0 //hours
                )
            {
                allowed = true;
            }
            break;
        case ALLOW_UNIT_POWER:
            if (strcmp(unit, "w") == 0 //watts
                || strcmp(unit, "mw") == 0 //milliwatts
                )
            {
                allowed = true;
            }
            break;
        case ALLOW_UNIT_VOLTS:
            if (strcmp(unit, "v") == 0 //volts
                || strcmp(unit, "mv") == 0 //millivolts
                )
            {
                allowed = true;
            }
            break;
        case ALLOW_UNIT_AMPS:
            if (strcmp(unit, "a") == 0 //amps
                || strcmp(unit, "ma") == 0 //milliamps
                )
            {
                allowed = true;
            }
            break;
        case ALLOW_UNIT_TEMPERATURE:
            if (strcmp(unit, "c") == 0 //celsius
                || strcmp(unit, "f") == 0 //fahrenheit
                || strcmp(unit, "k") == 0 //kelvin
                )
            {
                allowed = true;
            }
            break;
        }
    }
    return allowed;
}

bool get_And_Validate_Integer_Input_ULL(const char* strToConvert, char** unit, eAllowedUnitInput unittype, unsigned long long* outputInteger)
{
    if (strToConvert && outputInteger)
    {
        bool ret = true;
        bool hex = false;
        const char* tmp = strToConvert;
        char* localstrend = M_NULLPTR;
        char** endPtrToUse = &localstrend;
        if (unit)
        {
            endPtrToUse = unit;
        }
        while (*tmp != '\0')
        {
            if ((!safe_isxdigit(*tmp)) && (*tmp != 'x') && (*tmp != 'h'))
            {
                ret = false;
                break;
            }
            else if (!safe_isdigit(*tmp))
            {
                hex = true;
            }
            tmp++;
        }
        if (!ret && unit)//only check for a valid unit if the caller provided an endptr to get the unit out for their use
        {
            //check if a valid unit is present to allow this to continue
            if (is_Allowed_Unit_For_Get_And_Validate_Input(tmp, unittype))
            {
                //This will only return true when this is at the end of a string
                //meaning the user provided 64KB or something like that, so this matched to KB
                //This allows for the utilities calling this to multiply the output integer into a value that makes sense
                ret = true;
            }
        }
        //If everything is a valid hex digit. 
        if (ret)
        {
            errno = 0;//ISO secure coding standard recommends this to ensure errno is interpretted correctly after this call
            if (hex)
            {
                *outputInteger = strtoull(strToConvert, endPtrToUse, 16);
            }
            else
            {
                *outputInteger = strtoull(strToConvert, endPtrToUse, 10);
            }
        }
        else
        {
            ret = false;
        }
        //Final Check
        if (ret && ((*outputInteger == ULLONG_MAX && errno == ERANGE) || (strToConvert == *endPtrToUse && *outputInteger == 0)))
        {
            ret = false;
        }
        return ret;
    }
    else
    {
        return false;
    }
}

bool get_And_Validate_Integer_Input_UL(const char* strToConvert, char** unit, eAllowedUnitInput unittype, unsigned long* outputInteger)
{
    if (strToConvert && outputInteger)
    {
        bool ret = true;
        bool hex = false;
        const char* tmp = strToConvert;
        char* localstrend = M_NULLPTR;
        char** endPtrToUse = &localstrend;
        if (unit)
        {
            endPtrToUse = unit;
        }
        while (*tmp != '\0')
        {
            if ((!safe_isxdigit(*tmp)) && (*tmp != 'x') && (*tmp != 'h'))
            {
                ret = false;
                break;
            }
            else if (!safe_isdigit(*tmp))
            {
                hex = true;
            }
            tmp++;
        }
        if (!ret && unit)//only check for a valid unit if the caller provided an endptr to get the unit out for their use
        {
            //check if a valid unit is present to allow this to continue
            if (is_Allowed_Unit_For_Get_And_Validate_Input(tmp, unittype))
            {
                //This will only return true when this is at the end of a string
                //meaning the user provided 64KB or something like that, so this matched to KB
                //This allows for the utilities calling this to multiply the output integer into a value that makes sense
                ret = true;
            }
        }
        //If everything is a valid hex digit. 
        if (ret)
        {
            errno = 0;//ISO secure coding standard recommends this to ensure errno is interpretted correctly after this call
            if (hex)
            {
                *outputInteger = strtoul(strToConvert, endPtrToUse, 16);
            }
            else
            {
                *outputInteger = strtoul(strToConvert, endPtrToUse, 10);
            }
        }
        else
        {
            ret = false;
        }
        //Final Check
        if (ret && (((*outputInteger == ULONG_MAX && errno == ERANGE) || (strToConvert == *endPtrToUse && *outputInteger == 0))))
        {
            ret = false;
        }
        return ret;
    }
    else
    {
        return false;
    }
}

bool get_And_Validate_Integer_Input_UI(const char* strToConvert, char** unit, eAllowedUnitInput unittype, unsigned int* outputInteger)
{
    unsigned long temp = 0;
    bool ret = get_And_Validate_Integer_Input_UL(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined (UINT_MAX) && defined (ULONG_MAX) && ULONG_MAX > UINT_MAX
        if (temp > UINT_MAX)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = UINT_MAX;
        }
        else
#endif //ULONG_MAX !> UINT_MAX
        {
            *outputInteger = C_CAST(unsigned int, temp);
        }
    }
    return ret;
}
bool get_And_Validate_Integer_Input_US(const char* strToConvert, char** unit, eAllowedUnitInput unittype, unsigned short* outputInteger)
{
    unsigned long temp = 0;
    bool ret = get_And_Validate_Integer_Input_UL(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined (USHORT_MAX) && defined (ULONG_MAX) && ULONG_MAX > USHORT_MAX
        if (temp > USHORT_MAX)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = USHORT_MAX;
        }
        else
#endif //ULONG_MAX !> USHORT_MAX
        {
            *outputInteger = C_CAST(unsigned short, temp);
        }
    }
    return ret;
}

bool get_And_Validate_Integer_Input_UC(const char* strToConvert, char** unit, eAllowedUnitInput unittype, unsigned char* outputInteger)
{
    unsigned long temp = 0;
    bool ret = get_And_Validate_Integer_Input_UL(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined (UCHAR_MAX) && defined (ULONG_MAX) && ULONG_MAX > UCHAR_MAX
        if (temp > UCHAR_MAX)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = UCHAR_MAX;
        }
        else
#endif //ULONG_MAX !> UCHAR_MAX
        {
            *outputInteger = C_CAST(unsigned char, temp);
        }
    }
    return ret;
}

bool get_And_Validate_Integer_Input_LL(const char* strToConvert, char** unit, eAllowedUnitInput unittype, long long* outputInteger)
{
    if (strToConvert && outputInteger)
    {
        bool ret = true;
        bool hex = false;
        const char* tmp = strToConvert;
        char* localstrend = M_NULLPTR;
        char** endPtrToUse = &localstrend;
        if (unit)
        {
            endPtrToUse = unit;
        }
        while (*tmp != '\0')
        {

            if ((!safe_isxdigit(*tmp)) && (*tmp != 'x') && (*tmp != 'h'))
            {
                ret = false;
                break;
            }
            else if (!safe_isdigit(*tmp))
            {
                hex = true;
            }
            tmp++;
        }
        if (!ret && unit)//only check for a valid unit if the caller provided an endptr to get the unit out for their use
        {
            //check if a valid unit is present to allow this to continue
            if (is_Allowed_Unit_For_Get_And_Validate_Input(tmp, unittype))
            {
                //This will only return true when this is at the end of a string
                //meaning the user provided 64KB or something like that, so this matched to KB
                //This allows for the utilities calling this to multiply the output integer into a value that makes sense
                ret = true;
            }
        }
        //If everything is a valid hex digit. 
        if (ret)
        {
            errno = 0;//ISO secure coding standard recommends this to ensure errno is interpretted correctly after this call
            if (hex)
            {
                *outputInteger = strtoll(strToConvert, endPtrToUse, 16);
            }
            else
            {
                *outputInteger = strtoll(strToConvert, endPtrToUse, 10);
            }
        }
        else
        {
            ret = false;
        }
        //Final Check
        if (ret && (((*outputInteger == LLONG_MAX || *outputInteger == LLONG_MIN) && errno == ERANGE) || (strToConvert == *endPtrToUse && *outputInteger == 0)))
        {
            ret = false;
        }
        return ret;
    }
    else
    {
        return false;
    }
}

bool get_And_Validate_Integer_Input_L(const char* strToConvert, char** unit, eAllowedUnitInput unittype, long* outputInteger)
{
    if (strToConvert && outputInteger)
    {
        bool ret = true;
        bool hex = false;
        const char* tmp = strToConvert;
        char* localstrend = M_NULLPTR;
        char** endPtrToUse = &localstrend;
        if (unit)
        {
            endPtrToUse = unit;
        }
        while (*tmp != '\0')
        {
            if ((!safe_isxdigit(*tmp)) && (*tmp != 'x') && (*tmp != 'h'))
            {
                ret = false;
                break;
            }
            else if (!safe_isdigit(*tmp))
            {
                hex = true;
            }
            tmp++;
        }
        if (!ret && unit)//only check for a valid unit if the caller provided an endptr to get the unit out for their use
        {
            //check if a valid unit is present to allow this to continue
            if (is_Allowed_Unit_For_Get_And_Validate_Input(tmp, unittype))
            {
                //This will only return true when this is at the end of a string
                //meaning the user provided 64KB or something like that, so this matched to KB
                //This allows for the utilities calling this to multiply the output integer into a value that makes sense
                ret = true;
            }
        }
        //If everything is a valid hex digit. 
        if (ret)
        {
            errno = 0;//ISO secure coding standard recommends this to ensure errno is interpretted correctly after this call
            if (hex)
            {
                *outputInteger = strtol(strToConvert, endPtrToUse, 16);
            }
            else
            {
                *outputInteger = strtol(strToConvert, endPtrToUse, 10);
            }
        }
        else
        {
            ret = false;
        }
        //Final Check
        if (ret && (((*outputInteger == LONG_MAX || *outputInteger == LONG_MIN) && errno == ERANGE) || (strToConvert == *endPtrToUse && *outputInteger == 0)))
        {
            ret = false;
        }
        return ret;
    }
    else
    {
        return false;
    }
}

bool get_And_Validate_Integer_Input_I(const char* strToConvert, char** unit, eAllowedUnitInput unittype, int* outputInteger)
{
    long temp = 0;
    bool ret = get_And_Validate_Integer_Input_L(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined (INT_MAX) && defined (LONG_MAX) && defined (INT_MIN) && LONG_MAX > INT_MAX  && LONG_MIN < INT_MIN
        if (temp > INT_MAX)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = INT_MAX;
        }
        else if (temp < INT_MIN)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = INT_MIN;
        }
        else
#endif //ULONG_MAX !> INT_MAX
        {
            *outputInteger = C_CAST(short, temp);
        }
    }
    return ret;
}

bool get_And_Validate_Integer_Input_S(const char* strToConvert, char** unit, eAllowedUnitInput unittype, short* outputInteger)
{
    long temp = 0;
    bool ret = get_And_Validate_Integer_Input_L(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined (SHORT_MAX) && defined (LONG_MAX) && defined (SHORT_MIN) && LONG_MAX > SHORT_MAX  && LONG_MIN < SHORT_MIN
        if (temp > SHORT_MAX)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = SHORT_MAX;
        }
        else if (temp < SHORT_MIN)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = SHORT_MIN;
        }
        else
#endif //ULONG_MAX !> SHORT_MAX
        {
            *outputInteger = C_CAST(short, temp);
        }
    }
    return ret;
}

bool get_And_Validate_Integer_Input_C(const char* strToConvert, char** unit, eAllowedUnitInput unittype, char* outputInteger)
{
    long temp = 0;
    bool ret = get_And_Validate_Integer_Input_L(strToConvert, unit, unittype, &temp);
    if (ret)
    {
#if defined (CHAR_MAX) && defined (LONG_MAX) && defined (CHAR_MIN) && LONG_MAX > CHAR_MAX  && LONG_MIN < CHAR_MIN
        if (temp > CHAR_MAX)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = CHAR_MAX;
        }
        else if (temp < CHAR_MIN)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = CHAR_MIN;
        }
        else
#endif //ULONG_MAX !> CHAR_MAX
        {
            *outputInteger = C_CAST(char, temp);
        }
    }
    return ret;
}

bool get_And_Validate_Integer_Input_Uint64(const char* strToConvert, char** unit, eAllowedUnitInput unittype, uint64_t* outputInteger)
{
#if defined(USING_C11) && defined (get_Valid_Integer_Input)
    //let the generic selection macro do this
    return get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger);
#elif defined (LP64_DATA_MODEL) || defined (ILP64_DATA_MODEL)
    unsigned long temp = 0;
    bool ret = get_And_Validate_Integer_Input_UL(strToConvert, unit, unittype, &temp);
    if (ret && temp > UINT64_MAX)
    {
        ret = false;
        errno = ERANGE;
        *outputInteger = UINT64_MAX;
    }
    else
    {
        *outputInteger = C_CAST(uint64_t, temp);
    }
    return ret;
#else
    unsigned long long temp = 0;
    bool ret = get_And_Validate_Integer_Input_ULL(strToConvert, unit, unittype, &temp);
    if (ret && temp > UINT64_MAX)
    {
        ret = false;
        errno = ERANGE;
        *outputInteger = UINT64_MAX;
    }
    else
    {
        *outputInteger = C_CAST(uint64_t, temp);
    }
    return ret;
#endif
}

bool get_And_Validate_Float_Input(const char* strToConvert, char** unit, eAllowedUnitInput unittype, float* outputFloat)
{
    if (strToConvert && outputFloat)
    {
        char* localstrend = M_NULLPTR;
        char** endPtrToUse = &localstrend;
        if (unit)
        {
            endPtrToUse = unit;
        }
        errno = 0;//ISO secure coding standard recommends this to ensure errno is interpretted correctly after this call
        *outputFloat = strtof(strToConvert, endPtrToUse);
        if ((*outputFloat >= HUGE_VALF && errno == ERANGE) || strToConvert == *endPtrToUse)
        {
            return false;
        }
        else
        {
            bool unitallowed = is_Allowed_Unit_For_Get_And_Validate_Input(*endPtrToUse, unittype);
            if (unit)
            {
                if (unitallowed)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else if (!unit && unitallowed)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    else
    {
        return false;
    }
}

bool get_And_Validate_Double_Input(const char* strToConvert, char** unit, eAllowedUnitInput unittype, double* outputFloat)
{
    if (strToConvert && outputFloat)
    {
        char* localstrend = M_NULLPTR;
        char** endPtrToUse = &localstrend;
        if (unit)
        {
            endPtrToUse = unit;
        }
        errno = 0;//ISO secure coding standard recommends this to ensure errno is interpretted correctly after this call
        *outputFloat = strtod(strToConvert, endPtrToUse);
        if ((*outputFloat >= HUGE_VAL && errno == ERANGE) || strToConvert == *endPtrToUse)
        {
            return false;
        }
        else
        {
            bool unitallowed = is_Allowed_Unit_For_Get_And_Validate_Input(*endPtrToUse, unittype);
            if (unit)
            {
                if (unitallowed)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else if (!unit && unitallowed)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    else
    {
        return false;
    }
}

bool get_And_Validate_LDouble_Input(const char* strToConvert, char** unit, eAllowedUnitInput unittype, long double* outputFloat)
{
    if (strToConvert && outputFloat)
    {
        char* localstrend = M_NULLPTR;
        char** endPtrToUse = &localstrend;
        if (unit)
        {
            endPtrToUse = unit;
        }
        errno = 0;//ISO secure coding standard recommends this to ensure errno is interpretted correctly after this call
        *outputFloat = strtold(strToConvert, unit);
        if ((*outputFloat >= HUGE_VALL && errno == ERANGE) || strToConvert == *endPtrToUse)
        {
            return false;
        }
        else
        {
            bool unitallowed = is_Allowed_Unit_For_Get_And_Validate_Input(*endPtrToUse, unittype);
            if (unit)
            {
                if (unitallowed)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else if (!unit && unitallowed)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    else
    {
        return false;
    }
}

//NOTE: This function is deprecated as you should use the one that matches your integer type instead for best error handling.
bool get_And_Validate_Integer_Input(const char* strToConvert, uint64_t* outputInteger)
{
    return get_And_Validate_Integer_Input_Uint64(strToConvert, M_NULLPTR, ALLOW_UNIT_NONE, outputInteger);
}

bool get_And_Validate_Integer_Input_Uint32(const char* strToConvert, char** unit, eAllowedUnitInput unittype, uint32_t* outputInteger)
{
#if defined(USING_C11) && defined (get_Valid_Integer_Input)
    //let the generic selection macro do this
    return get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger);
#else
    unsigned long temp = 0;
    bool ret = get_And_Validate_Integer_Input_UL(strToConvert, unit, unittype, &temp);
    if (ret && temp > UINT32_MAX)
    {
        ret = false;
        errno = ERANGE;
        *outputInteger = UINT32_MAX;
    }
    else
    {
        *outputInteger = C_CAST(uint32_t, temp);
    }
    return ret;
#endif
}

bool get_And_Validate_Integer_Input_Uint16(const char* strToConvert, char** unit, eAllowedUnitInput unittype, uint16_t* outputInteger)
{
    if (strToConvert && outputInteger)
    {
        uint32_t temp = 0;
        bool ret = get_And_Validate_Integer_Input_Uint32(strToConvert, unit, unittype, &temp);
        if (ret && temp > UINT16_MAX)
        {
            ret = false;
            errno = ERANGE;//manually set a range error since this is outside of what is expected for this function
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
}

bool get_And_Validate_Integer_Input_Uint8(const char* strToConvert, char** unit, eAllowedUnitInput unittype, uint8_t* outputInteger)
{
    if (strToConvert && outputInteger)
    {
        uint32_t temp = 0;
        bool ret = get_And_Validate_Integer_Input_Uint32(strToConvert, unit, unittype, &temp);
        if (ret && temp > UINT8_MAX)
        {
            ret = false;
            errno = ERANGE;//manually set a range error since this is outside of what is expected for this function
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
}

bool get_And_Validate_Integer_Input_Int64(const char* strToConvert, char** unit, eAllowedUnitInput unittype, int64_t* outputInteger)
{
#if defined(USING_C11) && defined (get_Valid_Integer_Input)
    //let the generic selection macro do this
    return get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger);
#elif defined (LP64_DATA_MODEL) || defined (ILP64_DATA_MODEL)
    long temp = 0;
    bool ret = get_And_Validate_Integer_Input_L(strToConvert, unit, unittype, &temp);
    if (ret)
    {
        if (temp > INT64_MAX)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = INT64_MAX;
        }
        else if (temp < INT64_MIN)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = INT64_MIN;
        }
        else
        {
            *outputInteger = C_CAST(int64_t, temp);
        }
    }
    return ret;
#else
    long long temp = 0;
    bool ret = get_And_Validate_Integer_Input_LL(strToConvert, unit, unittype, &temp);
    if (ret)
    {
        if (temp > INT64_MAX)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = INT64_MAX;
        }
        else if (temp < INT64_MIN)
        {
            ret = false;
            errno = ERANGE;
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

bool get_And_Validate_Integer_Input_Int32(const char* strToConvert, char** unit, eAllowedUnitInput unittype, int32_t* outputInteger)
{
#if defined(USING_C11) && defined (get_Valid_Integer_Input)
    //let the generic selection macro do this
    return get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger);
#else
    long temp = 0;
    bool ret = get_And_Validate_Integer_Input_L(strToConvert, unit, unittype, &temp);
    if (ret)
    {
        if (temp > INT32_MAX)
        {
            ret = false;
            errno = ERANGE;
            *outputInteger = INT32_MAX;
        }
        else if (temp < INT32_MIN)
        {
            ret = false;
            errno = ERANGE;
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

bool get_And_Validate_Integer_Input_Int16(const char* strToConvert, char** unit, eAllowedUnitInput unittype, int16_t* outputInteger)
{
    if (strToConvert && outputInteger)
    {
        int32_t temp = 0;
        bool ret = get_And_Validate_Integer_Input_Int32(strToConvert, unit, unittype, &temp);
        if (ret && (temp > INT16_MAX || temp < INT16_MIN))
        {
            ret = false;
            errno = ERANGE;//manually set a range error since this is outside of what is expected for this function
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
}

bool get_And_Validate_Integer_Input_Int8(const char* strToConvert, char** unit, eAllowedUnitInput unittype, int8_t* outputInteger)
{
    if (strToConvert && outputInteger)
    {
        int32_t temp = 0;
        bool ret = get_And_Validate_Integer_Input_Int32(strToConvert, unit, unittype, &temp);
        if (ret && (temp > INT8_MAX || temp < INT8_MIN))
        {
            ret = false;
            errno = ERANGE;//manually set a range error since this is outside of what is expected for this function
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
}

#if !defined (__STDC_ALLOC_LIB__) && !defined (POSIX_2008) && !defined (USING_C23)
//getdelim and getline are not available, so define them ourselves for our own use

ssize_t getdelim(char** M_RESTRICT lineptr, size_t* M_RESTRICT n, int delimiter, FILE* stream)
{
    char* currentptr = M_NULLPTR;
    char* endptr = M_NULLPTR;
    if (lineptr == M_NULLPTR || n == M_NULLPTR || stream == M_NULLPTR)
    {
        errno = EINVAL;
        return -1;
    }
    if (*lineptr == M_NULLPTR || *n == 0)
    {
        *n = BUFSIZ;
        *lineptr = C_CAST(char*, malloc(*n));
        if (M_NULLPTR == *lineptr)
        {
            errno = ENOMEM;
            return -1;
        }
    }
    currentptr = *lineptr;
    endptr = *lineptr + *n;
    //read using fgetc until delimiter is encountered in the stream or end of the stream is reached
    do
    {
        int c = fgetc(stream);
        if (c == EOF)
        {
            //hit end of the stream.
            if (feof(stream))
            {
                ssize_t numchars = C_CAST(ssize_t, C_CAST(intptr_t, currentptr) - C_CAST(intptr_t, *lineptr));
                if (numchars > 0)
                {
                    //read all the characters in the stream to the end.
                    //set M_NULLPTR terminator and return how many chars were written
                    currentptr += 1;
                    *currentptr = '\0';
                    return numchars;
                }
            }
            //errno value???
            return -1;
        }
        //add to the stream
        *currentptr = C_CAST(char, c);//This cast is necessary to stop a warning. C is an int so that EOF can be defined.
        currentptr += 1;
        //check if we got the delimiter
        if (c == delimiter)
        {
            *currentptr = '\0';
            //calculate how may characters were read and return that value
            return C_CAST(ssize_t, C_CAST(intptr_t, currentptr) - C_CAST(intptr_t, *lineptr));
        }
        //check if writing next two characters will overflow the buffer (next char + null terminator if needed)
        if (C_CAST(intptr_t, currentptr) + 2 >= C_CAST(intptr_t, endptr))
        {
            //reallocate. Simple method is to double the current buffer size
            char* temp = M_NULLPTR;
            size_t newsize = *n * 2;
            ssize_t numchars = C_CAST(ssize_t, C_CAST(intptr_t, currentptr) - C_CAST(intptr_t, *lineptr));
#if defined (SSIZE_MAX)
            if (newsize > SSIZE_MAX)
            {
                errno = EOVERFLOW;
                return -1;
            }
#endif //SSIZE_MAX
            temp = safe_reallocf(C_CAST(void**, lineptr), newsize);
            if (temp == M_NULLPTR)
            {
                errno = ENOMEM;
                return -1;
            }
            *lineptr = temp;
            *n = newsize;
            currentptr = *lineptr + numchars;
            endptr = *lineptr + *n;
        }
    } while (1);
}

ssize_t getline(char** lineptr, size_t* n, FILE* stream)
{
    return getdelim(lineptr, n, '\n', stream);
}

#endif //__STDC_ALLOC_LIB__

void print_Return_Enum(const char* funcName, eReturnValues ret)
{
    if (M_NULLPTR == funcName)
    {
        printf("Unknown funtion returning: ");
    }
    else
    {
        printf("%s returning: ", funcName);
    }

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
    case UNKNOWN:
        printf("UNKNOWN\n");
        break;
        //NO DEFAULT CASE! This will cause warnings when an enum value is not in this switch-case so that it is never out of date!
    }
    printf("\n");
}

static void internal_Print_Data_Buffer(uint8_t* dataBuffer, uint32_t bufferLen, bool showPrint, bool showOffset)
{
    uint32_t printIter = 0, offset = 0;
    uint32_t offsetWidth = 2;//used to figure out how wide we need to pad with 0's for consistent output, 2 is the minimum width
    printf("data buffer length = %" PRIu32 "\n", bufferLen);
    if (showOffset)
    {
        if (bufferLen <= UINT8_MAX)
        {
            offsetWidth = 2;
            printf("\n        "); //0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ");
        }
        else if (bufferLen <= UINT16_MAX)
        {
            offsetWidth = 4;
            printf("\n          "); //0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ");
        }
        else if (bufferLen <= 0xFFFFFF)
        {
            offsetWidth = 6;
            printf("\n            "); //0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ");
        }
        else//32bit width, don't care about 64bit since max size if 32bit
        {
            offsetWidth = 8;
            printf("\n              "); //0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ");
        }
        //we print out 2 (0x) + printf formatting width + 2 (spaces) then the offsets

        for (printIter = 0; printIter < 16 && printIter < bufferLen; printIter++)
        {
            printf("%" PRIX32 "  ", printIter);
        }
    }
    DECLARE_ZERO_INIT_ARRAY(char, lineBuff, 18);
    uint8_t lineBuffIter = 0;
    for (printIter = 0, offset = 0; printIter < bufferLen; ++printIter, ++lineBuffIter)
    {
        if (lineBuffIter > sizeof(lineBuff))
        {
            lineBuffIter = 0;
        }

        //for every 16 bytes we print, we need to make a newline, then print the offset (hex) before we print the data again
        if (printIter % 16 == 0)
        {
            if (showOffset)
            {
                switch (offsetWidth)
                {
                case 4:
                    printf("\n  0x%04"PRIX32" ", offset);
                    break;
                case 6:
                    printf("\n  0x%06"PRIX32" ", offset);
                    break;
                case 8:
                    printf("\n  0x%08"PRIX32" ", offset);
                    break;
                case 2:
                default:
                    printf("\n  0x%02"PRIX32" ", offset);
                    break;
                }
            }
            else
            {
                printf("\n  ");
            }
            offset += 16;
        }
        printf("%02"PRIX8" ", dataBuffer[printIter]);
        if (showPrint)
        {
            if (safe_isascii(dataBuffer[printIter]) && safe_isprint(C_CAST(int, dataBuffer[printIter])))
            {
                lineBuff[lineBuffIter] = C_CAST(char, dataBuffer[printIter]);
            }
            else
            {
                lineBuff[lineBuffIter] = '.';
            }
        }
        if (showPrint && ((printIter + 1) % 16 == 0 || printIter + 1 == bufferLen))
        {
            lineBuff[17] = '\0';
            lineBuffIter = UINT8_MAX;//this is done to cause an overflow when the ++happens during the loop
            //need to calculate if padding is needed before printing character translations...but how...
            uint32_t spacePadding = (printIter + 1) % 16;
            if (spacePadding)
            {
                uint32_t counter = 0;
                while (counter < ((16 - spacePadding)))
                {
                    printf("   ");
                    counter++;
                }
            }
            //space after last printed hex 
            printf("  ");
            printf("%s", lineBuff);
            memset(lineBuff, 0, sizeof(lineBuff) / sizeof(*lineBuff));
        }
    }
    printf("\n\n");
}

void print_Data_Buffer(uint8_t* dataBuffer, uint32_t bufferLen, bool showPrint)
{
    internal_Print_Data_Buffer(dataBuffer, bufferLen, showPrint, true);
}

void print_Pipe_Data(uint8_t* dataBuffer, uint32_t bufferLen)
{
    internal_Print_Data_Buffer(dataBuffer, bufferLen, false, false);
}
