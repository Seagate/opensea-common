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
//
// \file secure_file.h
// \brief Defines functions for working with a file securely according to cert-c
// recommendations.
//        System type unique functionality is available in
//        systemtype_secure_file.c
//

#include "secure_file.h"
#include "common_types.h"
#include "io_utils.h"
#include "memory_safety.h"
#include "string_utils.h"
#include "time_utils.h"
#include "type_conversion.h"

#include <stdarg.h>
#include <string.h>

#if defined(_WIN32)
#    include <direct.h> //getcwd
#else
#    include <unistd.h> //getcwd
#endif

eReturnValues replace_File_Name_In_Path(char fullPath[OPENSEA_PATH_MAX], char* newFileName)
{
    char*  ptr        = strrchr(fullPath, SYSTEM_PATH_SEPARATOR);
    size_t ptrLen     = SIZE_T_C(0);
    size_t fullLength = SIZE_T_C(0);
    if (M_NULLPTR != ptr)
    {
        ptr += 1;
    }
    else
    {
        return FAILURE;
    }
    ptrLen = safe_strlen(ptr);
    // now that we have a valid pointer, set all the remaining characters to
    // null, then set the new file name in place.
    safe_memset(ptr, ptrLen + 1, 0,
                ptrLen); // destsz has + 1 for null terminator not included in strlen.
    fullLength = (OPENSEA_PATH_MAX - safe_strlen(fullPath));
    snprintf(ptr, fullLength, "%s", newFileName);
    return SUCCESS;
}

void free_File_Attributes(fileAttributes** attributes)
{
    if (attributes != M_NULLPTR)
    {
        if (*attributes != M_NULLPTR)
        {
#if defined(_WIN32)
            if ((*attributes)->winSecurityDescriptor)
            {
                explicit_zeroes((*attributes)->winSecurityDescriptor, (*attributes)->securityDescriptorStringLength);
                safe_Free(C_CAST(void**, &(*attributes)->winSecurityDescriptor));
                (*attributes)->securityDescriptorStringLength = 0;
            }
#endif //_WIN32
            explicit_zeroes(*attributes, sizeof(fileAttributes));
            safe_Free(M_REINTERPRET_CAST(void**, attributes));
        }
    }
}

void free_Secure_File_Info(secureFileInfo** fileInfo)
{
    if (fileInfo != M_NULLPTR)
    {
        if (*fileInfo != M_NULLPTR)
        {
            if ((*fileInfo)->attributes != M_NULLPTR)
            {
                free_File_Attributes(&(*fileInfo)->attributes);
                (*fileInfo)->attributes = M_NULLPTR;
            }
            if ((*fileInfo)->uniqueID != M_NULLPTR)
            {
                explicit_zeroes((*fileInfo)->uniqueID, sizeof(fileUniqueIDInfo));
                safe_Free(C_CAST(void**, &(*fileInfo)->uniqueID));
            }
            explicit_zeroes(*fileInfo, sizeof(secureFileInfo));
        }
        safe_Free(C_CAST(void**, fileInfo));
    }
}

// This exists to fix this cert-c identified issue
// https://wiki.sei.cmu.edu/confluence/display/c/EXP42-C.+Do+not+compare+padding+data
bool compare_File_Unique_ID(const fileUniqueIDInfo* a, const fileUniqueIDInfo* b)
{
    bool equal = false;
    if (a != M_NULLPTR && b != M_NULLPTR)
    {
        if (a->inode == b->inode && 0 == memcmp(a->fileid, b->fileid, FILE_UNIQUE_ID_ARR_MAX))
        {
            equal = true;
        }
    }
    return equal;
}

