//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012-2021 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file common_windows.c
// \brief Implements functions that are common to MSFT Windows.
//
#include "common.h"
#include "common_windows.h"
#include <windows.h> //needed for all the stuff to get the windows version
#include <tchar.h>
#include <strsafe.h> //needed in the code written to get the windows version since I'm using a Microsoft provided string concatenation call-tje
#include <io.h> //needed for getting the size of a file in windows
#include <lmcons.h> //for UNLEN

bool os_Directory_Exists(const char * const pathToCheck)
{
    DWORD attrib = INVALID_FILE_ATTRIBUTES;
    size_t pathCheckLength = (strlen(pathToCheck) + 1) * sizeof(TCHAR);
    TCHAR *localPathToCheckBuf = C_CAST(TCHAR*, calloc(pathCheckLength, sizeof(TCHAR)));
    if (!localPathToCheckBuf)
    {
        return false;
    }
    CONST TCHAR *localPathToCheck = &localPathToCheckBuf[0];
    _stprintf_s(localPathToCheckBuf, pathCheckLength, TEXT("%hs"), pathToCheck);

    attrib = GetFileAttributes(localPathToCheck);

    safe_Free(localPathToCheckBuf)
    localPathToCheck = NULL;

    if (attrib == INVALID_FILE_ATTRIBUTES)
    {
        //printf("getlasterror = %d(0x%x)\n", GetLastError(), GetLastError());
        return false;
    }
    else if (attrib & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool os_File_Exists(const char * const filetoCheck)
{
    DWORD attrib = INVALID_FILE_ATTRIBUTES;
    size_t fileCheckLength = (strlen(filetoCheck) + 1) * sizeof(TCHAR);
    TCHAR *localFileToCheckBuf = C_CAST(TCHAR*, calloc(fileCheckLength, sizeof(TCHAR)));
    if (!localFileToCheckBuf)
    {
        return false;
    }
    CONST TCHAR *localFileToCheck = &localFileToCheckBuf[0];
    _stprintf_s(localFileToCheckBuf, fileCheckLength, TEXT("%hs"), filetoCheck);

    attrib = GetFileAttributes(localFileToCheck);

    safe_Free(localFileToCheckBuf)
    localFileToCheck = NULL;

    if (attrib == INVALID_FILE_ATTRIBUTES)
    {
        //printf("getlasterror = %d(0x%x)\n", GetLastError(), GetLastError());
        return false;
    }
    else if (attrib & FILE_ATTRIBUTE_DIRECTORY)
    {
        return false; //It's a directory, not a file. 
    }
    else
    {
        return true;
    }
}

int get_Full_Path(const char * pathAndFile, char fullPath[OPENSEA_PATH_MAX])
{
    if (!pathAndFile || !fullPath)
    {
        return BAD_PARAMETER;
    }
    //TODO:?Use macro to work with the wide charcter version?
    DWORD result = GetFullPathNameA(pathAndFile, OPENSEA_PATH_MAX, fullPath, NULL);
    if (result == 0)
    {
        //fatal error
        return FAILURE;
    }
    else if (result > OPENSEA_PATH_MAX)
    {
        //This case is when the function returns a buffer size that is required to hold the full path name.
        //DWORD lastError = GetLastError();
        //TODO: change things based on error code?
        return FAILURE;
    }

    return SUCCESS;
}

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

void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    static bool defaultsSet = false;
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD theColor = 0;
    if (!defaultsSet)
    {
        //First time we are setting colors backup the default settings so they can be restored properly later.
        get_Console_Default_Color();
        defaultsSet = true;
    }
    if (foregroundBackground)//change foreground color
    {
        switch (consoleColor)
        {
        case DARK_BLUE:
            theColor = FOREGROUND_BLUE;
            break;
        case BLUE:
            theColor = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case DARK_GREEN:
            theColor = FOREGROUND_GREEN;
            break;
        case GREEN:
            theColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case DARK_RED:
            theColor = FOREGROUND_RED;
            break;
        case RED:
            theColor = FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case BLACK:
            theColor = 0;//this should mean no colors or black
            break;
        case BROWN:
            theColor = FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        case YELLOW:
            theColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case TEAL:
            theColor = FOREGROUND_BLUE | FOREGROUND_GREEN;
            break;
        case CYAN:
            theColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case PURPLE:
            theColor = FOREGROUND_BLUE | FOREGROUND_RED;
            break;
        case MAGENTA:
            theColor = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case WHITE:
            theColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case DARK_GRAY:
            theColor = FOREGROUND_INTENSITY;
            break;
        case GRAY:
            theColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
            break;
        case DEFAULT://fall through to default
        default:
            //theColor = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
            theColor = get_Console_Default_Color();
        }
        SetConsoleTextAttribute(consoleHandle, theColor);
    }
    else//change background color
    {
        switch (consoleColor)
        {
        case DARK_BLUE:
            theColor = BACKGROUND_BLUE;
            break;
        case BLUE:
            theColor = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
            break;
        case DARK_GREEN:
            theColor = BACKGROUND_GREEN;
            break;
        case GREEN:
            theColor = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case DARK_RED:
            theColor = BACKGROUND_RED;
            break;
        case RED:
            theColor = BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case BLACK:
            theColor = 0;//this should mean no colors or black
            break;
        case BROWN:
            theColor = BACKGROUND_RED | BACKGROUND_GREEN;
            break;
        case YELLOW:
            theColor = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case TEAL:
            theColor = BACKGROUND_BLUE | BACKGROUND_GREEN;
            break;
        case CYAN:
            theColor = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case PURPLE:
            theColor = BACKGROUND_BLUE | BACKGROUND_RED;
            break;
        case MAGENTA:
            theColor = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case WHITE:
            theColor = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case DARK_GRAY:
            theColor = BACKGROUND_INTENSITY;
            break;
        case GRAY:
            theColor = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
            break;
        case DEFAULT://fall through to default
        default:
            //theColor = 0;//black background
            //theColor |= FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;//white foreground
            theColor = get_Console_Default_Color();
        }
        SetConsoleTextAttribute(consoleHandle, theColor);
    }
}

eArchitecture get_Compiled_Architecture(void)
{
    //check which compiler we're using to use it's preprocessor definitions
    //intel compiler and visual c seem to use the same preprocessor definitions
    #if defined _MSC_VER || defined __INTEL_COMPILER
        #if defined _M_X64 || defined _M_AMD64
            return OPENSEA_ARCH_X86_64;
        #elif defined _M_ALPHA
            return OPENSEA_ARCH_ALPHA;
        #elif defined _M_ARM || defined _M_ARMT
            return OPENSEA_ARCH_ARM;
        #elif defined _M_IX86
            return OPENSEA_ARCH_X86;
        #elif defined _M_IA64
            return OPENSEA_ARCH_IA_64;
        #elif defined _M_PPC //32bits I'm guessing - TJE
            return OPENSEA_ARCH_POWERPC;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #elif defined __MINGW32__ || defined __MINGW64__ || defined __clang__ || defined __GNUC__ //I'm guessing that all these compilers will use the same macro definitions since the sourceforge pages aren't 100% complete (clang I'm unsure about)
        #if defined __alpha__
            return OPENSEA_ARCH_ALPHA;
        #elif defined __amd64__ || defined __x86_64__
            return OPENSEA_ARCH_X86_64;
        #elif defined __arm__ || defined __thumb__
            return OPENSEA_ARCH_ARM;
        #elif defined __aarch64__
            return OPENSEA_ARCH_ARM64;
        #elif defined __i386__ || defined __i486__ || defined __i586__ || defined __i686__
            return OPENSEA_ARCH_X86;
        #elif defined __ia64__ || defined __IA64__
            return OPENSEA_ARCH_IA_64;
        #elif defined __powerpc64__ || defined __PPC64__ || defined __ppc64__ || defined _ARCH_PPC64
            return OPENSEA_ARCH_POWERPC64;
        #elif defined __powerpc__ || defined __POWERPC__ || defined __PPC__ || defined __ppc__ || defined _ARCH_PPC
            return OPENSEA_ARCH_POWERPC;
        #elif defined __sparc__
            return OPENSEA_ARCH_SPARC;
        #elif defined __MINGW32__ && defined _X86_
            return OPENSEA_ARCH_X86;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #else
        return OPENSEA_ARCH_UNKNOWN;
    #endif
}

eEndianness calculate_Endianness(void)
{
    static eEndianness endian = OPENSEA_UNKNOWN_ENDIAN;//using static so that it should only need to run this code once...not that it takes a long time, but this may help optimise this.
    if (endian == OPENSEA_UNKNOWN_ENDIAN)
    {
        union
        {
            uint32_t value;
            uint8_t data[sizeof(uint32_t)];
        } number;

        number.data[0] = 0x00;
        number.data[1] = 0x01;
        number.data[2] = 0x02;
        number.data[3] = 0x03;

        switch (number.value)
        {
        case UINT32_C(0x00010203):
            endian = OPENSEA_BIG_ENDIAN;
            break;
        case UINT32_C(0x03020100):
            endian = OPENSEA_LITTLE_ENDIAN;
            break;
        case UINT32_C(0x02030001):
            endian = OPENSEA_BIG_WORD_ENDIAN;
            break;
        case UINT32_C(0x01000302):
            endian = OPENSEA_LITTLE_WORD_ENDIAN;
            break;
        default:
            endian = OPENSEA_UNKNOWN_ENDIAN;
            break;
        }
    }
    return endian;
}

//from what I can tell, windows basically only ever runs on little endian, but this complete check SHOULD MOSTLY work
eEndianness get_Compiled_Endianness(void)
{
    //check which compiler we're using to use it's preprocessor definitions
    //intel compiler and visual c seem to use the same preprocessor definitions
    #if defined _MSC_VER || defined __INTEL_COMPILER
        #if defined _M_X64 || defined _M_AMD64
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_ALPHA
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_ARM || defined _M_ARMT
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_IX86
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_IA64
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_PPC //32bits I'm guessing - TJE
            return OPENSEA_BIG_ENDIAN;
        #elif defined __LITTLE_ENDIAN__
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined __BIG_ENDIAN__
            return OPENSEA_BIG_ENDIAN;
        #else
            return calculate_Endianness();
        #endif
    #elif defined __MINGW32__ || defined __MINGW64__ || defined __clang__ || defined __GNUC__ //I'm guessing that all these compilers will use the same macro definitions since the sourceforge pages aren't 100% complete (clang I'm unsure about)
        #if defined (__BYTE_ORDER__)
            #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
                return OPENSEA_BIG_ENDIAN;
            #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
                return OPENSEA_LITTLE_ENDIAN;
            #else
                return calculate_Endianness();
            #endif
        #elif defined (__BIG_ENDIAN__)
            return OPENSEA_BIG_ENDIAN;
        #elif defined (__LITTLE_ENDIAN__)
            return OPENSEA_LITTLE_ENDIAN;
        #else
            //check architecture specific defines...
            #if defined (__ARMEB__) || defined (__THUMBEB__) || defined (__AARCH64EB__) || defined (_MIPSEB) || defined (__MIPSEB) || defined (__MIPSEB__)
                return OPENSEA_BIG_ENDIAN;
            #elif defined (__ARMEL__) || defined (__THUMBEL__) || defined (__AARCH64EL__) || defined (_MIPSEL) || defined (__MIPSEL) || defined (__MIPSEL__)
                return OPENSEA_LITTLE_ENDIAN;
            #else
                return calculate_Endianness();
            #endif
        #endif
    #endif
}

bool is_Windows_8_Or_Higher(void)
{
    bool isWindows8OrHigher = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 6;
    windowsVersionInfo.dwMinorVersion = 2;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows8OrHigher = true;
    }
    return isWindows8OrHigher;
}

