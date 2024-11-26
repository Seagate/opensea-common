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
#    define __STDC_WANT_LIB_EXT1__ 1 // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
#endif
#if !defined(__STDC_WANT_LIB_EXT2__)
// asking to get dynamic memory functions (mostly strdup) in C11. Only
// guaranteed available if __STDC_ALLOC_LIB__ defined, but asking anyways...most
// systems we support will have this-TJE
#    define __STDC_WANT_LIB_EXT2__ 1 // NOLINT(bugprone-reserved-identifier,cert-dcl37-c,cert-dcl51-cpp)
#endif

// Macro to check GCC version(major.minor)
#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#    define IS_GCC_VERSION(major, minor) ((__GNUC__ > (major) || (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor))))
#else
#    define IS_GCC_VERSION(major, minor) (0)
#endif

// Macro to check GCC full version (major.minor.patch)
#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#    define IS_GCC_FULL_VERSION(major, minor, patch)                                                                   \
        ((__GNUC__ > (major) ||                                                                                        \
          (__GNUC__ == (major) &&                                                                                      \
           (__GNUC_MINOR__ > (minor) || (__GNUC_MINOR__ == (minor) && __GNUC_PATCHLEVEL__ >= (patch))))))
#else
#    define IS_GCC_FULL_VERSION(major, minor, patch) (0)
#endif

// Macro to check Clang version(major.minor)
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

// Macro to check MinGW32 version(major.minor)
#if defined(__MINGW32__) && defined(__MINGW32_MAJOR_VERSION) && defined(__MINGW32_MINOR_VERSION)
#    define IS_MINGW32_VERSION(major, minor)                                                                           \
        ((__MINGW32_MAJOR_VERSION > (major) ||                                                                         \
          (__MINGW32_MAJOR_VERSION == (major) && __MINGW32_MINOR_VERSION >= (minor))))
#else
#    define IS_MINGW32_VERSION(major, minor) (0)
#endif

// Macro to check MinGW-w64 version(major.minor)
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

// Macro to check MSVC version
#if defined(_MSC_VER)
#    define IS_MSVC_VERSION(msvcver) (_MSC_VER > (msvcver))
#else
#    define IS_MSVC_VERSION(msvcver) (0)
#endif

// Macro to check MSVC full version using _MSC_FULL_VER
#if defined(_MSC_FULL_VER)
#    define IS_MSVC_FULL_VERSION(msvcfullver) (_MSC_FULL_VER > (msvcfullver))
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
// Macro to check glibc version (major.minor)
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

// Macro to check uClibc version (major.minor)
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

// Macro to check klibc version (major.minor)
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

#if IS_MSVC_VERSION(MSVC_2010) && !defined(__clang__)
#    define DISABLE_WARNING_4255 _Pragma("warning(push)") _Pragma("warning(disable: 4255)")

#    define RESTORE_WARNING_4255 _Pragma("warning(pop)")
#else
#    define DISABLE_WARNING_4255
#    define RESTORE_WARNING_4255
#endif //_MSVC && !clang workaround for Windows API headers

#if defined(_WIN32)
DISABLE_WARNING_4255
#    include <windows.h>
RESTORE_WARNING_4255
// This pragma is needed to tell a library including opensea-common to look for
// Version.lib for the version helping information
//  This only works in MSVC which is why mingw gets filtered out below.
//  Meson/makefile projects will need to link these manually.
// NOTE: ARM requires 10.0.16299.0 API to get this library!
#    if !defined(__MINGW32__) && !defined(__MINGW64__)
#        pragma comment(lib, "Version.lib")  // for getting Windows system versions
#        pragma comment(lib, "Advapi32.lib") // WinPE check and windows 10 version check
#    endif
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
#        define POSIX_1988
#    endif
#    if _POSIX_VERSION >= 199009L
#        define POSIX_1990
#    endif
#    if defined(_POSIX2_C_VERSION) && _POSIX2_C_VERSION >= 199209L
#        define POSIX_1992
#    endif
#    if _POSIX_VERSION >= 199309L
#        define POSIX_1993
#    endif
#    if _POSIX_VERSION >= 199506L
#        define POSIX_1996
#    endif
#    if _POSIX_VERSION >= 200112L
#        define POSIX_2001
#    endif
#    if _POSIX_VERSION >= 200809L
#        define POSIX_2008
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
#        define USING_XPG3 /* 1989 */
#    endif
#    if _XOPEN_VERSION >= 4
#        define USING_XPG4 /* 1992 */
#        if defined(_XOPEN_UNIX)
#            define USING_SUS /* UNIX95 */
#        endif
#    endif
#    if _XOPEN_VERSION >= 500
#        define USING_SUS2 /* UNIX98 */
#    endif
#    if _XOPEN_VERSION >= 600
#        define USING_SUS3 /* UNIX03 */
#    endif
#    if _XOPEN_VERSION >= 700
#        define USING_SUS4
#    endif
#endif //_XOPEN_VERSION
// NOTE: Some functions that were added to posix, xopen, sus may have come from
// BSD 4.x. Use sys/param.h to check for these old bsd versions

