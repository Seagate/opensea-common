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
// \file common_nix.c
// \brief Implements functions that are common to (u)nix like platforms code.
//
#include "common.h"
#include "common_nix.h"
#include <sys/types.h>
//The defines below are to enable fstat64 which i used in the function to read a file size (for large files, 64bit numbers may be required)
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#include <sys/stat.h>
#include <sys/param.h> //hopefully this is available on all 'nix systems
#include <sys/utsname.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>//for scan dir in linux to get os name. We can move ifdef this if it doesn't work for other OS's
#include <pwd.h>
#include <grp.h>

//freeBSD doesn't have the 64 versions of these functions...so I'm defining things this way to make it work. - TJE
#if defined(__FreeBSD__)
#define stat64 stat
#define fstat64 fstat
#endif

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
    char *resolvedPath = realpath(pathAndFile, fullPath);
    if (resolvedPath)
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}

void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    //linux/BSD use the ANSI escape sequences to change colors http://tldp.org/HOWTO/Bash-Prompt-HOWTO/x329.html
    if (foregroundBackground)//change foreground color
    {
        switch (consoleColor)
        {
        case DARK_BLUE:
            printf("\033[0;34m");
            break;
        case BLUE:
            printf("\033[1;34m");
            break;
        case DARK_GREEN:
            printf("\033[0;32m");
            break;
        case GREEN:
            printf("\033[1;32m");
            break;
        case DARK_RED:
            printf("\033[0;31m");
            break;
        case RED:
            printf("\033[1;31m");
            break;
        case BLACK:
            printf("\033[0;30m");
            break;
        case BROWN:
            printf("\033[0;33m");
            break;
        case YELLOW:
            printf("\033[1;33m");
            break;
        case TEAL:
            printf("\033[0;36m");
            break;
        case CYAN:
            printf("\033[1;36m");
            break;
        case PURPLE:
            printf("\033[0;35m");
            break;
        case MAGENTA:
            printf("\033[1;35m");
            break;
        case WHITE:
            printf("\033[1;37m");
            break;
        case DARK_GRAY:
            printf("\033[1;30m");
            break;
        case GRAY:
            printf("\033[0;37m");
            break;
        case DEFAULT://fall through to default (which is white)
        default:
            printf("\033[0m");
        }
    }
    else//change background color
    {
        switch (consoleColor)
        {
        case DARK_BLUE:
            printf("\033[7;34m");
            break;
        case BLUE:
            printf("\033[7;1;34m");
            break;
        case DARK_GREEN:
            printf("\033[7;32m");
            break;
        case GREEN:
            printf("\033[7;1;32m");
            break;
        case DARK_RED:
            printf("\033[7;31m");
            break;
        case RED:
            printf("\033[7;1;31m");
            break;
        case BLACK:
            printf("\033[7;30m");
            break;
        case BROWN:
            printf("\033[7;33m");
            break;
        case YELLOW:
            printf("\033[7;1;33m");
            break;
        case TEAL:
            printf("\033[7;36m");
            break;
        case CYAN:
            printf("\033[7;1;36m");
            break;
        case PURPLE:
            printf("\033[7;35m");
            break;
        case MAGENTA:
            printf("\033[7;1;35m");
            break;
        case WHITE:
            printf("\033[7;1;37m");
            break;
        case DARK_GRAY:
            printf("\033[7;1;30m");
            break;
        case GRAY:
            printf("\033[7;37m");
            break;
        case DEFAULT://fall through to default (which is white)
        default:
            printf("\033[0m");
        }
    }
}

