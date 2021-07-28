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
// \file common.c
// \brief Implements functions that are common to OS & Non-OS code.
//
#include "common.h"
#include <ctype.h>

#if defined (_WIN32)
#include <windows.h> //used for setting color output to the command prompt and Sleep()
#else
#include <unistd.h> //needed for usleep() or nanosleep()
#include <time.h>
#include <errno.h>
#endif
#include <stdlib.h>//aligned allocation functions come from here
#include <math.h>

void delay_Milliseconds(uint32_t milliseconds)
{
#if defined(_WIN32) && !defined (UEFI_C_SOURCE)
    Sleep(milliseconds);
#else
    //according to this link: http://linux.die.net/man/3/usleep
    //usleep is obsolete starting in POSIX 2001 and removed entirely in POSIX 2008 and nanosleep is supposed to be used instead.
    //the usleep code is left in just in case it is needed for any reason, but nanosleep works as expected
    #if defined _POSIX_VERSION && _POSIX_VERSION >= 199309L && defined _POSIX_TIMERS 
        struct timespec delayTime;
        delayTime.tv_sec = milliseconds / 1000;
        delayTime.tv_nsec = 1000000 * (milliseconds % 1000);
        nanosleep(&delayTime, NULL);
    #else
        usleep((useconds_t)(milliseconds * 1000));
    #endif
#endif
}

void delay_Seconds(uint32_t seconds)
{
    delay_Milliseconds(1000 * seconds);
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
        //temp = malloc(size);

        if (0 != posix_memalign( &temp, alignment, size))
        {
            temp = NULL;//make sure the system we are running didn't change this.
        }

        return temp;
    #else

    #if !defined(UEFI_C_SOURCE) && defined (__STDC__) && defined (__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        //C11 added an aligned alloc function we can use
        //One additional requirement of this function is that the size must be a multiple of alignment, so we will check and round up the size to make this easy.
        if (size % alignment)
        {
            size = size + alignment - (size % alignment);
        }
        return aligned_alloc(alignment, size);
    #elif !defined(UEFI_C_SOURCE) && defined (__INTEL_COMPILER) || defined (__ICC)
        //_mm_malloc
        return _mm_malloc((int)size, (int)alignment);
    #elif !defined(UEFI_C_SOURCE) && defined (_POSIX_VERSION) && _POSIX_VERSION >= 200112L
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
                temp += alignment - (((size_t)temp) % alignment);
                //aligned.
                //now save the original pointer location
                size_t *savedLocationData = (size_t*)(temp - sizeof(size_t));
                *savedLocationData = (size_t)originalLocation;
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
    #if !defined(UEFI_C_SOURCE) && defined (__STDC__) && defined (__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
        //just call free
        free(ptr);
    #elif !defined(UEFI_C_SOURCE) && defined (__INTEL_COMPILER) || defined (__ICC)
        //_mm_free
        _mm_free(ptr);
    #elif !defined(UEFI_C_SOURCE) && defined (_POSIX_VERSION) && _POSIX_VERSION >= 200112L
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
            tempPtr = (void*)(*((size_t*)tempPtr));
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
    if (originalSize > 0)//if this is zero, they don't want or care to keep the data
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
    #elif defined (_POSIX_VERSION) && _POSIX_VERSION >= 200112L
        //use sysconf: http://man7.org/linux/man-pages/man3/sysconf.3.html
        return (size_t)sysconf(_SC_PAGESIZE);
    #elif defined (_POSIX_VERSION) //this may not be the best way to test this, but I think it will be ok.
        //use get page size: http://man7.org/linux/man-pages/man2/getpagesize.2.html
        return (size_t)getpagesize();
    #elif defined (_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
        SYSTEM_INFO system;
        memset(&system, 0, sizeof(SYSTEM_INFO));
        GetSystemInfo(&system);
        return (size_t)system.dwPageSize;
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
    *byteToSwap = ((*byteToSwap & 0x0F) << 4) | ((*byteToSwap & 0xF0) >> 4);
}

void byte_Swap_16(uint16_t *wordToSwap)
{
    *wordToSwap = ((*wordToSwap & 0x00FF) << 8) | ((*wordToSwap & 0xFF00) >> 8);
}

void byte_Swap_Int16(int16_t *signedWordToSwap)
{
    *signedWordToSwap = ((*signedWordToSwap & 0x00FF) << 8) | ((*signedWordToSwap & 0xFF00) >> 8);
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
    *doubleWordToSwap = ((*doubleWordToSwap & 0x0000FFFF) << 16) | ((*doubleWordToSwap & 0xFFFF0000) >> 16);
    *doubleWordToSwap = ((*doubleWordToSwap & 0x00FF00FF) << 8) | ((*doubleWordToSwap & 0xFF00FF00) >> 8);
}

