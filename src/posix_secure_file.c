// SPDX-License-Identifier: MPL-2.0

//! \file posix_secure_file.c
//! \brief POSIX system specific secure file implementation of secure_file.h functions
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
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

#if defined(__linux__) && !defined(_GNU_SOURCE)
#    define _GNU_SOURCE 1
#endif

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

#if defined(__linux__) && !defined(UEFI_C_SOURCE)
#    include <fcntl.h>

static bool path_root_attributes_secure(const struct stat* attrs)
{
    // On Linux POSIX ACLs, the group mode bits include the effective ACL mask.
    return S_ISDIR(attrs->st_mode) && attrs->st_uid == 0 && attrs->st_gid == 0 &&
           (attrs->st_mode & (S_IWGRP | S_IWOTH)) == 0;
}
#endif

M_NODISCARD fileAttributes* os_Get_File_Attributes_By_Name(const char* M_NONNULL filetoCheck)
{
    fileAttributes* attrs = M_NULLPTR;
    struct stat     st;
    M_INITIALIZE_STRUCTURE(&st, sizeof(struct stat));
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
    return attrs;
}

M_NODISCARD fileAttributes* os_Get_File_Attributes_By_File(FILE* M_NONNULL file)
{
    fileAttributes* attrs = M_NULLPTR;
    struct stat     st;
    M_INITIALIZE_STRUCTURE(&st, sizeof(struct stat));
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
    return attrs;
}

M_NODISCARD fileUniqueIDInfo* os_Get_File_Unique_Identifying_Information(FILE* M_NONNULL file)
{
    fileUniqueIDInfo* uniqueID = M_NULLPTR;
#if defined(UEFI_C_SOURCE)
    M_USE_UNUSED(file);
#else
    struct stat st;
    M_INITIALIZE_STRUCTURE(&st, sizeof(struct stat));
    if (file != M_NULLPTR && fstat(fileno(file), &st) == 0)
    {
        // device ID and inode
        uniqueID = M_REINTERPRET_CAST(fileUniqueIDInfo*, safe_calloc(1, sizeof(fileUniqueIDInfo)));
        if (uniqueID != M_NULLPTR)
        {
            uniqueID->deviceid = M_STATIC_CAST(uint64_t, st.st_dev);
            uniqueID->inode    = st.st_ino;
        }
    }
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

        // If an error message is already present, then we need to free it before
        // creating a new one to prevent a memory leak
        if (*outputError != M_NULLPTR)
        {
            explicit_zeroes(*outputError, safe_strlen(*outputError));
            safe_free(outputError);
        }

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

    M_INITIALIZE_STRUCTURE(&buf, sizeof(struct stat));

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
            M_STATIC_CAST(ssize_t,
                          SSIZE_MAX)); // This cast fixes some weird warnings in some cross-compilers for ARM CPUs
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
                print_str("recursive link check failed\n");
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
        print_str("Checking UIDs\n");
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
                print_str("root UID detected, getting user's ID\n");
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

