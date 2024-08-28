// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file env_detect.h
// \brief Detects the compilation environment for standards, extensions, etc. Also detects CPU type and endianness
//

#pragma once

#include "predef_env_detect.h"
#include "code_attributes.h"
#include "common_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif //__cplusplus

    M_DECLARE_ENUM(eArchitecture,
        OPENSEA_ARCH_UNKNOWN,
        OPENSEA_ARCH_X86,
        OPENSEA_ARCH_X86_64,
        OPENSEA_ARCH_ARM,
        OPENSEA_ARCH_ARM64,
        OPENSEA_ARCH_POWERPC,
        OPENSEA_ARCH_POWERPC64,
        OPENSEA_ARCH_IA_64,
        OPENSEA_ARCH_SPARC,
        OPENSEA_ARCH_ALPHA,
        OPENSEA_ARCH_SYSTEMZ,
        OPENSEA_ARCH_MIPS,
        /*Add more architectures here as we support them*/
        OPENSEA_ARCH_RESERVED
    );

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

    M_DECLARE_ENUM(eEndianness,
        OPENSEA_LITTLE_ENDIAN,
        OPENSEA_BIG_ENDIAN,
        OPENSEA_LITTLE_WORD_ENDIAN,/*uncommon, probably won't be found/used*/
        OPENSEA_BIG_WORD_ENDIAN,/*uncommon, probably won't be found/used*/
        OPENSEA_UNKNOWN_ENDIAN /*if this is returned, then __LITTLE_ENDIAN__ or __BIG_ENDIAN__ needs to be defined for the compilation. Or you can ignore this and assume little endian - TJE*/
    );

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
    M_DECLARE_ENUM(eOSType,
        OS_UNKNOWN,
        OS_WINDOWS,
        OS_LINUX,
        OS_FREEBSD,
        OS_SOLARIS,
        OS_UEFI,
        OS_MACOSX,
        OS_AIX,
        OS_TRU64,
        OS_OPENBSD,
        OS_NETBSD,
        OS_DRAGONFLYBSD,
        OS_HPUX,
        OS_ESX,
        /*Add more operating systems here as we add support for them in our libraries*/
        );

    //https://msdn.microsoft.com/en-us/library/windows/desktop/ms724832(v=vs.85).aspx
    //http://blogs.msdn.com/b/chuckw/archive/2010/02/24/what-s-in-a-version-number.aspx
    //http://blogs.msdn.com/b/chuckw/archive/2013/09/10/manifest-madness.aspx
    typedef struct _WindowsVersionNumber
    {
        uint32_t majorVersion;
        uint32_t minorVersion;
        uint32_t buildNumber;
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
            //Add other OS Versioning Schemes here for each OS we support
        }versionType;
    }OSVersionNumber, *ptrOSVersionNumber;

    //very large due to utsname potentially having different sizes for some fields: https://man7.org/linux/man-pages/man2/uname.2.html
    #define OS_NAME_SIZE (512) 

    //-----------------------------------------------------------------------------
    //
    // get_Operating_System_Version_And_Name(eOSType *osType, ptrOSVersionNumber versionNumber, char *operatingSystemName)
    //
    // \brief   Description: Gets the version number of the OS opensea-* libs are currently running on. Windows will get the OS version number (not Win7 vs Win8, but 6.1 vs 6.2). Nix systems will get whatever is placed into uname -r (solaris will also get uname -v information)
    //
    // Entry:
    //      \param[out] versionNumber - pointer to the OSVersionNumber struct. This will be filled in with version information upon SUCCESSful completion.
    //      \param[out] operatingSystemName - (optional, set to M_NULLPTR if not being used) This will be a string with the friendly, human readable name of the OS. This will likely be the name of the release as it was announced. IE: Windows 6.2 = Windows 8.
    //
    // Exit:
    //      \return SUCCESS = got version information, !SUCCESS = failure getting version information.
    //
    //-----------------------------------------------------------------------------
    eReturnValues get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char *operatingSystemName);

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

    M_DECLARE_ENUM(eCompiler,
        OPENSEA_COMPILER_UNKNOWN,
        OPENSEA_COMPILER_MICROSOFT_VISUAL_C_CPP,
        OPENSEA_COMPILER_GCC,
        OPENSEA_COMPILER_CLANG,
        OPENSEA_COMPILER_MINGW,
        OPENSEA_COMPILER_INTEL_C_CPP,
        OPENSEA_COMPILER_SUNPRO_C_CPP,
        OPENSEA_COMPILER_IBM_XL_C_CPP,
        OPENSEA_COMPILER_IBM_SYSTEMZ_C_CPP,
        OPENSEA_COMPILER_HP_A_CPP,
        /*Add other compilers here if we ever add more than those above (which not all listed above are supported!)*/
        );

    typedef struct _compilerVersion
    {
        uint16_t major;
        uint16_t minor;
        uint16_t patch;
    }compilerVersion, * ptrCompilerVersion;

    //-----------------------------------------------------------------------------
    //
    //  get_Compiler_Info(eCompiler *compilerUsed, ptrCompilerVersion compilerVersionInfo)
    //
    //! \brief   Description:  This call will return which compiled and version of that compiler was used when compiling opensea-common (and likely the rest of opensea-* libs) 
    //
    //  Entry:
    //!   \param[out] compilerUsed = pointer to a eCompiler type that will be set with an enum value representing the compiler that was used on successful completion..
    //!   \param[out] compilerVersionInfo = pointer to the compilerVersion struct. This will be filled in with version information on successful completion.
    //!
    //  Exit:
    //!   \return SUCCESS on successful completion, !SUCCESS if problems encountered
    //
    //-----------------------------------------------------------------------------
    eReturnValues get_Compiler_Info(eCompiler* compilerUsed, ptrCompilerVersion compilerVersionInfo);

    //-----------------------------------------------------------------------------
    //
    //  print_Compiler(eCompiler compilerUsed)
    //
    //! \brief   Description:  This takes an eCompiler type and prints out the name of the compiler.
    //
    //  Entry:
    //!   \param[in] compilerUsed = eCompiler type that will be printed to the screen in human readable form
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Compiler(eCompiler compilerUsed);

    //-----------------------------------------------------------------------------
    //
    //  print_Compiler_Version_Info(ptrCompilerVersion compilerVersionInfo)
    //
    //! \brief   Description:  prints out the compiler version information in the form major.minor.patch from the compilerVersion struct
    //
    //  Entry:
    //!   \param[in] compilerVersionInfo = pointer to the compilerVersion struct that holds the compiler version information.
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void print_Compiler_Version_Info(ptrCompilerVersion compilerVersionInfo);

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

