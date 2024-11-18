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
// \file env_detect.c
// \brief Detects the compilation environment for standards, extensions, etc.
// Also detects CPU type and endianness
//

#include "env_detect.h"
#include "io_utils.h"
#include "memory_safety.h"
#include "string_utils.h"
#include "type_conversion.h"

#include <string.h>

void print_Architecture(eArchitecture arch)
{
    switch (arch)
    {
    case OPENSEA_ARCH_X86:
        printf("X86");
        break;
    case OPENSEA_ARCH_X86_64:
        printf("X86_64");
        break;
    case OPENSEA_ARCH_ARM:
        printf("ARM");
        break;
    case OPENSEA_ARCH_ARM64:
        printf("ARM64");
        break;
    case OPENSEA_ARCH_POWERPC:
        printf("PPC");
        break;
    case OPENSEA_ARCH_POWERPC64:
        printf("PPC64");
        break;
    case OPENSEA_ARCH_IA_64:
        printf("IA64");
        break;
    case OPENSEA_ARCH_SPARC:
        printf("SPARC");
        break;
    case OPENSEA_ARCH_ALPHA:
        printf("Alpha");
        break;
    case OPENSEA_ARCH_SYSTEMZ:
        printf("SystemZ");
        break;
    case OPENSEA_ARCH_MIPS:
        printf("MIPS");
        break;
    case OPENSEA_ARCH_UNKNOWN:
    case OPENSEA_ARCH_RESERVED:
        printf("Unknown Architecture");
        break;
    }
}

void print_Endianness(eEndianness endian, bool shortPrint)
{
    switch (endian)
    {
    case OPENSEA_LITTLE_ENDIAN:
        if (shortPrint)
        {
            printf("LSB");
        }
        else
        {
            printf("Little Endian");
        }
        break;
    case OPENSEA_BIG_ENDIAN:
        if (shortPrint)
        {
            printf("MSB");
        }
        else
        {
            printf("Big Endian");
        }
        break;
    case OPENSEA_LITTLE_WORD_ENDIAN:
        if (shortPrint)
        {
            printf("LSW");
        }
        else
        {
            printf("Little Endian (Word)");
        }
        break;
    case OPENSEA_BIG_WORD_ENDIAN:
        if (shortPrint)
        {
            printf("MSW");
        }
        else
        {
            printf("Big Endian (Word)");
        }
        break;
    case OPENSEA_UNKNOWN_ENDIAN:
        if (shortPrint)
        {
            printf("???");
        }
        else
        {
            printf("Unknown Endianness");
        }
        break;
    }
}

void print_OS_Type(eOSType osType)
{
    switch (osType)
    {
    case OS_WINDOWS:
        printf("Windows");
        break;
    case OS_LINUX:
        printf("Linux");
        break;
    case OS_FREEBSD:
        printf("FreeBSD");
        break;
    case OS_SOLARIS:
        printf("Solaris");
        break;
    case OS_MACOSX:
        printf("Mac OSX");
        break;
    case OS_AIX:
        printf("AIX");
        break;
    case OS_TRU64:
        printf("TRU64");
        break;
    case OS_OPENBSD:
        printf("OpenBSD");
        break;
    case OS_NETBSD:
        printf("NetBSD");
        break;
    case OS_DRAGONFLYBSD:
        printf("Dragonfly BSD");
        break;
    case OS_HPUX:
        printf("HP UX");
        break;
    case OS_ESX:
        printf("VMWare ESXi");
        break;
    case OS_UEFI:
        printf("UEFI");
        break;
    case OS_UNKNOWN:
    default:
        printf("Unknown OS");
        break;
    }
}