bool is_Windows_8_One_Or_Higher(void)
{
    bool isWindows81OrHigher = false;
    //Will only work if app manifested correctly
    /*
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 6;
    windowsVersionInfo.dwMinorVersion = 3;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows81OrHigher = true;
    }
    /*/
    //This uses our method below that checks the version from kernel.dll
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion > 6)
    {
        //Win10 or higher
        isWindows81OrHigher = true;
    }
    else if (windowsVersion.versionType.windowsVersion.majorVersion == 6 && windowsVersion.versionType.windowsVersion.minorVersion >= 3)
    {
        isWindows81OrHigher = true;
    }
    //*/
    return isWindows81OrHigher;
}

bool is_Windows_10_Or_Higher(void)
{
    bool isWindows10OrHigher = false;
    //Will only work if app manifested correctly
    /*
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 10;
    windowsVersionInfo.dwMinorVersion = 0;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows10OrHigher = true;
    }
    /*/
    //This uses our method below that checks the version from kernel.dll
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10)
    {
        //Win10 or higher
        isWindows10OrHigher = true;
    }
    //*/
    return isWindows10OrHigher;
}

bool is_Windows_10_Version_1507_Or_Higher(void)
{
    bool isWindows10_1507OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 10240)
    {
        //Win10 or higher
        isWindows10_1507OrHigher = true;
    }
    return isWindows10_1507OrHigher;
}

