// SPDX-License-Identifier: MPL-2.0

//! \file windows_version_detect.c
//! \brief Code to detect the Windows OS version number
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2025 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "windows_version_detect.h"
#include "common_types.h"
#include "env_detect.h"
#include "memory_safety.h"
#include "type_conversion.h"

DISABLE_WARNING_4255
#include <windows.h>
RESTORE_WARNING_4255

#include <string.h>
#include <strsafe.h> //needed in the code written to get the windows version since I'm using a Microsoft provided string concatenation call-tje

// possible MSDN api calls:
// https://msdn.microsoft.com/en-us/library/windows/desktop/aa370624(v=vs.85).aspx
// https://msdn.microsoft.com/en-us/library/windows/hardware/ff561910(v=vs.85).aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724451(v=vs.85).aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms725492(v=vs.85).aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724358(v=vs.85).aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx
// //Used this one since it is supposed to work! From MSDN: To obtain the full
// version number for the operating system, call the GetFileVersionInfo function
// on one of the system DLLs, such as Kernel32.dll, then call VerQueryValue to
// obtain the
// \\StringFileInfo\\<lang><codepage>\\ProductVersion subblock of the file
// version information. NOTE: Changed from GetFileVersionInfo as it did not
// report the correct up to date information that was expected.
//       Using RtlGetVersion from ntdll.dll instead
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(POSVERSIONINFOEXW);

