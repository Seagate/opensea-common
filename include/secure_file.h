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

//! \file secure_file.h
//! \brief Defines functions for working with a file securely according to cert-c
//! recommendations and rules

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include <stdarg.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#if defined(_WIN32)
//! \def S_ISREG(m)
//! \brief Checks if the file is a regular file.
//! \param m Mode to check.
//! \return Non-zero if the file is a regular file, 0 otherwise.
#    if !defined(S_ISREG)
#        define S_ISREG(m) (m & _S_IFMT) == _S_IFREG
#    endif // S_ISREG

//! \def S_ISDIR(m)
//! \brief Checks if the file is a directory.
//! \param m Mode to check.
//! \return Non-zero if the file is a directory, 0 otherwise.
#    if !defined(S_ISDIR)
#        define S_ISDIR(m) (m & _S_IFMT) == _S_IFDIR
#    endif // S_ISDIR

//! \def S_ISCHR(m)
//! \brief Checks if the file is a character device.
//! \param m Mode to check.
//! \return Non-zero if the file is a character device, 0 otherwise.
#    if !defined(S_ISCHR)
#        define S_ISCHR(m) (m & _S_IFMT) == _S_IFCHR
#    endif // S_ISCHR

//! \def S_ISBLK(m)
//! \brief Checks if the file is a block device.
//! \param m Mode to check.
//! \return Always returns 0 as Windows does not have a flag for block devices.
#    if !defined(S_ISBLK)
#        define S_ISBLK(m) (0) /* Windows doesn't have a flag for this */
#    endif                     // S_ISBLK

//! \def S_ISFIFO(m)
//! \brief Checks if the file is a FIFO (named pipe).
//! \param m Mode to check.
//! \return Non-zero if the file is a FIFO, 0 otherwise.
#    if !defined(S_ISFIFO)
#        define S_ISFIFO(m) (m & _S_IFMT) == _S_IFIFO
#    endif // S_ISFIFO

//! \def S_ISLNK(m)
//! \brief Checks if the file is a symbolic link.
//! \param m Mode to check.
//! \return Always returns 0 as Windows does not have a flag for symbolic links.
#    if !defined(S_ISLNK)
#        define S_ISLNK(m) (0) /* Windows doesn't have a flag for this */
#    endif                     // S_ISLNK

//! \def S_ISSOCK(m)
//! \brief Checks if the file is a socket.
//! \param m Mode to check.
//! \return Always returns 0 as Windows does not have a flag for sockets.
#    if !defined(S_ISSOCK)
#        define S_ISSOCK(m) (0) /* Windows doesn't have a flag for this */
#    endif                      // S_ISSOCK

//! \def S_TYPEISMQ(buf)
//! \brief Checks if the file is a message queue.
//! \param buf Buffer to check.
//! \return Always returns 0 as Windows does not have a flag for message queues.
#    if !defined(S_TYPEISMQ)
#        define S_TYPEISMQ(buf) (0) /* Windows doesn't have a flag for this */
#    endif                          // S_TYPEISMQ

//! \def S_TYPEISSEM(buf)
//! \brief Checks if the file is a semaphore.
//! \param buf Buffer to check.
//! \return Always returns 0 as Windows does not have a flag for semaphores.
#    if !defined(S_TYPEISSEM)
#        define S_TYPEISSEM(buf) (0) /* Windows doesn't have a flag for this */
#    endif                           // S_TYPEISSEM

//! \def S_TYPEISSHM(buf)
//! \brief Checks if the file is a shared memory object.
//! \param buf Buffer to check.
//! \return Always returns 0 as Windows does not have a flag for shared memory objects.
#    if !defined(S_TYPEISSHM)
#        define S_TYPEISSHM(buf) (0) /* Windows doesn't have a flag for this */
#    endif                           // S_TYPEISSHM

#endif // _WIN32