bool os_Is_Directory_Secure(const char* M_NONNULL fullpath, char** outputError)
{
#if defined(__linux__) && !defined(UEFI_C_SOURCE)
    struct stat rootattrs;
    int         rootfd     = open("/", O_PATH | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    bool        rootsecure = rootfd >= 0 && fstat(rootfd, &rootattrs) == 0 && path_root_attributes_secure(&rootattrs);
    if (rootfd >= 0)
    {
        close(rootfd);
    }
    if (!rootsecure)
    {
        set_dir_security_output_error_message(
            outputError, "Error: / must be owned by UID/GID 0 and not writable by group or others.\n");
        return false;
    }
#endif
    unsigned int num_symlinks = 0U;
    return internal_OS_Is_Directory_Secure(fullpath, num_symlinks, outputError);
}

#if defined(__linux__) && !defined(UEFI_C_SOURCE)
#    define PATH_POLICY_MAX_RULES 32
#    define PATH_POLICY_MAX_BYTES 65536

typedef struct sPathDirectoryRule
{
    char         path[OPENSEA_PATH_MAX];
    bool         allowGroupWrite;
    bool         allowOtherWrite;
    bool         requireStickyBit;
    unsigned int specified;
} pathDirectoryRule;

typedef struct sPathOutputPolicy
{
    size_t            count;
    pathDirectoryRule rules[PATH_POLICY_MAX_RULES];
} pathOutputPolicy;

static bool path_policy_error(char** error, const char* message)
{
    set_dir_security_output_error_message(error, "Path-security policy: %s\n", message);
    return false;
}

static bool path_policy_canonical_name(const char* path)
{
    if (path[0] != '/' || path[1] == '\0')
    {
        return false;
    }
    for (const char* component = path + 1; *component != '\0';)
    {
        const char* end    = strchr(component, '/');
        size_t      length = end != M_NULLPTR ? M_STATIC_CAST(size_t, end - component) : strlen(component);
        if (length == 0 || (length == 1 && component[0] == '.') ||
            (length == 2 && component[0] == '.' && component[1] == '.') || (end != M_NULLPTR && end[1] == '\0'))
        {
            return false;
        }
        component += length;
        if (*component == '/')
        {
            ++component;
        }
    }
    return true;
}

static char* path_policy_trim(char* text)
{
    while (*text == ' ' || *text == '\t')
    {
        ++text;
    }
    size_t length = strlen(text);
    while (length > 0 && (text[length - 1] == ' ' || text[length - 1] == '\t' || text[length - 1] == '\r'))
    {
        text[--length] = '\0';
    }
    return text;
}

static bool path_policy_parse(char* text, size_t length, pathOutputPolicy* policy, char** error)
{
    pathDirectoryRule* rule = M_NULLPTR;
    memset(policy, 0, sizeof(*policy));
    if (length > PATH_POLICY_MAX_BYTES)
    {
        return path_policy_error(error, "file exceeds 65536 bytes");
    }
    for (size_t i = 0; i < length; ++i)
    {
        unsigned char ch = M_STATIC_CAST(unsigned char, text[i]);
        if ((ch < 32 && ch != '\n' && ch != '\r' && ch != '\t') || ch > 126)
        {
            return path_policy_error(error, "only printable ASCII, tabs, and line endings are accepted");
        }
    }
    text[length] = '\0';
    for (char* line = text; line != M_NULLPTR;)
    {
        char* next = strchr(line, '\n');
        if (next != M_NULLPTR)
        {
            *next++ = '\0';
        }
        if (strlen(line) >= OPENSEA_PATH_MAX + 64)
        {
            return path_policy_error(error, "line is too long");
        }
        line = path_policy_trim(line);
        if (*line == '[')
        {
            size_t linelen = strlen(line);
            if (linelen < 16 || strncmp(line, "[directory \"", 12) != 0 || strcmp(line + linelen - 2, "\"]") != 0 ||
                policy->count == PATH_POLICY_MAX_RULES)
            {
                return path_policy_error(error, "invalid section or too many directory rules");
            }
            line[linelen - 2] = '\0';
            const char* path  = line + 12;
            if (strlen(path) >= OPENSEA_PATH_MAX || strchr(path, '"') != M_NULLPTR || !path_policy_canonical_name(path))
            {
                return path_policy_error(error, "directory must be absolute and canonical; root rules are forbidden");
            }
            for (size_t i = 0; i < policy->count; ++i)
            {
                if (strcmp(policy->rules[i].path, path) == 0)
                {
                    return path_policy_error(error, "duplicate directory rule");
                }
            }
            rule = &policy->rules[policy->count++];
            if (safe_strcpy(rule->path, sizeof(rule->path), path) != 0)
            {
                return path_policy_error(error, "cannot copy directory rule");
            }
        }
        else if (*line != '\0' && *line != '#' && *line != ';')
        {
            char* equals = strchr(line, '=');
            if (rule == M_NULLPTR || equals == M_NULLPTR)
            {
                return path_policy_error(error, "setting outside directory section or missing equals sign");
            }
            *equals             = '\0';
            const char* key     = path_policy_trim(line);
            const char* value   = path_policy_trim(equals + 1);
            bool        enabled = strcmp(value, "true") == 0;
            if (!enabled && strcmp(value, "false") != 0)
            {
                return path_policy_error(error, "boolean must be true or false");
            }
            unsigned int field   = 0;
            bool*        setting = M_NULLPTR;
            if (strcmp(key, "allow_group_write") == 0)
            {
                field   = 1;
                setting = &rule->allowGroupWrite;
            }
            else if (strcmp(key, "allow_other_write") == 0)
            {
                field   = 2;
                setting = &rule->allowOtherWrite;
            }
            else if (strcmp(key, "require_sticky_bit") == 0)
            {
                field   = 4;
                setting = &rule->requireStickyBit;
            }
            else
            {
                return path_policy_error(error, "unknown setting");
            }
            if ((rule->specified & field) != 0)
            {
                return path_policy_error(error, "duplicate setting");
            }
            rule->specified |= field;
            *setting = enabled;
        }
        line = next;
    }
    for (size_t i = 0; i < policy->count; ++i)
    {
        rule = &policy->rules[i];
        if ((rule->allowGroupWrite || rule->allowOtherWrite) && !rule->requireStickyBit)
        {
            return path_policy_error(error, "write exceptions require require_sticky_bit=true");
        }
    }
    return true;
}

static bool path_policy_admin_attributes(const struct stat* attrs, bool directory)
{
    return (directory ? S_ISDIR(attrs->st_mode) : S_ISREG(attrs->st_mode)) && attrs->st_uid == 0 &&
           (attrs->st_mode & (S_IWGRP | S_IWOTH)) == 0;
}

static bool path_policy_load(int rootfd, pathOutputPolicy* policy, char** error)
{
    static const char* const components[] = {"etc", "opensea", "path-security.conf"};
    int                      parent       = fcntl(rootfd, F_DUPFD_CLOEXEC, 0);
    if (parent < 0)
    {
        return path_policy_error(error, "cannot duplicate root descriptor");
    }
    memset(policy, 0, sizeof(*policy));
    for (size_t i = 0; i < 3; ++i)
    {
        bool directory = i < 2;
        int  fd        = openat(parent, components[i],
                                O_NOFOLLOW | O_CLOEXEC | (directory ? O_PATH | O_DIRECTORY : O_RDONLY | O_NONBLOCK));
        int  openerror = errno;
        close(parent);
        if (fd < 0)
        {
            if (openerror == ENOENT)
            {
                return true;
            }
            return path_policy_error(error, "cannot securely open /etc/opensea/path-security.conf");
        }
        struct stat attrs;
        if (fstat(fd, &attrs) != 0 || !path_policy_admin_attributes(&attrs, directory))
        {
            close(fd);
            return path_policy_error(error,
                                     "policy and containing directories must be root-owned without untrusted write");
        }
        if (directory)
        {
            parent = fd;
            continue;
        }
        if (attrs.st_size < 0 || attrs.st_size > PATH_POLICY_MAX_BYTES)
        {
            close(fd);
            return path_policy_error(error, "file exceeds 65536 bytes");
        }
        char* text = M_REINTERPRET_CAST(char*, safe_calloc(PATH_POLICY_MAX_BYTES + 1, sizeof(char)));
        if (text == M_NULLPTR)
        {
            close(fd);
            return path_policy_error(error, "cannot allocate parser buffer");
        }
        size_t  used   = 0;
        ssize_t amount = 0;
        do
        {
            amount = read(fd, text + used, PATH_POLICY_MAX_BYTES + 1 - used);
            if (amount > 0)
            {
                used += M_STATIC_CAST(size_t, amount);
            }
        } while ((amount > 0 && used <= PATH_POLICY_MAX_BYTES) || (amount < 0 && errno == EINTR));
        struct stat after;
        bool        unchanged =
            fstat(fd, &after) == 0 && after.st_size == attrs.st_size && after.st_mtim.tv_sec == attrs.st_mtim.tv_sec &&
            after.st_mtim.tv_nsec == attrs.st_mtim.tv_nsec && after.st_ctim.tv_sec == attrs.st_ctim.tv_sec &&
            after.st_ctim.tv_nsec == attrs.st_ctim.tv_nsec && path_policy_admin_attributes(&after, false);
        close(fd);
        bool valid = false;
        if (amount != 0 || used > PATH_POLICY_MAX_BYTES || !unchanged || used != M_STATIC_CAST(size_t, attrs.st_size))
        {
            path_policy_error(error, "read failed, file changed, or size limit exceeded");
        }
        else
        {
            valid = path_policy_parse(text, used, policy, error);
            for (size_t rule = 0; valid && rule < policy->count; ++rule)
            {
                char* resolved = realpath(policy->rules[rule].path, M_NULLPTR);
                struct stat ruleattrs;
                if (resolved == M_NULLPTR || strcmp(resolved, policy->rules[rule].path) != 0 ||
                    stat(resolved, &ruleattrs) != 0 || !S_ISDIR(ruleattrs.st_mode))
                {
                    valid = path_policy_error(error, "rules must name existing canonical directories, not aliases");
                }
                safe_free(&resolved);
            }
        }
        safe_free(&text);
        return valid;
    }
    return path_policy_error(error, "incomplete policy traversal");
}

static bool path_output_component_secure(const char*             path,
                                         const struct stat*      attrs,
                                         bool                    leaf,
                                         const pathOutputPolicy* policy,
                                         bool*                   exceptionUsed,
                                         char**                  error)
{
    if (!S_ISDIR(attrs->st_mode) ||
        (attrs->st_uid != 0 && attrs->st_uid != geteuid() && !(geteuid() == 0 && attrs->st_uid == get_sudo_uid())))
    {
        return path_policy_error(error, "output directory has an untrusted owner or type");
    }
    const pathDirectoryRule* rule = M_NULLPTR;
    for (size_t i = 0; i < policy->count; ++i)
    {
        if (strcmp(policy->rules[i].path, path) == 0)
        {
            rule = &policy->rules[i];
            break;
        }
    }
    if (rule != M_NULLPTR && rule->requireStickyBit && (attrs->st_mode & S_ISVTX) == 0)
    {
        return path_policy_error(error, "configured output ancestor is missing its required sticky bit");
    }
    mode_t writable = attrs->st_mode & (S_IWGRP | S_IWOTH);
    if (writable == 0)
    {
        return true;
    }
    // Exceptions admit shared ancestors, never a shared output directory.
    if (leaf || rule == M_NULLPTR || attrs->st_uid != 0 || (attrs->st_mode & S_ISVTX) == 0 ||
        ((writable & S_IWGRP) != 0 && !rule->allowGroupWrite) || ((writable & S_IWOTH) != 0 && !rule->allowOtherWrite))
    {
        set_dir_security_output_error_message(error, "Insecure output directory component: %s\n", path);
        return false;
    }
    *exceptionUsed = true;
    return true;
}

static int path_output_parent(int                     rootfd,
                              const char*             path,
                              const pathOutputPolicy* policy,
                              bool*                   exceptionUsed,
                              char**                  error)
{
    char copy[OPENSEA_PATH_MAX];
    char prefix[OPENSEA_PATH_MAX] = "";
    if (strcmp(path, "/") != 0 && !path_policy_canonical_name(path))
    {
        path_policy_error(error, "output parent is not canonical");
        return -1;
    }
    if (safe_strcpy(copy, sizeof(copy), path) != 0)
    {
        path_policy_error(error, "output parent is too long");
        return -1;
    }
    int parent = fcntl(rootfd, F_DUPFD_CLOEXEC, 0);
    if (parent < 0)
    {
        path_policy_error(error, "cannot duplicate output root descriptor");
        return -1;
    }
    for (char* component = copy + 1; *component != '\0';)
    {
        char* next = strchr(component, '/');
        if (next != M_NULLPTR)
        {
            *next = '\0';
        }
        int fd = openat(parent, component, O_PATH | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
        close(parent);
        if (fd < 0)
        {
            path_policy_error(error, "cannot open output directory component without following links");
            return -1;
        }
        struct stat attrs;
        if (safe_strcat(prefix, sizeof(prefix), "/") != 0 || safe_strcat(prefix, sizeof(prefix), component) != 0 ||
            fstat(fd, &attrs) != 0 ||
            !path_output_component_secure(prefix, &attrs, next == M_NULLPTR, policy, exceptionUsed, error))
        {
            close(fd);
            if (error != M_NULLPTR && *error == M_NULLPTR)
            {
                path_policy_error(error, "cannot inspect output directory component");
            }
            return -1;
        }
        parent = fd;
        if (next == M_NULLPTR)
        {
            break;
        }
        component = next + 1;
    }
    return parent;
}

secureFileInfo* os_Create_Exclusive_Secure_Log(const char* filename)
{
    secureFileInfo* info = M_REINTERPRET_CAST(secureFileInfo*, safe_calloc(1, sizeof(secureFileInfo)));
    if (info == M_NULLPTR)
    {
        return M_NULLPTR;
    }
    info->error                = SEC_FILE_INSECURE_PATH;
    char**            error    = M_CONST_CAST(char**, &info->errorString);
    int               rootfd   = -1;
    int               parentfd = -1;
    int               fd       = -1;
    pathOutputPolicy* policy   = M_NULLPTR;
    char              parent[OPENSEA_PATH_MAX];
    char*             canonical = M_NULLPTR;
    char*             fullpath  = M_CONST_CAST(char*, info->fullpath);
    const char*       basename  = filename != M_NULLPTR ? strrchr(filename, '/') : M_NULLPTR;
    basename                    = basename != M_NULLPTR ? basename + 1 : filename;
    bool        exceptionUsed   = false;
    size_t      parentlen       = 0;
    struct stat attrs;

    if (basename == M_NULLPTR || *basename == '\0' || strcmp(basename, ".") == 0 || strcmp(basename, "..") == 0 ||
        strlen(filename) >= OPENSEA_PATH_MAX)
    {
        info->error = SEC_FILE_INVALID_PATH;
        path_policy_error(error, "invalid output filename");
        goto cleanup;
    }
    parentlen = M_STATIC_CAST(size_t, basename - filename);
    if (parentlen == 0)
    {
        if (safe_strcpy(parent, sizeof(parent), ".") != 0)
        {
            goto cleanup;
        }
    }
    else
    {
        memcpy(parent, filename, parentlen);
        parent[parentlen] = '\0';
    }
    rootfd = open("/", O_PATH | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (rootfd < 0 || fstat(rootfd, &attrs) != 0 || !path_root_attributes_secure(&attrs))
    {
        path_policy_error(error, "/ must be owned by UID/GID 0 and not writable by group or others");
        goto cleanup;
    }
    policy = M_REINTERPRET_CAST(pathOutputPolicy*, safe_calloc(1, sizeof(pathOutputPolicy)));
    if (policy == M_NULLPTR)
    {
        path_policy_error(error, "cannot allocate output policy");
        goto cleanup;
    }
    if (!path_policy_load(rootfd, policy, error))
    {
        goto cleanup;
    }
    canonical = realpath(parent, M_NULLPTR);
    if (canonical == M_NULLPTR || strlen(canonical) >= OPENSEA_PATH_MAX)
    {
        path_policy_error(error, "cannot canonicalize output parent");
        goto cleanup;
    }
    parentfd = path_output_parent(rootfd, canonical, policy, &exceptionUsed, error);
    if (parentfd < 0)
    {
        goto cleanup;
    }
    if (safe_strcpy(fullpath, OPENSEA_PATH_MAX, canonical) != 0 ||
        (strcmp(canonical, "/") != 0 && safe_strcat(fullpath, OPENSEA_PATH_MAX, "/") != 0) ||
        safe_strcat(fullpath, OPENSEA_PATH_MAX, basename) != 0)
    {
        info->error = SEC_FILE_INVALID_PATH;
        path_policy_error(error, "output path is too long");
        goto cleanup;
    }
    fd = openat(parentfd, basename, O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        info->error = errno == EEXIST ? SEC_FILE_FILE_ALREADY_EXISTS : SEC_FILE_FAILURE;
        set_dir_security_output_error_message(error, "Cannot exclusively create diagnostic output (errno %d)\n", errno);
        goto cleanup;
    }
    if (fstat(fd, &attrs) != 0 || !S_ISREG(attrs.st_mode) || attrs.st_uid != geteuid() || attrs.st_nlink != 1 ||
        (attrs.st_mode & (S_IRWXG | S_IRWXO)) != 0)
    {
        info->error = SEC_FILE_INVALID_FILE_ATTRIBUTES;
        path_policy_error(error, "new output file has invalid attributes");
        goto cleanup;
    }
    info->file = fdopen(fd, "wb");
    if (info->file == M_NULLPTR)
    {
        info->error = SEC_FILE_FAILURE;
        path_policy_error(error, "cannot attach stream to output descriptor");
        goto cleanup;
    }
    fd               = -1;
    info->attributes = os_Get_File_Attributes_By_File(info->file);
    info->uniqueID   = os_Get_File_Unique_Identifying_Information(info->file);
    if (info->attributes == M_NULLPTR || info->uniqueID == M_NULLPTR)
    {
        fclose(info->file);
        info->file  = M_NULLPTR;
        info->error = SEC_FILE_INVALID_FILE_ATTRIBUTES;
        path_policy_error(error, "cannot allocate output identity information");
        goto cleanup;
    }
    info->filename = strrchr(info->fullpath, '/') + 1;
    info->fileno   = fileno(info->file);
    info->isValid  = true;
    info->error    = SEC_FILE_SUCCESS;
    if (exceptionUsed)
    {
        fprintf(stderr, "WARNING: /etc/opensea/path-security.conf relaxed ancestor write checks for %s\n",
                info->fullpath);
    }

cleanup:
    if (fd >= 0)
    {
        close(fd);
    }
    if (parentfd >= 0)
    {
        close(parentfd);
    }
    if (rootfd >= 0)
    {
        close(rootfd);
    }
    safe_free_core(M_REINTERPRET_CAST(void**, &policy));
    safe_free(&canonical);
    return info;
}
#endif

bool os_Directory_Exists(const char* M_NONNULL pathToCheck)
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

bool os_File_Exists(const char* M_NONNULL filetoCheck)
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

int64_t os_Get_File_Size(FILE* M_NONNULL filePtr)
{
    struct stat st;
    M_INITIALIZE_STRUCTURE(&st, sizeof(struct stat));
    if (0 == fstat(fileno(filePtr), &st))
    {
        return st.st_size;
    }
    else
    {
        return INT64_C(-1);
    }
}

M_PARAM_RO(1) eReturnValues os_Create_Directory(const char* M_NONNULL filePath)
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
M_PARAM_RO(1) eReturnValues os_Create_Secure_Directory(const char* M_NONNULL filePath)
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
M_PARAM_RO(1)
M_PARAM_WO(2)
eReturnValues get_Full_Path(const char* M_NONNULL pathAndFile, char fullPath[M_NONNULL_ARRAY OPENSEA_PATH_MAX])
{
    errno              = 0;
    char* resolvedPath = realpath(pathAndFile, fullPath);
    if (resolvedPath != M_NULLPTR)
    {
        return SUCCESS;
    }
    else if (errno != 0)
    {
        print_str("realpath failed, reason: ");
        print_Errno_To_Screen(errno);
        return FAILURE;
    }
    else
    {
#if defined(_DEBUG)
        printf("realpath failed for %s, for unknown reason.\n", pathAndFile);
#endif
        return FAILURE;
    }
}