// The purpose of this function is to perform the security validation necessary
// to make sure this is a valid file on the system and minimize path traversal
// and validate permissions as much as reasonably possible. The goal is
// mitigation of https://cwe.mitre.org/data/definitions/22.html Will be using
// recommendations from
// https://wiki.sei.cmu.edu/confluence/pages/viewpage.action?pageId=87151932
//      as much as possible to accomplish this.-TJE
// expectedFileInfo can be M_NULLPTR for the first time opening a file.
//  If reopening a file used earlier, it is recommended to provide this info so
//  it can be validated as the same file It is recommended to not reopen files,
//  but that may not always be possible. So this exists to help validate that a
//  file has not changed in some unexpected way.-TJE
M_NODISCARD secureFileInfo* secure_Open_File(const char*       filename,
                                             const char*       mode,
                                             const fileExt*    extList,
                                             fileAttributes*   expectedFileInfo,
                                             fileUniqueIDInfo* uniqueIdInfo /*optional*/)
{
    secureFileInfo* fileInfo = M_REINTERPRET_CAST(secureFileInfo*, safe_calloc(1, sizeof(secureFileInfo)));
    if (fileInfo && filename && mode) /* Not checking extList and outInfo because it is optional */
    {
        bool  creatingFile  = false;
        bool  exclusiveFlag = false;
        char* internalmode  = M_CONST_CAST(char*, mode); // we will dup mode if we need to modify it later, so
                                                         // this cast is to get rid of a warning
        bool  duplicatedModeForInternalUse = false;
        char* intFileName                  = M_NULLPTR; // allocated/dup'd later
        fileInfo->file                     = M_NULLPTR;
        if (strchr(internalmode, 'w') || strchr(internalmode, 'a'))
        {
            // file is being created and does not exist yet.
            // Get the canonical path to the directory that the file will be in
            creatingFile = true;
        }
        // C11 adds 'x' to mode so that on a write, this forces a failure
        // Since we may not always be in a system with C11 or support for
        // wx/w+x, handle this manually.
        if (strchr(internalmode, 'w') && strchr(internalmode, 'x'))
        {
            exclusiveFlag = true;
#if !defined(USING_C11) && !(defined(_MSC_VER) && _MSC_VER < 1800) &&                                                  \
    !(defined(__GLIBC__) || defined(__GNU_LIBRARY__)) &&                                                               \
    !(defined(USING_MUSL_LIBC) && USING_MUSL_LIBC > 0) // Glibc supports x. VS2013+ support x
            // For systems that do not support 'x' need to adjust the mode
            // manually so that we do not pass an invalid 'x' flag when it is
            // not supported.
            //       While this can always be done regardless of
            //       standard/library, it is better to pass flags onwards when
            //       they are supported for additional verification by the
            //       library/system - TJE
            duplicatedModeForInternalUse = true;
            if (0 != safe_strdup(&internalmode, mode))
            {
                fileInfo->error = SEC_FILE_INVALID_MODE;
                return fileInfo;
            }
            char* thex = strchr(internalmode, 'x');
            if (thex)
            {
                // remove it since it is not supported outside C11 and a few
                // \libraries that use it as an extension
                if (safe_strlen(thex) > 1)
                {
                    size_t lenx = safe_strlen(thex);
                    memmove(thex, thex + 1, lenx - 1);
                    thex[lenx - 1] = '\0';
                }
                else
                {
                    thex[0] = '\0';
                }
            }
#endif //! C11 && !MSVC 2013+ && !GLIBC
        }
        else if (strchr(internalmode, 'x'))
        {
            // invalid mode. X is only allowed with W, W+
            fileInfo->error = SEC_FILE_INVALID_MODE;
            return fileInfo;
        }
        // Get canonical path here before doing anything else???
        if (creatingFile)
        {
            // setup path to the directory to make sure it exists since the file
            // does not yet. NOTE: If we only have the current file name, no
            // relative path, etc, need to call a function to get the current
            // working directory to
            //       use that as the path.
            if (strchr(filename, '/') // always check for forwards slash since
                                      // Windows can accept this
#if defined(_WIN32)
                || strchr(filename,
                          '\\') // only check for backslash on Windows since
                                // this is a common path seperator on Windows
                                // and is the "prefered" format
#endif                          //_WIN32
            )
            {
                // there is a path or relative path that can be used to setup
                // for getting the canonical path
                char* lastsep = strrchr(filename, '/');
#if defined(_WIN32)
                // In Windows, we need to determine if the final seperator is a
                // / or \. So also check for \ and figure out which was the last
                // one. A user can pass a path with both and it can be accepted
                // by Windows, which is why we validate both of these.
                char* lastwinsep = strrchr(filename, '\\');
                if (lastsep == M_NULLPTR || C_CAST(uintptr_t, lastwinsep) > C_CAST(uintptr_t, lastsep))
                {
                    // backslash was detected last, so change to this pointer
                    // instead for strndup
                    lastsep = lastwinsep;
                }
#endif //_WIN32
       // path only. No file name
                if (0 != safe_strndup(&intFileName, filename, C_CAST(uintptr_t, lastsep) - C_CAST(uintptr_t, filename)))
                {
                    fileInfo->error = SEC_FILE_INVALID_PATH;
                    return fileInfo;
                }
            }
            else
            {
                // no path, only the filename itself.
                // get the current working directory to use for the canonical
                // file and path. Will use getcwd to get this information. NOTE:
                // A common extension to getcwd is passing M_NULLPTR will have
                // it allocate and return a buffer.
                //       This seems to exist on Linux, FreeBSD, Windows
                //       (_getcwd), openbsd, netbsd, and solaris...possibly
                //       more. While this is convenient, we are not using this
                //       to make sure behavior is as expected according to POSIX
                // NOTE: Windows can also use
                // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcurrentdirectory
                // need to allocate and call again
                char* workingdir = safe_malloc(OPENSEA_PATH_MAX);
                if (workingdir == M_NULLPTR)
                {
                    // return an error for invalid path
                    fileInfo->error = SEC_FILE_INVALID_PATH;
                    return fileInfo;
                }
                workingdir = getcwd(workingdir, OPENSEA_PATH_MAX);
                if (workingdir == M_NULLPTR)
                {
                    // return an error for invalid path
                    fileInfo->error = SEC_FILE_INVALID_PATH;
                    return fileInfo;
                }
                intFileName = workingdir; // we will free this later when we
                                          // free intfilename
            }
        }
        else
        {
            // not creating a new file, so we will have what we need to get a
            // full canonical path
            if (0 != safe_strdup(&intFileName, filename))
            {
                fileInfo->error = SEC_FILE_INVALID_PATH;
                return fileInfo;
            }
        }
        if (intFileName == M_NULLPTR)
        {
            // return an error for invalid path
            fileInfo->error = SEC_FILE_INVALID_PATH;
            return fileInfo;
        }
        if (SUCCESS != get_Full_Path(intFileName, M_CONST_CAST(char*, fileInfo->fullpath)))
        {
            // unable to get the full path to this file.
            // This means something went wrong, and we need to return an error.
            fileInfo->error = SEC_FILE_INVALID_PATH;
            safe_free(&intFileName);
            return fileInfo;
        }

        bool fileexists = false;
        if (!creatingFile || exclusiveFlag)
        {
            fileexists = os_File_Exists(intFileName);
            if (exclusiveFlag && fileexists)
            {
                fileInfo->error = SEC_FILE_FILE_ALREADY_EXISTS;
                if (duplicatedModeForInternalUse)
                {
                    safe_free(&internalmode);
                }
                safe_free(&intFileName);
                return fileInfo;
            }
            else if (!creatingFile && !fileexists)
            {
                fileInfo->error = SEC_FILE_INVALID_FILE;
                if (duplicatedModeForInternalUse)
                {
                    safe_free(&internalmode);
                }
                safe_free(&intFileName);
                return fileInfo;
            }
        }
        fileAttributes* beforeattrs = M_NULLPTR;
        if (creatingFile)
        {
            // append the filename to the end on fileInfo->fullpath
            // first add a trailing slash since one will not be present
            safe_strcat(M_CONST_CAST(char*, fileInfo->fullpath), OPENSEA_PATH_MAX, SYSTEM_PATH_SEPARATOR_STR);
            if (strchr(filename, '/') // always check for forwards slash since
                                      // Windows can accept this
#if defined(_WIN32)
                || strchr(filename,
                          '\\') // only check for backslash on Windows since
                                // this is a common path seperator on Windows
                                // and is the "prefered" format
#endif                          //_WIN32
            )
            {
                // there is a path or relative path that we do not want to
                // append to the canonical path
                char* lastsep = strrchr(filename, '/');
#if defined(_WIN32)
                // In Windows, we need to determine if the final seperator is a
                // / or \. So also check for \ and figure out which was the last
                // one. A user can pass a path with both and it can be accepted
                // by Windows, which is why we validate both of these.
                char* lastwinsep = strrchr(filename, '\\');
                if (C_CAST(uintptr_t, lastwinsep) > C_CAST(uintptr_t, lastsep))
                {
                    // backslash was detected last, so change to this pointer
                    // instead for strndup
                    lastsep = lastwinsep;
                }
#endif //_WIN32
                safe_strcat(M_CONST_CAST(char*, fileInfo->fullpath), OPENSEA_PATH_MAX, lastsep + 1);
            }
            else
            {
                safe_strcat(M_CONST_CAST(char*, fileInfo->fullpath), OPENSEA_PATH_MAX, filename);
            }
        }
        else
        {
            // Validate the file extension if one was provided to check for
            // since the full path will also resolve the extension.
            //         This check does not guarantee a file is correct since
            //         anyone can rename an extension, but it should help make
            //         sure the file being opened is closer to what is expected
            //         by the caller
            if (extList)
            {
                fileExt* currentExtension = M_CONST_CAST(fileExt*, extList); // we will increment this pointer as
                                                                             // needed, so it cannot be const
                bool foundValidExtension = false;
                while (currentExtension && currentExtension->ext != M_NULLPTR)
                {
                    char* extension = strrchr(fileInfo->fullpath, '.');
                    if (extension &&
                        (strcmp(extension, currentExtension->ext) == 0 ||
                         (currentExtension->caseInsensitive && strcasecmp(extension, currentExtension->ext) == 0)))
                    {
                        // valid extension
                        foundValidExtension = true;
                        break;
                    }
                    currentExtension += 1;
                }
                if (!foundValidExtension)
                {
                    safe_free(&intFileName);
                    if (duplicatedModeForInternalUse)
                    {
                        safe_free(&internalmode);
                    }
                    fileInfo->error = SEC_FILE_INVALID_FILE_EXTENSION;
                    return fileInfo;
                }
            }
            beforeattrs = os_Get_File_Attributes_By_Name(fileInfo->fullpath);
            // Do a first pass on file attribute comparison with the expected
            // file attributes. If no match, return an error This may be done
            // again later after opening the file since some things will not be
            // available without opening the file handle.
            if (expectedFileInfo)
            {
                // check device ID and inode. These are unique on POSIX as a
                // file ID. Windows is not far enough along to get this info
                // yet.
                if ((expectedFileInfo->deviceID != beforeattrs->deviceID) ||
                    (expectedFileInfo->inode != beforeattrs->inode) ||
                    (expectedFileInfo->userID != beforeattrs->userID) ||
                    (expectedFileInfo->groupID != beforeattrs->groupID))
                {
                    fileInfo->error = SEC_FILE_INVALID_FILE_ATTRIBTUES;
                    safe_free(&intFileName);
                    free_File_Attributes(&beforeattrs);
                    if (duplicatedModeForInternalUse)
                    {
                        safe_free(&internalmode);
                    }
                    return fileInfo;
                }
#if defined(_WIN32)
                // Validate Windows SIDs are equivalent
                // Convert the strings back to structures and use EqualSid and
                // EqualAcl to do the comparisons. There may be other formatting
                // in the string that does not affect the permissions which is
                // why the conversion is necessary.
                if (!exact_Compare_SIDS_And_DACL_Strings(beforeattrs->winSecurityDescriptor,
                                                         expectedFileInfo->winSecurityDescriptor))
                {
                    fileInfo->error = SEC_FILE_INVALID_FILE_ATTRIBTUES;
                    safe_free(&intFileName);
                    if (duplicatedModeForInternalUse)
                    {
                        safe_free(&internalmode);
                    }
                    free_File_Attributes(&beforeattrs);
                    return fileInfo;
                }
#endif //_WIN32
            }
        }
        free_File_Attributes(&beforeattrs);

        // Canonical path will already have the proper system path separator in
        // it. No need to search / in Windows
        fileInfo->filename =
            strrchr(fileInfo->fullpath, SYSTEM_PATH_SEPARATOR) + 1; // plus 1 to get past final seperator

        // Need to verify only the path. Passing the file in with it will cause
        // it to fail since it is not a directory
        char* pathOnly               = M_CONST_CAST(char*, fileInfo->fullpath);
        bool  allocatedLocalPathOnly = false;
        char* lastsep                = strrchr(fileInfo->fullpath, '/');
#if defined(_WIN32)
        // In Windows, we need to determine if the final seperator is a / or \.
        // So also check for \ and figure out which was the last one.
        // A user can pass a path with both and it can be accepted by Windows,
        // which is why we validate both of these.
        char* lastwinsep = strrchr(fileInfo->fullpath, '\\');
        if (C_CAST(uintptr_t, lastwinsep) > C_CAST(uintptr_t, lastsep))
        {
            // backslash was detected last, so change to this pointer instead
            // for strndup
            lastsep = lastwinsep;
        }
#endif //_WIN32

        if (safe_strndup(&pathOnly, fileInfo->fullpath,
                         C_CAST(uintptr_t, lastsep) - C_CAST(uintptr_t, fileInfo->fullpath)) == 0 &&
            pathOnly != M_NULLPTR)
        {
            allocatedLocalPathOnly = true;
        }
        else
        {
            pathOnly = M_CONST_CAST(char*, fileInfo->fullpath);
        }

#if defined(_DEBUG)
        printf("Checking directory security: %s\n", pathOnly);
#endif
        // This flag can disable the file path security check.
        // NOTE: Currently disabling _WIN32 due to new Windows security feature breaking
        // how current code works.
        //       This will be reenabled once we have resolved the low-level issue.
        char* dirSecurityError = M_NULLPTR;
#if defined(DISABLE_SECURE_FILE_PATH_CHECK) || defined(_WIN32)
#    if !defined(_WIN32)
#        pragma message(                                                                                               \
            "WARNING: Disabling Cert-C directory security check. This is not recommended for production level code.")
#    endif //!_WIN32
        M_USE_UNUSED(dirSecurityError);
        if (/* DISABLES CODE */ (true))
#else
        // Check for secure directory - This code must traverse the full path
        // and validate permissions of the directories.
        if (os_Is_Directory_Secure(pathOnly, &dirSecurityError))
#endif // DISABLE_SECURE_FILE_PATH_CHECK || _WIN32
        {
            errno_t fopenError = 0;
            fileInfo->file     = M_NULLPTR;
            fopenError         = safe_fopen(&fileInfo->file, fileInfo->fullpath, internalmode);
            if (fopenError == 0 && fileInfo->file != M_NULLPTR)
            {
                // file is opened, check unique id if it was passed in to
                // compare against
                fileInfo->uniqueID = os_Get_File_Unique_Identifying_Information(fileInfo->file);
                // compre to provided unique ID
                if (uniqueIdInfo)
                {
                    if (!compare_File_Unique_ID(uniqueIdInfo, fileInfo->uniqueID))
                    {
                        // This is not the same file as was meant to be opened.
                        // Close it and return an error.
                        M_STATIC_CAST(void, fclose(fileInfo->file));
                        fileInfo->file  = M_NULLPTR;
                        fileInfo->error = SEC_FILE_INVALID_FILE_UNIQUE_ID;
                        safe_free(&intFileName);
                        if (duplicatedModeForInternalUse)
                        {
                            safe_free(&internalmode);
                        }
                        if (pathOnly && allocatedLocalPathOnly)
                        {
                            safe_free(&pathOnly);
                        }
                        safe_free(&dirSecurityError);
                        return fileInfo;
                    }
                }
                fileInfo->attributes = os_Get_File_Attributes_By_File(fileInfo->file);
                // compare to user provided attributes
                if (expectedFileInfo)
                {
                    // devid and inode already checked, but if user didn't
                    // provide unique id, it will not hurt to check again
                    if ((expectedFileInfo->deviceID != fileInfo->attributes->deviceID) ||
                        (expectedFileInfo->inode != fileInfo->attributes->inode) ||
                        (expectedFileInfo->userID != fileInfo->attributes->userID) ||
                        (expectedFileInfo->groupID != fileInfo->attributes->groupID))
                    {
                        M_STATIC_CAST(void, fclose(fileInfo->file));
                        fileInfo->file  = M_NULLPTR;
                        fileInfo->error = SEC_FILE_INVALID_FILE_ATTRIBTUES;
                        safe_free(&intFileName);
                        if (duplicatedModeForInternalUse)
                        {
                            safe_free(&internalmode);
                        }
                        if (pathOnly && allocatedLocalPathOnly)
                        {
                            safe_free(&pathOnly);
                        }
                        safe_free(&dirSecurityError);
                        return fileInfo;
                    }
#if defined(_WIN32)
                    // Validate Windows SIDs are equivalent
                    // Convert the strings back to structures and use EqualSid
                    // and EqualAcl to do the comparisons. There may be other
                    // formatting in the string that does not affect the
                    // permissions which is why the conversion is necessary.
                    if (!exact_Compare_SIDS_And_DACL_Strings(fileInfo->attributes->winSecurityDescriptor,
                                                             expectedFileInfo->winSecurityDescriptor))
                    {
                        M_STATIC_CAST(void, fclose(fileInfo->file));
                        fileInfo->file  = M_NULLPTR;
                        fileInfo->error = SEC_FILE_INVALID_FILE_ATTRIBTUES;
                        safe_free(&intFileName);
                        if (duplicatedModeForInternalUse)
                        {
                            safe_free(&internalmode);
                        }
                        if (pathOnly && allocatedLocalPathOnly)
                        {
                            safe_free(&pathOnly);
                        }
                        safe_free(&dirSecurityError);
                        return fileInfo;
                    }
#endif //_WIN32
                }

                fileInfo->isValid = true;

#if defined(USING_C11) && defined(to_sizet)
                fileInfo->fileSize = to_sizet(fileInfo->attributes->filesize);
#else
                fileInfo->fileSize = int64_to_sizet(fileInfo->attributes->filesize);
#endif
                fileInfo->fileno = fileno(fileInfo->file);
#if defined(_DEBUG)
                printf("Filesize set to %zu\n", fileInfo->fileSize);
#endif
            }
            else
            {
                switch (fopenError)
                {
                    // add other errno values to check for?
                    // https://pubs.opengroup.org/onlinepubs/9699919799/functions/fopen.html
                case EINVAL:
                default:
                    fileInfo->error = SEC_FILE_FAILURE;
                    break;
                }
            }
        }
        else
        {
            fileInfo->error = SEC_FILE_INSECURE_PATH;
            printf("Insecure path detected: %s\n", dirSecurityError);
        }
        safe_free(&dirSecurityError);
        if (pathOnly && allocatedLocalPathOnly)
        {
            safe_free(&pathOnly);
        }
        if (duplicatedModeForInternalUse)
        {
            safe_free(&internalmode);
        }
        safe_free(&intFileName);
    }
    else if (fileInfo)
    {
        fileInfo->file  = M_NULLPTR;
        fileInfo->error = SEC_FILE_FAILURE;
    }
    return fileInfo;
}

