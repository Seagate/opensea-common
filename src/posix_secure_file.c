// SPDX-License-Identifier: MPL-2.0

//! \file posix_secure_file.c
//! \brief POSIX system specific secure file implementation of secure_file.h functions
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

// NOTE: UEFI_C_SOURCE can use most, if not all, of the functionality in here.
//       There are some differences. For example, the secure directory cannot
//       detect uid's, but otherwise it should work. We can ifdef around the
//       differences here since this is more similar than in Windows without a
//       completely new/seperate implementation Readlink is also missing in UEFI
//       EDK2, but that likely means any filesystem links cannot be traversed
//       anyways
//  https://github.com/tianocore/edk2-libc/blob/caea801aac338aa60f85a7c10148ca0b4440fff3/StdLib/Include/sys/stat.h

#include "io_utils.h"
#include "memory_safety.h"
#include "secure_file.h"
#include "secured_env_vars.h"
#include "string_utils.h"
#include "type_conversion.h"

#include <libgen.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

M_NODISCARD fileAttributes* os_Get_File_Attributes_By_Name(const char* filetoCheck)
{
    fileAttributes* attrs = M_NULLPTR;
    struct stat     st;
    safe_memset(&st, sizeof(struct stat), 0, sizeof(struct stat));
    DISABLE_NONNULL_COMPARE
    if (filetoCheck != M_NULLPTR && stat(filetoCheck, &st) == 0)
    {
        attrs = M_REINTERPRET_CAST(fileAttributes*, safe_calloc(1, sizeof(fileAttributes)));
        if (attrs != M_NULLPTR)
        {
#if defined(UEFI_C_SOURCE)
            attrs->deviceID             = 0;
            attrs->inode                = 0;
            attrs->numberOfLinks        = 0;
            attrs->userID               = 0;
            attrs->groupID              = 0;
            attrs->representedDeviceID  = 0;
            attrs->fileStatusChangeTime = 0;
#else
            attrs->deviceID             = st.st_dev;
            attrs->inode                = st.st_ino;
            attrs->numberOfLinks        = st.st_nlink;
            attrs->userID               = st.st_uid;
            attrs->groupID              = st.st_gid;
            attrs->representedDeviceID  = st.st_rdev;
            attrs->fileStatusChangeTime = st.st_ctime;
#endif // UEFI_C_SOURCE
            attrs->filemode             = st.st_mode;
            attrs->filesize             = st.st_size;
            attrs->fileLastAccessTime   = st.st_atime;
            attrs->fileModificationTime = st.st_mtime;
        }
    }
    RESTORE_NONNULL_COMPARE
    return attrs;
}

M_NODISCARD fileAttributes* os_Get_File_Attributes_By_File(FILE* file)
{
    fileAttributes* attrs = M_NULLPTR;
    struct stat     st;
    safe_memset(&st, sizeof(struct stat), 0, sizeof(struct stat));
    DISABLE_NONNULL_COMPARE
    if (file != M_NULLPTR && fstat(fileno(file), &st) == 0)
    {
        attrs = M_REINTERPRET_CAST(fileAttributes*, safe_calloc(1, sizeof(fileAttributes)));
        if (attrs != M_NULLPTR)
        {
#if defined(UEFI_C_SOURCE)
            attrs->deviceID             = 0;
            attrs->inode                = 0;
            attrs->numberOfLinks        = 0;
            attrs->userID               = 0;
            attrs->groupID              = 0;
            attrs->representedDeviceID  = 0;
            attrs->fileStatusChangeTime = 0;
#else
            attrs->deviceID             = st.st_dev;
            attrs->inode                = st.st_ino;
            attrs->numberOfLinks        = st.st_nlink;
            attrs->userID               = st.st_uid;
            attrs->groupID              = st.st_gid;
            attrs->representedDeviceID  = st.st_rdev;
            attrs->fileStatusChangeTime = st.st_ctime;
#endif // UEFI_C_SOURCE
            attrs->filemode             = st.st_mode;
            attrs->filesize             = st.st_size;
            attrs->fileLastAccessTime   = st.st_atime;
            attrs->fileModificationTime = st.st_mtime;
        }
    }
    RESTORE_NONNULL_COMPARE
    return attrs;
}