eArchitecture get_Compiled_Architecture(void)
{
    //check which compiler we're using to use it's preprocessor definitions
    #if defined __INTEL_COMPILER //Intel's C/C++ compiler
        #if defined _M_X64 || defined _M_AMD64
            return OPENSEA_ARCH_X86_64;
        #elif defined _M_ALPHA
            return OPENSEA_ARCH_ALPHA;
        #elif defined _M_ARM || defined _M_ARMT
            return OPENSEA_ARCH_ARM;
        #elif defined _M_IX86
            return OPENSEA_ARCH_X86;
        #elif defined _M_IA64
            return OPENSEA_ARCH_IA_64;
        #elif defined _M_PPC //32bits I'm guessing - TJE
            return OPENSEA_ARCH_POWERPC;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #elif defined __CYGWIN__ || defined __clang__ || defined __GNUC__ //I'm guessing that all these compilers will use the same macro definitions since the sourceforge pages aren't 100% complete (clang I'm most unsure about)
        #if defined __alpha__
            return OPENSEA_ARCH_ALPHA;
        #elif defined __amd64__ || defined __x86_64__
            return OPENSEA_ARCH_X86_64;
        #elif defined __arm__ || defined __thumb__
            return OPENSEA_ARCH_ARM;
        #elif defined __aarch64__
            return OPENSEA_ARCH_ARM64;
        #elif defined __i386__ || defined __i486__ || defined __i586__ || defined __i686__
            return OPENSEA_ARCH_X86;
        #elif defined __ia64__ || defined __IA64__
            return OPENSEA_ARCH_IA_64;
        #elif defined __powerpc64__ || defined __PPC64__ || defined __ppc64__ || defined _ARCH_PPC64
            return OPENSEA_ARCH_POWERPC64;
        #elif defined __powerpc__ || defined __POWERPC__ || defined __PPC__ || defined __ppc__ || defined _ARCH_PPC
            return OPENSEA_ARCH_POWERPC;
        #elif defined __sparc__
            return OPENSEA_ARCH_SPARC;
        #elif defined __s390__ || defined __s390x__ || defined __zarch__
            return OPENSEA_ARCH_SYSTEMZ;
        #elif defined __mips__
            return OPENSEA_ARCH_MIPS;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #elif defined __SUNPRO_C || defined __SUNPRO_CC //SUN/Oracle compilers (unix)
        #if defined __amd64__ || defined __x86_64__
            return OPENSEA_ARCH_X86_64;
        #elif defined __i386
            return OPENSEA_ARCH_X86;
        #elif defined __sparc
            return OPENSEA_ARCH_SPARC;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #elif defined __IBMC__ || defined __IBMCPP__ //IBM compiler (unix, linux)
        #if defined __370__ || defined __THW_370__
            return OPENSEA_ARCH_SYSTEMZ;
        #elif defined __THW_INTEL__
            return OPENSEA_ARCH_X86;
        #elif defined _ARCH_PPC64
            return OPENSEA_ARCH_POWERPC64;
        #elif defined _ARCH_PPC
            return OPENSEA_ARCH_POWERPC;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #else
        return OPENSEA_ARCH_UNKNOWN;
    #endif
}
//https://sourceforge.net/p/predef/wiki/Endianness/
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
    #if defined (__BYTE_ORDER__)
        #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            return OPENSEA_BIG_ENDIAN;
        #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            return OPENSEA_LITTLE_ENDIAN;
        #else
            return calculate_Endianness();
        #endif
    #else
        #if defined (__BIG_ENDIAN__)
            return OPENSEA_BIG_ENDIAN;
        #elif defined (__LITTLE_ENDIAN__)
            return OPENSEA_LITTLE_ENDIAN;
        #else
            //check architecture specific defines...
            #if defined (__ARMEB__) || defined (__THUMBEB__) || defined (__AARCH64EB__) || defined (_MIPSEB) || defined (__MIPSEB) || defined (__MIPSEB__)
                return OPENSEA_BIG_ENDIAN;
            #elif defined (__ARMEL__) || defined (__THUMBEL__) || defined (__AARCH64EL__) || defined (_MIPSEL) || defined (__MIPSEL) || defined (__MIPSEL__)
                return OPENSEA_LITTLE_ENDIAN;
            #else
                return calculate_Endianness();
            #endif
        #endif
    #endif
    return calculate_Endianness();
}

