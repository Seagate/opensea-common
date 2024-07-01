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
// \file common_windows.c
// \brief Implements functions that are common to MSFT Windows.
//
#include "common.h"
#include "common_windows.h"
#include "common_platform.h"
#include <windows.h> //needed for all the stuff to get the windows version
#include <tchar.h>
#include <strsafe.h> //needed in the code written to get the windows version since I'm using a Microsoft provided string concatenation call-tje
#include <io.h> //needed for getting the size of a file in windows
#include <lmcons.h> //for UNLEN
#include <string.h>
#include <AclAPI.h>
#include <sddl.h>

#include <sys/types.h>
#include <sys/stat.h>

#if defined (HAVE_NTIFS)
#include <ntifs.h>
#else
//https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_reparse_data_buffer
//Defining this ourselves to be able to access this.
typedef struct _REPARSE_DATA_BUFFER {
    ULONG  ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union {
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG  Flags;
            WCHAR  PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR  PathBuffer[1];
        } MountPointReparseBuffer;
        struct {
            UCHAR DataBuffer[1];
        } GenericReparseBuffer;
    } DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER, * PREPARSE_DATA_BUFFER;
#endif //HAVE_NTIFS

/*
When stdin, stdout, and stderr aren't associated with a stream (for example, in a Windows application without a console window), 
the file descriptor values for these streams are returned from _fileno as the special value -2. 
Similarly, if you use a 0, 1, or 2 as the file descriptor parameter instead of the result of a call to _fileno, 
_get_osfhandle also returns the special value -2 when the file descriptor is not associated with a stream, and does not set errno. 
However, this is not a valid file handle value, and subsequent calls that attempt to use it are likely to fail.
*/
#define WIN_FD_INVALID (-2)

static bool win_File_Attributes_By_Name(const char* const filename, LPWIN32_FILE_ATTRIBUTE_DATA attributes)
{
    bool success = false;
    if (filename && attributes)
    {
        size_t pathCheckLength = (strlen(filename) + 1) * sizeof(TCHAR);
        TCHAR* localPathToCheckBuf = C_CAST(TCHAR*, calloc(pathCheckLength, sizeof(TCHAR)));
        if (!localPathToCheckBuf)
        {
            return false;
        }
        CONST TCHAR* localPathToCheck = &localPathToCheckBuf[0];
        _stprintf_s(localPathToCheckBuf, pathCheckLength, TEXT("%hs"), filename);

        BOOL gotAttributes = GetFileAttributesEx(localPathToCheck, GetFileExInfoStandard, attributes);
        if (gotAttributes)
        {
            success = true;
        }
        safe_Free(localPathToCheckBuf)
        localPathToCheck = NULL;
    }
    return success;
}

static bool win_File_Attributes_By_File(FILE *file, LPBY_HANDLE_FILE_INFORMATION attributes)
{
    bool success = false;
    if (file && attributes)
    {
        int fd = _fileno(file);
        if (fd == WIN_FD_INVALID)
        {
            return false;
        }
        HANDLE msftHandle = C_CAST(HANDLE, _get_osfhandle(fd));
        if (msftHandle == INVALID_HANDLE_VALUE || C_CAST(intptr_t, msftHandle) == WIN_FD_INVALID)
        {
            return false;
        }
        if (SUCCESS == GetFileInformationByHandle(msftHandle, attributes))
        {
            success = true;
        }
    }
    return success;
}

static bool win_Get_File_Security_Info_By_Name(const char* const filename, fileAttributes* attrs)
{
    bool success = false;
    if (filename && attrs)
    {
        size_t pathCheckLength = (strlen(filename) + 1) * sizeof(TCHAR);
        TCHAR* localPathToCheckBuf = C_CAST(TCHAR*, calloc(pathCheckLength, sizeof(TCHAR)));
        if (!localPathToCheckBuf)
        {
            return false;
        }
        _stprintf_s(localPathToCheckBuf, pathCheckLength, TEXT("%hs"), filename);
        CONST TCHAR* localPathToCheck = &localPathToCheckBuf[0];
        
        //Do not need SACL since it may not be available and is about triggering audits instead of a dacl which defines who has access to a file.
        SECURITY_INFORMATION secInfo = OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
        PSECURITY_DESCRIPTOR secDescriptor = M_NULLPTR;
        PSID owner = M_NULLPTR, group = M_NULLPTR;
        PACL dacl = M_NULLPTR;
        if (ERROR_SUCCESS == GetNamedSecurityInfo(localPathToCheck, SE_FILE_OBJECT, secInfo, &owner, &group, &dacl, NULL, &secDescriptor))
        {
            ULONG tempLen = 0;
            LPSTR temp = M_NULLPTR;
            if (TRUE == ConvertSecurityDescriptorToStringSecurityDescriptorA(secDescriptor, SDDL_REVISION, secInfo, &temp, &tempLen))
            {
                /* do not use strdup or strndup here. There are some extra nulls at the end of what Windows allocates that we are preserving in memcpy */
                attrs->winSecurityDescriptor = C_CAST(char*, calloc(tempLen, sizeof(char)));
                if (attrs->winSecurityDescriptor)
                {
                    attrs->securityDescriptorStringLength = tempLen;
                    memcpy_s(attrs->winSecurityDescriptor, attrs->securityDescriptorStringLength, temp, tempLen);
                    success = true;
                }
                SecureZeroMemory(temp, tempLen);
                LocalFree(temp);
                temp = M_NULLPTR;
            }
            SecureZeroMemory(secDescriptor, GetSecurityDescriptorLength(secDescriptor));
            LocalFree(secDescriptor);
            secDescriptor = M_NULLPTR;
        }
    }
    return success;
}

static bool win_Get_File_Security_Info_By_File(FILE *file, fileAttributes* attrs)
{
    bool success = false;
    if (file && attrs)
    {
        //Do not need SACL since it may not be available and is about triggering audits instead of a dacl which defines who has access to a file.
        SECURITY_INFORMATION secInfo = OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
        PSECURITY_DESCRIPTOR secDescriptor = M_NULLPTR;
        PSID owner = M_NULLPTR, group = M_NULLPTR;
        PACL dacl = M_NULLPTR;
        HANDLE msftHandle = INVALID_HANDLE_VALUE;
        int fd = _fileno(file);
        if (fd == WIN_FD_INVALID)
        {
            return false;
        }
        msftHandle = C_CAST(HANDLE, _get_osfhandle(fd));
        if (msftHandle == INVALID_HANDLE_VALUE || C_CAST(intptr_t, msftHandle) == WIN_FD_INVALID)
        {
            return false;
        }
        if (ERROR_SUCCESS == GetSecurityInfo(msftHandle, SE_FILE_OBJECT, secInfo, &owner, &group, &dacl, NULL, &secDescriptor))
        {
            ULONG tempLen = 0;
            LPSTR temp = M_NULLPTR;
            if (TRUE == ConvertSecurityDescriptorToStringSecurityDescriptorA(secDescriptor, SDDL_REVISION, secInfo, &temp, &tempLen))
            {
                /* do not use strdup or strndup here. There are some extra nulls at the end of what Windows allocates that we are preserving in memcpy */
                attrs->winSecurityDescriptor = C_CAST(char*, calloc(tempLen, sizeof(char)));
                if (attrs->winSecurityDescriptor)
                {
                    attrs->securityDescriptorStringLength = tempLen;
                    memcpy_s(attrs->winSecurityDescriptor, attrs->securityDescriptorStringLength, temp, tempLen);
                    success = true;
                }
                SecureZeroMemory(temp, attrs->securityDescriptorStringLength);
                LocalFree(temp);
                temp = M_NULLPTR;
            }
            SecureZeroMemory(secDescriptor, GetSecurityDescriptorLength(secDescriptor));
            LocalFree(secDescriptor);
            secDescriptor = M_NULLPTR;
        }
    }
    return success;
}