// Detect the LP64 pr LLP64 mode for the system/compiler.
// This is meant to be used within various opensea-common functions, but defined
// in this header so that it can be used easily for other uses
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
#elif defined(__LP32__) || defined(_LP32)
#    define LP32_DATA_MODEL
#else
// Add more data model info here as necessary.
// If a data model is not already defined above, then some built in logic is
// used to decide functions to call in some scenarios.
#endif

#if (defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__))
#    define HAVE_C11_ANNEX_K
#endif // check for annex k

#if defined(__STDC_SECURE_LIB__) || defined(__GOT_SECURE_LIB__)
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
#    define ENV_64BIT
#else // CPU Check
#    define ENV_32BIT
#endif // CPU Check

    // NOTE: These warning disables are only needed in MSVC for the C11 generic min/max implementations.
    // Without them you get a warning about applying a unary - on an unsigned type.
    // More casts can be used to work around this, but this was easier since it is otherwise correct code -TJE
    // If this becomes useful in other places, this should be moved to a different file, but since it's
    // only used here, it can stay here for now. - TJE
    // Started in VS2012???
#if IS_MSVC_VERSION(MSVC_2012) && !defined(__clang__)
#    define DISABLE_WARNING_4146 _Pragma("warning(push)") _Pragma("warning(disable: 4146)")
#    define RESTORE_WARNING_4146 _Pragma("warning(pop)")
#else
#    define DISABLE_WARNING_4146
#    define RESTORE_WARNING_4146
#endif //_MSVC && !clang workaround for min/max macros

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

#if IS_CLANG_VERSION(3, 0)
#    define DISABLE_WARNING_ZERO_LENGTH_ARRAY                                                                          \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wzero-length-array\"")
#    define RESTORE_WARNING_ZERO_LENGTH_ARRAY _Pragma("clang diagnostic pop")
#elif IS_MSVC_VERSION(MSVC_2005)
#    define DISABLE_WARNING_ZERO_LENGTH_ARRAY _Pragma("warning(push)") _Pragma("warning(disable: 4200)")
#    define RESTORE_WARNING_ZERO_LENGTH_ARRAY _Pragma("warning(pop)")
#else
#    define DISABLE_WARNING_ZERO_LENGTH_ARRAY
#    define RESTORE_WARNING_ZERO_LENGTH_ARRAY
#endif

#if IS_CLANG_VERSION(3, 0)
#    define DISABLE_WARNING_FORMAT_NONLITERAL                                                                          \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wformat-nonliteral\"")
#    define RESTORE_WARNING_FORMAT_NONLITERAL _Pragma("clang diagnostic pop")
#elif IS_GCC_VERSION(4, 1)
#    define DISABLE_WARNING_FORMAT_NONLITERAL                                                                          \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wformat-nonliteral\"")
#    define RESTORE_WARNING_FORMAT_NONLITERAL _Pragma("GCC diagnostic pop")
#else
#    define DISABLE_WARNING_FORMAT_NONLITERAL
#    define RESTORE_WARNING_FORMAT_NONLITERAL
#endif

#if defined(__cplusplus)
}
#endif
