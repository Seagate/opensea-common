//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2018-2021 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file common_uefi.c
// \brief Definitions and function calls (and includes) specific to UEFI


#include "common.h"
#include "common_uefi.h"
#include <time.h> //may need sys/time.h to to the timers. gettimeofday & struct timeval
#include <utime.h>
#include <stdlib.h>
#include <sys/stat.h>
//UEFI EDK includes
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>//to get global boot services pointer. This pointer should be checked before use, but any app using stdlib will have this set.
#include <Library/PrintLib.h> //to convert CHAR16 string to CHAR8. may also be able to use stdlib in someway, but this seems to work
#include <Protocol/SimpleTextOut.h> //for colors
#include <Uefi/UefiBaseType.h> //for EFI_STATUS definitions

bool os_Directory_Exists(const char * const pathToCheck)
{
    struct stat st;
    if (stat(pathToCheck, &st) == SUCCESS)
    {
        return (st.st_mode & S_IFDIR) != 0;
    }
    return false;
}

bool os_File_Exists(const char * const filetoCheck)
{
    struct stat st;
    return (stat(filetoCheck, &st) == SUCCESS);
}

int get_Full_Path(const char * pathAndFile, char fullPath[OPENSEA_PATH_MAX])
{
    char *resolvedPath = realpath(C_CAST(char*, pathAndFile), C_CAST(char*, fullPath));
    if (resolvedPath)
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

int get_Simple_Text_Output_Protocol_Ptr(void **pOutput)
{
    int ret = SUCCESS;
    EFI_STATUS uefiStatus = EFI_SUCCESS;
    EFI_HANDLE *handle = NULL;
    EFI_GUID outputGuid = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
    UINTN nodeCount = 0;

    if (!gBS) //make sure global boot services pointer is valid before accessing it.
    {
        return MEMORY_FAILURE;
    }

    uefiStatus = gBS->LocateHandleBuffer ( ByProtocol, &outputGuid, NULL, &nodeCount, &handle);
    if(EFI_ERROR(uefiStatus))
    {
        return FAILURE;
    }
    //NOTE: This code below assumes that we only care to change color output on node 0. This seems to work from a quick test, but may not be correct. Not sure what the other 2 nodes are for...serial?
    uefiStatus = gBS->OpenProtocol( handle[0], &outputGuid, pOutput, gImageHandle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    //TODO: based on the error code, rather than assuming failure, check for supported/not supported.
    if(EFI_ERROR(uefiStatus))
    {
        ret = FAILURE;
    }
    return ret;
}

void close_Simple_Text_Output_Protocol_Ptr(void **pOutput)
{
    EFI_STATUS uefiStatus = EFI_SUCCESS;
    EFI_HANDLE *handle = NULL;
    EFI_GUID outputGuid = EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID;
    UINTN nodeCount = 0;

    if (!gBS) //make sure global boot services pointer is valid before accessing it.
    {
        return;
    }

    uefiStatus = gBS->LocateHandleBuffer ( ByProtocol, &outputGuid, NULL, &nodeCount, &handle);
    if(EFI_ERROR(uefiStatus))
    {
        return;
    }
    //NOTE: This code below assumes that we only care to change color output on node 0. This seems to work from a quick test, but may not be correct. Not sure what the other 2 nodes are for...serial?
    uefiStatus = gBS->CloseProtocol( handle[0], &outputGuid, gImageHandle, NULL);
    //TODO: based on the error code, rather than assuming failure, check for supported/not supported.
    if(EFI_ERROR(uefiStatus))
    {
        perror("Failed to close simple text output protocol\n");
    }
    return;
}

static int32_t get_Default_Console_Colors()
{
    static int32_t defaultAttributes = INT32_MAX;
    if (defaultAttributes == INT32_MAX)
    {
        EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *outputProtocol;
        if (SUCCESS == get_Simple_Text_Output_Protocol_Ptr((void**)&outputProtocol))
        {
            defaultAttributes = outputProtocol->Mode->Attribute;
            //printf("Default text output attributes are set to %" PRIX32 "\n", defaultAttributes);
            close_Simple_Text_Output_Protocol_Ptr((void**)&outputProtocol);
        }
    }
    return defaultAttributes;
}

//Comment from simpletextout.h describes acceptable colors....we will try setting whatever we can, but it may not actually work!
// For Foreground, and EFI_* value is valid from EFI_BLACK(0x00) to
// EFI_WHITE (0x0F).
// For Background, only EFI_BLACK, EFI_BLUE, EFI_GREEN, EFI_CYAN,
// EFI_RED, EFI_MAGENTA, EFI_BROWN, and EFI_LIGHTGRAY are acceptable
void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    static bool defaultsSet = false;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *outputProtocol;
    if (!defaultsSet)
    {
        //First time we are setting colors backup the default settings so they can be restored properly later.
        get_Default_Console_Colors();
        defaultsSet = true;
    }
    if (SUCCESS == get_Simple_Text_Output_Protocol_Ptr((void **)&outputProtocol))
    {
        if (foregroundBackground)//change foreground color
        {
            //save current background color
            uint8_t currentBackground = M_Nibble1(C_CAST(unsigned long long, outputProtocol->Mode->Attribute));
            switch (consoleColor)
            {
            case DARK_BLUE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_BLUE, currentBackground));
                break;
            case BLUE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTBLUE, currentBackground));
                break;
            case DARK_GREEN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_GREEN, currentBackground));
                break;
            case GREEN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTGREEN, currentBackground));
                break;
            case DARK_RED:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_RED, currentBackground));
                break;
            case RED:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTRED, currentBackground));
                break;
            case BLACK:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_BLACK, currentBackground));
                break;
            case BROWN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_BROWN, currentBackground));
                break;
            case YELLOW:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_YELLOW, currentBackground));
                break;
            case TEAL:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_CYAN, currentBackground));
                break;
            case CYAN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTCYAN, currentBackground));
                break;
            case PURPLE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_MAGENTA, currentBackground));
                break;
            case MAGENTA:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTMAGENTA, currentBackground));
                break;
            case WHITE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_WHITE, currentBackground));
                break;
            case DARK_GRAY:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_DARKGRAY, currentBackground));
                break;
            case GRAY:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(EFI_LIGHTGRAY, currentBackground));
                break;
            case DEFAULT:
            default:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(M_Nibble0(C_CAST(unsigned long long, get_Default_Console_Colors()), currentBackground)));
                break;
            }
        }
        else//change background color
        {
            uint8_t currentForeground = M_Nibble0(C_CAST(unsigned long long, outputProtocol->Mode->Attribute));
            switch (consoleColor)
            {
            case DARK_BLUE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_BLUE));
                break;
            case BLUE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTBLUE));
                break;
            case DARK_GREEN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_GREEN));
                break;
            case GREEN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTGREEN));
                break;
            case DARK_RED:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_RED));
                break;
            case RED:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTRED));
                break;
            case BLACK:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_BLACK));
                break;
            case BROWN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_BROWN));
                break;
            case YELLOW:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_YELLOW));
                break;
            case TEAL:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_CYAN));
                break;
            case CYAN:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTCYAN));
                break;
            case PURPLE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_MAGENTA));
                break;
            case MAGENTA:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTMAGENTA));
                break;
            case WHITE:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_WHITE));
                break;
            case DARK_GRAY:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_DARKGRAY));
                break;
            case GRAY:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, EFI_LIGHTGRAY));
                break;
            case DEFAULT:
            default:
                outputProtocol->SetAttribute(outputProtocol, EFI_TEXT_ATTR(currentForeground, M_Nibble1(C_CAST(unsigned long long, get_Default_Console_Colors()))));
                break;
            }
        }
        //close the protocol since we are done for now.
        close_Simple_Text_Output_Protocol_Ptr((void**)&outputProtocol);
    }
}
//TODO: Need to validate if these are set correctly when switching between ARM/AARCH64/IA32/X^$ UEFI builds. If not, may need to use MDE_CPU_x macros instead
//MDE_CPU_IPF - itanium
//MDE_CPU_X64 - x86_64
//MDE_CPU_IA32 - x86
//MDE_CPU_EBC
//MDE_CPU_ARM
//MDE_CPU_AARCH64
eArchitecture get_Compiled_Architecture(void)
{
    //All of these are defined somewhere in EDK2
    #if defined (MDE_CPU_IPF)
        return OPENSEA_ARCH_IA_64;
    #elif defined (MDE_CPU_X64)
        return OPENSEA_ARCH_X86_64;
    #elif defined (MDE_CPU_IA32)
        return OPENSEA_ARCH_X86;
    #elif defined (MDE_CPU_EBC)
        return OPENSEA_ARCH_UNKNOWN;
    #elif defined (MDE_CPU_ARM)
        return OPENSEA_ARCH_ARM;
    #elif defined (MDE_CPU_AARCH64)
        return OPENSEA_ARCH_ARM64;
    #else 
      return OPENSEA_ARCH_UNKNOWN;
    #endif

    //check which compiler we're using to use it's preprocessor definitions
//  #if defined __INTEL_COMPILER || defined _MSC_VER //Intel's C/C++ compiler & Visual studio compiler
//      #if defined _M_X64 || defined _M_AMD64
//          return OPENSEA_ARCH_X86_64;
//      #elif defined _M_ALPHA
//          return OPENSEA_ARCH_ALPHA;
//      #elif defined _M_ARM || defined _M_ARMT
//          return OPENSEA_ARCH_ARM;
//      #elif defined _M_IX86
//          return OPENSEA_ARCH_X86;
//      #elif defined _M_IA64
//          return OPENSEA_ARCH_IA_64;
//      #elif defined _M_PPC //32bits I'm guessing - TJE
//          return OPENSEA_ARCH_POWERPC;
//      #else
//          return OPENSEA_ARCH_UNKNOWN;
//      #endif
//  #elif defined __MINGW32__ || defined __MINGW64__ || defined __CYGWIN__ || defined __clang__ || defined __GNUC__ //I'm guessing that all these compilers will use the same macro definitions since the sourceforge pages aren't 100% complete (clang I'm most unsure about)
//      #if defined __alpha__
//          return OPENSEA_ARCH_ALPHA;
//      #elif defined __amd64__ || defined __x86_64__
//          return OPENSEA_ARCH_X86_64;
//      #elif defined __arm__ || defined __thumb__
//          return OPENSEA_ARCH_ARM;
//      #elif defined __aarch64__
//          return OPENSEA_ARCH_ARM64;
//      #elif defined __i386__ || defined __i486__ || defined __i586__ || defined __i686__
//          return OPENSEA_ARCH_X86;
//      #elif defined __ia64__ || defined __IA64__
//          return OPENSEA_ARCH_IA_64;
//      #elif defined __powerpc64__ || defined __PPC64__ || defined __ppc64__ || defined _ARCH_PPC64
//          return OPENSEA_ARCH_POWERPC64;
//      #elif defined __powerpc__ || defined __POWERPC__ || defined __PPC__ || defined __ppc__ || defined _ARCH_PPC
//          return OPENSEA_ARCH_POWERPC;
//      #elif defined __sparc__
//          return OPENSEA_ARCH_SPARC;
//      #elif defined __s390__ || defined __s390x__ || defined __zarch__
//          return OPENSEA_ARCH_SYSTEMZ;
//      #elif defined __mips__
//          return OPENSEA_ARCH_MIPS;
//      #else
//          return OPENSEA_ARCH_UNKNOWN;
//      #endif
//  #elif defined __SUNPRO_C || defined __SUNPRO_CC //SUN/Oracle compilers (unix)
//      #if defined __amd64__ || defined __x86_64__
//          return OPENSEA_ARCH_X86_64;
//      #elif defined __i386
//          return OPENSEA_ARCH_X86;
//      #elif defined __sparc
//          return OPENSEA_ARCH_SPARC;
//      #else
//          return OPENSEA_ARCH_UNKNOWN;
//      #endif
//  #elif defined __IBMC__ || defined __IBMCPP__ //IBM compiler (unix, linux)
//      #if defined __370__ || defined __THW_370__
//          return OPENSEA_ARCH_SYSTEMZ;
//      #elif defined __THW_INTEL__
//          return OPENSEA_ARCH_X86;
//      #elif defined _ARCH_PPC64
//          return OPENSEA_ARCH_POWERPC64;
//      #elif defined _ARCH_PPC
//          return OPENSEA_ARCH_POWERPC;
//      #else
//          return OPENSEA_ARCH_UNKNOWN;
//      #endif
//  #else
//      return OPENSEA_ARCH_UNKNOWN;
//  #endif
}