bool is_Windows_10_Version_1511_Or_Higher(void)
{
    bool isWindows10_1511OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 10586)
    {
        //Win10 or higher
        isWindows10_1511OrHigher = true;
    }
    return isWindows10_1511OrHigher;
}

bool is_Windows_10_Version_1607_Or_Higher(void)
{
    bool isWindows10_1607OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 14393)
    {
        //Win10 or higher
        isWindows10_1607OrHigher = true;
    }
    return isWindows10_1607OrHigher;
}

bool is_Windows_10_Version_1703_Or_Higher(void)
{
    bool isWindows10_1703OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 15063)
    {
        //Win10 or higher
        isWindows10_1703OrHigher = true;
    }
    return isWindows10_1703OrHigher;
}

bool is_Windows_10_Version_1709_Or_Higher(void)
{
    bool isWindows10_1709OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 16299)
    {
        //Win10 or higher
        isWindows10_1709OrHigher = true;
    }
    return isWindows10_1709OrHigher;
}

bool is_Windows_10_Version_1803_Or_Higher(void)
{
    bool isWindows10_1803OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 17134)
    {
        //Win10 or higher
        isWindows10_1803OrHigher = true;
    }
    return isWindows10_1803OrHigher;
}

bool is_Windows_10_Version_1809_Or_Higher(void)
{
    bool isWindows10_1809OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 17763)
    {
        //Win10 or higher
        isWindows10_1809OrHigher = true;
    }
    return isWindows10_1809OrHigher;
}

