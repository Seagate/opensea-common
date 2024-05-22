// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2012-2023 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file common.c
// \brief Implements functions that are common to OS & Non-OS code.
//
#include "common.h"
#include <ctype.h>

#if defined (_WIN32)
#include <windows.h> //used for setting color output to the command prompt and Sleep()
#else //_WIN32
#include <unistd.h> //needed for usleep() or nanosleep()
#include <time.h>
#include <errno.h>
#endif //Win32

#if defined (VMK_CROSS_COMP)
#include <mm_malloc.h> //doing this to shut up warnings about posix_memalign not available despite stdlib include  TJE
#endif //VMK_CROSS_COMP

#include <stdlib.h>//aligned allocation functions come from here
#include <math.h>
#include <locale.h> //used when getting time to replace ctime and asctime function to try and replicate the format exactly-TJE
#include <limits.h>
#include "common_platform.h"

time_t CURRENT_TIME = 0;
char CURRENT_TIME_STRING[CURRENT_TIME_STRING_LENGTH] = { 0 };

void delay_Milliseconds(uint32_t milliseconds)
{
#if defined(_WIN32) && !defined (UEFI_C_SOURCE)
    Sleep(milliseconds);
#else
    //according to this link: http://linux.die.net/man/3/usleep
    //usleep is obsolete starting in POSIX 2001 and removed entirely in POSIX 2008 and nanosleep is supposed to be used instead.
    //the usleep code is left in just in case it is needed for any reason, but nanosleep works as expected
    #if defined POSIX_1993 && defined _POSIX_TIMERS 
        struct timespec delayTime;
        //NOTE: tv_sec is long in C11, but time_t prior.
        //      tv_nsec is implementation defined until C23 where it is long long
        //      Because of this change due to standardization, there are extra ifdef's for the casts to fix conversion warnings.-TJE
        //Try using typeof for GCC and __typeof__ for clang, unless in C23 where typeof is standard.
        //    
        #if defined (USING_C23)
            //C23, so use definitions from C23
            delayTime.tv_sec = C_CAST(long, milliseconds / UINT32_C(1000));
            delayTime.tv_nsec = C_CAST(long long, UINT32_C(1000000) * (milliseconds % UINT32_C(1000)));
        #elif defined (__clang__)  
            //Use __typeof__
            delayTime.tv_sec = C_CAST(__typeof__(delayTime.tv_sec), milliseconds / UINT32_C(1000));
            delayTime.tv_nsec = C_CAST(__typeof__(delayTime.tv_nsec), UINT32_C(1000000) * (milliseconds % UINT32_C(1000)));
        #elif defined (__GNUC__)
            //Use typeof
            delayTime.tv_sec = C_CAST(typeof(delayTime.tv_sec), milliseconds / UINT32_C(1000));
            delayTime.tv_nsec = C_CAST(typeof(delayTime.tv_nsec), UINT32_C(1000000) * (milliseconds % UINT32_C(1000)));
        #elif defined (USING_C11)
            //Use long and long int as a best guess or most likely correct case
            delayTime.tv_sec = C_CAST(long, milliseconds / UINT32_C(1000));
            delayTime.tv_nsec = C_CAST(long int, UINT32_C(1000000) * (milliseconds % UINT32_C(1000)));
        #else 
            //use time_t and long int
            delayTime.tv_sec = C_CAST(time_t, milliseconds / UINT32_C(1000));
            delayTime.tv_nsec = C_CAST(long int, UINT32_C(1000000) * (milliseconds % UINT32_C(1000)));
        #endif
        
        nanosleep(&delayTime, NULL);
    #else //!POSIX 1993...try usleep instead
        usleep(C_CAST(useconds_t, milliseconds * UINT32_C(1000)));
    #endif //POSIX check
#endif
}

bool get_current_timestamp(void)
{
    bool retStatus = true;
    if (strcmp(CURRENT_TIME_STRING, "") == 0)
    {
        struct tm logTime;
        memset(&logTime, 0, sizeof(struct tm));
        CURRENT_TIME = time(NULL);
        memset(CURRENT_TIME_STRING, 0, sizeof(CURRENT_TIME_STRING) / sizeof(*CURRENT_TIME_STRING));
        retStatus = strftime(CURRENT_TIME_STRING, CURRENT_TIME_STRING_LENGTH, "%Y%m%dT%H%M%S", get_Localtime(&CURRENT_TIME, &logTime));
    }
    return retStatus;
}

void delay_Seconds(uint32_t seconds)
{
    delay_Milliseconds(UINT32_C(1000) * seconds);
}

#if defined (_MSC_VER) && _MSC_VER <= 1800 && defined _WIN32
int snprintf(char *buffer, size_t bufsz, const char *format, ...)
{
    int charCount = -1;
    va_list args, countargs;
    va_start(args, format);
    va_copy(countargs, args);//C99, but available in VS2013 which is the oldest VS compiler we expect to possibly work with this code.
    if (bufsz > 0) //Allow calling only when bufsz > 0. Let _vsnprintf evaluate if buffer is NULL in here.
    {
        errno = 0;
        charCount = _vsnprintf(buffer, bufsz, format, args);
    }
    if (charCount == -1)
    {
        if (errno == EINVAL || errno == EILSEQ)
        {
            //do not change the return value or attempt any other actions.
        }
        else
        {
            //out of space or some error occurred, so need to null terminate and calculate how long the buffer should have been for this format
            if (buffer && bufsz > 0)
            {
                //null terminate at bufsz since there was no more room, so we are at the end of the buffer already.
                buffer[bufsz - 1] = '\0';
            }
            charCount = _vscprintf(format, countargs);//gets the count of the number of args
        }
    }
    va_end(args);
    va_end(countargs);
    return charCount;
}
#endif

//returns number of milliseconds since 1970 UTC
uint64_t get_Milliseconds_Since_Unix_Epoch(void)
{
    uint64_t msSinceJan1970 = 0;
    //Check for C11's standardized API call.
    //Also check that TIME_UTC is defined, because if it is not, then this will not work.
    //This apparently causes an error in mingww64 environment in msys2 since this function is missing, but C11 is defined.
#if defined(USING_C11) && defined (TIME_UTC)
    struct timespec now;
    memset(&now, 0, sizeof(struct timespec));
    if (0 != timespec_get(&now, TIME_UTC))
    {
        //NOTE: Technically this is a time since the system's epoch as C standard does not set an epoch.
        //      I'm not sure there is a system we have encountered where it is not the Unix epoch, so this is fine for now - TJE
        msSinceJan1970 = (C_CAST(uint64_t, now.tv_sec) * UINT64_C(1000)) + (C_CAST(uint64_t, now.tv_nsec) / UINT64_C(1000000));
    }
    else 
#endif //C11
    {
#if defined (POSIX_1993) && defined _POSIX_TIMERS 
        //POSIX gettimeofday() or clock_gettime(CLOCK_REALTIME, ts) https://pubs.opengroup.org/onlinepubs/9699919799/functions/clock_getres.html 
        //NOTE: Using clock_gettime since it's more accurate and not affected by time-skew
        struct timespec posixnow;
        memset(&posixnow, 0, sizeof(struct timespec));
        if (0 == clock_gettime(CLOCK_REALTIME, &posixnow))
        {
            msSinceJan1970 = (C_CAST(uint64_t, posixnow.tv_sec) * UINT64_C(1000)) + (C_CAST(uint64_t, posixnow.tv_nsec) / UINT64_C(1000000));
        }
        else
#elif defined (_MSC_VER)
        //Use the function in the link below, but MSFT also documents another way to do this, which is what we've implemented -TJE
        //https://learn.microsoft.com/en-us/windows/win32/api/winternl/nf-winternl-rtltimetosecondssince1970
        FILETIME ftnow;
        FILETIME epochfile;
        SYSTEMTIME epoch;
        ULARGE_INTEGER nowAsInt;
        ULARGE_INTEGER epochAsInt;
        memset(&ftnow, 0, sizeof(FILETIME));
        memset(&epochfile, 0, sizeof(FILETIME));
        memset(&epoch, 0, sizeof(SYSTEMTIME));
        memset(&nowAsInt, 0, sizeof(ULARGE_INTEGER));
        memset(&epochAsInt, 0, sizeof(ULARGE_INTEGER));
        epoch.wYear = 1970;
        epoch.wMonth = 1;
        epoch.wSecond = 0;//MSFT says to set to first second.
        epoch.wDayOfWeek = 4;//Thursday
        epoch.wDay = 1;
        GetSystemTimeAsFileTime(&ftnow);
        memcpy(&nowAsInt, &ftnow, sizeof(ULARGE_INTEGER));//these are both 8 bytes in size, so it SHOULD be fine and MSFT says to do this-TJE
        SystemTimeToFileTime(&epoch, &epochfile);
        memcpy(&epochAsInt, &epochfile, sizeof(ULARGE_INTEGER));//these are both 8 bytes in size, so it SHOULD be fine and MSFT says to do this-TJE
        if (nowAsInt.QuadPart >= epochAsInt.QuadPart)//this should always be true, but seems like microsoft is likely checking this, so checking this as well-TJE
        {
            msSinceJan1970 = C_CAST(uint64_t, (nowAsInt.QuadPart - epochAsInt.QuadPart)) / UINT64_C(10000);//subtracting the 2 large integers gives 100 nanosecond intervals since jan 1, 1970, so divide by 10000 to get milliseconds (divide by 10000000 to get seconds)
        }
        else
#endif //OS unique methods to get this info
        {
            //time_t is not guaranteed to always be a specific number of units. This can be system specific/library specific.
            //so to convert using old standards, take a current time_t, convert it to struct tm, then calculate it.
            //If time_t is 32bits, then this will not work past 2038.
            //To detect this, we just need to check if the current year is less than 1970. If it is, it could be a roll over problem or some other clock issue.
            //in that case, just return 0.
            //NOTE: This code seems to work until approximately the year 275705. The following year I've noticed can be off by a day. There is probably a missing
            //      adjustment factor somewhere else, but this is close enough-TJE
            //NOTE: To take into account leap seconds you need a table, or you can average 1 every 1.5 years: https://www.nist.gov/pml/time-and-frequency-division/leap-seconds-faqs#often
            //      This is not currently taken into account -TJE
            struct tm nowstruct;
            time_t currentTime = time(NULL);
            memset(&nowstruct, 0, sizeof(struct tm));
            //to get the milliseconds, need to convert this to struct tm, then calculate this.
            get_UTCtime(&currentTime, &nowstruct);
            uint64_t currentyear = (C_CAST(uint64_t, nowstruct.tm_year) + UINT64_C(1900));
            if (currentyear >= UINT64_C(1970))
            {
                //need to account for leap years before calculating below. This will get how many leap years there are between the current year and 1970 to add to number of days for adjustment
                uint64_t leapyears = (currentyear - UINT64_C(1970)) / UINT64_C(4);
                //now subtract the number of skipped leap years (every 100 years unless divible by 400). This needs to be number since 1970
                uint64_t skippedleapyears = UINT64_C(0);
                //years 2000 and 2400 are leap years, but 1700, 1800, 1900, 2100, 2200, and 2300 are not.
                //To get skipped leap years, need to calculate based on number of centuries that have passed since 1970, then check how many have skipped leap year.
                //No need to worry about this until at least 2100, however if you skip the year 2000, this causes a problem in 24606 where it will be off by 1 day.-TJE
                if (currentyear >= UINT64_C(2000))//start at 2000 since it is the first century after 1970.
                {
                    //at least year 2100, so need to handle skipped leap years and how many of these have occurred
                    uint64_t centuriesSince1970 = ((currentyear - UINT64_C(1970)) / UINT64_C(100)) - UINT64_C(1);
                    //use a loop to check each century, starting in 2100 to see if it was a skipped leap year or not.
                    uint64_t checkCentury = UINT64_C(2100);
                    while (checkCentury <= (UINT64_C(2100) + (centuriesSince1970 * UINT64_C(100))))
                    {
                        if (checkCentury % UINT64_C(400) != UINT64_C(0))
                        {
                            //This century does not have a leap year
                            skippedleapyears += UINT64_C(1);
                        }
                        checkCentury += UINT64_C(100);
                    }
                }
                leapyears -= skippedleapyears;
                //Need to detect if the leap year is in the current year and if it has occured already or not.
                if (currentyear % UINT64_C(4) == UINT64_C(0))
                {
                    bool adjustforcurrentleapyear = true;
                    //leap year is in the current year, unless it is a year to skip a leap year (every 100 years, unless divisible by 400)
                    if (currentyear % UINT64_C(100) == UINT64_C(0) && currentyear % UINT64_C(400) != UINT64_C(0))
                    {
                        //currently in a century year without a leap year. No need for the adjustment below
                        adjustforcurrentleapyear = false;
                    }
                    if (adjustforcurrentleapyear)
                    {
                        //Check if it is past february 29th already or not.
                        if (nowstruct.tm_mon < 2)
                        {
                            //february or january
                            if (nowstruct.tm_mon == 0 || (nowstruct.tm_mon == 1 && nowstruct.tm_mday <= 29))
                            {
                                //subtract 1 from leapyears since it has not yet occurred for the current year
                                leapyears -= UINT64_C(1);
                            }
                        }
                    }
                }
                //use tm_sec, tm_min, tm_hour, tm_year, and tm_yday to convert this.-TJE
                msSinceJan1970 = (C_CAST(uint64_t, nowstruct.tm_sec) * UINT64_C(1000)) + (C_CAST(uint64_t, nowstruct.tm_min) * UINT64_C(60000)) + (C_CAST(uint64_t, nowstruct.tm_hour) * UINT64_C(3600000)) + ((C_CAST(uint64_t, nowstruct.tm_yday) + leapyears) * UINT64_C(86400000)) + ((currentyear - UINT64_C(1970)) * UINT64_C(31536000000));
            }
            else
            {
                //some weird time skew issue or 32bit time_t 2038 issue, so just return zero for an invalid value.-TJE
                //NOTE: with a 32bit time_t you could probably implement a clever workaround to adjust it to a correct year, but that is not worth
                //      implmenting in this code. I don't even know which modern OS is using a 32bit time_t in 2024. -TJE
                //      If a workaround to adjust the year was implemented, it would likely only work until some future time where the same 32bit problem happens again.-TJE
                msSinceJan1970 = UINT64_C(0);
            }
        }
    }
    return msSinceJan1970;
}