void byte_Swap_Int32(int32_t *signedDWord)
{
    *signedDWord = ((*signedDWord & 0x0000FFFF) << 16) | ((*signedDWord & 0xFFFF0000) >> 16);
    *signedDWord = ((*signedDWord & 0x00FF00FF) << 8) | ((*signedDWord & 0xFF00FF00) >> 8);
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
    *doubleWordToSwap = ((*doubleWordToSwap & 0x0000FFFF) << 16) | ((*doubleWordToSwap & 0xFFFF0000) >> 16);
}

void byte_Swap_64(uint64_t *quadWordToSwap)
{
    *quadWordToSwap = ((*quadWordToSwap & 0x00000000FFFFFFFFULL) << 32) | ((*quadWordToSwap & 0xFFFFFFFF00000000ULL) >> 32);
    *quadWordToSwap = ((*quadWordToSwap & 0x0000FFFF0000FFFFULL) << 16) | ((*quadWordToSwap & 0xFFFF0000FFFF0000ULL) >> 16);
    *quadWordToSwap = ((*quadWordToSwap & 0x00FF00FF00FF00FFULL) << 8) | ((*quadWordToSwap & 0xFF00FF00FF00FF00ULL) >> 8);
}

void word_Swap_64(uint64_t *quadWordToSwap)
{
    *quadWordToSwap = ((*quadWordToSwap & 0x00000000FFFFFFFFULL) << 32) | ((*quadWordToSwap & 0xFFFFFFFF00000000ULL) >> 32);
    *quadWordToSwap = ((*quadWordToSwap & 0x0000FFFF0000FFFFULL) << 16) | ((*quadWordToSwap & 0xFFFF0000FFFF0000ULL) >> 16);
}

void double_Word_Swap_64(uint64_t *quadWordToSwap)
{
    *quadWordToSwap = ((*quadWordToSwap & 0x00000000FFFFFFFFULL) << 32) | ((*quadWordToSwap & 0xFFFFFFFF00000000ULL) >> 32);
}

int16_t celsius_To_Fahrenheit(int16_t *celsius)
{
    int16_t fahrenheit = 0;
    fahrenheit = (int16_t)((*celsius * (9.0 / 5.0)) + 32.0);
    return fahrenheit;
}

int16_t fahrenheit_To_celsius(int16_t *fahrenheit)
{
    int16_t celsius = 0;
    celsius = (int16_t)((*fahrenheit - 32.0) * (5.0 / 9.0));
    return celsius;
}

int16_t celsius_To_Kelvin(int16_t *celsius)
{
    int16_t kelvin = 0;
    kelvin = (uint16_t)(*celsius + 273.15);
    return kelvin;
}

int16_t fahrenheit_To_Kelvin(int16_t *fahrenheit)
{
    int16_t kelvin = 0;
    kelvin = (int16_t)((5.0 / 9.0) * (*fahrenheit - 32.0) + 273.15);
    return kelvin;
}

int16_t kelvin_To_Celsius(int16_t *kelvin)
{
    int16_t celsius = 0;
    celsius = (uint16_t)(*kelvin - 273.15);
    return celsius;
}

int16_t kelvin_To_Fahrenheit(int16_t *kelvin)
{
    int16_t fahrenheit = 0;
    fahrenheit = (int16_t)((9.0 / 5.0) * (*kelvin - 273.15) + 32.0);
    return fahrenheit;
}

//use this to swap the bytes in a string...useful for ATA strings
void byte_Swap_String(char *stringToChange)
{
    uint32_t stringIter = 0;
    size_t   stringlen = strlen(stringToChange);
    char *swappedString = (char *)calloc(stringlen, sizeof(char));
    if (swappedString == NULL)
    {
        return;
    }
    memset(swappedString, 0, stringlen);
    if (stringlen > 0)
    {
        for (stringIter = 0; stringIter < stringlen; stringIter += 2)
        {
            swappedString[stringIter] = stringToChange[stringIter + 1];
            if (stringIter + 1 < stringlen)
            {
                swappedString[stringIter + 1] = stringToChange[stringIter];
            }
        }
    }
    strncpy(stringToChange, swappedString, stringlen);
    free(swappedString);
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

    while ((stringToChange[iter]) && iter < (strlen(stringToChange) - 1))  // having issues with the isspace command leaving extra chars in the string
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
    while (iter > 0 && isspace(stringToChange[iter - 1]))
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
    while (isspace(stringToChange[iter]) && iter < stringToChangeLen)
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
        stringToChange[iter] = (char)toupper(stringToChange[iter]);
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
        stringToChange[iter] = (char)tolower(stringToChange[iter]);
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
            stringToChange[iter] = (char)tolower(stringToChange[iter]);
        }
        else if (isupper(stringToChange[iter]))
        {
            stringToChange[iter] = (char)toupper(stringToChange[iter]);
        }
        iter++;
    }
}