bool is_Windows_10_Version_1903_Or_Higher(void)
{
    bool isWindows10_1903OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 18362)
    {
        //Win10 or higher
        isWindows10_1903OrHigher = true;
    }
    return isWindows10_1903OrHigher;
}

bool is_Windows_10_Version_1909_Or_Higher(void)
{
    bool isWindows10_1909OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 18363)
    {
        //Win10 or higher
        isWindows10_1909OrHigher = true;
    }
    return isWindows10_1909OrHigher;
}

bool is_Windows_10_Version_2004_Or_Higher(void)
{
    bool isWindows10_2004OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19041)
    {
        //Win10 or higher
        isWindows10_2004OrHigher = true;
    }
    return isWindows10_2004OrHigher;
}

bool is_Windows_10_Version_20H2_Or_Higher(void)
{
    bool isWindows10_20H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19042)
    {
        //Win10 or higher
        isWindows10_20H2OrHigher = true;
    }
    return isWindows10_20H2OrHigher;
}

bool is_Windows_10_Version_21H1_Or_Higher(void)
{
    bool isWindows10_21H1OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19043)
    {
        //Win10 or higher
        isWindows10_21H1OrHigher = true;
    }
    return isWindows10_21H1OrHigher;
}

bool is_Windows_10_Version_21H2_Or_Higher(void)
{
    bool isWindows10_21H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19044)
    {
        //Win10 or higher
        isWindows10_21H2OrHigher = true;
    }
    return isWindows10_21H2OrHigher;
}

bool is_Windows_11_Version_21H2_Or_Higher(void)
{
    bool isWindows10_21H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 22000)
    {
        //Win10 or higher
        isWindows10_21H2OrHigher = true;
    }
    return isWindows10_21H2OrHigher;
}