fileAttributes* os_Get_File_Attributes_By_Name(const char* const filetoCheck)
{
    fileAttributes* attrs = NULL;
    struct _stat64 st;
    memset(&st, 0, sizeof(struct _stat64));
    if (filetoCheck && _stat64(filetoCheck, &st) == 0)
    {
        attrs = C_CAST(fileAttributes*, calloc(1, sizeof(fileAttributes)));
        if (attrs)
        {
            WIN32_FILE_ATTRIBUTE_DATA winAttributes;
            memset(&winAttributes, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
            attrs->deviceID = st.st_dev;
            attrs->inode = st.st_ino;
            attrs->filemode = st.st_mode;
            attrs->numberOfLinks = st.st_nlink;
            attrs->userID = st.st_uid;
            attrs->groupID = st.st_gid;
            attrs->representedDeviceID = st.st_rdev;
            attrs->filesize = st.st_size;
            attrs->fileLastAccessTime = st.st_atime;
            attrs->fileModificationTime = st.st_mtime;
            attrs->fileStatusChangeTime = st.st_ctime;
            if (win_File_Attributes_By_Name(filetoCheck, &winAttributes))
            {
                attrs->fileFlags = winAttributes.dwFileAttributes;
            }
            win_Get_File_Security_Info_By_Name(filetoCheck, attrs);
        }
    }
    return attrs;
}


fileAttributes* os_Get_File_Attributes_By_File(FILE *file)
{
    fileAttributes* attrs = NULL;
    struct _stat64 st;
    memset(&st, 0, sizeof(struct _stat64));
    if (file && _fstat64(_fileno(file), &st) == 0)
    {
        attrs = C_CAST(fileAttributes*, calloc(1, sizeof(fileAttributes)));
        if (attrs)
        {
            BY_HANDLE_FILE_INFORMATION winAttributes;
            memset(&winAttributes, 0, sizeof(BY_HANDLE_FILE_INFORMATION));
            attrs->deviceID = st.st_dev;
            attrs->inode = st.st_ino;
            attrs->filemode = st.st_mode;
            attrs->numberOfLinks = st.st_nlink;
            attrs->userID = st.st_uid;
            attrs->groupID = st.st_gid;
            attrs->representedDeviceID = st.st_rdev;
            attrs->filesize = st.st_size;
            attrs->fileLastAccessTime = st.st_atime;
            attrs->fileModificationTime = st.st_mtime;
            attrs->fileStatusChangeTime = st.st_ctime;
            if (win_File_Attributes_By_File(file, &winAttributes))
            {
                attrs->fileFlags = winAttributes.dwFileAttributes;
            }
            win_Get_File_Security_Info_By_File(file, attrs);
        }
    }
    return attrs;
}


fileUniqueIDInfo* os_Get_File_Unique_Identifying_Information(FILE* file)
{
    if (file)
    {
        int fd = _fileno(file);//DO NOT CALL CLOSE ON FD!
        if (fd == WIN_FD_INVALID)//_get_osfhandle says this is a special value defined for MSFT for stdin, stdout, stderr
        {
            return NULL; //This is stdout, strerr, stdin and not a file that we can get other info on. This is not the correct thing to pass to this function!
        }
        HANDLE msftHandle = C_CAST(HANDLE, _get_osfhandle(fd));//DO NOT CALL CLOSE ON MSFTHANDLE
        if (msftHandle == INVALID_HANDLE_VALUE || C_CAST(intptr_t, msftHandle) == WIN_FD_INVALID)
        {
            return NULL;
        }
#if defined (WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_VERSION
        if (is_Windows_Vista_Or_Higher())//This ex function is only available on Vista and later OSs according to MSFT docs
        {
            //try to get ex file info, then if it fails (shouldn't happen) then fall back to old method to get this
            FILE_ID_INFO winfileid;
            memset(&winfileid, 0, sizeof(FILE_ID_INFO));
            if (TRUE == GetFileInformationByHandleEx(msftHandle, FileIdInfo, &winfileid, sizeof(FILE_ID_INFO)))
            {
                //full 128bit identifier available
                fileUniqueIDInfo* fileId = C_CAST(fileUniqueIDInfo*, calloc(1, sizeof(fileUniqueIDInfo)));
                if (fileId)
                {
                    fileId->volsn = winfileid.VolumeSerialNumber;
                    memcpy(&fileId->fileid[0], &winfileid.FileId.Identifier[0], FILE_UNIQUE_ID_ARR_MAX);
                }
                return fileId;
            }
        }
#endif //Windows vista API
        BY_HANDLE_FILE_INFORMATION winfileinfo;
        memset(&winfileinfo, 0, sizeof(BY_HANDLE_FILE_INFORMATION));
        if (TRUE == GetFileInformationByHandle(msftHandle, &winfileinfo))
        {
            fileUniqueIDInfo* fileId = C_CAST(fileUniqueIDInfo*, calloc(1, sizeof(fileUniqueIDInfo)));
            if (fileId)
            {
                fileId->volsn = winfileinfo.dwVolumeSerialNumber;
                memcpy(&fileId->fileid[0], &winfileinfo.nFileIndexHigh, sizeof(DWORD));
                memcpy(&fileId->fileid[sizeof(DWORD)], &winfileinfo.nFileIndexLow, sizeof(DWORD));
            }
            return fileId;
        }
    }
    return NULL;
}

static char* get_Current_User_SID(void)
{
    char* sidAsString = M_NULLPTR;
    HANDLE hToken = M_NULLPTR;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) 
    {
        DWORD dwSize = 0;
        GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
        PTOKEN_USER pUser = C_CAST(PTOKEN_USER, malloc(dwSize));
        if (pUser)
        {
            memset(pUser, 0, dwSize);
            if (GetTokenInformation(hToken, TokenUser, pUser, dwSize, &dwSize))
            {
                LPSTR pSidString = M_NULLPTR;
                if (ConvertSidToStringSidA(pUser->User.Sid, &pSidString))
                {
                    sidAsString = strdup(pSidString);//dup'ing this because want to use std malloc/free's instead of LocalFree
                    LocalFree(pSidString);
                }
            }
            explicit_zeroes(pUser, dwSize);
            safe_Free(pUser);
        }
        CloseHandle(hToken);
    }
    return sidAsString;
}

static bool is_Root_Path(const char* path)
{
    bool isroot = false;
    if (path)
    {
        //Check is for a drive letter + :
        //trailing \ is not checked because it is stripped off by win_dirname function when this is used to figure out number of directories to validate.
        if (strlen(path) == 2 && is_ASCII(path[0]) && path[1] == ':')
        {
            isroot = true;
        }
    }
    return isroot;
}

/* this expects path seperators to be '\\' otherwise it will fail */
static char* win_dirname(char* path)
{
    if (path)
    {
        char* lastseperator = strrchr(path, '\\');
        if (lastseperator)
        {
            if (!is_Root_Path(lastseperator))
            {
                /* Special case. The root path C:\ needs to leave the trailing backslash behind */
                *lastseperator = '\0';
            }
            return path;
        }
        else
        {
            return ".";
        }
    }
    else
    {
        return M_NULLPTR;
    }
}

//https://learn.microsoft.com/en-us/windows/win32/secauthz/well-known-sids
static bool is_Secure_Well_Known_SID(PSID sid)
{
    bool secure = false;
    if (sid && IsValidSid(sid))
    {
        if (IsWellKnownSid(sid, WinAccountAdministratorSid)
            || IsWellKnownSid(sid, WinLocalSystemSid)
            || IsWellKnownSid(sid, WinNtAuthoritySid)
            || IsWellKnownSid(sid, WinLocalSid)
            || IsWellKnownSid(sid, WinBuiltinAdministratorsSid)
            )
        {
            //Do we need to check any of these other SIDs for admins?
            // POSIX validates user or root. Do any of these others below in the if above as accepted???
            //WinBuiltinAdministratorsSid ??
            //WinAccountDomainAdminsSid ??
            //WinAccountCertAdminsSid ??
            //WinAccountSchemaAdminsSid ??
            //WinAccountEnterpriseAdminsSid ??
            //WinAccountPolicyAdminsSid ??
            //WinBuiltinHyperVAdminsSid ??
            //WinLocalAccountAndAdministratorSid ??
            //WinBuiltinStorageReplicaAdminsSid ??
            //WinAccountKeyAdminsSid ??
            //WinAccountEnterpriseKeyAdminsSid ??
            secure = true;
        }
    }
    return secure;
}

