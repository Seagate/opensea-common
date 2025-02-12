// SPDX-License-Identifier: MPL-2.0

//! \file predef_env_detect.h
//! \brief Detects the compilation environment for standards, extensions, etc
//! using only predefined macros from libc/compiler
//! This file does include some headers as needed to lookup various defined versions such as features.h, unistd.h,
//! sys/param.h, limits.h, endian.h, sys/endian.h, etc.
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#if !defined(__STDC_WANT_LIB_EXT1__)
// asking to get C11 _s functions since there is some ability to use them in
// some places.
#    define __STDC_WANT_LIB_EXT1__ 1 // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
#endif
#if !defined(__STDC_WANT_LIB_EXT2__)
// asking to get dynamic memory functions (mostly strdup) in C11. Only
// guaranteed available if __STDC_ALLOC_LIB__ defined, but asking anyways...most
// systems we support will have this-TJE
#    define __STDC_WANT_LIB_EXT2__ 1 // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
#endif

//! \def IS_GCC_VERSION(major, minor)
//! \brief Macro to check GCC version(major.minor)
//! \param[in] major major version of GCC to check for
//! \param[in] minor minor version of GCC to check for
#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#    define IS_GCC_VERSION(major, minor) ((__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor))))
#else
#    define IS_GCC_VERSION(major, minor) (0)
#endif

//! \def IS_GCC_FULL_VERSION(major, minor, patch)
//! \brief Macro to check GCC full version (major.minor.patch)
//! \param[in] major major version of GCC to check for
//! \param[in] minor minor version of GCC to check for
//! \param[in] patch patch version of GCC to check for
#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#    define IS_GCC_FULL_VERSION(major, minor, patch)                                                                   \
        ((__GNUC__ > (major) ||                                                                                        \
          (__GNUC__ == (major) &&                                                                                      \
           (__GNUC_MINOR__ > (minor) || (__GNUC_MINOR__ == (minor) && __GNUC_PATCHLEVEL__ >= (patch))))))
#else
#    define IS_GCC_FULL_VERSION(major, minor, patch) (0)
#endif

//! \def IS_CLANG_VERSION(major, minor)
//! \brief Macro to check Clang version(major.minor)
//! \param[in] major major version of Clang to check for
//! \param[in] minor minor version of Clang to check for

//! \def IS_CLANG_FULL_VERSION(major, minor, patch)
//! \brief Macro to check Clang full version (major.minor.patch)
//! \param[in] major major version of Clang to check for
//! \param[in] minor minor version of Clang to check for
//! \param[in] patch patch version of Clang to check for
#if defined(__clang__)
#    define IS_CLANG_VERSION(major, minor)                                                                             \
        (__clang_major__ > (major) || (__clang_major__ == (major) && __clang_minor__ >= (minor)))

// Macro to check Clang full version (major.minor.patch)
#    define IS_CLANG_FULL_VERSION(major, minor, patch)                                                                 \
        (__clang_major__ > (major) ||                                                                                  \
         (__clang_major__ == (major) &&                                                                                \
          (__clang_minor__ > (minor) || (__clang_minor__ == (minor) && __clang_patchlevel__ >= (patch)))))
#else
#    define IS_CLANG_VERSION(major, minor)             (0)
#    define IS_CLANG_FULL_VERSION(major, minor, patch) (0)
#endif

//! \def IS_MINGW32_VERSION(major, minor)
//! \brief Macro to check MinGW32 version(major.minor)
//! \param[in] major major version of Mingw32 to check for
//! \param[in] minor minor version of Mingw32 to check for
#if defined(__MINGW32__) && defined(__MINGW32_MAJOR_VERSION) && defined(__MINGW32_MINOR_VERSION)
#    define IS_MINGW32_VERSION(major, minor)                                                                           \
        ((__MINGW32_MAJOR_VERSION > (major) ||                                                                         \
          (__MINGW32_MAJOR_VERSION == (major) && __MINGW32_MINOR_VERSION >= (minor))))
#else
#    define IS_MINGW32_VERSION(major, minor) (0)
#endif

//! \def IS_MINGW64_VERSION(major, minor)
//! \brief Macro to check MinGW-w64 version(major.minor)
//! \param[in] major major version of MinGW-w64 to check for
//! \param[in] minor minor version of MinGW-w64 to check for
#if defined(__MINGW64__) && defined(__MINGW64_VERSION_MAJOR) && defined(__MINGW64_VERSION_MINOR)
#    define IS_MINGW64_VERSION(major, minor)                                                                           \
        ((__MINGW64_VERSION_MAJOR > (major) ||                                                                         \
          (__MINGW64_VERSION_MAJOR == (major) && __MINGW64_VERSION_MINOR >= (minor))))
#else
#    define IS_MINGW64_VERSION(major, minor) (0)
#endif

// https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
// MSVC version macros to assist with version checks
#define MSC_6 600
#define MSC_7 700
// "Visual" added to name for all following compilers
#define MSVC_1_0        800
#define MSVC_2_0        900
#define MSVC_4_0        1000
#define MSVC_4_1        1010
#define MSVC_4_2        1020
#define MSVC_5_0        1100
#define MSVC_6_0        1200
#define MSVC_2002       1300
#define MSVC_2003       1310
#define MSVC_2005       1400
#define MSVC_2008       1500
#define MSVC_2010       1600
#define MSVC_2012       1700
#define MSVC_2013       1800
#define MSVC_2015       1900
#define MSVC_2017       1910
#define MSVC_2017_15_3  1911
#define MSVC_2017_15_5  1912
#define MSVC_2017_15_6  1913
#define MSVC_2017_15_7  1914
#define MSVC_2017_15_8  1915
#define MSVC_2017_15_9  1916
#define MSVC_2019       1920
#define MSVC_2019_16_0  1920
#define MSVC_2019_16_1  1921
#define MSVC_2019_16_2  1922
#define MSVC_2019_16_3  1923
#define MSVC_2019_16_4  1924
#define MSVC_2019_16_5  1925
#define MSVC_2019_16_6  1926
#define MSVC_2019_16_7  1927
#define MSVC_2019_16_8  1928 // need full ver to differentiate
#define MSVC_2019_16_9  1928 // need full ver to differentiate
#define MSVC_2019_16_10 1929 // need full ver to differentiate
#define MSVC_2019_16_11 1929 // need full ver to differentiate
#define MSVC_2022_17_0  1930
#define MSVC_2022_17_1  1931
#define MSVC_2022_17_2  1932
#define MSVC_2022_17_3  1933
#define MSVC_2022_17_4  1934
#define MSVC_2022_17_5  1935
#define MSVC_2022_17_6  1936
#define MSVC_2022_17_7  1937
#define MSVC_2022_17_8  1938
#define MSVC_2022_17_9  1939
#define MSVC_2022_17_10 1940