#if defined (ENABLE_READ_USERNAME)
    #if defined (_WIN32)
    //This pragma is needed to tell a library including opensea-common to look for Version.lib for the version helping information in the .c file.
    //NOTE: ARM requires 10.0.16299.0 API to get this library!
    #if !defined (__MINGW32__) && !defined (__MINGW64__)
        #pragma comment(lib,"Advapi32.lib")//for checking for "run as administrator". May not be necessary to build some tools, but putting this here to prevent problems.
    #endif
#endif //_WIN32
    //This flag is required to enable this functionality.
    // A customer reported concerns about this reading the /etc/passwd file in Linux...the only way to map a UID to a username string
    // So the solution to this problem is to flag this capability to being off by default.
    // You must define this macro before building to use this functionality now.
    //-----------------------------------------------------------------------------
    //
    //  get_Current_User_Name
    //
    //! \brief   Description:  This function will look up the current user name using getuid(). This function allocates memory for the user name, so make sure to free it when it is done.
    //!                        In linux/unix systems this uses an API that opens the /etc/passwd file to do this mapping.
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return SUCCESS = no errors, userName allocated and ready to be user, BAD_PARAMTER = bas pointer, FAILURE = could not determine user name.
    //
    //-----------------------------------------------------------------------------
    eReturnValues get_Current_User_Name(char** userName);
#endif //ENABLE_READ_USERNAME

#if defined (__cplusplus)
}
#endif //__cplusplus
