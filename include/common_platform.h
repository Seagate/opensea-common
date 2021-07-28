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
// \file common_platform.h
// \brief Includes OS specific files and defines functions that will be implemented specific to an OS

#pragma once

#include "common.h"

//This file's sole purpose is to include the correct common_<os/platform>.h/.c files for common things we may want to do -TJE
//Currently everything is being done in common_nix.h. If something begins to get too specific to linux vs freebsd, then we'll need common_linux and common_freebsd files.
#if defined (UEFI_C_SOURCE)
#include "common_uefi.h"
#elif defined (__linux__) 
#include "common_nix.h"
//#include "common_linux.h"
#elif defined (__DragonFly__)
//#error "Need a DragonFly BSD common file"
#include "common_nix.h"
#elif defined (__FreeBSD__)
//#error "Need a FreeBSD common file"
#include "common_nix.h"
#elif defined (__NetBSD__)
#include "common_nix.h"
//#error "Need a NetBSD common file"
#elif defined (__OpenBSD__)
#include "common_nix.h"
//#error "Need a OpenBSD common file"
#elif defined (__sun)
#include "common_nix.h"

#elif defined (_WIN32)
#include "common_windows.h"
#elif defined (_AIX)//IBM Unix
#include "common_nix.h"
//#error "Need a AIX common file"
#elif defined (__hpux)//HP Unix
#include "common_nix.h"
//#error "Need a HP UX common file"
#elif defined (__APPLE__)
#include "common_nix.h"
#include <TargetConditionals.h>
#if defined (TARGET_OS_MAC)
//#error "Need a Apple common file"
#include "common_nix.h"
#include <TargetConditionals.h>
#else
#error "Need Apple embedded common file"
#endif
#elif defined (__digital__) //tru64 unix
#include "common_nix.h"
//#error "Need a TRU64 common file"
#elif defined (__CYGWIN__) && !defined (_WIN32)
//this is using CYGWIN with POSIX under Windows. This means that the Win API is not available, so use unix/linux common includes
#include "common_nix.h"
//#include "common_linux.h"
#else
#error "Unknown OS. Need to specify a common_<os>.h  file to use\n"
#endif