M_NODISCARD fileUniqueIDInfo* os_Get_File_Unique_Identifying_Information(FILE* file)
{
    fileUniqueIDInfo* uniqueID = M_NULLPTR;
#if defined(UEFI_C_SOURCE)
    M_USE_UNUSED(file);
#else
    struct stat st;
    safe_memset(&st, sizeof(struct stat), 0, sizeof(struct stat));
    DISABLE_NONNULL_COMPARE
    if (file != M_NULLPTR && fstat(fileno(file), &st))
    {
        // device ID and inode
        uniqueID = M_REINTERPRET_CAST(fileUniqueIDInfo*, safe_calloc(1, sizeof(fileUniqueIDInfo)));
        if (uniqueID != M_NULLPTR)
        {
            uniqueID->deviceid = st.st_dev;
            uniqueID->inode = st.st_ino;
        }
    }
    RESTORE_NONNULL_COMPARE
#endif
    return uniqueID;
}

// This function reads the environment variable SUDO_UID to determine
// if the user used "sudo" to execute the program
// on error, uid returned will be zero
static uid_t get_sudo_uid(void)
{
    static uid_t sudouid    = ROOT_UID_VAL;
    static bool  gotsudouid = false;
    if (!gotsudouid)
    {
        char* uidstr = M_NULLPTR;
        if (get_Environment_Variable("SUDO_UID", &uidstr) == ENV_VAR_SUCCESS && uidstr != M_NULLPTR)
        {
            unsigned long temp = 0UL;
            if (0 == safe_strtoul(&temp, uidstr, M_NULLPTR, BASE_10_DECIMAL))
            {
                sudouid = M_STATIC_CAST(uid_t, temp);
            }
        }
        safe_free(&uidstr);
        gotsudouid = true;
    }
    return sudouid;
}

FUNC_ATTR_PRINTF(2, 3) static void set_dir_security_output_error_message(char** outputError, const char* format, ...)
{
    if (outputError != M_NULLPTR)
    {
        va_list args;
        va_start(args, format);
        int result = vasprintf(outputError, format, args);
        va_end(args);
        if (result < 0 && *outputError != M_NULLPTR)
        {
            safe_free(outputError);
        }
    }
}

#define MAX_SYMLINKS_IN_PATH 5