bool is_Windows_Server_OS(void)
{
    bool isWindowsServer = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    windowsVersionInfo.wProductType = VER_NT_WORKSTATION;
    conditionMask = VerSetConditionMask(conditionMask, VER_PRODUCT_TYPE, VER_EQUAL);//checking for equality on the workstation attribute
    if (VerifyVersionInfo(&windowsVersionInfo, VER_PRODUCT_TYPE, conditionMask))
    {
        //Windows workstation/desktop
        isWindowsServer = false;
    }
    else
    {
        //Windows server/domain controller (which is a windows server version)
        isWindowsServer = true;
    }
    return isWindowsServer;
}
//TODO: If this ever fasly detects PE, it may be better to require checking for multiple keys and making sure they are all found.
bool is_Windows_PE(void)
{
    bool isWindowsPE = false;
    //To figure out if running in PE requires checking the registry. There is not another known way to look for this.
    //All other functions to read or check versions will return build numbers that match the equivalent non-PE version of windows, which can still be useful for determining supported APIs
    //We will be looking for one of these keys:
    /*
    -HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\WinPE
    -HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\MiniNT
    -HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\MiniNT
    //NOTE: 3 below are not used today. Only these top two are checked at this time
    -HKEY_LOCAL_MACHINE\system\currentcontrolset\control\PEBootType (PE2)
    -HKEY_LOCAL_MACHINE\system\currentcontrolset\control\PEFirmwareType (PE4)
    -HKEY_LOCAL_MACHINE\System\Setup\SystemSetupInProgress
    */
    HKEY keyHandle;
    if (!isWindowsPE && ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WinPE"), 0, KEY_READ, &keyHandle))
    {
#if defined (_DEBUG)
        printf("Found HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WinPE\n");
#endif
        isWindowsPE = true;
        RegCloseKey(keyHandle);
        keyHandle = 0;
    }
    if (!isWindowsPE && ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\MiniNT"), 0, KEY_READ, &keyHandle))
    {
#if defined (_DEBUG)
        printf("Found HKLM\\SYSTEM\\CurrentControlSet\\Control\\MiniNT\n");
#endif
        isWindowsPE = true;
        RegCloseKey(keyHandle);
        keyHandle = 0;
    }
    if (!isWindowsPE && ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\ControlSet001\\Control\\MiniNT"), 0, KEY_READ, &keyHandle))
    {
#if defined (_DEBUG)
        printf("Found HKLM\\SYSTEM\\ControlSet001\\Control\\MiniNT\n");
#endif
        isWindowsPE = true;
        RegCloseKey(keyHandle);
        keyHandle = 0;
    }
    return isWindowsPE;
}

//possible MSDN api calls: 
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa370624(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/hardware/ff561910(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms724451(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms725492(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms724358(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx //Used this one since it is supposed to work!
//From MSDN: To obtain the full version number for the operating system, call the GetFileVersionInfo function on one of the system DLLs, such as Kernel32.dll, then call VerQueryValue to obtain the \\StringFileInfo\\<lang><codepage>\\ProductVersion subblock of the file version information.
int get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char *operatingSystemName)
{
    int ret = SUCCESS;
    bool isWindowsServer = is_Windows_Server_OS();
    bool isWindowsPE = is_Windows_PE();
    memset(versionNumber, 0, sizeof(OSVersionNumber));
    versionNumber->osVersioningIdentifier = OS_WINDOWS;
    //start getting the Windows version using the verifyVersionInfo call. I'm doing it this way since the "version helpers" are essentially doing the same thing but are not available to minGW
    
    static CONST TCHAR kernel32DLL[] = TEXT("\\kernel32.dll");
    TCHAR *systemPathBuf = C_CAST(TCHAR*, calloc(OPENSEA_PATH_MAX, sizeof(TCHAR)));
    CONST TCHAR *systemPath = &systemPathBuf[0];
    CONST TCHAR *subblock = TEXT(SYSTEM_PATH_SEPARATOR_STR);

    if(!systemPath)
    {
        return MEMORY_FAILURE;
    }

    UINT directoryStringLength = GetSystemDirectory(systemPathBuf, OPENSEA_PATH_MAX);
    if (directoryStringLength > OPENSEA_PATH_MAX || directoryStringLength == 0 || directoryStringLength > OPENSEA_PATH_MAX - sizeof(kernel32DLL) / sizeof(*kernel32DLL))
    {
        //error
        safe_Free(systemPathBuf)
        systemPath = NULL;
        return FAILURE;
    }
    //I'm using this Microsoft provided call to concatenate strings since it will concatenate properly for ansi or wide strings depending on whether UNICODE is set or not - TJE
    if (S_OK != StringCchCat(systemPathBuf, OPENSEA_PATH_MAX, kernel32DLL))
    {
        return FAILURE;
    }

    DWORD versionInfoSize = GetFileVersionInfoSize(systemPath, NULL);
    if (versionInfoSize > 0)
    {
        LPVOID ver = malloc(versionInfoSize);
        if (!ver)
        {
            safe_Free(systemPathBuf)
            systemPath = NULL;
            return MEMORY_FAILURE;
        }
        if (GetFileVersionInfo(systemPathBuf, 0, versionInfoSize, ver))
        {
            LPVOID block = NULL;
            UINT blockSize = sizeof(VS_FIXEDFILEINFO);//start with this size...should get at least this
            VS_FIXEDFILEINFO *versionFileInfo = NULL;
            if (VerQueryValue(C_CAST(LPCVOID, ver), subblock, &block, &blockSize) || blockSize < sizeof(VS_FIXEDFILEINFO))//this should run the first function before performing the comparison
            {
                versionFileInfo = C_CAST(VS_FIXEDFILEINFO*, block);
                versionNumber->versionType.windowsVersion.majorVersion = HIWORD(versionFileInfo->dwProductVersionMS);
                versionNumber->versionType.windowsVersion.minorVersion = LOWORD(versionFileInfo->dwProductVersionMS);
                versionNumber->versionType.windowsVersion.buildNumber = HIWORD(versionFileInfo->dwProductVersionLS);
            }
            else
            {
                //error
                ret = FAILURE;
            }
        }
        else
        {
            //error
            ret = FAILURE;
        }
        safe_Free(ver)
    }
    else
    {
        //error
        ret = FAILURE;
    }
    safe_Free(systemPathBuf)
    systemPath = NULL;

    if (ret == SUCCESS)
    {
        //Now that we know whether or not it's a server version and have gotten the version number, set the appropriate string for the OS.
        if (operatingSystemName)
        {
            switch (versionNumber->versionType.windowsVersion.majorVersion)
            {
            case 10://Win 10 or Server 2016
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 0:
                    if (isWindowsServer)
                    {
                        switch (versionNumber->versionType.windowsVersion.buildNumber)
                        {
                        case 17763:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2019");
                            break;
                        case 14393:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2016");
                            break;
                        case 18362:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 1903");
                            break;
                        case 18363:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 1909");
                            break;
                        case 19041:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 2004");
                            break;
                        case 19042:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 20H2");
                            break;
                        case 20348:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2022");
                            break;
                        default:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows Server Version");
                            break;
                        }
                    }
                    else
                    {
                        switch (versionNumber->versionType.windowsVersion.buildNumber)
                        {
                        case 10240:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1507");
                            break;
                        case 10586:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1511");
                            break;
                        case 14393:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1607");
                            break;
                        case 15063:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1703");
                            break;
                        case 16299:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1709");
                            break;
                        case 17134:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1803");
                            break;
                        case 17763:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1809");
                            break;
                        case 18362:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1903");
                            break;
                        case 18363:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1909");
                            break;
                        case 19041:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 2004");
                            break;
                        case 19042:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 20H2");
                            break;
                        case 19043:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 21H1");
                            break;
                        case 19044:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 21H2");
                            break;
                        case 22000:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 11, version 21H2");
                            break;
                        default:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows 10/11 version");
                            break;
                        }
                    }
                    break;
                default:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows Server Version");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows Version");
                    }
                    break;
                }
                break;
            case 6://Vista through 8.1
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 3:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2012 R2");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 8.1");
                    }
                    break;
                case 2:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2012");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 8");
                    }
                    break;
                case 1:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2008 R2");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 7");
                    }
                    break;
                case 0:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2008");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Vista");
                    }
                    break;
                default:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2012 R2 or higher");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 8.1 or higher");
                    }
                    break;
                }
                break;
            case 5://2000 through XP
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 2:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2003");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows XP 64-Bit Edition");
                    }
                    break;
                case 1:
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows XP");
                    break;
                case 0:
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 2000");
                    break;
                default:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2003 or higher");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "XP or higher");
                    }
                    break;
                }
                break;
            default:
                snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows OS");
                break;
            }
            if (isWindowsPE)
            {
                common_String_Concat(operatingSystemName, OS_NAME_SIZE, " (PE)");
            }
        }
    }
    return ret;
}