static bool is_Folder_Secure(const char *securityDescriptorString)
{
    bool secure = true;
    if (!securityDescriptorString)
    {
        return false;
    }
    char* mySidStr = get_Current_User_SID();
    PSID mySid = M_NULLPTR;
    PSECURITY_DESCRIPTOR secdesc = M_NULLPTR;
    ULONG secdesclen = 0;
    PSID userSid = M_NULLPTR;
    BOOL defaultOwner = FALSE;
    PACL dacl = M_NULLPTR;
    BOOL daclPresent = FALSE, daclDefault = FALSE;
    do
    {
        if (FALSE == ConvertStringSidToSidA(mySidStr, &mySid))
        {
            /* Handle Error */
            secure = false;
            break;
        }
        if (FALSE == IsValidSid(mySid))
        {
            /* Handle Error */
            secure = false;
            break;
        }
        if (FALSE == ConvertStringSecurityDescriptorToSecurityDescriptorA(securityDescriptorString, SDDL_REVISION, &secdesc, &secdesclen))
        {
            /* Handle Error */
            secure = false;
            break;
        }
        if (FALSE == IsValidSecurityDescriptor(secdesc))
        {
            /* Handle Error */
            secure = false;
            break;
        }
        if (FALSE == GetSecurityDescriptorOwner(secdesc, &userSid, &defaultOwner))
        {
            /* Handle Error */
            secure = false;
            break;
        }
        if (FALSE == IsValidSid(userSid))
        {
            /* Handle Error */
            secure = false;
            break;
        }
        if (!EqualSid(mySid, userSid) && is_Secure_Well_Known_SID(userSid))
        {
            /* Directory is owned by someone besides user/system/administrator */
            secure = false;
            break;
        }
        
        if (FALSE == GetSecurityDescriptorDacl(secdesc, &daclPresent, &dacl, &daclDefault))
        {
            /* Handle Error */
            secure = false;
            break;
        }

        if (FALSE == daclPresent || dacl == M_NULLPTR)
        {
            /* Missing DACL, so cannot verify permissions */
            secure = false;
            break;
        }

        if (FALSE == IsValidAcl(dacl))
        {
            /* Handle Error */
            secure = false;
            break;
        }

        /* dir is writable by others */
        for (DWORD iter = 0; secure && iter < dacl->AceCount; ++iter)
        {
            ACE_HEADER* aceHeader = M_NULLPTR;
            if (GetAce(dacl, iter, C_CAST(void**, &aceHeader)))
            {
                if (aceHeader->AceType == ACCESS_ALLOWED_ACE_TYPE)
                {
                    PACCESS_ALLOWED_ACE allowedACE = C_CAST(PACCESS_ALLOWED_ACE, aceHeader);
                    ACCESS_MASK accessMask = allowedACE->Mask;
                    PSID aceSID = C_CAST(PSID, &allowedACE->SidStart);
                    if (IsValidSid(aceSID))
                    {
                        if (accessMask & (FILE_GENERIC_WRITE | FILE_APPEND_DATA) &&
                            (!EqualSid(mySid, aceSID) && !is_Secure_Well_Known_SID(aceSID))
                            )
                        {
                            secure = false;
    #if defined (_DEBUG)
                            char* sidString = M_NULLPTR;
                            if (ConvertSidToStringSidA(aceSID, &sidString))
                            {
                                printf("Untrusted SID: %s\n", sidString);
                            }
    #endif //_DEBUG
                        }
    #if defined (_DEBUG)
                        else
                        {
                            char* sidString = M_NULLPTR;
                            if (ConvertSidToStringSidA(aceSID, &sidString))
                            {
                                printf("Trusted SID: %s\n", sidString);
                            }
                        }
    #endif //_DEBUG
                    }
                    else
                    {
                        secure = false;
                    }
                }
                /* Else? What to do about other ACEs?*/
            }
            else
            {
                /* Handle Error */
                secure = false;
            }
        }
    } while (0);

    SecureZeroMemory(mySidStr, strlen(mySidStr));
    safe_Free(mySidStr);
    if (mySid)
    {
        SecureZeroMemory(mySid, GetLengthSid(mySid));
        LocalFree(mySid);
        mySid = M_NULLPTR;
    }
    if (secdesc)
    {
        SecureZeroMemory(secdesc, secdesclen);
        LocalFree(secdesc);
        secdesc = M_NULLPTR;
    }

    return secure;
}

#define MAX_SYMLINKS_IN_PATH 5