void print_OS_Version(ptrOSVersionNumber versionNumber)
{
    // format is something similar to major.minor.rev
    switch (versionNumber->osVersioningIdentifier)
    {
    case OS_WINDOWS:
        printf("%" PRIu16 ".%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.windowsVersion.majorVersion,
               versionNumber->versionType.windowsVersion.minorVersion,
               versionNumber->versionType.windowsVersion.buildNumber);
        break;
    case OS_LINUX:
        printf("%" PRIu16 ".%" PRIu16 ".%" PRIu16 "-%" PRIu16 "", versionNumber->versionType.linuxVersion.kernelVersion,
               versionNumber->versionType.linuxVersion.majorVersion,
               versionNumber->versionType.linuxVersion.minorVersion,
               versionNumber->versionType.linuxVersion.securityAndBugFixesNumber);
        break;
    case OS_FREEBSD:
        printf("%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.freeBSDVersion.majorVersion,
               versionNumber->versionType.freeBSDVersion.minorVersion);
        break;
    case OS_SOLARIS:
        printf("%" PRIu16 ".%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.solarisVersion.sunOSMajorVersion,
               versionNumber->versionType.solarisVersion.sunOSMinorVersion,
               versionNumber->versionType.solarisVersion.sunOSRevision);
        if (versionNumber->versionType.solarisVersion.solarisMajorVersion > 0)
        {
            printf("-%" PRIu16 ".%" PRIu16 ".%" PRIu16 "",
                   versionNumber->versionType.solarisVersion.solarisMajorVersion,
                   versionNumber->versionType.solarisVersion.solarisMinorVersion,
                   versionNumber->versionType.solarisVersion.solarisRevision);
        }
        break;
    case OS_MACOSX:
        printf("%" PRIu16 ".%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.macOSVersion.majorVersion,
               versionNumber->versionType.macOSVersion.minorVersion, versionNumber->versionType.macOSVersion.revision);
        break;
    case OS_AIX:
        printf("%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.aixVersion.majorVersion,
               versionNumber->versionType.aixVersion.minorVersion);
        break;
    case OS_OPENBSD:
        printf("%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.openBSDVersion.majorVersion,
               versionNumber->versionType.openBSDVersion.minorVersion);
        break;
    case OS_NETBSD:
        printf("%" PRIu16 ".%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.netBSDVersion.majorVersion,
               versionNumber->versionType.netBSDVersion.minorVersion,
               versionNumber->versionType.netBSDVersion.revision);
        break;
    case OS_DRAGONFLYBSD:
        printf("%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.dragonflyVersion.majorVersion,
               versionNumber->versionType.dragonflyVersion.minorVersion);
        break;
    case OS_HPUX:
        printf("%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.hpuxVersion.majorVersion,
               versionNumber->versionType.hpuxVersion.minorVersion);
        break;
    case OS_TRU64:
        printf("%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.tru64Version.majorVersion,
               versionNumber->versionType.tru64Version.minorVersion);
        break;
    case OS_ESX:
        printf("%" PRIu16 ".%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.esxiVersion.majorVersion,
               versionNumber->versionType.esxiVersion.minorVersion, versionNumber->versionType.esxiVersion.revision);
        break;
    case OS_UEFI:
        printf("%" PRIu16 ".%" PRIu16 "", versionNumber->versionType.uefiVersion.majorVersion,
               versionNumber->versionType.uefiVersion.minorVersion);
        break;
    case OS_UNKNOWN:
    default:
        printf("Unknown OS Version");
        break;
    }
}

eArchitecture get_Compiled_Architecture(void)
{
// NOTE: Removed compiler checks since some reuse the same definitions for the
// same CPU type in order to simplify this code more - TJE
#if defined __alpha__ || defined _M_ALPHA
    return OPENSEA_ARCH_ALPHA;
#elif defined __amd64__ || defined __x86_64__ || defined _M_X64 || defined _M_AMD64 || defined(MDE_CPU_X64)
    // NOTE: MSVC will defined _M_X64 for _M_ARM64EC mode as well (arm
    // compatibility mode)...not currently checking this since this is
    //       not needed. We already natively support ARM and AARCH64, so there
    //       is no need to compile this code in this compatibility mode.-TJE
    return OPENSEA_ARCH_X86_64;
#elif defined __arm__ || defined __thumb__ || defined _M_ARM || defined _M_ARMT || defined(MDE_CPU_ARM)
    return OPENSEA_ARCH_ARM;
#elif defined __aarch64__ || defined(MDE_CPU_AARCH64) || defined(_M_ARM64)
    return OPENSEA_ARCH_ARM64;
#elif defined __i386__ || defined __i486__ || defined __i586__ || defined __i686__ ||                                  \
    (defined __MINGW32__ && defined _X86_) || defined _M_IX86 || defined __i386 || defined __THW_INTEL__ ||            \
    defined(MDE_CPU_IA32)
    return OPENSEA_ARCH_X86;
#elif defined __ia64__ || defined __IA64__ || defined _M_IA64 || defined(MDE_CPU_IPF)
    return OPENSEA_ARCH_IA_64;
#elif defined __powerpc64__ || defined __PPC64__ || defined __ppc64__ || defined _ARCH_PPC64 || defined _ARCH_PPC64
    return OPENSEA_ARCH_POWERPC64;
#elif defined __powerpc__ || defined __POWERPC__ || defined __PPC__ || defined __ppc__ || defined _ARCH_PPC ||         \
    defined _M_PPC || defined _ARCH_PPC
    return OPENSEA_ARCH_POWERPC;
#elif defined __sparc__ || defined __sparc
    return OPENSEA_ARCH_SPARC;
#elif defined __s390__ || defined __s390x__ || defined __zarch__ || defined __370__ || defined __THW_370__
    return OPENSEA_ARCH_SYSTEMZ;
#elif defined                                                                                  __mips__
    return OPENSEA_ARCH_MIPS;
#else
    return OPENSEA_ARCH_UNKNOWN;
#endif
}