#if defined (__cplusplus)
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
extern "C"
{
#endif

    typedef enum _eConsoleColors
    {
        BLUE,
        GREEN,
        RED,
        BLACK,
        CYAN,
        MAGENTA,
        BROWN,
        YELLOW,
        WHITE,
        GRAY,
        DARK_BLUE,
        DARK_GREEN,
        DARK_RED,
        TEAL,
        DARK_GRAY,
        PURPLE,
        DEFAULT,//this means white on windows
    }eConsoleColors;

    ////////////////////////////////////////////////////////////////////////////////////////////
    //// Functions below have specific implementations for OS's in the files included above ////
    ////////////////////////////////////////////////////////////////////////////////////////////


    //-----------------------------------------------------------------------------
    //
    //  set_Console_Colors()
    //
    //! \brief   Description:  Set the foreground or background color in the console output
    //!                        When building a console application, you should call this function to 
    //!                        set the default color BEFORE you start changing any other colors. 
    //!                        This is due to how Windows and the lower level API works to set the console colors. This doesn't apply to 'nix - TJE
    //
    //  Entry:
    //!   \param[in] foregroundBackground - true = foreground, false = background
    //!   \param[in] consoleColor = one of the enum values defined in this header for which color you want.
    //!
    //  Exit:
    //!   \return void
    //
    //-----------------------------------------------------------------------------
    void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor);

    //-----------------------------------------------------------------------------
    //
    // bool os_Directory_Exits (const char * const pathToCheck)
    //
    // \brief   Description: Platform independent helper to check if path exists. 
    //                       WARNING: May not work with UNICODE path. 
    //
    // Entry:
    //      \param[in] pathToCheck The path that needs to be evaluated. 
    //
    // Exit:
    //      \return true if exists, false if it does not. 
    //
    //-----------------------------------------------------------------------------
    bool os_Directory_Exists(const char * const pathToCheck);

    //-----------------------------------------------------------------------------
    //
    // bool os_File_Exists (const char * const filetoCheck)
    //
    // \brief   Description: Platform independent helper to check if file exists. 
    //                       WARNING: May not work with UNICODE path. 
    //
    // Entry:
    //      \param[in] filetoCheck The file that needs to be evaluated. 
    //
    // Exit:
    //      \return true if exists, false if it does not. 
    //
    //-----------------------------------------------------------------------------
    bool os_File_Exists(const char * const filetoCheck);

    //windows and 'nix require a file to use for finding a path as far as I can tell.-TJE
    int get_Full_Path(const char * pathAndFile, char fullPath[OPENSEA_PATH_MAX]);

    int replace_File_Name_In_Path(char fullPath[OPENSEA_PATH_MAX], char *newFileName);

    typedef enum _eArchitecture
    {
        OPENSEA_ARCH_UNKNOWN,
        OPENSEA_ARCH_X86,
        OPENSEA_ARCH_X86_64,
        OPENSEA_ARCH_ARM,
        OPENSEA_ARCH_ARM64,//not supported right now
        OPENSEA_ARCH_POWERPC,
        OPENSEA_ARCH_POWERPC64,
        OPENSEA_ARCH_IA_64,//Itanium //not supported right now
        OPENSEA_ARCH_SPARC,//not supported right now
        OPENSEA_ARCH_ALPHA,//not supported right now
        OPENSEA_ARCH_SYSTEMZ,//not supported right now - IBM System z architecture
        OPENSEA_ARCH_MIPS,
        //Add more architectures here as we support them
        OPENSEA_ARCH_RESERVED
    }eArchitecture;

    //-----------------------------------------------------------------------------
    //
    // get_Compiled_Architecture(void)
    //
    // \brief   Description: returns an enum value specifying which processor architecture opensea-common (and likely the rest of opensea-* libs) was compiled for.
    //
    // Entry:
    //
    // Exit:
    //      \return void
    //
    //-----------------------------------------------------------------------------
    eArchitecture get_Compiled_Architecture(void);

    //-----------------------------------------------------------------------------
    //
    // print_Architecture(eArchitecture arch)
    //
    // \brief   Description: Prints out the architecture in human readable form (most likely short hand, ie: powerpc = PPC)
    //
    // Entry:
    //      \param[in] arch - eArchitecure type to print out
    //
    // Exit:
    //      \return void
    //
    //-----------------------------------------------------------------------------
    void print_Architecture(eArchitecture arch);

    typedef enum _eEndianness
    {
        OPENSEA_LITTLE_ENDIAN,
        OPENSEA_BIG_ENDIAN,
        OPENSEA_LITTLE_WORD_ENDIAN,//uncommon, probably won't be found/used
        OPENSEA_BIG_WORD_ENDIAN,//uncommon, probably won't be found/used
        OPENSEA_UNKNOWN_ENDIAN //if this is returned, then __LITTLE_ENDIAN__ or __BIG_ENDIAN__ needs to be defined for the compilation. Or you can ignore this and assume little endian - TJE
    }eEndianness;

    //-----------------------------------------------------------------------------
    //
    // eEndianness get_Compiled_Endianness(void)
    //
    // \brief   Description: returns an enum value specifying which endianness was detected when it was compiled.
    //          https://sourceforge.net/p/predef/wiki/Endianness/
    //
    // Entry:
    //
    // Exit:
    //      \return eEndianness value representing the endianness of the system
    //
    //-----------------------------------------------------------------------------
    eEndianness get_Compiled_Endianness(void);

    //-----------------------------------------------------------------------------
    //
    // print_Endianness(eEndianness endian)
    //
    // \brief   Description: Prints out the endianness in human readable form. I.E. LSB or MSB for short and Little Endian and Big Endian for long
    //
    // Entry:
    //      \param[in] endian - eEndianness type
    //      \param[in] shortPrint - print out 3 digit short hand endianness or long version (see description)
    //
    // Exit:
    //      \return void
    //
    //-----------------------------------------------------------------------------
    void print_Endianness(eEndianness endian, bool shortPrint);

    //Not all of these are supported at this time.
    typedef enum _eOSType
    {
        OS_UNKNOWN,
        OS_WINDOWS,
        OS_LINUX,
        OS_FREEBSD,
        OS_SOLARIS,
        OS_UEFI,
        OS_MACOSX,//not supported yet
        OS_AIX,//not supported yet
        OS_TRU64,//not supported yet
        OS_OPENBSD,//not supported yet
        OS_NETBSD,//not supported yet
        OS_DRAGONFLYBSD,//not supported yet
        OS_HPUX,//not supported yet
        OS_ESX,// supporting
        //Add more operating systems here as we add support for them in our libraries
    }eOSType;

    //https://msdn.microsoft.com/en-us/library/windows/desktop/ms724832(v=vs.85).aspx
    //http://blogs.msdn.com/b/chuckw/archive/2010/02/24/what-s-in-a-version-number.aspx
    //http://blogs.msdn.com/b/chuckw/archive/2013/09/10/manifest-madness.aspx
    typedef struct _WindowsVersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
        uint16_t buildNumber;
    }WindowsVersionNumber;

    //https://en.wikipedia.org/wiki/Linux_kernel#Version_numbering
    typedef struct _LinuxVersionNumber
    {
        uint16_t kernelVersion;
        uint16_t majorVersion;
        uint16_t minorVersion;
        uint16_t securityAndBugFixesNumber;
    }LinuxVersionNumber;//kernel version info

    //https://en.wikipedia.org/wiki/FreeBSD#Version_history
    typedef struct _FreeBSDVersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
    }FreeBSDVersionNumber;

    //https://en.wikipedia.org/wiki/Solaris_(operating_system)#Version_history
    typedef struct _SolarisVersionNumber
    {
        uint16_t solarisMajorVersion;
        uint16_t solarisMinorVersion;
        uint16_t solarisRevision;
        uint16_t sunOSMajorVersion;
        uint16_t sunOSMinorVersion;
        uint16_t sunOSRevision;
    }SolarisVersionNumber;

    //https://en.wikipedia.org/wiki/Darwin_(operating_system)#Release_history
    typedef struct _MacOSVersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
        uint16_t revision;
    }MacOSVersionNumber;

    typedef struct _AIXVersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
    }AIXVersionNumber;

    typedef struct _DragonflyVersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
    }DragonflyVersionNumber;

    typedef struct _OpenBSDVersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
    }OpenBSDVersionNumber;

    typedef struct _NetBSDVersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
        uint16_t revision;
    }NetBSDVersionNumber;

    typedef struct _Tru64VersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
    }Tru64VersionNumber;

    typedef struct _HPUXVersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
    }HPUXVersionNumber;

    //https://en.wikipedia.org/wiki/VMware_ESXi#Versionshistory
    typedef struct _ESXiVersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
        uint16_t revision;
    }ESXiVersionNumber;

    typedef struct _UEFIVersionNumber
    {
        uint16_t majorVersion;
        uint16_t minorVersion;
    }UEFIVersionNumber;

    typedef struct _OSVersionNumber
    {
        eOSType osVersioningIdentifier;
        union{
            WindowsVersionNumber windowsVersion;
            LinuxVersionNumber linuxVersion;
            FreeBSDVersionNumber freeBSDVersion;
            SolarisVersionNumber solarisVersion;
            UEFIVersionNumber uefiVersion;
            MacOSVersionNumber macOSVersion;
            AIXVersionNumber aixVersion;
            DragonflyVersionNumber dragonflyVersion;
            OpenBSDVersionNumber openBSDVersion;
            NetBSDVersionNumber netBSDVersion;
            Tru64VersionNumber tru64Version;
            HPUXVersionNumber hpuxVersion;
            ESXiVersionNumber esxiVersion;
            //TODO: Add other OS Versioning Schemes here for each OS we support
        }versionType;
    }OSVersionNumber, *ptrOSVersionNumber;

    #define OS_NAME_SIZE (40)

    //-----------------------------------------------------------------------------
    //
    // get_Operating_System_Version_And_Name(eOSType *osType, ptrOSVersionNumber versionNumber, char *operatingSystemName[40])
    //
    // \brief   Description: Gets the version number of the OS opensea-* libs are currently running on. Windows will get the OS version number (not Win7 vs Win8, but 6.1 vs 6.2). Nix systems will get whatever is placed into uname -r (solaris will also get uname -v information)
    //
    // Entry:
    //      \param[out] versionNumber - pointer to the OSVersionNumber struct. This will be filled in with version information upon successful completion.
    //      \param[out] operatingSystemName - (optional, set to NULL if not being used) This will be a string with the friendly, human readable name of the OS. This will likely be the name of the release as it was announced. IE: Windows 6.2 = Windows 8.
    //
    // Exit:
    //      \return SUCCESS = got version information, !SUCCESS = failure getting version information.
    //
    //-----------------------------------------------------------------------------
    int get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char *operatingSystemName);

    //-----------------------------------------------------------------------------
    //
    // print_OS_Type(eOSType osType)
    //
    // \brief   Description: Prints out the OS Type enum into a human readable form
    //
    // Entry:
    //      \param[in] osType - eOSType type to print out
    //
    // Exit:
    //      \return void
    //
    //-----------------------------------------------------------------------------
    void print_OS_Type(eOSType osType);

    //-----------------------------------------------------------------------------
    //
    // print_OS_Version(ptrOSVersionNumber versionNumber, eOSType osType)
    //
    // \brief   Description: Prints out the OS Version number. This will attempt to match the was version numbers are represented by each OS (typically #.#.#)
    //
    // Entry:
    //      \param[in] versionNumber - pointer to the OSVersionNumber structure with version information to print out
    //
    // Exit:
    //      \return void
    //
    //-----------------------------------------------------------------------------
    void print_OS_Version(ptrOSVersionNumber versionNumber);

    //-----------------------------------------------------------------------------
    //
    // int64_t os_Get_File_Size(FILE *filePtr)
    //
    // \brief   Description: returns the size of the file pointer to by the incoming file pointer. This uses GetFileSizeEx in Windows and fstat64 in nix systems.
    //
    // Entry:
    //      \param[in] filePtr - pointer to a file that you wish to get the size of
    //
    // Exit:
    //      \return int64_t file size in bytes. -1 is returned if there is an error.
    //
    //-----------------------------------------------------------------------------
    int64_t os_Get_File_Size(FILE *filePtr);

    typedef struct _seatimer_t
    {
        uint64_t timerStart;//system specific count value. May need to do a calculation with this value so using it directly doesn't make sense.
        uint64_t timerStop;//system specific count value. May need to do a calculation with this value so using it directly doesn't make sense.
    }seatimer_t;

    //-----------------------------------------------------------------------------
    //
    // void start_Timer(seatimer_t *timer)
    //
    // \brief   Description: set's timerStart in a seatimer_t structure that will be used later when calculating how long the timer ran for.
    //
    // Entry:
    //      \param[in,out] timer - pointer to a seatimer_t structure that will be used for the timer.
    //
    // Exit:
    //      \return VOID
    //
    //-----------------------------------------------------------------------------
    void start_Timer(seatimer_t *timer);

    //-----------------------------------------------------------------------------
    //
    // void stop_Timer(seatimer_t *timer)
    //
    // \brief   Description: set's timerStop in a seatimer_t structure that will be used later when calculating how long the timer ran for.
    //
    // Entry:
    //      \param[in,out] timer - pointer to a seatimer_t structure that will be used for the timer.
    //
    // Exit:
    //      \return VOID
    //
    //-----------------------------------------------------------------------------
    void stop_Timer(seatimer_t *timer);

    //-----------------------------------------------------------------------------
    //
    // uint64_t get_Nano_Seconds(seatimer_t timer)
    //
    // \brief   Description: Gets the number of nano seconds elapsed between timerStart and timerStop
    //
    // Entry:
    //      \param[in] timer - pointer to a seatimer_t structure that will be used for the timer.
    //
    // Exit:
    //      \return uint64_t value representing the number of nanoseconds elapsed.
    //
    //-----------------------------------------------------------------------------
    uint64_t get_Nano_Seconds(seatimer_t timer);

    //-----------------------------------------------------------------------------
    //
    // double get_Micro_Seconds(seatimer_t timer)
    //
    // \brief   Description: Gets the number of micro seconds elapsed between timerStart and timerStop
    //
    // Entry:
    //      \param[in] timer - pointer to a seatimer_t structure that will be used for the timer.
    //
    // Exit:
    //      \return double value representing the number of microseconds elapsed.
    //
    //-----------------------------------------------------------------------------
    double get_Micro_Seconds(seatimer_t timer);

    //-----------------------------------------------------------------------------
    //
    // double get_Milli_Seconds(seatimer_t timer)
    //
    // \brief   Description: Gets the number of milli seconds elapsed between timerStart and timerStop
    //
    // Entry:
    //      \param[in] timer - pointer to a seatimer_t structure that will be used for the timer.
    //
    // Exit:
    //      \return double value representing the number of milliseconds elapsed.
    //
    //-----------------------------------------------------------------------------
    double get_Milli_Seconds(seatimer_t timer);

    //-----------------------------------------------------------------------------
    //
    // double get_Seconds(seatimer_t timer)
    //
    // \brief   Description: Gets the number of seconds elapsed between timerStart and timerStop
    //
    // Entry:
    //      \param[in] timer - pointer to a seatimer_t structure that will be used for the timer.
    //
    // Exit:
    //      \return double value representing the number of seconds elapsed.
    //
    //-----------------------------------------------------------------------------
    double get_Seconds(seatimer_t timer);

    //-----------------------------------------------------------------------------
    //
    //  is_Running_Elevated
    //
    //! \brief   Description:  Function that checks if the process is currently running with elevated permissions or not. Useful since things like disc access require this.
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return true = is elevated permissions, false = not running with elevated permissions
    //
    //-----------------------------------------------------------------------------
    bool is_Running_Elevated(void);
    
    //-----------------------------------------------------------------------------
    //
    //  get_Current_User_Name
    //
    //! \brief   Description:  This function will look up the current user name using getuid(). This function allocates memory for the user name, so make sure to free it when it is done.
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return SUCCESS = no errors, userName allocated and ready to be user, BAD_PARAMTER = bas pointer, FAILURE = could not determine user name.
    //
    //-----------------------------------------------------------------------------
    int get_Current_User_Name(char **userName);

#if defined (__cplusplus)
} //extern "C"
#endif