/* This function requires Windows style seperators (\) to function properly! */
static bool internal_OS_Is_Directory_Secure(const char* fullpath, unsigned int num_symlinks)
{
    char* path_copy = M_NULLPTR;
    char** dirs = M_NULLPTR;
    ssize_t num_of_dirs = 1;
    bool secure = true;
    ssize_t i = 0;

    if (!fullpath || fullpath[0] == '\0')
    {
        /* Handle error */
        return false;
    }
    //Check for drive letter format:
    if (fullpath[1] != ':' && fullpath[2] != '\\')
    {
        /* Handle error */
        return false;
    }

    if (num_symlinks > MAX_SYMLINKS_IN_PATH)
    {
        /* Could be a symlink loop */
        /* Handle error */
        return false;
    }

    path_copy = strdup(fullpath);
    if (!path_copy)
    {
        /* Handle error */
        return false;
    }

    /* Figure out how far it is to the root */
    char* path_parent = path_copy;
    for (; !is_Root_Path(path_parent) &&
        (strcmp(path_parent, ".") != 0);
        path_parent = win_dirname(path_parent))
    {
        num_of_dirs++;
        if (num_of_dirs == SSIZE_MAX)
        {
            //stop before overflow to return an error
            break;
        }
    }
    if (num_of_dirs == SSIZE_MAX)
    {
        /* out of room to compare this many directories deep */
        return false;
    }
    /* Now num_of_dirs indicates # of dirs we must check */
    safe_Free(path_copy)
    dirs = C_CAST(char**, malloc(C_CAST(size_t, num_of_dirs) * sizeof(char*)));
    if (!dirs)
    {
        /* Handle error */
        return false;
    }
    dirs[num_of_dirs - 1] = strdup(fullpath);
    if (!dirs[num_of_dirs - 1])
    {
        /* Handle error */
        safe_Free(dirs)
        return false;
    }
    path_copy = strdup(fullpath);
    if (!path_copy)
    {
        /* Handle error */
        safe_Free(dirs)
        return false;
    }

    /* Now fill the dirs array */
    path_parent = path_copy;
    for (i = num_of_dirs - 2; i >= 0; i--)
    {
        path_parent = win_dirname(path_parent);
        dirs[i] = strdup(path_parent);
        if (!dirs[i])
        {
            /* Handle error */
            secure = false;
            break;
        }
    }
    safe_Free(path_copy);
    if (!secure)
    {
        //cleanup dirs before returning error
        //i is set to when strdup failed and was decrementing to zero/negatives
        //so use it + 1 as the starting point to go through and cleanup the stored directories to free up memory
        for (ssize_t cleanup = i + 1; cleanup <= num_of_dirs; cleanup++)
        {
            safe_Free(dirs[cleanup])
        }
        safe_Free(dirs)
        return secure;
    }

    /*
     * Traverse from the root to the fullpath,
     * checking permissions along the way.
     */
    for (i = 0; i < num_of_dirs; i++)
    {
        fileAttributes* attrs = M_NULLPTR;
        char* dirptr = dirs[i];
        bool appendedTrailingSlash = false;
        if (is_Root_Path(dirptr))
        {
            //append a trailing \ to the end before attempting to get the attributes
            size_t newlen = strlen(dirptr) + 2;
            dirptr = C_CAST(char*, calloc(newlen, sizeof(char)));
            if (dirptr)
            {
                memcpy(dirptr, dirs[i], newlen - 2);
                common_String_Concat(dirptr, newlen, "\\");
                appendedTrailingSlash = true;
            }
            else
            {
                dirptr = dirs[i];
            }
        }
        attrs = os_Get_File_Attributes_By_Name(dirptr);
        if (!attrs)
        {
            /* handle error */
            secure = false;
            if (appendedTrailingSlash)
            {
                safe_Free(dirptr)
            }
            break;
        }
        if (attrs->numberOfLinks > MAX_SYMLINKS_IN_PATH)
        {
            /* handle error */
            secure = false;
            free_File_Attributes(&attrs);
            if (appendedTrailingSlash)
            {
                safe_Free(dirptr)
            }
            break;
        }

        if (attrs->fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
        {
            /* This is a link. Need to resolve it and test it */
            HANDLE link = CreateFileA(dirptr, GENERIC_READ, FILE_SHARE_READ, M_NULLPTR, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT, M_NULLPTR);
            if (link != INVALID_HANDLE_VALUE)
            {
                PREPARSE_DATA_BUFFER reparseData = C_CAST(PREPARSE_DATA_BUFFER, malloc(sizeof(REPARSE_DATA_BUFFER) + MAX_PATH));
                if (reparseData)
                {
                    DWORD bytesRead = 0;
                    memset(reparseData, 0, sizeof(REPARSE_DATA_BUFFER) + MAX_PATH);
                    if (DeviceIoControl(link, FSCTL_GET_REPARSE_POINT, M_NULLPTR, 0, &reparseData, sizeof(reparseData), &bytesRead, M_NULLPTR))
                    {
                        size_t bufferSize = 0;
                        char* reparsePath = M_NULLPTR;
#if defined (__STDC_SECURE_LIB__)
                        wcstombs_s(&bufferSize, M_NULLPTR, 0, C_CAST(wchar_t*, reparseData->SymbolicLinkReparseBuffer.PathBuffer), 0);
#else
                        bufferSize = wcstombs(M_NULLPTR, C_CAST(wchar_t*, reparseData->SymbolicLinkReparseBuffer.PathBuffer), 0);
#endif
                        if (bufferSize > 0)
                        {
                            reparsePath = C_CAST(char*, malloc(bufferSize));
                            if (reparsePath)
                            {
#if defined (__STDC_SECURE_LIB__)
                                wcstombs_s(M_NULLPTR, reparsePath, bufferSize, C_CAST(wchar_t*, reparseData->SymbolicLinkReparseBuffer.PathBuffer), bufferSize);
#else
                                wcstombs(reparsePath, C_CAST(wchar_t*, reparseData->SymbolicLinkReparseBuffer.PathBuffer), bufferSize);
#endif
                                num_symlinks++;
                                bool recurseSecure = internal_OS_Is_Directory_Secure(reparsePath, num_symlinks);
                                num_symlinks--;
                                if (!recurseSecure)
                                {
                                    secure = false;
                                }
                                safe_Free(reparsePath)
                            }
                            else
                            {
                                secure = false;
                            }
                        }
                        else
                        {
                            secure = false;
                        }
                    }
                    else
                    {
                        secure = false;
                    }
                    safe_Free(reparseData)
                }
                else
                {
                    secure = false;
                }
                CloseHandle(link);
                if (secure)
                {
                    continue;
                }
                else
                {
                    if (appendedTrailingSlash)
                    {
                        safe_Free(dirptr)
                    }
                    break;
                }
            }
            else
            {
                secure = false;
                free_File_Attributes(&attrs);
                if (appendedTrailingSlash)
                {
                    safe_Free(dirptr)
                }
                break;
            }
        }

        if (appendedTrailingSlash)
        {
            safe_Free(dirptr)
        }

        if (!(attrs->fileFlags & FILE_ATTRIBUTE_DIRECTORY))
        {
            /* Not a directory */
            secure = false;
            free_File_Attributes(&attrs);
            break;
        }

        secure = is_Folder_Secure(attrs->winSecurityDescriptor);

        free_File_Attributes(&attrs);
    }

    for (i = 0; i < num_of_dirs; i++)
    {
        safe_Free(dirs[i])
    }

    safe_Free(dirs)
    return secure;
}

bool os_Is_Directory_Secure(const char* fullpath)
{
    //This was implemented as close as possible to https://wiki.sei.cmu.edu/confluence/display/c/FIO15-C.+Ensure+that+file+operations+are+performed+in+a+secure+directory
    unsigned int num_symlinks = 0;
    return internal_OS_Is_Directory_Secure(fullpath, num_symlinks);
}

bool os_Directory_Exists(const char * const pathToCheck)
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    memset(&fileAttributes, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
    if (win_File_Attributes_By_Name(pathToCheck, &fileAttributes))
    {
        if (fileAttributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

eReturnValues os_Create_Directory(const char * filePath)
{
    BOOL returnValue;
    size_t filePathLength = (strlen(filePath) + 1) * sizeof(TCHAR);
    TCHAR *pathNameBuf = C_CAST(TCHAR*, calloc(filePathLength, sizeof(TCHAR)));
    if (pathNameBuf)
    {
        CONST TCHAR* pathName = &pathNameBuf[0];
        _stprintf_s(pathNameBuf, filePathLength, TEXT("%hs"), filePath);

        returnValue = CreateDirectory(pathName, NULL);
        if (returnValue == FALSE)
        {
#if defined (_DEBUG)
            print_Windows_Error_To_Screen(GetLastError());
#endif
            return FAILURE;
        }
        else
        {
            return SUCCESS;
        }
    }
    else
    {
        return MEMORY_FAILURE;
    }
}

bool os_File_Exists(const char * const filetoCheck)
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    memset(&fileAttributes, 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
    if (win_File_Attributes_By_Name(filetoCheck, &fileAttributes))
    {
        if (!(fileAttributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

eReturnValues get_Full_Path(const char * pathAndFile, char fullPath[OPENSEA_PATH_MAX])
{
    if (!pathAndFile || !fullPath)
    {
        return BAD_PARAMETER;
    }
    size_t localPathAndFileLength = (strlen(pathAndFile) + 1) * sizeof(TCHAR);
    TCHAR* localpathAndFileBuf = C_CAST(TCHAR*, calloc(localPathAndFileLength, sizeof(TCHAR)));
    TCHAR fullPathOutput[OPENSEA_PATH_MAX] = { 0 };
    if (!localpathAndFileBuf)
    {
        return false;
    }
    CONST TCHAR* localpathAndFile = &localpathAndFileBuf[0];
    _stprintf_s(localpathAndFileBuf, localPathAndFileLength, TEXT("%hs"), pathAndFile);
    DWORD result = GetFullPathName(localpathAndFile, OPENSEA_PATH_MAX, fullPathOutput, NULL);
    if (result == 0)
    {
        //fatal error
        return FAILURE;
    }
    else if (result > OPENSEA_PATH_MAX)
    {
        //This case is when the function returns a buffer size that is required to hold the full path name.
        //DWORD lastError = GetLastError();
        return FAILURE;
    }
#if defined (UNICODE)
    snprintf(fullPath, OPENSEA_PATH_MAX, "%ws", fullPathOutput);
#else
    snprintf(fullPath, OPENSEA_PATH_MAX, "%hs", fullPathOutput);
#endif
    //Check if this file even exists to make this more like the behavior of the POSIX realpath function.
    if (!os_File_Exists(fullPath))
    {
        memset(fullPath, 0, OPENSEA_PATH_MAX);
        return FAILURE;
    }
    //Future work, use this API instead??? https://learn.microsoft.com/en-us/windows/win32/api/pathcch/nf-pathcch-pathcchcanonicalizeex
    return SUCCESS;
}

static bool CompareAces(PACL pAcl1, PACL pAcl2) 
{
    if (pAcl1 == M_NULLPTR || pAcl2 == M_NULLPTR)
    {
        return false;
    }
    if (pAcl1->AceCount != pAcl2->AceCount)
    {
        return false;
    }

    for (DWORD i = 0; i < pAcl1->AceCount; i++) 
    {
        PACE_HEADER pAce1 = M_NULLPTR;
        PACE_HEADER pAce2 = M_NULLPTR;

        if (!GetAce(pAcl1, i, C_CAST(LPVOID*, &pAce1)))
        {
            return false;
        }

        if (!GetAce(pAcl2, i, C_CAST(LPVOID*, &pAce2)))
        {
            return false;
        }

        // Compare the ACEs
        if (pAce1->AceType != pAce2->AceType ||
            pAce1->AceFlags != pAce2->AceFlags ||
            pAce1->AceSize != pAce2->AceSize ||
            memcmp((PVOID)((PBYTE)pAce1 + sizeof(ACE_HEADER)),
                (PVOID)((PBYTE)pAce2 + sizeof(ACE_HEADER)),
                pAce1->AceSize - sizeof(ACE_HEADER)) != 0) 
        {
            return false;
        }
    }

    return true;
}

//This function is meant to be called after reading and converting SIDs and DACL to a string.
//So if a caller wants to do it based off file name alone, or file name and FILE *handle comparison, they can read those, then call this.
bool exact_Compare_SIDS_And_DACL_Strings(const char* sidsAndDACLstr1, const char* sidsAndDACLstr2)
{
    bool match = false;
    //This function is not just doing strcmp because that will not work.
    //convert these back to the raw structures, then compare them.
    if (sidsAndDACLstr1 && sidsAndDACLstr2)
    {
        PSECURITY_DESCRIPTOR secDesc1 = M_NULLPTR, secDesc2 = M_NULLPTR;
        ULONG secDesc1len = 0, secDesc2len = 0;
        PSID owner1 = M_NULLPTR, owner2 = M_NULLPTR;
        PSID group1 = M_NULLPTR, group2 = M_NULLPTR;
        PACL dacl1 = M_NULLPTR, dacl2 = M_NULLPTR;
        BOOL validdesc1 = FALSE, validdesc2 = FALSE;
        BOOL validown1 = FALSE, validown2 = FALSE;
        BOOL validgroup1 = FALSE, validgroup2 = FALSE;
        BOOL validdacl1 = FALSE, validdacl2 = FALSE;
        BOOL defaultown1 = FALSE, defaultown2 = FALSE;
        BOOL defaultgroup1 = FALSE, defaultgroup2 = FALSE;
        BOOL defaultdacl1 = FALSE, defaultdacl2 = FALSE;
        BOOL dacl1present = FALSE, dacl2present = FALSE;
        if (TRUE == ConvertStringSecurityDescriptorToSecurityDescriptorA(sidsAndDACLstr1, SDDL_REVISION, &secDesc1, &secDesc1len))
        {
            validdesc1 = IsValidSecurityDescriptor(secDesc1);
        }
        if (TRUE == ConvertStringSecurityDescriptorToSecurityDescriptorA(sidsAndDACLstr2, SDDL_REVISION, &secDesc2, &secDesc2len))
        {
            validdesc2 = IsValidSecurityDescriptor(secDesc2);
        }
        if (validdesc1 && validdesc2)
        {
            if (GetSecurityDescriptorOwner(secDesc1, &owner1, &defaultown1))
            {
                validown1 = IsValidSid(owner1);
            }
            if (GetSecurityDescriptorGroup(secDesc1, &group1, &defaultgroup1))
            {
                validgroup1 = IsValidSid(group1);
            }
            if (GetSecurityDescriptorOwner(secDesc2, &owner2, &defaultown2))
            {
                validown2 = IsValidSid(owner2);
            }
            if (GetSecurityDescriptorGroup(secDesc2, &group2, &defaultgroup2))
            {
                validgroup2 = IsValidSid(group2);
            }
            //Now that we have read all of the possible values....lets compare everything
            if ((validown1 && validown2 && defaultown1 == defaultown2 && EqualSid(owner1, owner2))
                && (validgroup1 && validgroup2 && defaultgroup1 == defaultgroup2 && EqualSid(group1, group2))
                )
            {
                //owner and group are the same.
                //Compare the dacls
                if (GetSecurityDescriptorDacl(secDesc1, &dacl1present, &dacl1, &defaultdacl1))
                {
                    validdacl1 = IsValidAcl(dacl1);
                }
                if (GetSecurityDescriptorDacl(secDesc2, &dacl2present, &dacl2, &defaultdacl2))
                {
                    validdacl2 = IsValidAcl(dacl2);
                }
                if (validdacl1 && validdacl2 && dacl1present == dacl2present && defaultdacl1 == defaultdacl2 && CompareAces(dacl1, dacl2))
                {
                    match = true;
                }
            }
        }
        if (secDesc1)
        {
            SecureZeroMemory(secDesc1, secDesc1len);
            LocalFree(secDesc1);
            secDesc1 = M_NULLPTR;
        }
        if (secDesc2)
        {
            SecureZeroMemory(secDesc2, secDesc2len);
            LocalFree(secDesc2);
            secDesc2 = M_NULLPTR;
        }
    }
    return match;
}

static uint16_t get_Console_Default_Color(void)
{
    static uint16_t defaultConsoleAttributes = UINT16_MAX;
    if (defaultConsoleAttributes == UINT16_MAX)
    {
        CONSOLE_SCREEN_BUFFER_INFO defaultInfo;
        memset(&defaultInfo, 0, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &defaultInfo))
        {
            defaultConsoleAttributes = defaultInfo.wAttributes;
        }
        else
        {
            //set defaultColorVal to zero
            defaultConsoleAttributes = 0;
        }
    }
    return defaultConsoleAttributes;
}

static uint16_t get_Console_Current_Color(void)
{
    uint16_t currentConsoleAttributes = UINT16_MAX;
    CONSOLE_SCREEN_BUFFER_INFO currentInfo;
    memset(&currentInfo, 0, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &currentInfo))
    {
        currentConsoleAttributes = currentInfo.wAttributes;
    }
    else
    {
        //set defaultColorVal to zero
        currentConsoleAttributes = 0;
    }
    return currentConsoleAttributes;
}

void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
{
    if (foregroundBackground)
    {
        set_Console_Foreground_Background_Colors(consoleColor, CONSOLE_COLOR_CURRENT);
    }
    else
    {
        set_Console_Foreground_Background_Colors(CONSOLE_COLOR_CURRENT, consoleColor);
    }
}

void set_Console_Foreground_Background_Colors(eConsoleColors foregroundColor, eConsoleColors backgroundColor)
{
    static bool defaultsSet = false;
    static WORD defaultColorValue = 0;
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD theColor = 0;
    if (!defaultsSet)
    {
        //First time we are setting colors backup the default settings so they can be restored properly later.
        defaultColorValue = get_Console_Default_Color();
        defaultsSet = true;
    }
    theColor = get_Console_Current_Color();//get current colors after defaults are setup.
    //now change what is requested
    if (foregroundColor != CONSOLE_COLOR_CURRENT)
    {
        //clear out foreground bits, then set the requested color
        theColor &= 0xFFF0;//foreground are lowest 4 bits
        switch (foregroundColor)
        {
        case CONSOLE_COLOR_BLUE:
            theColor |= FOREGROUND_BLUE;
            break;
        case CONSOLE_COLOR_BRIGHT_BLUE:
            theColor |= FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_GREEN:
            theColor |= FOREGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_GREEN:
            theColor |= FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_RED:
            theColor |= FOREGROUND_RED;
            break;
        case CONSOLE_COLOR_BRIGHT_RED:
            theColor |= FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_BLACK:
            theColor |= 0;//this should mean no colors or black
            break;
        case CONSOLE_COLOR_YELLOW:
            theColor |= FOREGROUND_RED | FOREGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_YELLOW:
            theColor |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_CYAN:
            theColor |= FOREGROUND_BLUE | FOREGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_CYAN:
            theColor |= FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_MAGENTA:
            theColor |= FOREGROUND_BLUE | FOREGROUND_RED;
            break;
        case CONSOLE_COLOR_BRIGHT_MAGENTA:
            theColor |= FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_BRIGHT_WHITE:
            theColor |= FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_GRAY:
            theColor |= FOREGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_WHITE:
            theColor |= FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
            break;
        case CONSOLE_COLOR_DEFAULT://fall through to default
        default:
            theColor |= (defaultColorValue & 0x000F);
            break;
        }
    }
    if (backgroundColor != CONSOLE_COLOR_CURRENT)
    {
        //clear out background bits, then set the requested color
        theColor &= 0xFF0F;//foreground are middle 4 bits
        switch (backgroundColor)
        {
        case CONSOLE_COLOR_BLUE:
            theColor |= BACKGROUND_BLUE;
            break;
        case CONSOLE_COLOR_BRIGHT_BLUE:
            theColor |= BACKGROUND_BLUE | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_GREEN:
            theColor |= BACKGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_GREEN:
            theColor |= BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_RED:
            theColor |= BACKGROUND_RED;
            break;
        case CONSOLE_COLOR_BRIGHT_RED:
            theColor |= BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_BLACK:
            theColor |= 0;//this should mean no colors or black
            break;
        case CONSOLE_COLOR_YELLOW:
            theColor |= BACKGROUND_RED | BACKGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_YELLOW:
            theColor |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_CYAN:
            theColor |= BACKGROUND_BLUE | BACKGROUND_GREEN;
            break;
        case CONSOLE_COLOR_BRIGHT_CYAN:
            theColor |= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_MAGENTA:
            theColor |= BACKGROUND_BLUE | BACKGROUND_RED;
            break;
        case CONSOLE_COLOR_BRIGHT_MAGENTA:
            theColor |= BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_BRIGHT_WHITE:
            theColor |= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_GRAY:
            theColor |= BACKGROUND_INTENSITY;
            break;
        case CONSOLE_COLOR_WHITE:
            theColor |= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
            break;
        case CONSOLE_COLOR_DEFAULT://fall through to default
        default:
            theColor |= (defaultColorValue & 0x00F0);
            break;
        }
    }
    SetConsoleTextAttribute(consoleHandle, theColor);
    return;
}

eArchitecture get_Compiled_Architecture(void)
{
    //check which compiler we're using to use it's preprocessor definitions
    //intel compiler and visual c seem to use the same preprocessor definitions
    #if defined _MSC_VER || defined __INTEL_COMPILER
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
    #elif defined __MINGW32__ || defined __MINGW64__ || defined __clang__ || defined __GNUC__ //I'm guessing that all these compilers will use the same macro definitions since the sourceforge pages aren't 100% complete (clang I'm unsure about)
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
        #elif defined __MINGW32__ && defined _X86_
            return OPENSEA_ARCH_X86;
        #else
            return OPENSEA_ARCH_UNKNOWN;
        #endif
    #else
        return OPENSEA_ARCH_UNKNOWN;
    #endif
}

#if defined __clang__
// clang specific because behavior can differ even with the GCC diagnostic being "compatible"
// https ://clang.llvm.org/docs/UsersManual.html#controlling-diagnostics-via-pragmas
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#elif defined __GNUC__ && __GNUC__ >= 3
//temporarily disable the warning for unused function
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif //__clang__, __GNUC__
static eEndianness calculate_Endianness(void)
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
#if defined __clang__
#pragma clang diagnostic pop
#elif defined __GNUC__ && __GNUC__ >= 3
//reenable the unused function warning
#pragma GCC diagnostic pop
#endif //__clang__, __GNUC__

//from what I can tell, windows basically only ever runs on little endian, but this complete check SHOULD MOSTLY work
eEndianness get_Compiled_Endianness(void)
{
    //check which compiler we're using to use it's preprocessor definitions
    //intel compiler and visual c seem to use the same preprocessor definitions
    #if defined _MSC_VER || defined __INTEL_COMPILER
        #if defined _M_X64 || defined _M_AMD64
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_ALPHA
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_ARM || defined _M_ARMT
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_IX86
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_IA64
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined _M_PPC //32bits I'm guessing - TJE
            return OPENSEA_BIG_ENDIAN;
        #elif defined __LITTLE_ENDIAN__
            return OPENSEA_LITTLE_ENDIAN;
        #elif defined __BIG_ENDIAN__
            return OPENSEA_BIG_ENDIAN;
        #else
            return calculate_Endianness();
        #endif
    #elif defined __MINGW32__ || defined __MINGW64__ || defined __clang__ || defined __GNUC__ //I'm guessing that all these compilers will use the same macro definitions since the sourceforge pages aren't 100% complete (clang I'm unsure about)
        #if defined (__BYTE_ORDER__)
            #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
                return OPENSEA_BIG_ENDIAN;
            #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
                return OPENSEA_LITTLE_ENDIAN;
            #else
                return calculate_Endianness();
            #endif
        #elif defined (__BIG_ENDIAN__)
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
}

bool is_Windows_Vista_Or_Higher(void)
{
    bool isWindowsVistaOrHigher = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 6;
    windowsVersionInfo.dwMinorVersion = 0;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindowsVistaOrHigher = true;
    }
    return isWindowsVistaOrHigher;
}

bool is_Windows_7_Or_Higher(void)
{
    bool isWindows7OrHigher = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 6;
    windowsVersionInfo.dwMinorVersion = 1;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows7OrHigher = true;
    }
    return isWindows7OrHigher;
}

bool is_Windows_8_Or_Higher(void)
{
    bool isWindows8OrHigher = false;
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 6;
    windowsVersionInfo.dwMinorVersion = 2;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows8OrHigher = true;
    }
    return isWindows8OrHigher;
}

bool is_Windows_8_One_Or_Higher(void)
{
    bool isWindows81OrHigher = false;
    //Will only work if app manifested correctly
    /*
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 6;
    windowsVersionInfo.dwMinorVersion = 3;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows81OrHigher = true;
    }
    /*/
    //This uses our method below that checks the version from kernel.dll
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion > 6)
    {
        //Win10 or higher
        isWindows81OrHigher = true;
    }
    else if (windowsVersion.versionType.windowsVersion.majorVersion == 6 && windowsVersion.versionType.windowsVersion.minorVersion >= 3)
    {
        isWindows81OrHigher = true;
    }
    //*/
    return isWindows81OrHigher;
}

bool is_Windows_10_Or_Higher(void)
{
    bool isWindows10OrHigher = false;
    //Will only work if app manifested correctly
    /*
    OSVERSIONINFOEX windowsVersionInfo;
    DWORDLONG conditionMask = 0;
    memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
    conditionMask = 0;
    //Now get the actual version of the OS...start at windows vista and work forward from there.
    windowsVersionInfo.dwMajorVersion = 10;
    windowsVersionInfo.dwMinorVersion = 0;
    conditionMask = VerSetConditionMask(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    conditionMask = VerSetConditionMask(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    if (VerifyVersionInfo(&windowsVersionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask))
    {
        isWindows10OrHigher = true;
    }
    /*/
    //This uses our method below that checks the version from kernel.dll
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10)
    {
        //Win10 or higher
        isWindows10OrHigher = true;
    }
    //*/
    return isWindows10OrHigher;
}

bool is_Windows_10_Version_1507_Or_Higher(void)
{
    bool isWindows10_1507OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 10240)
    {
        //Win10 or higher
        isWindows10_1507OrHigher = true;
    }
    return isWindows10_1507OrHigher;
}