static int lin_file_filter(const struct dirent *entry, const char *stringMatch)
{
    int match = 0;
    size_t filenameLength = strlen(entry->d_name) + 6;
    char *filename = C_CAST(char*, calloc(filenameLength, sizeof(char)));
    struct stat s;
    snprintf(filename, filenameLength, "/etc/%s", entry->d_name);
    if (stat(filename, &s) == 0)
    {
        if (S_ISREG(s.st_mode)) //must be a file. TODO: are links ok? I don't think we need them, but may need to revisit this.
        {
            //non-zero means valid match. zero means not a match
            char *inString = strstr(entry->d_name, stringMatch);
            if (inString)
            {
                //found a file!
                //make sure the string to match is at the end of the entry's name!
                size_t nameLen = strlen(entry->d_name);
                size_t matchLen = strlen(stringMatch);
                if (0 == strncmp(entry->d_name + nameLen - matchLen, stringMatch, matchLen))
                {
                    match = 1;
                }
            }
        }
    }
    safe_Free(filename)
    return match;
}

static int release_file_filter( const struct dirent *entry )
{
    return lin_file_filter(entry, "-release");//looks like almost all linux specific releases put it as -release
}

static int version_file_filter( const struct dirent *entry )
{
    return lin_file_filter(entry, "version");//most, but not all do a _version, but some do a -, so this should work for both
}

