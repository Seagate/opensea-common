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
// \file predef_env_detect.h
// \brief Detects the compilation environment for standards, extensions, etc
// using only predefined macros from libc/compiler
//

#pragma once

#if !defined(__STDC_WANT_LIB_EXT1__)
// asking to get C11 _s functions since there is some ability to use them in
// some places.
#define __STDC_WANT_LIB_EXT1__ 1
#endif
#if !defined(__STDC_WANT_LIB_EXT2__)
// asking to get dynamic memory functions (mostly strdup) in C11. Only
// guaranteed available if __STDC_ALLOC_LIB__ defined, but asking anyways...most
// systems we support will have this-TJE
#define __STDC_WANT_LIB_EXT2__ 1
#endif

#if defined(__unix__) || defined(__APPLE__) || defined(HAVE_UNISTD)
#include <unistd.h> //to ensure we can check for POSIX versions
#endif              //__unix__ || __APPLE__ || HAVE_UNISTD

#if defined(__unix__) || defined(__APPLE__) || defined(HAVE_SYSPARAM)
#include <sys/param.h> //can be helpful to do compile-time version/capabilities identification
#endif                 //__unix__ || __APPLE__ || HAVE_SYSPARAM

#if defined(_WIN32)
#include <windows.h>
// This pragma is needed to tell a library including opensea-common to look for
// Version.lib for the version helping information
//  This only works in MSVC which is why mingw gets filtered out below.
//  Meson/makefile projects will need to link these manually.
// NOTE: ARM requires 10.0.16299.0 API to get this library!
#if !defined(__MINGW32__) && !defined(__MINGW64__)
#pragma comment(lib, "Version.lib")  // for getting Windows system versions
#pragma comment(lib, "Advapi32.lib") // WinPE check and windows 10 version check
#endif
#endif //_WIN32

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
#if defined(__STDC__)
#define USING_C89
#if defined(__STDC_VERSION__)
#if __STDC_VERSION__ >= 199409L
#define USING_C94
#endif //__STDC_VERSION__ >= 199409L
#if __STDC_VERSION__ >= 199901L
#define USING_C99
#endif //__STDC_VERSION__ >= 199901L
#if __STDC_VERSION__ >= 201112L
#define USING_C11
#endif //__STDC_VERSION__ >= 201112L
#if __STDC_VERSION__ >= 201710L
#define USING_C17
#endif //__STDC_VERSION__ >= 201710L
#if __STDC_VERSION__ >= 202311L
#define USING_C23
#endif //__STDC_VERSION__ >= 202311L
#endif //__STDC_VERSION__
#elif defined(_MSC_VER) && !defined(__cplusplus)
// Set an approximate standard based on MSVC_VER
// https://learn.microsoft.com/en-us/cpp/overview/compiler-versions?view=msvc-170#service-releases-starting-with-visual-studio-2017
// https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
#if _MSC_VER >= 600
#define USING_C89
#endif //_MSC_VER >= 600
#if defined(_WCHAR_T_DEFINED)
// https://learn.microsoft.com/en-us/cpp/build/reference/zc-wchar-t-wchar-t-is-native-type?view=msvc-170
// NOTE: This may not be perfect, but it's the best thing I can find to
// check-TJE
#define USING_C94
#endif //_WCHAR_T_DEFINED
#if _MSC_VER >= 1800
// NOTE: This is not exactly true, but close enough.
//       MSFT does not support VLAs which is the biggest missing piece of C99
//       support However, starting in MSVC2013 most everything else from C99 was
//       implemented. Setting this since we don't use VLAs and don't need the
//       missing math calls in our code base
#define USING_C99
#endif //_MSC_VER >= 1800
#if _MSC_VER >= 1920 && _MSC_VER < 1932 && defined(_MSC_EXTENSIONS) &&                                                 \
    (defined(__STDC_NO_VLA__) || defined(__STDC_NO_ATOMICS__) || defined(__STDC_NO_COMPLEX__) ||                       \
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
#define USING_C11
#define USING_C17
#endif // MSC_VER and lots of other flags to figure out if it's running in
       // C11/17 mode
// NOTE: Not sure when/if C23 will be supported, but hopefully it will show up
// under the standard macros above going forward-TJE
#endif //__STDC__

#if defined(__cplusplus)
#define USING_CPP98
#if __cplusplus >= 201103L
#define USING_CPP11
#endif
#if __cplusplus >= 201402L
#define USING_CPP14
#endif
#if __cplusplus >= 201703L
#define USING_CPP17
#endif
#if __cplusplus >= 202002L
#define USING_CPP20
#endif
#if __cplusplus >= 202302L
#define USING_CPP23
#endif
#if defined(_MSC_VER)
// Check _MSVC_LANG  as it may or maynot match __cplusplus in some versions of
// msvc This may not be an issue with VS2022, but it was an issue at least in
// VS2015
#if !defined(USING_CPP11) && _MSVC_LANG >= 201103L
#define USING_CPP11
#endif
#if !defined(USING_CPP14) && _MSVC_LANG >= 201402L
#define USING_CPP14
#endif
#if !defined(USING_CPP17) && _MSVC_LANG >= 201703L
#define USING_CPP17
#endif
#if !defined(USING_CPP20) && _MSVC_LANG >= 202002L
#define USING_CPP20
#endif
#if !defined(USING_CPP23) && _MSVC_LANG >= 202302L
#define USING_CPP23
#endif
#endif //_MSC_VER
#endif //__cplusplus

// POSIX version checks. NOTE: not taking into account xopen/sus versions here,
// just posix May need to check for other optional posix features using these
// other macros: https://linux.die.net/man/7/posixoptions
#if defined(_POSIX_VERSION)
#if _POSIX_VERSION >= 198808L
#define POSIX_1988
#endif
#if _POSIX_VERSION >= 199009L
#define POSIX_1990
#endif
#if defined(_POSIX2_C_VERSION) && _POSIX2_C_VERSION >= 199209L
#define POSIX_1992
#endif
#if _POSIX_VERSION >= 199309L
#define POSIX_1993
#endif
#if _POSIX_VERSION >= 199506L
#define POSIX_1996
#endif
#if _POSIX_VERSION >= 200112L
#define POSIX_2001
#endif
#if _POSIX_VERSION >= 200809L
#define POSIX_2008
#endif
#endif //_POSIX_VERSION

// detect X/open portability guide/single unix specification support
// X/Open is a portability guide
// This eventually became the single unix specifications which are defined under
// here as well.
#if defined(_XOPEN_VERSION)
// version 1 and 2 of XOpen?
// V1 published in 1985
// V2 published in 1987
#if _XOPEN_VERSION >= 3
#define USING_XPG3 // 1989
#endif
#if _XOPEN_VERSION >= 4
#define USING_XPG4 // 1992
#if defined(_XOPEN_UNIX)
#define USING_SUS // UNIX95
#endif
#endif
#if _XOPEN_VERSION >= 500
#define USING_SUS2 // UNIX98
#endif
#if _XOPEN_VERSION >= 600
#define USING_SUS3 // UNIX03
#endif
#if _XOPEN_VERSION >= 700
#define USING_SUS4
#endif
#endif //_XOPEN_VERSION
// NOTE: Some functions that were added to posix, xopen, sus may have come from
// BSD 4.x. Use sys/param.h to check for these old bsd versions

// Detect the LP64 pr LLP64 mode for the system/compiler.
// This is meant to be used within various opensea-common functions, but defined
// in this header so that it can be used easily for other uses
#if defined(__LP64__) || defined(_LP64)
#define LP64_DATA_MODEL
#elif defined(_WIN32)
#if defined(_WIN64)
#define LLP64_DATA_MODEL
#else
#define ILP32_DATA_MODEL
#endif
#elif defined(__ILP32__) || defined(_ILP32)
#define ILP32_DATA_MODEL
#elif defined(__ILP64__) || defined(_ILP64)
#define ILP64_DATA_MODEL
#elif defined(__LP32__) || defined(_LP32)
#define LP32_DATA_MODEL
#else
// Add more data model info here as necessary.
// If a data model is not already defined above, then some built in logic is
// used to decide functions to call in some scenarios.
#endif

#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__))
#define HAVE_C11_ANNEX_K
// NOTE: These are _s functions, but they MAY NOT MATCH the _s functions
// Microsoft has available.
//       Some have different parameters or different parameter order among other
//       things. Some do match and are the same. To check for Microsoft's
//       version, check for __STDC_SECURE_LIB__
#endif // check for annex k

// Detect the system we are compiling for as best we can
#if defined(UEFI_C_SOURCE)
#define SYSTEM_UEFI
#elif defined(__linux__)
#define SYSTEM_LINUX
#elif defined(__DragonFly__)
#define SYSTEM_DRAGONFLY
#elif defined(__FreeBSD__)
#define SYSTEM_FREEBSD
#elif defined(__NetBSD__)
#define SYSTEM_NETBSD
#elif defined(__OpenBSD__)
#define SYSTEM_OPENBSD
#elif defined(__illumos__)
#define SYSTEM_ILLUMOS
#elif defined(__sun)
#define SYSTEM_SOLARIS
#elif defined(_WIN32)
#define SYSTEM_WINDOWS
#elif defined(_AIX) // IBM Unix
#define SYSTEM_AIX
#elif defined(__hpux) // HP Unix
#define SYSTEM_HPUX
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if defined(TARGET_OS_MAC)
#define SYSTEM_MACOS
#else
// embedded apple os
#define SYSTEM_EMBEDDED_APPLE_OS
#endif
#elif defined(__digital__) // tru64 unix
#define SYSTEM_TRU64
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define SYSTEM_CYGWIN
#else
#define SYSTEM_UNKNOWN
#endif

#if defined(USING_C11) && !defined(HAVE_C11_GENERIC_SELECTION)
// workaround for early C11 compilers that may still be used with this code to
// disable generic selection when not supported
#if defined(__clang__) && __clang__ > 3 || (defined(__clang_minor__) && __clang__ == 3 && __clang_minor__ >= 3)
#define HAVE_C11_GENERIC_SELECTION
#elif defined(__GNUC__) && (__GNUC__ >= 5 || (defined(__GNUC_MINOR__) && __GNUC__ == 4 && __GNUC_MINOR__ >= 9))
#define HAVE_C11_GENERIC_SELECTION
#elif defined(_MSC_VER) && _MSC_VER >= 1928
/* SDK version 10.0.20348.0 is technically when it arrived, but you also need
 * VS2019 16.8 and later */
/*TODO: Move the Windows SDK version detection to this file and add that check
 * to the ifdef above */
#define HAVE_C11_GENERIC_SELECTION
#endif
#endif //

#if defined(__cplusplus)
}
#endif
