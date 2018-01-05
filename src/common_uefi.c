//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012 - 2017 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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

bool os_Directory_Exists(const char * const pathToCheck)
{
    return false;
}

bool os_File_Exists(const char * const filetoCheck)
{
    return false;
}

int get_Full_Path(const char * pathAndFile, char fullPath[OPENSEA_PATH_MAX])
{
    return FAILURE;
}

void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    //ANSI escape sequences? Or something else?
}

eArchitecture get_Compiled_Architecture(void)
{
    return OPENSEA_ARCH_UNKNOWN;
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
    return calculate_Endianness();
}

int get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char *operatingSystemName)
{
    //TODO: get the UEFI version information and return it.
    //Also, set the operating system name to "UEFI" or "EFI" (If old enough version)
    sprintf(&operatingSystemName[0],"UEFI");
    versionNumber->osVersioningIdentifier = OS_UEFI;
    versionNumber->versionType.uefiVersion.majorVersion = 0;
    versionNumber->versionType.uefiVersion.minorVersion = 0;
    versionNumber->versionType.uefiVersion.revision = 0;
    return NOT_SUPPORTED;
}

int64_t os_Get_File_Size(FILE *filePtr)
{
    return -1;
}

void start_Timer(seatimer_t *timer)
{
    //Cannot use clock_gettime in UEFI EDK2...at least I can't find it in any definitions for it
    /*
    struct timespec startTimespec;
    int ret = 0;
    memset(&startTimespec, 0, sizeof(struct timespec));
    ret = clock_gettime(CLOCK_MONOTONIC, &startTimespec);
    if (0 == ret)//hopefully this always works...-TJE
    {
    //        printf("Start Time:  %lu\n", startTimespec.tv_nsec);
        timer->timerStart = (uint64_t) (startTimespec.tv_sec * (uint64_t) 1000000000) + startTimespec.tv_nsec;
    }
    //    else
    //    {
    //       printf("Bad start_timer Ret:  %d\n",ret);
    //    }
    /*/
    struct timeval startTimespec;
    int ret = 0;
    memset(&startTimespec, 0, sizeof(struct timeval));
    ret = gettimeofday(&startTimespec, NULL);
    if (0 == ret)//hopefully this always works...-TJE
    {
    //        printf("Start Time:  %lu\n", startTimespec.tv_nsec);
        timer->timerStart = (uint64_t) (startTimespec.tv_sec * (uint64_t) 1000000000) + (uint64_t)(startTimespec.tv_usec * (uint64_t) 1000);
    }
    //    else
    //    {
    //       printf("Bad start_timer Ret:  %d\n",ret);
    //    }
    //*/
}

void stop_Timer(seatimer_t *timer)
{
    //Cannot use clock_gettime in UEFI EDK2...at least I can't find it in any definitions for it
    /*
    struct timespec stopTimespec;
    int ret = 0;
    memset(&stopTimespec, 0, sizeof(struct timespec));
    ret = clock_gettime(CLOCK_MONOTONIC, &stopTimespec);
    if (0 == ret)//hopefully this always works...-TJE
    {
    //        printf("Start Time:  %lu\n", startTimespec.tv_nsec);
    timer->timerStart = (uint64_t) (stopTimespec.tv_sec * (uint64_t) 1000000000) + stopTimespec.tv_nsec;
    }
    //    else
    //    {
    //       printf("Bad start_timer Ret:  %d\n",ret);
    //    }
    /*/
    struct timeval stopTimespec;
    int ret = 0;
    memset(&stopTimespec, 0, sizeof(struct timeval));
    ret = gettimeofday(&stopTimespec, NULL);
    if (0 == ret)//hopefully this always works...-TJE
    {
    //        printf("Stop Time:  %lu\n", stopTimespec.tv_usec);
        timer->timerStop = (uint64_t) (stopTimespec.tv_sec * (uint64_t) 1000000000) + (uint64_t)(stopTimespec.tv_usec * (uint64_t) 1000);
    }
    //    else
    //    {
    //       printf("Bad start_timer Ret:  %d\n",ret);
    //    }
    //*/
}

uint64_t get_Nano_Seconds(seatimer_t timer)
{
    return timer.timerStop - timer.timerStart;
}

double get_Micro_Seconds(seatimer_t timer)
{
    uint64_t nanoseconds = get_Nano_Seconds(timer);
    return ((double)nanoseconds / 1000.00);
}

double get_Milli_Seconds(seatimer_t timer)
{
    return (get_Micro_Seconds(timer) / 1000.00);
}

double get_Seconds(seatimer_t timer)
{
    return (get_Milli_Seconds(timer) / 1000.00);
}
