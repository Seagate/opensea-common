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

//! \file env_detect.h
//! \brief Detects the compilation environment for standards, extensions, endianness, etc

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "predef_env_detect.h"

#if defined(__cplusplus)
extern "C"
{
#endif //__cplusplus

    //! \enum eArchitecture
    //! \brief Enum representing different processor architectures.
    //!
    //! This enum defines various processor architectures supported by the application.
    M_DECLARE_ENUM(eArchitecture,
                   /*!< Unknown architecture. */
                   OPENSEA_ARCH_UNKNOWN,
                   /*!< x86 architecture. */
                   OPENSEA_ARCH_X86,
                   /*!< x86_64 architecture. */
                   OPENSEA_ARCH_X86_64,
                   /*!< ARM architecture. */
                   OPENSEA_ARCH_ARM,
                   /*!< ARM64 architecture. */
                   OPENSEA_ARCH_ARM64,
                   /*!< PowerPC architecture. */
                   OPENSEA_ARCH_POWERPC,
                   /*!< PowerPC64 architecture. */
                   OPENSEA_ARCH_POWERPC64,
                   /*!< IA-64 architecture. */
                   OPENSEA_ARCH_IA_64,
                   /*!< SPARC architecture. */
                   OPENSEA_ARCH_SPARC,
                   /*!< Alpha architecture. */
                   OPENSEA_ARCH_ALPHA,
                   /*!< SystemZ architecture. */
                   OPENSEA_ARCH_SYSTEMZ,
                   /*!< MIPS architecture. */
                   OPENSEA_ARCH_MIPS,
                   /*!< Reserved for future use. */
                   OPENSEA_ARCH_RESERVED);

    //! \brief Returns the compiled architecture.
    //!
    //! This function returns an enum value specifying which processor architecture
    //! the application was compiled for.
    //!
    //! \return eArchitecture value representing the compiled architecture.
    eArchitecture get_Compiled_Architecture(void);

    //! \brief Prints the architecture in human-readable form.
    //!
    //! This function prints out the architecture in a human-readable form
    //!
    //! \param[in] arch The eArchitecture type to print out.
    void print_Architecture(eArchitecture arch);

    //! \enum eEndianness
    //! \brief Enum representing different endianness types.
    //!
    //! This enum defines various endianness types supported by the application.
    M_DECLARE_ENUM(eEndianness,
                   /*!< Little-endian. */
                   OPENSEA_LITTLE_ENDIAN,
                   /*!< Big-endian. */
                   OPENSEA_BIG_ENDIAN,
                   /*!< Little-word-endian (uncommon). */
                   OPENSEA_LITTLE_WORD_ENDIAN,
                   /*!< Big-word-endian (uncommon). */
                   OPENSEA_BIG_WORD_ENDIAN,
                   /*!< Unknown endianness. If this is returned, then __LITTLE_ENDIAN__ or __BIG_ENDIAN__ needs to be
                      defined for the compilation. */
                   OPENSEA_UNKNOWN_ENDIAN);

    //! \brief Returns the compiled endianness.
    //!
    //! This function returns an enum value specifying which endianness was detected when the application was compiled.
    //! More information: https://sourceforge.net/p/predef/wiki/Endianness/
    //!
    //! \return eEndianness value representing the endianness of the system.
    eEndianness get_Compiled_Endianness(void);

    //! \brief Prints out the endianness in human-readable form.
    //!
    //! This function prints out the endianness in human-readable form.
    //!
    //! \param[in] endian The eEndianness type to print.
    //! \param[in] shortPrint If true, prints the 3-digit shorthand endianness; otherwise, prints the long version.
    void print_Endianness(eEndianness endian, bool shortPrint);

    //! \enum eOSType
    //! \brief Enum representing different operating systems.
    //!
    //! This enum defines various operating systems supported by the application.
    M_DECLARE_ENUM(eOSType,
                   /*!< Unknown operating system. */
                   OS_UNKNOWN,
                   /*!< Windows operating system. */
                   OS_WINDOWS,
                   /*!< Linux operating system. */
                   OS_LINUX,
                   /*!< FreeBSD operating system. */
                   OS_FREEBSD,
                   /*!< Solaris operating system. */
                   OS_SOLARIS,
                   /*!< UEFI environment. */
                   OS_UEFI,
                   /*!< macOS operating system. */
                   OS_MACOSX,
                   /*!< AIX operating system. */
                   OS_AIX,
                   /*!< Tru64 operating system. */
                   OS_TRU64,
                   /*!< OpenBSD operating system. */
                   OS_OPENBSD,
                   /*!< NetBSD operating system. */
                   OS_NETBSD,
                   /*!< DragonFly BSD operating system. */
                   OS_DRAGONFLYBSD,
                   /*!< HP-UX operating system. */
                   OS_HPUX,
                   /*!< VMware ESXi operating system. */
                   OS_ESX
                   /*!< Add more operating systems here as we add support for them in our libraries. */
    );

    //! \brief Structure representing a Windows version number.
    //!
    //! This structure defines the version number for Windows operating systems.
    //! \see https://msdn.microsoft.com/en-us/library/windows/desktop/ms724832(v=vs.85).aspx
    //! \see http://blogs.msdn.com/b/chuckw/archive/2010/02/24/what-s-in-a-version-number.aspx
    //! \see http://blogs.msdn.com/b/chuckw/archive/2013/09/10/manifest-madness.aspx
    typedef struct sWindowsVersionNumber
    {
        uint32_t majorVersion; /*!< Major version number. */
        uint32_t minorVersion; /*!< Minor version number. */
        uint32_t buildNumber;  /*!< Build number. */
    } WindowsVersionNumber;

    //! \brief Structure representing a Linux version number.
    //!
    //! This structure defines the version number for Linux operating systems.
    //! \see https://en.wikipedia.org/wiki/Linux_kernel#Version_numbering
    typedef struct sLinuxVersionNumber
    {
        uint16_t kernelVersion;             /*!< Kernel version number. */
        uint16_t majorVersion;              /*!< Major version number. */
        uint16_t minorVersion;              /*!< Minor version number. */
        uint16_t securityAndBugFixesNumber; /*!< Security and bug fixes number. */
    } LinuxVersionNumber;

    //! \brief Structure representing a FreeBSD version number.
    //!
    //! This structure defines the version number for FreeBSD operating systems.
    //! \see https://en.wikipedia.org/wiki/FreeBSD#Version_history
    typedef struct sFreeBSDVersionNumber
    {
        uint16_t majorVersion; /*!< Major version number. */
        uint16_t minorVersion; /*!< Minor version number. */
    } FreeBSDVersionNumber;

    //! \brief Structure representing a Solaris version number.
    //!
    //! This structure defines the version number for Solaris operating systems.
    //! \see https://en.wikipedia.org/wiki/Solaris_(operating_system)#Version_history
    typedef struct sSolarisVersionNumber
    {
        uint16_t solarisMajorVersion; /*!< Solaris major version number. */
        uint16_t solarisMinorVersion; /*!< Solaris minor version number. */
        uint16_t solarisRevision;     /*!< Solaris revision number. */
        uint16_t sunOSMajorVersion;   /*!< SunOS major version number. */
        uint16_t sunOSMinorVersion;   /*!< SunOS minor version number. */
        uint16_t sunOSRevision;       /*!< SunOS revision number. */
    } SolarisVersionNumber;

    //! \brief Structure representing a macOS version number.
    //!
    //! This structure defines the version number for macOS operating systems.
    //! \see https://en.wikipedia.org/wiki/Darwin_(operating_system)#Release_history
    typedef struct sMacOSVersionNumber
    {
        uint16_t majorVersion; /*!< Major version number. */
        uint16_t minorVersion; /*!< Minor version number. */
        uint16_t revision;     /*!< Revision number. */
    } MacOSVersionNumber;

    //! \brief Structure representing an AIX version number.
    //!
    //! This structure defines the version number for AIX operating systems.
    typedef struct sAIXVersionNumber
    {
        uint16_t majorVersion; /*!< Major version number. */
        uint16_t minorVersion; /*!< Minor version number. */
    } AIXVersionNumber;

    //! \brief Structure representing a DragonFly BSD version number.
    //!
    //! This structure defines the version number for DragonFly BSD operating systems.
    typedef struct sDragonflyVersionNumber
    {
        uint16_t majorVersion; /*!< Major version number. */
        uint16_t minorVersion; /*!< Minor version number. */
    } DragonflyVersionNumber;

    //! \brief Structure representing an OpenBSD version number.
    //!
    //! This structure defines the version number for OpenBSD operating systems.
    typedef struct sOpenBSDVersionNumber
    {
        uint16_t majorVersion; /*!< Major version number. */
        uint16_t minorVersion; /*!< Minor version number. */
    } OpenBSDVersionNumber;

    //! \brief Structure representing a NetBSD version number.
    //!
    //! This structure defines the version number for NetBSD operating systems.
    typedef struct sNetBSDVersionNumber
    {
        uint16_t majorVersion; /*!< Major version number. */
        uint16_t minorVersion; /*!< Minor version number. */
        uint16_t revision;     /*!< Revision number. */
    } NetBSDVersionNumber;

    //! \brief Structure representing a Tru64 version number.
    //!
    //! This structure defines the version number for Tru64 operating systems.
    typedef struct sTru64VersionNumber
    {
        uint16_t majorVersion; /*!< Major version number. */
        uint16_t minorVersion; /*!< Minor version number. */
    } Tru64VersionNumber;

    //! \brief Structure representing an HP-UX version number.
    //!
    //! This structure defines the version number for HP-UX operating systems.
    typedef struct sHPUXVersionNumber
    {
        uint16_t majorVersion; /*!< Major version number. */
        uint16_t minorVersion; /*!< Minor version number. */
    } HPUXVersionNumber;

    //! \brief Structure representing a VMware ESXi version number.
    //!
    //! This structure defines the version number for VMware ESXi operating systems.
    //! \see https://en.wikipedia.org/wiki/VMware_ESXi#Versionshistory
    typedef struct sESXiVersionNumber
    {
        uint16_t majorVersion; /*!< Major version number. */
        uint16_t minorVersion; /*!< Minor version number. */
        uint16_t revision;     /*!< Revision number. */
    } ESXiVersionNumber;

    //! \brief Structure representing a UEFI version number.
    //!
    //! This structure defines the version number for UEFI environments.
    typedef struct sUEFIVersionNumber
    {
        uint16_t majorVersion; /*!< Major version number. */
        uint16_t minorVersion; /*!< Minor version number. */
    } UEFIVersionNumber;

    //! \brief Structure representing an OS version number.
    //!
    //! This structure defines the version number for various operating systems.
    typedef struct sOSVersionNumber
    {
        eOSType osVersioningIdentifier; /*!< Identifier for the OS versioning scheme. */
        union
        {
            WindowsVersionNumber   windowsVersion;   /*!< Windows version number. */
            LinuxVersionNumber     linuxVersion;     /*!< Linux version number. */
            FreeBSDVersionNumber   freeBSDVersion;   /*!< FreeBSD version number. */
            SolarisVersionNumber   solarisVersion;   /*!< Solaris version number. */
            UEFIVersionNumber      uefiVersion;      /*!< UEFI version number. */
            MacOSVersionNumber     macOSVersion;     /*!< macOS version number. */
            AIXVersionNumber       aixVersion;       /*!< AIX version number. */
            DragonflyVersionNumber dragonflyVersion; /*!< DragonFly BSD version number. */
            OpenBSDVersionNumber   openBSDVersion;   /*!< OpenBSD version number. */
            NetBSDVersionNumber    netBSDVersion;    /*!< NetBSD version number. */
            Tru64VersionNumber     tru64Version;     /*!< Tru64 version number. */
            HPUXVersionNumber      hpuxVersion;      /*!< HP-UX version number. */
            ESXiVersionNumber      esxiVersion;      /*!< VMware ESXi version number. */
            // Add other OS Versioning Schemes here for each OS we support
        } versionType; /*!< Union of version numbers for different operating systems. */
    } OSVersionNumber, *ptrOSVersionNumber;

//! \def OS_NAME_SIZE
//! \brief Defines the size of the OS name buffer.
//!
//! This macro defines the size of the buffer used to store the OS name. The size is large due to the potential
//! differences in the sizes of some fields in utsname.
//! \see https://man7.org/linux/man-pages/man2/uname.2.html
#define OS_NAME_SIZE (512)

    //! \brief Gets the version number of the OS the opensea-* libs are currently running on.
    //!
    //! This function retrieves the version number of the operating system. For Windows, it gets the OS version number
    //! (e.g., 6.1 vs 6.2). For Unix-like systems, it retrieves the information from `uname -r` (and `uname -v` for
    //! Solaris).
    //!
    //! \param[out] versionNumber Pointer to the OSVersionNumber struct. This will be filled with version information
    //! upon successful completion.
    //! \param[out] operatingSystemName (Optional, set to M_NULLPTR if not used) This will
    //! be a string with the friendly, human-readable name of the OS. For example, Windows 6.2 = Windows 8.
    //! \return SUCCESS if version information was successfully retrieved, otherwise a failure code.
    eReturnValues get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char* operatingSystemName);

    //! \brief Prints the OS Type enum in a human-readable form.
    //!
    //! This function prints the OS Type enum in a human-readable form.
    //!
    //! \param[in] osType The eOSType type to print.
    void print_OS_Type(eOSType osType);

    //! \brief Prints the OS Version number.
    //!
    //! This function prints the OS Version number, attempting to match the way version numbers are represented by each
    //! OS (typically #.#.#).
    //!
    //! \param[in] versionNumber Pointer to the OSVersionNumber structure with version information to print.
    void print_OS_Version(ptrOSVersionNumber versionNumber);

    //! \enum eCompiler
    //! \brief Enum representing different compilers.
    //!
    //! This enum defines various compilers supported by the application.
    M_DECLARE_ENUM(
        eCompiler,
        /*!< Unknown compiler. */
        OPENSEA_COMPILER_UNKNOWN,
        /*!< Microsoft Visual C/C++ compiler. */
        OPENSEA_COMPILER_MICROSOFT_VISUAL_C_CPP,
        /*!< GCC compiler. */
        OPENSEA_COMPILER_GCC,
        /*!< Clang compiler. */
        OPENSEA_COMPILER_CLANG,
        /*!< MinGW compiler. */
        OPENSEA_COMPILER_MINGW,
        /*!< Intel C/C++ compiler. */
        OPENSEA_COMPILER_INTEL_C_CPP,
        /*!< SunPro C/C++ compiler. */
        OPENSEA_COMPILER_SUNPRO_C_CPP,
        /*!< IBM XL C/C++ compiler. */
        OPENSEA_COMPILER_IBM_XL_C_CPP,
        /*!< IBM SystemZ C/C++ compiler. */
        OPENSEA_COMPILER_IBM_SYSTEMZ_C_CPP,
        /*!< HP aC++ compiler. */
        OPENSEA_COMPILER_HP_A_CPP
        /*!< Add other compilers here if we ever add more than those above (not all listed above are supported). */
    );

    //! \brief Structure representing a compiler version number.
    //!
    //! This structure defines the version number for compilers.
    typedef struct scompilerVersion
    {
        uint16_t major; /*!< Major version number. */
        uint16_t minor; /*!< Minor version number. */
        uint16_t patch; /*!< Patch version number. */
    } compilerVersion, *ptrCompilerVersion;

    //! \brief Gets the compiler information.
    //!
    //! This function returns the compiler and version used when compiling opensea-common (and likely the rest of the
    //! opensea-* libs).
    //!
    //! \param[out] compilerUsed Pointer to an eCompiler type that will be set with an enum value representing the
    //! compiler used upon successful completion.
    //! \param[out] compilerVersionInfo Pointer to the compilerVersion struct.
    //! This will be filled with version information upon successful completion.
    //! \return SUCCESS on successful completion, otherwise a failure code.
    eReturnValues get_Compiler_Info(eCompiler* compilerUsed, ptrCompilerVersion compilerVersionInfo);

    //! \brief Prints the name of the compiler.
    //!
    //! This function takes an eCompiler type and prints out the name of the compiler in human-readable form.
    //!
    //! \param[in] compilerUsed The eCompiler type to print.
    void print_Compiler(eCompiler compilerUsed);

    //! \brief Prints the compiler version information.
    //!
    //! This function prints the compiler version information in the form major.minor.patch from the compilerVersion
    //! struct.
    //!
    //! \param[in] compilerVersionInfo Pointer to the compilerVersion struct that holds the compiler version
    //! information.
    void print_Compiler_Version_Info(ptrCompilerVersion compilerVersionInfo);

    //! \brief Checks if the process is currently running with elevated permissions.
    //!
    //! This function checks if the process is currently running with elevated permissions. This is useful for
    //! operations that require elevated permissions, such as disk access.
    //!
    //! \return true if the process is running with elevated permissions, false otherwise.
    bool is_Running_Elevated(void);