// Some specific full versions when needing to differentiate on the _MSC_FULL_VER macros
#define MSVC_FULL_2019_16_8  192829333 // need full ver to differentiate
#define MSVC_FULL_2019_16_9  192829910 // need full ver to differentiate
#define MSVC_FULL_2019_16_10 192929917 // need full ver to differentiate
#define MSVC_FULL_2019_16_11 192930129 // need full ver to differentiate

//! \def IS_MSVC_VERSION(msvcver)
//! \brief Macro to check MSVC version
//! \param[in] msvcver msvc version that _MSC_VER can be set to in various versions.
//! Use one of the definitions above for easy use
#if defined(_MSC_VER)
#    define IS_MSVC_VERSION(msvcver) (_MSC_VER >= (msvcver))
#else
#    define IS_MSVC_VERSION(msvcver) (0)
#endif

//! \def IS_MSVC_FULL_VERSION(msvcfullver)
//! \brief Macro to check MSVC full version using _MSC_FULL_VER
//! \param[in] msvcfullver msvc version that _MSC_FULL_VER can be set to in various versions.
//! Use one of the definitions above for easy use
#if defined(_MSC_FULL_VER)
#    define IS_MSVC_FULL_VERSION(msvcfullver) (_MSC_FULL_VER >= (msvcfullver))
#else
#    define IS_MSVC_FULL_VERSION(msvcfullver) (0)
#endif

// Including this to allow libc to include any features.h (if it exists) to help with libc version checking -TJE
// https://github.com/cpredef/predef/blob/master/Libraries.md
#if defined(__has_include)
#    if __has_include(<features.h>)
#        include <features.h>
#    else
#        include <limits.h>
#    endif // has features.h
#else
#    include <limits.h>
#endif // does not support __has_include

//! \def IS_GLIBC_VERSION(major, minor)
//! \brief Macro to check Glibc version (major.minor)
//! \param[in] major major version of Glibc to check for
//! \param[in] minor minor version of Glibc to check for
#if defined(__GNU_LIBRARY__) && defined(__GNU_LIBRARY_MINOR__)
#    define THIS_IS_GLIBC
#    define IS_GLIBC_VERSION(major, minor)                                                                             \
        ((__GNU_LIBRARY__ > (major) || (__GNU_LIBRARY__ == (major) && __GNU_LIBRARY_MINOR__ >= (minor))))
#elif defined(__GLIBC__) && defined(__GLIBC_MINOR__)
#    define THIS_IS_GLIBC
#    define IS_GLIBC_VERSION(major, minor)                                                                             \
        ((__GLIBC__ > (major) || (__GLIBC__ == (major) && __GLIBC_MINOR__ >= (minor))))
#else
#    define IS_GLIBC_VERSION(major, minor) (0)
#endif

//! \def IS_UCLIBC_VERSION(major, minor)
//! \brief Macro to check uClibc version (major.minor)
//! \param[in] major major version of uClibc to check for
//! \param[in] minor minor version of uClibc to check for

//! \def IS_UCLIBC_FULL_VERSION(major, minor, patch)
//! \brief Macro to check full uClibc version (major.minor.patch)
//! \param[in] major major version of uClibc to check for
//! \param[in] minor minor version of uClibc to check for
//! \param[in] patch patch version of uClibc to check for
#if defined(__UCLIBC__) && defined(__UCLIBC_MAJOR__) && defined(__UCLIBC_MINOR__)
#    define IS_UCLIBC_VERSION(major, minor)                                                                            \
        ((__UCLIBC_MAJOR__ > (major) || (__UCLIBC_MAJOR__ == (major) && __UCLIBC_MINOR__ >= (minor))))
#    define IS_UCLIBC_FULL_VERSION(major, minor, patch)                                                                \
        (__UCLIBC_MAJOR__ > (major) ||                                                                                 \
         (__UCLIBC_MAJOR__ == (major) &&                                                                               \
          (__UCLIBC_MINOR__ > (minor) || (__UCLIBC_MINOR__ == (minor) && __UCLIBC_SUBLEVEL__ >= (patch)))))
#else
#    define IS_UCLIBC_VERSION(major, minor)             (0)
#    define IS_UCLIBC_FULL_VERSION(major, minor, patch) (0)
#endif

//! \def IS_KLIBC_VERSION(major, minor)
//! \brief Macro to check klibc version (major.minor)
//! \param[in] major major version of klibc to check for
//! \param[in] minor minor version of klibc to check for

//! \def IS_KLIBC_VERSION(major, minor, patch)
//! \brief Macro to check full klibc version (major.minor.patch)
//! \param[in] major major version of klibc to check for
//! \param[in] minor minor version of klibc to check for
//! \param[in] patch patch version of klibc to check for
#if defined(__KLIBC__) && defined(__KLIBC_MINOR__) && defined(__KLIBC_PATCHLEVEL__)
#    define IS_KLIBC_VERSION(major, minor)                                                                             \
        ((__KLIBC__ > (major) || (__KLIBC__ == (major) && __KLIBC_MINOR__ >= (minor))))
#    define IS_KLIBC_FULL_VERSION(major, minor, patch)                                                                 \
        (__KLIBC__ > (major) ||                                                                                        \
         (__KLIBC__ == (major) &&                                                                                      \
          (__KLIBC_MINOR__ > (minor) || (__KLIBC_MINOR__ == (minor) && __KLIBC_PATCHLEVEL__ >= (patch)))))
#else
#    define IS_KLIBC_VERSION(major, minor)             (0)
#    define IS_KLIBC_FULL_VERSION(major, minor, patch) (0)
#endif

#if defined(__unix__) || defined(__APPLE__) || defined(HAVE_UNISTD)
#    include <unistd.h> //to ensure we can check for POSIX versions
#endif                  //__unix__ || __APPLE__ || HAVE_UNISTD

#if defined(__unix__) || defined(__APPLE__) || defined(HAVE_SYSPARAM)
#    include <sys/param.h> //can be helpful to do compile-time version/capabilities identification
#endif                     //__unix__ || __APPLE__ || HAVE_SYSPARAM

//! \def DISABLE_WARNING_4255
//! \brief disabled MSVC warning 4255. Needed around including windows.h

//! \def RESTORE_WARNING_4255
//! \brief restores MSVC warning 4255 back to being enabled.
#if IS_MSVC_VERSION(MSVC_2010) && !defined(__clang__)
#    define DISABLE_WARNING_4255 __pragma(warning(push)) __pragma(warning(disable : 4255))

#    define RESTORE_WARNING_4255 __pragma(warning(pop))
#else
#    define DISABLE_WARNING_4255
#    define RESTORE_WARNING_4255
#endif //_MSVC && !clang workaround for Windows API headers

#if defined(_WIN32)
#    include <sdkddkver.h>
DISABLE_WARNING_4255
#    include <windows.h>
RESTORE_WARNING_4255
#    include <winsdkver.h>
// This pragma is needed to tell a library including opensea-common to look for
// Version.lib for the version helping information
//  This only works in MSVC which is why mingw gets filtered out below.
//  Meson/makefile projects will need to link these manually.
// NOTE: ARM requires 10.0.16299.0 API to get this library!
#    if !defined(__MINGW32__) && !defined(__MINGW64__)
#        pragma comment(lib, "Version.lib")  // for getting Windows system versions
#        pragma comment(lib, "Advapi32.lib") // WinPE check and windows 10 version check
#    endif

