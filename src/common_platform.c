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
// \file common_platform.c
// \brief Implements functions that are common to platforms.
//
#include "common_platform.h"

//This file should only really exist for some things like printing things out that would be common and not specific to an OS-TJE

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
    default:
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
    //default:
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
    //format is something similar to major.minor.rev
    switch (versionNumber->osVersioningIdentifier)
    {
    case OS_WINDOWS:
        printf("%"PRIu16".%"PRIu16".%"PRIu16"", versionNumber->versionType.windowsVersion.majorVersion, versionNumber->versionType.windowsVersion.minorVersion, versionNumber->versionType.windowsVersion.buildNumber);
        break;
    case OS_LINUX:
        printf("%"PRIu16".%"PRIu16".%"PRIu16"-%"PRIu16"", versionNumber->versionType.linuxVersion.kernelVersion, versionNumber->versionType.linuxVersion.majorVersion, versionNumber->versionType.linuxVersion.minorVersion, versionNumber->versionType.linuxVersion.securityAndBugFixesNumber);
        break;
    case OS_FREEBSD:
        printf("%"PRIu16".%"PRIu16"", versionNumber->versionType.freeBSDVersion.majorVersion, versionNumber->versionType.freeBSDVersion.minorVersion);
        break;
    case OS_SOLARIS:
        printf("%"PRIu16".%"PRIu16".%"PRIu16"", versionNumber->versionType.solarisVersion.sunOSMajorVersion, versionNumber->versionType.solarisVersion.sunOSMinorVersion, versionNumber->versionType.solarisVersion.sunOSRevision);
        if (versionNumber->versionType.solarisVersion.solarisMajorVersion > 0)
        {
            printf("-%"PRIu16".%"PRIu16".%"PRIu16"", versionNumber->versionType.solarisVersion.solarisMajorVersion, versionNumber->versionType.solarisVersion.solarisMinorVersion, versionNumber->versionType.solarisVersion.solarisRevision);
        }
        break;
    case OS_MACOSX:
        printf("%"PRIu16".%"PRIu16".%"PRIu16"", versionNumber->versionType.macOSVersion.majorVersion, versionNumber->versionType.macOSVersion.minorVersion, versionNumber->versionType.macOSVersion.revision);
        break;
    case OS_AIX:
        printf("%"PRIu16".%"PRIu16"", versionNumber->versionType.aixVersion.majorVersion, versionNumber->versionType.aixVersion.minorVersion);
        break;
    case OS_OPENBSD:
        printf("%"PRIu16".%"PRIu16"", versionNumber->versionType.openBSDVersion.majorVersion, versionNumber->versionType.openBSDVersion.minorVersion);
        break;
    case OS_NETBSD:
        printf("%"PRIu16".%"PRIu16".%"PRIu16"", versionNumber->versionType.netBSDVersion.majorVersion, versionNumber->versionType.netBSDVersion.minorVersion, versionNumber->versionType.netBSDVersion.revision);
        break;
    case OS_DRAGONFLYBSD:
        printf("%"PRIu16".%"PRIu16"", versionNumber->versionType.dragonflyVersion.majorVersion, versionNumber->versionType.dragonflyVersion.minorVersion);
        break;
    case OS_HPUX:
        printf("%"PRIu16".%"PRIu16"", versionNumber->versionType.hpuxVersion.majorVersion, versionNumber->versionType.hpuxVersion.minorVersion);
        break;
    case OS_TRU64:
        printf("%"PRIu16".%"PRIu16"", versionNumber->versionType.tru64Version.majorVersion, versionNumber->versionType.tru64Version.minorVersion);
        break;
    case OS_ESX:
        printf("%"PRIu16".%"PRIu16".%"PRIu16"", versionNumber->versionType.esxiVersion.majorVersion, versionNumber->versionType.esxiVersion.minorVersion, versionNumber->versionType.esxiVersion.revision);
        break;
    case OS_UEFI:
        printf("%"PRIu16".%"PRIu16"", versionNumber->versionType.uefiVersion.majorVersion, versionNumber->versionType.uefiVersion.minorVersion);
        break;
    case OS_UNKNOWN:
    default:
        printf("Unknown OS Version");
        break;
    }
}

int replace_File_Name_In_Path(char fullPath[OPENSEA_PATH_MAX], char *newFileName)
{
    char *ptr = NULL;
    size_t ptrLen = 0, fullLength = 0;
    if (NULL != (ptr = strrchr(fullPath, SYSTEM_PATH_SEPARATOR)))
    {
        ptr += 1;
    }
    else
    {
        return FAILURE;
    }
    ptrLen = strlen(ptr);
    //now that we have a valid pointer, set all the remaining characters to null, then set the new file name in place.
    memset(ptr, 0, ptrLen);
    fullLength = (OPENSEA_PATH_MAX - strlen(fullPath));
    snprintf(ptr, fullLength, "%s", newFileName);
    return SUCCESS;
}
