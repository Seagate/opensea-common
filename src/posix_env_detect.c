// SPDX-License-Identifier: MPL-2.0

//! \file posix_env_detect.c
//! \brief POSIX specific environment detection functions/implementations for env_detect.h
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "common_types.h"
#include "io_utils.h"
#include "memory_safety.h"
#include "secure_file.h"
#include "string_utils.h"
#include "type_conversion.h"

#include <ctype.h>
#include <dirent.h> //for scan dir in linux to get os name. We can move ifdef this if it doesn't work for other OS's
#include <grp.h>
#include <libgen.h>
#include <pwd.h>
#include <string.h>
#include <sys/param.h>
#include <sys/utsname.h>
#include <unistd.h>

static int lin_file_filter(const struct dirent* entry, const char* stringMatch)
{
    int    match          = 0;
    size_t filenameLength = safe_strlen(entry->d_name) + safe_strlen("/etc/") + 1;
    char*  filename       = M_REINTERPRET_CAST(char*, safe_calloc(filenameLength, sizeof(char)));
    if (filename)
    {
        struct stat s;
        snprintf(filename, filenameLength, "/etc/%s", entry->d_name);
        if (stat(filename, &s) == 0)
        {
            if (S_ISREG(s.st_mode)) // must be a file. TODO: are links ok? I
                                    // don't think we need them, but may need to
                                    // revisit this.
            {
                // non-zero means valid match. zero means not a match
                char* inString = strstr(entry->d_name, stringMatch);
                if (inString)
                {
                    // found a file!
                    // make sure the string to match is at the end of the
                    // entry's name!
                    size_t nameLen  = safe_strlen(entry->d_name);
                    size_t matchLen = safe_strlen(stringMatch);
                    if (0 == strncmp(entry->d_name + nameLen - matchLen, stringMatch, matchLen))
                    {
                        match = 1;
                    }
                }
            }
        }
        safe_free(&filename);
    }
    return match;
}

static int release_file_filter(const struct dirent* entry)
{
    // looks like almost all linux specific
    // releases put it as -release
    return lin_file_filter(entry, "-release");
}

static int version_file_filter(const struct dirent* entry)
{
    // most, but not all do a _version, but some do a -, so this
    // should work for both
    return lin_file_filter(entry, "version");
}

// This is a simple function intended to resolve Clang-tidy warnings.
// All it does is cast the result to NULL.
// This is ok in this case as we will not ever be reusing this FILE pointer.
static M_INLINE void close_lin_release_file(FILE* release)
{
    M_STATIC_CAST(void, fclose(release));
}

static bool get_Linux_Info_From_OS_Release_File(char* operatingSystemName)
{
    bool gotLinuxInfo = false;
    if (operatingSystemName)
    {
        bool etcRelease    = os_File_Exists("/etc/os-release");
        bool usrLibRelease = os_File_Exists("/usr/lib/os-release");
        if (etcRelease || usrLibRelease) // available on newer OS's
        {
#define RELEASE_FILE_NAME_LENGTH 21
            DECLARE_ZERO_INIT_ARRAY(char, releasefile, RELEASE_FILE_NAME_LENGTH);
            // read this file and get the linux name
            FILE*   release   = M_NULLPTR;
            errno_t fileerror = 0;
            if (etcRelease)
            {
                fileerror = safe_fopen(&release, "/etc/os-release", "r");
                snprintf(releasefile, RELEASE_FILE_NAME_LENGTH, "/etc/os-release");
            }
            else
            {
                fileerror = safe_fopen(&release, "/usr/lib/os-release", "r");
                snprintf(releasefile, RELEASE_FILE_NAME_LENGTH, "/usr/lib/os-release");
            }
            if (fileerror == 0 && release)
            {
                int         releasefileno = fileno(release);
                struct stat releasestat;
                safe_memset(&releasestat, sizeof(struct stat), 0, sizeof(struct stat));
                if (releasefileno >= 0 && 0 == fstat(releasefileno, &releasestat))
                {
                    off_t releaseSize = releasestat.st_size;
                    if (releaseSize > 0)
                    {
                        char* releaseMemory =
                            M_REINTERPRET_CAST(char*, safe_calloc(C_CAST(size_t, releaseSize), sizeof(char)));
                        if (fread(releaseMemory, sizeof(char), C_CAST(size_t, releaseSize), release) ==
                                C_CAST(size_t, releaseSize) &&
                            !ferror(release))
                        {
                            // Use the "PRETTY_NAME" field
                            char*   saveptr    = M_NULLPTR;
                            rsize_t releaselen = safe_strlen(releaseMemory);
                            char*   tok        = safe_String_Token(releaseMemory, &releaselen, "\n", &saveptr);
                            while (tok != M_NULLPTR)
                            {
                                if (strncmp(tok, "PRETTY_NAME=", safe_strlen("PRETTY_NAME=")) == 0)
                                {
                                    gotLinuxInfo = true;
                                    snprintf(&operatingSystemName[0], OS_NAME_SIZE, "%.*s",
                                             C_CAST(int, safe_strlen(tok) - 1 - safe_strlen("PRETTY_NAME=\"")),
                                             tok + safe_strlen("PRETTY_NAME=\""));
                                    break;
                                }
                                tok = safe_String_Token(M_NULLPTR, &releaselen, "\n", &saveptr);
                            }
                        }
                        safe_free(&releaseMemory);
                    }
                }
                close_lin_release_file(release);
            }
        }
    }
    return gotLinuxInfo;
}