#    if defined(WIN_API_TARGET_VERSION)
#        undef WIN_API_TARGET_VERSION
#        pragma message(                                                                                               \
            "WIN_API_TARGET_VERSION no longer needs to be defined. Automatically defined from sdkddkver.h now")
#    endif

// The macros for NTDDI_VERSION & _WIN32_WINNT & WINVER are rarely checked once
// in Win 10 since they all get set to the same thing, so the below targets
// should also be defined in the preprocessor so that we know which version of
// the WIN10 SDK is being targeted.
#    if defined(WDK_NTDDI_VERSION)
#        define WIN_API_TARGET_VERSION WDK_NTDDI_VERSION
#    else
// Check which, if any older NTDDI macros are available and set the
// highest...likely an older compiler or SDK version has been found that doesn't
// set the above macro like WIN10 SDKs
#        if defined(NTDDI_WINBLUE)
#            define WIN_API_TARGET_VERSION NTDDI_WINBLUE
#        elif defined(NTDDI_WIN8)
#            define WIN_API_TARGET_VERSION NTDDI_WIN8
#        elif defined(NTDDI_WIN7)
#            define WIN_API_TARGET_VERSION NTDDI_WIN7
#        elif defined(NTDDI_WIN6SP4)
#            define WIN_API_TARGET_VERSION NTDDI_WIN6SP4
#        elif defined(NTDDI_WIN6SP3)
#            define WIN_API_TARGET_VERSION NTDDI_WIN6SP3
#        elif defined(NTDDI_WIN6SP2)
#            define WIN_API_TARGET_VERSION NTDDI_WIN6SP2
#        elif defined(NTDDI_WIN6SP1)
#            define WIN_API_TARGET_VERSION NTDDI_WIN6SP1
#        elif defined(NTDDI_WIN6)
#            define WIN_API_TARGET_VERSION NTDDI_WIN6
#        elif defined(NTDDI_WS03SP4)
#            define WIN_API_TARGET_VERSION NTDDI_WS03SP4
#        elif defined(NTDDI_WS03SP3)
#            define WIN_API_TARGET_VERSION NTDDI_WS03SP3
#        elif defined(NTDDI_WS03SP2)
#            define WIN_API_TARGET_VERSION NTDDI_WS03SP2
#        elif defined(NTDDI_WS03SP1)
#            define WIN_API_TARGET_VERSION NTDDI_WS03SP1
#        elif defined(NTDDI_WS03)
#            define WIN_API_TARGET_VERSION NTDDI_WS03
#        elif defined(NTDDI_WINXPSP4)
#            define WIN_API_TARGET_VERSION NTDDI_WINXPSP4
#        elif defined(NTDDI_WINXPSP3)
#            define WIN_API_TARGET_VERSION NTDDI_WINXPSP3
#        elif defined(NTDDI_WINXPSP2)
#            define WIN_API_TARGET_VERSION NTDDI_WINXPSP2
#        elif defined(NTDDI_WINXPSP1)
#            define WIN_API_TARGET_VERSION NTDDI_WINXPSP1
#        elif defined(NTDDI_WINXP)
#            define WIN_API_TARGET_VERSION NTDDI_WINXP
#        elif defined(NTDDI_WIN2KSP4)
#            define WIN_API_TARGET_VERSION NTDDI_WIN2KSP4
#        elif defined(NTDDI_WIN2KSP3)
#            define WIN_API_TARGET_VERSION NTDDI_WIN2KSP3
#        elif defined(NTDDI_WIN2KSP2)
#            define WIN_API_TARGET_VERSION NTDDI_WIN2KSP2
#        elif defined(NTDDI_WIN2KSP1)
#            define WIN_API_TARGET_VERSION NTDDI_WIN2KSP1
#        elif defined(NTDDI_WIN2K)
#            define WIN_API_TARGET_VERSION NTDDI_WIN2K
#        else
#            define WIN_API_TARGET_VERSION                                                                             \
                1 /*this is an unknown version...assume that no target API is                                          \
                     known*/
#            pragma message("WARNING: No Windows API detected! Things may not work at all!")
#        endif // checking NTDDI_versions
#    endif     // WDK_NTDDI_VERSION

#else //!_WIN32

#    define WIN_API_TARGET_VERSION 0

#endif //_WIN32

//
// _WIN32_WINNT version constants
//
#define SEA_WIN32_WINNT_NT4          0x0400 // Windows NT 4.0
#define SEA_WIN32_WINNT_WIN2K        0x0500 // Windows 2000
#define SEA_WIN32_WINNT_WINXP        0x0501 // Windows XP
#define SEA_WIN32_WINNT_WS03         0x0502 // Windows Server 2003
#define SEA_WIN32_WINNT_WIN6         0x0600 // Windows Vista
#define SEA_WIN32_WINNT_VISTA        0x0600 // Windows Vista
#define SEA_WIN32_WINNT_WS08         0x0600 // Windows Server 2008
#define SEA_WIN32_WINNT_LONGHORN     0x0600 // Windows Vista
#define SEA_WIN32_WINNT_WIN7         0x0601 // Windows 7
#define SEA_WIN32_WINNT_WIN8         0x0602 // Windows 8
#define SEA_WIN32_WINNT_WINBLUE      0x0603 // Windows 8.1
#define SEA_WIN32_WINNT_WINTHRESHOLD 0x0A00 // Windows 10
#define SEA_WIN32_WINNT_WIN10        0x0A00 // Windows 10

// SDK Versions can be found here:
// https://en.wikipedia.org/wiki/Microsoft_Windows_SDK Add more below as we need
// them These defines were added for comparison so you know which Windows API is
// being targetted. We defined these ourselves because it is confusing which
// Win10 API is being targetted since the naming convention is not well
// documented compared to how the packages are listed online to be installed. -
// TJE