bool is_Windows_10_Version_1511_Or_Higher(void)
{
    bool isWindows10_1511OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 10586)
    {
        //Win10 or higher
        isWindows10_1511OrHigher = true;
    }
    return isWindows10_1511OrHigher;
}

bool is_Windows_10_Version_1607_Or_Higher(void)
{
    bool isWindows10_1607OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 14393)
    {
        //Win10 or higher
        isWindows10_1607OrHigher = true;
    }
    return isWindows10_1607OrHigher;
}

bool is_Windows_10_Version_1703_Or_Higher(void)
{
    bool isWindows10_1703OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 15063)
    {
        //Win10 or higher
        isWindows10_1703OrHigher = true;
    }
    return isWindows10_1703OrHigher;
}

bool is_Windows_10_Version_1709_Or_Higher(void)
{
    bool isWindows10_1709OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 16299)
    {
        //Win10 or higher
        isWindows10_1709OrHigher = true;
    }
    return isWindows10_1709OrHigher;
}

bool is_Windows_10_Version_1803_Or_Higher(void)
{
    bool isWindows10_1803OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 17134)
    {
        //Win10 or higher
        isWindows10_1803OrHigher = true;
    }
    return isWindows10_1803OrHigher;
}

bool is_Windows_10_Version_1809_Or_Higher(void)
{
    bool isWindows10_1809OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 17763)
    {
        //Win10 or higher
        isWindows10_1809OrHigher = true;
    }
    return isWindows10_1809OrHigher;
}

