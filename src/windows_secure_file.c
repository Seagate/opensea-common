// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file windows_secure_file.c
// \brief implements secure file API for Windows
//

#include <windows.h> //needed for all the stuff to get the windows version
#include <AclAPI.h>
#include <sddl.h>
#include <io.h> //needed for getting the size of a file in windows
#include <tchar.h>
#include <string.h>

#include "common_types.h"
#include "type_conversion.h"
#include "secure_file.h"
#include "memory_safety.h"
#include "string_utils.h"
#include "error_translation.h"

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

int64_t os_Get_File_Size(FILE* filePtr)
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
        return INT64_C(-1);
    }
}

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
        TCHAR* localPathToCheckBuf = C_CAST(TCHAR*, safe_calloc(pathCheckLength, sizeof(TCHAR)));
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
        safe_Free(C_CAST(void**, &localPathToCheckBuf));
        localPathToCheck = M_NULLPTR;
    }
    return success;
}

static bool win_File_Attributes_By_File(FILE* file, LPBY_HANDLE_FILE_INFORMATION attributes)
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
        TCHAR* localPathToCheckBuf = C_CAST(TCHAR*, safe_calloc(pathCheckLength, sizeof(TCHAR)));
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
        if (ERROR_SUCCESS == GetNamedSecurityInfo(localPathToCheck, SE_FILE_OBJECT, secInfo, &owner, &group, &dacl, M_NULLPTR, &secDescriptor))
        {
            ULONG tempLen = 0;
            LPSTR temp = M_NULLPTR;
            if (TRUE == ConvertSecurityDescriptorToStringSecurityDescriptorA(secDescriptor, SDDL_REVISION, secInfo, &temp, &tempLen))
            {
                /* do not use strdup or strndup here. There are some extra nulls at the end of what Windows allocates that we are preserving in memcpy */
                attrs->winSecurityDescriptor = C_CAST(char*, safe_calloc(tempLen, sizeof(char)));
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

static bool win_Get_File_Security_Info_By_File(FILE* file, fileAttributes* attrs)
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
        if (ERROR_SUCCESS == GetSecurityInfo(msftHandle, SE_FILE_OBJECT, secInfo, &owner, &group, &dacl, M_NULLPTR, &secDescriptor))
        {
            ULONG tempLen = 0;
            LPSTR temp = M_NULLPTR;
            if (TRUE == ConvertSecurityDescriptorToStringSecurityDescriptorA(secDescriptor, SDDL_REVISION, secInfo, &temp, &tempLen))
            {
                /* do not use strdup or strndup here. There are some extra nulls at the end of what Windows allocates that we are preserving in memcpy */
                attrs->winSecurityDescriptor = C_CAST(char*, safe_calloc(tempLen, sizeof(char)));
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
    fileAttributes* attrs = M_NULLPTR;
    struct _stat64 st;
    memset(&st, 0, sizeof(struct _stat64));
    if (filetoCheck && _stat64(filetoCheck, &st) == 0)
    {
        attrs = C_CAST(fileAttributes*, safe_calloc(1, sizeof(fileAttributes)));
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


fileAttributes* os_Get_File_Attributes_By_File(FILE* file)
{
    fileAttributes* attrs = M_NULLPTR;
    struct _stat64 st;
    memset(&st, 0, sizeof(struct _stat64));
    if (file && _fstat64(_fileno(file), &st) == 0)
    {
        attrs = C_CAST(fileAttributes*, safe_calloc(1, sizeof(fileAttributes)));
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
            return M_NULLPTR; //This is stdout, strerr, stdin and not a file that we can get other info on. This is not the correct thing to pass to this function!
        }
        HANDLE msftHandle = C_CAST(HANDLE, _get_osfhandle(fd));//DO NOT CALL CLOSE ON MSFTHANDLE
        if (msftHandle == INVALID_HANDLE_VALUE || C_CAST(intptr_t, msftHandle) == WIN_FD_INVALID)
        {
            return M_NULLPTR;
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
                fileUniqueIDInfo* fileId = C_CAST(fileUniqueIDInfo*, safe_calloc(1, sizeof(fileUniqueIDInfo)));
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
            fileUniqueIDInfo* fileId = C_CAST(fileUniqueIDInfo*, safe_calloc(1, sizeof(fileUniqueIDInfo)));
            if (fileId)
            {
                fileId->volsn = winfileinfo.dwVolumeSerialNumber;
                memcpy(&fileId->fileid[0], &winfileinfo.nFileIndexHigh, sizeof(DWORD));
                memcpy(&fileId->fileid[sizeof(DWORD)], &winfileinfo.nFileIndexLow, sizeof(DWORD));
            }
            return fileId;
        }
    }
    return M_NULLPTR;
}

static char* get_Current_User_SID(void)
{
    char* sidAsString = M_NULLPTR;
    HANDLE hToken = M_NULLPTR;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        DWORD dwSize = 0;
        GetTokenInformation(hToken, TokenUser, M_NULLPTR, 0, &dwSize);
        PTOKEN_USER pUser = C_CAST(PTOKEN_USER, safe_malloc(dwSize));
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
            safe_Free(C_CAST(void**, &pUser));
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
        if (strlen(path) == 2 && safe_isascii(path[0]) && path[1] == ':')
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

static bool is_Folder_Secure(const char* securityDescriptorString)
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
    safe_Free(C_CAST(void**, &mySidStr));
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
    safe_Free(C_CAST(void**, &path_copy));
    dirs = C_CAST(char**, safe_malloc(C_CAST(size_t, num_of_dirs) * sizeof(char*)));
    if (!dirs)
    {
        /* Handle error */
        return false;
    }
    dirs[num_of_dirs - 1] = strdup(fullpath);
    if (!dirs[num_of_dirs - 1])
    {
        /* Handle error */
        safe_Free(C_CAST(void**, &dirs));
        return false;
    }
    path_copy = strdup(fullpath);
    if (!path_copy)
    {
        /* Handle error */
        safe_Free(C_CAST(void**, &dirs));
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
    safe_Free(C_CAST(void**, &path_copy));
    if (!secure)
    {
        //cleanup dirs before returning error
        //i is set to when strdup failed and was decrementing to zero/negatives
        //so use it + 1 as the starting point to go through and cleanup the stored directories to free up memory
        for (ssize_t cleanup = i + 1; cleanup <= num_of_dirs; cleanup++)
        {
            safe_Free(C_CAST(void**, &dirs[cleanup]));
        }
        safe_Free(C_CAST(void**, &dirs));
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
            dirptr = C_CAST(char*, safe_calloc(newlen, sizeof(char)));
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
                safe_Free(C_CAST(void**, &dirptr));
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
                safe_Free(C_CAST(void**, &dirptr));
            }
            break;
        }

        if (attrs->fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
        {
            /* This is a link. Need to resolve it and test it */
            HANDLE link = CreateFileA(dirptr, GENERIC_READ, FILE_SHARE_READ, M_NULLPTR, OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT, M_NULLPTR);
            if (link != INVALID_HANDLE_VALUE)
            {
                PREPARSE_DATA_BUFFER reparseData = C_CAST(PREPARSE_DATA_BUFFER, safe_malloc(sizeof(REPARSE_DATA_BUFFER) + MAX_PATH));
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
                            reparsePath = C_CAST(char*, safe_malloc(bufferSize));
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
                                safe_Free(C_CAST(void**, &reparsePath));
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
                    safe_Free(C_CAST(void**, &reparseData));
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
                        safe_Free(C_CAST(void**, &dirptr));
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
                    safe_Free(C_CAST(void**, &dirptr));
                }
                break;
            }
        }

        if (appendedTrailingSlash)
        {
            safe_Free(C_CAST(void**, &dirptr));
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
        safe_Free(C_CAST(void**, &dirs[i]));
    }

    safe_Free(C_CAST(void**, &dirs));
    return secure;
}

bool os_Is_Directory_Secure(const char* fullpath)
{
    //This was implemented as close as possible to https://wiki.sei.cmu.edu/confluence/display/c/FIO15-C.+Ensure+that+file+operations+are+performed+in+a+secure+directory
    unsigned int num_symlinks = 0;
    return internal_OS_Is_Directory_Secure(fullpath, num_symlinks);
}

bool os_Directory_Exists(const char* const pathToCheck)
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

eReturnValues os_Create_Directory(const char* filePath)
{
    BOOL returnValue;
    size_t filePathLength = (strlen(filePath) + 1) * sizeof(TCHAR);
    TCHAR* pathNameBuf = C_CAST(TCHAR*, safe_calloc(filePathLength, sizeof(TCHAR)));
    if (pathNameBuf)
    {
        CONST TCHAR* pathName = &pathNameBuf[0];
        _stprintf_s(pathNameBuf, filePathLength, TEXT("%hs"), filePath);

        returnValue = CreateDirectory(pathName, M_NULLPTR);
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

bool os_File_Exists(const char* const filetoCheck)
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

eReturnValues get_Full_Path(const char* pathAndFile, char fullPath[OPENSEA_PATH_MAX])
{
    if (!pathAndFile || !fullPath)
    {
        return BAD_PARAMETER;
    }
    size_t localPathAndFileLength = (strlen(pathAndFile) + 1) * sizeof(TCHAR);
    TCHAR* localpathAndFileBuf = C_CAST(TCHAR*, safe_calloc(localPathAndFileLength, sizeof(TCHAR)));
    DECLARE_ZERO_INIT_ARRAY(TCHAR, fullPathOutput, OPENSEA_PATH_MAX);
    if (!localpathAndFileBuf)
    {
        return false;
    }
    CONST TCHAR* localpathAndFile = &localpathAndFileBuf[0];
    _stprintf_s(localpathAndFileBuf, localPathAndFileLength, TEXT("%hs"), pathAndFile);
    DWORD result = GetFullPathName(localpathAndFile, OPENSEA_PATH_MAX, fullPathOutput, M_NULLPTR);
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