// this returns the file's memory to review.
// since it is heap allocated, it must be free'd by the caller
// since this is an internal static function, not need for separate function to
// call free.
static char* read_Linux_etc_File_For_OS_Info(char* dirent_entry_name)
{
    char* etcFileMem = M_NULLPTR;
    if (dirent_entry_name)
    {
        size_t  fileNameLength = safe_strlen(dirent_entry_name) + safe_strlen("/etc/") + 1;
        char*   fileName       = M_REINTERPRET_CAST(char*, safe_calloc(fileNameLength, sizeof(char)));
        FILE*   release        = M_NULLPTR;
        errno_t fileopenerr    = 0;
        if (fileName)
        {
            snprintf(fileName, fileNameLength, "/etc/%s", dirent_entry_name);
            fileopenerr = safe_fopen(&release, fileName, "r");
            if (fileopenerr == 0 && release)
            {
                int         releaseFileno = fileno(release);
                struct stat direntfilestat;
                safe_memset(&direntfilestat, sizeof(struct stat), 0, sizeof(struct stat));
                if (releaseFileno >= 0 && 0 == fstat(releaseFileno, &direntfilestat))
                {
                    // read it
                    off_t releaseSize = direntfilestat.st_size;
                    if (releaseSize > 0)
                    {
                        etcFileMem = M_REINTERPRET_CAST(char*, safe_calloc(C_CAST(size_t, releaseSize), sizeof(char)));
                        if (etcFileMem)
                        {
                            if (fread(etcFileMem, sizeof(char), C_CAST(size_t, releaseSize), release) !=
                                    C_CAST(size_t, releaseSize) ||
                                ferror(release))
                            {
                                // some error occurred reading the file, so free
                                // this memory to return a checkable null
                                // pointer.
                                safe_free(&etcFileMem);
                            }
                        }
                    }
                    close_lin_release_file(release);
                }
            }
            safe_free(&fileName);
        }
    }
    return etcFileMem;
}