static bool internal_OS_Is_Directory_Secure(const char* fullpath, unsigned int num_symlinks, char** outputError)
{
    char*       path_copy   = M_NULLPTR;
    char**      dirs        = M_NULLPTR;
    ssize_t     num_of_dirs = SSIZE_T_C(1);
    bool        secure      = true;
    ssize_t     i           = SSIZE_T_C(0);
    ssize_t     r           = SSIZE_T_C(0);
    struct stat buf;
#if !defined(UEFI_C_SOURCE)
    uid_t my_uid = geteuid();
#endif
    errno_t error = 0;

    safe_memset(&buf, sizeof(struct stat), 0, sizeof(struct stat));

    if (!fullpath || fullpath[0] != '/')
    {
        /* Handle error */
        set_dir_security_output_error_message(outputError, "Error: Full path must start with \"/\".\n");
        return false;
    }

    if (num_symlinks > MAX_SYMLINKS_IN_PATH)
    {
        /* Could be a symlink loop */
        /* Handle error */
        set_dir_security_output_error_message(
            outputError, "Error: Too many symbolic links (must be fewer than %d links)\n", MAX_SYMLINKS_IN_PATH);
        return false;
    }

    error = safe_strdup(&path_copy, fullpath);

    if (error != 0 || path_copy == M_NULLPTR)
    {
        /* Handle error */
        set_dir_security_output_error_message(
            outputError, "Error: Unable to duplicate fullpath to path copy: %s (Out of memory)\n", fullpath);
        return false;
    }

    /* Figure out how far it is to the root */
    char* path_parent = path_copy;
    for (; ((strcmp(path_parent, "/") != 0) && (strcmp(path_parent, "//") != 0) && (strcmp(path_parent, ".") != 0));
         path_parent = dirname(path_parent))
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
        set_dir_security_output_error_message(
            outputError,
            "Error: The path contains too many directories. Please reduce the number of directories in the path and "
            "try again. Must be less than %zd directories in the path.\n",
            SSIZE_MAX);
        safe_free(&path_copy);
        return false;
    }
    /* Now num_of_dirs indicates # of dirs we must check */
    safe_free(&path_copy);

    dirs = M_REINTERPRET_CAST(char**, safe_malloc(C_CAST(size_t, num_of_dirs) * sizeof(char*)));
    if (dirs == M_NULLPTR)
    {
        /* Handle error */
        set_dir_security_output_error_message(outputError,
                                              "Error: Unable to allocate memory for directory validation. Please "
                                              "ensure sufficient memory is available and try again.\n");
        return false;
    }

    error = safe_strdup(&dirs[num_of_dirs - 1], fullpath);

    if (error != 0 || dirs[num_of_dirs - 1] == M_NULLPTR)
    {
        /* Handle error */
        set_dir_security_output_error_message(
            outputError, "Error: Unable to duplicate fullpath into dirs array: %s (Out of memory)\n", fullpath);
        safe_free(M_REINTERPRET_CAST(void**, &dirs));
        return false;
    }

    error = safe_strdup(&path_copy, fullpath);

    if (error != 0 || path_copy == M_NULLPTR)
    {
        /* Handle error */
        set_dir_security_output_error_message(
            outputError, "Error: Unable to duplicate fullpath to path copy: %s (Out of memory)\n", fullpath);
        safe_free(&dirs[num_of_dirs - 1]);
        safe_free(M_REINTERPRET_CAST(void**, &dirs));
        return false;
    }

    /* Now fill the dirs array */
    path_parent = path_copy;
    for (i = num_of_dirs - SSIZE_T_C(2); i >= SSIZE_T_C(0); i--)
    {
        path_parent = dirname(path_parent);
        error       = safe_strdup(&dirs[i], path_parent);
        if (error != 0 || dirs[i] == M_NULLPTR)
        {
            /* Handle error */
            set_dir_security_output_error_message(
                outputError, "Error: Unable to duplicate path parent: %s (Out of memory)\n", path_parent);
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
        for (ssize_t cleanup = i + SSIZE_T_C(1); cleanup <= num_of_dirs; cleanup++)
        {
            safe_free(&dirs[cleanup]);
        }
        safe_free(M_REINTERPRET_CAST(void**, &dirs));
        return secure;
    }

    /*
     * Traverse from the root to the fullpath,
     * checking permissions along the way.
     */
    for (i = SSIZE_T_C(0); i < num_of_dirs; i++)
    {
        ssize_t linksize = SSIZE_T_C(0);
        char*   link     = M_NULLPTR;
#if defined(_DEBUG)
        printf("Checking \"%s\"\n", dirs[i]);
#endif
        if (lstat(dirs[i], &buf) != 0)
        {
            /* Handle error */
            set_dir_security_output_error_message(
                outputError,
                "Error: Failed to read file status for %s. This operation is necessary to retrieve ownership and "
                "permission details. Please check the path and ensure you have the required permissions.\n",
                dirs[i]);
            secure = false;
            break;
        }

#if defined(S_ISLNK)
        if (S_ISLNK(buf.st_mode))
        {
            /* Symlink, test linked-to file */
            if (buf.st_size < 0)
            {
                /* Handle error */
                secure = false;
                set_dir_security_output_error_message(outputError,
                                                      "Error: Invalid link size for %s. The size of the symbolic link "
                                                      "is negative, which indicates a potential filesystem issue.\n",
                                                      dirs[i]);
                break;
            }
            linksize = buf.st_size + 1;
            if (!(link = M_REINTERPRET_CAST(char*, safe_malloc(C_CAST(size_t, linksize)))))
            {
                /* Handle error */
                secure = false;
                set_dir_security_output_error_message(
                    outputError, "Error: Unable to allocate memory to read the link for %s. (Out of memory)\n",
                    dirs[i]);
                break;
            }

            r = readlink(dirs[i], link, C_CAST(size_t, linksize));
            // NOLINTBEGIN(bugprone-branch-clone)
            if (r == SSIZE_T_C(-1))
            {
                /* Handle error */
                set_dir_security_output_error_message(outputError,
                                                      "Error: Failed to read the symbolic link for %s. Please check "
                                                      "the path and ensure the link exists and is accessible.\n",
                                                      dirs[i]);
                secure = false;
                safe_free(&link);
                break;
            }
            else if (r >= linksize)
            {
                /* Handle truncation error */
                set_dir_security_output_error_message(
                    outputError,
                    "Error: The symbolic link for %s is truncated. The link is too long to be read completely.\n",
                    dirs[i]);
                secure = false;
                safe_free(&link);
                break;
            }
            // NOLINTEND(bugprone-branch-clone)
            link[r] = '\0';

            num_symlinks++;
            bool recurseSecure = internal_OS_Is_Directory_Secure(link, num_symlinks, outputError);
            num_symlinks--;

            if (!recurseSecure)
            {
#    if defined(_DEBUG)
                printf("recursive link check failed\n");
#    endif
                secure = false;
                safe_free(&link);
                break;
            }
            safe_free(&link);
            continue;
        }
#endif // S_ISLNK - if this macro does not exist, then cannot check for links

        if (!S_ISDIR(buf.st_mode))
        {
            /* Not a directory */
            set_dir_security_output_error_message(
                outputError, "Error: %s is not a directory. Cannot verify for secure path.\n", dirs[i]);
            secure = false;
            break;
        }

#if !defined(UEFI_C_SOURCE)
#    if defined(_DEBUG)
        printf("Checking UIDs\n");
#    endif
        if ((buf.st_uid != my_uid) && (buf.st_uid != ROOT_UID_VAL))
        {
            /* Before we assume insecure, check if this was executed as sudo by
             * the user as this directory may be the user's directory and ok to
             * access */
            /* Only do this if the euid read above is set to zero */
            if (my_uid == ROOT_UID_VAL)
            {
#    if defined(_DEBUG)
                printf("root UID detected, getting user's ID\n");
#    endif
                uid_t sudouid = get_sudo_uid();
#    if defined(_DEBUG)
                printf("UID detected: %u\n", sudouid);
#    endif
                if (sudouid != ROOT_UID_VAL)
                {
                    if (buf.st_uid != sudouid)
                    {
                        /* Directory is owned by someone besides user or root */
                        secure = false;
                        set_dir_security_output_error_message(
                            outputError,
                            "Error: Directory (%s) owned by someone other than user or root. Owner: %u Current User: "
                            "%u. Recommended action: \"chown %u:%u %s\"\n",
                            dirs[i], buf.st_uid, sudouid, sudouid, sudouid, dirs[i]);
                        break;
                    }
                }
                else
                {
                    /* Directory is owned by someone besides user or root */
                    secure = false;
                    set_dir_security_output_error_message(
                        outputError,
                        "Error: Directory (%s) owned by someone other than user or root. Owner: %u Current User: %u. "
                        "Recommended action: \"chown %u:%u %s\"\n",
                        dirs[i], buf.st_uid, my_uid, my_uid, my_uid, dirs[i]);
                    break;
                }
            }
            else
            {
                /* Directory is owned by someone besides user or root */
                secure = false;
                set_dir_security_output_error_message(
                    outputError,
                    "Error: Directory (%s) owned by someone other than user or root. Owner: %u Current User: %u. "
                    "Recommended action: \"chown %u:%u %s\"\n",
                    dirs[i], buf.st_uid, my_uid, my_uid, my_uid, dirs[i]);
                break;
            }
        }
#endif //! UEFI_C_SOURCE

        if (buf.st_mode & (S_IWGRP | S_IWOTH))
        {
            /* dir is writable by others */
            secure = false;
            if (buf.st_mode & S_IWGRP)
            {
                set_dir_security_output_error_message(
                    outputError,
                    "Error: Directory (%s) writable by group. Disable write permissions for groups. Recommended "
                    "action: \"chmod u=rwx,g=rx,o=rx %s\"\n",
                    dirs[i], dirs[i]);
            }
            else
            {
                set_dir_security_output_error_message(
                    outputError,
                    "Error: Directory (%s) writable by others. Disable write permissions for others. Recommended "
                    "action: \"chmod u=rwx,g=rx,o=rx %s\"\n",
                    dirs[i], dirs[i]);
            }
            break;
        }
    }

    for (i = SSIZE_T_C(0); i < num_of_dirs; i++)
    {
        safe_free(&dirs[i]);
    }
    safe_free(M_REINTERPRET_CAST(void**, &dirs));
    return secure;
}