bool is_Windows_10_Version_1903_Or_Higher(void)
{
    bool isWindows10_1903OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 18362)
    {
        //Win10 or higher
        isWindows10_1903OrHigher = true;
    }
    return isWindows10_1903OrHigher;
}

bool is_Windows_10_Version_1909_Or_Higher(void)
{
    bool isWindows10_1909OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 18363)
    {
        //Win10 or higher
        isWindows10_1909OrHigher = true;
    }
    return isWindows10_1909OrHigher;
}

bool is_Windows_10_Version_2004_Or_Higher(void)
{
    bool isWindows10_2004OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19041)
    {
        //Win10 or higher
        isWindows10_2004OrHigher = true;
    }
    return isWindows10_2004OrHigher;
}

bool is_Windows_10_Version_20H2_Or_Higher(void)
{
    bool isWindows10_20H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19042)
    {
        //Win10 or higher
        isWindows10_20H2OrHigher = true;
    }
    return isWindows10_20H2OrHigher;
}

bool is_Windows_10_Version_21H1_Or_Higher(void)
{
    bool isWindows10_21H1OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19043)
    {
        //Win10 or higher
        isWindows10_21H1OrHigher = true;
    }
    return isWindows10_21H1OrHigher;
}

bool is_Windows_10_Version_21H2_Or_Higher(void)
{
    bool isWindows10_21H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19044)
    {
        //Win10 or higher
        isWindows10_21H2OrHigher = true;
    }
    return isWindows10_21H2OrHigher;
}

bool is_Windows_10_Version_22H2_Or_Higher(void)
{
    bool isWindows10_22H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 19045)
    {
        //Win10 or higher
        isWindows10_22H2OrHigher = true;
    }
    return isWindows10_22H2OrHigher;
}

bool is_Windows_11_Version_21H2_Or_Higher(void)
{
    bool isWindows10_21H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 22000)
    {
        //Win10 or higher
        isWindows10_21H2OrHigher = true;
    }
    return isWindows10_21H2OrHigher;
}

bool is_Windows_11_Version_22H2_Or_Higher(void)
{
    bool isWindows11_22H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 22621)
    {
        isWindows11_22H2OrHigher = true;
    }
    return isWindows11_22H2OrHigher;
}

bool is_Windows_11_Version_23H2_Or_Higher(void)
{
    bool isWindows11_23H2OrHigher = false;
    OSVersionNumber windowsVersion;
    memset(&windowsVersion, 0, sizeof(OSVersionNumber));
    get_Operating_System_Version_And_Name(&windowsVersion, NULL);
    if (windowsVersion.versionType.windowsVersion.majorVersion >= 10 && windowsVersion.versionType.windowsVersion.buildNumber >= 22631)
    {
        isWindows11_23H2OrHigher = true;
    }
    return isWindows11_23H2OrHigher;
}