eReturnValues read_Win_Version(ptrOSVersionNumber versionNumber)
{
    eReturnValues ret                  = SUCCESS;
    static DWORD  winMajor             = DWORD_C(0);
    static DWORD  winMinor             = DWORD_C(0);
    static DWORD  winBuild             = DWORD_C(0);
    static bool   readVersionFromNTDLL = false;
    safe_memset(versionNumber, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    versionNumber->osVersioningIdentifier = OS_WINDOWS;

    if (!readVersionFromNTDLL)
    {
        static CONST TCHAR ntdll[]       = TEXT("\\ntdll.dll");
        TCHAR*             systemPathBuf = M_REINTERPRET_CAST(TCHAR*, safe_calloc(OPENSEA_PATH_MAX, sizeof(TCHAR)));
        CONST TCHAR*       systemPath    = &systemPathBuf[0];

        if (systemPath == M_NULLPTR)
        {
            return MEMORY_FAILURE;
        }

        UINT directoryStringLength = GetSystemDirectory(systemPathBuf, OPENSEA_PATH_MAX);
        if (directoryStringLength > OPENSEA_PATH_MAX || directoryStringLength == 0 ||
            directoryStringLength > OPENSEA_PATH_MAX - SIZE_OF_STACK_ARRAY(ntdll))
        {
            // error
            safe_free_tchar(&systemPathBuf);
            systemPath = M_NULLPTR;
            return FAILURE;
        }
        // I'm using this Microsoft provided call to concatenate strings since
        // it will concatenate properly for ansi or wide strings depending on
        // whether UNICODE is set or not - TJE
        if (S_OK != StringCchCat(systemPathBuf, OPENSEA_PATH_MAX, ntdll))
        {
            return FAILURE;
        }

        HMODULE hMod = INVALID_HANDLE_VALUE;
        if (MSFT_BOOL_TRUE(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, systemPathBuf, &hMod)))
        {
            if (hMod != INVALID_HANDLE_VALUE)
            {
                RtlGetVersionPtr rtlgetverptr =
                    C_CAST(RtlGetVersionPtr, C_CAST(void*, GetProcAddress(hMod, "RtlGetVersion")));
                if (rtlgetverptr != M_NULLPTR)
                {
                    OSVERSIONINFOEXW osInfo;
                    safe_memset(&osInfo, sizeof(OSVERSIONINFOEXW), 0, sizeof(OSVERSIONINFOEXW));
                    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
                    if (rtlgetverptr(&osInfo) >= 0) // not using NT_SUCCESS from ntdef.h because it
                                                    // causes lots or redefinition errors, but this is
                                                    // all that macro does
                    {
                        versionNumber->versionType.windowsVersion.majorVersion = osInfo.dwMajorVersion;
                        versionNumber->versionType.windowsVersion.minorVersion = osInfo.dwMinorVersion;
                        versionNumber->versionType.windowsVersion.buildNumber  = osInfo.dwBuildNumber;
                        winMajor                                               = osInfo.dwMajorVersion;
                        winMinor                                               = osInfo.dwMinorVersion;
                        winBuild                                               = osInfo.dwBuildNumber;
                        ret                                                    = SUCCESS;
                        readVersionFromNTDLL                                   = true;
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
        safe_free_tchar(&systemPathBuf);
        systemPath = M_NULLPTR;
    }
    else
    {
        versionNumber->versionType.windowsVersion.majorVersion = winMajor;
        versionNumber->versionType.windowsVersion.minorVersion = winMinor;
        versionNumber->versionType.windowsVersion.buildNumber  = winBuild;
    }
    return ret;
}

bool is_Windows_Vista_Or_Higher(void)
{
    bool            isWindowsVistaOrHigher = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG       conditionMask = 0;
    safe_memset(&windowsVersionInfo, sizeof(OSVERSIONINFOEX), 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    // Now get the actual version of the OS...start at windows vista and work
    // forward from there.
    windowsVersionInfo.dwMajorVersion = DWORD_C(6);
    windowsVersionInfo.dwMinorVersion = DWORD_C(0);
    conditionMask                     = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask                     = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindowsVistaOrHigher = true;
    }
    return isWindowsVistaOrHigher;
}

bool is_Windows_7_Or_Higher(void)
{
    bool            isWindows7OrHigher = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG       conditionMask = 0;
    safe_memset(&windowsVersionInfo, sizeof(OSVERSIONINFOEX), 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    // Now get the actual version of the OS...start at windows vista and work
    // forward from there.
    windowsVersionInfo.dwMajorVersion = DWORD_C(6);
    windowsVersionInfo.dwMinorVersion = DWORD_C(1);
    conditionMask                     = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask                     = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows7OrHigher = true;
    }
    return isWindows7OrHigher;
}

bool is_Windows_8_Or_Higher(void)
{
    bool            isWindows8OrHigher = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG       conditionMask = 0;
    safe_memset(&windowsVersionInfo, sizeof(OSVERSIONINFOEX), 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    // Now get the actual version of the OS...start at windows vista and work
    // forward from there.
    windowsVersionInfo.dwMajorVersion = DWORD_C(6);
    windowsVersionInfo.dwMinorVersion = DWORD_C(2);
    conditionMask                     = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask                     = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows8OrHigher = true;
    }
    return isWindows8OrHigher;
}

//! \fn static M_INLINE bool windows_Version_GT_Check(WindowsVersionNumber winver,
//!                                                   uint32_t major,
//!                                                   uint32_t minor,
//!                                                   uint32_t buildnumber)
//! \brief Checks if the provided Windows version is greater than or equal to the specified version
//! \param[in] winver The Windows version number to check.
//! \param[in] major The major version number to compare against.
//! \param[in] minor The minor version number to compare against.
//! \param[in] buildnumber The build number to compare against.
//! \return true if the provided Windows version is greater than or equal to the specified version,
//! false otherwise.
//! \note Example usage: windows_Version_GT_Check(version, 6, 3, 1) to check if the provided info is
//! version 6.3.1 or greater.
static M_INLINE bool windows_Version_GT_Check(WindowsVersionNumber winver,
                                              uint32_t             major,
                                              uint32_t             minor,
                                              uint32_t             buildnumber)
{
    return (winver.majorVersion > major) || (winver.majorVersion == major && winver.minorVersion > minor) ||
           (winver.majorVersion == major && winver.minorVersion == minor && winver.buildNumber >= buildnumber);
}

bool is_Windows_8_One_Or_Higher(void)
{
    bool            isWindows81OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 6, 3, 0))
    {
        isWindows81OrHigher = true;
    }
    return isWindows81OrHigher;
}

bool is_Windows_10_Or_Higher(void)
{
    bool            isWindows10OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 0))
    {
        isWindows10OrHigher = true;
    }
    return isWindows10OrHigher;
}