#define WIN_API_TARGET_UNKNOWN         1 // Unknown target, or not defined by builder
#define WIN_API_TARGET_NT              0x04000000
#define WIN_API_TARGET_2000            0x05000000
#define WIN_API_TARGET_2000_SP1        0x05000100
#define WIN_API_TARGET_2000_SP2        0x05000200
#define WIN_API_TARGET_2000_SP3        0x05000300
#define WIN_API_TARGET_2000_SP4        0x05000400
#define WIN_API_TARGET_XP              0x05010000
#define WIN_API_TARGET_XP_SP1          0x05010100
#define WIN_API_TARGET_XP_SP2          0x05010200
#define WIN_API_TARGET_XP_SP3          0x05010300
#define WIN_API_TARGET_XP_SP4          0x05010400
#define WIN_API_TARGET_SERVER_2003     0x05020000
#define WIN_API_TARGET_SERVER_2003_SP1 0x05020100
#define WIN_API_TARGET_SERVER_2003_SP2 0x05020200
#define WIN_API_TARGET_SERVER_2003_SP3 0x05020300
#define WIN_API_TARGET_SERVER_2003_SP4 0x05020400
// Windows server 2008 and vista share API versions. Use Vista defines instead
// where possible - TJE
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
#define WIN_API_TARGET_WIN10_10240      0x0A000001 // 10.0.10240.0 //Win 10 API, build 10240
#define WIN_API_TARGET_WIN10_10586      0x0A000002 // 10.0.10586.0 //Win 10 API, build 10586
#define WIN_API_TARGET_WIN10_14393      0x0A000003 // 10.0.14393.0 //Win 10 API, build 14393
#define WIN_API_TARGET_WIN10_15063      0x0A000004 // 10.0.15063.0 //Win 10 API, build 15063
#define WIN_API_TARGET_WIN10_16299      0x0A000005 // 10.0.16299.0 //Win 10 API, build 16299
#define WIN_API_TARGET_WIN10_17134      0x0A000006 // 10.0.17134.0 //Win 10 API, build 17134
#define WIN_API_TARGET_WIN10_17763      0x0A000007 // 10.0.17763.0 //Win 10 API, build 17763
#define WIN_API_TARGET_WIN10_18362      0x0A000008 // 10.0.18362.0 //Win 10 API, build 18362
#define WIN_API_TARGET_WIN10_19041      0x0A000009 // 10.0.19041.0 //Win 10 API, build 19041
#define WIN_API_TARGET_WIN10_20348      0x0A00000A // 10.0.20348.0 //Win 10 API, build 20348
#define WIN_API_TARGET_WIN10_22000      0x0A00000B // 10.0.22000.0 //Win 10 API, build 22000
#define WIN_API_TARGET_WIN11_22621      0x0A00000C // 10.0.22621.755 //Win 11 API, build 22621
#define WIN_API_TARGET_WIN11_22621_1778 0x0A00000D // 10.0.22621.1778
#define WIN_API_TARGET_WIN11_22621_2428 0x0A00000E // 10.0.22621.2428
#define WIN_API_TARGET_WIN11_22621_3235 0x0A00000F // 10.0.22621.3235
#define WIN_API_TARGET_WIN11_26100      0x0A000010 // 10.0.26100.0 //Win 11 API, build 26100