eEndianness calculate_Endianness(void)
{
    static eEndianness endian = OPENSEA_UNKNOWN_ENDIAN;//using static so that it should only need to run this code once...not that it takes a long time, but this may help optimise this.
    if (endian == OPENSEA_UNKNOWN_ENDIAN)
    {
        union
        {
            uint32_t value;
            uint8_t data[sizeof(uint32_t)];
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

eEndianness get_Compiled_Endianness(void)
{
    #if defined (MDE_CPU_IPF)
        return OPENSEA_LITTLE_ENDIAN;
    #elif defined (MDE_CPU_X64)
        return OPENSEA_LITTLE_ENDIAN;
    #elif defined (MDE_CPU_IA32)
        return OPENSEA_LITTLE_ENDIAN;
    #elif defined (MDE_CPU_EBC)
        //unknown what endianness this is.
    #elif defined (MDE_CPU_ARM)
        return OPENSEA_LITTLE_ENDIAN;
    #elif defined (MDE_CPU_AARCH64)
        return OPENSEA_LITTLE_ENDIAN;
    #endif


//  #if defined _MSC_VER || defined __INTEL_COMPILER && !defined (__GNUC__)
//      #if defined _M_X64 || defined _M_AMD64
//          return OPENSEA_LITTLE_ENDIAN;
//      #elif defined _M_ALPHA
//          return OPENSEA_LITTLE_ENDIAN;
//      #elif defined _M_ARM || defined _M_ARMT
//          return OPENSEA_LITTLE_ENDIAN;
//      #elif defined _M_IX86
//          return OPENSEA_LITTLE_ENDIAN;
//      #elif defined _M_IA64
//          return OPENSEA_LITTLE_ENDIAN;
//      #elif defined _M_PPC //32bits I'm guessing - TJE
//          return OPENSEA_BIG_ENDIAN;
//      #elif defined __LITTLE_ENDIAN__
//          return OPENSEA_LITTLE_ENDIAN;
//      #elif defined __BIG_ENDIAN__
//          return OPENSEA_BIG_ENDIAN;
//      #else
//          return calculate_Endianness();
//      #endif
//  #else
//      #if defined (__BYTE_ORDER__)
//          #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
//              return OPENSEA_BIG_ENDIAN;
//          #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
//              return OPENSEA_LITTLE_ENDIAN;
//          #else
//              return calculate_Endianness();
//          #endif
//      #else
//          #if defined (__BIG_ENDIAN__)
//              return OPENSEA_BIG_ENDIAN;
//          #elif defined (__LITTLE_ENDIAN__)
//              return OPENSEA_LITTLE_ENDIAN;
//          #else
//              //check architecture specific defines...
//              #if defined (__ARMEB__) || defined (__THUMBEB__) || defined (__AARCH64EB__) || defined (_MIPSEB) || defined (__MIPSEB) || defined (__MIPSEB__)
//                  return OPENSEA_BIG_ENDIAN;
//              #elif defined (__ARMEL__) || defined (__THUMBEL__) || defined (__AARCH64EL__) || defined (_MIPSEL) || defined (__MIPSEL) || defined (__MIPSEL__)
//                  return OPENSEA_LITTLE_ENDIAN;
//              #else
//                  return calculate_Endianness();
//              #endif
//          #endif
//      #endif
//  #endif
    return calculate_Endianness();
}

int get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char *operatingSystemName)
{
    //get the UEFI version information and return it.
    //Also, set the operating system name to "UEFI" or "EFI" (If old enough version)
    versionNumber->osVersioningIdentifier = OS_UEFI;
    versionNumber->versionType.uefiVersion.majorVersion = M_Word1(gST->Hdr.Revision);
    versionNumber->versionType.uefiVersion.minorVersion = M_Word0(gST->Hdr.Revision);
    //gST->FirmwareVendor and gST->FirmwareRevision should be part of the operating system name!
    //The FirmwareVendor is a CHAR16 string, so it needs conversion. The revision is a UINT32 and stores some vendor specific version data (display as hex)
    char firmwareVendorASCII[80] = { 0 };
    AsciiSPrintUnicodeFormat(firmwareVendorASCII, 80, L"%s", gST->FirmwareVendor);
    if (strlen(firmwareVendorASCII) == 0)
    {
        snprintf(firmwareVendorASCII, 80, "Unknown Firmware Vendor");
    }
    if (versionNumber->versionType.uefiVersion.majorVersion >= 2)
    {
        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "UEFI - %s - 0x%08" PRIX32, firmwareVendorASCII, gST->FirmwareRevision);
    }
    else
    {
        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "EFI - %s - 0x%08" PRIX32, firmwareVendorASCII, gST->FirmwareRevision);
    }
    return SUCCESS;
}