//code is written based on the table in this link https://en.wikipedia.org/wiki/Uname
//This code is not complete for all operating systems. I only added in support for the ones we are most interested in or are already using today. -TJE
int get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char *operatingSystemName)
{
    int ret = SUCCESS;
    //check what is filled in by the uname call in the utsname struct (BUT DON"T CHECK THE DOMAIN NAME since that is GNU specific)
    struct utsname unixUname;
    memset(&unixUname, 0, sizeof(struct utsname));
    if (0 == uname(&unixUname))
    {
        convert_String_To_Upper_Case(unixUname.sysname);
        if (strcmp("LINUX", unixUname.sysname) == 0)//some OSs like Android or ESX may fall into here, so this section may need expansion if that happens
        {
            bool linuxOSNameFound = false;
            versionNumber->osVersioningIdentifier = OS_LINUX;
            //linux kernels are versioned as kernel.major.minor-securityAndBugFixes-SomeString
            //older linux kernels don't have the -securityAndBugFixes on the end
            if (EOF == sscanf(unixUname.release,"%"SCNu16".%"SCNu16".%"SCNu16"-%"SCNu16"%*s",&versionNumber->versionType.linuxVersion.kernelVersion, &versionNumber->versionType.linuxVersion.majorVersion, &versionNumber->versionType.linuxVersion.minorVersion, &versionNumber->versionType.linuxVersion.securityAndBugFixesNumber))
            {
                ret = FAILURE;
            }
            bool etcRelease = os_File_Exists("/etc/os-release");
            bool usrLibRelease = os_File_Exists("/usr/lib/os-release");
            if (etcRelease || usrLibRelease)//available on newer OS's
            {
                //read this file and get the linux name
                FILE *release = NULL;
                if(etcRelease)
                {
                    release = fopen("/etc/os-release","r");
                }
                else
                {
                    release = fopen("/usr/lib/os-release","r");
                }
                if (release)
                {
                    //read it
                    fseek(release,ftell(release),SEEK_END);
                    long int releaseSize = ftell(release);
                    rewind(release);
                    char *releaseMemory = C_CAST(char*, calloc(releaseSize, sizeof(char)));
                    if (fread(releaseMemory, sizeof(char), releaseSize, release))
                    {
                        //Use the "PRETTY_NAME" field
                        bool done = false;
                        char *tok = strtok(releaseMemory, "\n");
                        while (tok != NULL && !done)
                        {
                            if (strncmp(tok, "PRETTY_NAME=", strlen("PRETTY_NAME=")) == 0)
                            {
                                linuxOSNameFound = true;

                                if (operatingSystemName)
                                {
                                    snprintf(&operatingSystemName[0], OS_NAME_SIZE, "%.*s", C_CAST(int, strlen(tok) - 1 - strlen("PRETTY_NAME=\"")), tok + strlen("PRETTY_NAME=\""));
                                }
                                done = true;
                                break;
                            }
                            tok = strtok(NULL, "\n");
                        }
                    }
                    safe_Free(releaseMemory)
                    fclose(release);
                }
            }
            else
            {
                //try other release files before /etc/issue. More are here than are implemented: http://linuxmafia.com/faq/Admin/release-files.html
                //This may not cover all of the possible Linux's, but it should get most of them. We'll check for the most common ways of specifying the file to keep this simpler.
                //NOTE: This may also work on some other unix systems, so we may want to try it on those as well.
                struct dirent **osrelease;//most use a file named *-release
                struct dirent **osversion;//some use a file named *_version
                int releaseFileCount = scandir("/etc", &osrelease, release_file_filter, alphasort);
                int versionFileCount = scandir("/etc", &osversion, version_file_filter, alphasort);
                int lsbReleaseOffset = -1;//-1 is invalid.
                if(releaseFileCount > 0)//ideally this will only ever be 1
                {
                    for (int releaseIter = 0; releaseIter < releaseFileCount; ++releaseIter)
                    {
                        if (0 == strcmp(osrelease[releaseIter]->d_name, "lsb-release"))
                        {
                            lsbReleaseOffset = releaseIter;
                            //Don't read this file for version info yet. it's common across systems but the other release and version files are more interesting to us
                        }
                        else
                        {
                            size_t fileNameLength = strlen(osrelease[releaseIter]->d_name) + 6;
                            char *fileName = C_CAST(char *, calloc(fileNameLength, sizeof(char)));
                            snprintf(fileName, fileNameLength, "/etc/%s", osrelease[releaseIter]->d_name);
                            FILE *release = fopen(fileName, "r");
                            if (release)
                            {
                                //read it
                                fseek(release,ftell(release),SEEK_END);
                                long int releaseSize = ftell(release);
                                rewind(release);
                                char *releaseMemory = C_CAST(char*, calloc(releaseSize,sizeof(char)));
                                if (fread(releaseMemory, sizeof(char), releaseSize, release))
                                {
                                    linuxOSNameFound = true;
                                    if (operatingSystemName)
                                    {
                                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "%.*s", C_CAST(int, releaseSize), releaseMemory);
                                    }
                                }
                                safe_Free(releaseMemory)
                                fclose(release);
                            }
                            safe_Free(fileName)
                        }
                        if (linuxOSNameFound)
                        {
                            //we got a name, so break out of the loop (in case there are multiple files, we only need to read one)
                            break;
                        }
                    }
                }
                if(!linuxOSNameFound && versionFileCount > 0)//ideally this will only ever be 1
                {
                    //For now, only reading the first entry...this SHOULD be ok.
                    size_t fileNameLength = strlen(osversion[0]->d_name) + 6;
                    char *fileName = C_CAST(char*, calloc(fileNameLength, sizeof(char)));
                    snprintf(fileName, fileNameLength, "/etc/%s", osversion[0]->d_name);
                    FILE *version = fopen(fileName, "r");
                    if (version)
                    {
                        //read it
                        fseek(version,ftell(version),SEEK_END);
                        long int versionSize = ftell(version);
                        rewind(version);
                        char *versionMemory = C_CAST(char*, calloc(versionSize,sizeof(char)));
                        if (fread(versionMemory, sizeof(char), versionSize, version))
                        {
                            linuxOSNameFound = true;
                            if (operatingSystemName)
                            {
                                snprintf(&operatingSystemName[0], OS_NAME_SIZE, "%.*s", C_CAST(int, versionSize), versionMemory);
                            }
                        }
                        safe_Free(versionMemory)
                        fclose(version);
                    }
                    safe_Free(fileName)
                }
                if (!linuxOSNameFound && lsbReleaseOffset >= 0)
                {
                    //this case means that we found the lbs-release file, but haven't read it yet because we kept searching for other files to use for version information first.
                    //So now we need to read it for the version information.
                    //We use this last because it may not contain something as friendly and useful as we would like that the other version files provide.
                    size_t fileNameLength = strlen(osrelease[lsbReleaseOffset]->d_name) + 6;
                    char *fileName = C_CAST(char *, calloc(fileNameLength, sizeof(char)));
                    snprintf(fileName, fileNameLength, "/etc/%s", osrelease[lsbReleaseOffset]->d_name);
                    FILE *release = fopen(fileName, "r");
                    if (release)
                    {
                        //read it
                        fseek(release,ftell(release),SEEK_END);
                        long int releaseSize = ftell(release);
                        rewind(release);
                        char *releaseMemory = C_CAST(char*, calloc(releaseSize,sizeof(char)));
                        if (fread(releaseMemory, sizeof(char), releaseSize, release))
                        {
                            linuxOSNameFound = true;
                            if (operatingSystemName)
                            {
                                snprintf(&operatingSystemName[0], OS_NAME_SIZE, "%.*s", C_CAST(int, releaseSize), releaseMemory);
                            }
                        }
                        safe_Free(releaseMemory)
                        fclose(release);
                    }
                    safe_Free(fileName)
                }
                for (int iter = 0; iter < releaseFileCount; ++iter)
                {
                    safe_Free(osrelease[iter])
                }
                for (int iter = 0; iter < versionFileCount; ++iter)
                {
                    safe_Free(osversion[iter])
                }
                safe_Free(osrelease)
                safe_Free(osversion)
                if (linuxOSNameFound)
                {
                    //remove any control characters from the string. We don't need them for what we're doing
                    for (size_t iter = 0; iter < strlen(operatingSystemName); ++iter)
                    {
                        if (iscntrl(operatingSystemName[iter]))
                        {
                            operatingSystemName[iter] = ' ';
                        }
                    }
                    //set the null terminator at the end to make sure it's there and not removed in the loop above
                    operatingSystemName[OS_NAME_SIZE-1] = '\0';
                }
            }
            //only use /etc/issue if we couldn't get a version from anywhere else.
            if (!linuxOSNameFound && os_File_Exists("/etc/issue"))//set the operating system name from /etc/issue (if available, otherwise set "Unknown Linux OS")
            {
                //read this file and get the linux name
                FILE *issue = fopen("/etc/issue","r");
                if (issue)
                {
                    //read it
                    fseek(issue,ftell(issue),SEEK_END);
                    long int issueSize = ftell(issue);
                    rewind(issue);
                    char *issueMemory = C_CAST(char*, calloc(issueSize,sizeof(char)));
                    if (fread(issueMemory, sizeof(char), issueSize, issue))
                    {
                        linuxOSNameFound = true;
                        if (operatingSystemName)
                        {
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "%.*s", C_CAST(int, issueSize), issueMemory);
                        }
                    }
                    safe_Free(issueMemory)
                    fclose(issue);
                }
            }
            //if we couldn't find a name, set unknown linux os
            if (!linuxOSNameFound && operatingSystemName)
            {
                snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Linux OS");
            }
        }
        else if (strcmp("FREEBSD", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_FREEBSD;
            //FreeBSD version is stored as Major.Minor-SomeString
            if (EOF == sscanf(unixUname.release,"%"SCNu16".%"SCNu16"%*s",&versionNumber->versionType.freeBSDVersion.majorVersion, &versionNumber->versionType.freeBSDVersion.minorVersion))
            {
                ret = FAILURE;
            }
            //set the OS Name
            if (operatingSystemName)
            {
                if (ret != FAILURE)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE, "FreeBSD %"PRIu16".%"PRIu16"", versionNumber->versionType.freeBSDVersion.majorVersion, versionNumber->versionType.freeBSDVersion.minorVersion);
                }
                else
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown FreeBSD OS Version");
                }
            }
        }
        else if (strcmp("SUNOS", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_SOLARIS;
            //Solaris stores the SunOS version in release
            if (EOF == sscanf(unixUname.release,"%"SCNu16".%"SCNu16".%"SCNu16"%*s",&versionNumber->versionType.solarisVersion.sunOSMajorVersion, &versionNumber->versionType.solarisVersion.sunOSMinorVersion, &versionNumber->versionType.solarisVersion.sunOSRevision))
            {
                ret = FAILURE;
            }
            //set OS name as Solaris "unixUname.version" for the OS Name
            if (operatingSystemName)
            {
                snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Solaris %s", unixUname.version);
            }
            //The Solaris Version/name is stored in version
            if (isdigit(unixUname.version[0]))
            {
                //set OS name as Solaris x.x
                if (EOF == sscanf(unixUname.version,"%"SCNu16".%"SCNu16".%"SCNu16"%*s",&versionNumber->versionType.solarisVersion.solarisMajorVersion, &versionNumber->versionType.solarisVersion.solarisMinorVersion, &versionNumber->versionType.solarisVersion.solarisRevision))
                {
                    //do nothing for now - TJE
                }
            }
        }
        else if (strcmp("DARWIN", unixUname.sysname) == 0)//Mac OSX
        {
            versionNumber->osVersioningIdentifier = OS_MACOSX;
            if (EOF == sscanf(unixUname.release,"%"SCNu16".%"SCNu16".%"SCNu16"%*s",&versionNumber->versionType.macOSVersion.majorVersion, &versionNumber->versionType.macOSVersion.minorVersion, &versionNumber->versionType.macOSVersion.revision))
            {
                ret = FAILURE;
            }
            //set the OS Name from the major numbers starting with "Puma"
            //https://en.wikipedia.org/wiki/Darwin_(operating_system)
            if (operatingSystemName)
            {
                switch (versionNumber->versionType.macOSVersion.majorVersion)
                {
                case 5://puma
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Mac OS X 10.1 Puma");
                    break;
                case 6://jaguar
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Mac OS X 10.2 Jaguar");
                    break;
                case 7://panther
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Mac OS X 10.3 Panther");
                    break;
                case 8://tiger
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Mac OS X 10.4 Tiger");
                    break;
                case 9://leopard
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Mac OS X 10.5 Leopard");
                    break;
                case 10://snow leopard
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Mac OS X 10.6 Snow Leopard");
                    break;
                case 11://lion
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Mac OS X 10.7 Lion");
                    break;
                case 12://Mountain Lion
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "OS X 10.8 Mountain Lion");
                    break;
                case 13://mavericks
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "OS X 10.9 Mavericks");
                    break;
                case 14://Yosemite
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "OS X 10.10 Yosemite");
                    break;
                case 15://el capitan
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "OS X 10.11 El Capitan");
                    break;
                default:
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Unknown Mac OS X Version");
                    break;
                }
            }
        }
        else if (strcmp("AIX", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_AIX;
            versionNumber->versionType.aixVersion.majorVersion = C_CAST(uint16_t, atoi(unixUname.version));
            versionNumber->versionType.aixVersion.minorVersion = C_CAST(uint16_t, atoi(unixUname.release));
            if (operatingSystemName)
            {
                snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "AIX %"PRIu16".%"PRIu16"", versionNumber->versionType.aixVersion.majorVersion, versionNumber->versionType.aixVersion.minorVersion);
            }
        }
        else if (strcmp("DRAGONFLY", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_DRAGONFLYBSD;
            if (EOF == sscanf(unixUname.release, "%"SCNu16".%"SCNu16"%*s", &versionNumber->versionType.dragonflyVersion.majorVersion, &versionNumber->versionType.dragonflyVersion.minorVersion))
            {
                ret = FAILURE;
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Unknown Dragonfly BSD Version");
                }
            }
            else
            {
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Dragonfly BSD %"PRIu16".%"PRIu16"", versionNumber->versionType.dragonflyVersion.majorVersion, versionNumber->versionType.dragonflyVersion.minorVersion);
                }
            }
        }
        else if (strcmp("OPENBSD", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_OPENBSD;
            versionNumber->versionType.openBSDVersion.majorVersion = C_CAST(uint16_t, atoi(unixUname.version));
            versionNumber->versionType.openBSDVersion.minorVersion = C_CAST(uint16_t, atoi(unixUname.release));
            if (operatingSystemName)
            {
                snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "OpenBSD %"PRIu16".%"PRIu16"", versionNumber->versionType.openBSDVersion.majorVersion, versionNumber->versionType.openBSDVersion.minorVersion);
            }
        }
        else if (strcmp("NETBSD", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_NETBSD;
            if (EOF == sscanf(unixUname.release, "%"SCNu16".%"SCNu16".%"SCNu16"%*s", &versionNumber->versionType.netBSDVersion.majorVersion, &versionNumber->versionType.netBSDVersion.minorVersion, &versionNumber->versionType.netBSDVersion.revision))
            {
                ret = FAILURE;
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Unknown NetBSD Version");
                }
            }
            else
            {
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "NetBSD %s", unixUname.release);
                }
            }
        }
        else if (strcmp("OSF1", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_TRU64;
            if (EOF == sscanf(unixUname.release, "V%"SCNu16".%"SCNu16"", &versionNumber->versionType.tru64Version.majorVersion, &versionNumber->versionType.tru64Version.minorVersion))
            {
                ret = FAILURE;
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Unknown Tru64 Version");
                }
            }
            else
            {
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Tru64 %s", unixUname.release);
                }
            }
        }
        else if (strcmp("HP-UX", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_HPUX;
            if (EOF == sscanf(unixUname.release, "B.%"SCNu16".%"SCNu16"", &versionNumber->versionType.hpuxVersion.majorVersion, &versionNumber->versionType.hpuxVersion.minorVersion))
            {
                ret = FAILURE;
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Unknown HP-UX Version");
                }
            }
            else
            {
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "HP-UX %"PRIu16".%"PRIu16"", versionNumber->versionType.hpuxVersion.majorVersion, versionNumber->versionType.hpuxVersion.minorVersion);
                }
            }
        }
        else if (strcmp("VMKERNEL", unixUname.sysname) == 0)
        {
            versionNumber->osVersioningIdentifier = OS_ESX;
            if (EOF == sscanf(unixUname.release, "%"SCNu16".%"SCNu16".%"SCNu16"%*s", &versionNumber->versionType.esxiVersion.majorVersion, &versionNumber->versionType.esxiVersion.minorVersion, &versionNumber->versionType.esxiVersion.revision))
            {
                ret = FAILURE;
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "Unknown ESXi Version");
                }
            }
            else
            {
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE,  "ESXi %s", unixUname.release);
                }
            }
        }
        else //don't know what unix this is so return not supported
        {
#if defined(_DEBUG)
            printf("sysname : %s version : %s release : %s \n", unixUname.sysname, unixUname.version, unixUname.release);
#endif
            ret = NOT_SUPPORTED;
        }
    }
    else
    {
        ret = FAILURE;
    }
    return ret;
}