// temporarily disable the unused function warning.
// The reason for this is because in some situations this function is used and
// in others it won't be. rather than making a complicated ifdef around it,
// disabling the warning for now-TJE
#if defined __clang__
// clang specific because behavior can differ even with the GCC diagnostic being
// "compatible" https
// ://clang.llvm.org/docs/UsersManual.html#controlling-diagnostics-via-pragmas
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-function"
#elif defined __GNUC__ && __GNUC__ >= 3
// temporarily disable the warning for unused function
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-function"
#endif //__clang__, __GNUC__

// https://sourceforge.net/p/predef/wiki/Endianness/
static eEndianness calculate_Endianness(void)
{
    static eEndianness endian = OPENSEA_UNKNOWN_ENDIAN; // using static so that it should only need to
                                                        // run this code once...not that it takes a long
                                                        // time, but this may help optimise this.
    if (endian == OPENSEA_UNKNOWN_ENDIAN)
    {
        union
        {
            uint32_t value;
            uint8_t  data[sizeof(uint32_t)];
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

// reenable the unused function warning
#if defined __clang__
#    pragma clang diagnostic pop
#elif defined __GNUC__ && __GNUC__ >= 3
#    pragma GCC diagnostic pop
#endif //__clang__, __GNUC__

// MDE_CPU_EBC should always calculate the endianness for EFI byte code since we
// do not know the exact endianness through other means
eEndianness get_Compiled_Endianness(void)
{
#if defined(__BYTE_ORDER__)
#    if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return OPENSEA_BIG_ENDIAN;
#    elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return OPENSEA_LITTLE_ENDIAN;
#    else
    return calculate_Endianness();
#    endif
#else
#    if defined(__BIG_ENDIAN__)
    return OPENSEA_BIG_ENDIAN;
#    elif defined(__LITTLE_ENDIAN__)
    return OPENSEA_LITTLE_ENDIAN;
#    else
// check architecture specific defines...
#        if defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIPSEB) ||                \
            defined(__MIPSEB) || defined(__MIPSEB__)
    return OPENSEA_BIG_ENDIAN;
#        elif defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) ||              \
            defined(__MIPSEL) || defined(__MIPSEL__) || defined _M_X64 || defined _M_AMD64 || defined _M_ALPHA ||      \
            defined _M_ARM || defined _M_ARMT || defined _M_IX86 || defined _M_IA64 ||                                 \
            defined _M_PPC /* This is a special Windows case for PPC                                                   \
                              as NT ran it in little endian mode */                                                    \
            || defined MDE_CPU_X64 || defined MDE_CPU_IA32 || defined MDE_CPU_ARM || defined MDE_CPU_AARCH64
    return OPENSEA_LITTLE_ENDIAN;
#        else
    return calculate_Endianness();
#        endif
#    endif
#endif
}

eReturnValues get_Compiler_Info(eCompiler* compilerUsed, ptrCompilerVersion compilerVersionInfo)
{
    eReturnValues ret = SUCCESS; // should always return this unless we don't
                                 // find a compiler we support.
    // make sure we set unknown and clear out version info before we continue.
    *compilerUsed = OPENSEA_COMPILER_UNKNOWN;
    safe_memset(compilerVersionInfo, sizeof(compilerVersion), 0, sizeof(compilerVersion));
    // Time to start checking predefined compiler macros
#if defined _MSC_VER
    // Microsoft Visual C/C++ compiler (code is written to use the _MSC_FULL_VER
    // from 2003 and later and we only really support 2013 and higher due to C99
    // usage)-TJE
    *compilerUsed = OPENSEA_COMPILER_MICROSOFT_VISUAL_C_CPP;
#    define MS_VERSION_STRING_LENGTH 10
    DECLARE_ZERO_INIT_ARRAY(char, msVersion, MS_VERSION_STRING_LENGTH);
    snprintf(msVersion, MS_VERSION_STRING_LENGTH, "%u", _MSC_FULL_VER);
    DECLARE_ZERO_INIT_ARRAY(char, msMajor, 3);
    DECLARE_ZERO_INIT_ARRAY(char, msMinor, 3);
    DECLARE_ZERO_INIT_ARRAY(char, msPatch, 6);
    snprintf(msMajor, 3, "%.2s", &msVersion[0]);
    snprintf(msMinor, 3, "%.2s", &msVersion[2]);
    snprintf(msPatch, 6, "%.5s", &msVersion[4]);
    compilerVersionInfo->major = C_CAST(uint16_t, strtoul(msMajor, M_NULLPTR, BASE_10_DECIMAL));
    compilerVersionInfo->minor = C_CAST(uint16_t, strtoul(msMinor, M_NULLPTR, BASE_10_DECIMAL));
    compilerVersionInfo->patch = C_CAST(uint16_t, strtoul(msPatch, M_NULLPTR, BASE_10_DECIMAL));
#elif defined __MINGW64__
    *compilerUsed = OPENSEA_COMPILER_MINGW;
    compilerVersionInfo->major = __MINGW64_VERSION_MAJOR;
    compilerVersionInfo->minor = __MINGW64_VERSION_MINOR;
    compilerVersionInfo->patch = 0;
#elif defined __MINGW32__
    *compilerUsed              = OPENSEA_COMPILER_MINGW;
    compilerVersionInfo->major = __MINGW32_MAJOR_VERSION;
    compilerVersionInfo->minor = __MINGW32_MINOR_VERSION;
    compilerVersionInfo->patch = 0;
#elif defined __clang__
    *compilerUsed              = OPENSEA_COMPILER_CLANG;
    compilerVersionInfo->major = __clang_major__;
    compilerVersionInfo->minor = __clang_minor__;
    compilerVersionInfo->patch = __clang_patchlevel__;
#elif defined __HP_aCC
    // untested
    *compilerUsed = OPENSEA_COMPILER_HP_A_CPP;
#    define HP_ACC_VERSION_STRING_LENGTH 7
    DECLARE_ZERO_INIT_ARRAY(char, hpVersion, HP_ACC_VERSION_STRING_LENGTH);
    snprintf(hpVersion, HP_ACC_VERSION_STRING_LENGTH, "%u", __HP_aCC);
    DECLARE_ZERO_INIT_ARRAY(char, hpMajor, 3);
    DECLARE_ZERO_INIT_ARRAY(char, hpMinor, 3);
    DECLARE_ZERO_INIT_ARRAY(char, hpPatch, 3);
    snprintf(hpMajor, 3, "%.2s", &hpVersion[0]);
    snprintf(hpMinor, 3, "%.2s", &hpVersion[2]);
    snprintf(hpPatch, 3, "%.2s", &hpVersion[4]);
    compilerVersionInfo->major = C_CAST(uint16_t, strtoul(hpMajor, M_NULLPTR, BASE_10_DECIMAL));
    compilerVersionInfo->minor = C_CAST(uint16_t, strtoul(hpMinor, M_NULLPTR, BASE_10_DECIMAL));
    compilerVersionInfo->patch = C_CAST(uint16_t, strtoul(hpPatch, M_NULLPTR, BASE_10_DECIMAL));
#elif defined __IBMC__ || defined __IBMCPP__
    // untested
    // detect if it's xl or lx for system z
#    if defined __COMPILER_VER__
    // system z
    compilerVersionInfo->major = M_Nibble6(__COMPILER_VER__);
    compilerVersionInfo->minor = M_Byte2(__COMPILER_VER__);
    compilerVersionInfo->patch = M_Word0(__COMPILER_VER__);
#    else
    // standard xl
    compilerVersionInfo->major = M_Byte1(__xlC__);
    compilerVersionInfo->minor = M_Byte0(__xlC__);
    compilerVersionInfo->patch = M_Byte1(__xlC_ver__);
#    endif
#elif defined __INTEL_COMPILER
    // untested
    *compilerUsed = OPENSEA_COMPILER_INTEL_C_CPP;
#    define INTEL_VERSION_STRING_MAX_LENGTH 4;
    DECLARE_ZERO_INIT_ARRAY(char, intelVersion, 4);
    snprintf(intelVersion, INTEL_VERSION_STRING_MAX_LENGTH, "%u", __INTEL_COMPILER);
    DECLARE_ZERO_INIT_ARRAY(char, intelMajor, 2);
    DECLARE_ZERO_INIT_ARRAY(char, intelMinor, 2);
    DECLARE_ZERO_INIT_ARRAY(char, intelPatch, 2);
    snprintf(intelMajor, 2, "%.1s", &intelVersion[0]);
    snprintf(intelMinor, 2, "%.1s", &intelVersion[1]);
    snprintf(intelPatch, 2, "%.1s", &intelVersion[2]);
    compilerVersionInfo->major = C_CAST(uint16_t, strtoul(intelMajor, M_NULLPTR, BASE_0_AUTO));
    compilerVersionInfo->minor = C_CAST(uint16_t, strtoul(intelMinor, M_NULLPTR, BASE_0_AUTO));
    compilerVersionInfo->patch = C_CAST(uint16_t, strtoul(intelPatch, M_NULLPTR, BASE_0_AUTO));
#elif defined __SUNPRO_C || defined __SUNPRO_CC
    // untested
    // code below is written for versions 5.10 and later. (latest release as of
    // writing this code is version 5.12) - TJE
    *compilerUsed              = OPENSEA_COMPILER_SUNPRO_C_CPP;
    compilerVersionInfo->major = M_Nibble3(__SUNPRO_C);
    compilerVersionInfo->minor =
        (M_Nibble2(__SUNPRO_C) << 4) | M_Nibble1(__SUNPRO_C); // I think this is wrong. Version 5.10 is represented
                                                              // as 0x5100. Need to put extra code in here to fix
                                                              // this when we actually support this compiler.
    compilerVersionInfo->patch = M_Nibble0(__SUNPRO_C);
#elif defined                       __GNUC__ // CYGWIN?
    // GCC C/C++ compiler
    *compilerUsed              = OPENSEA_COMPILER_GCC;
    compilerVersionInfo->major = __GNUC__;
    compilerVersionInfo->minor = __GNUC_MINOR__;
    compilerVersionInfo->patch = __GNUC_PATCHLEVEL__; // introduced in GCC 3.0, but even that is super
                                                      // old so this shouldn't cause any problems.-TJE
#else
    ret = NOT_SUPPORTED;
#endif
    return ret;
}

void print_Compiler(eCompiler compilerUsed)
{
    switch (compilerUsed)
    {
    case OPENSEA_COMPILER_MICROSOFT_VISUAL_C_CPP:
        printf("Microsoft Visual C/C++");
        break;
    case OPENSEA_COMPILER_GCC:
        printf("GCC");
        break;
    case OPENSEA_COMPILER_CLANG:
        printf("Clang");
        break;
    case OPENSEA_COMPILER_MINGW:
        printf("MinGW");
        break;
    case OPENSEA_COMPILER_INTEL_C_CPP:
        printf("Intel C/C++");
        break;
    case OPENSEA_COMPILER_SUNPRO_C_CPP:
        printf("Oracle Sunpro C/C++");
        break;
    case OPENSEA_COMPILER_IBM_XL_C_CPP:
        printf("IBM XL C/C++");
        break;
    case OPENSEA_COMPILER_IBM_SYSTEMZ_C_CPP:
        printf("IBM XL C/C++ for SystemZ");
        break;
    case OPENSEA_COMPILER_HP_A_CPP:
        printf("HP aCC");
        break;
    case OPENSEA_COMPILER_UNKNOWN:
        // default:
        printf("Unknown Compiler");
        break;
    }
}

void print_Compiler_Version_Info(ptrCompilerVersion compilerVersionInfo)
{
    printf("%" PRIu16 ".%" PRIu16 ".%" PRIu16 "", compilerVersionInfo->major, compilerVersionInfo->minor,
           compilerVersionInfo->patch);
}