#if defined(__cplusplus)
extern "C"
{
#endif

// Set macros to make it easier to check specific C standards compatibility.
// NOTE: Do not rely strictly on these macros for all features from all
// standards, especially in MSVC
//       Prior to VS2022 17.2
//       You may still need additional checks before using certain APIs or
//       features since there are a lot of missing pieces in some Windows
//       environements.
#if defined(__STDC__) || defined(__STDC_VERSION__)
#    define USING_C89
#    if defined(__STDC_VERSION__)
#        if __STDC_VERSION__ >= 199409L
#            define USING_C94
#        endif //__STDC_VERSION__ >= 199409L
#        if __STDC_VERSION__ >= 199901L
#            define USING_C99
#        endif //__STDC_VERSION__ >= 199901L
#        if __STDC_VERSION__ >= 201112L
#            define USING_C11
#        endif //__STDC_VERSION__ >= 201112L
#        if __STDC_VERSION__ >= 201710L
#            define USING_C17
#        endif //__STDC_VERSION__ >= 201710L
#        if __STDC_VERSION__ >= 202311L
#            define USING_C23
#        endif //__STDC_VERSION__ >= 202311L
#    endif     //__STDC_VERSION__
#elif defined(_MSC_VER) && !defined(__cplusplus)
// Set an approximate standard based on MSVC_VER
// https://learn.microsoft.com/en-us/cpp/overview/compiler-versions?view=msvc-170#service-releases-starting-with-visual-studio-2017
// https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
#    if _MSC_VER >= MSC_6
#        define USING_C89
#    endif //_MSC_VER >= MSC_6
#    if defined(_WCHAR_T_DEFINED)
// https://learn.microsoft.com/en-us/cpp/build/reference/zc-wchar-t-wchar-t-is-native-type?view=msvc-170
// NOTE: This may not be perfect, but it's the best thing I can find to
// check-TJE
#        define USING_C94
#    endif //_WCHAR_T_DEFINED
#    if _MSC_VER >= MSVC_2013
// NOTE: This is not exactly true, but close enough.
//       MSFT does not support VLAs which is the biggest missing piece of C99
//       support However, starting in MSVC2013 most everything else from C99 was
//       implemented. Setting this since we don't use VLAs and don't need the
//       missing math calls in our code base
#        define USING_C99
#    endif //_MSC_VER >= MSVC_2013
#    if _MSC_VER >= MSVC_2019 && _MSC_VER < MSVC_2022_17_2 && defined(_MSC_EXTENSIONS) &&                              \
        (defined(__STDC_NO_VLA__) || defined(__STDC_NO_ATOMICS__) || defined(__STDC_NO_COMPLEX__) ||                   \
         defined(__STDC_NO_THREADS__))
// NOTE: This is far from perfect.
//       VS2019 added a way to specify the standard, however the __STDC__ macros
//       above are only defined when using /Za until VS2022 17.2 where these are
//       defined when specifying /std::c11 or c17 So in this case, we are
//       detecting VS2019 running without /Za, and specifying C11 or C17 by also
//       checking __STDC_NO_VLA__ since that appears to be defined when
//       specifying C11 or C17 and using extensions. I'm sure this can be
//       further refined.
// NOTE: When using C11 or C17, MSFT documentation says these are implemented
// the same in their case
//       So set both of these version macros
//__STDC_HOSTED__???
#        define USING_C11
#        define USING_C17
#    endif // MSC_VER and lots of other flags to figure out if it's running in
           // C11/17 mode
// NOTE: Not sure when/if C23 will be supported, but hopefully it will show up
// under the standard macros above going forward-TJE
#endif     //__STDC__

#if defined(__cplusplus)
#    define USING_CPP98
#    if __cplusplus >= 201103L
#        define USING_CPP11
#    endif
#    if __cplusplus >= 201402L
#        define USING_CPP14
#    endif
#    if __cplusplus >= 201703L
#        define USING_CPP17
#    endif
#    if __cplusplus >= 202002L
#        define USING_CPP20
#    endif
#    if __cplusplus >= 202302L
#        define USING_CPP23
#    endif
#    if defined(_MSC_VER)
// Check _MSVC_LANG  as it may or maynot match __cplusplus in some versions of
// msvc This may not be an issue with VS2022, but it was an issue at least in
// VS2015
#        if !defined(USING_CPP11) && _MSVC_LANG >= 201103L
#            define USING_CPP11
#        endif
#        if !defined(USING_CPP14) && _MSVC_LANG >= 201402L
#            define USING_CPP14
#        endif
#        if !defined(USING_CPP17) && _MSVC_LANG >= 201703L
#            define USING_CPP17
#        endif
#        if !defined(USING_CPP20) && _MSVC_LANG >= 202002L
#            define USING_CPP20
#        endif
#        if !defined(USING_CPP23) && _MSVC_LANG >= 202302L
#            define USING_CPP23
#        endif
#    endif //_MSC_VER
#endif     //__cplusplus

// POSIX version checks. NOTE: not taking into account xopen/sus versions here,
// just posix May need to check for other optional posix features using these
// other macros: https://linux.die.net/man/7/posixoptions
#if defined(_POSIX_VERSION)
#    if _POSIX_VERSION >= 198808L
//! \def POSIX_1988
//! \brief Defined when POSIX 1988 is supported.
#        define POSIX_1988
#    endif
#    if _POSIX_VERSION >= 199009L
//! \def POSIX_1990
//! \brief Defined when POSIX 1990 is supported.
#        define POSIX_1990
#    endif
#    if defined(_POSIX2_C_VERSION) && _POSIX2_C_VERSION >= 199209L
//! \def POSIX_1992
//! \brief Defined when POSIX 1992 is supported.
#        define POSIX_1992
#    endif
#    if _POSIX_VERSION >= 199309L
//! \def POSIX_1993
//! \brief Defined when POSIX 1993 is supported.
#        define POSIX_1993
#    endif
#    if _POSIX_VERSION >= 199506L
//! \def POSIX_1996
//! \brief Defined when POSIX 1996 is supported.
#        define POSIX_1996
#    endif
#    if _POSIX_VERSION >= 200112L
//! \def POSIX_2001
//! \brief Defined when POSIX 2001 is supported.
#        define POSIX_2001
#    endif
#    if _POSIX_VERSION >= 200809L
//! \def POSIX_2008
//! \brief Defined when POSIX 2008 is supported.
#        define POSIX_2008
#    endif
#    if _POSIX_VERSION >= 201709L
//! \def POSIX_2017
//! \brief Defined when POSIX 2017 is supported.
#        define POSIX_2017
#    endif
#    if _POSIX_VERSION >= 202409L
//! \def POSIX_2024
//! \brief Defined when POSIX 2024 is supported.
#        define POSIX_2024
#    endif
#endif //_POSIX_VERSION

// detect X/open portability guide/single unix specification support
// X/Open is a portability guide
// This eventually became the single unix specifications which are defined under
// here as well.
#if defined(_XOPEN_VERSION)
// version 1 and 2 of XOpen?
// V1 published in 1985
// V2 published in 1987
#    if _XOPEN_VERSION >= 3
//! \def USING_XPG3
//! \brief Defined when X/open portability guide version 3 is supported
#        define USING_XPG3 /* 1989 */
#    endif
#    if _XOPEN_VERSION >= 4
//! \def USING_XPG4
//! \brief Defined when X/open portability guide version 4 is supported
#        define USING_XPG4 /* 1992 */
#        if defined(_XOPEN_UNIX)
//! \def USING_SUS
//! \brief Defined when Single Unix Specification is supported
#            define USING_SUS /* UNIX95 */
#        endif
#    endif
#    if _XOPEN_VERSION >= 500
//! \def USING_SUS2
//! \brief Defined when Single Unix Specification version 2 is supported
#        define USING_SUS2 /* UNIX98 */
#    endif
#    if _XOPEN_VERSION >= 600
//! \def USING_SUS3
//! \brief Defined when Single Unix Specification version 3 is supported
#        define USING_SUS3 /* UNIX03 */
#    endif
#    if _XOPEN_VERSION >= 700
//! \def USING_SUS4
//! \brief Defined when Single Unix Specification version 4 is supported
#        define USING_SUS4
#    endif
#endif //_XOPEN_VERSION
    // NOTE: Some functions that were added to posix, xopen, sus may have come from
    // BSD 4.x. Use sys/param.h to check for these old bsd versions

#if defined(__sysv__) || defined(__SVR4) || defined(__svr4__)
//! \def SVR4_ENV
//! \brief Unix System 5 compatible environment
#    define SVR4_ENV
#endif

// Detect the LP64 pr LLP64 mode for the system/compiler.
// This is meant to be used within various opensea-common functions, but defined
// in this header so that it can be used easily for other uses
// https://en.cppreference.com/w/c/language/arithmetic_types discusses data models
// More discussion on data models: https://archive.opengroup.org/public/tech/aspen/lp64_wp.htm
#if defined(__LP64__) || defined(_LP64)
#    define LP64_DATA_MODEL
#elif defined(_WIN32)
#    if defined(_WIN64)
#        define LLP64_DATA_MODEL
#    else
#        define ILP32_DATA_MODEL
#    endif
#elif defined(__ILP32__) || defined(_ILP32)
#    define ILP32_DATA_MODEL
#elif defined(__ILP64__) || defined(_ILP64)
#    define ILP64_DATA_MODEL
#elif defined(__LP32__) || defined(_LP32) || defined(_WIN16)
#    define LP32_DATA_MODEL
#else
// Add more data model info here as necessary.
// If a data model is not already defined above, then some built in logic is
// used to decide functions to call in some scenarios.
// Check type sizes to determine the data model???
#    define MISSING_DATA_MODEL_INFO
#endif

#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__))
//! \def HAVE_C11_ANNEX_K
//! \brief C11 Annex K functions are available
#    define HAVE_C11_ANNEX_K
#endif // check for annex k

#if defined(__STDC_SECURE_LIB__) || defined(__GOT_SECURE_LIB__)
//! \def HAVE_MSFT_SECURE_LIB
//! \brief Microsoft secure library functions are available (_s)
#    define HAVE_MSFT_SECURE_LIB
#endif // Microsoft secure lib

// Detect the system we are compiling for as best we can
#if defined(UEFI_C_SOURCE)
#    define SYSTEM_UEFI
#elif defined(__linux__)
#    define SYSTEM_LINUX
#elif defined(__DragonFly__)
#    define SYSTEM_DRAGONFLY
#elif defined(__FreeBSD__)
#    define SYSTEM_FREEBSD
#elif defined(__NetBSD__)
#    define SYSTEM_NETBSD
#elif defined(__OpenBSD__)
#    define SYSTEM_OPENBSD
#elif defined(__illumos__)
#    define SYSTEM_ILLUMOS
#elif defined(__sun)
#    define SYSTEM_SOLARIS
#elif defined(_WIN32)
#    define SYSTEM_WINDOWS
#elif defined(_AIX) // IBM Unix
#    define SYSTEM_AIX
#elif defined(__hpux) // HP Unix
#    define SYSTEM_HPUX
#elif defined(__APPLE__)
#    include <TargetConditionals.h>
#    if defined(TARGET_OS_MAC)
#        define SYSTEM_MACOS
#    else
// embedded apple os
#        define SYSTEM_EMBEDDED_APPLE_OS
#    endif
#elif defined(__digital__) // tru64 unix
#    define SYSTEM_TRU64
#elif defined(__CYGWIN__) && !defined(_WIN32)
#    define SYSTEM_CYGWIN
#else
#    define SYSTEM_UNKNOWN
#endif

// Macros to simplify version checks for operating systems.
// NOTE: Not every OS will have a macro.
// For example: AIX and openBSD you can do defined(_AIX(M)(m)) to check a version.
//     where M is a major version and m is a minor version
// Examples: defined (_AIX43) to check for AIX 4.3
//           defined (OpenBSD3_9) to check for OpenBSD 3.9
// OS's with version check macros are those that encode a version into a macro as a numeric value
// Example: FreeBSD, NetBSD

//! \def IS_FREEBSD_VERSION(major, minor, patch)
//! \brief Macro to check full FreeBSD version (major.minor.patch)
//! \param[in] major major version of FreeBSD to check for
//! \param[in] minor minor version of FreeBSD to check for
//! \param[in] patch patch version of FreeBSD to check for
#if defined(__FreeBSD__) && defined(__FreeBSD_version)
#    define FREEBSD_FULL_VERSION_ENCODE(major, minor, revision) ((major)*100000 + (minor)*1000 + (revision))
#    define IS_FREEBSD_VERSION(major, minor, revision)                                                                 \
        (__FreeBSD_version >= FREEBSD_FULL_VERSION_ENCODE(major, minor, revision))
#else
#    define IS_FREEBSD_VERSION(major, minor, revision) (0)
#endif

//! \def IS_NETBSD_VERSION(major, minor, patch)
//! \brief Macro to check full NetBSD version (major.minor.patch)
//! \param[in] major major version of NetBSD to check for
//! \param[in] minor minor version of NetBSD to check for
//! \param[in] patch patch version of NetBSD to check for
#if defined(__NetBSD__) && defined(__NetBSD_Version__)
#    define NETBSD_VERSION_ENCODE(major, minor, patch) ((major)*100000000 + (minor)*1000000 + (patch)*100)
#    define IS_NETBSD_VERSION(major, minor, patch)     (__NetBSD_Version__ >= NETBSD_VERSION_ENCODE(major, minor, patch))
#else
#    define IS_NETBSD_VERSION(major, minor, patch) (0)
#endif

// Clang supports these other methods to check for generic selection support
//__has_feature(c_generic_selections) or  __has_extension(c_generic_selections)

//! \def HAVE_C11_GENERIC_SELECTION
//! \brief Macro to check for C11 Generic selection macro support
#if defined(USING_C11) && !defined(HAVE_C11_GENERIC_SELECTION)
// workaround for early C11 compilers that may still be used with this code to
// disable generic selection when not supported
#    if IS_GCC_VERSION(4, 9)
#        define HAVE_C11_GENERIC_SELECTION
#    elif IS_CLANG_VERSION(3, 3)
#        define HAVE_C11_GENERIC_SELECTION
#    elif IS_MSVC_VERSION(MSVC_2019_16_8)
/* SDK version 10.0.20348.0 is technically when it arrived, but you also need
 * VS2019 16.8 and later */
/*TODO: Move the Windows SDK version detection to this file and add that check
 * to the ifdef above */
#        define HAVE_C11_GENERIC_SELECTION
#    endif
#endif // C11 & !generic selection

#if defined(_WIN64) || defined(_M_IA64) || defined(_M_ALPHA) || defined(_M_X64) || defined(_M_AMD64) ||                \
    defined(__alpha__) || defined(__amd64__) || defined(__x86_64__) || defined(__aarch64__) || defined(__ia64__) ||    \
    defined(__IA64__) || defined(__powerpc64__) || defined(__PPC64__) || defined(__ppc64__) ||                         \
    defined(_ARCH_PPC64) // 64bit
//! \def ENV_64BIT
//! \brief This is set when compiling in a 64bit environment
#    define ENV_64BIT
#else // CPU Check
//! \def ENV_32BIT
//! \brief This is set when compiling in a 32bit environment
#    define ENV_32BIT
#endif // CPU Check

//! \def ENV_BIG_ENDIAN
//! \brief This is set when compiling in a big endian environment

//! \def ENV_LITTLE_ENDIAN
//! \brief This is set when compiling in a little endian environment
#if defined(__has_include)
#    if __has_include(<endian.h>)
#        include <endian.h>
#    elif __has_include(<sys/endian.h>)
#        include <sys/endian.h>
#    endif
#elif defined(POSIX_2024)
#    include <endian.h>
#endif
#if defined(__BYTE_ORDER__)
#    if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#        define ENV_BIG_ENDIAN
#    elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#        define ENV_LITTLE_ENDIAN
#    endif
#endif
#if !defined(ENV_BIG_ENDIAN) && !defined(ENV_LITTLE_ENDIAN)
#    if defined(__BIG_ENDIAN__)
#        define ENV_BIG_ENDIAN
#    elif defined(__LITTLE_ENDIAN__)
#        define ENV_LITTLE_ENDIAN
#    else
// check architecture specific defines...
#        if defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIPSEB) ||                \
            defined(__MIPSEB) || defined(__MIPSEB__)
#            define ENV_BIG_ENDIAN
#        elif defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) ||              \
            defined(__MIPSEL) || defined(__MIPSEL__) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_ALPHA) ||   \
            defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARMT) || defined(_M_IX86) || defined(_M_IA64) ||        \
            defined(_M_PPC) /* This is a special Windows case for PPC as NT ran it in little endian mode */            \
            || defined(MDE_CPU_X64) || defined(MDE_CPU_IA32) || defined(MDE_CPU_ARM) || defined(MDE_CPU_AARCH64)