#if defined(ENABLE_READ_USERNAME)
#    if defined(_WIN32)
// This pragma is needed to tell a library including opensea-common to look for
// Version.lib for the version helping information in the .c file. NOTE: ARM
// requires 10.0.16299.0 API to get this library!
#        if !defined(__MINGW32__) && !defined(__MINGW64__)
#            pragma comment(lib, "Advapi32.lib") // for checking for "run as administrator". May
                                                 // not be necessary to build some tools, but
                                                 // putting this here to prevent problems.
#        endif
#    endif //_WIN32
    // This flag is required to enable this functionality.
    //  A customer reported concerns about this reading the /etc/passwd file in
    //  Linux...the only way to map a UID to a username string So the solution to
    //  this problem is to flag this capability to being off by default. You must
    //  define this macro before building to use this functionality now.

    //! \brief Looks up the current user name.
    //!
    //! This function looks up the current user name using getuid(). It allocates memory for the user name, so make sure
    //! to free it when done. In Linux/Unix systems, this uses an API that opens the /etc/passwd file to do this
    //! mapping.
    //!
    //! \param[out] userName Pointer to a char* that will be allocated and filled with the user name upon successful
    //! completion.
    //! \return SUCCESS if no errors occurred and userName is allocated and ready to be used, BAD_PARAMETER
    //! if a bad pointer was provided, FAILURE if the user name could not be determined.
    eReturnValues get_Current_User_Name(char** userName);
#endif // ENABLE_READ_USERNAME

#if defined(__cplusplus)
}
#endif //__cplusplus