//! \def FILE_UNIQUE_ID_ARR_MAX
//! \brief Maximum size of the file ID array.
//! \note Windows may use 8 or 16 bytes depending on the filesystem.
//! FAT/NTFS can use only 64 bits, but REFS needs at least 128 bits to identify this uniquely.
#define FILE_UNIQUE_ID_ARR_MAX (16)

    //! \struct sfileUniqueIDInfo
    //! \brief Structure to hold unique file ID information for cross-platform compatibility.
    //! \note Windows uses unsigned long for volume serial number (vol SN) and two unsigned longs for index high/low.
    //! REFS needs a 128-bit identifier, requiring the extended version to read the full identifier. Linux uses ino_t
    //! and dev_t to check for unique file IDs. Both POSIX and Windows recommend combining elements to create a unique
    //! ID (WINDOWS: vol SN + file index, POSIX: ino_t + dev_t). This structure uses a generic array to hold the
    //! necessary bytes of data.
    typedef struct sfileUniqueIDInfo
    {
        union
        {
            //! \var inode
            //! \brief Inode number for the file (used in POSIX).
            uint64_t inode;

            //! \var volsn
            //! \brief Volume serial number for the file (used in Windows).
            uint64_t volsn;
        };

        union
        {
            //! \var deviceid
            //! \brief Device ID from struct stat in POSIX.
            uint64_t deviceid;

            //! \var fileid
            //! \brief File ID array for Windows.
            //! \note Windows may use 8 or 16 bytes depending on the filesystem. FAT/NTFS can use only 64 bits, but REFS
            //! needs at least 128 bits to identify this uniquely.
            uint8_t fileid[FILE_UNIQUE_ID_ARR_MAX];
        };
    } fileUniqueIDInfo;

    //! \fn bool compare_File_Unique_ID(const fileUniqueIDInfo* a, const fileUniqueIDInfo* b)
    //! \brief Compares two fileUniqueIDInfo structures to determine if they represent the same file.
    //! \param[in] a Pointer to the first fileUniqueIDInfo structure.
    //! \param[in] b Pointer to the second fileUniqueIDInfo structure.
    //! \return true if the structures represent the same file, false otherwise.
    //! \note This function is used because a memcmp is not appropriate for comparing fileUniqueIDInfo structures.
    bool compare_File_Unique_ID(const fileUniqueIDInfo* a, const fileUniqueIDInfo* b);

    //! \fn M_NODISCARD fileUniqueIDInfo* os_Get_File_Unique_Identifying_Information(FILE* file)
    //! \brief Retrieves unique identifying information for a given file.
    //! \param[in] file Pointer to the FILE object representing the file.
    //! \return Pointer to a fileUniqueIDInfo structure containing the unique identifying information for the file.
    //! \note The M_NODISCARD attribute indicates that the return value should not be discarded.
    //! \note The returned pointer must be freed by the caller.
    M_NODISCARD fileUniqueIDInfo* os_Get_File_Unique_Identifying_Information(FILE* file);

    //! \fn M_NODISCARD bool os_Is_Directory_Secure(const char* fullpath, char** outputError)
    //! \brief Evaluates if every directory in the given path is secure.
    //! \param[in] fullpath The full path to evaluate.
    //! \param[out] outputError Pointer to a string that will hold an error message if the function fails.
    //! Can be null. If not null, the error message must be freed by calling safe_free() when done.
    //! \return true if all directories in the path are secure, false otherwise.
    //! \note Security is determined following the CERT-C rule FIO-15: Ensure that file operations are performed
    //! in a secure directory
    //! (https://wiki.sei.cmu.edu/confluence/display/c/FIO15-C.+Ensure+that+file+operations+are+performed+in+a+secure+directory).
    //! \note The function ensures that the full real path, from the root of the system/volume to the last directory
    //!  specified, is secure.
    //! \note Each directory must be owned by either the current user or root.
    //! \note Each directory must not be writable by groups or others to be secure and free of tampering while working
    //! within it.
    M_NODISCARD bool os_Is_Directory_Secure(const char* fullpath, char** outputError);

    //! \struct sfileAttributes
    //! \brief Structure to hold file attributes for cross-platform compatibility.
    //! \note Most members of this structure match the stat structure. There are some differences which is why we define
    //! it without that structure. The main reason to NOT use struct stat is that Windows has a version, but to get the
    //! 64-bit version would make this a mess to define. So, we define it in a way that is best for cross-platform use.
    //! Some fields may not be set to anything other than 0 on Windows due to not having the same concepts as on
    //! Unix/Unix-like systems.
    typedef struct sfileAttributes
    {
        //! \var deviceID
        //! \brief Device ID.
        dev_t deviceID;

        //! \var inode
        //! \brief Inode number.
        ino_t inode;

        //! \var filemode
        //! \brief File mode (permissions).
        mode_t filemode;

        //! \var numberOfLinks
        //! \brief Number of hard links.
        nlink_t numberOfLinks;

        //! \var userID
        //! \brief User ID of the file owner.
        uid_t userID;

        //! \var groupID
        //! \brief Group ID of the file owner.
        gid_t groupID;

        //! \var representedDeviceID
        //! \brief Device ID if this file represents a device.
        dev_t representedDeviceID;

        //! \var filesize
        //! \brief Size of the file in bytes.
        oscoffset_t filesize;

        //! \var fileLastAccessTime
        //! \brief Last access time in milliseconds since Unix epoch (converted from Windows file epoch on Windows).
        int64_t fileLastAccessTime;

        //! \var fileModificationTime
        //! \brief Last modification time in milliseconds since Unix epoch (converted from Windows file epoch on
        //! Windows).
        int64_t fileModificationTime;

        //! \var fileStatusChangeTime
        //! \brief Last status change time in milliseconds since Unix epoch (converted from Windows file epoch on
        //! Windows).
        int64_t fileStatusChangeTime;

        //! \var fileFlags
        //! \brief File flags (Windows-specific).
        uint32_t fileFlags;

        //! \var securityControlFlags
        //! \brief Security control flags (Windows-specific).
        uint16_t securityControlFlags;

#if defined(_WIN32) // TODO: since these are "generic" types and not Windows unique types, do they need to be in this
                    // wrapper???
        //! \var securityDescriptorStringLength
        //! \brief Length of the security descriptor string (Windows-specific).
        unsigned long securityDescriptorStringLength;

        //! \var winSecurityDescriptor
        //! \brief Security descriptor string (Windows-specific). Allocated by malloc. Call memset_explicit on this to
        //! zero it out before freeing.
        char* winSecurityDescriptor;
#endif // _WIN32
    } fileAttributes;

    //! \fn void free_File_Attributes(fileAttributes** attributes)
    //! \brief Frees the memory allocated for a fileAttributes structure.
    //! \param[in,out] attributes Pointer to the fileAttributes structure to be freed. The pointer will be set to NULL
    //! after the memory is freed. \note The Windows SIDs are allocated and require deletion of their own. This will be
    //! cleaned up when calling this function.
    void free_File_Attributes(fileAttributes** attributes);

    //! \fn M_NODISCARD fileAttributes* os_Get_File_Attributes_By_Name(const char* filetoCheck)
    //! \brief Retrieves the attributes of a file or directory by its name.
    //! \param[in] filetoCheck The path to the file or directory to check.
    //! \return Pointer to a fileAttributes structure containing the attributes of the file or directory.
    //! \note The M_NODISCARD attribute indicates that the return value should not be discarded.
    //! \note The returned pointer must be freed by the caller using free_File_Attributes().
    M_NODISCARD fileAttributes* os_Get_File_Attributes_By_Name(const char* filetoCheck);

    //! \fn M_NODISCARD fileAttributes* os_Get_File_Attributes_By_File(FILE* file)
    //! \brief Retrieves the attributes of a file by its FILE pointer.
    //! \param[in] file Pointer to the FILE object representing the file.
    //! \return Pointer to a fileAttributes structure containing the attributes of the file.
    //! \note This is the preferred method to ensure that the attributes are read from exactly the same file as
    //! expected once the file has been opened.
    //! \note The M_NODISCARD attribute indicates that the return value should not be discarded.
    //! \note The returned pointer must be freed by the caller using free_File_Attributes().
    M_NODISCARD fileAttributes* os_Get_File_Attributes_By_File(FILE* file);

    //! \enum eSecureFileError
    //! \brief Secure file API error codes.
    M_DECLARE_ENUM(eSecureFileError,
                   SEC_FILE_SUCCESS,
                   SEC_FILE_INVALID_FILE,
                   SEC_FILE_INVALID_PATH,
                   SEC_FILE_FILE_ALREADY_EXISTS,
                   SEC_FILE_INVALID_FILE_EXTENSION,
                   SEC_FILE_INVALID_FILE_ATTRIBUTES,
                   SEC_FILE_INVALID_FILE_ATTRIBTUES = SEC_FILE_INVALID_FILE_ATTRIBUTES, /* Typo. Migrate to correct spelling */
                   SEC_FILE_INVALID_FILE_UNIQUE_ID,
                   SEC_FILE_INSECURE_PATH,
                   SEC_FILE_INVALID_MODE,
                   SEC_FILE_INVALID_SECURE_FILE,
                   SEC_FILE_FAILURE_CLOSING_FILE,
                   SEC_FILE_BUFFER_TOO_SMALL,
                   SEC_FILE_INVALID_PARAMETER,
                   SEC_FILE_READ_WRITE_ERROR,
                   SEC_FILE_END_OF_FILE_REACHED,
                   SEC_FILE_WRITE_DISK_FULL,
                   SEC_FILE_SEEK_FAILURE,
                   SEC_FILE_FLUSH_FAILURE,
                   SEC_FILE_CANNOT_REMOVE_FILE_STILL_OPEN,
                   SEC_FILE_FAILURE);

    //! \var SEC_FILE_SUCCESS
    //! \brief Successful return from secure file API call.

    //! \var SEC_FILE_INVALID_FILE
    //! \brief Invalid file.

    //! \var SEC_FILE_INVALID_PATH
    //! \brief Invalid path.

    //! \var SEC_FILE_FILE_ALREADY_EXISTS
    //! \brief Attempting to write and create a file that already exists.

    //! \var SEC_FILE_INVALID_FILE_EXTENSION
    //! \brief Invalid file extension.

    //! \var SEC_FILE_INVALID_FILE_ATTRIBUTES
    //! \brief Invalid file attributes structure.

    //! \var SEC_FILE_INVALID_FILE_UNIQUE_ID
    //! \brief Invalid file unique ID structure.

    //! \var SEC_FILE_INSECURE_PATH
    //! \brief File path is not secure. Cannot continue with secure file API.

    //! \var SEC_FILE_INVALID_MODE
    //! \brief Mode string contains invalid options.

    //! \var SEC_FILE_INVALID_SECURE_FILE
    //! \brief Passed in M_NULLPTR secureFileInfo structure to a function that requires this.

    //! \var SEC_FILE_FAILURE_CLOSING_FILE
    //! \brief A failure occurred while trying to close the file.

    //! \var SEC_FILE_BUFFER_TOO_SMALL
    //! \brief Provided buffer is too small for read/write.

    //! \var SEC_FILE_INVALID_PARAMETER
    //! \brief This can be returned if a function (like fread/fwrite) requires a parameter that was not provided.

    //! \var SEC_FILE_READ_WRITE_ERROR
    //! \brief Did not read or write as many bytes as requested, cannot determine a more specific reason.

    //! \var SEC_FILE_END_OF_FILE_REACHED
    //! \brief Reached the end of the file...can be a case of success if this is expected.

    //! \var SEC_FILE_WRITE_DISK_FULL
    //! \brief Cannot write any more data due to an error from running out of space.

    //! \var SEC_FILE_SEEK_FAILURE
    //! \brief Cannot seek to the specified offset in the file.

    //! \var SEC_FILE_FLUSH_FAILURE
    //! \brief Failure while flushing the secure file occurred.

    //! \var SEC_FILE_CANNOT_REMOVE_FILE_STILL_OPEN
    //! \brief The ability to call a function like POSIX's unlink is not possible as there is no such function to
    //! provide this behavior.

    //! \var SEC_FILE_FAILURE
    //! \brief Generic undefinable error.

    //! \struct secureFileInfo
    //! \brief Structure to hold secure file information.
    //! Free this using the function free_Secure_File_Info
    //! \note Most members of this structure match the stat structure.
    //! There are some differences which is why we define it without that structure.
    //! The main reason to NOT use struct stat is that Windows has a version, but to get the 64-bit version
    //!  would make this a mess to define. So, we define it in a way that is best for cross-platform use.
    //! Some fields may not be set to anything other than 0 on Windows due to not having the same concepts as on
    //! Unix/Unix-like systems.
    typedef struct ssecureFileInfo
    {
        //! \var error
        //! \brief Error code indicating the status of the secure file operation.
        eSecureFileError error;

        //! \var isValid
        //! \brief Indicates if the secure file information is valid.
        bool isValid;

        //! \var file
        //! \brief Pointer to the FILE object.
        FILE* file;

        //! \var fullpath
        //! \brief Contains the full canonicalized path and filename.
        const char fullpath[OPENSEA_PATH_MAX];

        //! \var filename
        //! \brief Pointer to just the name section in the fullpath above.
        const char* filename;

        //! \var fileno
        //! \brief POSIX fileno. Translated using fileno(FILE *file).
        int fileno;

        //! \var fileSize
        //! \brief Converted from the file attributes file size. May be smaller if size_t cannot represent the whole
        //! file size. Ex: 4GB file on 32-bit OS.
        size_t fileSize;

        //! \var attributes
        //! \brief Pointer to the file attributes structure.
        fileAttributes* attributes;

        //! \var uniqueID
        //! \brief Pointer to the unique file ID structure.
        fileUniqueIDInfo* uniqueID;

        //! \var errorString
        //! \brief If non-NULL, contains a human-readable error message for the failure that occurred. Dynamically
        //! allocated.
        const char* errorString;
    } secureFileInfo;

    //! \fn void free_Secure_File_Info(secureFileInfo** fileInfo)
    //! \brief Securely frees the memory allocated for a secureFileInfo structure.
    //! \param[in,out] fileInfo Pointer to the secureFileInfo structure to be freed. The pointer will be set to NULL
    //! after the memory is freed.
    //! \note The Windows SIDs are allocated and require deletion of their own. This will be cleaned up when calling
    //! this function.
    void free_Secure_File_Info(secureFileInfo** fileInfo);

    //! \struct sfileExt
    //! \brief Structure to specify a list of supported file extensions for secure file opening.
    //! \note This structure is optional, but if you want to only open files with specific extensions, this will
    //! evaluate that based on the passed-in list. The list is terminated by a final entry with the string "ext"
    //! set to a NULL pointer.
    typedef struct sfileExt
    {
        //! \var ext
        //! \brief The file extension to check.
        //! \note For the terminating value in this list, set ext to M_NULLPTR.
        const char* ext;

        //! \var caseInsensitive
        //! \brief Indicates whether the comparison should be case-insensitive.
        //! \note By default, the comparison is case-sensitive. Change this to true if you want to allow extensions
        //! like .bin, .BIN, .bIN, .Bin, etc.
        bool caseInsensitive;
    } fileExt;

    //! \fn M_NODISCARD secureFileInfo* secure_Open_File(const char* filename,
    //!                                                  const char* mode,
    //!                                                  const fileExt* extList /*optional*/,
    //!                                                  fileAttributes* expectedFileInfo /*optional*/,
    //!                                                  fileUniqueIDInfo* uniqueIdInfo /*optional*/)
    //! \brief Opens a file securely, performing necessary security validation to ensure it is a valid file on the
    //! system.
    //! \param[in] filename The name of the file to open.
    //! \param[in] mode The mode in which to open the file.
    //! \param[in] extList Optional list of file extensions to open the file with. Any extension found that does not
    //! match will be treated as an error/file not found
    //! \param[in] expectedFileInfo Optional expected file attributes
    //! for validation that a file matches known attributes.
    //! \param[in] uniqueIdInfo Optional unique file ID information
    //! for validation that a file matches known unique identifying info
    //! \return Pointer to a secureFileInfo structure
    //! containing the secure file information.
    //! \note The purpose of this function is to perform the security validation
    //! necessary to make sure this is a valid file on the system and minimize path traversal and validate permissions
    //! as much as reasonably possible. The goal is mitigation of CWE-22: Improper Limitation of a Pathname to a
    //! Restricted Directory ('Path Traversal'). Will be using recommendations from CERT-C FIO15-C: Ensure that file
    //! operations are performed in a secure directory as much as possible to accomplish this.
    //! \note expectedFileInfo
    //! can be M_NULLPTR for the first time opening a file. If reopening a file used earlier, it is recommended to
    //! provide this info so it can be validated as the same file. It is recommended to not reopen files, but that may
    //! not always be possible. So this exists to help validate that a file has not changed in some unexpected way.
    M_NODISCARD secureFileInfo* secure_Open_File(const char*       filename,
                                                 const char*       mode,
                                                 const fileExt*    extList /*optional*/,
                                                 fileAttributes*   expectedFileInfo /*optional*/,
                                                 fileUniqueIDInfo* uniqueIdInfo /*optional*/);

    // M_NODISCARD secureFileInfo* secure_Reopen_File(secureFileInfo* fileInfo);

    //! \fn M_NODISCARD eSecureFileError secure_Close_File(secureFileInfo* fileInfo)
    //! \brief Closes a file that was opened securely. This does NOT free any part of secureFileInfo.
    //! \param[in] fileInfo pointer to secureFileInfo structure for securely opened file.
    //! \return Error code indicating whether the file closed successfully or not.
    //! \note If a file fails to close successfully, then it is no longer to be used at all.
    //! The caller MUST check this result to know if something went wrong to warn the user
    //! and to handle this error properly and prevent the software from using this file again.
    M_NODISCARD eSecureFileError secure_Close_File(secureFileInfo* fileInfo);

    // TODO: Need to finish the secure rename functions
    // M_DECLARE_ENUM(eSecureFileRename, SEC_RENAME_DO_NOT_REPLACE_EXISTING, SEC_RENAME_REPLACE_EXISTING);

    // NOTE: This will convert the filename into a canonical path internally to
    // ensure a valid path is provided.
    //       Since the low-level APIs need a file name rather than FILE *, this
    //       takes the same kind of input.
    // eSecureFileError secure_Rename_File_By_Name(const char* filename,
    // eSecureFileRename renameInfo);

    //! \fn M_NODISCARD eSecureFileError secure_Read_File(secureFileInfo* M_RESTRICT fileInfo, void* M_RESTRICT buffer,
    //! size_t buffersize, size_t elementsize, size_t count, size_t* numberread /*optional*/)
    //! \brief Reads data from a secure file.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \param[out] buffer Pointer to the buffer where the read data will be stored.
    //! \param[in] buffersize The size of the buffer in bytes.
    //! \param[in] elementsize The size of each element to be read.
    //! \param[in] count The number of elements to be read.
    //! \param[out] numberread Optional pointer to a size_t variable where the number of elements read will be stored.
    //! \return eSecureFileError indicating the result of the read operation.
    //! \note This function performs error checking and size validation internally based on input parameters.
    M_NODISCARD eSecureFileError secure_Read_File(secureFileInfo* M_RESTRICT fileInfo,
                                                  void* M_RESTRICT           buffer,
                                                  size_t                     buffersize,
                                                  size_t                     elementsize,
                                                  size_t                     count,
                                                  size_t*                    numberread /*optional*/);

    //! \fn M_NODISCARD eSecureFileError secure_Write_File(secureFileInfo* M_RESTRICT fileInfo, void* M_RESTRICT buffer,
    //! size_t buffersize, size_t elementsize, size_t count, size_t* numberwritten /*optional*/)
    //! \brief Writes data to a secure file.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \param[in] buffer Pointer to the buffer containing the data to be written.
    //! \param[in] buffersize The size of the buffer in bytes.
    //! \param[in] elementsize The size of each element to be written.
    //! \param[in] count The number of elements to be written.
    //! \param[out] numberwritten Optional pointer to a size_t variable where the number of elements written
    //! will be stored.
    //! \return eSecureFileError indicating the result of the write operation.
    //! \note This function performs error checking and size validation internally based on input parameters.
    M_NODISCARD eSecureFileError secure_Write_File(secureFileInfo* M_RESTRICT fileInfo,
                                                   void* M_RESTRICT           buffer,
                                                   size_t                     buffersize,
                                                   size_t                     elementsize,
                                                   size_t                     count,
                                                   size_t*                    numberwritten /*optional*/);

    //! \fn M_NODISCARD eSecureFileError secure_Seek_File(secureFileInfo* fileInfo, oscoffset_t offset, int
    //! initialPosition)
    //! \brief Sets the file position indicator for a secure file.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \param[in] offset The offset to set the file position indicator to.
    //! \param[in] initialPosition The initial position from which the offset is applied (SEEK_SET, SEEK_CUR, or
    //! SEEK_END).
    //! \return eSecureFileError indicating the result of the seek operation.
    M_NODISCARD eSecureFileError secure_Seek_File(secureFileInfo* fileInfo, oscoffset_t offset, int initialPosition);

    //! \fn M_NODISCARD eSecureFileError secure_Rewind_File(secureFileInfo* fileInfo)
    //! \brief Sets the file position indicator to the beginning of a secure file.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \return eSecureFileError indicating the result of the rewind operation.
    M_NODISCARD eSecureFileError secure_Rewind_File(secureFileInfo* fileInfo);

    //! \fn M_NODISCARD oscoffset_t secure_Tell_File(secureFileInfo* fileInfo)
    //! \brief Gets the current file position indicator for a secure file.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \return The current file position indicator.
    M_NODISCARD oscoffset_t secure_Tell_File(secureFileInfo* fileInfo);

    //! \fn M_NODISCARD eSecureFileError secure_Remove_File(secureFileInfo* fileInfo)
    //! \brief Removes a secure file. If the file is still open, it will be unlinked; otherwise, it will be removed.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \return eSecureFileError indicating the result of the remove operation.
    M_NODISCARD eSecureFileError secure_Remove_File(secureFileInfo* fileInfo);

    //! \enum eSecureFileDeleteNameAction
    //! \brief Enum for actions to take when deleting a file by name.
    M_DECLARE_ENUM(eSecureFileDeleteNameAction, SEC_DELETE_NAME_FAIL_IF_OPEN, SEC_DELETE_NAME_UNLINK_IF_OPEN);

    //! \fn M_NODISCARD eSecureFileError secure_Delete_File_By_Name(const char* filename, eSecureFileDeleteNameAction
    //! deleteAction)
    //! \brief Deletes a file by name, with specified action if the file is open.
    //! \param[in] filename The name of the file to delete.
    //! \param[in] deleteAction The action to take if the file is open (fail or unlink).
    //! \return eSecureFileError indicating the result of the delete operation.
    M_NODISCARD eSecureFileError secure_Delete_File_By_Name(const char*                 filename,
                                                            eSecureFileDeleteNameAction deleteAction);

    //! \fn eSecureFileError secure_Flush_File(secureFileInfo* fileInfo)
    //! \brief Flushes the output buffer of a secure file.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \return eSecureFileError indicating the result of the flush operation.
    eSecureFileError secure_Flush_File(secureFileInfo* fileInfo);

    //! \fn eSecureFileError secure_GetPos_File(secureFileInfo* M_RESTRICT fileInfo, fpos_t* M_RESTRICT pos)
    //! \brief Gets the current file position indicator for a secure file.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \param[out] pos Pointer to an fpos_t variable where the current file position will be stored.
    //! \return eSecureFileError indicating the result of the get position operation.
    eSecureFileError secure_GetPos_File(secureFileInfo* M_RESTRICT fileInfo, fpos_t* M_RESTRICT pos);

    //! \fn eSecureFileError secure_SetPos_File(secureFileInfo* fileInfo, const fpos_t* pos)
    //! \brief Sets the file position indicator for a secure file.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \param[in] pos Pointer to an fpos_t variable containing the new file position.
    //! \return eSecureFileError indicating the result of the set position operation.
    eSecureFileError secure_SetPos_File(secureFileInfo* fileInfo, const fpos_t* pos);

    //! \fn FUNC_ATTR_PRINTF(2, 0) eSecureFileError secure_vfprintf_File(secureFileInfo* M_RESTRICT fileInfo,
    //!                                                                  const char* M_RESTRICT format,
    //!                                                                  va_list args)
    //! \brief Writes formatted output to a secure file using a variable argument list.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \param[in] format The format string.
    //! \param[in] args The variable argument list.
    //! \return eSecureFileError indicating the result of the write operation.
    //! \note This function works like vfprintf_s from the C11 standard for the secure file structure.
    FUNC_ATTR_PRINTF(2, 0)
    eSecureFileError secure_vfprintf_File(secureFileInfo* M_RESTRICT fileInfo,
                                          const char* M_RESTRICT     format,
                                          va_list                    args);

    //! \fn FUNC_ATTR_PRINTF(2, 3) eSecureFileError secure_fprintf_File(secureFileInfo* M_RESTRICT fileInfo,
    //!                                                                 const char* M_RESTRICT format,
    //!                                                                 ...)
    //! \brief Writes formatted output to a secure file.
    //! \param[in] fileInfo Pointer to the secureFileInfo structure representing the file.
    //! \param[in] format The format string.
    //! \param[in] ... The variable arguments.
    //! \return eSecureFileError indicating the result of the write operation.
    //! \note This function works like fprintf_s from the C11 standard for the secure file structure.
    FUNC_ATTR_PRINTF(2, 3)
    eSecureFileError secure_fprintf_File(secureFileInfo* M_RESTRICT fileInfo, const char* M_RESTRICT format, ...);

    //! \fn bool os_Directory_Exists(const char* pathToCheck)
    //! \brief Platform-independent helper to check if a directory exists.
    //! \param[in] pathToCheck The path that needs to be evaluated.
    //! \return true if the directory exists, false if it does not.
    //! \note WARNING: May not work with UNICODE path.
    bool os_Directory_Exists(const char* pathToCheck);

    //! \fn bool os_File_Exists(const char* filetoCheck)
    //! \brief Platform-independent helper to check if a file exists.
    //! \param[in] filetoCheck The file that needs to be evaluated.
    //! \return true if the file exists, false if it does not.
    //! \note WARNING: May not work with UNICODE path.
    bool os_File_Exists(const char* filetoCheck);

    //! \fn eReturnValues os_Create_Directory(const char* filePath)
    //! \brief Creates a new directory.
    //! \param[in] filePath The path of the directory to create.
    //! \return SUCCESS on successful directory creation, FAILURE when directory creation fails.
    //! \note Recommend using os_Create_Secure_Directory instead!
    eReturnValues os_Create_Directory(const char* filePath);

    //! \fn eReturnValues os_Create_Secure_Directory(const char* filePath)
    //! \brief Creates a directory with permissions acceptable for secure path validation.
    //! Directories are created with user permissions for rwx, group permissions for rx, and
    //! other permissions for rx.
    //! \param[in] filePath The path of the directory to create.
    //! \return SUCCESS on successful directory creation, FAILURE when directory creation fails.
    eReturnValues os_Create_Secure_Directory(const char* filePath);

    //! \fn eReturnValues get_Full_Path(const char* pathAndFile, char fullPath[OPENSEA_PATH_MAX])
    //! \brief Retrieves the full canonicalized path for a given file.
    //! \param[in] pathAndFile The path and file name to evaluate.
    //! \param[out] fullPath The buffer to store the full canonicalized path.
    //! \return SUCCESS on successful path retrieval, FAILURE when path retrieval fails.
    //! \note Windows and 'nix require a file to use for finding a path as far as I can tell.
    eReturnValues get_Full_Path(const char* pathAndFile, char fullPath[OPENSEA_PATH_MAX]);

    //! \fn eReturnValues replace_File_Name_In_Path(char fullPath[OPENSEA_PATH_MAX], char* newFileName)
    //! \brief Replaces the current file name in the full path with a new one.
    //! This can be useful for opening a file in the same location with a different name or
    //! to output to a file to the same location with a different name.
    //! \return SUCCESS when this completes without an error, otherwise returns a failure code.
    eReturnValues replace_File_Name_In_Path(char fullPath[OPENSEA_PATH_MAX], char* newFileName);

    //! \fn int64_t os_Get_File_Size(FILE* filePtr)
    //! \brief Returns the size of the file pointed to by the incoming file pointer.
    //! \param[in] filePtr Pointer to a file that you wish to get the size of.
    //! \return int64_t file size in bytes. -1 is returned if there is an error.
    //! \note This uses GetFileSizeEx in Windows and fstat64 in Unix-like systems.
    int64_t os_Get_File_Size(FILE* filePtr);