#            define ENV_LITTLE_ENDIAN
#        else
    // If you get to here, you will need to update the methods to check the endianness above for your compiler/platform.
#            error "Unknown endianness. Please update the definitions to properly detect endianness"
#        endif
#    endif
#endif

//! \def DISABLE_WARNING_4146
//! \brief disables MSVC warning 4146.
//! \details These warning disables are only needed in MSVC for the C11 generic min/max implementations.
//! Without them you get a warning about applying a unary - on an unsigned type.

//! \def RESTORE_WARNING_4146
//! \brief restores MSVC warning 4146 back to being enabled.
//! \details These warning disables are only needed in MSVC for the C11 generic min/max implementations.
//! Without them you get a warning about applying a unary - on an unsigned type.
#if IS_MSVC_VERSION(MSVC_2012) && !defined(__clang__)
#    define DISABLE_WARNING_4146 __pragma(warning(push)) __pragma(warning(disable : 4146))
#    define RESTORE_WARNING_4146 __pragma(warning(pop))
#else
#    define DISABLE_WARNING_4146
#    define RESTORE_WARNING_4146
#endif //_MSVC && !clang workaround for min/max macros

//! \def DISABLE_WARNING_SIGN_CONVERSION
//! \brief Disables warning about sign conversion

//! \def RESTORE_WARNING_SIGN_CONVERSION
//! \brief Restored warning about sign conversion
#if IS_CLANG_VERSION(3, 0)
#    define DISABLE_WARNING_SIGN_CONVERSION                                                                            \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wsign-conversion\"")
#    define RESTORE_WARNING_SIGN_CONVERSION _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(4, 3)
#    define DISABLE_WARNING_SIGN_CONVERSION                                                                            \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wsign-conversion\"")
#    define RESTORE_WARNING_SIGN_CONVERSION _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_WARNING_SIGN_CONVERSION
#    define RESTORE_WARNING_SIGN_CONVERSION
#endif