int64_t os_Get_File_Size(FILE *filePtr)
{
    struct stat64 st;
    memset(&st, 0, sizeof(struct stat64));
    if (0 == fstat64(fileno(filePtr), &st))
    {
        return st.st_size;
    }
    else
    {
        return -1;
    }
}

void start_Timer(seatimer_t *timer)
{
    struct timespec startTimespec;
    int ret = 0;
    memset(&startTimespec, 0, sizeof(struct timespec));
    ret = clock_gettime(CLOCK_MONOTONIC, &startTimespec);
    if (0 == ret)//hopefully this always works...-TJE
    {
//        printf("Start Time:  %lu\n", startTimespec.tv_nsec);
        timer->timerStart = C_CAST(uint64_t, startTimespec.tv_sec * UINT64_C(1000000000)) + startTimespec.tv_nsec;
    }
//    else
//    {
//       printf("Bad start_timer Ret:  %d\n",ret);
//    }
}

void stop_Timer(seatimer_t *timer)
{
    struct timespec stopTimespec;
    int ret = 0;
    memset(&stopTimespec, 0, sizeof(struct timespec));
    ret = clock_gettime(CLOCK_MONOTONIC, &stopTimespec);
    if (0 == ret)//hopefully this always works...-TJE
    {
//        printf("Stop Time:  %lu\n", stopTimespec.tv_nsec);
        timer->timerStop = C_CAST(uint64_t, stopTimespec.tv_sec * UINT64_C(1000000000)) + stopTimespec.tv_nsec;
    }
//    else
//    {
//       printf("Bad stop_timer Ret:  %d\n",ret);
//    }
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

bool is_Running_Elevated()
{
    bool isElevated = false;
    if (getuid() == 0 || geteuid() == 0)
    {
        isElevated = true;
    }
    return isElevated;
}

static size_t get_Sys_Username_Max_Length()
{
    #if defined (_POSIX_VERSION) && _POSIX_VERSION >= 200112L
        //get this in case the system is configured differently
        return sysconf(_SC_LOGIN_NAME_MAX);
    #else
        return 256;//this should be more than big enough. Some searching indicates 32 is usually the default
    #endif
}

//If this is successful, this function allocates enough memory to hold the full user's name for you.
//NOTE: This is static since it will probably only be used here...we may want to  enable this for use elsewhere if we want to print fancy warnings with the user's name
static bool get_User_Name_From_ID(uid_t userID, char **userName)
{
    bool success = false;
    if(userName)
    {
        #if defined _POSIX_C_SOURCE && defined (_POSIX_VERSION) && _POSIX_VERSION >= 200112L
            //use reentrant call instead.
            char *rawBuffer = NULL;
            long pwdSize = -1;
            int error = 0;
            struct passwd userInfoBuf;
            struct passwd *userInfo = NULL;
            #if defined (_SC_GETPW_R_SIZE_MAX)
                pwdSize = sysconf(_SC_GETPW_R_SIZE_MAX);
            #endif
            if(pwdSize == -1)
            {
                //some linux man pages suggest 16384
                pwdSize = 1024;//start with this, will increment it below if it fails to read
            }
            rawBuffer = C_CAST(char*, calloc(pwdSize, sizeof(char)));
            if(rawBuffer)
            {
                while (ERANGE == (error = getpwuid_r(userID, &userInfoBuf, rawBuffer, pwdSize, &userInfo)))
                {
                    //this means there was not enough memory allocated in order to read this.
                    //realloc with more memory and try again
                    char *temp = NULL;
                    pwdSize *= 2;
                    temp = realloc(rawBuffer, pwdSize);
                    if(!temp)
                    {
                        safe_Free(rawBuffer)
                        break;
                    }
                    rawBuffer = temp;
                    memset(rawBuffer, 0, pwdSize);
                }
                if(error == 0 && userInfo && rawBuffer)
                {
                    //success
                    size_t userNameLength = strlen(userInfo->pw_name);
                    if(userNameLength > 0 && userNameLength <= get_Sys_Username_Max_Length())//make sure userNameLength is valid and not too large
                    {
                        *userName = C_CAST(char*, calloc(userNameLength + 1, sizeof(char)));//add 1 to ensure room for NULL termination
                        if(*userName)
                        {
                            snprintf(*userName, userNameLength + 1, "%s", userInfo->pw_name);
                            success = true;
                        }
                    }
                }
            }            
        #else
            struct passwd *userInfo = getpwuid(userID);
            if(userInfo)
            {
                size_t userNameLength = strlen(userInfo->pw_name);
                if(userNameLength > 0 && userNameLength <= get_Sys_Username_Max_Length())//make sure userNameLength is valid and not too large
                {
                    *userName = C_CAST(char*, calloc(userNameLength + 1, sizeof(char)));//add 1 to ensure room for NULL termination
                    if(*userName)
                    {
                        snprintf(*userName, userNameLength + 1, "%s", userInfo->pw_name);
                        success = true;
                    }
                }
            }
        #endif
    }
    return success;
}
//Gets the user name for who is running the process
int get_Current_User_Name(char **userName)
{
    int ret = SUCCESS;
    if (userName)
    {
        if (!get_User_Name_From_ID(getuid(), userName))
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

