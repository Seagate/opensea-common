// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012-2024 Seagate Technology LLC and/or its Affiliates, All
// Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file windows_env_detect.c
// \brief Functions for env_detect.h for Windows to get username (if enabled)
// and read OS information.
//

#include "common_types.h"
#include "io_utils.h"
#include "memory_safety.h"
#include "string_utils.h"
#include "type_conversion.h"
#include "windows_version_detect.h"

DISABLE_WARNING_4255
#include <windows.h> // NOLINT(clang-diagnostic-error)
RESTORE_WARNING_4255

#include <strsafe.h> //needed in the code written to get the windows version since I'm using a Microsoft provided string concatenation call-tje
#if defined(ENABLE_READ_USERNAME)
#    include <lmcons.h> //for UNLEN
#endif
#include <string.h>

eReturnValues get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char* operatingSystemName)
{
    eReturnValues ret = read_Win_Version(versionNumber);
    if (ret == SUCCESS)
    {
        // Now that we know whether or not it's a server version and have gotten
        // the version number, set the appropriate string for the OS.
        if (operatingSystemName)
        {
            bool isWindowsServer = is_Windows_Server_OS();
            bool isWindowsPE     = is_Windows_PE();
            switch (versionNumber->versionType.windowsVersion.majorVersion)
            {
            case 10: // Win 10 or Server 2016
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 0:
                    if (isWindowsServer)
                    {
                        switch (versionNumber->versionType.windowsVersion.buildNumber)
                        {
                        case 14393:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2016");
                            break;
                        case 17763:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2019");
                            break;
                        case 18362:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 1903");
                            break;
                        case 18363:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 1909");
                            break;
                        case 19041:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 2004");
                            break;
                        case 19042:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 20H2");
                            break;
                        case 20348:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2022");
                            break;
                        default:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE,
                                                "Unknown Windows Server Version");
                            break;
                        }
                    }
                    else
                    {
                        switch (versionNumber->versionType.windowsVersion.buildNumber)
                        {
                        case 10240:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1507");
                            break;
                        case 10586:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1511");
                            break;
                        case 14393:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1607");
                            break;
                        case 15063:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1703");
                            break;
                        case 16299:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1709");
                            break;
                        case 17134:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1803");
                            break;
                        case 17763:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1809");
                            break;
                        case 18362:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1903");
                            break;
                        case 18363:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1909");
                            break;
                        case 19041:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 2004");
                            break;
                        case 19042:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 20H2");
                            break;
                        case 19043:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 21H1");
                            break;
                        case 19044:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 21H2");
                            break;
                        case 19045:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 22H2");
                            break;
                        case 22000:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 11, version 21H2");
                            break;
                        case 22621:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 11, version 22H2");
                            break;
                        case 22631:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 11, version 23H2");
                            break;
                        default:
                            snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows 10/11 version");
                            break;
                        }
                    }
                    break;
                default:
                    if (isWindowsServer)
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows Server Version");
                    }
                    else
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows Version");
                    }
                    break;
                }
                break;
            case 6: // Vista through 8.1
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 3:
                    if (isWindowsServer)
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2012 R2");
                    }
                    else
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 8.1");
                    }
                    break;
                case 2:
                    if (isWindowsServer)
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2012");
                    }
                    else
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 8");
                    }
                    break;
                case 1:
                    if (isWindowsServer)
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2008 R2");
                    }
                    else
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 7");
                    }
                    break;
                case 0:
                    if (isWindowsServer)
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2008");
                    }
                    else
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Vista");
                    }
                    break;
                default:
                    if (isWindowsServer)
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2012 R2 or higher");
                    }
                    else
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 8.1 or higher");
                    }
                    break;
                }
                break;
            case 5: // 2000 through XP
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 2:
                    if (isWindowsServer)
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2003");
                    }
                    else
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows XP 64-Bit Edition");
                    }
                    break;
                case 1:
                    snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows XP");
                    break;
                case 0:
                    snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows 2000");
                    break;
                default:
                    if (isWindowsServer)
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2003 or higher");
                    }
                    else
                    {
                        snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "XP or higher");
                    }
                    break;
                }
                break;
            default:
                snprintf_err_handle(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows OS");
                break;
            }
            if (isWindowsPE)
            {
                safe_strcat(operatingSystemName, OS_NAME_SIZE, " (PE)");
            }
        }
    }
    return ret;
}

bool is_Running_Elevated(void)
{
    bool   isElevated     = false;
    HANDLE currentProcess = M_NULLPTR;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &currentProcess))
    {
        TOKEN_ELEVATION elevation;
        DWORD           returnedSize = sizeof(TOKEN_ELEVATION);
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

#if defined(ENABLE_READ_USERNAME)
// Gets the user name for who is running the process
// https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getusernamea?redirectedfrom=MSDN
// NOTE: Not using Ex version at this time to avoid linking yet another library.
// This can be added if necessary, or this doesn't do quite what we want it to
// do. -TJE
eReturnValues get_Current_User_Name(char** userName)
{
    eReturnValues ret = SUCCESS;
    if (userName)
    {
        DWORD localNameLength = UNLEN + 1; // start with this for input
        DECLARE_ZERO_INIT_ARRAY(TCHAR, localName, UNLEN + 1);
        if (TRUE == GetUserName(localName, &localNameLength))
        {
            const char* isAdmin = " (admin)"; // This will be concatenated to the string if
                                              // running as administrator since we only get the
                                              // user's name in Windows.
            size_t usernameLength = _tcslen(localName) + safe_strlen(isAdmin) + 1;
            *userName             = M_REINTERPRET_CAST(char*, safe_calloc(usernameLength, sizeof(char)));
            if (*userName)
            {
#    if defined UNICODE
                size_t charsConverted = SIZE_T_C(0);
                // convert output to a char string
                if (wcstombs_s(&charsConverted, *userName, usernameLength, localName, usernameLength))
                {
                    safe_free(userName);
                    ret = FAILURE;
                }
#    else
                // just copy it over after allocating
                if (strcpy_s(*userName, usernameLength, localName))
                {
                    safe_free(userName);
                    return FAILURE;
                }
#    endif
                if (is_Running_Elevated())
                {
                    if (strcat_s(*userName, usernameLength, isAdmin))
                    {
                        safe_free(userName);
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
#endif // ENABLE_READ_USERNAME