//! \def DISABLE_WARNING_ZERO_LENGTH_ARRAY
//! \brief Disables warning about a zero length array at the end of a structure

//! \def RESTORE_WARNING_ZERO_LENGTH_ARRAY
//! \brief Restores warning about a zero length array at the end of a structure
#if IS_CLANG_VERSION(3, 0)
#    define DISABLE_WARNING_ZERO_LENGTH_ARRAY                                                                          \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wzero-length-array\"")
#    define RESTORE_WARNING_ZERO_LENGTH_ARRAY _Pragma("clang diagnostic pop")
#elif IS_MSVC_VERSION(MSVC_2005)
#    define DISABLE_WARNING_ZERO_LENGTH_ARRAY __pragma(warning(push)) __pragma(warning(disable : 4200))
#    define RESTORE_WARNING_ZERO_LENGTH_ARRAY __pragma(warning(pop))
#else
#    define DISABLE_WARNING_ZERO_LENGTH_ARRAY
#    define RESTORE_WARNING_ZERO_LENGTH_ARRAY
#endif

//! \def DISABLE_WARNING_FORMAT_NONLITERAL
//! \brief Disables warning about a using something other than a string literal as a format string.

//! \def RESTORE_WARNING_FORMAT_NONLITERAL
//! \brief Restores warning about a using something other than a string literal as a format string.
#if IS_CLANG_VERSION(2, 6)
#    define DISABLE_WARNING_FORMAT_NONLITERAL                                                                          \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wformat-nonliteral\"")
#    define RESTORE_WARNING_FORMAT_NONLITERAL _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(3, 4)
#    define DISABLE_WARNING_FORMAT_NONLITERAL                                                                          \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wformat-nonliteral\"")
#    define RESTORE_WARNING_FORMAT_NONLITERAL _Pragma("GCC diagnostic pop")
#elif IS_MSVC_VERSION(MSVC_2005)
#    define DISABLE_WARNING_FORMAT_NONLITERAL __pragma(warning(push)) __pragma(warning(disable : 4774))
#    define RESTORE_WARNING_FORMAT_NONLITERAL __pragma(warning(pop))
#else
#    define DISABLE_WARNING_FORMAT_NONLITERAL
#    define RESTORE_WARNING_FORMAT_NONLITERAL
#endif

//! \def DISABLE_WARNING_CPP11_COMPAT
//! \brief Disables warning about using something that may collide with a part of the C++11 standard.
//! This is used around a null pointer type/class for C++98

//! \def RESTORE_WARNING_CPP11_COMPAT
//! \brief Restores warning about using something that may collide with a part of the C++11 standard.
//! This is used around a null pointer type/class for C++98
#if IS_CLANG_VERSION(1, 0)
#    define DISABLE_WARNING_CPP11_COMPAT                                                                               \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wc++0x-compat\"")
#    define RESTORE_WARNING_CPP11_COMPAT _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(4, 7)
#    define DISABLE_WARNING_CPP11_COMPAT                                                                               \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wc++11-compat\"")
#    define RESTORE_WARNING_CPP11_COMPAT _Pragma("GCC diagnostic pop")
#elif IS_GCC_FULL_VERSION(4, 3, 6)
#    define DISABLE_WARNING_CPP11_COMPAT                                                                               \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wc++0x-compat\"")
#    define RESTORE_WARNING_CPP11_COMPAT _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_WARNING_CPP11_COMPAT
#    define RESTORE_WARNING_CPP11_COMPAT
#endif

//! \def DISABLE_WARNING_FLOAT_EQUAL
//! \brief Disables warning about using == for comparing double/float values.
//! Generally you should never do this. There are rare uses which is why this needs to be able to be
//! disabled and restored in those rare cases.

//! \def RESTORE_WARNING_FLOAT_EQUAL
//! \brief Restores warning about using == for comparing double/float values.
//! Generally you should never do this. There are rare uses which is why this needs to be able to be
//! disabled and restored in those rare cases.
#if IS_CLANG_VERSION(3, 0)
#    define DISABLE_WARNING_FLOAT_EQUAL                                                                                \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wfloat-equal\"")
#    define RESTORE_WARNING_FLOAT_EQUAL _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(4, 5)
#    define DISABLE_WARNING_FLOAT_EQUAL                                                                                \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")
#    define RESTORE_WARNING_FLOAT_EQUAL _Pragma("GCC diagnostic pop")
#elif IS_GCC_FULL_VERSION(4, 3, 6)
#    define DISABLE_WARNING_FLOAT_EQUAL                                                                                \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wfloat-equal\"")
#    define RESTORE_WARNING_FLOAT_EQUAL _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_WARNING_FLOAT_EQUAL
#    define RESTORE_WARNING_FLOAT_EQUAL
#endif

//! \def DISABLE_NONNULL_COMPARE
//! \brief Disables warning comparing a pointer to NULL when it is marked with an attribute as must be non-null.
//! This is used internally within safe_ functions to maintain null pointer checks for bounds checking

//! \def RESTORE_NONNULL_COMPARE
//! \brief Restores Disables warning comparing a pointer to NULL when it is marked with an attribute as must be
//! non-null. This is used internally within safe_ functions to maintain null pointer checks for bounds checking
#if IS_CLANG_VERSION(3, 2)
#    define DISABLE_NONNULL_COMPARE                                                                                    \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wtautological-pointer-compare\"")
#    define RESTORE_NONNULL_COMPARE _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(7, 1)
#    define DISABLE_NONNULL_COMPARE                                                                                    \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wnonnull-compare\"")
#    define RESTORE_NONNULL_COMPARE _Pragma("GCC diagnostic pop")
#elif IS_GCC_VERSION(4, 3)
#    define DISABLE_NONNULL_COMPARE _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wnonnull\"")
#    define RESTORE_NONNULL_COMPARE _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_NONNULL_COMPARE
#    define RESTORE_NONNULL_COMPARE
#endif

#if defined(__INTELLISENSE__) || defined(__clang_analyzer__) || defined(__CDT_PARSER__)
//! \def DEV_ENVIRONMENT
//! \brief Defined to help create different help pop-ups within an IDE.
//!
//! This is used around the safe_ functions that call the constraint handler to popup type
//! information in the IDE, and at compile time use a macro instead.
#    define DEV_ENVIRONMENT
#endif