bool is_Windows_10_Version_1507_Or_Higher(void)
{
    bool            isWindows10_1507OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 10240))
    {
        isWindows10_1507OrHigher = true;
    }
    return isWindows10_1507OrHigher;
}

bool is_Windows_10_Version_1511_Or_Higher(void)
{
    bool            isWindows10_1511OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 10586))
    {
        isWindows10_1511OrHigher = true;
    }
    return isWindows10_1511OrHigher;
}

bool is_Windows_10_Version_1607_Or_Higher(void)
{
    bool            isWindows10_1607OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 14393))
    {
        isWindows10_1607OrHigher = true;
    }
    return isWindows10_1607OrHigher;
}

bool is_Windows_10_Version_1703_Or_Higher(void)
{
    bool            isWindows10_1703OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 15063))
    {
        isWindows10_1703OrHigher = true;
    }
    return isWindows10_1703OrHigher;
}

bool is_Windows_10_Version_1709_Or_Higher(void)
{
    bool            isWindows10_1709OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 16299))
    {
        isWindows10_1709OrHigher = true;
    }
    return isWindows10_1709OrHigher;
}

bool is_Windows_10_Version_1803_Or_Higher(void)
{
    bool            isWindows10_1803OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 17134))
    {
        isWindows10_1803OrHigher = true;
    }
    return isWindows10_1803OrHigher;
}

bool is_Windows_10_Version_1809_Or_Higher(void)
{
    bool            isWindows10_1809OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 17763))
    {
        isWindows10_1809OrHigher = true;
    }
    return isWindows10_1809OrHigher;
}

bool is_Windows_10_Version_1903_Or_Higher(void)
{
    bool            isWindows10_1903OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 18362))
    {
        isWindows10_1903OrHigher = true;
    }
    return isWindows10_1903OrHigher;
}

bool is_Windows_10_Version_1909_Or_Higher(void)
{
    bool            isWindows10_1909OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 18363))
    {
        isWindows10_1909OrHigher = true;
    }
    return isWindows10_1909OrHigher;
}

bool is_Windows_10_Version_2004_Or_Higher(void)
{
    bool            isWindows10_2004OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 19041))
    {
        isWindows10_2004OrHigher = true;
    }
    return isWindows10_2004OrHigher;
}

bool is_Windows_10_Version_20H2_Or_Higher(void)
{
    bool            isWindows10_20H2OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 19042))
    {
        isWindows10_20H2OrHigher = true;
    }
    return isWindows10_20H2OrHigher;
}

bool is_Windows_10_Version_21H1_Or_Higher(void)
{
    bool            isWindows10_21H1OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 19043))
    {
        isWindows10_21H1OrHigher = true;
    }
    return isWindows10_21H1OrHigher;
}

bool is_Windows_10_Version_21H2_Or_Higher(void)
{
    bool            isWindows10_21H2OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 19044))
    {
        isWindows10_21H2OrHigher = true;
    }
    return isWindows10_21H2OrHigher;
}

bool is_Windows_10_Version_22H2_Or_Higher(void)
{
    bool            isWindows10_22H2OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 19045))
    {
        isWindows10_22H2OrHigher = true;
    }
    return isWindows10_22H2OrHigher;
}

bool is_Windows_11_Version_21H2_Or_Higher(void)
{
    bool            isWindows10_21H2OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 22000))
    {
        isWindows10_21H2OrHigher = true;
    }
    return isWindows10_21H2OrHigher;
}

bool is_Windows_11_Version_22H2_Or_Higher(void)
{
    bool            isWindows11_22H2OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 22621))
    {
        isWindows11_22H2OrHigher = true;
    }
    return isWindows11_22H2OrHigher;
}

bool is_Windows_11_Version_23H2_Or_Higher(void)
{
    bool            isWindows11_23H2OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 22631))
    {
        isWindows11_23H2OrHigher = true;
    }
    return isWindows11_23H2OrHigher;
}