//TODO: C11 says supported alignments are implementation defined
//      We may want an if/else to call back to a generic method if it fails some day. (unlikely, so not done right now)
//      NOTE: There may also be other functions to do this for other compilers or systems, but they are not known today. Add them as necessary
//      NOTE: some systems may have memalign instead of the posix definition, but it is not clear how to detect that implementation with feature test macros.
//      NOTE: In UEFI, using the EDK2, malloc will provide an 8-byte alignment, so it may be possible to do some aligned allocations using it without extra work. but we can revist that later.
void *malloc_aligned(size_t size, size_t alignment)
{
    /**
     * Lets not do anything for VMWare
     */
    #if defined (VMK_CROSS_COMP)
        #ifdef _DEBUG
            printf("<--%s size : %d  alignment : %d\n",__FUNCTION__, size, alignment);
        #endif
        void *temp = NULL;
        if (0 != posix_memalign( &temp, alignment, size))
        {
            temp = NULL;//make sure the system we are running didn't change this.
        }

        return temp;
    #else

    #if !defined(__MINGW32__) && !defined(UEFI_C_SOURCE) && defined (USING_C11) && !defined(_MSC_VER)
        //C11 added an aligned alloc function we can use
        //One additional requirement of this function is that the size must be a multiple of alignment, so we will check and round up the size to make this easy.
        if (size % alignment)
        {
            size = size + alignment - (size % alignment);
        }
        return aligned_alloc(alignment, size);
    #elif !defined(UEFI_C_SOURCE) && defined (__INTEL_COMPILER) || defined (__ICC)
        //_mm_malloc
        return _mm_malloc(C_CAST(int, size), C_CAST(int, alignment));
    #elif !defined(UEFI_C_SOURCE) && defined (POSIX_2001)
        //POSIX.1-2001 and higher define support for posix_memalign
        void *temp = NULL;
        if (0 != posix_memalign( &temp, alignment, size))
        {
            temp = NULL;//make sure the system we are running didn't change this.
        }
        return temp;
    #elif !defined(UEFI_C_SOURCE) && defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
        //mingw32 has an aligned malloc function that is not available in mingw64.
        return __mingw_aligned_malloc(size, alignment);
    #elif !defined(UEFI_C_SOURCE) && defined (_MSC_VER) || defined (__MINGW64_VERSION_MAJOR)
        //use the MS _aligned_malloc function. Mingw64 will support this as well from what I can find - TJE
        return _aligned_malloc(size, alignment);
    #else
        //need a generic way to do this with some math and overallocating...not preferred but can make it work.
        //This can waste a LOT of memory in some cases depending on the required alignment.
        //This way means overallocating and adding to get to the required alignment...then knowing how much we over aligned by.
        //Will store the original starting pointer right before the aligned pointer we return to the caller.
        void *temp = NULL;
        //printf("\trequested allocation: size = %zu  alignment = %zu\n", size, alignment);
        if (size && (alignment > 0) && ((alignment & (alignment - 1)) == 0))//Check that we have a size to allocate and enforce that the alignment value is a power of 2.
        {
            size_t requiredExtraBytes = sizeof(size_t);//We will store the original beginning address in front of the return data pointer
            const size_t totalAllocation = size + alignment + requiredExtraBytes;
            temp = malloc(totalAllocation);
            if (temp)
            {
                const void * const originalLocation = temp;
                temp += requiredExtraBytes;//allow enough room for storing the original pointer location
                //now offset based on the required alignment
                temp += alignment - (C_CAST(size_t, temp) % alignment);
                //aligned.
                //now save the original pointer location
                size_t *savedLocationData = C_CAST(size_t*, temp - sizeof(size_t));
                *savedLocationData = C_CAST(size_t, originalLocation);
            }
        }
        //else
        //{
        //    printf("\trequest did not meet requirements for generic allocation function\n");
        //}
        return temp;
    #endif

    #endif
}

void free_aligned(void* ptr)
{
    #if defined (VMK_CROSS_COMP)
    free(ptr);
    #else
    #if !defined(__MINGW32__) && !defined(UEFI_C_SOURCE) && defined (USING_C11) && !defined(_MSC_VER)
        //just call free
        free(ptr);
    #elif !defined(UEFI_C_SOURCE) && defined (__INTEL_COMPILER) || defined (__ICC)
        //_mm_free
        _mm_free(ptr);
    #elif !defined(UEFI_C_SOURCE) && defined (POSIX_2001)
        //POSIX.1-2001 and higher define support for posix_memalign
        //Just call free
        free(ptr);
    #elif !defined(UEFI_C_SOURCE) && defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
        //mingw32 has an aligned malloc function that is not available in mingw64.
        __mingw_aligned_free(ptr);
    #elif !defined(UEFI_C_SOURCE) && defined (_MSC_VER) || defined (__MINGW64_VERSION_MAJOR)
        //use the MS _aligned_free function
        _aligned_free(ptr);
    #else
        //original pointer
        if (ptr)
        {
            //find the starting address from the original allocation.
            void *tempPtr = ptr - sizeof(size_t);//this gets us to where it was stored
            //this will set it back to what it's supposed to be
            tempPtr = C_CAST(void*, *(C_CAST(size_t*, tempPtr)));
            free(tempPtr);
        }
    #endif
    #endif
}

void *calloc_aligned(size_t num, size_t size, size_t alignment)
{
    //call malloc aligned and memset
    void *zeroedMem = NULL;
    size_t numSize = num * size;
    if (numSize)
    {
        zeroedMem = malloc_aligned(numSize, alignment);

        if (zeroedMem)
        {
            memset(zeroedMem, 0, numSize);
        }
    }
    return zeroedMem;
}

void *realloc_aligned(void *alignedPtr, size_t originalSize, size_t size, size_t alignment)
{
    void *temp = NULL;
    if (originalSize == 0)//if this is zero, they don't want or care to keep the data
    {
        free_aligned(alignedPtr);
        alignedPtr = NULL;
    }
    if (size)
    {
        temp = malloc_aligned(size, alignment);
        if (alignedPtr && originalSize && temp)
        {
            memcpy(temp, alignedPtr, originalSize);
        }
    }
    if (alignedPtr && temp)
    {
        //free the old pointer
        free_aligned(alignedPtr);
    }
    return temp;
}

size_t get_System_Pagesize(void)
{
    #if defined (UEFI_C_SOURCE)
        return 4096;//This is not the processor page size, just the pagesize allocated by EDK2. It's in <dePkg/Include/Uefi/UedfiBaseType.h
    #elif defined (POSIX_2001)
        //use sysconf: http://man7.org/linux/man-pages/man3/sysconf.3.html
        return C_CAST(size_t, sysconf(_SC_PAGESIZE));
    #elif defined (_POSIX_VERSION) //this may not be the best way to test this, but I think it will be ok.
        //use get page size: http://man7.org/linux/man-pages/man2/getpagesize.2.html
        return C_CAST(size_t, getpagesize());
    #elif defined (_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
        SYSTEM_INFO winSysInfo;
        memset(&winSysInfo, 0, sizeof(SYSTEM_INFO));
        GetSystemInfo(&winSysInfo);
        return C_CAST(size_t, winSysInfo.dwPageSize);
    #else
        return -1;//unknown, so return something easy to see an error with.
    #endif
}

void *malloc_page_aligned(size_t size)
{
    size_t pageSize = get_System_Pagesize();
    if (pageSize)
    {
        return malloc_aligned(size, pageSize);
    }
    else
    {
        return NULL;
    }
}

void free_page_aligned(void* ptr)
{
    free_aligned(ptr);
}

void *calloc_page_aligned(size_t num, size_t size)
{
    size_t pageSize = get_System_Pagesize();
    if (pageSize)
    {
        return calloc_aligned(num, size, get_System_Pagesize());
    }
    else
    {
        return NULL;
    }
}

void *realloc_page_aligned(void *alignedPtr, size_t originalSize, size_t size)
{
    size_t pageSize = get_System_Pagesize();
    if (pageSize)
    {
        return realloc_aligned(alignedPtr, originalSize, size, get_System_Pagesize());
    }
    else
    {
        return NULL;
    }
}

void nibble_Swap(uint8_t *byteToSwap)
{
    *byteToSwap = C_CAST(uint8_t, ((*byteToSwap & UINT8_C(0x0F)) << 4)) | C_CAST(uint8_t, ((*byteToSwap & UINT8_C(0xF0)) >> 4));
}

