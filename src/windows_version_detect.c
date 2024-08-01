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
// \file windows_version_detect.c
// \brief Functions to detect the version of Windows the software is running in.
//

#include "common_types.h"
#include "type_conversion.h"
#include "memory_safety.h"
#include "windows_version_detect.h"
#include "env_detect.h"

#include <windows.h>

#include <strsafe.h> //needed in the code written to get the windows version since I'm using a Microsoft provided string concatenation call-tje
#include <string.h>

//possible MSDN api calls: 
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa370624(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/hardware/ff561910(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms724451(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms725492(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms724358(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx //Used this one since it is supposed to work!
//From MSDN: To obtain the full version number for the operating system, call the GetFileVersionInfo function on one of the system DLLs, such as Kernel32.dll, then call VerQueryValue to obtain the \\StringFileInfo\\<lang><codepage>\\ProductVersion subblock of the file version information.
//NOTE: Changed from GetFileVersionInfo as it did not report the correct up to date information that was expected.
//      Using RtlGetVersion from ntdll.dll instead
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(POSVERSIONINFOEXW);

eReturnValues read_Win_Version(ptrOSVersionNumber versionNumber)
{
    eReturnValues ret = SUCCESS;
    static DWORD winMajor = 0;
    static DWORD winMinor = 0;
    static DWORD winBuild = 0;
    static bool readVersionFromNTDLL = false;
    memset(versionNumber, 0, sizeof(OSVersionNumber));
    versionNumber->osVersioningIdentifier = OS_WINDOWS;

    if (!readVersionFromNTDLL)
    {
        static CONST TCHAR ntdll[] = TEXT("\\ntdll.dll");
        TCHAR* systemPathBuf = C_CAST(TCHAR*, safe_calloc(OPENSEA_PATH_MAX, sizeof(TCHAR)));
        CONST TCHAR* systemPath = &systemPathBuf[0];

        if (!systemPath)
        {
            return MEMORY_FAILURE;
        }

        UINT directoryStringLength = GetSystemDirectory(systemPathBuf, OPENSEA_PATH_MAX);
        if (directoryStringLength > OPENSEA_PATH_MAX || directoryStringLength == 0 || directoryStringLength > OPENSEA_PATH_MAX - sizeof(ntdll) / sizeof(*ntdll))
        {
            //error
            safe_Free(C_CAST(void**, &systemPathBuf));
            systemPath = M_NULLPTR;
            return FAILURE;
        }
        //I'm using this Microsoft provided call to concatenate strings since it will concatenate properly for ansi or wide strings depending on whether UNICODE is set or not - TJE
        if (S_OK != StringCchCat(systemPathBuf, OPENSEA_PATH_MAX, ntdll))
        {
            return FAILURE;
        }

        HMODULE hMod = INVALID_HANDLE_VALUE;
        if (TRUE == GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, systemPathBuf, &hMod))
        {
            if (hMod != INVALID_HANDLE_VALUE)
            {
                RtlGetVersionPtr rtlgetverptr = C_CAST(RtlGetVersionPtr, C_CAST(void*, GetProcAddress(hMod, "RtlGetVersion")));
                if (rtlgetverptr != M_NULLPTR)
                {
                    OSVERSIONINFOEXW osInfo;
                    memset(&osInfo, 0, sizeof(OSVERSIONINFOEXW));
                    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
                    if (rtlgetverptr(&osInfo) >= 0)//not using NT_SUCCESS from ntdef.h because it causes lots or redefinition errors, but this is all that macro does
                    {
                        versionNumber->versionType.windowsVersion.majorVersion = osInfo.dwMajorVersion;
                        versionNumber->versionType.windowsVersion.minorVersion = osInfo.dwMinorVersion;
                        versionNumber->versionType.windowsVersion.buildNumber = osInfo.dwBuildNumber;
                        winMajor = osInfo.dwMajorVersion;
                        winMinor = osInfo.dwMinorVersion;
                        winBuild = osInfo.dwBuildNumber;
                        ret = SUCCESS;
                        readVersionFromNTDLL = true;
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
        safe_Free(C_CAST(void**, &systemPathBuf));
        systemPath = M_NULLPTR;
    }
    else
    {
        versionNumber->versionType.windowsVersion.majorVersion = winMajor;
        versionNumber->versionType.windowsVersion.minorVersion = winMinor;
        versionNumber->versionType.windowsVersion.buildNumber = winBuild;
    }
    return ret;
}

bool is_Windows_Vista_Or_Higher(void)
{
    bool isWindowsVistaOrHigher = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 6;
    windowsVersionInfo.dwMinorVersion = 0;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindowsVistaOrHigher = true;
    }
    return isWindowsVistaOrHigher;
}

bool is_Windows_7_Or_Higher(void)
{
    bool isWindows7OrHigher = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 6;
    windowsVersionInfo.dwMinorVersion = 1;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows7OrHigher = true;
    }
    return isWindows7OrHigher;
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
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
    read_Win_Version(&windowsVersion);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19044)
    {
        //Win10 or higher
        isWindows10_21H2OrHigher = true;
    }
    return isWindows10_21H2OrHigher;
}

bool is_Windows_10_Version_22H2_Or_Higher(void)
{
    bool isWindows10_22H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19045)
    {
        //Win10 or higher
        isWindows10_22H2OrHigher = true;
    }
    return isWindows10_22H2OrHigher;
}

bool is_Windows_11_Version_21H2_Or_Higher(void)
{
    bool isWindows10_21H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 22000)
    {
        //Win10 or higher
        isWindows10_21H2OrHigher = true;
    }
    return isWindows10_21H2OrHigher;
}

bool is_Windows_11_Version_22H2_Or_Higher(void)
{
    bool isWindows11_22H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 22621)
    {
        isWindows11_22H2OrHigher = true;
    }
    return isWindows11_22H2OrHigher;
}

bool is_Windows_11_Version_23H2_Or_Higher(void)
{
    bool isWindows11_23H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 22631)
    {
        isWindows11_23H2OrHigher = true;
    }
    return isWindows11_23H2OrHigher;
}

bool is_Windows_Server_OS(void)
{
    static bool checkedForServer = false;
    static bool isWindowsServer = false;
    if (!checkedForServer)
    {
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
        checkedForServer = true;
    }
    return isWindowsServer;
}
//Consider checking for multiple keys in the future.
bool is_Windows_PE(void)
{
    static bool regread = false;
    static bool isWindowsPE = false;
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
    if (!regread)
    {
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
        regread = true;
    }
    return isWindowsPE;
}