static bool get_Linux_Info_From_Distribution_Specific_Files(char* operatingSystemName)
{
    bool gotLinuxInfo = false;
    if (operatingSystemName)
    {
        // try other release files before /etc/issue. More are here than are
        // implemented: http://linuxmafia.com/faq/Admin/release-files.html This
        // may not cover all of the possible Linux's, but it should get most of
        // them. We'll check for the most common ways of specifying the file to
        // keep this simpler. NOTE: This may also work on some other unix
        // systems, so we may want to try it on those as well.
        struct dirent** osrelease; // most use a file named *-release
        struct dirent** osversion; // some use a file named *_version
        int             releaseFileCount = scandir("/etc", &osrelease, release_file_filter, alphasort);
        int             versionFileCount = scandir("/etc", &osversion, version_file_filter, alphasort);
        int             lsbReleaseOffset = -1; //-1 is invalid.
        if (releaseFileCount > 0)              // ideally this will only ever be 1
        {
            for (int releaseIter = 0; releaseIter < releaseFileCount; ++releaseIter)
            {
                if (0 == strcmp(osrelease[releaseIter]->d_name, "lsb-release"))
                {
                    lsbReleaseOffset = releaseIter;
                    // Don't read this file for version info yet. it's common
                    // across systems but the other release and version files
                    // are more interesting to us
                }
                else
                {
                    char* releaseFile = read_Linux_etc_File_For_OS_Info(osrelease[releaseIter]->d_name);
                    if (releaseFile)
                    {
                        gotLinuxInfo = true;
                        if (operatingSystemName)
                        {
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "%.*s",
                                     C_CAST(int, safe_strlen(releaseFile)), releaseFile);
                        }
                        safe_free(&releaseFile);
                    }
                }
                if (gotLinuxInfo)
                {
                    // we got a name, so break out of the loop (in case there
                    // are multiple files, we only need to read one)
                    break;
                }
            }
        }
        if (!gotLinuxInfo && versionFileCount > 0) // ideally this will only ever be 1
        {
            // For now, only reading the first entry...this SHOULD be ok.
            char* versionFile = read_Linux_etc_File_For_OS_Info(osversion[0]->d_name);
            if (versionFile)
            {
                gotLinuxInfo = true;
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE, "%.*s", C_CAST(int, safe_strlen(versionFile)),
                             versionFile);
                }
                safe_free(&versionFile);
            }
        }
        if (!gotLinuxInfo && lsbReleaseOffset >= 0)
        {
            // this case means that we found the lbs-release file, but haven't
            // read it yet because we kept searching for other files to use for
            // version information first. So now we need to read it for the
            // version information. We use this last because it may not contain
            // something as friendly and useful as we would like that the other
            // version files provide.
            char* lsbreleaseFile = read_Linux_etc_File_For_OS_Info(osrelease[lsbReleaseOffset]->d_name);
            if (lsbreleaseFile)
            {
                gotLinuxInfo = true;
                if (operatingSystemName)
                {
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE, "%.*s", C_CAST(int, safe_strlen(lsbreleaseFile)),
                             lsbreleaseFile);
                }
                safe_free(&lsbreleaseFile);
            }
        }
        // clean up the memory allocated for the list of version files and
        // release files.
        for (int iter = 0; iter < releaseFileCount; ++iter)
        {
            safe_free_dirent(&osrelease[iter]);
        }
        for (int iter = 0; iter < versionFileCount; ++iter)
        {
            safe_free_dirent(&osversion[iter]);
        }
        safe_free_dirent(osrelease);
        safe_free_dirent(osversion);
        if (gotLinuxInfo)
        {
            // remove any control characters from the string. We don't need them
            // for what we're doing
            for (size_t iter = SIZE_T_C(0); iter < safe_strlen(operatingSystemName); ++iter)
            {
                if (safe_iscntrl(operatingSystemName[iter]))
                {
                    operatingSystemName[iter] = ' ';
                }
            }
            // set the null terminator at the end to make sure it's there and
            // not removed in the loop above
            operatingSystemName[OS_NAME_SIZE - 1] = '\0';
        }
    }
    return gotLinuxInfo;
}

static bool get_Linux_Info_From_ETC_Issue(char* operatingSystemName)
{
    bool gotLinuxInfo = false;
    if (operatingSystemName)
    {
        // read this file and get the linux name
        FILE*   issue       = M_NULLPTR;
        errno_t fileopenerr = safe_fopen(&issue, "/etc/issue", "r");
        if (fileopenerr == 0 && issue)
        {
            int         issueFileno = fileno(issue);
            struct stat issuestat;
            safe_memset(&issuestat, sizeof(struct stat), 0, sizeof(struct stat));
            if (issueFileno >= 0 && 0 == fstat(issueFileno, &issuestat))
            {
                // read it
                off_t issueSize = issuestat.st_size;
                if (issueSize > 0)
                {
                    char* issueMemory = M_REINTERPRET_CAST(char*, safe_calloc(C_CAST(size_t, issueSize), sizeof(char)));
                    if (issueMemory)
                    {
                        if (fread(issueMemory, sizeof(char), C_CAST(size_t, issueSize), issue) ==
                                C_CAST(size_t, issueSize) &&
                            !ferror(issue))
                        {
                            gotLinuxInfo = true;
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "%.*s", C_CAST(int, issueSize),
                                     issueMemory);
                        }
                        safe_free(&issueMemory);
                    }
                }
                close_lin_release_file(issue);
            }
        }
    }
    return gotLinuxInfo;
}