bool is_Windows_11_Version_24H2_Or_Higher(void)
{
    bool            isWindows11_24H2OrHigher = false;
    OSVersionNumber windowsVersion;
    safe_memset(&windowsVersion, sizeof(OSVersionNumber), 0, sizeof(OSVersionNumber));
    read_Win_Version(&windowsVersion);
    if (windows_Version_GT_Check(windowsVersion.versionType.windowsVersion, 10, 0, 26100))
    {
        isWindows11_24H2OrHigher = true;
    }
    return isWindows11_24H2OrHigher;
}

bool is_Windows_Server_OS(void)
{
    static bool checkedForServer = false;
    static bool isWindowsServer  = false;
    if (!checkedForServer)
    {
        OSVERSIONINFOEX windowsVersionInfo;
        DWORDLONG       conditionMask = 0;
        safe_memset(&windowsVersionInfo, sizeof(OSVERSIONINFOEX), 0, sizeof(OSVERSIONINFOEX));
        windowsVersionInfo.wProductType = VER_NT_WORKSTATION;
        conditionMask                   = VerSetConditionMask(conditionMask, VER_PRODUCT_TYPE,
                                                              VER_EQUAL); // checking for equality on the workstation attribute
        if (VerifyVersionInfo(&windowsVersionInfo, VER_PRODUCT_TYPE, conditionMask))
        {
            // Windows workstation/desktop
            isWindowsServer = false;
        }
        else
        {
            // Windows server/domain controller (which is a windows server
            // version)
            isWindowsServer = true;
        }
        checkedForServer = true;
    }
    return isWindowsServer;
}
// Consider checking for multiple keys in the future.
bool is_Windows_PE(void)
{
    static bool regread     = false;
    static bool isWindowsPE = false;
    // To figure out if running in PE requires checking the registry. There is
    // not another known way to look for this. All other functions to read or
    // check versions will return build numbers that match the equivalent non-PE
    // version of windows, which can still be useful for determining supported
    // APIs We will be looking for one of these keys:
    /*
    -HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\WinPE
    -HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\MiniNT
    -HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\MiniNT
    //NOTE: 3 below are not used today. Only these top two are checked at this
    time -HKEY_LOCAL_MACHINE\system\currentcontrolset\control\PEBootType (PE2)
    -HKEY_LOCAL_MACHINE\system\currentcontrolset\control\PEFirmwareType (PE4)
    -HKEY_LOCAL_MACHINE\System\Setup\SystemSetupInProgress
    */
    if (!regread)
    {
        HKEY keyHandle;
        if (!isWindowsPE && ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                                                          TEXT("SOFTWARE\\Microsoft\\Windows "
                                                               "NT\\CurrentVersion\\WinPE"),
                                                          0, KEY_READ, &keyHandle))
        {
#if defined(_DEBUG)
            printf("Found HKLM\\SOFTWARE\\Microsoft\\Windows "
                   "NT\\CurrentVersion\\WinPE\n");
#endif
            isWindowsPE = true;
            RegCloseKey(keyHandle);
            keyHandle = 0;
        }
        if (!isWindowsPE &&
            ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\MiniNT"), 0,
                                          KEY_READ, &keyHandle))
        {
#if defined(_DEBUG)
            print_str("Found HKLM\\SYSTEM\\CurrentControlSet\\Control\\MiniNT\n");
#endif
            isWindowsPE = true;
            RegCloseKey(keyHandle);
            keyHandle = 0;
        }
        if (!isWindowsPE &&
            ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\ControlSet001\\Control\\MiniNT"), 0,
                                          KEY_READ, &keyHandle))
        {
#if defined(_DEBUG)
            print_str("Found HKLM\\SYSTEM\\ControlSet001\\Control\\MiniNT\n");
#endif
            isWindowsPE = true;
            RegCloseKey(keyHandle);
            keyHandle = 0;
        }
        regread = true;
    }
    return isWindowsPE;
}