int64_t os_Get_File_Size(FILE *filePtr)
{
    return -1;
}

void start_Timer(seatimer_t *timer)
{
    struct timeval startTimespec;
    int ret = 0;
    memset(&startTimespec, 0, sizeof(struct timeval));
    ret = gettimeofday(&startTimespec, NULL);
    if (0 == ret)//hopefully this always works...-TJE
    {
        timer->timerStart = C_CAST(uint64_t, startTimespec.tv_sec * UINT64_C(1000000000)) + (C_CAST(uint64_t, startTimespec.tv_usec) * UINT64_C(1000));
    }
}

void stop_Timer(seatimer_t *timer)
{
    struct timeval stopTimespec;
    int ret = 0;
    memset(&stopTimespec, 0, sizeof(struct timeval));
    ret = gettimeofday(&stopTimespec, NULL);
    if (0 == ret)//hopefully this always works...-TJE
    {
        timer->timerStop = C_CAST(uint64_t, stopTimespec.tv_sec * UINT64_C(1000000000)) + (C_CAST(uint64_t, stopTimespec.tv_usec) * UINT64_C(1000));
    }
}

uint64_t get_Nano_Seconds(seatimer_t timer)
{
    return timer.timerStop - timer.timerStart;
}

double get_Micro_Seconds(seatimer_t timer)
{
    uint64_t nanoseconds = get_Nano_Seconds(timer);
    return (C_CAST(double, nanoseconds) / 1000.00);
}