#if defined(_WIN32)
    //! \fn bool exact_Compare_SIDS_And_DACL_Strings(const char* sidsAndDACLstr1, const char* sidsAndDACLstr2)
    //! \brief Compares two SID and DACL strings for exact equality.
    //! \param[in] sidsAndDACLstr1 The first SID and DACL string to compare.
    //! \param[in] sidsAndDACLstr2 The second SID and DACL string to compare.
    //! \return true if the strings are exactly equal, false otherwise.
    bool exact_Compare_SIDS_And_DACL_Strings(const char* sidsAndDACLstr1, const char* sidsAndDACLstr2);
#endif

    //! \enum eLogFileNamingConvention
    //! \brief Enum for log file naming conventions.
    typedef enum eLogFileNamingConventionEnum
    {
        NAMING_SERIAL_NUMBER_DATE_TIME, //!< Use serial number and date/time to avoid collisions with existing files.
        NAMING_SERIAL_NUMBER_ONLY,      //!< Use only the serial number (device identifier).
        NAMING_BYUSER,                  //!< Allow the command line user to name the file.
    } eLogFileNamingConvention;

    //! \fn char* generate_Log_Name(eLogFileNamingConvention logFileNamingConvention,
    //!                             const char* deviceIdentifier,
    //!                             size_t deviceIDLen,
    //!                             const char* logPath,
    //!                             size_t logPathLen,
    //!                             const char* logName,
    //!                             size_t logNameLen,
    //!                             const char* logExt,
    //!                             size_t logExtLen)
    //! \brief Generates a log file name based on the specified naming convention and other parameters
    //! \param[in] logFileNamingConvention The naming convention to use. Required
    //! \param[in] deviceIdentifier The device identifier. Required
    //! \param[in] deviceIDLen The length of the device identifier. Required
    //! \param[in] logPath Optional path to output the log file to.
    //! Will be checked for security. If NULL, the current directory will be used.
    //! \param[in] logPathLen The length of the log path. May be 0.
    //! \param[in] logName Optional name of the log file from the drive, FARM, DST, etc.
    //! \param[in] logNameLen The length of the log name. May be 0.
    //! \param[in] logExt Optional extension for the log file. If NULL, set to .bin.
    //! \param[in] logExtLen The length of the log extension. May be 0.
    //! \return Pointer to the generated log file name. Must be free'd by the caller with free()
    char* generate_Log_Name(eLogFileNamingConvention logFileNamingConvention,
                            const char*              deviceIdentifier,
                            size_t                   deviceIDLen,
                            const char*              logPath,
                            size_t                   logPathLen,
                            const char*              logName,
                            size_t                   logNameLen,
                            const char*              logExt,
                            size_t                   logExtLen);

    //! \fn eReturnValues create_And_Open_Secure_Log_File(const char* deviceIdentifier,
    //!                                                   size_t deviceIDLen,
    //!                                                   secureFileInfo** file,
    //!                                                   eLogFileNamingConvention logFileNamingConvention,
    //!                                                   const char* logPath,
    //!                                                   size_t logPathLen,
    //!                                                   const char* logName,
    //!                                                   size_t logNameLen,
    //!                                                   const char* logExt,
    //!                                                   size_t logExtLen)
    //! \brief Generates a log file name based on the serial number, time, and other inputs,
    //! and opens the file for writing
    //! \param[in] deviceIdentifier The device identifier string (Serial number, etc)
    //! \param[in] deviceIDLen The length of the device identifier.
    //! \param[in,out] file Pointer to a secureFileInfo structure that will hold an open file handle upon successful
    //! completion.
    //! \param[in] logFileNamingConvention The naming convention to use.
    //! \param[in] logPath Optional path to output the log file to. Will be checked for security.
    //! If NULL, the current directory will be used.
    //! \param[in] logPathLen The length of the log path. May be 0.
    //! \param[in] logName Optional name of the log file from the drive, FARM, DST, etc.
    //! \param[in] logNameLen The length of the log name. May be 0.
    //! \param[in] logExt Optional extension for the log file. If NULL, set to .bin.
    //! \param[in] logExtLen The length of the log extension. May be 0.
    //! \return SUCCESS if everything worked, !SUCCESS means something went wrong.
    //! \note This function does not return the name used as that is part of the secureFileInfo.
    eReturnValues create_And_Open_Secure_Log_File(const char*              deviceIdentifier,
                                                  size_t                   deviceIDLen,
                                                  secureFileInfo**         file,
                                                  eLogFileNamingConvention logFileNamingConvention,
                                                  const char*              logPath,
                                                  size_t                   logPathLen,
                                                  const char*              logName,
                                                  size_t                   logNameLen,
                                                  const char*              logExt,
                                                  size_t                   logExtLen);

#if defined(__cplusplus)
}
#endif