// We can add additional things to do before closing the file to validate
// things, flush, etc as needed to make this better.
M_NODISCARD eSecureFileError secure_Close_File(secureFileInfo* fileInfo)
{
    if (fileInfo)
    {
        fileInfo->error = SEC_FILE_INVALID_FILE;
        if (fileInfo->file != M_NULLPTR)
        {
            int closeres = fclose(fileInfo->file);
            if (closeres == 0)
            {
                fileInfo->file  = M_NULLPTR;
                fileInfo->error = SEC_FILE_SUCCESS;
            }
            else if (closeres == EOF)
            {
                fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
            }
            else
            {
                // unknown result, so call this an error.
                fileInfo->error = SEC_FILE_FAILURE;
            }
        }
        else if (fileInfo->error != SEC_FILE_FAILURE_CLOSING_FILE)
        {
            // File was never opened, so return no error
            fileInfo->error = SEC_FILE_SUCCESS;
        }
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

M_NODISCARD eSecureFileError secure_Read_File(secureFileInfo* M_RESTRICT fileInfo,
                                              void* M_RESTRICT           buffer,
                                              size_t                     buffersize,
                                              size_t                     elementsize,
                                              size_t                     count,
                                              size_t*                    numberread /*optional*/)
{
    if (fileInfo)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        fileInfo->error = SEC_FILE_INVALID_FILE;
        if (fileInfo->file)
        {
            size_t readres = SIZE_T_C(0);
#if defined(HAVE_MSFT_SECURE_LIB)
            readres = fread_s(buffer, buffersize, elementsize, count, fileInfo->file);
#else
            if (buffer == M_NULLPTR)
            {
                fileInfo->error = SEC_FILE_INVALID_PARAMETER;
                return fileInfo->error;
            }
            else if (buffersize < (elementsize * count))
            {
                fileInfo->error = SEC_FILE_BUFFER_TOO_SMALL;
                return fileInfo->error;
            }
            readres = fread(buffer, elementsize, count, fileInfo->file);
#endif
            if (numberread)
            {
                *numberread = readres;
            }
            if (readres == 0)
            {
                if (elementsize == 0 || count == 0)
                {
                    fileInfo->error = SEC_FILE_SUCCESS;
                }
                else
                {
                    fileInfo->error = SEC_FILE_READ_WRITE_ERROR;
                }
            }
            else if (readres < count)
            {
                if (feof(fileInfo->file))
                {
                    // end of stream/file
                    fileInfo->error = SEC_FILE_END_OF_FILE_REACHED;
                }
                else if (ferror(fileInfo->file))
                {
                    fileInfo->error = SEC_FILE_READ_WRITE_ERROR;
                }
                else
                {
                    // some other kind of error???
                    fileInfo->error = SEC_FILE_FAILURE;
                }
            }
            else if (readres == count)
            {
                fileInfo->error = SEC_FILE_SUCCESS;
            }
            else
            {
                // unknown result, so call this an error.
                fileInfo->error = SEC_FILE_FAILURE;
            }
        }
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

M_NODISCARD eSecureFileError secure_Write_File(secureFileInfo* M_RESTRICT fileInfo,
                                               void* M_RESTRICT           buffer,
                                               size_t                     buffersize,
                                               size_t                     elementsize,
                                               size_t                     count,
                                               size_t*                    numberwritten /*optional*/)
{
    if (fileInfo)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        fileInfo->error = SEC_FILE_INVALID_FILE;
        if (fileInfo->file)
        {
            size_t writeres = SIZE_T_C(0);
            if (buffer == M_NULLPTR)
            {
                fileInfo->error = SEC_FILE_INVALID_PARAMETER;
                return fileInfo->error;
            }
            else if (buffersize < (elementsize * count))
            {
                fileInfo->error = SEC_FILE_BUFFER_TOO_SMALL;
                return fileInfo->error;
            }
            writeres = fwrite(buffer, elementsize, count, fileInfo->file);
            if (numberwritten)
            {
                *numberwritten = writeres;
            }
            if (writeres == 0)
            {
                if (elementsize == 0 || count == 0)
                {
                    fileInfo->error = SEC_FILE_SUCCESS;
                }
                else
                {
                    fileInfo->error = SEC_FILE_READ_WRITE_ERROR;
                }
            }
            else if (writeres < count)
            {
                if (ferror(fileInfo->file))
                {
                    fileInfo->error = SEC_FILE_WRITE_DISK_FULL;
                }
                else
                {
                    // some other kind of error???
                    fileInfo->error = SEC_FILE_READ_WRITE_ERROR;
                }
            }
            else if (writeres == count)
            {
                fileInfo->error = SEC_FILE_SUCCESS;
            }
            else
            {
                // unknown result, so call this an error.
                fileInfo->error = SEC_FILE_FAILURE;
            }
        }
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

M_NODISCARD eSecureFileError secure_Seek_File(secureFileInfo* fileInfo, oscoffset_t offset, int initialPosition)
{
    if (fileInfo)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        fileInfo->error = SEC_FILE_INVALID_FILE;
        if (fileInfo->file)
        {
            int seekres = 0;
            // Windows has _fseeki64, which may be better to use instead for
            // larger files or to be compatible with larger files. Linux/posix
            // have fseeko and ftello which use off_t which can be wider as
            // well. (POSIX 2001)
#if defined(_WIN32) /*version check for this function?*/
            seekres = _fseeki64(fileInfo->file, offset, initialPosition);
#elif defined(POSIX_2001)
            seekres = fseeko(fileInfo->file, offset, initialPosition);
#else
            seekres         = fseek(fileInfo->file, offset, initialPosition);
#endif
            if (seekres == 0)
            {
                fileInfo->error = SEC_FILE_SUCCESS;
            }
            else
            {
                fileInfo->error = SEC_FILE_SEEK_FAILURE;
            }
        }
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

M_NODISCARD eSecureFileError secure_Rewind_File(secureFileInfo* fileInfo)
{
    if (fileInfo)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        fileInfo->error = SEC_FILE_INVALID_FILE;
        if (fileInfo->file)
        {
            fileInfo->error = secure_Seek_File(fileInfo, 0, SEEK_SET);
#if defined(HAVE_MSFT_SECURE_LIB)
            clearerr_s(fileInfo->file);
#else
            clearerr(fileInfo->file); // rewind clears errors and eof indicators, so
                                      // call this to clear the errors in the stream.
#endif
        }
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

M_NODISCARD oscoffset_t secure_Tell_File(secureFileInfo* fileInfo)
{
    if (fileInfo)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        fileInfo->error = SEC_FILE_INVALID_FILE;
        if (fileInfo->file)
        {
            oscoffset_t tellres = 0;
            // Windows has _fseeki64, which may be better to use instead for larger files or to be compatible with
            // larger files. Linux/posix have fseeko and ftello which use off_t which can be wider as well. (POSIX 2001)
            errno = 0; // ISO secure coding standard recommends this to ensure errno is interpretted correctly after
                       // this call
#if defined(_WIN32)    /*version check for this function?*/
            tellres = _ftelli64(fileInfo->file);
#elif defined(POSIX_2001)
            tellres = ftello(fileInfo->file);
#else
            tellres         = ftell(fileInfo->file);
#endif
            if (tellres >= 0)
            {
                fileInfo->error = SEC_FILE_SUCCESS;
            }
            else
            {
                fileInfo->error = SEC_FILE_SEEK_FAILURE;
            }
            return tellres;
        }
        return -1;
    }
    return -1;
}

eSecureFileError secure_Flush_File(secureFileInfo* fileInfo)
{
    if (fileInfo)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        fileInfo->error = SEC_FILE_INVALID_FILE;
        if (fileInfo->file)
        {
            int fflushres = fflush(fileInfo->file);
            if (fflushres == 0)
            {
                fileInfo->error = SEC_FILE_SUCCESS;
            }
            else if (fflushres == EOF && ferror(fileInfo->file))
            {
                fileInfo->error = SEC_FILE_FLUSH_FAILURE;
            }
            else /*not sure what to clasify this error as*/
            {
                fileInfo->error = SEC_FILE_FAILURE;
            }
        }
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

M_NODISCARD eSecureFileError secure_Remove_File(secureFileInfo* fileInfo)
{
    if (fileInfo)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        fileInfo->error = SEC_FILE_INVALID_PATH;
        if (fileInfo->file && safe_strlen(fileInfo->fullpath) > 0)
        {
            // unlink the file is possible
#if defined(_WIN32)
            if (0 != _unlink(fileInfo->fullpath))
            {
                fileInfo->error = SEC_FILE_FAILURE;
            }
#elif defined(POSIX_2001) || defined(BSD4_3) || defined(__svr4__)
            if (0 != unlink(fileInfo->fullpath))
            {
                fileInfo->error = SEC_FILE_FAILURE;
            }
#else
            fileInfo->error = SEC_FILE_CANNOT_REMOVE_FILE_STILL_OPEN;
#endif
        }
        else if (safe_strlen(fileInfo->fullpath) > 0)
        {
            // remove the file
            if (0 != remove(fileInfo->fullpath))
            {
                fileInfo->error = SEC_FILE_FAILURE;
            }
        }
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

M_NODISCARD eSecureFileError secure_Delete_File_By_Name(const char* filename, eSecureFileDeleteNameAction deleteAction)
{
    if (filename)
    {
        // first get cannonical name
        DECLARE_ZERO_INIT_ARRAY(char, fullpath, 4096);
        if (SUCCESS != get_Full_Path(filename, C_CAST(char*, fullpath)))
        {
            // unable to get the full path to this file.
            // This means something went wrong, and we need to return an error.
            return SEC_FILE_INVALID_PATH;
        }
        char* lastsep = strrchr(fullpath, '/');
#if defined(_WIN32)
        // In Windows, we need to determine if the final seperator is a / or \.
        // So also check for \ and figure out which was the last one.
        // A user can pass a path with both and it can be accepted by Windows,
        // which is why we validate both of these.
        char* lastwinsep = strrchr(fullpath, '\\');
        if (C_CAST(uintptr_t, lastwinsep) > C_CAST(uintptr_t, lastsep))
        {
            // backslash was detected last, so change to this pointer instead
            // for strndup
            lastsep = lastwinsep;
        }
#endif //_WIN32
       // Check for secure directory - This code must traverse the full path and
       // validate permissions of the directories.
        char* pathOnly = M_NULLPTR;
        if (safe_strndup(&pathOnly, fullpath, C_CAST(uintptr_t, lastsep) - C_CAST(uintptr_t, fullpath)) != 0 ||
            pathOnly == M_NULLPTR)
        {
            pathOnly = C_CAST(char*, fullpath);
        }
        if (!os_Is_Directory_Secure(pathOnly, M_NULLPTR))
        {
            safe_free(&pathOnly);
            return SEC_FILE_INSECURE_PATH;
        }
        safe_free(&pathOnly);
        // Check if the file is already open before attempting to remove it
        errno_t fileerror  = 0;
        FILE*   checkExist = M_NULLPTR;
        fileerror          = safe_fopen(&checkExist, fullpath, "r");
        if (checkExist != M_NULLPTR && fileerror == 0)
        {
            M_STATIC_CAST(void, fclose(checkExist));
            checkExist = M_NULLPTR;
            if (0 == remove(fullpath))
            {
                return SEC_FILE_SUCCESS;
            }
            else
            {
                return SEC_FILE_FAILURE;
            }
        }
        else if (os_File_Exists(fullpath))
        {
            switch (deleteAction)
            {
            case SEC_DELETE_NAME_FAIL_IF_OPEN:
                return SEC_FILE_CANNOT_REMOVE_FILE_STILL_OPEN;
            case SEC_DELETE_NAME_UNLINK_IF_OPEN:
                // File cannot be opened, so we must assume something else has
                // it open, so unlink instead-TJE
#if defined(_WIN32)
                if (0 != _unlink(fullpath))
                {
                    return SEC_FILE_FAILURE;
                }
                return SEC_FILE_SUCCESS;
#elif defined(POSIX_2001) || defined(BSD4_3) || defined(__svr4__)
                if (0 != unlink(fullpath))
                {
                    return SEC_FILE_FAILURE;
                }
                return SEC_FILE_SUCCESS;
#else
                return SEC_FILE_CANNOT_REMOVE_FILE_STILL_OPEN;
#endif
            }
        }
        return SEC_FILE_FAILURE;
    }
    return SEC_FILE_INVALID_PATH;
}

eSecureFileError secure_GetPos_File(secureFileInfo* M_RESTRICT fileInfo, fpos_t* M_RESTRICT pos)
{
    if (fileInfo && pos)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        fileInfo->error = SEC_FILE_INVALID_FILE;
        if (fileInfo->file)
        {
            int getposres = fgetpos(fileInfo->file, pos);
            if (getposres == 0)
            {
                fileInfo->error = SEC_FILE_SUCCESS;
            }
            else
            {
                // TODO: inspect errno - ISO C security recommendation
                fileInfo->error = SEC_FILE_FAILURE;
            }
        }
        return fileInfo->error;
    }
    else if (fileInfo)
    {
        // pos is invalid
        fileInfo->error = SEC_FILE_INVALID_PARAMETER;
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

eSecureFileError secure_SetPos_File(secureFileInfo* fileInfo, const fpos_t* pos)
{
    if (fileInfo && pos)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        fileInfo->error = SEC_FILE_INVALID_FILE;
        if (fileInfo->file)
        {
            int setposres = fsetpos(fileInfo->file, pos);
            if (setposres == 0)
            {
                fileInfo->error = SEC_FILE_SUCCESS;
            }
            else
            {
                // TODO: inspect errno - ISO C security recommendation
                fileInfo->error = SEC_FILE_FAILURE;
            }
        }
        return fileInfo->error;
    }
    else if (fileInfo)
    {
        // pos is invalid
        fileInfo->error = SEC_FILE_INVALID_PARAMETER;
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

FUNC_ATTR_PRINTF(2, 0)
eSecureFileError secure_vfprintf_File(secureFileInfo* M_RESTRICT fileInfo, const char* M_RESTRICT format, va_list args)
{
    if (fileInfo && fileInfo->file)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        if (format)
        {
            int vfprintfresult = 0;
#if defined(HAVE_C11_ANNEX_K) || defined(HAVE_MSFT_SECURE_LIB)
            vfprintfresult = vfprintf_s(fileInfo->file, format, args);
#else
            va_list verifyargs;
            va_list vfprintfargs;
            va_copy(verifyargs, args);
            if (verify_Format_String_And_Args(format, verifyargs) > 0)
            {
                va_copy(vfprintfargs, args);
                // nolint line is because of a false positive....clearly copying the args before calling this-TJE
                vfprintfresult =
                    vfprintf(fileInfo->file, format, vfprintfargs); // NOLINT(clang-analyzer-valist.Uninitialized)
                va_end(vfprintfargs);
            }
            else
            {
                vfprintfresult = -1;
            }
            va_end(verifyargs);
#endif
            if (vfprintfresult < 0 || ferror(fileInfo->file))
            {
                fileInfo->error = SEC_FILE_READ_WRITE_ERROR;
            }
            else
            {
                fileInfo->error = SEC_FILE_SUCCESS;
            }
        }
        else
        {
            // M_NULLPTR pointer for the format string
            fileInfo->error = SEC_FILE_INVALID_PARAMETER;
        }
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

FUNC_ATTR_PRINTF(2, 3)
eSecureFileError secure_fprintf_File(secureFileInfo* M_RESTRICT fileInfo, const char* M_RESTRICT format, ...)
{
    if (fileInfo && fileInfo->file)
    {
        if (fileInfo->error == SEC_FILE_FAILURE_CLOSING_FILE)
        {
            return SEC_FILE_FAILURE_CLOSING_FILE;
        }
        if (format)
        {
            va_list args;
            va_start(args, format);
            eSecureFileError result = secure_vfprintf_File(fileInfo, format, args);
            va_end(args);
            return result;
        }
        else
        {
            // M_NULLPTR pointer for the format string
            fileInfo->error = SEC_FILE_INVALID_PARAMETER;
        }
        return fileInfo->error;
    }
    return SEC_FILE_INVALID_SECURE_FILE;
}

char* generate_Log_Name(eLogFileNamingConvention logFileNamingConvention, // required
                        const char*              deviceIdentifier,        // required
                        size_t                   deviceIDLen,             // required
                        const char*              logPath, // optional /*requested path to output to. Will be checked for
                                                          // security. If NULL, current directory will be used*/
                        size_t      logPathLen,           // may be 0
                        const char* logName,              // optional /*name of the log file from the drive,
                                                          // FARM, DST, etc*/
                        size_t      logNameLen,           // may be 0
                        const char* logExt,   // optional /*extension for the log file. If NULL, set to .bin*/
                        size_t      logExtLen // may be 0
)
{
    DECLARE_ZERO_INIT_ARRAY(char, path, OPENSEA_PATH_MAX);
    char* logExtension  = M_NULLPTR;
    char* generatedName = M_NULLPTR;
    if ((deviceIdentifier == M_NULLPTR || deviceIDLen == 0) && logFileNamingConvention != NAMING_BYUSER)
    {
        return M_NULLPTR;
    }
    if (logPath && logPathLen > 0)
    {
        snprintf(path, OPENSEA_PATH_MAX, "%s", logPath);
        // if there is no path separator at the end of this string, then we need
        // to append one before using this path below
        if (path[safe_strnlen(path, OPENSEA_PATH_MAX) - 1] != SYSTEM_PATH_SEPARATOR)
        {
            safe_strcat(path, OPENSEA_PATH_MAX, SYSTEM_PATH_SEPARATOR_STR);
        }
    }
    else
    {
        // set to relative current working directory ./ or .\\ in Windows. This
        // will be expanded by secure file to a full path
        snprintf(path, OPENSEA_PATH_MAX, ".%c", SYSTEM_PATH_SEPARATOR);
    }
    if (logExt && logExtLen > SIZE_T_C(0))
    {
        // check if there is already an extension with a dot, otherwise add a
        // dot then the extension
        if (logExt[0] == '.')
        {
            if (0 != safe_strdup(&logExtension, logExt))
            {
                return M_NULLPTR;
            }
        }
        else
        {
            if (asprintf(&logExtension, ".%s", logExt) < 0)
            {
                if (logExtension != M_NULLPTR)
                {
                    // allocated, but had some other failure.
                    // free this and return M_NULLPTR
                    safe_free(&logExtension);
                }
                return M_NULLPTR;
            }
        }
    }
    else
    {
        // no extension provided, but allocate an empty string so as not to
        // cause undefined behavrior in the format string interpretation below
        logExtension = M_REINTERPRET_CAST(char*, safe_calloc(1, sizeof(char)));
    }
    if (logExtension == M_NULLPTR)
    {
        return M_NULLPTR;
    }
    get_current_timestamp(); // make sure timestamp is set
    switch (logFileNamingConvention)
    {
    case NAMING_SERIAL_NUMBER_ONLY:
        if (asprintf(&generatedName, "%s%s%s", path, deviceIdentifier, logExtension) < 0)
        {
            if (generatedName != M_NULLPTR)
            {
                // allocated, but had some other failure.
                // free this and return M_NULLPTR
                safe_free(&generatedName);
            }
            safe_free(&logExtension);
            return M_NULLPTR;
        }
        break;
    case NAMING_SERIAL_NUMBER_DATE_TIME:
        if (logName && logNameLen > 0)
        {
            if (asprintf(&generatedName, "%s%s_%s_%s%s", path, deviceIdentifier, logName, CURRENT_TIME_STRING,
                         logExtension) < 0)
            {
                if (generatedName != M_NULLPTR)
                {
                    // allocated, but had some other failure.
                    // free this and return M_NULLPTR
                    safe_free(&generatedName);
                }
                safe_free(&logExtension);
                return M_NULLPTR;
            }
        }
        else
        {
            if (asprintf(&generatedName, "%s%s_unknown_%s%s", path, deviceIdentifier, CURRENT_TIME_STRING,
                         logExtension) < 0)
            {
                if (generatedName != M_NULLPTR)
                {
                    // allocated, but had some other failure.
                    // free this and return M_NULLPTR
                    safe_free(&generatedName);
                }
                safe_free(&logExtension);
                return M_NULLPTR;
            }
        }
        break;
    case NAMING_BYUSER:
        if (asprintf(&generatedName, "%s%s%s", path, deviceIdentifier, logExtension) < 0)
        {
            if (generatedName != M_NULLPTR)
            {
                // allocated, but had some other failure.
                // free this and return M_NULLPTR
                safe_free(&generatedName);
            }
            safe_free(&logExtension);
            return M_NULLPTR;
        }
        break;
    }
    safe_free(&logExtension);
    return generatedName;
}

eReturnValues create_And_Open_Secure_Log_File(
    const char*              deviceIdentifier,        // required
    size_t                   deviceIDLen,             // required
    secureFileInfo**         file,                    // required
    eLogFileNamingConvention logFileNamingConvention, // required
    const char*              logPath,                 // optional /*requested path to output to. Will be checked for
                                                      // security. If NULL, current directory will be used*/
    size_t      logPathLen,                           // may be 0
    const char* logName,                              // optional /*name of the log file from the drive,
                                                      // FARM, DST, etc*/
    size_t      logNameLen,                           // may be 0
    const char* logExt,                               // optional /*extension for the log file. If NULL, set to .bin*/
    size_t      logExtLen                             // may be 0
    // NOTE: This function does not return the name used as that is part of the
    // secureFileInfo -TJE
)
{
    eReturnValues result = SUCCESS;
    // Step 1: Validate required parameters
    if (deviceIdentifier == M_NULLPTR || deviceIDLen == 0 || file == M_NULLPTR || logPathLen > OPENSEA_PATH_MAX ||
        logNameLen > RSIZE_MAX || logExtLen > RSIZE_MAX)
    {
        return BAD_PARAMETER;
    }
    // Step 2: Generate the logfile name from the provided parameters
    char* logGeneratedName = generate_Log_Name(logFileNamingConvention, deviceIdentifier, deviceIDLen, logPath,
                                               logPathLen, logName, logNameLen, logExt, logExtLen);
    if (logGeneratedName == M_NULLPTR)
    {
        return MEMORY_FAILURE;
    }
    // Step 3: Create the secure file (which will check directory security, etc
    // for us. NOTE: Always opened with X for exclusive so as not to overwrite
    // an existing file)
    *file = secure_Open_File(logGeneratedName, "wxb", M_NULLPTR, M_NULLPTR, M_NULLPTR);
    // Step 4: return error code depending on what happened from attempting to
    // do all of these steps
    if (!file)
    {
        result = MEMORY_FAILURE;
    }
    else if ((*file)->error == SEC_FILE_INSECURE_PATH)
    {
        result = INSECURE_PATH;
    }
    else if ((*file)->error != SEC_FILE_SUCCESS)
    {
        result = FAILURE;
        // Should we give more error information here or just call it a
        // failure???-TJE
    }
    safe_free(&logGeneratedName);
    return result;
}