double get_Milli_Seconds(seatimer_t timer)
{
    return (get_Micro_Seconds(timer) / 1000.00);
}

double get_Seconds(seatimer_t timer)
{
    return (get_Milli_Seconds(timer) / 1000.00);
}

//Use %d to print this out or the output look really strange
#define PRI_UINTN "d"
void print_EFI_STATUS_To_Screen(EFI_STATUS efiStatus)
{
    switch (efiStatus)
    {
    case EFI_SUCCESS:
        printf("%" PRI_UINTN " - Success\n", efiStatus);
        break;
    case EFI_LOAD_ERROR:
        printf("%" PRI_UINTN " - Load Error\n", efiStatus);
        break;
    case EFI_INVALID_PARAMETER:
        printf("%" PRI_UINTN " - Invalid Parameter\n", efiStatus);
        break;
    case EFI_UNSUPPORTED:
        printf("%" PRI_UINTN " - Unsupported\n", efiStatus);
        break;
    case EFI_BAD_BUFFER_SIZE:
        printf("%" PRI_UINTN " - Bad Buffer Size\n", efiStatus);
        break;
    case EFI_BUFFER_TOO_SMALL:
        printf("%" PRI_UINTN " - Buffer Too Small\n", efiStatus);
        break;
    case EFI_NOT_READY:
        printf("%" PRI_UINTN " - Not Ready\n", efiStatus);
        break;
    case EFI_DEVICE_ERROR:
        printf("%" PRI_UINTN " - Device Error\n", efiStatus);
        break;
    case EFI_WRITE_PROTECTED:
        printf("%" PRI_UINTN " - Write Protected\n", efiStatus);
        break;
    case EFI_OUT_OF_RESOURCES:
        printf("%" PRI_UINTN " - Out Of Resources\n", efiStatus);
        break;
    case EFI_VOLUME_CORRUPTED:
        printf("%" PRI_UINTN " - Volume Corrupted\n", efiStatus);
        break;
    case EFI_VOLUME_FULL:
        printf("%" PRI_UINTN " - Volume Full\n", efiStatus);
        break;
    case EFI_NO_MEDIA:
        printf("%" PRI_UINTN " - No Media\n", efiStatus);
        break;
    case EFI_MEDIA_CHANGED:
        printf("%" PRI_UINTN " - Media Changed\n", efiStatus);
        break;
    case EFI_NOT_FOUND:
        printf("%" PRI_UINTN " - Not Found\n", efiStatus);
        break;
    case EFI_ACCESS_DENIED:
        printf("%" PRI_UINTN " - Access Denied\n", efiStatus);
        break;
    case EFI_NO_RESPONSE:
        printf("%" PRI_UINTN " - No Response\n", efiStatus);
        break;
    case EFI_NO_MAPPING:
        printf("%" PRI_UINTN " - No Mapping\n", efiStatus);
        break;
    case EFI_TIMEOUT:
        printf("%" PRI_UINTN " - Timeout\n", efiStatus);
        break;
    case EFI_NOT_STARTED:
        printf("%" PRI_UINTN " - Not Started\n", efiStatus);
        break;
    case EFI_ALREADY_STARTED:
        printf("%" PRI_UINTN " - Already Started\n", efiStatus);
        break;
    case EFI_ABORTED:
        printf("%" PRI_UINTN " - Aborted\n", efiStatus);
        break;
    case EFI_ICMP_ERROR:
        printf("%" PRI_UINTN " - ICMP Error\n", efiStatus);
        break;
    case EFI_TFTP_ERROR:
        printf("%" PRI_UINTN " - TFTP Error\n", efiStatus);
        break;
    case EFI_PROTOCOL_ERROR:
        printf("%" PRI_UINTN " - Protocol Error\n", efiStatus);
        break;
    case EFI_INCOMPATIBLE_VERSION:
        printf("%" PRI_UINTN " - Incompatible Version\n", efiStatus);
        break;
    case EFI_SECURITY_VIOLATION:
        printf("%" PRI_UINTN " - Security Violation\n", efiStatus);
        break;
    case EFI_CRC_ERROR:
        printf("%" PRI_UINTN " - CRC Error\n", efiStatus);
        break;
    case EFI_END_OF_MEDIA:
        printf("%" PRI_UINTN " - End Of Media\n", efiStatus);
        break;
    case EFI_END_OF_FILE:
        printf("%" PRI_UINTN " - End Of File\n", efiStatus);
        break;
    case EFI_INVALID_LANGUAGE:
        printf("%" PRI_UINTN " - Invalid Language\n", efiStatus);
        break;
    case EFI_COMPROMISED_DATA:
        printf("%" PRI_UINTN " - Compromised Data\n", efiStatus);
        break;
    case EFI_HTTP_ERROR:
        printf("%" PRI_UINTN " - HTTP Error\n", efiStatus);
        break;
    case EFI_WARN_UNKNOWN_GLYPH:
        printf("%" PRI_UINTN " - Warning Unknown Glyph\n", efiStatus);
        break;
    case EFI_WARN_DELETE_FAILURE:
        printf("%" PRI_UINTN " - Warning Delete Failure\n", efiStatus);
        break;
    case EFI_WARN_WRITE_FAILURE:
        printf("%" PRI_UINTN " - Warning Write Failure\n", efiStatus);
        break;
    case EFI_WARN_BUFFER_TOO_SMALL:
        printf("%" PRI_UINTN " - Warning Buffer Too Small\n", efiStatus);
        break;
    case EFI_WARN_STALE_DATA:
        printf("%" PRI_UINTN " - Warning Stale Data\n", efiStatus);
        break;
    case EFI_WARN_FILE_SYSTEM:
        printf("%" PRI_UINTN " - Warning File System\n", efiStatus);
        break;
    default:
        printf("%" PRI_UINTN " - Unknown EFI Status value\n", efiStatus);
        break;
    }
}

bool is_Running_Elevated()
{
    return true;//In UEFI, there is no concept of admin or sudo. The getuid function always returns zero, and the geteuid function isn't present at all currently, so it's safe to just return true here.
}

int get_Current_User_Name(char **userName)
{
    int ret = SUCCESS;
    if (userName)
    {
        //while unix functions are there, they are all stubs, so we're just going to return "efi" as the username.
#define UEFI_USER_NAME_LENGTH 4
        *userName = C_CAST(char*, calloc(UEFI_USER_NAME_LENGTH, sizeof(char)));
        if (*userName)
        {
            snprintf(*userName, UEFI_USER_NAME_LENGTH, "efi");
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