// This function is meant to be used to parse the version numbers from the uname
// strings. NOTE: not all systems use all versions....this will only parse until
// the last valid vX version pointer.
//       Assumes all version numbers are in base 10. If we run into a system
//       using a different base, this will need to become a parameter-TJE this
//       is somewhat generic to work on any passed in into. \param verStr = the
//       version string to parse. This must be null terminated! \param prefix =
//       if the system has a prefix before the version starts, supply it here.
//       Can be M_NULLPTR. If provided, must be a null terminated string \param
//       validVerSeperators = each char in this str is considered a valid
//       character to seperate the version numbers. This is required. Any
//       character found not in this list will trigger a failure \param v1 =
//       first version number found to fill \param v2 = second version number
//       found to fill \param v3 = third version number found to fill \param v4
//       = fourth version number found to fill
static bool get_Version_From_Uname_Str(const char* verStr,
                                       const char* prefix /*optional*/,
                                       const char* validVerSeperators,
                                       uint16_t*   versions,
                                       uint16_t    versionCount)
{
    if (verStr && validVerSeperators && versions && versionCount > 0)
    {
        bool  success = true;
        char* endptr  = M_NULLPTR;
        char* strscan = M_CONST_CAST(char*,
                                     verStr); // removing const in order to allow changing strscan
                                              // pointer as we iterate through the string.
        uint16_t versionoffset = UINT16_C(0);
        size_t   verseplen     = safe_strlen(validVerSeperators);
        if (prefix)
        {
            // move the pointer past the prefix.
            strscan += safe_strlen(prefix);
        }
        while (success && versionoffset < versionCount && strscan)
        {
            unsigned long value = 0UL;
            if (0 != safe_strtoul(&value, strscan, &endptr, BASE_10_DECIMAL))
            {
                // When this happens, the value read is zero and the conversion
                // failed returning a zero. Most likely a prefix was missing to
                // move past above.
                success = false;
                break;
            }
            else if (value > UINT16_MAX)
            {
                // This expects all version numbers to be uint16_t, so if this
                // is above that limit, return a failure
                errno   = ERANGE;
                success = false;
                break;
            }
            else
            {
                // A valid value was returned, so we can set this and return it
                versions[versionoffset] = C_CAST(uint16_t, value);
                versionoffset += 1;
                strscan = endptr;
                // now check if endptr is at the end of the string, or is a
                // valid version seperator to move past
                if (endptr)
                {
                    size_t iter     = SIZE_T_C(0);
                    bool   validsep = false;
                    if (strcmp(endptr, "") == 0)
                    {
                        // end of the string
                        break;
                    }
                    for (; iter < verseplen; iter += 1)
                    {
                        // check if this is a valid seperator from the list.
                        // If not, set an error
                        if (endptr[0] == validVerSeperators[iter])
                        {
                            validsep = true;
                            break;
                        }
                    }
                    if (!validsep)
                    {
                        success = false;
                        break;
                    }
                    else
                    {
                        // move past the seperator. There should only be 1 at a
                        // time with how version numbers work
                        strscan += 1;
                    }
                }
                else
                {
                    // no longer valid.
                    break;
                }
            }
        }
        return success;
    }
    else
    {
        return false;
    }
}

static eReturnValues get_Linux_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                            char*              operatingSystemName,
                                            struct utsname*    unixUname)
{
    eReturnValues ret              = SUCCESS;
    bool          linuxOSNameFound = false;
    uint8_t       linuxOSInfoCount = UINT8_C(0);
    DECLARE_ZERO_INIT_ARRAY(uint16_t, list, 4);
#define LINUX_OS_INFO_COUNT_MAX_METHODS_TO_ATTEMPT (3)
    versionNumber->osVersioningIdentifier = OS_LINUX;
    // linux kernels are versioned as
    // kernel.major.minor-securityAndBugFixes-SomeString older linux
    // kernels don't have the -securityAndBugFixes on the end
    if (get_Version_From_Uname_Str(unixUname->release, M_NULLPTR, ".-", list, 4))
    {
        versionNumber->versionType.linuxVersion.kernelVersion             = list[0];
        versionNumber->versionType.linuxVersion.majorVersion              = list[1];
        versionNumber->versionType.linuxVersion.minorVersion              = list[2];
        versionNumber->versionType.linuxVersion.securityAndBugFixesNumber = list[3];
    }
    else
    {
        // retry in case of old kernel
        if (get_Version_From_Uname_Str(unixUname->release, M_NULLPTR, ".-", list, 3))
        {
            versionNumber->versionType.linuxVersion.kernelVersion             = list[0];
            versionNumber->versionType.linuxVersion.majorVersion              = list[1];
            versionNumber->versionType.linuxVersion.minorVersion              = list[2];
            versionNumber->versionType.linuxVersion.securityAndBugFixesNumber = 0;
        }
        else
        {
            ret = FAILURE;
        }
    }
    while (!linuxOSNameFound && linuxOSInfoCount < LINUX_OS_INFO_COUNT_MAX_METHODS_TO_ATTEMPT)
    {
        switch (linuxOSInfoCount)
        {
        case 0:
            linuxOSNameFound = get_Linux_Info_From_OS_Release_File(operatingSystemName);
            break;
        case 1:
            linuxOSNameFound = get_Linux_Info_From_Distribution_Specific_Files(operatingSystemName);
            break;
        case 2:
            linuxOSNameFound = get_Linux_Info_From_ETC_Issue(operatingSystemName);
            break;
        }
        ++linuxOSInfoCount;
    }
    // if we couldn't find a name, set unknown linux os
    if (!linuxOSNameFound && operatingSystemName)
    {
        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Linux OS");
    }
    return ret;
}

