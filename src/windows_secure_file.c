// SPDX-License-Identifier: MPL-2.0

//! \file windows_secure_file.c
//! \brief Windows system specific secure file implementation of secure_file.h functions
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "warning_ctl.h"

DISABLE_WARNING_4255
#include <AclAPI.h>
#include <io.h> //needed for getting the size of a file in windows
#include <sddl.h>
#include <string.h>
#include <tchar.h>
#include <windows.h>
RESTORE_WARNING_4255

#include "common_types.h"
#include "error_translation.h"
#include "io_utils.h"
#include "memory_safety.h"
#include "secure_file.h"
#include "secured_env_vars.h"
#include "string_utils.h"
#include "type_conversion.h"

int64_t os_Get_File_Size(FILE* M_NONNULL filePtr)
{
    if (filePtr != M_NULLPTR)
    {
        LARGE_INTEGER fileSize;
        intptr_t      osfHandle = _get_osfhandle(_fileno(filePtr));
        // convert the fileptr to a HANDLE
        HANDLE fileHandle = C_CAST(HANDLE, osfHandle);
        fileSize.QuadPart = 0; // set to something before we call GetFileSizeEx
        // use GetFileSizeEx to get the size of the file as a 64bit integer
        if (GetFileSizeEx(fileHandle, &fileSize))
        {
            return fileSize.QuadPart;
        }
        else
        {
            return INT64_C(-1);
        }
    }
    return INT64_C(-1);
}

/*
When stdin, stdout, and stderr aren't associated with a stream (for example, in
a Windows application without a console window), the file descriptor values for
these streams are returned from _fileno as the special value -2. Similarly, if
you use a 0, 1, or 2 as the file descriptor parameter instead of the result of a
call to _fileno, _get_osfhandle also returns the special value -2 when the file
descriptor is not associated with a stream, and does not set errno. However,
this is not a valid file handle value, and subsequent calls that attempt to use
it are likely to fail.
*/
#define WIN_FD_INVALID (-2)