int64_t os_Get_File_Size(FILE *filePtr)
{
    LARGE_INTEGER fileSize;
    intptr_t osfHandle = _get_osfhandle(_fileno(filePtr));
    //convert the fileptr to a HANDLE
    HANDLE fileHandle = C_CAST(HANDLE, osfHandle);
    fileSize.QuadPart = 0;//set to something before we call GetFileSizeEx
    //use GetFileSizeEx to get the size of the file as a 64bit integer
    if (GetFileSizeEx(fileHandle, &fileSize))
    {
        return fileSize.QuadPart;
    }
    else
    {
        return -1;
    }
}

void start_Timer(seatimer_t *timer)
{
    LARGE_INTEGER tempLargeInt;
    tempLargeInt.QuadPart = 0;
    if (QueryPerformanceCounter(&tempLargeInt))//according to MSDN this will always return success on XP and later systems
    {
        timer->timerStart = tempLargeInt.QuadPart;
    }
}

void stop_Timer(seatimer_t *timer)
{
    LARGE_INTEGER tempLargeInt;
    memset(&tempLargeInt, 0, sizeof(LARGE_INTEGER));
    if (QueryPerformanceCounter(&tempLargeInt))//according to MSDN this will always return success on XP and later systems
    {
        timer->timerStop = tempLargeInt.QuadPart;
    }
}