static eReturnValues get_FreeBSD_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                              char*              operatingSystemName,
                                              struct utsname*    unixUname)
{
    eReturnValues ret = SUCCESS;
    DECLARE_ZERO_INIT_ARRAY(uint16_t, list, 2);
    versionNumber->osVersioningIdentifier = OS_FREEBSD;
    // FreeBSD version is stored as Major.Minor-SomeString
    if (get_Version_From_Uname_Str(unixUname->release, M_NULLPTR, ".", list, 2))
    {
        versionNumber->versionType.freeBSDVersion.majorVersion = list[0];
        versionNumber->versionType.freeBSDVersion.minorVersion = list[1];
    }
    else
    {
        ret = FAILURE;
    }
    // set the OS Name
    if (operatingSystemName)
    {
        if (ret != FAILURE)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "FreeBSD %" PRIu16 ".%" PRIu16 "",
                     versionNumber->versionType.freeBSDVersion.majorVersion,
                     versionNumber->versionType.freeBSDVersion.minorVersion);
        }
        else
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown FreeBSD OS Version");
        }
    }
    return ret;
}

static eReturnValues get_SunOS_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                            char*              operatingSystemName,
                                            struct utsname*    unixUname)
{
    eReturnValues ret = SUCCESS;
    DECLARE_ZERO_INIT_ARRAY(uint16_t, list, 3);
    versionNumber->osVersioningIdentifier = OS_SOLARIS;
    // Solaris stores the SunOS version in release
    if (get_Version_From_Uname_Str(unixUname->release, M_NULLPTR, ".", list, 3))
    {
        versionNumber->versionType.solarisVersion.sunOSMajorVersion = list[0];
        versionNumber->versionType.solarisVersion.sunOSMinorVersion = list[1];
        versionNumber->versionType.solarisVersion.sunOSRevision     = list[2];
    }
    else
    {
        ret = FAILURE;
    }
    // set OS name as Solaris "unixUname.version" for the OS Name
    if (operatingSystemName)
    {
        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Solaris %s", unixUname->version);
    }
    // The Solaris Version/name is stored in version
    if (safe_strlen(unixUname->version) > 0 && safe_isdigit(unixUname->version[0]))
    {
        // set OS name as Solaris x.x
        if (get_Version_From_Uname_Str(unixUname->version, M_NULLPTR, ".", list, 3))
        {
            versionNumber->versionType.solarisVersion.solarisMajorVersion = list[0];
            versionNumber->versionType.solarisVersion.solarisMinorVersion = list[1];
            versionNumber->versionType.solarisVersion.solarisRevision     = list[2];
        }
    }
    return ret;
}

static eReturnValues get_Darwin_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                             char*              operatingSystemName,
                                             struct utsname*    unixUname)
{
    eReturnValues ret = SUCCESS;
    DECLARE_ZERO_INIT_ARRAY(uint16_t, list, 3);
    versionNumber->osVersioningIdentifier = OS_MACOSX;
    if (get_Version_From_Uname_Str(unixUname->release, M_NULLPTR, ".", list, 3))
    {
        versionNumber->versionType.macOSVersion.majorVersion = list[0];
        versionNumber->versionType.macOSVersion.minorVersion = list[1];
        versionNumber->versionType.macOSVersion.revision     = list[2];
    }
    else
    {
        ret = FAILURE;
    }
    // set the OS Name from the major numbers starting with "Puma"
    // https://en.wikipedia.org/wiki/Darwin_(operating_system)
    if (operatingSystemName)
    {
        switch (versionNumber->versionType.macOSVersion.majorVersion)
        {
        case 5: // puma
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Mac OS X 10.1 Puma");
            break;
        case 6: // jaguar
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Mac OS X 10.2 Jaguar");
            break;
        case 7: // panther
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Mac OS X 10.3 Panther");
            break;
        case 8: // tiger
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Mac OS X 10.4 Tiger");
            break;
        case 9: // leopard
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Mac OS X 10.5 Leopard");
            break;
        case 10: // snow leopard
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Mac OS X 10.6 Snow Leopard");
            break;
        case 11: // lion
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Mac OS X 10.7 Lion");
            break;
        case 12: // Mountain Lion
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "OS X 10.8 Mountain Lion");
            break;
        case 13: // mavericks
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "OS X 10.9 Mavericks");
            break;
        case 14: // Yosemite
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "OS X 10.10 Yosemite");
            break;
        case 15: // el capitan
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "OS X 10.11 El Capitan");
            break;
        default:
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Mac OS X Version");
            break;
        }
    }
    return ret;
}