bool is_Windows_Server_OS(void)
{
    static bool checkedForServer = false;
    static bool isWindowsServer = false;
    if (!checkedForServer)
    {
        OSVERSIONINFOEX windowsVersionInfo;
        DWORDLONG conditionMask = 0;
        memset(&windowsVersionInfo, 0, sizeof(OSVERSIONINFOEX));
        windowsVersionInfo.wProductType = VER_NT_WORKSTATION;
        conditionMask = VerSetConditionMask(conditionMask, VER_PRODUCT_TYPE, VER_EQUAL);//checking for equality on the workstation attribute
        if (VerifyVersionInfo(&windowsVersionInfo, VER_PRODUCT_TYPE, conditionMask))
        {
            //Windows workstation/desktop
            isWindowsServer = false;
        }
        else
        {
            //Windows server/domain controller (which is a windows server version)
            isWindowsServer = true;
        }
        checkedForServer = true;
    }
    return isWindowsServer;
}
//Consider checking for multiple keys in the future.
bool is_Windows_PE(void)
{
    static bool regread = false;
    static bool isWindowsPE = false;
    //To figure out if running in PE requires checking the registry. There is not another known way to look for this.
    //All other functions to read or check versions will return build numbers that match the equivalent non-PE version of windows, which can still be useful for determining supported APIs
    //We will be looking for one of these keys:
    /*
    -HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\WinPE
    -HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\MiniNT
    -HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\MiniNT
    //NOTE: 3 below are not used today. Only these top two are checked at this time
    -HKEY_LOCAL_MACHINE\system\currentcontrolset\control\PEBootType (PE2)
    -HKEY_LOCAL_MACHINE\system\currentcontrolset\control\PEFirmwareType (PE4)
    -HKEY_LOCAL_MACHINE\System\Setup\SystemSetupInProgress
    */
    if (!regread)
    {
        HKEY keyHandle;
        if (!isWindowsPE && ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WinPE"), 0, KEY_READ, &keyHandle))
        {
    #if defined (_DEBUG)
            printf("Found HKLM\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WinPE\n");
    #endif
            isWindowsPE = true;
            RegCloseKey(keyHandle);
            keyHandle = 0;
        }
        if (!isWindowsPE && ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\CurrentControlSet\\Control\\MiniNT"), 0, KEY_READ, &keyHandle))
        {
    #if defined (_DEBUG)
            printf("Found HKLM\\SYSTEM\\CurrentControlSet\\Control\\MiniNT\n");
    #endif
            isWindowsPE = true;
            RegCloseKey(keyHandle);
            keyHandle = 0;
        }
        if (!isWindowsPE && ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\ControlSet001\\Control\\MiniNT"), 0, KEY_READ, &keyHandle))
        {
    #if defined (_DEBUG)
            printf("Found HKLM\\SYSTEM\\ControlSet001\\Control\\MiniNT\n");
    #endif
            isWindowsPE = true;
            RegCloseKey(keyHandle);
            keyHandle = 0;
        }
        regread = true;
    }
    return isWindowsPE;
}

//possible MSDN api calls: 
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa370624(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/hardware/ff561910(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms724451(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms725492(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms724358(v=vs.85).aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx //Used this one since it is supposed to work!
//From MSDN: To obtain the full version number for the operating system, call the GetFileVersionInfo function on one of the system DLLs, such as Kernel32.dll, then call VerQueryValue to obtain the \\StringFileInfo\\<lang><codepage>\\ProductVersion subblock of the file version information.
//NOTE: Changed from GetFileVersionInfo as it did not report the correct up to date information that was expected.
//      Using RtlGetVersion from ntdll.dll instead
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(POSVERSIONINFOEXW);

eReturnValues get_Operating_System_Version_And_Name(ptrOSVersionNumber versionNumber, char *operatingSystemName)
{
    eReturnValues ret = SUCCESS;
    static DWORD winMajor = 0;
    static DWORD winMinor = 0;
    static DWORD winBuild = 0;
    static bool readVersionFromNTDLL = false;
    bool isWindowsServer = is_Windows_Server_OS();
    bool isWindowsPE = is_Windows_PE();
    memset(versionNumber, 0, sizeof(OSVersionNumber));
    versionNumber->osVersioningIdentifier = OS_WINDOWS;
    
    if (!readVersionFromNTDLL)
    {
        static CONST TCHAR ntdll[] = TEXT("\\ntdll.dll");
        TCHAR *systemPathBuf = C_CAST(TCHAR*, calloc(OPENSEA_PATH_MAX, sizeof(TCHAR)));
        CONST TCHAR *systemPath = &systemPathBuf[0];

        if (!systemPath)
        {
            return MEMORY_FAILURE;
        }

        UINT directoryStringLength = GetSystemDirectory(systemPathBuf, OPENSEA_PATH_MAX);
        if (directoryStringLength > OPENSEA_PATH_MAX || directoryStringLength == 0 || directoryStringLength > OPENSEA_PATH_MAX - sizeof(ntdll) / sizeof(*ntdll))
        {
            //error
            safe_Free(systemPathBuf)
            systemPath = NULL;
            return FAILURE;
        }
        //I'm using this Microsoft provided call to concatenate strings since it will concatenate properly for ansi or wide strings depending on whether UNICODE is set or not - TJE
        if (S_OK != StringCchCat(systemPathBuf, OPENSEA_PATH_MAX, ntdll))
        {
            return FAILURE;
        }

        HMODULE hMod = INVALID_HANDLE_VALUE;
        if (TRUE == GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, systemPathBuf, &hMod))
        {
            if (hMod != INVALID_HANDLE_VALUE)
            {
                RtlGetVersionPtr rtlgetverptr = C_CAST(RtlGetVersionPtr, GetProcAddress(hMod, "RtlGetVersion"));
                if (rtlgetverptr != M_NULLPTR)
                {
                    OSVERSIONINFOEXW osInfo = { 0 };
                    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
                    if (0 == rtlgetverptr(&osInfo))
                    {
                        versionNumber->versionType.windowsVersion.majorVersion = osInfo.dwMajorVersion;
                        versionNumber->versionType.windowsVersion.minorVersion = osInfo.dwMinorVersion;
                        versionNumber->versionType.windowsVersion.buildNumber = osInfo.dwBuildNumber;
                        winMajor = osInfo.dwMajorVersion;
                        winMinor = osInfo.dwMinorVersion;
                        winBuild = osInfo.dwBuildNumber;
                        ret = SUCCESS;
                        readVersionFromNTDLL = true;
                    }
                }
                else
                {
                    ret = FAILURE;
                }
            }
            else
            {
                ret = FAILURE;
            }
        }
        safe_Free(systemPathBuf)
        systemPath = NULL;
    }
    else
    {
        versionNumber->versionType.windowsVersion.majorVersion = winMajor;
        versionNumber->versionType.windowsVersion.minorVersion = winMinor;
        versionNumber->versionType.windowsVersion.buildNumber  = winBuild;
    }

    if (ret == SUCCESS)
    {
        //Now that we know whether or not it's a server version and have gotten the version number, set the appropriate string for the OS.
        if (operatingSystemName)
        {
            switch (versionNumber->versionType.windowsVersion.majorVersion)
            {
            case 10://Win 10 or Server 2016
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 0:
                    if (isWindowsServer)
                    {
                        switch (versionNumber->versionType.windowsVersion.buildNumber)
                        {
                        case 14393:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2016");
                            break;
                        case 17763:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2019");
                            break;
                        case 18362:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 1903");
                            break;
                        case 18363:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 1909");
                            break;
                        case 19041:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 2004");
                            break;
                        case 19042:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server, version 20H2");
                            break;
                        case 20348:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2022");
                            break;
                        default:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows Server Version");
                            break;
                        }
                    }
                    else
                    {
                        switch (versionNumber->versionType.windowsVersion.buildNumber)
                        {
                        case 10240:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1507");
                            break;
                        case 10586:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1511");
                            break;
                        case 14393:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1607");
                            break;
                        case 15063:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1703");
                            break;
                        case 16299:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1709");
                            break;
                        case 17134:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1803");
                            break;
                        case 17763:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1809");
                            break;
                        case 18362:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1903");
                            break;
                        case 18363:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 1909");
                            break;
                        case 19041:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 2004");
                            break;
                        case 19042:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 20H2");
                            break;
                        case 19043:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 21H1");
                            break;
                        case 19044:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 21H2");
                            break;
                        case 19045:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 10, version 22H2");
                            break;
                        case 22000:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 11, version 21H2");
                            break;
                        case 22621:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 11, version 22H2");
                            break;
                        case 22631:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 11, version 23H2");
                            break;
                        default:
                            snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows 10/11 version");
                            break;
                        }
                    }
                    break;
                default:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows Server Version");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows Version");
                    }
                    break;
                }
                break;
            case 6://Vista through 8.1
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 3:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2012 R2");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 8.1");
                    }
                    break;
                case 2:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2012");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 8");
                    }
                    break;
                case 1:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2008 R2");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 7");
                    }
                    break;
                case 0:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2008");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Vista");
                    }
                    break;
                default:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2012 R2 or higher");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 8.1 or higher");
                    }
                    break;
                }
                break;
            case 5://2000 through XP
                switch (versionNumber->versionType.windowsVersion.minorVersion)
                {
                case 2:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2003");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows XP 64-Bit Edition");
                    }
                    break;
                case 1:
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows XP");
                    break;
                case 0:
                    snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows 2000");
                    break;
                default:
                    if (isWindowsServer)
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Windows Server 2003 or higher");
                    }
                    else
                    {
                        snprintf(&operatingSystemName[0], OS_NAME_SIZE, "XP or higher");
                    }
                    break;
                }
                break;
            default:
                snprintf(&operatingSystemName[0], OS_NAME_SIZE, "Unknown Windows OS");
                break;
            }
            if (isWindowsPE)
            {
                common_String_Concat(operatingSystemName, OS_NAME_SIZE, " (PE)");
            }
        }
    }
    return ret;
}

