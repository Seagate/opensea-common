//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012 - 2020 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file common_windows.h
// \brief Definitions and Windows specific functions

#pragma once

#if defined (__cplusplus)
extern "C"
{
#endif

    //This pragma is needed to tell a library including opensea-common to look for Version.lib for the version helping information in the .c file.
    //NOTE: ARM requires 10.0.16299.0 API to get this library!
    #if !defined (__MINGW32__) && !defined (__MINGW64__)
    #pragma comment(lib,"Version.lib")//for getting Windows system versions
    #pragma comment(lib,"Advapi32.lib")//for checking for "run as administrator". May not be necessary to build some tools, but putting this here to prevent problems.
    #endif

    #include <windows.h>
#if !defined(NTDDSCSI_INCLUDED)
    #include <ntddscsi.h>
    #define NTDDSCSI_INCLUDED
#endif
    #define OPENSEA_PATH_MAX MAX_PATH

    #define SYSTEM_PATH_SEPARATOR '\\'
    #define SYSTEM_PATH_SEPARATOR_STR "\\"

    //  
    // _WIN32_WINNT version constants  
    //  
    #define SEA_WIN32_WINNT_NT4                    0x0400 // Windows NT 4.0  
    #define SEA_WIN32_WINNT_WIN2K                  0x0500 // Windows 2000  
    #define SEA_WIN32_WINNT_WINXP                  0x0501 // Windows XP  
    #define SEA_WIN32_WINNT_WS03                   0x0502 // Windows Server 2003  
    #define SEA_WIN32_WINNT_WIN6                   0x0600 // Windows Vista  
    #define SEA_WIN32_WINNT_VISTA                  0x0600 // Windows Vista  
    #define SEA_WIN32_WINNT_WS08                   0x0600 // Windows Server 2008  
    #define SEA_WIN32_WINNT_LONGHORN               0x0600 // Windows Vista  
    #define SEA_WIN32_WINNT_WIN7                   0x0601 // Windows 7  
    #define SEA_WIN32_WINNT_WIN8                   0x0602 // Windows 8  
    #define SEA_WIN32_WINNT_WINBLUE                0x0603 // Windows 8.1  
    #define SEA_WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10  
    #define SEA_WIN32_WINNT_WIN10                  0x0A00 // Windows 10  

    //Visual studio MSVC versions that are easy to check for.
    //values come from here:https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
    #define SEA_MSC_VER_VS2013      1800
    #define SEA_MSC_VER_VS2015      1900
    #define SEA_MSC_VER_VS2017_RTW  1910
    #define SEA_MSC_VER_VS2017_15_3 1911
    #define SEA_MSC_VER_VS2017_15_5 1912
    #define SEA_MSC_VER_VS2017_15_6 1913
    #define SEA_MSC_VER_VS2017_15_7 1914
    #define SEA_MSC_VER_VS2017_15_8 1915
    #define SEA_MSC_VER_VS2017_15_9 1916
    #define SEA_MSC_VER_VS2019_RTW  1920
    #define SEA_MSC_VER_VS2019_16_1 1921
    #define SEA_MSC_VER_VS2019_16_2 1922
    #define SEA_MSC_VER_VS2019_16_3 1923


    //-----------------------------------------------------------------------------
    //
    //  is_Windows_8_Or_Higher()
    //
    //! \brief   Description:  Checks if the current OS is Windows 8 or higher (this is essentially the same as what is in version_helper.h, but that file is not available to MinGW
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return true = Windows 8 or higher, false = Windows 7 or lower
    //
    //-----------------------------------------------------------------------------
    bool is_Windows_8_Or_Higher();

    //-----------------------------------------------------------------------------
    //
    //  is_Windows_8_One_Or_Higher()
    //
    //! \brief   Description:  Checks if the current OS is Windows 8.1 or higher. This does NOT require manifesting as 8.1 or higher to work. It uses the version number in kernel32.dll
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return true = Windows 8.1 or higher, false = Windows 8 or lower
    //
    //-----------------------------------------------------------------------------
    bool is_Windows_8_One_Or_Higher();

    //-----------------------------------------------------------------------------
    //
    //  is_Windows_10_Or_Higher()
    //
    //! \brief   Description:  Checks if the current OS is Windows 10 or higher. This does NOT require manifesting as 8.1 or higher to work. It uses the version number in kernel32.dll
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return true = Windows 8 or higher, false = Windows 8.1 or lower
    //
    //-----------------------------------------------------------------------------
    bool is_Windows_10_Or_Higher();

    //-----------------------------------------------------------------------------
    //
    //  print_Windows_Error_To_Screen(unsigned int windowsError)
    //
    //! \brief   Description:  Prints the error number and it's meaning to the screen followed by a newline character
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return VOID
    //
    //-----------------------------------------------------------------------------
    void print_Windows_Error_To_Screen(unsigned int windowsError);

    //The macros for NTDDI_VERSION & _WIN32_WINNT & WINVER are rarely checked once in Win 10 since they all get set to the same thing, so the below targets should also be defined in the preprocessor
    //so that we know which version of the WIN10 SDK is being targeted.

#if !defined(WIN_API_TARGET_VERSION)
#if defined _MSC_VER
#pragma message ("WARNING: WIN_API_TARGET_VERSION is not set. This should be set to a value in common_windows.h. If unsure, define it as 81000000L")
#else //assuming other compilers support #warning since they likely emulate GCC's behavior
#warning WARNING: WIN_API_TARGET_VERSION is not set.This should be set to a value in common_windows.h. If unsure, define it as 81000000L
#endif
#endif
    //SDK Versions can be found here: https://en.wikipedia.org/wiki/Microsoft_Windows_SDK
    //Add more below as we need them
    //These defines were added for comparison so you know which Windows API is being targetted.
    //The preprocessor should define WIN_API_TARGET_VERSION as one of these values so that it is known which API version is being targetted for build.
    //This is not available through including header files as far as I know - TJE
    //Not defining this may cause some compiler errors as different calls will be available to different API versions...same with different API flags to some functions.
    #define WIN_API_TARGET_UNKNOWN 0 //Unknown target, or not defined by builder
    #define WIN_API_TARGET_8            80507270L //8.0.50727 //Added in case we need it trying to support older compiler or MinGW against older Windows API (VS2012 API version)
    #define WIN_API_TARGET_81           81516360L //8.1.51636 Included with VS2013 (8.1A) //In VS2013 & VS2015, you select the Windows 8.1 API as the lowest target. VS2013 doesn't offer an API higher than this.
    #define WIN_API_TARGET_81_AND_OLDER 81000000L //8.1.00000.0 //This is left here for compatibility before adding the definitions above.
    #define WIN_API_TARGET_WIN10_10240  100102400L //10.0.10240.0 //Win 10 API, build 10240
    #define WIN_API_TARGET_WIN10_10586  100105860L //10.0.10586.0 //Win 10 API, build 10586
    #define WIN_API_TARGET_WIN10_14393  100143930L //10.0.14393.0 //Win 10 API, build 14393
    #define WIN_API_TARGET_WIN10_15063  100150630L //10.0.15063.0 //Win 10 API, build 15063
    #define WIN_API_TARGET_WIN10_16299  100162990L //10.0.16299.0 //Win 10 API, build 16299
    #define WIN_API_TARGET_WIN10_17134  100171340L //10.0.17134.0 //Win 10 API, build 17134
    #define WIN_API_TARGET_WIN10_17763  100177630L //10.0.17763.0 //Win 10 API, build 17763
    #define WIN_API_TARGET_WIN10_18362  100183620L //10.0.18362.0 //Win 10 API, build 18362

        
#if defined (__cplusplus)
}
#endif