static eReturnValues get_AIX_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                          char*              operatingSystemName,
                                          struct utsname*    unixUname)
{
    eReturnValues ret                     = SUCCESS;
    unsigned long temp                    = 0UL;
    versionNumber->osVersioningIdentifier = OS_AIX;
    if (0 == safe_strtoul(&temp, unixUname->version, M_NULLPTR, BASE_10_DECIMAL))
    {
        versionNumber->versionType.aixVersion.majorVersion = M_STATIC_CAST(uint16_t, temp);
    }
    if (0 == safe_strtoul(&temp, unixUname->version, M_NULLPTR, BASE_10_DECIMAL))
    {
        versionNumber->versionType.aixVersion.minorVersion = M_STATIC_CAST(uint16_t, temp);
    }
    if (operatingSystemName)
    {
        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "AIX %" PRIu16 ".%" PRIu16 "",
                 versionNumber->versionType.aixVersion.majorVersion,
                 versionNumber->versionType.aixVersion.minorVersion);
    }
    return ret;
}

static eReturnValues get_Dragonfly_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                                char*              operatingSystemName,
                                                struct utsname*    unixUname)
{
    eReturnValues ret = SUCCESS;
    DECLARE_ZERO_INIT_ARRAY(uint16_t, list, 2);
    versionNumber->osVersioningIdentifier = OS_DRAGONFLYBSD;
    if (get_Version_From_Uname_Str(unixUname->release, M_NULLPTR, ".", list, 2))
    {
        versionNumber->versionType.dragonflyVersion.majorVersion = list[0];
        versionNumber->versionType.dragonflyVersion.minorVersion = list[1];
        if (operatingSystemName)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Dragonfly BSD %" PRIu16 ".%" PRIu16 "",
                     versionNumber->versionType.dragonflyVersion.majorVersion,
                     versionNumber->versionType.dragonflyVersion.minorVersion);
        }
    }
    else
    {
        ret = FAILURE;
        if (operatingSystemName)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Dragonfly BSD Version");
        }
    }
    return ret;
}

static eReturnValues get_OpenBSD_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                              char*              operatingSystemName,
                                              struct utsname*    unixUname)
{
    eReturnValues ret                     = SUCCESS;
    unsigned long temp                    = 0UL;
    versionNumber->osVersioningIdentifier = OS_OPENBSD;
    if (0 == safe_strtoul(&temp, unixUname->version, M_NULLPTR, BASE_10_DECIMAL))
    {
        versionNumber->versionType.openBSDVersion.majorVersion = M_STATIC_CAST(uint16_t, temp);
    }
    if (0 == safe_strtoul(&temp, unixUname->version, M_NULLPTR, BASE_10_DECIMAL))
    {
        versionNumber->versionType.openBSDVersion.minorVersion = M_STATIC_CAST(uint16_t, temp);
    }
    if (operatingSystemName)
    {
        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "OpenBSD %" PRIu16 ".%" PRIu16 "",
                 versionNumber->versionType.openBSDVersion.majorVersion,
                 versionNumber->versionType.openBSDVersion.minorVersion);
    }
    return ret;
}

static eReturnValues get_NetBSD_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                             char*              operatingSystemName,
                                             struct utsname*    unixUname)
{
    eReturnValues ret = SUCCESS;
    DECLARE_ZERO_INIT_ARRAY(uint16_t, list, 3);
    versionNumber->osVersioningIdentifier = OS_NETBSD;
    if (get_Version_From_Uname_Str(unixUname->release, M_NULLPTR, ".", list, 3))
    {
        versionNumber->versionType.netBSDVersion.majorVersion = list[0];
        versionNumber->versionType.netBSDVersion.minorVersion = list[1];
        versionNumber->versionType.netBSDVersion.revision     = list[2];
        if (operatingSystemName)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "NetBSD %s", unixUname->release);
        }
    }
    else
    {
        ret = FAILURE;
        if (operatingSystemName)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown NetBSD Version");
        }
    }
    return ret;
}

static eReturnValues get_OSF1_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                           char*              operatingSystemName,
                                           struct utsname*    unixUname)
{
    eReturnValues ret = SUCCESS;
    DECLARE_ZERO_INIT_ARRAY(uint16_t, list, 2);
    versionNumber->osVersioningIdentifier = OS_TRU64;
    if (get_Version_From_Uname_Str(unixUname->release, "V", ".", list, 2))
    {
        versionNumber->versionType.tru64Version.majorVersion = list[0];
        versionNumber->versionType.tru64Version.minorVersion = list[1];
        if (operatingSystemName)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Tru64 %s", unixUname->release);
        }
    }
    else
    {
        ret = FAILURE;
        if (operatingSystemName)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Tru64 Version");
        }
    }
    return ret;
}

