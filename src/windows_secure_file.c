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
// \file windows_secure_file.c
// \brief implements secure file API for Windows
//

#include "predef_env_detect.h"

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

#if defined(HAVE_NTIFS)
#    include <ntifs.h>
#else
// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_reparse_data_buffer
// Defining this ourselves to be able to access this.
typedef struct sREPARSE_DATA_BUFFER
{
    ULONG  ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    union
    {
        struct
        {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            ULONG  Flags;
            WCHAR  PathBuffer[1];
        } SymbolicLinkReparseBuffer;
        struct
        {
            USHORT SubstituteNameOffset;
            USHORT SubstituteNameLength;
            USHORT PrintNameOffset;
            USHORT PrintNameLength;
            WCHAR  PathBuffer[1];
        } MountPointReparseBuffer;
        struct
        {
            UCHAR DataBuffer[1];
        } GenericReparseBuffer;
    } DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;
#endif // HAVE_NTIFS

static M_INLINE void safe_free_reparse_data_buf(REPARSE_DATA_BUFFER** reparse)
{
    safe_Free(M_REINTERPRET_CAST(void**, reparse));
}

int64_t os_Get_File_Size(FILE* filePtr)
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
        if (!localPathToCheckBuf)
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
            if (TRUE == ConvertSecurityDescriptorToStringSecurityDescriptorA(secDescriptor, SDDL_REVISION, secInfo,
                                                                             &temp, &tempLen))
            {
                /* do not use strdup or strndup here. There are some extra nulls
                 * at the end of what Windows allocates that we are preserving
                 * in memcpy */
                attrs->winSecurityDescriptor = M_REINTERPRET_CAST(char*, safe_calloc(tempLen, sizeof(char)));
                if (attrs->winSecurityDescriptor)
                {
                    attrs->securityDescriptorStringLength = tempLen;
                    safe_memcpy(attrs->winSecurityDescriptor, attrs->securityDescriptorStringLength, temp, tempLen);
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
            if (TRUE == ConvertSecurityDescriptorToStringSecurityDescriptorA(secDescriptor, SDDL_REVISION, secInfo,
                                                                             &temp, &tempLen))
            {
                /* do not use strdup or strndup here. There are some extra nulls
                 * at the end of what Windows allocates that we are preserving
                 * in memcpy */
                attrs->winSecurityDescriptor = M_REINTERPRET_CAST(char*, safe_calloc(tempLen, sizeof(char)));
                if (attrs->winSecurityDescriptor)
                {
                    attrs->securityDescriptorStringLength = tempLen;
                    safe_memcpy(attrs->winSecurityDescriptor, attrs->securityDescriptorStringLength, temp, tempLen);
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

M_NODISCARD fileAttributes* os_Get_File_Attributes_By_Name(const char* filetoCheck)
{
    fileAttributes* attrs = M_NULLPTR;
    struct _stat64  st;
    safe_memset(&st, sizeof(struct _stat64), 0, sizeof(struct _stat64));
    if (filetoCheck && _stat64(filetoCheck, &st) == 0)
    {
        attrs = M_REINTERPRET_CAST(fileAttributes*, safe_calloc(1, sizeof(fileAttributes)));
        if (attrs)
        {
            WIN32_FILE_ATTRIBUTE_DATA winAttributes;
            safe_memset(&winAttributes, sizeof(WIN32_FILE_ATTRIBUTE_DATA), 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
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

M_NODISCARD fileAttributes* os_Get_File_Attributes_By_File(FILE* file)
{
    fileAttributes* attrs = M_NULLPTR;
    struct _stat64  st;
    safe_memset(&st, sizeof(struct _stat64), 0, sizeof(struct _stat64));
    if (file && _fstat64(_fileno(file), &st) == 0)
    {
        attrs = M_REINTERPRET_CAST(fileAttributes*, safe_calloc(1, sizeof(fileAttributes)));
        if (attrs)
        {
            BY_HANDLE_FILE_INFORMATION winAttributes;
            safe_memset(&winAttributes, sizeof(BY_HANDLE_FILE_INFORMATION), 0, sizeof(BY_HANDLE_FILE_INFORMATION));
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

M_NODISCARD fileUniqueIDInfo* os_Get_File_Unique_Identifying_Information(FILE* file)
{
    if (file)
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
#if defined(WIN_API_TARGET_VERSION) && WIN_API_TARGET_VERSION >= WIN_API_TARGET_VISTA
        if (is_Windows_Vista_Or_Higher()) // This ex function is only available
                                          // on Vista and later OSs according to
                                          // MSFT docs
        {
            // try to get ex file info, then if it fails (shouldn't happen) then
            // fall back to old method to get this
            FILE_ID_INFO winfileid;
            safe_memset(&winfileid, sizeof(FILE_ID_INFO), 0, sizeof(FILE_ID_INFO));
            if (TRUE == GetFileInformationByHandleEx(msftHandle, FileIdInfo, &winfileid, sizeof(FILE_ID_INFO)))
            {
                // full 128bit identifier available
                fileUniqueIDInfo* fileId =
                    M_REINTERPRET_CAST(fileUniqueIDInfo*, safe_calloc(1, sizeof(fileUniqueIDInfo)));
                if (fileId)
                {
                    fileId->volsn = winfileid.VolumeSerialNumber;
                    safe_memcpy(&fileId->fileid[0], FILE_UNIQUE_ID_ARR_MAX, &winfileid.FileId.Identifier[0],
                                FILE_UNIQUE_ID_ARR_MAX);
                }
                return fileId;
            }
        }
#endif // Windows vista API
        BY_HANDLE_FILE_INFORMATION winfileinfo;
        safe_memset(&winfileinfo, sizeof(BY_HANDLE_FILE_INFORMATION), 0, sizeof(BY_HANDLE_FILE_INFORMATION));
        if (TRUE == GetFileInformationByHandle(msftHandle, &winfileinfo))
        {
            fileUniqueIDInfo* fileId = M_REINTERPRET_CAST(fileUniqueIDInfo*, safe_calloc(1, sizeof(fileUniqueIDInfo)));
            if (fileId)
            {
                fileId->volsn = winfileinfo.dwVolumeSerialNumber;
                safe_memcpy(&fileId->fileid[0], FILE_UNIQUE_ID_ARR_MAX, &winfileinfo.nFileIndexHigh, sizeof(DWORD));
                safe_memcpy(&fileId->fileid[sizeof(DWORD)], FILE_UNIQUE_ID_ARR_MAX - sizeof(DWORD),
                            &winfileinfo.nFileIndexLow, sizeof(DWORD));
            }
            return fileId;
        }
    }
    return M_NULLPTR;
}

static M_INLINE void safe_free_token_user(TOKEN_USER** user)
{
    safe_Free(M_REINTERPRET_CAST(void**, user));
}

static char* get_Current_User_SID(void)
{
    char*  sidAsString = M_NULLPTR;
    HANDLE hToken      = M_NULLPTR;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
    {
        DWORD dwSize = DWORD_C(0);
        GetTokenInformation(hToken, TokenUser, M_NULLPTR, 0, &dwSize);
        PTOKEN_USER pUser = M_REINTERPRET_CAST(PTOKEN_USER, safe_malloc(dwSize));
        if (pUser)
        {
            safe_memset(pUser, dwSize, 0, dwSize);
            if (GetTokenInformation(hToken, TokenUser, pUser, dwSize, &dwSize))
            {
                LPSTR pSidString = M_NULLPTR;
                if (ConvertSidToStringSidA(pUser->User.Sid, &pSidString))
                {
                    // dup'ing this because want to use std malloc/free's instead of LocalFree
                    safe_strdup(&sidAsString, pSidString);
                    LocalFree(pSidString);
                }
            }
            explicit_zeroes(pUser, dwSize);
            safe_free_token_user(&pUser);
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
        // Check is for a drive letter + :
        // trailing \ is not checked because it is stripped off by win_dirname
        // function when this is used to figure out number of directories to
        // validate.
        if (safe_strlen(path) == 2 && safe_isascii(path[0]) && path[1] == ':')
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
                /* Special case. The root path C:\ needs to leave the trailing
                 * backslash behind */
                *lastseperator = '\0';
            }
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
static bool is_Secure_Well_Known_SID(PSID sid, bool allowUsersAndAuthenticatedUsers)
{
    bool secure = false;
    if (sid && IsValidSid(sid))
    {
        // NOLINTBEGIN(bugprone-branch-clone)
        // Disabling clang-tidy here because it is easier to read these as
        // separate cases and it may end up changing in the future-TJE
        if (IsWellKnownSid(sid, WinAccountAdministratorSid) || IsWellKnownSid(sid, WinLocalSystemSid) ||
            IsWellKnownSid(sid, WinNtAuthoritySid) || IsWellKnownSid(sid, WinBuiltinAdministratorsSid))
        {
            // Do we need to check any of these other SIDs for admins?
            //  POSIX validates user or root. Do any of these others below in
            //  the if above as accepted???
            // WinLocalSid
            // WinBuiltinAdministratorsSid ??
            // WinAccountDomainAdminsSid ??
            // WinAccountCertAdminsSid ??
            // WinAccountSchemaAdminsSid ??
            // WinAccountEnterpriseAdminsSid ??
            // WinAccountPolicyAdminsSid ??
            // WinBuiltinHyperVAdminsSid ??
            // WinLocalAccountAndAdministratorSid ??
            // WinBuiltinStorageReplicaAdminsSid ??
            // WinAccountKeyAdminsSid ??
            // WinAccountEnterpriseKeyAdminsSid ??
            secure = true;
        }
        else if (allowUsersAndAuthenticatedUsers &&
                 (IsWellKnownSid(sid, WinBuiltinUsersSid) || IsWellKnownSid(sid, WinAuthenticatedUserSid)))
        {
            // This is a special case for C: and C:\Users where these
            // accounts/groups also has permission and must be trusted.
            secure = true;
        }
        // NOLINTEND(bugprone-branch-clone)
    }
    return secure;
}

static bool get_System_Volume(char* winSysVol, size_t winSysVolLen)
{
    // Need to read the system drive's info so we can setup specific permissions
    // allowed for specific directories.-TJE
    static bool validsystemvol = false;
    static DECLARE_ZERO_INIT_ARRAY(char, windowsSystemVolume, MAX_PATH);
    if (validsystemvol && safe_strnlen(windowsSystemVolume, MAX_PATH) > 0)
    {
        safe_memset(winSysVol, winSysVolLen, 0, winSysVolLen);
        safe_strcat(winSysVol, winSysVolLen, windowsSystemVolume);
    }
    else
    {
        if (GetWindowsDirectoryA(windowsSystemVolume, MAX_PATH) == 0)
        {
            // This function failed so try reading this environment variable
            // instead
            char* systemDrive = M_NULLPTR;
            safe_memset(windowsSystemVolume, MAX_PATH, 0, MAX_PATH);
            if (ENV_VAR_SUCCESS != get_Environment_Variable("SystemDrive", &systemDrive))
            {
#if defined(_DEBUG)
                printf("Failed reading the system drive environment variable.\n");
#endif //_DEBUG
            }
            else
            {
                safe_strcat(windowsSystemVolume, MAX_PATH, systemDrive);
                if (M_NULLPTR == strchr(windowsSystemVolume, '\\'))
                {
                    safe_strcat(windowsSystemVolume, MAX_PATH, "\\");
                }
            }
            safe_free(&systemDrive);
        }
        if (safe_strnlen(windowsSystemVolume, MAX_PATH) > 0)
        {
            // modify this info to be just the volume as we need to know this,
            // not C:\Windows. It MAY already be set to C:\, but it may not.
            char* slashPointer = strchr(windowsSystemVolume, '\\');
            if (slashPointer && safe_strnlen(slashPointer + 1, MAX_PATH - (C_CAST(uintptr_t, slashPointer) -
                                                                           C_CAST(uintptr_t, windowsSystemVolume))))
            {
                // need to add a null after this backslash so it's set to only
                // the volume in the path. This root directory is necessary to
                // know to properly validate other permissions.
                *(slashPointer + 1) = '\0';
            }
            validsystemvol = true;
            safe_memset(winSysVol, winSysVolLen, 0, winSysVolLen);
            safe_strcat(winSysVol, winSysVolLen, windowsSystemVolume);
        }
    }
    return validsystemvol;
}

FUNC_ATTR_PRINTF(2, 3) static void set_dir_security_output_error_message(char** outputError, const char* format, ...)
{
    if (outputError != M_NULLPTR)
    {
        va_list args;
        va_start(args, format);

#if IS_CLANG_VERSION(3, 0)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wformat-nonliteral"
#elif IS_GCC_VERSION(4, 1)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

        int result = vasprintf(outputError, format, args);

#if IS_CLANG_VERSION(3, 0)
#    pragma clang diagnostic pop
#elif IS_GCC_VERSION(4, 1)
#    pragma GCC diagnostic pop
#endif

        va_end(args);
        if (result < 0 && *outputError != M_NULLPTR)
        {
            safe_free(outputError);
        }
    }
}

static bool is_Folder_Secure(const char* securityDescriptorString, const char* dirptr, char** outputError)
{
    bool secure = true;
    if (securityDescriptorString == M_NULLPTR || dirptr == M_NULLPTR)
    {
        set_dir_security_output_error_message(outputError, "Invalid security descriptor string or directory given.\n");
        return false;
    }
    DECLARE_ZERO_INIT_ARRAY(char, windowsSystemVolume, MAX_PATH);
    char*                mySidStr                        = get_Current_User_SID();
    PSID                 mySid                           = M_NULLPTR;
    PSID                 winTI                           = M_NULLPTR; // To check for Windows trusted installer SID
    PSID                 everyoneGroupSID                = M_NULLPTR; // for C:\\Users directory
    PSECURITY_DESCRIPTOR secdesc                         = M_NULLPTR;
    ULONG                secdesclen                      = ULONG_C(0);
    PSID                 userSid                         = M_NULLPTR;
    BOOL                 defaultOwner                    = FALSE;
    PACL                 dacl                            = M_NULLPTR;
    BOOL                 daclPresent                     = FALSE;
    BOOL                 daclDefault                     = FALSE;
    bool                 allowUsersAndAuthenticatedUsers = false;
    bool                 allowEveryoneGroup              = false;
    do
    {
        if (FALSE == ConvertStringSidToSidA(mySidStr, &mySid))
        {
            /* Handle Error */
            set_dir_security_output_error_message(outputError,
                                                  "Failed to convert current user's SID string to sid structure\n");
            secure = false;
            break;
        }
        if (FALSE == IsValidSid(mySid))
        {
            /* Handle Error */
            set_dir_security_output_error_message(outputError, "Invalid sid\n");
            secure = false;
            break;
        }
        if (FALSE == ConvertStringSecurityDescriptorToSecurityDescriptorA(securityDescriptorString, SDDL_REVISION,
                                                                          &secdesc, &secdesclen))
        {
            /* Handle Error */
            set_dir_security_output_error_message(
                outputError, "Failed to convert security descriptor string to security descriptor structure\n");
            secure = false;
            break;
        }
        if (FALSE == IsValidSecurityDescriptor(secdesc))
        {
            /* Handle Error */
            set_dir_security_output_error_message(outputError, "Invalid security descriptor\n");
            secure = false;
            break;
        }
        if (FALSE == GetSecurityDescriptorOwner(secdesc, &userSid, &defaultOwner))
        {
            /* Handle Error */
            set_dir_security_output_error_message(outputError, "Failed to get security descriptor owner\n");
            secure = false;
            break;
        }
        if (FALSE == IsValidSid(userSid))
        {
            /* Handle Error */
            set_dir_security_output_error_message(outputError, "Invalid SID for security descriptor owner\n");
            secure = false;
            break;
        }
        if (!EqualSid(mySid, userSid) && !is_Secure_Well_Known_SID(userSid, false))
        {
            if (get_System_Volume(windowsSystemVolume, MAX_PATH))
            {
                // Need to check for the system directory
                if (strstr(dirptr, windowsSystemVolume) == dirptr)
                {
                    const char* WindowsTrustedInstallSID = "S-1-5-80-956008885-3418522649-1831038044-1853292631-"
                                                           "2271478464";
                    if (FALSE == ConvertStringSidToSidA(WindowsTrustedInstallSID, &winTI))
                    {
                        secure = false;
                        break;
                    }
#if defined(_DEBUG)
                    printf("This path is on the system drive. Set flags for "
                           "only specific folders to validate "
                           "permissions correctly\n");
#endif //_DEBUG
       // Set the isWindowsSystemDir to allow specific owners/groups for
       // specific folders to trust them. These are specific cases due to how
       // Windows sets permissions for these folders. Specifically, the root
       // volume for Windows allows all local users access. This is normally a
       // security issue in this code, however this is how Windows sets the
       // permissions itself, so allow it in this case. C:\, C:\Users\ allow
       // this permission C:\ also allows the NT installer user. NOTE: This is
       // not a complete list of all the specific cases for all Windows systems
       // paths at this time.
       //      Generally software using this is not accessing these Windows
       //      specific folders like C:\Windows\System32, etc so these are not
       //      listed here.
                    if (strcmp(dirptr, windowsSystemVolume) == 0)
                    {
                        if (EqualSid(userSid, winTI))
                        {
                            // This case the Windows trusted installer is found
                            // and can be trusted. This should only happen on
                            // the system root directory (C:) in Windows
#if defined(_DEBUG)
                            printf("Detected Windows trusted installer as owner "
                                   "of this directory and allowed to trust "
                                   "it.\n");
#endif                                                              //_DEBUG
                            allowUsersAndAuthenticatedUsers = true; // S-1-5-11 and S-1-5-32-545
                        }
                        else
                        {
                            /* Directory is owned by someone besides
                             * user/system/administrator */
                            secure          = false;
                            char* sidString = M_NULLPTR;
                            if (ConvertSidToStringSidA(userSid, &sidString))
                            {
                                set_dir_security_output_error_message(
                                    outputError, "Directory (%s) owned by SID (not trusted): %s\n", dirptr, sidString);
                                LocalFree(sidString);
                                sidString = M_NULLPTR;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    /* Directory is owned by someone besides
                     * user/system/administrator */
                    secure          = false;
                    char* sidString = M_NULLPTR;
                    if (ConvertSidToStringSidA(userSid, &sidString))
                    {
                        set_dir_security_output_error_message(
                            outputError, "Directory (%s) owned by SID (not trusted): %s\n", dirptr, sidString);
                        LocalFree(sidString);
                        sidString = M_NULLPTR;
                    }
                    break;
                }
            }
            else
            {
                /* Directory is owned by someone besides
                 * user/system/administrator */
                secure          = false;
                char* sidString = M_NULLPTR;
                if (ConvertSidToStringSidA(userSid, &sidString))
                {
                    set_dir_security_output_error_message(
                        outputError, "Directory (%s) owned by SID (not trusted): %s\n", dirptr, sidString);
                    LocalFree(sidString);
                    sidString = M_NULLPTR;
                }
                break;
            }
        }
        else
        {
            // work around specific directories
            if (get_System_Volume(windowsSystemVolume, MAX_PATH))
            {
                // Need to check for the system directory
                if (strstr(dirptr, windowsSystemVolume) == dirptr)
                {
                    // need to construct the paths to allow based on the windows
                    // volume path read earlier Using MAX_PATH for each of these
                    // for simplicity. Can reduce memory by shortening, but this
                    // is fine for now on this limited list.
                    DECLARE_ZERO_INIT_ARRAY(char, usersdir, MAX_PATH);
                    safe_strcat(usersdir, MAX_PATH, windowsSystemVolume);
                    safe_strcat(usersdir, MAX_PATH,
                                "Users"); // no trailing slash as the paths passed in
                                          // are not completed with a slash
                    if (strcmp(dirptr, usersdir) == 0)
                    {
                        allowUsersAndAuthenticatedUsers = true; // S-1-5-11 and S-1-5-32-545
                        allowEveryoneGroup              = true; // S-1-1-0
                    }
                }
            }
        }

        if (FALSE == GetSecurityDescriptorDacl(secdesc, &daclPresent, &dacl, &daclDefault))
        {
            /* Handle Error */
            secure = false;
            set_dir_security_output_error_message(outputError, "Unable to retrieve DACL from security descriptor: %s\n",
                                                  dirptr);
            break;
        }

        if (FALSE == daclPresent || dacl == M_NULLPTR)
        {
            /* Missing DACL, so cannot verify permissions */
            secure = false;
            set_dir_security_output_error_message(outputError, "DACL Missing. Cannot verify permissions: %s\n", dirptr);
            break;
        }

        if (FALSE == IsValidAcl(dacl))
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
            if (GetAce(dacl, iter, C_CAST(void**, &aceHeader)))
            {
                if (aceHeader->AceType == ACCESS_ALLOWED_ACE_TYPE)
                {
                    PACCESS_ALLOWED_ACE allowedACE = C_CAST(PACCESS_ALLOWED_ACE, aceHeader);
                    ACCESS_MASK         accessMask = allowedACE->Mask;
                    PSID                aceSID     = C_CAST(PSID, &allowedACE->SidStart);
                    if (IsValidSid(aceSID))
                    {
                        if (accessMask & (FILE_GENERIC_WRITE | FILE_APPEND_DATA) &&
                            (!EqualSid(mySid, aceSID) &&
                             !is_Secure_Well_Known_SID(aceSID, allowUsersAndAuthenticatedUsers)))
                        {
                            LPCSTR everyoneGroup = "S-1-1-0";
                            if (allowEveryoneGroup && everyoneGroupSID == M_NULLPTR)
                            {
                                ConvertStringSidToSidA(everyoneGroup, &everyoneGroupSID);
                            }

                            if (!(allowEveryoneGroup && everyoneGroupSID != M_NULLPTR &&
                                  EqualSid(aceSID, everyoneGroupSID)))
                            {
                                char* sidString = M_NULLPTR;
                                secure          = false;
                                if (ConvertSidToStringSidA(aceSID, &sidString))
                                {
                                    set_dir_security_output_error_message(
                                        outputError,
                                        "Directory (%s) can be accessed by SID (not trusted, must be removed to be "
                                        "secure): %s\n",
                                        dirptr, sidString);
                                    LocalFree(sidString);
                                    sidString = M_NULLPTR;
                                }
                            }
#if defined(_DEBUG)
                            else
                            {
                                char* sidString = M_NULLPTR;
                                if (ConvertSidToStringSidA(aceSID, &sidString))
                                {
                                    printf("Trusted SID: %s\n", sidString);
                                    LocalFree(sidString);
                                    sidString = M_NULLPTR;
                                }
                            }
#endif //_DEBUG
                        }
#if defined(_DEBUG)
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
                        set_dir_security_output_error_message(
                            outputError, "Invalid ACE in DACL. Directory (%s) cannot be trusted\n", dirptr);
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

    SecureZeroMemory(mySidStr, safe_strlen(mySidStr));
    safe_free(&mySidStr);
    if (mySid != M_NULLPTR)
    {
        SecureZeroMemory(mySid, GetLengthSid(mySid));
        LocalFree(mySid);
        mySid = M_NULLPTR;
    }
    if (winTI != M_NULLPTR)
    {
        SecureZeroMemory(winTI, GetLengthSid(winTI));
        LocalFree(winTI);
        winTI = M_NULLPTR;
    }
    if (everyoneGroupSID != M_NULLPTR)
    {
        SecureZeroMemory(everyoneGroupSID, GetLengthSid(everyoneGroupSID));
        LocalFree(everyoneGroupSID);
        everyoneGroupSID = M_NULLPTR;
    }
    if (secdesc != M_NULLPTR)
    {
        SecureZeroMemory(secdesc, secdesclen);
        LocalFree(secdesc);
        secdesc = M_NULLPTR;
    }

    return secure;
}

#define MAX_SYMLINKS_IN_PATH 5

/* This function requires Windows style seperators (\) to function properly! */
static bool internal_OS_Is_Directory_Secure(const char* fullpath, unsigned int num_symlinks, char** outputError)
{
    char*   path_copy   = M_NULLPTR;
    char**  dirs        = M_NULLPTR;
    ssize_t num_of_dirs = SSIZE_T_C(1);
    bool    secure      = true;
    ssize_t i           = SSIZE_T_C(0);
    errno_t error       = 0;

    // TODO: Set error message appropriate for Windows errors detected
    M_USE_UNUSED(outputError);

    if (!fullpath || fullpath[0] == '\0')
    {
        /* Handle error */
        return false;
    }
    // Check for drive letter format:
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

    error = safe_strdup(&path_copy, fullpath);
    if (error != 0 || path_copy == M_NULLPTR)
    {
        /* Handle error */
        return false;
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
        /* out of room to compare this many directories deep */
        return false;
    }
    /* Now num_of_dirs indicates # of dirs we must check */
    safe_free(&path_copy);
    dirs = M_REINTERPRET_CAST(char**, safe_malloc(C_CAST(size_t, num_of_dirs) * sizeof(char*)));
    if (!dirs)
    {
        /* Handle error */
        return false;
    }
    error = safe_strdup(&dirs[num_of_dirs - SSIZE_T_C(1)], fullpath);
    if (error != 0 || dirs[num_of_dirs - SSIZE_T_C(1)] == M_NULLPTR)
    {
        /* Handle error */
        safe_free(dirs);
        return false;
    }
    error = safe_strdup(&path_copy, fullpath);
    if (error != 0 || path_copy == M_NULLPTR)
    {
        /* Handle error */
        safe_free(dirs);
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
        // cleanup dirs before returning error
        // i is set to when strdup failed and was decrementing to zero/negatives
        // so use it + 1 as the starting point to go through and cleanup the
        // stored directories to free up memory
        for (ssize_t cleanup = i + 1; cleanup <= num_of_dirs; cleanup++)
        {
            safe_free(&dirs[cleanup]);
        }
        safe_free(dirs);
        return secure;
    }

    /*
     * Traverse from the root to the fullpath,
     * checking permissions along the way.
     */
    for (i = 0; i < num_of_dirs && secure; i++)
    {
        fileAttributes* attrs                 = M_NULLPTR;
        char*           dirptr                = dirs[i];
        bool            appendedTrailingSlash = false;
        if (is_Root_Path(dirptr))
        {
            // append a trailing \ to the end before attempting to get the
            // attributes
            size_t newlen = safe_strlen(dirptr) + 2;
            dirptr        = M_REINTERPRET_CAST(char*, safe_calloc(newlen, sizeof(char)));
            if (dirptr)
            {
                safe_memcpy(dirptr, newlen, dirs[i], newlen - 2);
                safe_strcat(dirptr, newlen, "\\");
                appendedTrailingSlash = true;
            }
            else
            {
                dirptr = dirs[i];
            }
        }
#if defined(_DEBUG)
        printf("dirptr = %s\n", dirptr);
#endif //_DEBUG

        attrs = os_Get_File_Attributes_By_Name(dirptr);
        if (!attrs)
        {
            /* handle error */
            secure = false;
            set_dir_security_output_error_message(outputError, "Unable to read directory attributes: %s\n", dirptr);
            if (appendedTrailingSlash)
            {
                safe_free(&dirptr);
            }
            break;
        }
        if (attrs->numberOfLinks > MAX_SYMLINKS_IN_PATH)
        {
            /* handle error */
            secure = false;
            set_dir_security_output_error_message(outputError, "Too many symlinks in path (>%d): %s\n",
                                                  MAX_SYMLINKS_IN_PATH, dirptr);
            free_File_Attributes(&attrs);
            if (appendedTrailingSlash)
            {
                safe_free(&dirptr);
            }
            break;
        }

        if (attrs->fileFlags & FILE_ATTRIBUTE_REPARSE_POINT)
        {
            /* This is a link. Need to resolve it and test it */
            HANDLE link = CreateFileA(dirptr, GENERIC_READ, FILE_SHARE_READ, M_NULLPTR, OPEN_EXISTING,
                                      FILE_FLAG_OPEN_REPARSE_POINT, M_NULLPTR);
            if (link != INVALID_HANDLE_VALUE)
            {
                PREPARSE_DATA_BUFFER reparseData =
                    M_REINTERPRET_CAST(PREPARSE_DATA_BUFFER, safe_malloc(sizeof(REPARSE_DATA_BUFFER) + MAX_PATH));
                if (reparseData)
                {
                    DWORD bytesRead = DWORD_C(0);
                    safe_memset(reparseData, sizeof(REPARSE_DATA_BUFFER) + MAX_PATH, 0,
                                sizeof(REPARSE_DATA_BUFFER) + MAX_PATH);
                    if (DeviceIoControl(link, FSCTL_GET_REPARSE_POINT, M_NULLPTR, 0, &reparseData, sizeof(reparseData),
                                        &bytesRead, M_NULLPTR))
                    {
                        size_t bufferSize  = SIZE_T_C(0);
                        char*  reparsePath = M_NULLPTR;
#if defined(HAVE_MSFT_SECURE_LIB)
                        wcstombs_s(&bufferSize, M_NULLPTR, 0,
                                   C_CAST(wchar_t*, reparseData->SymbolicLinkReparseBuffer.PathBuffer), 0);
#else
                        bufferSize =
                            wcstombs(M_NULLPTR, C_CAST(wchar_t*, reparseData->SymbolicLinkReparseBuffer.PathBuffer), 0);
#endif
                        if (bufferSize > 0 && bufferSize != SIZE_MAX)
                        {
                            reparsePath = M_REINTERPRET_CAST(char*, safe_calloc(bufferSize + 1, sizeof(char)));
                            if (reparsePath)
                            {
                                size_t conversionSize = SIZE_T_C(0);
#if defined(HAVE_MSFT_SECURE_LIB)
                                wcstombs_s(&conversionSize, reparsePath, bufferSize + 1,
                                           C_CAST(wchar_t*, reparseData->SymbolicLinkReparseBuffer.PathBuffer),
                                           bufferSize + 1);
#else
                                conversionSize = wcstombs(
                                    reparsePath, C_CAST(wchar_t*, reparseData->SymbolicLinkReparseBuffer.PathBuffer),
                                    bufferSize + 1);
#endif
                                if (conversionSize != bufferSize || conversionSize == SIZE_MAX)
                                {
                                    // an error occurred during the conversion,
                                    // so consider this not secure.
                                    secure = false;
                                }
                                else
                                {
                                    num_symlinks++;
                                    bool recurseSecure =
                                        internal_OS_Is_Directory_Secure(reparsePath, num_symlinks, outputError);
                                    num_symlinks--;
                                    if (!recurseSecure)
                                    {
                                        secure = false;
                                    }
                                }
                                safe_free(&reparsePath);
                            }
                            else
                            {
                                set_dir_security_output_error_message(
                                    outputError, "Unable to allocate memory to check reparse point in path: %s\n",
                                    dirptr);
                                secure = false;
                            }
                        }
                        else
                        {
                            set_dir_security_output_error_message(
                                outputError, "Unable to calculate memory length to check reparse point in path: %s\n",
                                dirptr);
                            secure = false;
                        }
                    }
                    else
                    {
                        set_dir_security_output_error_message(
                            outputError,
                            "Unable to issue FSCTL_GET_REPASE_POINT to validate reparse point in path: %s\n", dirptr);
                        secure = false;
                    }
                    safe_free_reparse_data_buf(&reparseData);
                }
                else
                {
                    set_dir_security_output_error_message(
                        outputError, "Unable to allocate memory to check reparse point in path: %s\n", dirptr);
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
                        safe_free(&dirptr);
                    }
                    break;
                }
            }
            else
            {
                set_dir_security_output_error_message(outputError,
                                                      "Unable to open handle to reparse point in path: %s\n", dirptr);
                secure = false;
                free_File_Attributes(&attrs);
                if (appendedTrailingSlash)
                {
                    safe_free(&dirptr);
                }
                break;
            }
        }

        if (!(attrs->fileFlags & FILE_ATTRIBUTE_DIRECTORY))
        {
            /* Not a directory */
            secure = false;
            set_dir_security_output_error_message(outputError,
                                                  "%s is not a directory. Cannot validate as part of path.\n", dirptr);
            free_File_Attributes(&attrs);
            if (appendedTrailingSlash)
            {
                safe_free(&dirptr);
            }
            break;
        }

        secure = is_Folder_Secure(attrs->winSecurityDescriptor, dirptr, outputError);
        if (appendedTrailingSlash)
        {
            safe_free(&dirptr);
        }

        free_File_Attributes(&attrs);
    }

    for (i = 0; i < num_of_dirs; i++)
    {
        safe_free(&dirs[i]);
    }

    safe_free(dirs);
    return secure;
}

M_NODISCARD bool os_Is_Directory_Secure(const char* fullpath, char** outputError)
{
    // This was implemented as close as possible to
    // https://wiki.sei.cmu.edu/confluence/display/c/FIO15-C.+Ensure+that+file+operations+are+performed+in+a+secure+directory
    unsigned int num_symlinks = 0;
    return internal_OS_Is_Directory_Secure(fullpath, num_symlinks, outputError);
}

bool os_Directory_Exists(const char* pathToCheck)
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    safe_memset(&fileAttributes, sizeof(WIN32_FILE_ATTRIBUTE_DATA), 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
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
    BOOL   returnValue    = FALSE;
    size_t filePathLength = (safe_strlen(filePath) + 1) * sizeof(TCHAR);
    TCHAR* pathNameBuf    = M_REINTERPRET_CAST(TCHAR*, safe_calloc(filePathLength, sizeof(TCHAR)));
    if (pathNameBuf)
    {
        CONST TCHAR* pathName = &pathNameBuf[0];
        _stprintf_s(pathNameBuf, filePathLength, TEXT("%hs"), filePath);

        returnValue = CreateDirectory(pathName, M_NULLPTR);
        safe_free_tchar(&pathNameBuf);
        if (returnValue == FALSE)
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

bool os_File_Exists(const char* filetoCheck)
{
    WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
    safe_memset(&fileAttributes, sizeof(WIN32_FILE_ATTRIBUTE_DATA), 0, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
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
    size_t localPathAndFileLength = (safe_strlen(pathAndFile) + 1) * sizeof(TCHAR);
    TCHAR* localpathAndFileBuf    = M_REINTERPRET_CAST(TCHAR*, safe_calloc(localPathAndFileLength, sizeof(TCHAR)));
    DECLARE_ZERO_INIT_ARRAY(TCHAR, fullPathOutput, OPENSEA_PATH_MAX);
    if (!localpathAndFileBuf)
    {
        return FAILURE;
    }
    CONST TCHAR* localpathAndFile = &localpathAndFileBuf[0];
    _stprintf_s(localpathAndFileBuf, localPathAndFileLength, TEXT("%hs"), pathAndFile);
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
    snprintf(fullPath, OPENSEA_PATH_MAX, "%ls", fullPathOutput);
#else
#    if defined(_MSC_VER)
    // Microsoft uses hs but this is not standard and using %s is the standard
    snprintf(fullPath, OPENSEA_PATH_MAX, "%hs", fullPathOutput);
#    else
    snprintf(fullPath, OPENSEA_PATH_MAX, "%s", fullPathOutput);
#    endif
#endif
    // Check if this file even exists to make this more like the behavior of the
    // POSIX realpath function.
    if (!os_File_Exists(fullPath) && !os_Directory_Exists(fullPath))
    {
        safe_memset(fullPath, OPENSEA_PATH_MAX, 0, OPENSEA_PATH_MAX);
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
bool exact_Compare_SIDS_And_DACL_Strings(const char* sidsAndDACLstr1, const char* sidsAndDACLstr2)
{
    bool match = false;
    // This function is not just doing strcmp because that will not work.
    // convert these back to the raw structures, then compare them.
    if (sidsAndDACLstr1 && sidsAndDACLstr2)
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
        if (TRUE == ConvertStringSecurityDescriptorToSecurityDescriptorA(sidsAndDACLstr1, SDDL_REVISION, &secDesc1,
                                                                         &secDesc1len))
        {
            validdesc1 = IsValidSecurityDescriptor(secDesc1);
        }
        if (TRUE == ConvertStringSecurityDescriptorToSecurityDescriptorA(sidsAndDACLstr2, SDDL_REVISION, &secDesc2,
                                                                         &secDesc2len))
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
            // Now that we have read all of the possible values....lets compare
            // everything
            if ((validown1 && validown2 && defaultown1 == defaultown2 && EqualSid(owner1, owner2)) &&
                (validgroup1 && validgroup2 && defaultgroup1 == defaultgroup2 && EqualSid(group1, group2)))
            {
                // owner and group are the same.
                // Compare the dacls
                if (GetSecurityDescriptorDacl(secDesc1, &dacl1present, &dacl1, &defaultdacl1))
                {
                    validdacl1 = IsValidAcl(dacl1);
                }
                if (GetSecurityDescriptorDacl(secDesc2, &dacl2present, &dacl2, &defaultdacl2))
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