void byte_Swap_16(uint16_t *wordToSwap)
{
    *wordToSwap = C_CAST(uint16_t, ((*wordToSwap & UINT16_C(0x00FF)) << 8)) | C_CAST(uint16_t, ((*wordToSwap & UINT16_C(0xFF00)) >> 8));
}

void byte_Swap_Int16(int16_t *signedWordToSwap)
{
    *signedWordToSwap = C_CAST(int16_t, ((*signedWordToSwap & UINT16_C(0x00FF)) << 8)) | C_CAST(int16_t, ((*signedWordToSwap & UINT16_C(0xFF00)) >> 8));
}

void big_To_Little_Endian_16(uint16_t *wordToSwap)
{
    if (get_Compiled_Endianness() == OPENSEA_LITTLE_ENDIAN)
    {
        byte_Swap_16(wordToSwap);
    }
}

void byte_Swap_32(uint32_t *doubleWordToSwap)
{
    *doubleWordToSwap = C_CAST(uint32_t, ((*doubleWordToSwap & UINT32_C(0x0000FFFF)) << 16)) | C_CAST(uint32_t, ((*doubleWordToSwap & UINT32_C(0xFFFF0000)) >> 16));
    *doubleWordToSwap = C_CAST(uint32_t, ((*doubleWordToSwap & UINT32_C(0x00FF00FF)) << 8)) | C_CAST(uint32_t, ((*doubleWordToSwap & UINT32_C(0xFF00FF00)) >> 8));
}

void byte_Swap_Int32(int32_t *signedDWord)
{
    // uint32_t temp = C_CAST(uint32_t, *signedDWord);
    // byte_Swap_32(&temp);
    // *signedDWord = C_CAST(int32_t, temp);

    *signedDWord = C_CAST(int32_t, ((*signedDWord & C_CAST(int32_t, INT32_C(0x0000FFFF))) << 16)) | C_CAST(int32_t, ((*signedDWord & C_CAST(int32_t, INT32_C(0xFFFF0000))) >> 16));
    *signedDWord = C_CAST(int32_t,((*signedDWord & C_CAST(int32_t, INT32_C(0x00FF00FF))) << 8)) | C_CAST(int32_t,((*signedDWord & C_CAST(int32_t, INT32_C(0xFF00FF00))) >> 8));
}
void big_To_Little_Endian_32(uint32_t *doubleWordToSwap)
{
    if (get_Compiled_Endianness() == OPENSEA_LITTLE_ENDIAN)
    {
        byte_Swap_32(doubleWordToSwap);
    }
}

void word_Swap_32(uint32_t *doubleWordToSwap)
{
    *doubleWordToSwap = ((*doubleWordToSwap & UINT32_C(0x0000FFFF)) << 16) | ((*doubleWordToSwap & UINT32_C(0xFFFF0000)) >> 16);
}

void byte_Swap_64(uint64_t *quadWordToSwap)
{
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00000000FFFFFFFF)) << 32) | ((*quadWordToSwap & UINT64_C(0xFFFFFFFF00000000)) >> 32);
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x0000FFFF0000FFFF)) << 16) | ((*quadWordToSwap & UINT64_C(0xFFFF0000FFFF0000)) >> 16);
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00FF00FF00FF00FF)) << 8) | ((*quadWordToSwap & UINT64_C(0xFF00FF00FF00FF00)) >> 8);
}

void word_Swap_64(uint64_t *quadWordToSwap)
{
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00000000FFFFFFFF)) << 32) | ((*quadWordToSwap & UINT64_C(0xFFFFFFFF00000000)) >> 32);
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x0000FFFF0000FFFF)) << 16) | ((*quadWordToSwap & UINT64_C(0xFFFF0000FFFF0000)) >> 16);
}

void double_Word_Swap_64(uint64_t *quadWordToSwap)
{
    *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00000000FFFFFFFF)) << 32) | ((*quadWordToSwap & UINT64_C(0xFFFFFFFF00000000)) >> 32);
}

int16_t celsius_To_Fahrenheit(int16_t *celsius)
{
    int16_t fahrenheit = 0;
    fahrenheit = C_CAST(int16_t, (*celsius * (9.0 / 5.0)) + 32.0);
    return fahrenheit;
}

int16_t fahrenheit_To_celsius(int16_t *fahrenheit)
{
    int16_t celsius = 0;
    celsius = C_CAST(int16_t, (*fahrenheit - 32.0) * (5.0 / 9.0));
    return celsius;
}

int16_t celsius_To_Kelvin(int16_t *celsius)
{
    int16_t kelvin = 0;
    kelvin = C_CAST(int16_t, *celsius + 273.15);
    return kelvin;
}

int16_t fahrenheit_To_Kelvin(int16_t *fahrenheit)
{
    int16_t kelvin = 0;
    kelvin = C_CAST(int16_t, (5.0 / 9.0) * (*fahrenheit - 32.0) + 273.15);
    return kelvin;
}

int16_t kelvin_To_Celsius(int16_t *kelvin)
{
    int16_t celsius = 0;
    celsius = C_CAST(int16_t, *kelvin - 273.15);
    return celsius;
}

int16_t kelvin_To_Fahrenheit(int16_t *kelvin)
{
    int16_t fahrenheit = 0;
    fahrenheit = C_CAST(int16_t, (9.0 / 5.0) * (*kelvin - 273.15) + 32.0);
    return fahrenheit;
}

//use this to swap the bytes in a string...useful for ATA strings
void byte_Swap_String(char *stringToChange)
{
    size_t stringlen = strlen(stringToChange) + 1;
    if (stringlen > 1)//greater than 1 since we append 1 for a null
    {
        char* swappedString = C_CAST(char*, calloc(stringlen, sizeof(char)));
        if (swappedString == NULL)
        {
            return;
        }

        for (size_t stringIter = 0; stringIter < (stringlen - 1); stringIter += 2)//strlen - 1 to make sure NULL terminator will not be touched with other changes made in this function -TJE
        {
            swappedString[stringIter] = stringToChange[stringIter + 1];
            if (stringIter + 1 < stringlen)
            {
                swappedString[stringIter + 1] = stringToChange[stringIter];
            }
        }
        memset(stringToChange, 0, stringlen);
        memcpy(stringToChange, swappedString, stringlen);
        safe_Free(swappedString);
    }
}
void remove_Whitespace_Left(char *stringToChange)
{
    size_t iter = 0, len = 0;
    if (stringToChange == NULL)
    {
        return;
    }
    len = strspn(stringToChange, " \t\n\v\f"); //only touch spaces at the beginning of the string, not the whole string
    if (len == 0)
    {
        return;
    }

    while ((iter < (strlen(stringToChange) - 1) && stringToChange[iter]))  // having issues with the isspace command leaving extra chars in the string
    {
        stringToChange[iter] = stringToChange[iter + len];
        iter++;
    }
}
void remove_Trailing_Whitespace(char *stringToChange)
{
    size_t iter = 0;
    if (stringToChange == NULL)
    {
        return;
    }
    iter = (strlen(stringToChange));
    if (iter == 0)
    {
        return;
    }
    while (iter > 0 && is_ASCII(stringToChange[iter - 1]) && isspace(stringToChange[iter - 1]))
    {
        stringToChange[iter - 1] = '\0'; //replace spaces with NULL terminators
        iter--;
    }
}

void remove_Leading_Whitespace(char *stringToChange)
{
    size_t iter = 0, stringToChangeLen = 0;
    if (stringToChange == NULL)
    {
        return;
    }
    stringToChangeLen = strlen(stringToChange);
    while (is_ASCII(stringToChange[iter]) && isspace(stringToChange[iter]) && iter < stringToChangeLen)
    {
        iter++;
    }
    if (iter > 0)
    {
        memmove(&stringToChange[0], &stringToChange[iter], stringToChangeLen - iter);
        memset(&stringToChange[stringToChangeLen - iter], 0, iter);//should this be a null? Or a space? Leaving as null for now since it seems to work...
    }
}

void remove_Leading_And_Trailing_Whitespace(char *stringToChange)
{
    remove_Leading_Whitespace(stringToChange);
    remove_Trailing_Whitespace(stringToChange);
}

void convert_String_To_Upper_Case(char *stringToChange)
{
    size_t stringLen = 0, iter = 0;
    if (stringToChange == NULL)
    {
        return;
    }
    stringLen = strlen(stringToChange);
    if (stringLen == 0)
    {
        return;
    }
    while (iter <= stringLen)
    {
        stringToChange[iter] = C_CAST(char, toupper(stringToChange[iter]));
        iter++;
    }
}

void convert_String_To_Lower_Case(char *stringToChange)
{
    size_t stringLen = 0, iter = 0;
    if (stringToChange == NULL)
    {
        return;
    }
    stringLen = strlen(stringToChange);
    if (stringLen == 0)
    {
        return;
    }
    while (iter <= stringLen)
    {
        stringToChange[iter] = C_CAST(char, tolower(stringToChange[iter]));
        iter++;
    }
}

void convert_String_To_Inverse_Case(char *stringToChange)
{
    size_t stringLen = 0, iter = 0;
    if (stringToChange == NULL)
    {
        return;
    }
    stringLen = strlen(stringToChange);
    if (stringLen == 0)
    {
        return;
    }
    while (iter <= stringLen)
    {
        if (islower(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, tolower(stringToChange[iter]));
        }
        else if (isupper(stringToChange[iter]))
        {
            stringToChange[iter] = C_CAST(char, toupper(stringToChange[iter]));
        }
        iter++;
    }
}

size_t find_last_occurrence_in_string(const char *originalString, const char *stringToFind)
{
    char *stringToCompare = C_CAST(char*, originalString);
    size_t last_occurrence = strlen(originalString);

    while (stringToCompare != NULL)
    {
        char *partialString = strstr(stringToCompare, stringToFind);
        if (partialString != NULL)
        {
            last_occurrence = strlen(partialString);
            partialString += strlen(stringToFind);
            stringToCompare = strstr(partialString, stringToFind);
        }
        else
        {
            break;
        }
    }

    return last_occurrence;
}

size_t find_first_occurrence_in_string(char *originalString, char *stringToFind)
{
    char *stringToCompare = originalString;

    char *partialString = strstr(stringToCompare, stringToFind);
    if (partialString != NULL)
    {
        return (size_t)(partialString - originalString);
    }

    return strlen(originalString);
}

bool wildcard_Match(char * pattern, char * data)
{
    if (*pattern == '\0' && *data == '\0')
        return true;

    if (*pattern == *data || *pattern == '?')
        return *data != '\0' && wildcard_Match(pattern + 1, data + 1);

    if (*pattern == '*')
        return wildcard_Match(pattern + 1, data) || (*data != '\0' && wildcard_Match(pattern, data + 1));

    return false;
}

