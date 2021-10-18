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

    #include <sdkddkver.h>
    #include <winsdkver.h>
    #include <windows.h>
#if !defined(NTDDSCSI_INCLUDED)
    #include <ntddscsi.h>
    #define NTDDSCSI_INCLUDED
#endif
    //more info on max path in Windows
    //https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation?tabs=cmd
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
    bool is_Windows_8_Or_Higher(void);

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
    bool is_Windows_8_One_Or_Higher(void);

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
    bool is_Windows_10_Or_Higher(void);

    //Functions to help with different sub versions of Windows 10
    bool is_Windows_10_Version_1507_Or_Higher(void);
    bool is_Windows_10_Version_1511_Or_Higher(void);
    bool is_Windows_10_Version_1607_Or_Higher(void);
    bool is_Windows_10_Version_1703_Or_Higher(void);
    bool is_Windows_10_Version_1709_Or_Higher(void);
    bool is_Windows_10_Version_1803_Or_Higher(void);
    bool is_Windows_10_Version_1809_Or_Higher(void);
    bool is_Windows_10_Version_1903_Or_Higher(void);
    bool is_Windows_10_Version_1909_Or_Higher(void);
    bool is_Windows_10_Version_2004_Or_Higher(void);
    bool is_Windows_10_Version_20H2_Or_Higher(void);
    bool is_Windows_10_Version_21H1_Or_Higher(void);
    bool is_Windows_10_Version_21H2_Or_Higher(void);
    
    //Windows 11 check below
    bool is_Windows_11_Version_21H2_Or_Higher(void);

    //-----------------------------------------------------------------------------
    //
    //  is_Windows_PE()
    //
    //! \brief   Description:  Checks if the application is currently running in the Windows Pre-Installation Environment (PE).
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return true = running in PE, false = running in standard windows
    //
    //-----------------------------------------------------------------------------
    bool is_Windows_PE(void);

    //-----------------------------------------------------------------------------
    //
    //  print_Windows_Error_To_Screen(unsigned int windowsError)
    //
    //! \brief   Description:  Prints the error number and it's meaning to the screen followed by a newline character
    //
    //  Entry:
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Windows_Error_To_Screen(unsigned int windowsError);

    //The macros for NTDDI_VERSION & _WIN32_WINNT & WINVER are rarely checked once in Win 10 since they all get set to the same thing, so the below targets should also be defined in the preprocessor
    //so that we know which version of the WIN10 SDK is being targeted.

#if defined(WIN_API_TARGET_VERSION)
#undef WIN_API_TARGET_VERSION
#pragma message ("WIN_API_TARGET_VERSION no longer needs to be defined. Automatically defined from sdkddkver.h now")
#endif

#if defined (WDK_NTDDI_VERSION)
    #define WIN_API_TARGET_VERSION WDK_NTDDI_VERSION
#else
    //Check which, if any older NTDDI macros are available and set the highest...likely an older compiler or SDK version has been found that doesn't set the above macro like WIN10 SDKs
    #if defined (NTDDI_WINBLUE)
        #define WIN_API_TARGET_VERSION NTDDI_WINBLUE
    #elif defined (NTDDI_WIN8)
        #define WIN_API_TARGET_VERSION NTDDI_WIN8
    #elif defined (NTDDI_WIN7)
        #define WIN_API_TARGET_VERSION NTDDI_WIN7
    #elif defined (NTDDI_WIN6SP4)
        #define WIN_API_TARGET_VERSION NTDDI_WIN6SP4
    #elif defined (NTDDI_WIN6SP3)
        #define WIN_API_TARGET_VERSION NTDDI_WIN6SP3
    #elif defined (NTDDI_WIN6SP2)
        #define WIN_API_TARGET_VERSION NTDDI_WIN6SP2
    #elif defined (NTDDI_WIN6SP1)
        #define WIN_API_TARGET_VERSION NTDDI_WIN6SP1
    #elif defined (NTDDI_WS03SP4)
        #define WIN_API_TARGET_VERSION NTDDI_WS03SP4
    #elif defined (NTDDI_WS03SP3)
        #define WIN_API_TARGET_VERSION NTDDI_WS03SP3
    #elif defined (NTDDI_WS03SP2)
        #define WIN_API_TARGET_VERSION NTDDI_WS03SP2
    #elif defined (NTDDI_WS03SP1)
        #define WIN_API_TARGET_VERSION NTDDI_WS03SP1
    #elif defined (NTDDI_WS03)
        #define WIN_API_TARGET_VERSION NTDDI_WS03
    #elif defined (NTDDI_WINXPSP4)
        #define WIN_API_TARGET_VERSION NTDDI_WINXPSP4
    #elif defined (NTDDI_WINXPSP3)
        #define WIN_API_TARGET_VERSION NTDDI_WINXPSP3
    #elif defined (NTDDI_WINXPSP2)
        #define WIN_API_TARGET_VERSION NTDDI_WINXPSP2
    #elif defined (NTDDI_WINXPSP1)
        #define WIN_API_TARGET_VERSION NTDDI_WINXPSP1
    #elif defined (NTDDI_WINXP)
        #define WIN_API_TARGET_VERSION NTDDI_WINXP
    #elif defined (NTDDI_WIN2KSP4)
        #define WIN_API_TARGET_VERSION NTDDI_WIN2KSP4
    #elif defined (NTDDI_WIN2KSP3)
        #define WIN_API_TARGET_VERSION NTDDI_WIN2KSP3
    #elif defined (NTDDI_WIN2KSP2)
        #define WIN_API_TARGET_VERSION NTDDI_WIN2KSP2
    #elif defined (NTDDI_WIN2KSP1)
        #define WIN_API_TARGET_VERSION NTDDI_WIN2KSP1
    #elif defined (NTDDI_WIN2K)
        #define WIN_API_TARGET_VERSION NTDDI_WIN2K    
    #else
        #define WIN_API_TARGET_VERSION 1 /*this is an unknown version...assume that no target API is known*/
        #pragma message ("WARNING: No Windows API detected! Things may not work at all!")
    #endif//checking NTDDI_versions