static eReturnValues get_HPUX_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                           char*              operatingSystemName,
                                           struct utsname*    unixUname)
{
    eReturnValues ret = SUCCESS;
    DECLARE_ZERO_INIT_ARRAY(uint16_t, list, 2);
    versionNumber->osVersioningIdentifier = OS_HPUX;
    if (get_Version_From_Uname_Str(unixUname->release, "B.", ".", list, 2))
    {
        versionNumber->versionType.hpuxVersion.majorVersion = list[0];
        versionNumber->versionType.hpuxVersion.minorVersion = list[1];
        if (operatingSystemName)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "HP-UX %" PRIu16 ".%" PRIu16 "",
                     versionNumber->versionType.hpuxVersion.majorVersion,
                     versionNumber->versionType.hpuxVersion.minorVersion);
        }
    }
    else
    {
        ret = FAILURE;
        if (operatingSystemName)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown HP-UX Version");
        }
    }
    return ret;
}

static eReturnValues get_VMKernel_Ver_And_Name(ptrOSVersionNumber versionNumber,
                                               char*              operatingSystemName,
                                               struct utsname*    unixUname)
{
    eReturnValues ret = SUCCESS;
    DECLARE_ZERO_INIT_ARRAY(uint16_t, list, 3);
    versionNumber->osVersioningIdentifier = OS_ESX;
    if (get_Version_From_Uname_Str(unixUname->release, M_NULLPTR, ".", list, 3))
    {
        versionNumber->versionType.esxiVersion.majorVersion = list[0];
        versionNumber->versionType.esxiVersion.minorVersion = list[1];
        versionNumber->versionType.esxiVersion.revision     = list[2];
        if (operatingSystemName)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "ESXi %s", unixUname->release);
        }
    }
    else
    {
        ret = FAILURE;
        if (operatingSystemName)
        {
            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown ESXi Version");
        }
    }
    return ret;
}