void print_Return_Enum(char *funcName, eReturnValues ret)
{
    if (NULL == funcName)
    {
        printf("Unknown funtion returning: ");
    }
    else
    {
        printf("%s returning: ", funcName);
    }

    switch (ret)
    {
    case SUCCESS:
        printf("SUCCESS\n");
        break;
    case FAILURE:
        printf("FAILURE\n");
        break;
    case NOT_SUPPORTED:
        printf("NOT SUPPORTED\n");
        break;
    case COMMAND_FAILURE:
        printf("COMMAND FAILURE\n");
        break;
    case IN_PROGRESS:
        printf("IN PROGRESS\n");
        break;
    case ABORTED:
        printf("ABORTED\n");
        break;
    case BAD_PARAMETER:
        printf("BAD PARAMETER\n");
        break;
    case MEMORY_FAILURE:
        printf("MEMORY FAILURE\n");
        break;
    case OS_PASSTHROUGH_FAILURE:
        printf("OS PASSTHROUGH FAILURE\n");
        break;
    case LIBRARY_MISMATCH:
        printf("LIBRARY MISMATCH\n");
        break;
    case FROZEN:
        printf("FROZEN\n");
        break;
    case PERMISSION_DENIED:
        printf("PERMISSION DENIED\n");
        break;
    case FILE_OPEN_ERROR:
        printf("FILE OPEN ERROR\n");
        break;
    case WARN_INCOMPLETE_RFTRS:
        printf("WARNING INCOMPLETE RTFRS\n");
        break;
    case OS_COMMAND_TIMEOUT:
        printf("COMMAND TIMEOUT\n");
        break;
    case WARN_NOT_ALL_DEVICES_ENUMERATED:
        printf("WARNING NOT ALL DEVICES ENUMERATED\n");
        break;
    case WARN_INVALID_CHECKSUM:
        printf("WARN INVALID CHECKSUM\n");
        break;
    case OS_COMMAND_NOT_AVAILABLE:
        printf("OS COMMAND NOT AVAILABLE\n");
        break;
    case OS_COMMAND_BLOCKED:
        printf("OS COMMAND BLOCKED\n");
        break;
    case COMMAND_INTERRUPTED:
        printf("COMMAND INTERRUPTED\n");
        break;
    case VALIDATION_FAILURE:
        printf("VALIDATION FAILURE\n");
        break;
    case STRIP_HDR_FOOTER_FAILURE:
        printf("STRIP HDR FOOTER FAILURE\n");
        break;
    case PARSE_FAILURE:
        printf("PARSE FAILURE\n");
        break;
    case INVALID_LENGTH:
        printf("INVALID LENGTH\n");
        break;
    case ERROR_WRITING_FILE:
        printf("ERROR WRITING FILE\n");
        break;
    case TIMEOUT:
        printf("TIMEOUT\n");
        break;
    case OS_TIMEOUT_TOO_LARGE:
        printf("OS TIMEOUT TOO LARGE\n");
        break;
    case PARSING_EXCEPTION_FAILURE:
        printf("PARSING EXCEPTION FAILURE\n");
        break;
    case POWER_CYCLE_REQUIRED:
        printf("POWER CYCLE REQUIRED\n");
        break;
    case DIR_CREATION_FAILED:
        printf("DIR CREATION FAILED\n");
        break;
    case FILE_READ_ERROR:
        printf("FILE READ ERROR\n");
        break;
    case DEVICE_ACCESS_DENIED:
        printf("DEVICE ACCESS DENIED\n");
        break;
    case NOT_PARSED:
        printf("NOT PARSED\n");
        break;
    case MISSING_INFORMATION:
        printf("MISSING INFORMATION\n");
        break;
    case TRUNCATED_FILE:
        printf("TRUNCATED FILE\n");
        break;
    case UNKNOWN:
        printf("UNKNOWN\n");
        break;
    //NO DEFAULT CASE! This will cause warnings when an enum value is not in this switch-case so that it is never out of date!
    }
    printf("\n");
}

static void internal_Print_Data_Buffer(uint8_t* dataBuffer, uint32_t bufferLen, bool showPrint, bool showOffset)
{
    uint32_t printIter = 0, offset = 0;
    uint32_t offsetWidth = 2;//used to figure out how wide we need to pad with 0's for consistent output, 2 is the minimum width
    if (showOffset)
    {
        if (bufferLen <= UINT8_MAX)
        {
            offsetWidth = 2;
            printf("\n        "); //0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ");
        }
        else if (bufferLen <= UINT16_MAX)
        {
            offsetWidth = 4;
            printf("\n          "); //0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ");
        }
        else if (bufferLen <= 0xFFFFFF)
        {
            offsetWidth = 6;
            printf("\n            "); //0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ");
        }
        else//32bit width, don't care about 64bit since max size if 32bit
        {
            offsetWidth = 8;
            printf("\n              "); //0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ");
        }
        //we print out 2 (0x) + printf formatting width + 2 (spaces) then the offsets

        for (printIter = 0; printIter < 16 && printIter < bufferLen; printIter++)
        {
            printf("%" PRIX32 "  ", printIter);
        }
    }
    char lineBuff[18] = { 0 };
    uint8_t lineBuffIter = 0;
    for (printIter = 0, offset = 0; printIter < bufferLen; ++printIter, ++lineBuffIter)
    {
        if (lineBuffIter > sizeof(lineBuff))
        {
            lineBuffIter = 0;
        }

        //for every 16 bytes we print, we need to make a newline, then print the offset (hex) before we print the data again
        if (printIter % 16 == 0)
        {
            if (showOffset)
            {
                switch (offsetWidth)
                {
                case 4:
                    printf("\n  0x%04"PRIX32" ", offset);
                    break;
                case 6:
                    printf("\n  0x%06"PRIX32" ", offset);
                    break;
                case 8:
                    printf("\n  0x%08"PRIX32" ", offset);
                    break;
                case 2:
                default:
                    printf("\n  0x%02"PRIX32" ", offset);
                    break;
                }
            }
            else
            {
                printf("\n  ");
            }
            offset += 16;
        }
        printf("%02"PRIX8" ", dataBuffer[printIter]);
        if (showPrint)
        {
            if (is_ASCII(dataBuffer[printIter]) && isprint(C_CAST(int, dataBuffer[printIter])))
            {
                lineBuff[lineBuffIter] = C_CAST(char, dataBuffer[printIter]);
            }
            else
            {
                lineBuff[lineBuffIter] = '.';
            }
        }
        if (showPrint && ((printIter + 1) % 16 == 0 || printIter + 1 == bufferLen))
        {
            lineBuff[17] = '\0';
            lineBuffIter = UINT8_MAX;//this is done to cause an overflow when the ++happens during the loop
            //need to calculate if padding is needed before printing character translations...but how...
            uint32_t spacePadding = (printIter + 1) % 16;
            if (spacePadding)
            {
                uint32_t counter = 0;
                while (counter < ((16 - spacePadding)))
                {
                    printf("   ");
                    counter++;
                }
            }
            //space after last printed hex 
            printf("  ");
            printf("%s", lineBuff);
            memset(lineBuff, 0, sizeof(lineBuff) / sizeof(*lineBuff));
        }
    }
    printf("\n\n");
}

void print_Data_Buffer(uint8_t* dataBuffer, uint32_t bufferLen, bool showPrint)
{
    internal_Print_Data_Buffer(dataBuffer, bufferLen, showPrint, true);
}

void print_Pipe_Data(uint8_t* dataBuffer, uint32_t bufferLen)
{
    internal_Print_Data_Buffer(dataBuffer, bufferLen, false, false);
}

eReturnValues metric_Unit_Convert(double *byteValue, char** metricUnit)
{
    eReturnValues ret = SUCCESS;
    uint8_t unitCounter = 0;

    if (!byteValue || !metricUnit || !*metricUnit)
    {
        return BAD_PARAMETER;
    }

    while ((*byteValue / 1000.0) >= 1 && (unitCounter + 1) < 8)
    {
        *byteValue = *byteValue / 1000.00;
        unitCounter += 1;
    }
    switch (unitCounter)
    {
    case 0:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "B");
        break;
    case 1:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "KB");
        break;
    case 2:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "MB");
        break;
    case 3:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "GB");
        break;
    case 4:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "TB");
        break;
    case 5:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "PB");
        break;
    case 6:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "EB");
        break;
    case 7:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "ZB");
        break;
    case 8:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "YB");
        break;
    default:
        ret = FAILURE;
    }
    return ret;
}

eReturnValues capacity_Unit_Convert(double *byteValue, char** capacityUnit)
{
    eReturnValues ret = SUCCESS;
    uint8_t unitCounter = 0;

    if (!byteValue || !capacityUnit || !*capacityUnit)
    {
        return BAD_PARAMETER;
    }

    while ((*byteValue / 1024.0) >= 1 && (unitCounter + 1) < 8)
    {
        *byteValue = *byteValue / 1024.00;
        unitCounter += 1;
    }
    switch (unitCounter)
    {
    case 0:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "B");
        break;
    case 1:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "KiB");
        break;
    case 2:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "MiB");
        break;
    case 3:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "GiB");
        break;
    case 4:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "TiB");
        break;
    case 5:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "PiB");
        break;
    case 6:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "EiB");
        break;
    case 7:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "ZiB");
        break;
    case 8:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "YiB");
        break;
    default:
        ret = FAILURE;
    }
    return ret;
}