size_t find_last_occurrence_in_string(char *originalString, char *stringToFind)
{  
    char *stringToCompare = originalString;
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

void print_Return_Enum(char *funcName, int ret)
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
    case COMMAND_TIMEOUT:
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
    default:
        printf("UNKNOWN: %d\n", ret);
        break;
    }
    printf("\n");
}

void print_Data_Buffer(uint8_t *dataBuffer, uint32_t bufferLen, bool showPrint)
{
    uint32_t printIter = 0, offset = 0;
    uint32_t offsetWidth = 2;//used to figure out how wide we need to pad with 0's for consistent output, 2 is the minimum width
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
            offset += 16;
        }
        printf("%02"PRIX8" ", dataBuffer[printIter]);
        if (showPrint)
        {
            if (isprint((int)dataBuffer[printIter]))
            {
                lineBuff[lineBuffIter] = dataBuffer[printIter];
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

int metric_Unit_Convert(double *byteValue, char** metricUnit)
{
    int ret = SUCCESS;
    uint8_t unitCounter = 0;

    while ((*byteValue / 1000.0) >= 1 && (unitCounter + 1) < 8)
    {
        *byteValue = *byteValue / 1000.00;
        unitCounter += 1;
    }
    switch (unitCounter)
    {
    case 0:
        strcpy(*metricUnit, "B");
        break;
    case 1:
        strcpy(*metricUnit, "KB");
        break;
    case 2:
        strcpy(*metricUnit, "MB");
        break;
    case 3:
        strcpy(*metricUnit, "GB");
        break;
    case 4:
        strcpy(*metricUnit, "TB");
        break;
    case 5:
        strcpy(*metricUnit, "PB");
        break;
    case 6:
        strcpy(*metricUnit, "EB");
        break;
    case 7:
        strcpy(*metricUnit, "ZB");
        break;
    case 8:
        strcpy(*metricUnit, "YB");
        break;
    default:
        ret = FAILURE;
    }
    return ret;
}

int capacity_Unit_Convert(double *byteValue, char** capacityUnit)
{
    int ret = SUCCESS;
    uint8_t unitCounter = 0;

    while ((*byteValue / 1024.0) >= 1 && (unitCounter + 1) < 8)
    {
        *byteValue = *byteValue / 1024.00;
        unitCounter += 1;
    }
    switch (unitCounter)
    {
    case 0:
        strcpy(*capacityUnit, "B");
        break;
    case 1:
        strcpy(*capacityUnit, "KiB");
        break;
    case 2:
        strcpy(*capacityUnit, "MiB");
        break;
    case 3:
        strcpy(*capacityUnit, "GiB");
        break;
    case 4:
        strcpy(*capacityUnit, "TiB");
        break;
    case 5:
        strcpy(*capacityUnit, "PiB");
        break;
    case 6:
        strcpy(*capacityUnit, "EiB");
        break;
    case 7:
        strcpy(*capacityUnit, "ZiB");
        break;
    case 8:
        strcpy(*capacityUnit, "YiB");
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
            if (((uint_fast8_t*)ptrData)[iter] != UINT8_C(0) || ((uint_fast8_t*)ptrData)[iterEnd] != UINT8_C(0))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

void convert_Seconds_To_Displayable_Time_Double(double secondsToConvert, uint8_t *years, uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    double tempCalcValue = secondsToConvert;
    //get seconds up to a maximum of 60
    if (seconds)
    {
        *seconds = (uint8_t)(fmod(tempCalcValue, 60.0));
    }
    tempCalcValue /= 60.0;
    //get minutes up to a maximum of 60
    if (minutes)
    {
        *minutes = (uint8_t)(fmod(tempCalcValue, 60.0));
    }
    tempCalcValue /= 60.0;
    //get hours up to a maximum of 24
    if (hours)
    {
        *hours = (uint8_t)(fmod(tempCalcValue, 24.0));
    }
    tempCalcValue /= 24.0;
    //get days up to 365
    if (days)
    {
        *days = (uint8_t)(fmod(tempCalcValue, 365.0));
    }
    tempCalcValue /= 365.0;
    //get years
    if (years)
    {
        *years = (uint8_t)(tempCalcValue);
    }
}

void convert_Seconds_To_Displayable_Time(uint64_t secondsToConvert, uint8_t *years, uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    uint64_t tempCalcValue = secondsToConvert;
    //get seconds up to a maximum of 60
    if (seconds)
    {
        *seconds = (uint8_t)(tempCalcValue % 60);
    }
    tempCalcValue /= 60;
    //get minutes up to a maximum of 60
    if (minutes)
    {
        *minutes = (uint8_t)(tempCalcValue % 60);
    }
    tempCalcValue /= 60;
    //get hours up to a maximum of 24
    if (hours)
    {
        *hours = (uint8_t)(tempCalcValue % 24);
    }
    tempCalcValue /= 24;
    //get days up to 365
    if (days)
    {
        *days = (uint8_t)(tempCalcValue % 365);
    }
    tempCalcValue /= 365;
    //get years
    if (years)
    {
        *years = (uint8_t)(tempCalcValue);
    }
}

void print_Time_To_Screen(uint8_t *years, uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
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
        printf(" %"PRIu8" day", *days);
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
    seed32Array[1] = (uint32_t)((int32_t)seed >> 1);//converting to signed int to perform arithmetic shift, then back for the seed value
    //using that initialization, run the random number generator for a more random seed...may or may not be needed, but I'm doing this anyways - Tyler
    seed32Array[0] = xorshiftplus32();
    seed32Array[0] = xorshiftplus32();
}
void seed_64(uint64_t seed)
{
    //first initialize
    seed64Array[0] = seed;
    seed64Array[1] = (uint64_t)((int64_t)seed >> 2);//converting to signed int to perform arithmetic shift, then back for the seed value
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

int fill_Random_Pattern_In_Buffer(uint8_t *ptrData, uint32_t dataLength)
{
    uint32_t *localPtr = (uint32_t*)ptrData;
    if (dataLength < sizeof(uint32_t) || !ptrData)
    {
        return BAD_PARAMETER;
    }
    seed_32((uint32_t)time(NULL));
    for (uint32_t iter = 0; iter < (dataLength / sizeof(uint32_t)); ++iter)
    {
        localPtr[iter] = xorshiftplus32();
    }
    return SUCCESS;
}

int fill_Hex_Pattern_In_Buffer(uint32_t hexPattern, uint8_t *ptrData, uint32_t dataLength)
{
    uint32_t *localPtr = (uint32_t*)ptrData;
    if (!ptrData || dataLength < sizeof(uint32_t))
    {
        return BAD_PARAMETER;
    }
    for (uint32_t iter = 0; iter < (dataLength / sizeof(uint32_t)); ++iter)
    {
        localPtr[iter] = hexPattern;
    }
    return SUCCESS;
}

int fill_Incrementing_Pattern_In_Buffer(uint8_t incrementStartValue, uint8_t *ptrData, uint32_t dataLength)
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

int fill_ASCII_Pattern_In_Buffer(const char *asciiPattern, uint32_t patternLength, uint8_t *ptrData, uint32_t dataLength)
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

int fill_Pattern_Buffer_Into_Another_Buffer(uint8_t *inPattern, uint32_t inpatternLength, uint8_t *ptrData, uint32_t dataLength)
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
#if defined _POSIX_VERSION && _POSIX_VERSION >= 200112L && defined _POSIX_THREAD_SAFE_FUNCTIONS
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
#if defined _POSIX_VERSION && _POSIX_VERSION >= 200112L && defined _POSIX_THREAD_SAFE_FUNCTIONS
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
        if (0 == strftime(buffer, bufferSize, "%c", timeptr))
        {
            //This means the array was too small...which shouldn't happen...but in case it does, zero out the memory
            memset(buffer, 0, bufferSize);
        }
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
    uint8_t years = 0, months = 0, days = 0, hours = 0, minutes = 0, seconds = 0;
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

int get_Compiler_Info(eCompiler *compilerUsed, ptrCompilerVersion compilerVersionInfo)
{
    int ret = SUCCESS;//should always return this unless we don't find a compiler we support.
    //make sure we set unknown and clear out version info before we continue.
    *compilerUsed = OPENSEA_COMPILER_UNKNOWN;
    memset(compilerVersionInfo, 0, sizeof(compilerVersion));
    //Time to start checking predefined compiler macros
#if defined _MSC_VER
    //Microsoft Visual C/C++ compiler (code is written to use the _MSC_FULL_VER from 2003 and later and we only really support 2013 and higher due to C99 usage)-TJE
    *compilerUsed = OPENSEA_COMPILER_MICROSOFT_VISUAL_C_CPP;
    char msVersion[10] = { 0 };
    sprintf(msVersion, "%u", _MSC_FULL_VER);
    char msMajor[3] = { 0 };
    char msMinor[3] = { 0 };
    char msPatch[6] = { 0 };
    strncpy(msMajor,&msVersion[0], 2);
    msMajor[2] = '\0';
    strncpy(msMinor,&msVersion[2], 2);
    msMinor[2] = '\0';
    strncpy(msPatch,&msVersion[4], 5);
    msPatch[5] = '\0';
    compilerVersionInfo->major = (uint16_t)atoi(msMajor);
    compilerVersionInfo->minor = (uint16_t)atoi(msMinor);
    compilerVersionInfo->patch = (uint16_t)atoi(msPatch);
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
    char hpVersion[7] = { 0 };
    sprintf(hpVersion, "%u", __HP_aCC);
    char hpMajor[3] = { 0 };
    char hpMinor[3] = { 0 };
    char hpPatch[3] = { 0 };
    strncpy(hpMajor, &hpVersion[0], 2);
    strncpy(hpMinor, &hpVersion[2], 2);
    strncpy(hpPatch, &hpVersion[4], 2);
    compilerVersionInfo->major = (uint16_t)atoi(hpMajor);
    compilerVersionInfo->minor = (uint16_t)atoi(hpMinor);
    compilerVersionInfo->patch = (uint16_t)atoi(hpPatch);
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
    char intelVersion[4] = { 0 };
    sprintf(intelVersion, "%u", __INTEL_COMPILER);
    char intelMajor[2] = { 0 };
    char intelMinor[2] = { 0 };
    char intelPatch[2] = { 0 };
    strncpy(intelMajor, &intelVersion[0], 1);
    strncpy(intelMinor, &intelVersion[1], 1);
    strncpy(intelPatch, &intelVersion[2], 1);
    compilerVersionInfo->major = (uint16_t)atoi(intelMajor);
    compilerVersionInfo->minor = (uint16_t)atoi(intelMinor);
    compilerVersionInfo->patch = (uint16_t)atoi(intelPatch);
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
    default:
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
    const char * tmp = strToConvert;
    while (*tmp != '\0')
    {
        if ( (!isxdigit(*tmp)) && (*tmp != 'x') && (*tmp != 'h') )
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
    //TODO: What about realllyyyy long hex value? 
    if (ret == true )
    {
        if ( hex )
        {
            *outputInteger = strtol(strToConvert, NULL, 16);
        }
        else
        {
            *outputInteger = strtol(strToConvert, NULL, 10);
        }
    }
    else
    {
        ret = false; 
    }
    //Final Check
    if (ret == true && errno != 0 && * outputInteger == 0)
    {
        ret = false;
    }

    return ret;
}

void print_Errno_To_Screen(int error)
{
#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && defined (__STDC_LIB_EXT1__)//This last piece must be defined by the compiler for this to be used...Only will be set if user defined __STDC_WANT_LIB_EXT1__
    size_t errorStringLen = strerrorlen_s(error);
    char *errorString = (char*)calloc(errorStringLen + 1, sizeof(char));
    if (errorString)
    {
        strerror(errorString, errorStringLen + 1, error);//returns 0 if whole string fits, nonzero if it was truncatd
        printf("%d - %s\n", error, errorString);
    }
    else
    {
        printf("%d - <Unable to convert error to string>\n", error);
    }
    safe_Free(errorString);
#elif defined (_MSC_VER) //TODO: Check for a specific version of visual studio?
    char errorString[1024] = { 0 };
    if(0 == strerror_s(errorString, 1024, error))
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
        result = 2 << (exponent - 1);
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
    int32_t  exponent = M_GETBITRANGE(floatValue, 30, 23) - 127;
    double sign = pow(-1.0, (double)M_GETBITRANGE(floatValue, 31, 31));

    int8_t power = -23;
    double mantisa = 1.0;
    for (uint8_t i = 0; i < 23; i++)
    {
        mantisa += (double)M_GETBITRANGE(floatValue, i, i) * (double)pow(2.0, power);
        power++;
    }

    *decimalValue = sign * (float)pow(2.0, exponent) * mantisa;
}