// Built-in / intrinsic functions detection
#if defined(__has_builtin)
#    if __has_builtin(__builtin_object_size)
#        define HAVE_BUILT_IN_OBJ_SIZE
#    endif
#    if __has_builtin(__builtin_bswap64)
#        define HAVE_BUILTIN_BSWAP
#    endif
#    if __has_builtin(__builtin_memchr)
#        define HAVE_BUILTIN_MEMCHR
#    endif
#    if __has_builtin(__builtin_memcmp)
#        define HAVE_BUILTIN_MEMCMP
#    endif
#    if __has_builtin(__builtin_memcpy)
#        define HAVE_BUILTIN_MEMCPY
#    endif
#    if __has_builtin(__builtin_memmove)
#        define HAVE_BUILTIN_MEMMOVE
#    endif
#    if __has_builtin(__builtin_memset)
#        define HAVE_BUILTIN_MEMSET
#    endif
#    if __has_builtin(__builtin_crc32_data8)
#        define HAVE_BUILT_IN_CRC32_DATA8
#    endif
#    if __has_builtin(__builtin_crc16_data8)
#        define HAVE_BUILT_IN_CRC16_DATA8
#    endif
#    if __has_builtin(__builtin_clzll)
#        define HAVE_BUILT_IN_CLZLL
#    endif
#    if __has_builtin(__builtin_ctzll)
#        define HAVE_BUILT_IN_CTZLL
#    endif
#    if __has_builtin(__builtin_popcountll)
#        define HAVE_BUILT_IN_POPCOUNTLL
#    endif
#    if __has_builtin(__builtin_clzl)
#        define HAVE_BUILT_IN_CLZL
#    endif
#    if __has_builtin(__builtin_ctzl)
#        define HAVE_BUILT_IN_CTZL
#    endif
#    if __has_builtin(__builtin_popcountl)
#        define HAVE_BUILT_IN_POPCOUNTL
#    endif
#    if __has_builtin(__builtin_clz)
#        define HAVE_BUILT_IN_CLZ
#    endif
#    if __has_builtin(__builtin_ctz)
#        define HAVE_BUILT_IN_CTZ
#    endif
#    if __has_builtin(__builtin_popcount)
#        define HAVE_BUILT_IN_POPCOUNTLL
#    endif
#    if __has_builtin(__builtin_clzg)
#        define HAVE_BUILT_IN_CLZG
#    endif
#    if __has_builtin(__builtin_ctzg)
#        define HAVE_BUILT_IN_CTZG
#    endif
#    if __has_builtin(__builtin_popcountg)
#        define HAVE_BUILT_IN_POPCOUNTG
#    endif
#    if __has_builtin(__builtin_stdc_bit_ceil)
#        define HAVE_BUILT_IN_STDC_BIT_CEIL
#    endif
#    if __has_builtin(__builtin_stdc_bit_floor)
#        define HAVE_BUILT_IN_STDC_BIT_FLOOR
#    endif
#    if __has_builtin(__builtin_stdc_bit_width)
#        define HAVE_BUILT_IN_STDC_BIT_WIDTH
#    endif
#    if __has_builtin(__builtin_stdc_count_ones)
#        define HAVE_BUILT_IN_STDC_COUNT_ONES
#    endif
#    if __has_builtin(__builtin_stdc_count_zeros)
#        define HAVE_BUILT_IN_STDC_COUNT_ZEROS
#    endif
#    if __has_builtin(__builtin_stdc_first_leading_one)
#        define HAVE_BUILT_IN_STDC_FIRST_LEADING_ONE
#    endif
#    if __has_builtin(__builtin_stdc_first_leading_zero)
#        define HAVE_BUILT_IN_STDC_FIRST_LEADING_ZERO
#    endif
#    if __has_builtin(__builtin_stdc_first_trailing_one)
#        define HAVE_BUILT_IN_STDC_FIRST_TRAILING_ONE
#    endif
#    if __has_builtin(__builtin_stdc_first_trailing_zero)
#        define HAVE_BUILT_IN_STDC_FIRST_TRAILING_ZERO
#    endif
#    if __has_builtin(__builtin_stdc_has_single_bit)
#        define HAVE_BUILT_IN_STDC_HAS_SINGLE_BIT
#    endif
#    if __has_builtin(__builtin_stdc_leading_ones)
#        define HAVE_BUILT_IN_STDC_LEADING_ONES
#    endif
#    if __has_builtin(__builtin_stdc_leading_zeros)
#        define HAVE_BUILT_IN_STDC_LEADING_ZEROS
#    endif
#    if __has_builtin(__builtin_stdc_trailing_ones)
#        define HAVE_BUILT_IN_STDC_TRAILING_ONES
#    endif
#    if __has_builtin(__builtin_stdc_trailing_zeros)
#        define HAVE_BUILT_IN_STDC_TRAILING_ZEROS
#    endif
#    if __has_builtin(__builtin_stdc_rotate_left)
#        define HAVE_BUILT_IN_STDC_ROTATE_LEFT
#    endif
#    if __has_builtin(__builtin_stdc_rotate_right)
#        define HAVE_BUILT_IN_STDC_ROTATE_RIGHT
#    endif
#endif //__has_builtin

#if !defined(HAVE_BUILT_IN_OBJ_SIZE) && IS_GCC_VERSION(4, 1)
#    define HAVE_BUILT_IN_OBJ_SIZE
#endif

#if IS_GCC_VERSION(3, 4)
#    if !defined(HAVE_BUILT_IN_CLZ)
#        define HAVE_BUILT_IN_CLZ
#    endif
#    if !defined(HAVE_BUILT_IN_CLZL)
#        define HAVE_BUILT_IN_CLZL
#    endif
#    if !defined(HAVE_BUILT_IN_CLZLL)
#        define HAVE_BUILT_IN_CLZLL
#    endif
#    if !defined(HAVE_BUILT_IN_CTZ)
#        define HAVE_BUILT_IN_CTZ
#    endif
#    if !defined(HAVE_BUILT_IN_CTZL)
#        define HAVE_BUILT_IN_CTZL
#    endif
#    if !defined(HAVE_BUILT_IN_CTZLL)
#        define HAVE_BUILT_IN_CTZLL
#    endif
#    if !defined(HAVE_BUILT_IN_POPCOUNT)
#        define HAVE_BUILT_IN_POPCOUNT
#    endif
#    if !defined(HAVE_BUILT_IN_POPCOUNTL)
#        define HAVE_BUILT_IN_POPCOUNTL
#    endif
#    if !defined(HAVE_BUILT_IN_POPCOUNTLL)
#        define HAVE_BUILT_IN_POPCOUNTLL
#    endif
#endif

#if !defined(HAVE_BUILTIN_BSWAP) && IS_GCC_VERSION(4, 8)
// GCC 4.8 and Clang 3.2 and later have some built-ins:
// https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html Otherwise rely on our
// own bit shifting.
#    define HAVE_BUILTIN_BSWAP
#endif

#if defined(__cplusplus)
}
#endif