bool os_Is_Directory_Secure(const char* fullpath, char** outputError)
{
    unsigned int num_symlinks = 0U;
    return internal_OS_Is_Directory_Secure(fullpath, num_symlinks, outputError);
}

bool os_Directory_Exists(const char* pathToCheck)
{
    fileAttributes* attrs = os_Get_File_Attributes_By_Name(pathToCheck);
    if (attrs != M_NULLPTR)
    {
        bool result = M_ToBool(S_ISDIR(attrs->filemode));
        free_File_Attributes(&attrs);
        return result;
    }
    else
    {
        return false;
    }
}

bool os_File_Exists(const char* filetoCheck)
{
    fileAttributes* attrs = os_Get_File_Attributes_By_Name(filetoCheck);
    if (attrs != M_NULLPTR)
    {
        bool result = M_ToBool(S_ISREG(attrs->filemode));
        free_File_Attributes(&attrs);
        return result;
    }
    else
    {
        return false;
    }
}

int64_t os_Get_File_Size(FILE* filePtr)
{
    struct stat st;
    safe_memset(&st, sizeof(struct stat), 0, sizeof(struct stat));
    if (0 == fstat(fileno(filePtr), &st))
    {
        return st.st_size;
    }
    else
    {
        return INT64_C(-1);
    }
}

eReturnValues os_Create_Directory(const char* filePath)
{
    // mkdirres should be an int as it is the output of the mkdir command
    // We are returning enum values, not the result of this command!
    int mkdirres = mkdir(filePath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (mkdirres == 0)
    {
        return SUCCESS;
    }
    else
    {
#if defined(_DEBUG)
        printf("Error: %s\n", strerror(errno));
#endif
        return FAILURE;
    }
}

// Does not set group write permissions or other write permissions.
// User gets rwx, group gets rx, other gets rx
eReturnValues os_Create_Secure_Directory(const char* filePath)
{
    // mkdirres should be an int as it is the output of the mkdir command
    // We are returning enum values, not the result of this command!
    int mkdirres = mkdir(filePath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (mkdirres == 0)
    {
        return SUCCESS;
    }
    else
    {
#if defined(_DEBUG)
        printf("Error: %s\n", strerror(errno));
#endif
        return FAILURE;
    }
}

// https://linux.die.net/man/3/realpath
eReturnValues get_Full_Path(const char* pathAndFile, char fullPath[OPENSEA_PATH_MAX])
{
    char* resolvedPath = realpath(pathAndFile, fullPath);
    if (resolvedPath)
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}