uint64_t get_Nano_Seconds(seatimer_t timer)
{
    LARGE_INTEGER frequency;//clock ticks per second
    uint64_t ticksPerNanosecond = 1000000000;//start with a count of nanoseconds per second
    uint64_t seconds = 0, nanoSeconds = 0;
    memset(&frequency, 0, sizeof(LARGE_INTEGER));
    QueryPerformanceFrequency(&frequency);//should always return success
    ticksPerNanosecond /= frequency.QuadPart;
    seconds = (timer.timerStop - timer.timerStart) / frequency.QuadPart;//converted to nanoseconds later
    nanoSeconds = ((timer.timerStop - timer.timerStart) % frequency.QuadPart) * ticksPerNanosecond;
    return ((seconds * 1000000000) + nanoSeconds);
}

double get_Micro_Seconds(seatimer_t timer)
{
    uint64_t nanoseconds = get_Nano_Seconds(timer);
    return (C_CAST(double, nanoseconds) / 1000.00);
}

double get_Milli_Seconds(seatimer_t timer)
{
    return (get_Micro_Seconds(timer) / 1000.00);
}

double get_Seconds(seatimer_t timer)
{
    return (get_Milli_Seconds(timer) / 1000.00);
}

void print_Windows_Error_To_Screen(unsigned int windowsError)
{
    TCHAR *windowsErrorString = NULL;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, windowsError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (TCHAR*)&windowsErrorString, 0, NULL);
    _tprintf_s(TEXT("%u - %s\n"), windowsError, windowsErrorString);
    LocalFree(windowsErrorString);
}

bool is_Running_Elevated(void)
{
    bool isElevated = false;
    HANDLE currentProcess = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &currentProcess))
    {
        TOKEN_ELEVATION elevation;
        DWORD returnedSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(currentProcess, TokenElevation, &elevation, sizeof(elevation), &returnedSize))
        {
            if (elevation.TokenIsElevated)
            {
                isElevated = true;
            }
        }
    }
    if (currentProcess)
    {
        CloseHandle(currentProcess);
    }
    return isElevated;
}

//Gets the user name for who is running the process
//https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getusernamea?redirectedfrom=MSDN
//NOTE: Not using Ex version at this time to avoid linking yet another library. This can be added if necessary, or this doesn't do quite what we want it to do. -TJE
int get_Current_User_Name(char **userName)
{
    int ret = SUCCESS;
    if (userName)
    {
        DWORD localNameLength = UNLEN + 1;//start with this for input
        TCHAR localName[UNLEN + 1] = { 0 };
        if (TRUE == GetUserName(localName, &localNameLength))
        {
            const char *isAdmin = " (admin)";//This will be concatenated to the string if running as administrator since we only get the user's name in Windows.
            size_t usernameLength = _tcslen(localName) + strlen(isAdmin) + 1;
            *userName = C_CAST(char*, calloc(usernameLength, sizeof(char)));
            if (*userName)
            {
#if defined UNICODE
                size_t charsConverted = 0;
                //convert output to a char string
                if (wcstombs_s(&charsConverted, *userName, usernameLength, localName, usernameLength))
                {
                    safe_Free(*userName)
                    ret = FAILURE;
                }
#else
                //just copy it over after allocating
                if (strcpy_s(*userName, usernameLength, localName))
                {
                    safe_Free(*userName)
                    return FAILURE;
                }
#endif
                if (is_Running_Elevated())
                {
                    if (strcat_s(*userName, usernameLength, isAdmin))
                    {
                        safe_Free(*userName)
                        return FAILURE;
                    }
                }
            }
            else
            {
                ret = FAILURE;
            }
        }
        else
        {
            ret = FAILURE;
        }
    }
    else
    {
        ret = BAD_PARAMETER;
    }
    return ret;
}