bool is_Empty(void *ptrData, size_t lengthBytes)
{
    if (ptrData)
    {
        for (size_t iter = 0, iterEnd = lengthBytes - 1; iter < lengthBytes && iterEnd >= iter; ++iter, --iterEnd)
        {
            if ((C_CAST(uint_fast8_t*, ptrData))[iter] != UINT8_C(0) || (C_CAST(uint_fast8_t*, ptrData))[iterEnd] != UINT8_C(0))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

void convert_Seconds_To_Displayable_Time_Double(double secondsToConvert, uint8_t *years, uint16_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    double tempCalcValue = secondsToConvert;
    //get seconds up to a maximum of 60
    if (seconds)
    {
        *seconds = C_CAST(uint8_t, fmod(tempCalcValue, 60.0));
    }
    tempCalcValue /= 60.0;
    //get minutes up to a maximum of 60
    if (minutes)
    {
        *minutes = C_CAST(uint8_t, fmod(tempCalcValue, 60.0));
    }
    tempCalcValue /= 60.0;
    //get hours up to a maximum of 24
    if (hours)
    {
        *hours = C_CAST(uint8_t, fmod(tempCalcValue, 24.0));
    }
    tempCalcValue /= 24.0;
    //get days up to 365
    if (days)
    {
        *days = C_CAST(uint16_t, fmod(tempCalcValue, 365.0));
    }
    tempCalcValue /= 365.0;
    //get years
    if (years)
    {
        *years = C_CAST(uint8_t, tempCalcValue);
    }
}

void convert_Seconds_To_Displayable_Time(uint64_t secondsToConvert, uint8_t *years, uint16_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    uint64_t tempCalcValue = secondsToConvert;
    //get seconds up to a maximum of 60
    if (seconds)
    {
        *seconds = C_CAST(uint8_t, tempCalcValue % UINT64_C(60));
    }
    tempCalcValue /= UINT64_C(60);
    //get minutes up to a maximum of 60
    if (minutes)
    {
        *minutes = C_CAST(uint8_t, tempCalcValue % UINT64_C(60));
    }
    tempCalcValue /= UINT64_C(60);
    //get hours up to a maximum of 24
    if (hours)
    {
        *hours = C_CAST(uint8_t, tempCalcValue % UINT64_C(24));
    }
    tempCalcValue /= UINT64_C(24);
    //get days up to 365
    if (days)
    {
        *days = C_CAST(uint16_t, tempCalcValue % UINT64_C(365));
    }
    tempCalcValue /= UINT64_C(365);
    //get years
    if (years)
    {
        *years = C_CAST(uint8_t, tempCalcValue);
    }
}

void print_Time_To_Screen(uint8_t *years, uint16_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    if (years && *years > 0)
    {
        printf(" %"PRIu8" year", *years);
        if (*years > 1)
        {
            printf("s");
        }
    }
    if (days && *days > 0)
    {
        printf(" %"PRIu16" day", *days);
        if (*days > 1)
        {
            printf("s");
        }
    }
    if (hours && *hours > 0)
    {
        printf(" %"PRIu8" hour", *hours);
        if (*hours > 1)
        {
            printf("s");
        }
    }
    if (minutes && *minutes > 0)
    {
        printf(" %"PRIu8" minute", *minutes);
        if (*minutes > 1)
        {
            printf("s");
        }
    }
    if (seconds && *seconds > 0)
    {
        printf(" %"PRIu8" second", *seconds);
        if (*seconds > 1)
        {
            printf("s");
        }
    }
    if (years || days || hours || minutes || seconds)
    {
        printf(" ");
    }
}

uint32_t seed32Array[2] = { 0x05EAF00D, 0x05EA51DE };
uint64_t seed64Array[2] = { 0x05EAF00D, 0x05EA51DE };

void seed_32(uint32_t seed)
{
    //first initialize
    seed32Array[0] = seed;
    seed32Array[1] = C_CAST(uint32_t, C_CAST(int32_t, seed) >> 1);//converting to signed int to perform arithmetic shift, then back for the seed value
    //using that initialization, run the random number generator for a more random seed...may or may not be needed, but I'm doing this anyways - Tyler
    seed32Array[0] = xorshiftplus32();
    seed32Array[0] = xorshiftplus32();
}
void seed_64(uint64_t seed)
{
    //first initialize
    seed64Array[0] = seed;
    seed64Array[1] = C_CAST(uint64_t, C_CAST(int64_t, seed) >> 2);//converting to signed int to perform arithmetic shift, then back for the seed value
    //using that initialization, run the random number generator for a more random seed...may or may not be needed, but I'm doing this anyways - Tyler
    seed64Array[0] = xorshiftplus64();
    seed64Array[0] = xorshiftplus64();
}

//shift values chosen randomly
uint32_t xorshiftplus32(void)
{
    uint32_t x = seed32Array[0];
    uint32_t const y = seed32Array[1];
    seed32Array[0] = y;
    x ^= x << 13;//a
    seed32Array[1] = x ^ y ^ (x >> 17) ^ (y >> 7); //b, c
    return (seed32Array[1] + y);
}

//shift values chosen randomly
uint64_t xorshiftplus64(void)
{
    uint64_t x = seed64Array[0];
    uint64_t const y = seed64Array[1];
    seed64Array[0] = y;
    x ^= x << 27;//a
    seed64Array[1] = x ^ y ^ (x >> 13) ^ (y >> 32); //b, c
    return (seed64Array[1] + y);
}

uint32_t random_Range_32(uint32_t rangeMin, uint32_t rangeMax)
{
    //doing this to prevent a possible overflow
    if (rangeMax == UINT32_MAX)
    {
        rangeMax -= 1;
    }
    //this will slightly bias the randomness...but not enough for us to care at this point in time
    //return (xorshiftplus32() % (rangeMax + 1 - rangeMin) + rangeMin);

    //This method below should return unbiased results. see http://c-faq.com/lib/randrange.html
    uint32_t d = (UINT32_MAX / (rangeMax - rangeMin + 1) + 1);
    if (d > 0)
    {
        return (rangeMin + xorshiftplus32() / d);
    }
    else
    {
        return 0;
    }
}
uint64_t random_Range_64(uint64_t rangeMin, uint64_t rangeMax)
{
    //doing this to prevent a possible overflow
    if (rangeMax == UINT64_MAX)
    {
        rangeMax -= 1;
    }
    //this will slightly bias the randomness...but not enough for us to care at this point in time
    //return (xorshiftplus64() % (rangeMax + 1 - rangeMin) + rangeMin);

    //This method below should return unbiased results. see http://c-faq.com/lib/randrange.html
    uint64_t d = (UINT64_MAX / (rangeMax - rangeMin + 1) + 1);
    if (d > 0)
    {
        return (rangeMin + xorshiftplus64() / d);
    }
    else
    {
        return 0;
    }
}

eReturnValues fill_Random_Pattern_In_Buffer(uint8_t *ptrData, uint32_t dataLength)
{
    size_t localPtrDataLen = ((dataLength + sizeof(uint32_t)) - 1) / sizeof(uint32_t);//round up to nearest uint32 amount
    uint32_t* localPtr = C_CAST(uint32_t*, calloc(localPtrDataLen, sizeof(uint32_t)));
    if (!localPtr)
    {
        return MEMORY_FAILURE;
    }
    seed_32(C_CAST(uint32_t, time(NULL)));
    for (uint32_t iter = 0; iter < (dataLength / sizeof(uint32_t)); ++iter)
    {
        localPtr[iter] = xorshiftplus32();
    }
    memcpy(ptrData, localPtr, dataLength);//copy only the length of the original pointer since we may have overallocated and rounded up earlier.
    safe_Free(localPtr)
    return SUCCESS;
}

eReturnValues fill_Hex_Pattern_In_Buffer(uint32_t hexPattern, uint8_t *ptrData, uint32_t dataLength)
{
    size_t localPtrDataLen = ((dataLength + sizeof(uint32_t)) - 1) / sizeof(uint32_t);//round up to nearest uint32 amount
    uint32_t *localPtr = C_CAST(uint32_t*, calloc(localPtrDataLen, sizeof(uint32_t)));
    if (!localPtr)
    {
        return MEMORY_FAILURE;
    }
    for (uint32_t iter = 0; iter < (dataLength / sizeof(uint32_t)); ++iter)
    {
        localPtr[iter] = hexPattern;
    }
    memcpy(ptrData, localPtr, dataLength);//copy only the length of the original pointer since we may have overallocated and rounded up earlier.
    safe_Free(localPtr)
    return SUCCESS;
}

eReturnValues fill_Incrementing_Pattern_In_Buffer(uint8_t incrementStartValue, uint8_t *ptrData, uint32_t dataLength)
{
    if (!ptrData)
    {
        return BAD_PARAMETER;
    }
    for (uint32_t iter = 0; iter < dataLength; ++iter)
    {
        ptrData[iter] = incrementStartValue++;
    }
    return SUCCESS;
}

eReturnValues fill_ASCII_Pattern_In_Buffer(const char *asciiPattern, uint32_t patternLength, uint8_t *ptrData, uint32_t dataLength)
{
    if (!ptrData || patternLength == 0 || !asciiPattern)
    {
        return BAD_PARAMETER;
    }
    for (uint32_t iter = 0; iter < dataLength; iter += patternLength)
    {
        memcpy(&ptrData[iter], asciiPattern, M_Min(patternLength, dataLength - iter));
    }
    return SUCCESS;
}

eReturnValues fill_Pattern_Buffer_Into_Another_Buffer(uint8_t *inPattern, uint32_t inpatternLength, uint8_t *ptrData, uint32_t dataLength)
{
    if (!ptrData || inpatternLength == 0 || !inPattern || dataLength == 0)
    {
        return BAD_PARAMETER;
    }
    for (uint32_t iter = 0; iter < dataLength; iter += inpatternLength)
    {
        memcpy(&ptrData[iter], inPattern, M_Min(inpatternLength, dataLength - iter));
    }
    return SUCCESS;
}

double convert_128bit_to_double(uint8_t * pData)
{
    double result = 0;
    int i;
    for (i = 0; i < 16; i++) {
        result *= 256;
        result += pData[15 - i];
    }
    return result;
}

//helper functions for time.h functions to be safer depending on what is supported on a given system and compiler.
//__STDC_LIB_EXT1__ can be checked for the C11 _s functions. Otherwise, check posix definitions or C standard versions, or possible compiler specific
//definitions depending on what is available.

struct tm * get_UTCtime(const time_t *timer, struct tm *buf)
{
    //There are a few different things we can do in here to be more safe depending on what is supported
    //by the system and compiler, so there are ifdefs to help with that...
    //Microsoft CRT uses different parameter order! May need to detect this if using the _s version in here
    if (timer && buf)
    {
        //TODO: C2x not fully defined yet, but can update this first check when it is and is supported.
#if defined (POSIX_2001) && defined _POSIX_THREAD_SAFE_FUNCTIONS
        //POSIX or C2x (C23 right now) have gmtime_r to use
        return gmtime_r(timer, buf);

#elif defined __STDC_LIB_EXT1__
        //If __STDC_LIB_EXT1__, can use gmtime_s
        //NOTE: This is only available if the __STDC_WANT_LIB_EXT1__ is defined before time.h and system library supports it
        //It is also possible to get through through a library implementation of these _s functions, but one is not currently used today.
        return gmtime_s(timer, buf);

#elif defined _MSC_VER
        //If MSFT CRT available, use microsoft gmtime_s which is incompatible with the standard
        if (0 != gmtime_s(buf, timer))
        {
            return NULL;
        }
#else
        //TODO: There may be better options beyond what is done below, but it may be a per-system implementation or something like that
        //      So there is room for improvement in this fallback function
        //Last thing we can do is use the unsafe version and copy it immediately to our own buffer if nothing else is possible
        memcpy(buf, gmtime(timer), sizeof(struct tm));
#endif
    }
    return buf;
}

struct tm * get_Localtime(const time_t *timer, struct tm *buf)
{
    //There are a few different things we can do in here to be more safe depending on what is supported
    //by the system and compiler, so there are ifdefs to help with that...
    //Microsoft CRT uses different parameter order! May need to detect this if using the _s version in here
    if (timer && buf)
    {
        //TODO: C2x not fully defined yet, but can update this first check when it is and is supported.
#if defined (POSIX_2001) && defined _POSIX_THREAD_SAFE_FUNCTIONS
        //POSIX or C2x (C23 right now) have localtime_r to use
        return localtime_r(timer, buf);

#elif defined __STDC_LIB_EXT1__
        //If __STDC_LIB_EXT1__, can use gmtime_s
        //NOTE: This is only available if the __STDC_WANT_LIB_EXT1__ is defined before time.h and system library supports it
        //It is also possible to get through through a library implementation of these _s functions, but one is not currently used today.
        return localtime_s(timer, buf);

#elif defined _MSC_VER
        //If MSFT CRT available, use microsoft localtime_s which is incompatible with the standard
        if (0 != localtime_s(buf, timer))
        {
            return NULL;
        }
#else
        //TODO: There may be better options beyond what is done below, but it may be a per-system implementation or something like that
        //      So there is room for improvement in this fallback function
        //Last thing we can do is use the unsafe version and copy it immediately to our own buffer if nothing else is possible
        memcpy(buf, localtime(timer), sizeof(struct tm));
#endif
    }
    return buf;
}

char * get_Time_String_From_TM_Structure(const struct tm * timeptr, char *buffer, size_t bufferSize)
{
    if (timeptr && buffer && bufferSize >= TIME_STRING_LENGTH)
    {
        //start with a known zeroed buffer
        memset(buffer, 0, bufferSize);
        //strftime is recommended to be used. Using format %c will return the matching output for this function
        //asctime (which this replaces uses the format: Www Mmm dd hh:mm:ss yyyy\n)
        //NOTE: %e is C99. C89's closest is %d which has a leading zero.
        //To be technically correct in replacing this, need to set locale to english instead of localized locale first.
        //After that is done, it can be reverted back.
        bool restoreLocale = false;
        char *currentLocale = strdup(setlocale(LC_TIME, NULL));
        if (setlocale(LC_TIME, "en-us.utf8"))
        {
            restoreLocale = true;
        }
        #if (defined(USING_C99) && !defined (__MINGW32__))//MINGW in msys2 is warning it does not know %e, so this is catching that. This will likely need a version check whenever it gets fixed-TJE
        if (0 == strftime(buffer, bufferSize, "%a %b %e %H:%M:%S %Y", timeptr))
        #else
        if (0 == strftime(buffer, bufferSize, "%a %b %d %H:%M:%S %Y", timeptr))
        #endif
        {
            //This means the array was too small...which shouldn't happen...but in case it does, zero out the memory
            memset(buffer, 0, bufferSize);
        }
        if (restoreLocale)
        {
            setlocale(LC_TIME, currentLocale);
        }
        safe_Free(currentLocale);
    }
    return buffer;
}


char* get_Current_Time_String(const time_t* timer, char *buffer, size_t bufferSize)
{
    if (timer && buffer && bufferSize >= CURRENT_TIME_STRING_LENGTH)
    {
        struct tm cTimeBuf;
        memset(&cTimeBuf, 0, sizeof(struct tm));
        get_Time_String_From_TM_Structure(get_Localtime(timer, &cTimeBuf), buffer, bufferSize);
    }
    return buffer;
}

time_t get_Future_Date_And_Time(time_t inputTime, uint64_t secondsInTheFuture)
{
    uint16_t days = 0;
    uint8_t years = 0, months = 0, hours = 0, minutes = 0, seconds = 0;
    struct tm futureTime;
    memset(&futureTime, 0, sizeof(struct tm));
    get_Localtime(C_CAST(const time_t*, &inputTime), &futureTime);
    //first break the input time into seperate units
    convert_Seconds_To_Displayable_Time(secondsInTheFuture, &years, &days, &hours, &minutes, &seconds);
    //now start setting the future time struct and checking when incrementing things for overflow
    //start with seconds
    if (seconds > 0)
    {
        if (futureTime.tm_sec + seconds > 59)
        {
            //increment minutes since we will adjust that next
            minutes++;
            futureTime.tm_sec = futureTime.tm_sec + seconds - 60;
        }
        else
        {
            futureTime.tm_sec += seconds;
        }
    }
    //now minutes
    if (minutes > 0)
    {
        if (futureTime.tm_min + minutes > 59)
        {
            hours++;
            futureTime.tm_min = futureTime.tm_min + minutes - 60;
        }
        else
        {
            futureTime.tm_min += minutes;
        }
    }
    //now hours
    if (hours > 0)
    {
        if (futureTime.tm_hour + hours > 23)
        {
            days++;
            futureTime.tm_hour = futureTime.tm_hour + hours - 24;
        }
        else
        {
            futureTime.tm_hour += hours;
        }
    }
    //now days (including week day information)
    if (days > 0)
    {
        //day of the year
        if (futureTime.tm_yday + days > 365)
        {
            years++;
            futureTime.tm_yday = futureTime.tm_yday + days - 366;
        }
        else
        {
            futureTime.tm_yday += days;
        }
        //day of the week
        if (futureTime.tm_wday + days > 6)
        {
            futureTime.tm_wday = futureTime.tm_wday + days - 7;
        }
        else
        {
            futureTime.tm_wday += days;
        }
        //day of the month (depends on the month!)
        uint8_t numberOfDaysInMonth = 31;//assume the most common
        switch (futureTime.tm_mon)
        {
        case 0://january
        case 2://march
        case 4://may
        case 6://july
        case 7://august
        case 9://october
        case 11://december
            //31 days a month
            numberOfDaysInMonth = 31;
            break;
        case 3://april
        case 5://june
        case 8://september
        case 10://november
            //30 days a month
            numberOfDaysInMonth = 30;
            break;
        case 1://february
            //28 days a month (do we need to handle leap year?)
            if (futureTime.tm_year % 400 == 0)
            {
                //leap year
                numberOfDaysInMonth = 29;
            }
            else if (futureTime.tm_year % 100 == 0)
            {
                //not a leap year
                numberOfDaysInMonth = 28;
            }
            else if (futureTime.tm_year % 4 == 0)
            {
                //leap year
                numberOfDaysInMonth = 29;
            }
            else
            {
                //not a leap year
                numberOfDaysInMonth = 28;
            }
            break;
        default://do nothing
            break;
        }
        if (futureTime.tm_mday + days > numberOfDaysInMonth)
        {
            months++;
            futureTime.tm_mday = futureTime.tm_mday + days - (numberOfDaysInMonth + 1);
        }
        else
        {
            futureTime.tm_mday += days;
        }
    }
    //now months
    if (months > 0)
    {
        if (futureTime.tm_mon + months > 11)
        {
            years++;
            futureTime.tm_mon = futureTime.tm_mon + months - 12;
        }
        else
        {
            futureTime.tm_mon += months;
        }
    }
    //now years
    if (years > 0)
    {
        //since 1900...just increment since no range applies today
        futureTime.tm_year += years;
    }
    return mktime(&futureTime);
}

eReturnValues get_Compiler_Info(eCompiler *compilerUsed, ptrCompilerVersion compilerVersionInfo)
{
    eReturnValues ret = SUCCESS;//should always return this unless we don't find a compiler we support.
    //make sure we set unknown and clear out version info before we continue.
    *compilerUsed = OPENSEA_COMPILER_UNKNOWN;
    memset(compilerVersionInfo, 0, sizeof(compilerVersion));
    //Time to start checking predefined compiler macros
#if defined _MSC_VER
    //Microsoft Visual C/C++ compiler (code is written to use the _MSC_FULL_VER from 2003 and later and we only really support 2013 and higher due to C99 usage)-TJE
    *compilerUsed = OPENSEA_COMPILER_MICROSOFT_VISUAL_C_CPP;
#define MS_VERSION_STRING_LENGTH 10
    char msVersion[MS_VERSION_STRING_LENGTH] = { 0 };
    snprintf(msVersion, MS_VERSION_STRING_LENGTH, "%u", _MSC_FULL_VER);
    char msMajor[3] = { 0 };
    char msMinor[3] = { 0 };
    char msPatch[6] = { 0 };
    snprintf(msMajor, 3, "%.2s", &msVersion[0]);
    snprintf(msMinor, 3, "%.2s", &msVersion[2]);
    snprintf(msPatch, 6, "%.5s", &msVersion[4]);
    compilerVersionInfo->major = C_CAST(uint16_t, atoi(msMajor));
    compilerVersionInfo->minor = C_CAST(uint16_t, atoi(msMinor));
    compilerVersionInfo->patch = C_CAST(uint16_t, atoi(msPatch));
#elif defined __MINGW64__
    *compilerUsed = OPENSEA_COMPILER_MINGW;
    compilerVersionInfo->major = __MINGW64_VERSION_MAJOR;
    compilerVersionInfo->minor = __MINGW64_VERSION_MINOR;
    compilerVersionInfo->patch = 0;
#elif defined __MINGW32__
    *compilerUsed = OPENSEA_COMPILER_MINGW;
    compilerVersionInfo->major = __MINGW32_MAJOR_VERSION;
    compilerVersionInfo->minor = __MINGW32_MINOR_VERSION;
    compilerVersionInfo->patch = 0;
#elif defined __clang__
    *compilerUsed = OPENSEA_COMPILER_CLANG;
    compilerVersionInfo->major = __clang_major__;
    compilerVersionInfo->minor = __clang_minor__;
    compilerVersionInfo->patch = __clang_patchlevel__;
#elif defined __HP_aCC
    //untested
    *compilerUsed = OPENSEA_COMPILER_HP_A_CPP;
#define HP_ACC_VERSION_STRING_LENGTH 7
    char hpVersion[HP_ACC_VERSION_STRING_LENGTH] = { 0 };
    snprintf(hpVersion, HP_ACC_VERSION_STRING_LENGTH, "%u", __HP_aCC);
    char hpMajor[3] = { 0 };
    char hpMinor[3] = { 0 };
    char hpPatch[3] = { 0 };
    snprintf(hpMajor, 3, "%.2s", &hpVersion[0]);
    snprintf(hpMinor, 3, "%.2s", &hpVersion[2]);
    snprintf(hpPatch, 3, "%.2s", &hpVersion[4]);
    compilerVersionInfo->major = C_CAST(uint16_t, atoi(hpMajor));
    compilerVersionInfo->minor = C_CAST(uint16_t, atoi(hpMinor));
    compilerVersionInfo->patch = C_CAST(uint16_t, atoi(hpPatch));
#elif defined __IBMC__ || defined __IBMCPP__
    //untested
    //detect if it's xl or lx for system z
    #if defined __COMPILER_VER__
    //system z
    compilerVersionInfo->major = M_Nibble6(__COMPILER_VER__);
    compilerVersionInfo->minor = M_Byte2(__COMPILER_VER__);
    compilerVersionInfo->patch = M_Word0(__COMPILER_VER__);
    #else
    //standard xl
    compilerVersionInfo->major = M_Byte1(__xlC__);
    compilerVersionInfo->minor = M_Byte0(__xlC__);
    compilerVersionInfo->patch = M_Byte1(__xlC_ver__);
    #endif
#elif defined __INTEL_COMPILER
    //untested
    *compilerUsed = OPENSEA_COMPILER_INTEL_C_CPP;
#define INTEL_VERSION_STRING_MAX_LENGTH 4;
    char intelVersion[4] = { 0 };
    snprintf(intelVersion, INTEL_VERSION_STRING_MAX_LENGTH, "%u", __INTEL_COMPILER);
    char intelMajor[2] = { 0 };
    char intelMinor[2] = { 0 };
    char intelPatch[2] = { 0 };
    snprintf(intelMajor, 2, "%.1s", &intelVersion[0]);
    snprintf(intelMinor, 2, "%.1s", &intelVersion[1]);
    snprintf(intelPatch, 2, "%.1s", &intelVersion[2]);
    compilerVersionInfo->major = C_CAST(uint16_t, atoi(intelMajor));
    compilerVersionInfo->minor = C_CAST(uint16_t, atoi(intelMinor));
    compilerVersionInfo->patch = C_CAST(uint16_t, atoi(intelPatch));
#elif defined __SUNPRO_C || defined __SUNPRO_CC
    //untested
    //code below is written for versions 5.10 and later. (latest release as of writing this code is version 5.12) - TJE
    *compilerUsed = OPENSEA_COMPILER_SUNPRO_C_CPP;
    compilerVersionInfo->major = M_Nibble3(__SUNPRO_C);
    compilerVersionInfo->minor = (M_Nibble2(__SUNPRO_C) << 4) | M_Nibble1(__SUNPRO_C);//I think this is wrong. Version 5.10 is represented as 0x5100. Need to put extra code in here to fix this when we actually support this compiler.
    compilerVersionInfo->patch = M_Nibble0(__SUNPRO_C);
#elif defined __GNUC__ //CYGWIN?
    //GCC C/C++ compiler
    *compilerUsed = OPENSEA_COMPILER_GCC;
    compilerVersionInfo->major = __GNUC__;
    compilerVersionInfo->minor = __GNUC_MINOR__;
    compilerVersionInfo->patch = __GNUC_PATCHLEVEL__;//introduced in GCC 3.0, but even that is super old so this shouldn't cause any problems.-TJE
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
    case OPENSEA_COMPILER_RESERVED:
    //default:
        printf("Unknown Compiler");
        break;
    }
}

void print_Compiler_Version_Info(ptrCompilerVersion compilerVersionInfo)
{
    printf("%"PRIu16".%"PRIu16".%"PRIu16"", compilerVersionInfo->major, compilerVersionInfo->minor, compilerVersionInfo->patch);
}

long int get_File_Size(FILE *filePtr)
{
    long int fileSize = 0;
    long int initialPos = ftell(filePtr);
    rewind(filePtr);
    fseek(filePtr, 0, SEEK_END);
    fileSize = ftell(filePtr);
    rewind(filePtr);
    fseek(filePtr, initialPos, SEEK_SET);
    return fileSize;
}

bool get_And_Validate_Integer_Input(const char * strToConvert, uint64_t * outputInteger)
{
    bool ret = true;
    bool hex = false;
    bool isZero = true;
    const char * tmp = strToConvert;
    while (*tmp != '\0')
    {
        //if non zero char, then isZero=false
        if (isZero && *tmp != '0' && (*tmp != 'x') && (*tmp != 'h'))
            isZero = false;

        if ((!isxdigit(*tmp)) && (*tmp != 'x') && (*tmp != 'h'))
        {
            ret = false;
            break;
        }
        else if (!isdigit(*tmp))
        {
            hex = true;
        }
        tmp++;
    }
    //If everything is a valid hex digit.    
    if (ret)
    {
        if (hex)
        {
            *outputInteger = C_CAST(uint64_t, strtoull(strToConvert, NULL, 16));
        }
        else
        {
            *outputInteger = C_CAST(uint64_t, strtoull(strToConvert, NULL, 10));
        }
    }
    else
    {
        ret = false;
    }

    //Final Check
    if (ret && ((*outputInteger == ULLONG_MAX && errno == ERANGE) || (*outputInteger == 0 && !isZero)))
    {
        ret = false;
    }

    return ret;
}

void print_Errno_To_Screen(int error)
{
#if defined (USING_C11) && defined (__STDC_LIB_EXT1__)//This last piece must be defined by the compiler for this to be used...Only will be set if user defined __STDC_WANT_LIB_EXT1__
    size_t errorStringLen = strerrorlen_s(error);
    char *errorString = C_CAST(char*, calloc(errorStringLen + 1, sizeof(char)));
    if (errorString)
    {
        strerror(errorString, errorStringLen + 1, error);//returns 0 if whole string fits, nonzero if it was truncatd
        printf("%d - %s\n", error, errorString);
    }
    else
    {
        printf("%d - <Unable to convert error to string>\n", error);
    }
    safe_Free(errorString)
#elif defined (__STDC_SECURE_LIB__) //This is a MSFT definition for their original _s functions that sometimes differ from the C11 standard
    char errorString[1024] = { 0 };
    if (0 == strerror_s(errorString, 1024, error))
    {
        printf("%d - %s\n", error, errorString);
    }
    else
    {
        printf("%d - <Unable to convert error to string>\n", error);
    }
#else
    printf("%d - %s\n", error, strerror(error));
#endif
}

uint64_t power_Of_Two(uint16_t exponent)
{
    uint64_t result = UINT64_C(1);
    switch (exponent)
    {
    case 0:
        break;
    case 1:
        result = UINT64_C(2);
        break;
    case 2:
        result = UINT64_C(4);
        break;
    case 3:
        result = UINT64_C(8);
        break;
    case 4:
        result = UINT64_C(16);
        break;
    case 5:
        result = UINT64_C(32);
        break;
    case 6:
        result = UINT64_C(64);
        break;
    case 7:
        result = UINT64_C(128);
        break;
    case 8:
        result = UINT64_C(256);
        break;
    case 9:
        result = UINT64_C(512);
        break;
    case 10:
        result = UINT64_C(1024);
        break;
    case 11:
        result = UINT64_C(2048);
        break;
    case 12:
        result = UINT64_C(4096);
        break;
    case 13:
        result = UINT64_C(8192);
        break;
    case 14:
        result = UINT64_C(16384);
        break;
    case 15:
        result = UINT64_C(32768);
        break;
    case 16:
        result = UINT64_C(65536);
        break;
    case 17:
        result = UINT64_C(131072);
        break;
    case 20:
        result = UINT64_C(262144);
        break;
    case 21:
        result = UINT64_C(524288);
        break;
    case 22:
        result = UINT64_C(1048576);
        break;
    case 23:
        result = UINT64_C(2097152);
        break;
    case 24:
        result = UINT64_C(4194304);
        break;
    case 25:
        result = UINT64_C(8388608);
        break;
    case 26:
        result = UINT64_C(16777216);
        break;
    case 27:
        result = UINT64_C(33554432);
        break;
    case 28:
        result = UINT64_C(67108864);
        break;
    case 29:
        result = UINT64_C(134217728);
        break;
    case 30:
        result = UINT64_C(268435456);
        break;
    default:
        result = UINT64_C(2) << (exponent - 1);
        //{
        //    //Since this case is 31 or higher, we can start with the value above to reduce how long this loop runs
        //    uint32_t shiftCounter = UINT64_C(30);//uint32 in case we do hit something huge in this loop and roll all the way around a uint16
        //    result = UINT64_C(268435456);
        //    while (shiftCounter < exponent)
        //    {
        //        result = result << UINT64_C(1); //multiply by 2
        //        ++shiftCounter;
        //    }
        //}
        break;
    }
    return result;
}

double raise_to_power(double number, double power)
{
    double result = 1.0;
    int64_t localPower = C_CAST(int64_t, power);
    if (localPower == INT64_C(0))
    {
        return 1.0;
    }
    if (localPower == INT64_C(1))
    {
        return number;
    }
    for (int64_t i = INT64_C(-1); i >= localPower && localPower != INT64_C(0); i--)
    {
        result = result * (1.0 / number);
    }
    return result;
}

//making it look similar to a std lib function like isPrint.
int is_ASCII(int c)
{
    if (c >= 0x7F || c < 0)
    {
        return 0;//false
    }
    else
    {
        return 1;//true
    }
}

void get_Decimal_From_4_byte_Float(uint32_t floatValue, double *decimalValue)
{
    int32_t  exponent = C_CAST(int32_t, M_GETBITRANGE(floatValue, 30, 23) - 127);
    double sign = pow(-1.0, C_CAST(double, M_GETBITRANGE(floatValue, 31, 31)));

    int8_t power = -23;
    double mantisa = 1.0;
    for (uint8_t i = 0; i < 23; i++)
    {
        mantisa += C_CAST(double, M_GETBITRANGE(floatValue, i, i)) * pow(2.0, power);
        power++;
    }

    *decimalValue = sign * pow(2.0, exponent) * mantisa;
}

//these string concatenation functions currently use snprintf for portability.
//We can look into using strlcat (BSD, glibc 2.38+) or strcpy_s (Annex_k, MSVC) as well to improve performance
// when these other calls are available.-TJE
//https://sourceware.org/glibc/wiki/FAQ#Why_no_strlcpy_.2F_strlcat.3F
// Using memccpy as it is available nearly everywhere and is significantly faster
//https://developers.redhat.com/blog/2019/08/12/efficient-string-copying-and-concatenation-in-c#
char* common_String_Concat(char* destination, size_t destinationSizeBytes, const char* source)
{
    if (destination && source && destinationSizeBytes > 0)
    {
#if defined (POSIX_2001) || defined (_MSC_VER) || defined (__MINGW32__) || defined (USING_C23)
        char* dup = strdup(destination);
        if (dup)
        {
            char* p = C_CAST(char*, memccpy(destination, dup, '\0', destinationSizeBytes));
            size_t destinationSizeBytesAvailable = destinationSizeBytes - (C_CAST(uintptr_t, p) - C_CAST(uintptr_t, destination) - int_to_sizet(1));
            if (NULL == memccpy(p - 1, source, '\0', destinationSizeBytesAvailable))
            {
                //add null terminator to the destination, overwriting last byte written to stay in bounds -TJE
                destination[destinationSizeBytes - int_to_sizet(1)] = '\0';
            }
            safe_Free(dup)
            return destination;
        }
#elif (defined (__FreeBSD__) && __FreeBSD__ >= 4) || (defined (__OpenBSD__) && defined(OpenBSD2_4)) || (defined (__NetBSD__) && defined (__NetBSD_Version__) && __NetBSD_Version >= 1040000300L)
        //use strlcat
        //FreeBSD 3.3 and later
        //openBSD 2.4 and later
        //netbsd  1.4.3
        strlcat(destination, source, destinationSizeBytes);
        return destination;
#else //memccpy, strlcat/strcpy not available
        size_t duplen = strlen(destination);
        char* dup = C_CAST(char*, calloc(duplen + 1, sizeof(char)));
        if (dup)
        {
            memcpy(dup, destination, duplen + 1);
            snprintf(destination, destinationSizeBytes, "%s%s", dup, source);
            safe_Free(dup)
            return destination;
        }
#endif   
    }
    return NULL;
}

char* common_String_Concat_Len(char* destination, size_t destinationSizeBytes, const char* source, int sourceLength)
{
    if (destination && source && destinationSizeBytes > 0 && sourceLength > 0)
    {
#if defined (POSIX_2001) || defined (_MSC_VER) || defined (__MINGW32__) || defined (USING_C23)
        char* dup = strdup(destination);
        if (dup)
        {
            char* p = C_CAST(char*, memccpy(destination, dup, '\0', destinationSizeBytes));
            size_t destinationSizeBytesAvailable = destinationSizeBytes - (C_CAST(uintptr_t, p) - C_CAST(uintptr_t, destination) - int_to_sizet(1));
            if (NULL == memccpy(p - 1, source, '\0', M_Min(destinationSizeBytesAvailable, int_to_sizet(sourceLength))))
            {
                if (int_to_sizet(sourceLength) >= destinationSizeBytesAvailable)
                {
                    //add null terminator to the destination, overwriting last byte written to stay in bounds -TJE
                    destination[destinationSizeBytes - int_to_sizet(1)] = '\0';
                }
                else
                {
                    //append a null terminator after the last written byte of the string - TJE
                    destination[destinationSizeBytes - destinationSizeBytesAvailable + int_to_sizet(sourceLength)] = '\0';
                }
            }
            safe_Free(dup)
            return destination;
        }
#elif (defined (__FreeBSD__) && __FreeBSD__ >= 4) || (defined (__OpenBSD__) && defined(OpenBSD2_4)) || (defined (__NetBSD__) && defined (__NetBSD_Version__) && __NetBSD_Version >= 1040000300L)
        //use strlcat
        //FreeBSD 3.3 and later
        //openBSD 2.4 and later
        //netbsd  1.4.3
        char* dup = C_CAST(char*, calloc(sourceLength + 1, sizeof(char)));
        if (dup)
        {
            strlcpy(dup, source, sourceLength);
            strlcat(destination, dup, destinationSizeBytes);
            safe_Free(dup)
            return destination;
        }
#else //memccpy, strlcat/strcpy not available
        size_t duplen = strlen(destination);
        char* dup = C_CAST(char*, calloc(duplen + 1, sizeof(char)));
        if (dup)
        {
            memcpy(dup, destination, duplen + 1);
            snprintf(destination, destinationSizeBytes, "%s%.*s", dup, sourceLength, source);
            safe_Free(dup)
            return destination;
        }
#endif
    }
    return NULL;
}

void* explicit_zeroes(void* dest, size_t count)
{
    if (dest && count > 0)
    {
#if defined (USING_C23) || defined (HAVE_MEMSET_EXPLICIT)
        return memset_explicit(dest, 0, count);
#elif defined (__STDC_LIB_EXT1__) || defined (HAVE_MEMSET_S)
        //use memset_s since it cannot be optimized out
        if (0 == memset_s(dest, count, 0, count))
        {
            return dest;
        }
        else
        {
            return NULL;
        }
#elif defined (_WIN32) && defined (_MSC_VER)
        //use microsoft's SecureZeroMemory function
        return SecureZeroMemory(dest, count);
#elif (defined (__FreeBSD__) && __FreeBSD__ >= 11) || (defined (__OpenBSD__) && defined(OpenBSD5_5)) || (defined (__GLIBC__) && defined (__GLIBC_MINOR__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 25) || defined (HAVE_EXPLICIT_BZERO)
        //TODO: MUSL seems to support this too, so need to figure out how to detect it there.
        //https://elixir.bootlin.com/musl/latest/source/src/string/explicit_bzero.c <- seems to appear first in 1.1.20
        //https://man.freebsd.org/cgi/man.cgi?query=explicit_bzero
        //https://www.gnu.org/software/gnulib/manual/html_node/explicit_005fbzero.html
        //https://man.dragonflybsd.org/?command=explicit_bzero&section=3
        //illumos has this too, but differs from oracle's solaris which has explicit_memset
        //https://illumos.org/man/3C/explicit_bzero
        explicit_bzero(dest, count);
        return dest;
#elif (defined (__NetBSD__) && defined (__NetBSD_Version__) && __NetBSD_Version >= 7000000000L /* net bsd version 7.0 and up*/) || defined (HAVE_EXPLICIT_MEMSET)
        //https://man.netbsd.org/NetBSD-8.0/explicit_memset.3
        //https://docs.oracle.com/cd/E88353_01/html/E37843/explicit-memset-3c.html
        //TODO: Solaris 11.4.12 added this, but I cannot find it in illumos based distributions
        return explicit_memset(dest, 0, count);
#else
        //one idea on the web is this ugly volatile function pointer to memset to stop the compiler optimization
        //so doing this so I don't need to make more per-compiler ifdefs for other solutions
        void* (*const volatile no_optimize_memset) (void*, int, size_t) = memset;
        return no_optimize_memset(dest, 0, count);
#endif
    }
    else
    {
        return NULL;
    }
}

#if !defined (SIZE_MAX)
#error "SIZE_MAX not defined! Please add a definition for this system!"
#endif

size_t int8_to_sizet(int8_t val)
{
    if (val < 0)
    {
        return 0;
    }
    #if defined(SIZE_MAX) && defined (INT8_MAX) && SIZE_MAX >= INT8_MAX
    else
    {
        return C_CAST(size_t, val);
    }
    #else //SIZE_MAX < INT8_MAX
    else if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t uint8_to_sizet(uint8_t val)
{
    #if defined(SIZE_MAX) && defined (UINT8_MAX) && SIZE_MAX >= UINT8_MAX
        return C_CAST(size_t, val);
    #else //SIZE_MAX < UINT8_MAX
        if (val <= SIZE_MAX)
        {
            return val;
        }
        else
        {
            return SIZE_MAX;
        }
    #endif
}

size_t int16_to_sizet(int16_t val)
{
    if (val < 0)
    {
        return 0;
    }
    #if defined(SIZE_MAX) && defined (INT16_MAX) && SIZE_MAX >= INT16_MAX
    else
    {
        return C_CAST(size_t, val);
    }
    #else //SIZE_MAX < INT8_MAX
    else if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t uint16_to_sizet(uint16_t val)
{
    #if defined(SIZE_MAX) && defined (UINT16_MAX) && SIZE_MAX >= UINT16_MAX
        return C_CAST(size_t, val);
    #else //SIZE_MAX < UINT16_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t int32_to_sizet(int32_t val)
{
    if (val < 0)
    {
        return 0;
    }
    #if defined(SIZE_MAX) && defined (INT32_MAX) && SIZE_MAX >= INT32_MAX
    else
    {
        return C_CAST(size_t, val);
    }
    #else //SIZE_MAX < INT32_MAX
    else if (C_CAST(uint32_t, val) <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t uint32_to_sizet(uint32_t val)
{
    #if defined(SIZE_MAX) && defined (UINT32_MAX) && SIZE_MAX >= UINT32_MAX
        return C_CAST(size_t, val);
    #else //SIZE_MAX < UINT32_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t int64_to_sizet(int64_t val)
{
    if (val < 0)
    {
        return 0;
    }
    #if defined(SIZE_MAX) && defined (INT64_MAX) && SIZE_MAX >= INT64_MAX
    else
    {
        return C_CAST(size_t, val);
    }
    #else //SIZE_MAX < INT64_MAX
    else if (C_CAST(uint64_t, val) <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t uint64_to_sizet(uint64_t val)
{
    #if defined(SIZE_MAX) && defined (UINT64_MAX) && SIZE_MAX >= UINT64_MAX
        return C_CAST(size_t, val);
    #else //SIZE_MAX < UINT64_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t char_to_sizet(char val)
{
    #if defined (CHAR_MIN) && CHAR_MIN < 0
    if (val < 0)
    {
        return 0;
    }
    #endif //CHAR_MIN < 0
    #if defined(SIZE_MAX) && defined (CHAR_MAX) && SIZE_MAX >= CHAR_MAX
    {
        return C_CAST(size_t, val);
    }
    #else //SIZE_MAX < CHAR_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t uchar_to_sizet(unsigned char val)
{
    #if defined(SIZE_MAX) && defined (UCHAR_MAX) && SIZE_MAX >= UCHAR_MAX
        return C_CAST(size_t, val);
    #else //SIZE_MAX < UCHAR_MAX
        if (val <= SIZE_MAX)
        {
            return val;
        }
        else
        {
            return SIZE_MAX;
        }
    #endif
}

size_t short_to_sizet(short val)
{
    if (val < 0)
    {
        return 0;
    }
    #if defined(SIZE_MAX) && defined (SHRT_MAX) && SIZE_MAX >= SHRT_MAX
    else
    {
        return C_CAST(size_t, val);
    }
    #else //SIZE_MAX < SHRT_MAX
    else if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t ushort_to_sizet(unsigned short val)
{
    #if defined(SIZE_MAX) && defined (USHRT_MAX) && SIZE_MAX >= USHRT_MAX
        return C_CAST(size_t, val);
    #else //SIZE_MAX < USHRT_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t int_to_sizet(int val)
{
    if (val < 0)
    {
        return 0;
    }
    #if defined(SIZE_MAX) && defined (INT_MAX) && SIZE_MAX >= INT_MAX
    else
    {
        return C_CAST(size_t, val);
    }
    #else //SIZE_MAX < INT_MAX
    else if (C_CAST(unsigned int, val) <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t uint_to_sizet(unsigned int val)
{
    #if defined(SIZE_MAX) && defined (UINT_MAX) && SIZE_MAX >= UINT_MAX
        return C_CAST(size_t, val);
    #else //SIZE_MAX < UINT_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t long_to_sizet(long val)
{
    if (val < 0)
    {
        return 0;
    }
    #if defined(SIZE_MAX) && defined (LONG_MAX) && SIZE_MAX >= LONG_MAX
    else
    {
        return C_CAST(size_t, val);
    }
    #else //SIZE_MAX < LONG_MAX
    else if (C_CAST(unsigned long, val) <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t ulong_to_sizet(unsigned long val)
{
    #if defined(SIZE_MAX) && defined (ULONG_MAX) && SIZE_MAX >= ULONG_MAX
        return C_CAST(size_t, val);
    #else //SIZE_MAX < ULONG_MAX
    if (val <= SIZE_MAX)
    {
        return val;
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t longlong_to_sizet(long long val)
{
    if (val < 0)
    {
        return 0;
    }
    #if defined(SIZE_MAX) && defined (LLONG_MAX) && SIZE_MAX >= LLONG_MAX
    else
    {
        return C_CAST(size_t, val);
    }
    #else //SIZE_MAX < LLONG_MAX
    else if (C_CAST(unsigned long long, val) <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}

size_t ulonglong_to_sizet(unsigned long long val)
{
    #if defined(SIZE_MAX) && defined (ULLONG_MAX) && SIZE_MAX >= ULLONG_MAX
        return C_CAST(size_t, val);
    #else //SIZE_MAX < ULLONG_MAX
    if (val <= SIZE_MAX)
    {
        return C_CAST(size_t, val);
    }
    else
    {
        return SIZE_MAX;
    }
    #endif
}


bool is_size_t_max(size_t val)
{
    if (val == SIZE_MAX)
    {
        return true;
    }
    else
    {
        return false;
    }
}

#if !defined (__STDC_ALLOC_LIB__) && !defined _POSIX_VERSION || (!defined (POSIX_2008))
//getdelim and getline are not available, so define them ourselves for our own use

ssize_t getdelim(char** M_RESTRICT lineptr, size_t* M_RESTRICT n, int delimiter, FILE* stream)
{
    char* currentptr = NULL;
    char* endptr = NULL;
    if (lineptr == NULL || n == NULL || stream == NULL)
    {
        errno = EINVAL;
        return -1;
    }
    if (*lineptr == NULL || *n == 0)
    {
        *n = BUFSIZ;
        *lineptr = C_CAST(char*, malloc(*n));
        if (NULL == *lineptr)
        {
            errno = ENOMEM;
            return -1;
        }
    }
    currentptr = *lineptr;
    endptr = *lineptr + *n;
    //read using fgetc until delimiter is encountered in the stream or end of the stream is reached
    do
    {
        int c = fgetc(stream);
        if (c == EOF)
        {
            //hit end of the stream.
            if (feof(stream))
            {
                ssize_t numchars = C_CAST(ssize_t, C_CAST(intptr_t, currentptr) - C_CAST(intptr_t, *lineptr));
                if (numchars > 0)
                {
                    //read all the characters in the stream to the end.
                    //set NULL terminator and return how many chars were written
                    currentptr += 1;
                    *currentptr = '\0';
                    return numchars;
                }
            }
            //errno value???
            return -1;
        }
        //add to the stream
        *currentptr = C_CAST(char, c);//This cast is necessary to stop a warning. C is an int so that EOF can be defined.
        currentptr += 1;
        //check if we got the delimiter
        if (c == delimiter)
        {
            *currentptr = '\0';
            //calculate how may characters were read and return that value
            return C_CAST(ssize_t, C_CAST(intptr_t, currentptr) - C_CAST(intptr_t, *lineptr));
        }
        //check if writing next two characters will overflow the buffer (next char + null terminator if needed)
        if (C_CAST(intptr_t, currentptr) + 2 >= C_CAST(intptr_t, endptr))
        {
            //reallocate. Simple method is to double the current buffer size
            char* temp = NULL;
            size_t newsize = *n * 2;
            ssize_t numchars = C_CAST(ssize_t, C_CAST(intptr_t, currentptr) - C_CAST(intptr_t, *lineptr));
#if defined (SSIZE_MAX)
            if (newsize > SSIZE_MAX)
            {
                errno = EOVERFLOW;
                return -1;
            }
#endif //SSIZE_MAX
            temp = realloc(*lineptr, newsize);
            if (temp == NULL)
            {
                errno = ENOMEM;
                return -1;
            }
            *lineptr = temp;
            *n = newsize;
            currentptr = *lineptr + numchars;
            endptr = *lineptr + *n;
        }
    } while (1);
}

ssize_t getline(char** lineptr, size_t* n, FILE* stream)
{
    return getdelim(lineptr, n, '\n', stream);
}

#endif //__STDC_ALLOC_LIB__