int64_t os_Get_File_Size(FILE *filePtr)
{
    LARGE_INTEGER fileSize;
    intptr_t osfHandle = _get_osfhandle(_fileno(filePtr));
    //convert the fileptr to a HANDLE
    HANDLE fileHandle = C_CAST(HANDLE, osfHandle);
    fileSize.QuadPart = 0;//set to something before we call GetFileSizeEx
    //use GetFileSizeEx to get the size of the file as a 64bit integer
    if (GetFileSizeEx(fileHandle, &fileSize))
    {
        return fileSize.QuadPart;
    }
    else
    {
        return -1;
    }
}

void start_Timer(seatimer_t *timer)
{
    LARGE_INTEGER tempLargeInt;
    tempLargeInt.QuadPart = 0;
    if (TRUE == QueryPerformanceCounter(&tempLargeInt))//according to MSDN this will always return success on XP and later systems
    {
        if (tempLargeInt.QuadPart >= 0)
        {
            timer->timerStart = C_CAST(uint64_t, tempLargeInt.QuadPart);
        }
    }
}

void stop_Timer(seatimer_t *timer)
{
    LARGE_INTEGER tempLargeInt;
    memset(&tempLargeInt, 0, sizeof(LARGE_INTEGER));
    if (TRUE == QueryPerformanceCounter(&tempLargeInt))//according to MSDN this will always return success on XP and later systems
    {
        if (tempLargeInt.QuadPart >= 0)
        {
            timer->timerStop = C_CAST(uint64_t, tempLargeInt.QuadPart);
        }
    }
}

uint64_t get_Nano_Seconds(seatimer_t timer)
{
    LARGE_INTEGER frequency;//clock ticks per second
    uint64_t ticksPerNanosecond = UINT64_C(1000000000);//start with a count of nanoseconds per second
    uint64_t seconds = 0, nanoSeconds = 0;
    memset(&frequency, 0, sizeof(LARGE_INTEGER));
    if (TRUE == QueryPerformanceFrequency(&frequency))
    {
        if (frequency.QuadPart > 0)//no equals since this is used in division and don't want to divide by zero
        {
            ticksPerNanosecond /= C_CAST(uint64_t, frequency.QuadPart);
            seconds = (timer.timerStop - timer.timerStart) / C_CAST(uint64_t, frequency.QuadPart);//converted to nanoseconds later
            nanoSeconds = ((timer.timerStop - timer.timerStart) % C_CAST(uint64_t, frequency.QuadPart)) * ticksPerNanosecond;
            return ((seconds * UINT64_C(1000000000)) + nanoSeconds);
        }
        else
        {
            return UINT64_C(0);
        }
    }
    else 
    {
        return UINT64_C(0);
    }
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

void print_Windows_Error_To_Screen(unsigned int windowsError)
{
    TCHAR *windowsErrorString = NULL;
    //Originally used: MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
    //switched to MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) to keep output consistent with all other verbose output.-TJE
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, windowsError, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), C_CAST(TCHAR*, &windowsErrorString), 0, NULL);
    //This cast is necessary to tell the Windows API to allocate the string, but it is necessary. Without it, this will not work.
    _tprintf_s(TEXT("%u - %s\n"), windowsError, windowsErrorString);
    LocalFree(windowsErrorString);
}

bool is_Running_Elevated(void)
{
    bool isElevated = false;
    HANDLE currentProcess = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &currentProcess))
    {
        TOKEN_ELEVATION elevation;
        DWORD returnedSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(currentProcess, TokenElevation, &elevation, sizeof(elevation), &returnedSize))
        {
            if (elevation.TokenIsElevated)
            {
                isElevated = true;
            }
        }
    }
    if (currentProcess)
    {
        CloseHandle(currentProcess);
    }
    return isElevated;
}

#if defined (ENABLE_READ_USERNAME)
//Gets the user name for who is running the process
//https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getusernamea?redirectedfrom=MSDN
//NOTE: Not using Ex version at this time to avoid linking yet another library. This can be added if necessary, or this doesn't do quite what we want it to do. -TJE
eReturnValues get_Current_User_Name(char **userName)
{
    eReturnValues ret = SUCCESS;
    if (userName)
    {
        DWORD localNameLength = UNLEN + 1;//start with this for input
        TCHAR localName[UNLEN + 1] = { 0 };
        if (TRUE == GetUserName(localName, &localNameLength))
        {
            const char *isAdmin = " (admin)";//This will be concatenated to the string if running as administrator since we only get the user's name in Windows.
            size_t usernameLength = _tcslen(localName) + strlen(isAdmin) + 1;
            *userName = C_CAST(char*, calloc(usernameLength, sizeof(char)));
            if (*userName)
            {
#if defined UNICODE
                size_t charsConverted = 0;
                //convert output to a char string
                if (wcstombs_s(&charsConverted, *userName, usernameLength, localName, usernameLength))
                {
                    safe_Free(*userName)
                    ret = FAILURE;
                }
#else
                //just copy it over after allocating
                if (strcpy_s(*userName, usernameLength, localName))
                {
                    safe_Free(*userName)
                    return FAILURE;
                }
#endif
                if (is_Running_Elevated())
                {
                    if (strcat_s(*userName, usernameLength, isAdmin))
                    {
                        safe_Free(*userName)
                        return FAILURE;
                    }
                }
            }
            else
            {
                ret = FAILURE;
            }
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
#endif //ENABLE_READ_USERNAME

static DWORD get_Input_Console_Default_Mode(void)
{
    static DWORD defaultConsoleMode = UINT32_MAX;
    if (defaultConsoleMode == UINT32_MAX)
    {
        if (FALSE == GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &defaultConsoleMode))
        {
            defaultConsoleMode = 0;
            //From MSFT documentation: https://learn.microsoft.com/en-us/windows/console/setconsolemode?redirectedfrom=MSDN
            //"When a console is created, all input modes except ENABLE_WINDOW_INPUT and ENABLE_VIRTUAL_TERMINAL_INPUT are enabled by default."
            defaultConsoleMode = ENABLE_ECHO_INPUT | ENABLE_INSERT_MODE | ENABLE_LINE_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE;
        }
    }
    return defaultConsoleMode;
}

static bool set_Input_Console_Mode(DWORD mode)
{
    return M_ToBool(SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode));
}

eReturnValues get_Secure_User_Input(const char* prompt, char** userInput, size_t* inputDataLen)
{
    eReturnValues ret = SUCCESS;
    DWORD defaultConMode = get_Input_Console_Default_Mode();
    DWORD conMode = defaultConMode;
    conMode &= C_CAST(DWORD, ~(ENABLE_ECHO_INPUT));
    printf("%s", prompt);
    fflush(stdout);
    //disable echoing typed characters so that
    if (set_Input_Console_Mode(conMode))
    {
        if (getline(userInput, inputDataLen, stdin) <= 0)
        {
            ret = FAILURE;
        }
        else
        {
            //remove newline from the end...convert to a null.
            size_t endofinput = strlen(*userInput);
            if ((*userInput)[endofinput - 1] == '\n')
            {
                (*userInput)[endofinput - 1] = '\0';
            }
        }
    }
    else
    {
        ret = FAILURE;
    }
    set_Input_Console_Mode(defaultConMode);
    printf("\n");
    return ret;
}