// code is written based on the table in this link
// https://en.wikipedia.org/wiki/Uname This code is not complete for all
// operating systems. I only added in support for the ones we are most
// interested in or are already using today. -TJE z/OS:
// https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-uname-display-current-operating-system-name
//   and
//   https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-osname-get-true-operating-system-name
eReturnValues get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char* operatingSystemName)
{
    eReturnValues ret = SUCCESS;
    // check what is filled in by the uname call in the utsname struct (BUT
    // DON"T CHECK THE DOMAIN NAME since that is GNU specific)
    struct utsname unixUname;
    safe_memset(&unixUname, sizeof(struct utsname), 0, sizeof(struct utsname));
    if (0 == uname(&unixUname))
    {
        if (strcasecmp("Linux", unixUname.sysname) == 0) // some OSs like Android or ESX may fall into here, so this
                                                         // section may need expansion if that happens
        {
            ret = get_Linux_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else if (strcasecmp("FreeBSD", unixUname.sysname) == 0)
        {
            ret = get_FreeBSD_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else if (strcasecmp("SunOS", unixUname.sysname) == 0)
        {
            ret = get_SunOS_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else if (strcasecmp("DARWIN", unixUname.sysname) == 0) // Mac OSX
        {
            ret = get_Darwin_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else if (strcasecmp("AIX", unixUname.sysname) == 0)
        {
            ret = get_AIX_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else if (strcasecmp("DRAGONFLY", unixUname.sysname) == 0)
        {
            ret = get_Dragonfly_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else if (strcasecmp("OPENBSD", unixUname.sysname) == 0)
        {
            ret = get_OpenBSD_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else if (strcasecmp("NETBSD", unixUname.sysname) == 0)
        {
            ret = get_NetBSD_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else if (strcasecmp("OSF1", unixUname.sysname) == 0)
        {
            ret = get_OSF1_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else if (strcasecmp("HP-UX", unixUname.sysname) == 0)
        {
            ret = get_HPUX_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else if (strcasecmp("VMKERNEL", unixUname.sysname) == 0)
        {
            ret = get_VMKernel_Ver_And_Name(versionNumber, operatingSystemName, &unixUname);
        }
        else // don't know what unix this is so return not supported
        {
#if defined(_DEBUG)
            printf("sysname : %s version : %s release : %s \n", unixUname.sysname, unixUname.version,
                   unixUname.release);
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

bool is_Running_Elevated(void)
{
    bool isElevated = false;
    if (getuid() == ROOT_UID_VAL || geteuid() == ROOT_UID_VAL)
    {
        isElevated = true;
    }
    return isElevated;
}

#if defined(ENABLE_READ_USERNAME)
static size_t get_Sys_Username_Max_Length(void)
{
#    if defined(POSIX_2001)
    // get this in case the system is configured differently
    return sysconf(_SC_LOGIN_NAME_MAX);
#    else
    return 256; // this should be more than big enough. Some searching indicates
                // 32 is usually the default
#    endif
}

// If this is successful, this function allocates enough memory to hold the full
// user's name for you. NOTE: This is static since it will probably only be used
// here...we may want to  enable this for use elsewhere if we want to print
// fancy warnings with the user's name
static bool get_User_Name_From_ID(uid_t userID, char** userName)
{
    bool success = false;
    if (userName)
    {
        if (userID == ROOT_UID_VAL)
        {
            if (0 == safe_strdup(userName, "root"))
            {
                success = true;
            }
        }
        else
        {
#    if defined(POSIX_2001) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
            // use reentrant call instead.
            char*          rawBuffer = M_NULLPTR;
            long           dataSize  = -1L;
            int            error     = 0;
            struct passwd  userInfoBuf;
            struct passwd* userInfo = M_NULLPTR;
#        if defined(_SC_GETPW_R_SIZE_MAX)
            dataSize = sysconf(_SC_GETPW_R_SIZE_MAX);
#        endif
            if (dataSize == -1L)
            {
                // some linux man pages suggest 16384
                dataSize = 1024L; // start with this, will increment it below if
                                  // it fails to read
            }
            rawBuffer = M_REINTERPRET_CAST(char*, safe_calloc(dataSize, sizeof(char)));
            if (rawBuffer)
            {
                while (ERANGE == (error = getpwuid_r(userID, &userInfoBuf, rawBuffer, dataSize, &userInfo)))
                {
                    // this means there was not enough memory allocated in order
                    // to read this. realloc with more memory and try again
                    char* temp = M_NULLPTR;
                    dataSize *= 2L;
                    temp = safe_realloc(rawBuffer, dataSize);
                    if (!temp)
                    {
                        safe_free(&rawBuffer);
                        break;
                    }
                    rawBuffer = temp;
                    safe_memset(rawBuffer, dataSize, 0, dataSize);
                }
                if (error == 0 && userInfo && rawBuffer)
                {
                    // success
                    size_t userNameLength =
                        safe_strlen(userInfo->pw_name) + 1; // add 1 to ensure room for M_NULLPTR termination
                    if (userNameLength > 1 && (userNameLength - 1) <= get_Sys_Username_Max_Length()) // make sure
                                                                                                     // userNameLength
                                                                                                     // is valid and not
                                                                                                     // too large
                    {
                        *userName = M_REINTERPRET_CAST(char*, safe_calloc(userNameLength, sizeof(char)));
                        if (*userName)
                        {
                            snprintf(*userName, userNameLength, "%s", userInfo->pw_name);
                            success = true;
                        }
                    }
                }
                explicit_zeroes(rawBuffer, dataSize);
                safe_free(&rawBuffer);
            }
            explicit_zeroes(userInfo, sizeof(struct passwd));
#    else  // defined (POSIX_2001) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
            struct passwd* userInfo = getpwuid(userID);
            if (userInfo)
            {
                size_t userNameLength =
                    safe_strlen(userInfo->pw_name) + 1; // add 1 to ensure room for M_NULLPTR termination
                if (userNameLength > 1 && (userNameLength - 1) <= get_Sys_Username_Max_Length()) // make sure
                                                                                                 // userNameLength is
                                                                                                 // valid and not too
                                                                                                 // large
                {
                    *userName = M_REINTERPRET_CAST(char*, safe_calloc(userNameLength, sizeof(char)));
                    if (*userName)
                    {
                        snprintf(*userName, userNameLength, "%s", userInfo->pw_name);
                        success = true;
                    }
                }
            }
            // this should be reasonably safe to do.
            // This structure can be in a static location, so writing zeroes to
            // it might be a good way to make sure this is cleared out after we
            // are done. the docs online say subsequent calls to getpwuid may
            // change it, making this not thread-safe, so I would assume this is
            // ok to do.
            explicit_zeroes(userInfo, sizeof(struct passwd));
#    endif // defined (POSIX_2001) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
        }
    }
    return success;
}
// Gets the user name for who is running the process
eReturnValues get_Current_User_Name(char** userName)
{
    eReturnValues ret = SUCCESS;
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
#endif // ENABLE_READ_USERNAME