static bool win_File_Attributes_By_Name(const char* filename, LPWIN32_FILE_ATTRIBUTE_DATA attributes)
{
    bool success = false;
    if (filename && attributes)
    {
        size_t pathCheckLength     = (safe_strlen(filename) + 1) * sizeof(TCHAR);
        TCHAR* localPathToCheckBuf = M_REINTERPRET_CAST(TCHAR*, safe_calloc(pathCheckLength, sizeof(TCHAR)));
        if (localPathToCheckBuf == M_NULLPTR)
        {
            return false;
        }
        CONST TCHAR* localPathToCheck = &localPathToCheckBuf[0];
        if (0 > _stprintf_s(localPathToCheckBuf, pathCheckLength, TEXT("%hs"), filename))
        {
            safe_free_tchar(&localPathToCheckBuf);
            return false;
        }

        BOOL gotAttributes = GetFileAttributesEx(localPathToCheck, GetFileExInfoStandard, attributes);
        if (MSFT_BOOL_TRUE(gotAttributes))
        {
            success = true;
        }
        safe_free_tchar(&localPathToCheckBuf);
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

static bool win_Get_File_Security_Info_By_Name(const char* filename, fileAttributes* attrs)
{
    bool success = false;
    if (filename && attrs)
    {
        size_t pathCheckLength     = (safe_strlen(filename) + 1) * sizeof(TCHAR);
        TCHAR* localPathToCheckBuf = M_REINTERPRET_CAST(TCHAR*, safe_calloc(pathCheckLength, sizeof(TCHAR)));
        if (localPathToCheckBuf == M_NULLPTR)
        {
            return false;
        }
        _stprintf_s(localPathToCheckBuf, pathCheckLength, TEXT("%hs"), filename);
        CONST TCHAR* localPathToCheck = &localPathToCheckBuf[0];

        // Do not need SACL since it may not be available and is about
        // triggering audits instead of a dacl which defines who has access to a
        // file.
        SECURITY_INFORMATION secInfo =
            OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
        PSECURITY_DESCRIPTOR secDescriptor = M_NULLPTR;
        PSID                 owner         = M_NULLPTR;
        PSID                 group         = M_NULLPTR;
        PACL                 dacl          = M_NULLPTR;
        if (ERROR_SUCCESS == GetNamedSecurityInfo(localPathToCheck, SE_FILE_OBJECT, secInfo, &owner, &group, &dacl,
                                                  M_NULLPTR, &secDescriptor))
        {
            ULONG tempLen = ULONG_C(0);
            LPSTR temp    = M_NULLPTR;
            if (MSFT_BOOL_TRUE(ConvertSecurityDescriptorToStringSecurityDescriptorA(secDescriptor, SDDL_REVISION,
                                                                                    secInfo, &temp, &tempLen)))
            {
                /* do not use strdup or strndup here. There are some extra nulls
                 * at the end of what Windows allocates that we are preserving
                 * in memcpy */
                attrs->winSecurityDescriptor = M_REINTERPRET_CAST(char*, safe_calloc(tempLen, sizeof(char)));
                if (attrs->winSecurityDescriptor)
                {
                    attrs->securityDescriptorStringLength = tempLen;
                    if (0 ==
                        safe_memcpy(attrs->winSecurityDescriptor, attrs->securityDescriptorStringLength, temp, tempLen))
                    {
                        success = true;
                    }
                }
                SecureZeroMemory(temp, tempLen);
                LocalFree(temp);
                temp = M_NULLPTR;
            }
            SecureZeroMemory(secDescriptor, GetSecurityDescriptorLength(secDescriptor));
            LocalFree(secDescriptor);
            secDescriptor = M_NULLPTR;
        }
        safe_free_tchar(&localPathToCheckBuf);
    }
    return success;
}

static bool win_Get_File_Security_Info_By_File(FILE* file, fileAttributes* attrs)
{
    bool success = false;
    if (file && attrs)
    {
        // Do not need SACL since it may not be available and is about
        // triggering audits instead of a dacl which defines who has access to a
        // file.
        SECURITY_INFORMATION secInfo =
            OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;
        PSECURITY_DESCRIPTOR secDescriptor = M_NULLPTR;
        PSID                 owner         = M_NULLPTR;
        PSID                 group         = M_NULLPTR;
        PACL                 dacl          = M_NULLPTR;
        HANDLE               msftHandle    = INVALID_HANDLE_VALUE;
        int                  fd            = _fileno(file);
        if (fd == WIN_FD_INVALID)
        {
            return false;
        }
        msftHandle = C_CAST(HANDLE, _get_osfhandle(fd));
        if (msftHandle == INVALID_HANDLE_VALUE || C_CAST(intptr_t, msftHandle) == WIN_FD_INVALID)
        {
            return false;
        }
        if (ERROR_SUCCESS ==
            GetSecurityInfo(msftHandle, SE_FILE_OBJECT, secInfo, &owner, &group, &dacl, M_NULLPTR, &secDescriptor))
        {
            ULONG tempLen = ULONG_C(0);
            LPSTR temp    = M_NULLPTR;
            if (MSFT_BOOL_TRUE(ConvertSecurityDescriptorToStringSecurityDescriptorA(secDescriptor, SDDL_REVISION,
                                                                                    secInfo, &temp, &tempLen)))
            {
                /* do not use strdup or strndup here. There are some extra nulls
                 * at the end of what Windows allocates that we are preserving
                 * in memcpy */
                attrs->winSecurityDescriptor = M_REINTERPRET_CAST(char*, safe_calloc(tempLen, sizeof(char)));
                if (attrs->winSecurityDescriptor)
                {
                    attrs->securityDescriptorStringLength = tempLen;
                    if (0 ==
                        safe_memcpy(attrs->winSecurityDescriptor, attrs->securityDescriptorStringLength, temp, tempLen))
                    {
                        success = true;
                    }
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

M_NODISCARD fileAttributes* os_Get_File_Attributes_By_Name(const char* M_NONNULL filetoCheck)
{
    fileAttributes* attrs = M_NULLPTR;
    struct _stat64  st;
    M_INITIALIZE_STRUCTURE(&st, sizeof(struct _stat64));
    if (filetoCheck != M_NULLPTR && _stat64(filetoCheck, &st) == 0)
    {
        attrs = M_REINTERPRET_CAST(fileAttributes*, safe_calloc(1, sizeof(fileAttributes)));
        if (attrs != M_NULLPTR)
        {
            WIN32_FILE_ATTRIBUTE_DATA winAttributes;
            M_INITIALIZE_STRUCTURE(&winAttributes, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
            attrs->deviceID             = st.st_dev;
            attrs->inode                = st.st_ino;
            attrs->filemode             = st.st_mode;
            attrs->numberOfLinks        = st.st_nlink;
            attrs->userID               = st.st_uid;
            attrs->groupID              = st.st_gid;
            attrs->representedDeviceID  = st.st_rdev;
            attrs->filesize             = st.st_size;
            attrs->fileLastAccessTime   = st.st_atime;
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

M_NODISCARD fileAttributes* os_Get_File_Attributes_By_File(FILE* M_NONNULL file)
{
    fileAttributes* attrs = M_NULLPTR;
    struct _stat64  st;
    M_INITIALIZE_STRUCTURE(&st, sizeof(struct _stat64));
    if (file != M_NULLPTR && _fstat64(_fileno(file), &st) == 0)
    {
        attrs = M_REINTERPRET_CAST(fileAttributes*, safe_calloc(1, sizeof(fileAttributes)));
        if (attrs != M_NULLPTR)
        {
            BY_HANDLE_FILE_INFORMATION winAttributes;
            M_INITIALIZE_STRUCTURE(&winAttributes, sizeof(BY_HANDLE_FILE_INFORMATION));
            attrs->deviceID             = st.st_dev;
            attrs->inode                = st.st_ino;
            attrs->filemode             = st.st_mode;
            attrs->numberOfLinks        = st.st_nlink;
            attrs->userID               = st.st_uid;
            attrs->groupID              = st.st_gid;
            attrs->representedDeviceID  = st.st_rdev;
            attrs->filesize             = st.st_size;
            attrs->fileLastAccessTime   = st.st_atime;
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

static void safe_free_file_unique_id_info(fileUniqueIDInfo** fileId)
{
    safe_free_core(M_REINTERPRET_CAST(void**, fileId));
}

M_NODISCARD fileUniqueIDInfo* os_Get_File_Unique_Identifying_Information(FILE* M_NONNULL file)
{
    if (file != M_NULLPTR)
    {
        int fd = _fileno(file);   // DO NOT CALL CLOSE ON FD!
        if (fd == WIN_FD_INVALID) //_get_osfhandle says this is a special value
                                  // defined for MSFT for stdin, stdout, stderr
        {
            return M_NULLPTR; // This is stdout, strerr, stdin and not a file
                              // that we can get other info on. This is not the
                              // correct thing to pass to this function!
        }
        HANDLE msftHandle = C_CAST(HANDLE, _get_osfhandle(fd)); // DO NOT CALL CLOSE ON MSFTHANDLE
        if (msftHandle == INVALID_HANDLE_VALUE || C_CAST(intptr_t, msftHandle) == WIN_FD_INVALID)
        {
            return M_NULLPTR;
        }
#if defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_VISTA && !defined(NO_FILE_ID_INFO)
        if (is_Windows_Vista_Or_Higher()) // This ex function is only available
                                          // on Vista and later OSs according to
                                          // MSFT docs
        {
            // try to get ex file info, then if it fails (shouldn't happen) then
            // fall back to old method to get this
            FILE_ID_INFO winfileid;
            M_INITIALIZE_STRUCTURE(&winfileid, sizeof(FILE_ID_INFO));
            if (MSFT_BOOL_TRUE(GetFileInformationByHandleEx(msftHandle, FileIdInfo, &winfileid, sizeof(FILE_ID_INFO))))
            {
                // full 128bit identifier available
                fileUniqueIDInfo* fileId =
                    M_REINTERPRET_CAST(fileUniqueIDInfo*, safe_calloc(1, sizeof(fileUniqueIDInfo)));
                if (fileId != M_NULLPTR)
                {
                    fileId->volsn = winfileid.VolumeSerialNumber;
                    if (0 != safe_memcpy(&fileId->fileid[0], FILE_UNIQUE_ID_ARR_MAX, &winfileid.FileId.Identifier[0],
                                         FILE_UNIQUE_ID_ARR_MAX))
                    {
                        safe_free_file_unique_id_info(&fileId);
                        return M_NULLPTR;
                    }
                }
                return fileId;
            }
        }
#endif // Windows vista API
        BY_HANDLE_FILE_INFORMATION winfileinfo;
        M_INITIALIZE_STRUCTURE(&winfileinfo, sizeof(BY_HANDLE_FILE_INFORMATION));
        if (MSFT_BOOL_TRUE(GetFileInformationByHandle(msftHandle, &winfileinfo)))
        {
            fileUniqueIDInfo* fileId = M_REINTERPRET_CAST(fileUniqueIDInfo*, safe_calloc(1, sizeof(fileUniqueIDInfo)));
            if (fileId != M_NULLPTR)
            {
                fileId->volsn = winfileinfo.dwVolumeSerialNumber;
                if (0 != safe_memcpy(&fileId->fileid[0], FILE_UNIQUE_ID_ARR_MAX, &winfileinfo.nFileIndexHigh,
                                     sizeof(DWORD)) ||
                    0 != safe_memcpy(&fileId->fileid[sizeof(DWORD)], FILE_UNIQUE_ID_ARR_MAX - sizeof(DWORD),
                                     &winfileinfo.nFileIndexLow, sizeof(DWORD)))
                {
                    safe_free_file_unique_id_info(&fileId);
                    return M_NULLPTR;
                }
            }
            return fileId;
        }
    }
    return M_NULLPTR;
}

static M_INLINE void safe_free_token_user(TOKEN_USER** user)
{
    safe_free_core(M_REINTERPRET_CAST(void**, user));
}

static char* get_Current_User_SID(void)
{
    char*  sidAsString = M_NULLPTR;
    HANDLE hToken      = M_NULLPTR;
    BOOL   opened      = OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken);
    if (!opened && GetLastError() == ERROR_NO_TOKEN)
    {
        opened = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
    }
    if (opened)
    {
        DWORD dwSize = DWORD_C(0);
        if (GetTokenInformation(hToken, TokenUser, M_NULLPTR, 0, &dwSize) ||
            GetLastError() != ERROR_INSUFFICIENT_BUFFER || dwSize < sizeof(TOKEN_USER))
        {
            CloseHandle(hToken);
            return M_NULLPTR;
        }
        PTOKEN_USER pUser = M_REINTERPRET_CAST(PTOKEN_USER, safe_malloc(dwSize));
        if (pUser != M_NULLPTR)
        {
            explicit_zeroes(pUser, dwSize);
            DWORD allocatedSize = dwSize;
            if (GetTokenInformation(hToken, TokenUser, pUser, allocatedSize, &dwSize) && IsValidSid(pUser->User.Sid))
            {
                LPSTR pSidString = M_NULLPTR;
                if (ConvertSidToStringSidA(pUser->User.Sid, &pSidString))
                {
                    // dup'ing this because want to use std malloc/free's instead of LocalFree
                    if (0 != safe_strdup(&sidAsString, pSidString))
                    {
                        sidAsString = M_NULLPTR;
                        perror("Failed to duplicate SID string");
                    }
                    LocalFree(pSidString);
                }
            }
            explicit_zeroes(pUser, allocatedSize);
            safe_free_token_user(&pUser);
        }
        CloseHandle(hToken);
    }
    return sidAsString;
}

static bool is_Root_Path(const char* path)
{
    bool isroot = false;
    if (path != M_NULLPTR)
    {
        // Check is for a drive letter + :
        // trailing \ is not checked because it is stripped off by win_dirname
        // function when this is used to figure out number of directories to
        // validate.
        if (safe_strlen(path) == 2 && ((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) &&
            path[1] == ':')
        {
            isroot = true;
        }
    }
    return isroot;
}

/* this expects path seperators to be '\\' otherwise it will fail */
static char* win_dirname(char* path)
{
    if (path != M_NULLPTR)
    {
        char* lastseperator = strrchr(path, '\\');
        if (lastseperator != M_NULLPTR)
        {
            *lastseperator = '\0';
            return path;
        }
        else
        {
            // previously returned ".", but this case does not appear to be hit
            // so changing to NULLPTR so that we do not need a const_cast-TJE
            return M_NULLPTR;
        }
    }
    else
    {
        return M_NULLPTR;
    }
}

// https://learn.microsoft.com/en-us/windows/win32/secauthz/well-known-sids
static bool is_Secure_Well_Known_SID(PSID sid)
{
    return sid != M_NULLPTR && IsValidSid(sid) &&
           (IsWellKnownSid(sid, WinLocalSystemSid) || IsWellKnownSid(sid, WinBuiltinAdministratorsSid));
}

FUNC_ATTR_PRINTF(2, 3) static void set_dir_security_output_error_message(char** outputError, const char* format, ...)
{
    if (outputError != M_NULLPTR)
    {
        va_list args;
        va_start(args, format);

        // If an error message is already present, then we need to free it before
        // creating a new one to prevent a memory leak
        if (*outputError != M_NULLPTR)
        {
            explicit_zeroes(*outputError, safe_strlen(*outputError));
            safe_free(outputError);
        }

        DISABLE_WARNING_FORMAT_NONLITERAL
        int result = vasprintf(outputError, format, args);
        RESTORE_WARNING_FORMAT_NONLITERAL

        va_end(args);
        if (result < 0 && *outputError != M_NULLPTR)
        {
            safe_free(outputError);
        }
    }
}

static bool is_Folder_Secure(PSECURITY_DESCRIPTOR secdesc,
                             const char*          dirptr,
                             PSID                 mySid,
                             PSID                 winTI,
                             bool                 leaf,
                             char**               outputError)
{
    bool secure = true;
    if (secdesc == M_NULLPTR || dirptr == M_NULLPTR || mySid == M_NULLPTR || winTI == M_NULLPTR)
    {
        set_dir_security_output_error_message(outputError,
                                              "Invalid security descriptor, trusted identity, or directory\n");
        return false;
    }
    PSID userSid      = M_NULLPTR;
    BOOL defaultOwner = FALSE;
    PACL dacl         = M_NULLPTR;
    BOOL daclPresent  = FALSE;
    BOOL daclDefault  = FALSE;
    do
    {
        if (MSFT_BOOL_FALSE(IsValidSid(mySid)) || MSFT_BOOL_FALSE(IsValidSid(winTI)))
        {
            /* Handle Error */
            set_dir_security_output_error_message(outputError, "Invalid sid\n");
            secure = false;
            break;
        }
        if (MSFT_BOOL_FALSE(IsValidSecurityDescriptor(secdesc)))
        {
            /* Handle Error */
            set_dir_security_output_error_message(outputError, "Invalid security descriptor\n");
            secure = false;
            break;
        }
        if (MSFT_BOOL_FALSE(GetSecurityDescriptorOwner(secdesc, &userSid, &defaultOwner)))
        {
            /* Handle Error */
            set_dir_security_output_error_message(outputError, "Failed to get security descriptor owner\n");
            secure = false;
            break;
        }
        if (MSFT_BOOL_FALSE(IsValidSid(userSid)))
        {
            /* Handle Error */
            set_dir_security_output_error_message(outputError, "Invalid SID for security descriptor owner\n");
            secure = false;
            break;
        }
        if (MSFT_BOOL_FALSE(EqualSid(mySid, userSid)) && MSFT_BOOL_FALSE(EqualSid(winTI, userSid)) &&
            !is_Secure_Well_Known_SID(userSid))
        {
            char* sidString = M_NULLPTR;
            secure          = false;
            if (MSFT_BOOL_TRUE(ConvertSidToStringSidA(userSid, &sidString)))
            {
                set_dir_security_output_error_message(outputError, "Directory (%s) has untrusted owner: %s\n", dirptr,
                                                      sidString);
                LocalFree(sidString);
            }
            else
            {
                set_dir_security_output_error_message(outputError, "Directory (%s) has untrusted owner\n", dirptr);
            }
            break;
        }

        if (MSFT_BOOL_FALSE(GetSecurityDescriptorDacl(secdesc, &daclPresent, &dacl, &daclDefault)))
        {
            /* Handle Error */
            secure = false;
            set_dir_security_output_error_message(outputError, "Unable to retrieve DACL from security descriptor: %s\n",
                                                  dirptr);
            break;
        }

        if (MSFT_BOOL_FALSE(daclPresent) || dacl == M_NULLPTR)
        {
            /* Missing DACL, so cannot verify permissions */
            secure = false;
            set_dir_security_output_error_message(outputError, "DACL Missing. Cannot verify permissions: %s\n", dirptr);
            break;
        }

        if (MSFT_BOOL_FALSE(IsValidAcl(dacl)))
        {
            /* Handle Error */
            secure = false;
            set_dir_security_output_error_message(outputError, "Invalid DACL received. Cannot verify permissions: %s\n",
                                                  dirptr);
            break;
        }

        /* dir is writable by others */
        for (DWORD iter = DWORD_C(0); secure && iter < dacl->AceCount; ++iter)
        {
            ACE_HEADER* aceHeader = M_NULLPTR;
            if (MSFT_BOOL_TRUE(GetAce(dacl, iter, C_CAST(void**, &aceHeader))))
            {
                if (aceHeader->AceSize < sizeof(ACE_HEADER))
                {
                    secure = false;
                    set_dir_security_output_error_message(outputError, "Directory (%s): truncated ACE %lu\n", dirptr,
                                                          iter);
                    break;
                }
                if ((aceHeader->AceFlags & INHERIT_ONLY_ACE) != 0 || aceHeader->AceType == ACCESS_DENIED_ACE_TYPE)
                {
                    // A deny cannot grant access. Ignoring it may conservatively reject a safe DACL.
                    continue;
                }
                if (aceHeader->AceType == ACCESS_ALLOWED_ACE_TYPE)
                {
                    size_t sidOffset = FIELD_OFFSET(ACCESS_ALLOWED_ACE, SidStart);
                    if (aceHeader->AceSize < sidOffset + GetSidLengthRequired(0))
                    {
                        secure = false;
                        set_dir_security_output_error_message(outputError, "Directory (%s): truncated allow ACE %lu\n",
                                                              dirptr, iter);
                        break;
                    }
                    PACCESS_ALLOWED_ACE allowedACE = C_CAST(PACCESS_ALLOWED_ACE, aceHeader);
                    ACCESS_MASK         accessMask = allowedACE->Mask;
                    PSID                aceSID     = C_CAST(PSID, &allowedACE->SidStart);
                    if (GetSidLengthRequired(*GetSidSubAuthorityCount(aceSID)) > aceHeader->AceSize - sidOffset)
                    {
                        secure = false;
                        set_dir_security_output_error_message(outputError, "Directory (%s): SID exceeds ACE %lu\n",
                                                              dirptr, iter);
                        break;
                    }
                    GENERIC_MAPPING mapping = {FILE_GENERIC_READ, FILE_GENERIC_WRITE, FILE_GENERIC_EXECUTE,
                                               FILE_ALL_ACCESS};
                    MapGenericMask(&accessMask, &mapping);
                    if ((accessMask & ~(M_STATIC_CAST(ACCESS_MASK, FILE_ALL_ACCESS))) != 0)
                    {
                        secure = false;
                        set_dir_security_output_error_message(
                            outputError, "Directory (%s): unsupported access mask 0x%08lx at ACE %lu\n", dirptr,
                            accessMask, iter);
                        break;
                    }
                    // Creating siblings does not authorize replacing an existing protected child.
                    ACCESS_MASK mutation =
                        DELETE | WRITE_DAC | WRITE_OWNER | FILE_DELETE_CHILD | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA;
                    if (leaf)
                    {
                        mutation |= FILE_ADD_FILE | FILE_ADD_SUBDIRECTORY;
                    }
                    if (MSFT_BOOL_TRUE(IsValidSid(aceSID)))
                    {
                        if ((accessMask & mutation) != 0 &&
                            (MSFT_BOOL_FALSE(EqualSid(mySid, aceSID)) && MSFT_BOOL_FALSE(EqualSid(winTI, aceSID)) &&
                             !is_Secure_Well_Known_SID(aceSID)))
                        {
                            char* sidString = M_NULLPTR;
                            secure          = false;
                            if (MSFT_BOOL_TRUE(ConvertSidToStringSidA(aceSID, &sidString)))
                            {
                                set_dir_security_output_error_message(
                                    outputError,
                                    "Directory (%s): ACE %lu SID %s grants mutation rights 0x%08lx (flags 0x%02x)\n",
                                    dirptr, iter, sidString, accessMask & mutation, aceHeader->AceFlags);
                                LocalFree(sidString);
                            }
                            else
                            {
                                set_dir_security_output_error_message(
                                    outputError, "Directory (%s): ACE %lu grants mutation rights to an untrusted SID\n",
                                    dirptr, iter);
                            }
                        }
                    }
                    else
                    {
                        set_dir_security_output_error_message(
                            outputError, "Invalid ACE in DACL. Directory (%s) cannot be trusted\n", dirptr);
                        secure = false;
                    }
                }
                else
                {
                    secure = false;
                    set_dir_security_output_error_message(
                        outputError, "Directory (%s): unsupported effective ACE type 0x%02x at index %lu\n", dirptr,
                        aceHeader->AceType, iter);
                }
            }
            else
            {
                /* Handle Error */
                secure = false;
                set_dir_security_output_error_message(outputError, "Directory (%s): cannot read ACE %lu\n", dirptr,
                                                      iter);
            }
        }
    } while (0);

    return secure;
}

static bool win_Inspect_Directory(const char* path, PSID mySid, PSID trustedInstaller, bool leaf, char** outputError)
{
    bool                 secure     = false;
    WCHAR*               widepath   = M_NULLPTR;
    PSECURITY_DESCRIPTOR descriptor = M_NULLPTR;
    HANDLE               directory  = INVALID_HANDLE_VALUE;
    int                  length     = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, path, -1, M_NULLPTR, 0);
    if (length <= 0)
    {
        set_dir_security_output_error_message(outputError, "Cannot decode directory path (Windows error %lu)\n",
                                              GetLastError());
        return false;
    }
    widepath = M_REINTERPRET_CAST(WCHAR*, safe_calloc(M_STATIC_CAST(size_t, length), sizeof(WCHAR)));
    if (widepath == M_NULLPTR || MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, path, -1, widepath, length) == 0)
    {
        set_dir_security_output_error_message(outputError, "Cannot allocate or convert directory path\n");
        safe_free_core(M_REINTERPRET_CAST(void**, &widepath));
        return false;
    }
    WCHAR root[]    = {widepath[0], L':', L'\\', L'\0'};
    UINT  driveType = GetDriveTypeW(root);
    if (driveType != DRIVE_FIXED && driveType != DRIVE_REMOVABLE && driveType != DRIVE_RAMDISK)
    {
        set_dir_security_output_error_message(outputError, "Unsupported or non-local filesystem: %s\n", path);
        safe_free_core(M_REINTERPRET_CAST(void**, &widepath));
        return false;
    }
    directory = CreateFileW(widepath, READ_CONTROL | FILE_READ_ATTRIBUTES,
                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, M_NULLPTR, OPEN_EXISTING,
                            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, M_NULLPTR);
    if (directory == INVALID_HANDLE_VALUE)
    {
        set_dir_security_output_error_message(outputError, "Cannot inspect directory (%s), Windows error %lu\n", path,
                                              GetLastError());
    }
    else
    {
        BY_HANDLE_FILE_INFORMATION attributes;
        if (!GetFileInformationByHandle(directory, &attributes))
        {
            set_dir_security_output_error_message(outputError, "Cannot read opened directory (%s), Windows error %lu\n",
                                                  path, GetLastError());
        }
        else if ((attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            set_dir_security_output_error_message(outputError, "Not a directory: %s\n", path);
        }
        else if ((attributes.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
        {
            // Reparse tags are not all symbolic links. Never decode an unvalidated union as one.
            set_dir_security_output_error_message(outputError,
                                                  "Reparse-point directory assessment is unsupported: %s\n", path);
        }
        else
        {
            DWORD flags = 0;
            if (!GetVolumeInformationByHandleW(directory, M_NULLPTR, 0, M_NULLPTR, M_NULLPTR, &flags, M_NULLPTR, 0) ||
                (flags & FILE_PERSISTENT_ACLS) == 0)
            {
                set_dir_security_output_error_message(
                    outputError, "Filesystem does not provide supported persistent ACLs: %s\n", path);
            }
            else
            {
                DWORD result =
                    GetSecurityInfo(directory, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
                                    M_NULLPTR, M_NULLPTR, M_NULLPTR, M_NULLPTR, &descriptor);
                if (result != ERROR_SUCCESS)
                {
                    set_dir_security_output_error_message(
                        outputError, "Cannot read directory security (%s), Windows error %lu\n", path, result);
                }
                else
                {
                    secure = is_Folder_Secure(descriptor, path, mySid, trustedInstaller, leaf, outputError);
                }
            }
        }
        CloseHandle(directory);
    }
    if (descriptor != M_NULLPTR)
    {
        LocalFree(descriptor);
    }
    safe_free_core(M_REINTERPRET_CAST(void**, &widepath));
    return secure;
}

/* This function requires Windows style seperators (\) to function properly! */
static bool internal_OS_Is_Directory_Secure(const char* fullpath, char** outputError)
{
    char*   path_copy   = M_NULLPTR;
    char**  dirs        = M_NULLPTR;
    ssize_t num_of_dirs = SSIZE_T_C(1);
    bool    secure      = true;
    ssize_t i           = SSIZE_T_C(0);
    errno_t error       = 0;

    if (!fullpath || safe_strnlen(fullpath, OPENSEA_PATH_MAX) < 3 ||
        safe_strnlen(fullpath, OPENSEA_PATH_MAX) == OPENSEA_PATH_MAX)
    {
        set_dir_security_output_error_message(outputError, "Invalid or overlong Windows directory path\n");
        return false;
    }
    // Check for drive letter format:
    if (!((fullpath[0] >= 'A' && fullpath[0] <= 'Z') || (fullpath[0] >= 'a' && fullpath[0] <= 'z')) ||
        fullpath[1] != ':' || fullpath[2] != '\\' || strchr(fullpath + 2, ':') != M_NULLPTR ||
        strchr(fullpath, '/') != M_NULLPTR)
    {
        set_dir_security_output_error_message(outputError,
                                              "Only absolute local drive-letter directory paths are supported\n");
        return false;
    }

    for (const char* component = fullpath + 3; *component != '\0';)
    {
        const char* end = strchr(component, '\\');
        size_t      len = end != M_NULLPTR ? M_STATIC_CAST(size_t, end - component) : strlen(component);
        if (len == 0 || component[len - 1] == '.' || component[len - 1] == ' ')
        {
            set_dir_security_output_error_message(
                outputError, "Directory path must not contain empty, dot, or ambiguous components\n");
            return false;
        }
        component += len;
        if (*component == '\\')
        {
            ++component;
        }
    }

    error = safe_strdup(&path_copy, fullpath);
    if (error != 0 || path_copy == M_NULLPTR)
    {
        set_dir_security_output_error_message(outputError, "Cannot allocate directory path copy\n");
        return false;
    }
    size_t pathlen = strlen(path_copy);
    if (pathlen >= 3 && path_copy[pathlen - 1] == '\\')
    {
        path_copy[pathlen - 1] = '\0';
    }

    /* Figure out how far it is to the root */
    char* path_parent = path_copy;
    for (; !is_Root_Path(path_parent) && (strcmp(path_parent, ".") != 0); path_parent = win_dirname(path_parent))
    {
        num_of_dirs++;
        if (num_of_dirs == SSIZE_MAX)
        {
            // stop before overflow to return an error
            break;
        }
    }
    if (num_of_dirs == SSIZE_MAX)
    {
        set_dir_security_output_error_message(outputError, "Too many directory components\n");
        safe_free(&path_copy);
        return false;
    }
    /* Now num_of_dirs indicates # of dirs we must check */
    safe_free(&path_copy);
    dirs = M_REINTERPRET_CAST(char**, safe_calloc(C_CAST(size_t, num_of_dirs), sizeof(char*)));
    if (dirs == M_NULLPTR)
    {
        set_dir_security_output_error_message(outputError, "Cannot allocate directory list\n");
        return false;
    }
    error = safe_strdup(&dirs[num_of_dirs - SSIZE_T_C(1)], fullpath);
    if (error != 0 || dirs[num_of_dirs - SSIZE_T_C(1)] == M_NULLPTR)
    {
        set_dir_security_output_error_message(outputError, "Cannot allocate directory entry\n");
        safe_free(M_REINTERPRET_CAST(void**, &dirs));
        return false;
    }
    pathlen = strlen(dirs[num_of_dirs - 1]);
    if (pathlen >= 3 && dirs[num_of_dirs - 1][pathlen - 1] == '\\')
    {
        dirs[num_of_dirs - 1][pathlen - 1] = '\0';
    }
    error = safe_strdup(&path_copy, dirs[num_of_dirs - 1]);
    if (error != 0 || path_copy == M_NULLPTR)
    {
        set_dir_security_output_error_message(outputError, "Cannot allocate directory traversal copy\n");
        safe_free(&dirs[num_of_dirs - SSIZE_T_C(1)]);
        safe_free(M_REINTERPRET_CAST(void**, &dirs));
        return false;
    }

    /* Now fill the dirs array */
    path_parent = path_copy;
    for (i = num_of_dirs - SSIZE_T_C(2); i >= SSIZE_T_C(0); i--)
    {
        path_parent = win_dirname(path_parent);
        error       = safe_strdup(&dirs[i], path_parent);
        if (error != 0 || dirs[i] == M_NULLPTR)
        {
            /* Handle error */
            secure = false;
            break;
        }
    }
    safe_free(&path_copy);
    if (!secure)
    {
        set_dir_security_output_error_message(outputError, "Cannot allocate directory ancestor\n");
        for (ssize_t cleanup = 0; cleanup < num_of_dirs; cleanup++)
        {
            safe_free(&dirs[cleanup]);
        }
        safe_free(M_REINTERPRET_CAST(void**, &dirs));
        return secure;
    }

    char* currentUser      = get_Current_User_SID();
    PSID  mySid            = M_NULLPTR;
    PSID  trustedInstaller = M_NULLPTR;
    if (currentUser == M_NULLPTR || !ConvertStringSidToSidA(currentUser, &mySid) ||
        !ConvertStringSidToSidA("S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464", &trustedInstaller))
    {
        set_dir_security_output_error_message(outputError, "Cannot determine effective Windows security identity\n");
        secure = false;
    }
    /*
     * Traverse from the root to the fullpath,
     * checking permissions along the way.
     */
    for (i = 0; i < num_of_dirs && secure; i++)
    {
        char* dirptr                = dirs[i];
        bool  appendedTrailingSlash = false;
        if (is_Root_Path(dirptr))
        {
            // append a trailing \ to the end before attempting to get the
            // attributes
            size_t newlen = safe_strlen(dirptr) + 2;
            dirptr        = M_REINTERPRET_CAST(char*, safe_calloc(newlen, sizeof(char)));
            if (dirptr != M_NULLPTR)
            {
                if (0 != safe_memcpy(dirptr, newlen, dirs[i], newlen - 2))
                {
                    secure = false;
                    set_dir_security_output_error_message(outputError, "Cannot copy volume root path\n");
                    safe_free(&dirptr);
                    break;
                }
                if (0 != safe_strcat(dirptr, newlen, "\\"))
                {
                    secure = false;
                    set_dir_security_output_error_message(outputError, "Cannot append volume root separator\n");
                    safe_free(&dirptr);
                    break;
                }
                appendedTrailingSlash = true;
            }
            else
            {
                secure = false;
                set_dir_security_output_error_message(outputError, "Cannot allocate volume root path\n");
                break;
            }
        }
#if defined(_DEBUG)
        printf("dirptr = %s\n", dirptr);
#endif //_DEBUG

        secure = win_Inspect_Directory(dirptr, mySid, trustedInstaller, i == num_of_dirs - 1, outputError);
        if (appendedTrailingSlash)
        {
            safe_free(&dirptr);
        }
    }

    for (i = 0; i < num_of_dirs; i++)
    {
        safe_free(&dirs[i]);
    }

    safe_free(M_REINTERPRET_CAST(void**, &dirs));
    safe_free(&currentUser);
    if (mySid != M_NULLPTR)
    {
        LocalFree(mySid);
    }
    if (trustedInstaller != M_NULLPTR)
    {
        LocalFree(trustedInstaller);
    }
    return secure;
}

M_NODISCARD bool os_Is_Directory_Secure(const char* M_NONNULL fullpath, char** outputError)
{
    // This was implemented as close as possible to
    // https://wiki.sei.cmu.edu/confluence/display/c/FIO15-C.+Ensure+that+file+operations+are+performed+in+a+secure+directory
    return internal_OS_Is_Directory_Secure(fullpath, outputError);
}

bool os_Directory_Exists(const char* M_NONNULL pathToCheck)
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    M_INITIALIZE_STRUCTURE(&fileAttributes, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
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

M_PARAM_RO(1) eReturnValues os_Create_Directory(const char* M_NONNULL filePath)
{
    BOOL   returnValue    = FALSE;
    size_t filePathLength = (safe_strlen(filePath) + 1) * sizeof(TCHAR);
    TCHAR* pathNameBuf    = M_REINTERPRET_CAST(TCHAR*, safe_calloc(filePathLength, sizeof(TCHAR)));
    if (pathNameBuf != M_NULLPTR)
    {
        CONST TCHAR* pathName = &pathNameBuf[0];
        _stprintf_s(pathNameBuf, filePathLength, TEXT("%hs"), filePath);

        returnValue = CreateDirectory(pathName, M_NULLPTR);
        safe_free_tchar(&pathNameBuf);
        if (MSFT_BOOL_FALSE(returnValue))
        {
#if defined(_DEBUG)
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

// TODO: Need to pass in a security descriptor with proper access information
//  We will need owner/group IDs that match the current user
//  DACL that makes sense to use for the user.
//  Ignoring SACL for now since that serves a different purpose at this time.
M_PARAM_RO(1) eReturnValues os_Create_Secure_Directory(const char* M_NONNULL filePath)
{
    return os_Create_Directory(filePath);
}

bool os_File_Exists(const char* M_NONNULL filetoCheck)
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    M_INITIALIZE_STRUCTURE(&fileAttributes, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
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

M_PARAM_RO(1)
M_PARAM_WO(2)
eReturnValues get_Full_Path(const char* M_NONNULL pathAndFile, char fullPath[M_NONNULL_ARRAY OPENSEA_PATH_MAX])
{
    DISABLE_NONNULL_COMPARE
    if (pathAndFile == M_NULLPTR || fullPath == M_NULLPTR)
    {
        return BAD_PARAMETER;
    }
    RESTORE_NONNULL_COMPARE
    size_t localPathAndFileLength = (safe_strlen(pathAndFile) + 1) * sizeof(TCHAR);
    TCHAR* localpathAndFileBuf    = M_REINTERPRET_CAST(TCHAR*, safe_calloc(localPathAndFileLength, sizeof(TCHAR)));
    DECLARE_ZERO_INIT_ARRAY(TCHAR, fullPathOutput, OPENSEA_PATH_MAX);
    if (localpathAndFileBuf == M_NULLPTR)
    {
        return FAILURE;
    }
    CONST TCHAR* localpathAndFile = &localpathAndFileBuf[0];
    if (0 > _stprintf_s(localpathAndFileBuf, localPathAndFileLength, TEXT("%hs"), pathAndFile))
    {
        perror("Error formatting path and file string before looking up full pathname.");
        safe_free_tchar(&localpathAndFileBuf);
        return MEMORY_FAILURE;
    }
    DWORD result = GetFullPathName(localpathAndFile, OPENSEA_PATH_MAX, fullPathOutput, M_NULLPTR);
    if (result == 0 || result > OPENSEA_PATH_MAX)
    {
        // fatal error
        safe_free_tchar(&localpathAndFileBuf);
        return FAILURE;
    }
#if defined(UNICODE)
    // NOTE: Microsoft accepts either ls (standard) or ws (microsoft specific).
    //       If this causes any warnings in MSVC, add an ifdef to use ws for
    //       that compiler.-TJE Microsoft also has a S extension for strings to
    //       specify single char with wprintf and wide char for printf that may
    //       also be used as needed-TJE
    if (0 > snprintf_err_handle(fullPath, OPENSEA_PATH_MAX, "%ls", fullPathOutput))
#else
#    if defined(_MSC_VER)
    // Microsoft uses hs but this is not standard and using %s is the standard
    if (0 > snprintf_err_handle(fullPath, OPENSEA_PATH_MAX, "%hs", fullPathOutput))
#    else
    if (0 != safe_strcpy(fullPath, OPENSEA_PATH_MAX, fullPathOutput))
#    endif
#endif
    {
        perror("Error copying full path output in Windows secure path");
    }
    // Check if this file even exists to make this more like the behavior of the
    // POSIX realpath function.
    if (!os_File_Exists(fullPath) && !os_Directory_Exists(fullPath))
    {
        explicit_zeroes(fullPath, OPENSEA_PATH_MAX);
        safe_free_tchar(&localpathAndFileBuf);
        return FAILURE;
    }
    safe_free_tchar(&localpathAndFileBuf);
    // Future work, use this API instead???
    // https://learn.microsoft.com/en-us/windows/win32/api/pathcch/nf-pathcch-pathcchcanonicalizeex
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

    for (DWORD i = DWORD_C(0); i < pAcl1->AceCount; i++)
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
        if (pAce1->AceType != pAce2->AceType || pAce1->AceFlags != pAce2->AceFlags ||
            pAce1->AceSize != pAce2->AceSize ||
            memcmp((PVOID)((PBYTE)pAce1 + sizeof(ACE_HEADER)), (PVOID)((PBYTE)pAce2 + sizeof(ACE_HEADER)),
                   pAce1->AceSize - sizeof(ACE_HEADER)) != 0)
        {
            return false;
        }
    }

    return true;
}

// This function is meant to be called after reading and converting SIDs and
// DACL to a string. So if a caller wants to do it based off file name alone, or
// file name and FILE *handle comparison, they can read those, then call this.
bool exact_Compare_SIDS_And_DACL_Strings(const char* M_NONNULL sidsAndDACLstr1, const char* M_NONNULL sidsAndDACLstr2)
{
    bool match = false;
    // This function is not just doing strcmp because that will not work.
    // convert these back to the raw structures, then compare them.
    if (sidsAndDACLstr1 != M_NULLPTR && sidsAndDACLstr2 != M_NULLPTR)
    {
        PSECURITY_DESCRIPTOR secDesc1      = M_NULLPTR;
        PSECURITY_DESCRIPTOR secDesc2      = M_NULLPTR;
        ULONG                secDesc1len   = ULONG_C(0);
        ULONG                secDesc2len   = ULONG_C(0);
        PSID                 owner1        = M_NULLPTR;
        PSID                 owner2        = M_NULLPTR;
        PSID                 group1        = M_NULLPTR;
        PSID                 group2        = M_NULLPTR;
        PACL                 dacl1         = M_NULLPTR;
        PACL                 dacl2         = M_NULLPTR;
        BOOL                 validdesc1    = FALSE;
        BOOL                 validdesc2    = FALSE;
        BOOL                 validown1     = FALSE;
        BOOL                 validown2     = FALSE;
        BOOL                 validgroup1   = FALSE;
        BOOL                 validgroup2   = FALSE;
        BOOL                 validdacl1    = FALSE;
        BOOL                 validdacl2    = FALSE;
        BOOL                 defaultown1   = FALSE;
        BOOL                 defaultown2   = FALSE;
        BOOL                 defaultgroup1 = FALSE;
        BOOL                 defaultgroup2 = FALSE;
        BOOL                 defaultdacl1  = FALSE;
        BOOL                 defaultdacl2  = FALSE;
        BOOL                 dacl1present  = FALSE;
        BOOL                 dacl2present  = FALSE;
        if (MSFT_BOOL_TRUE(ConvertStringSecurityDescriptorToSecurityDescriptorA(sidsAndDACLstr1, SDDL_REVISION,
                                                                                &secDesc1, &secDesc1len)))
        {
            validdesc1 = IsValidSecurityDescriptor(secDesc1);
        }
        if (MSFT_BOOL_TRUE(ConvertStringSecurityDescriptorToSecurityDescriptorA(sidsAndDACLstr2, SDDL_REVISION,
                                                                                &secDesc2, &secDesc2len)))
        {
            validdesc2 = IsValidSecurityDescriptor(secDesc2);
        }
        if (validdesc1 && validdesc2)
        {
            if (MSFT_BOOL_TRUE(GetSecurityDescriptorOwner(secDesc1, &owner1, &defaultown1)))
            {
                validown1 = IsValidSid(owner1);
            }
            if (MSFT_BOOL_TRUE(GetSecurityDescriptorGroup(secDesc1, &group1, &defaultgroup1)))
            {
                validgroup1 = IsValidSid(group1);
            }
            if (MSFT_BOOL_TRUE(GetSecurityDescriptorOwner(secDesc2, &owner2, &defaultown2)))
            {
                validown2 = IsValidSid(owner2);
            }
            if (MSFT_BOOL_TRUE(GetSecurityDescriptorGroup(secDesc2, &group2, &defaultgroup2)))
            {
                validgroup2 = IsValidSid(group2);
            }
            // Now that we have read all of the possible values....lets compare
            // everything
            if ((validown1 && validown2 && defaultown1 == defaultown2 && MSFT_BOOL_TRUE(EqualSid(owner1, owner2))) &&
                (validgroup1 && validgroup2 && defaultgroup1 == defaultgroup2 &&
                 MSFT_BOOL_TRUE(EqualSid(group1, group2))))
            {
                // owner and group are the same.
                // Compare the dacls
                if (MSFT_BOOL_TRUE(GetSecurityDescriptorDacl(secDesc1, &dacl1present, &dacl1, &defaultdacl1)))
                {
                    validdacl1 = IsValidAcl(dacl1);
                }
                if (MSFT_BOOL_TRUE(GetSecurityDescriptorDacl(secDesc2, &dacl2present, &dacl2, &defaultdacl2)))
                {
                    validdacl2 = IsValidAcl(dacl2);
                }
                if (validdacl1 && validdacl2 && dacl1present == dacl2present && defaultdacl1 == defaultdacl2 &&
                    CompareAces(dacl1, dacl2))
                {
                    match = true;
                }
            }
        }
        if (secDesc1 != M_NULLPTR)
        {
            SecureZeroMemory(secDesc1, secDesc1len);
            LocalFree(secDesc1);
            secDesc1 = M_NULLPTR;
        }
        if (secDesc2 != M_NULLPTR)
        {
            SecureZeroMemory(secDesc2, secDesc2len);
            LocalFree(secDesc2);
            secDesc2 = M_NULLPTR;
        }
    }
    return match;
}