#endif//WDK_NTDDI_VERSION

    //SDK Versions can be found here: https://en.wikipedia.org/wiki/Microsoft_Windows_SDK
    //Add more below as we need them
    //These defines were added for comparison so you know which Windows API is being targetted.
    //We defined these ourselves because it is confusing which Win10 API is being targetted since the naming convention is not well documented compared to 
    //how the packages are listed online to be installed. - TJE

    #define WIN_API_TARGET_UNKNOWN          1 //Unknown target, or not defined by builder
    #define WIN_API_TARGET_NT               0x04000000
    #define WIN_API_TARGET_2000             0x05000000   
    #define WIN_API_TARGET_2000_SP1         0x05000100
    #define WIN_API_TARGET_2000_SP2         0x05000200
    #define WIN_API_TARGET_2000_SP3         0x05000300
    #define WIN_API_TARGET_2000_SP4         0x05000400
    #define WIN_API_TARGET_XP               0x05010000
    #define WIN_API_TARGET_XP_SP1           0x05010100
    #define WIN_API_TARGET_XP_SP2           0x05010200
    #define WIN_API_TARGET_XP_SP3           0x05010300
    #define WIN_API_TARGET_XP_SP4           0x05010400
    #define WIN_API_TARGET_SERVER_2003      0x05020000
    #define WIN_API_TARGET_SERVER_2003_SP1  0x05020100
    #define WIN_API_TARGET_SERVER_2003_SP2  0x05020200
    #define WIN_API_TARGET_SERVER_2003_SP3  0x05020300
    #define WIN_API_TARGET_SERVER_2003_SP4  0x05020400
    //Windows server 2008 and vista share API versions. Use Vista defines instead where possible - TJE
    #define WIN_API_TARGET_SERVER_2008      0x06000100
    #define WIN_API_TARGET_SERVER_2008_SP2  0x06000200
    #define WIN_API_TARGET_SERVER_2008_SP3  0x06000300
    #define WIN_API_TARGET_SERVER_2008_SP4  0x06000400
    #define WIN_API_TARGET_VISTA            0x06000000
    #define WIN_API_TARGET_VISTA_SP1        0x06000100
    #define WIN_API_TARGET_VISTA_SP2        0x06000200
    #define WIN_API_TARGET_VISTA_SP3        0x06000300
    #define WIN_API_TARGET_VISTA_SP4        0x06000400
    #define WIN_API_TARGET_7                0x06010000
    #define WIN_API_TARGET_8                0x06020000
    #define WIN_API_TARGET_8_DOT_1          0x06030000
    #define WIN_API_TARGET_WIN10_THRESHOLD  0x0A000000
    #define WIN_API_TARGET_WIN10_10240      0x0A000001     //10.0.10240.0 //Win 10 API, build 10240
    #define WIN_API_TARGET_WIN10_10586      0x0A000002     //10.0.10586.0 //Win 10 API, build 10586
    #define WIN_API_TARGET_WIN10_14393      0x0A000003     //10.0.14393.0 //Win 10 API, build 14393
    #define WIN_API_TARGET_WIN10_15063      0x0A000004     //10.0.15063.0 //Win 10 API, build 15063
    #define WIN_API_TARGET_WIN10_16299      0x0A000005     //10.0.16299.0 //Win 10 API, build 16299
    #define WIN_API_TARGET_WIN10_17134      0x0A000006     //10.0.17134.0 //Win 10 API, build 17134
    #define WIN_API_TARGET_WIN10_17763      0x0A000007     //10.0.17763.0 //Win 10 API, build 17763
    #define WIN_API_TARGET_WIN10_18362      0x0A000008     //10.0.18362.0 //Win 10 API, build 18362
    #define WIN_API_TARGET_WIN10_19041      0x0A000009     //10.0.19041.0 //Win 10 API, build 19041
    #define WIN_API_TARGET_WIN10_20348      0x0A00000A     //10.0.20348.0 //Win 10 API, build 20348
    #define WIN_API_TARGET_WIN10_22000      0x0A00000B     //10.0.22000.0 //Win 10 API, build 22000
        
#if defined (__cplusplus)
}
#endif
