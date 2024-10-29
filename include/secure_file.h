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
// recommendations
//

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include <stdio.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#if defined(_WIN32)
// Windows does not supply the macros to test the file, so we are defining them
// ourselves so that we can use them when we want to. NOTE: This is only for the
// mode_t compatibility provided by Windows.
#if !defined(S_ISREG)
#define S_ISREG(m) (m & _S_IFMT) == _S_IFREG
#endif // S_ISREG

#if !defined(S_ISDIR)
#define S_ISDIR(m) (m & _S_IFMT) == _S_IFDIR
#endif // S_ISDIR

#if !defined(S_ISCHR)
#define S_ISCHR(m) (m & _S_IFMT) == _S_IFCHR
#endif // S_ISCHR

#if !defined(S_ISBLK)
#define S_ISBLK(m) (0) /*Windows doesn't have a flag for this*/
#endif                 // S_ISBLK

#if !defined(S_ISFIFO)
#define S_ISFIFO(m) (m & _S_IFMT) == _S_IFIFO
#endif // S_ISFIFO

#if !defined(S_ISLNK)
#define S_ISLNK(m) (0) /*Windows doesn't have a flag for this*/
#endif                 // S_ISLNK

#if !defined(S_ISSOCK)
#define S_ISSOCK(m) (0) /*Windows doesn't have a flag for this*/
#endif                  // S_ISSOCK

#if !defined(S_TYPEISMQ)
#define S_TYPEISMQ(buf) (0) /*Windows doesn't have a flag for this*/
#endif                      // S_TYPEISMQ

#if !defined(S_TYPEISSEM)
#define S_TYPEISSEM(buf) (0) /*Windows doesn't have a flag for this*/
#endif                       // S_TYPEISSEM

#if !defined(S_TYPEISSHM)
#define S_TYPEISSHM(buf) (0) /*Windows doesn't have a flag for this*/
#endif                       // S_TYPEISSHM
#endif                       //_WIN32

#define FILE_UNIQUE_ID_ARR_MAX (16)
    typedef struct sfileUniqueIDInfo
    {
        // Windows uses unsigned long for vol SN
        // Windows uses 2 unsigned longs for index high/low. NOTE: REFS needs a
        // 128bit identifier, need to use ex version to read full identifier Linux
        // has ino_t and dev_t to check for Idea: Both linux and windows recommend
        // combing things to create a unique ID (Windows: Vol SN + file index,
        // Linux: ino_t + dev_t)
        //       use a very generic thing to compare that can hold this many bytes
        //       of data...an array. low-level OS function to do the comparison???
        union {
            uint64_t inode;
            uint64_t volsn;
        };
        union {
            uint64_t deviceid;                       // dev_t from struct stat in posix
            uint8_t  fileid[FILE_UNIQUE_ID_ARR_MAX]; // Windows may use 8 or 16 bytes
                                                     // depending on filesystem. Fat/NTFS
                                                     // can use only 64bits, but REFS
                                                     // needs at least 128 bits to
                                                     // identify this uniquely.
        };
    } fileUniqueIDInfo;

    bool compare_File_Unique_ID(const fileUniqueIDInfo* a, const fileUniqueIDInfo* b);

    M_NODISCARD fileUniqueIDInfo* os_Get_File_Unique_Identifying_Information(FILE* file);

    M_NODISCARD bool os_Is_Directory_Secure(const char* fullpath);

    // Most members of this strucuture match the stat structure. There are some
    // differences which is why we define it without that strucure. Main reason to
    // NOT use struct stat is that Windows has a version, but to get the 64 version
    // would make this a mess to define. So defining it in a way that is best for
    // cross-platform use. Some fields may not be set to anything other than 0 on
    // Windows dues to not have the same concepts as on Unix/Unix-like systems
    typedef struct sfileAttributes
    {
        dev_t    deviceID;
        ino_t    inode;
        mode_t   filemode;
        nlink_t  numberOfLinks;
        uid_t    userID;
        gid_t    groupID;
        dev_t    representedDeviceID;
        offset_t filesize;
        int64_t  fileLastAccessTime;  // milliseconds since Unix epoch (in Windows,
                                      // converted from Windows file epoch to this value)
        int64_t fileModificationTime; // milliseconds since Unix epoch (in Windows,
                                      // converted from Windows file epoch to this
                                      // value)
        int64_t fileStatusChangeTime; // milliseconds since Unix epoch (in Windows,
                                      // converted from Windows file epoch to this
                                      // value)
        // Windows only below here for Windows specific info that may differ from
        // above
        uint32_t fileFlags;
        uint16_t securityControlFlags;
#if defined(_WIN32)
        // Windows user and group SID?
        unsigned long securityDescriptorStringLength;
        char*         winSecurityDescriptor; // Allocated by malloc. Call memset_explicit on
                                             // this to zero it out before freeing.
#endif                                       //_WIN32
    } fileAttributes;
    // DO NOT delete this structure manually. The windows SIDs are allocated and
    // require deletion of their own. this will be cleaned up when calling the
    // function to delete this for you!

    // This can be used on files or directories.
    M_NODISCARD fileAttributes* os_Get_File_Attributes_By_Name(const char* const filetoCheck);
    // This is the preferred method to check attributes are the same once a file has
    // been opened to make sure that the attributes are read from exactly the same
    // file as expected
    M_NODISCARD fileAttributes* os_Get_File_Attributes_By_File(FILE* file);

    void free_File_Attributes(fileAttributes** attributes);

    // For terminating value in this list, set ext to M_NULLPTR
    typedef struct sfileExt
    {
        const char* ext;
        bool        caseInsensitive; // by default to a case sensitive comparison. Only
                                     // change this if you want .bin and .BIN and .bIN and
                                     // .Bin...for example.
    } fileExt;

    M_DECLARE_ENUM(eSecureFileError,
                   SEC_FILE_SUCCESS,
                   SEC_FILE_INVALID_FILE,
                   SEC_FILE_INVALID_PATH,
                   SEC_FILE_FILE_ALREADY_EXISTS, /*attempting to write and create a file that
                                                    already exists*/
                   SEC_FILE_INVALID_FILE_EXTENSION,
                   SEC_FILE_INVALID_FILE_ATTRIBTUES,
                   SEC_FILE_INVALID_FILE_UNIQUE_ID,
                   SEC_FILE_INSECURE_PATH,
                   SEC_FILE_INVALID_MODE,         /*mode string contains invalid options*/
                   SEC_FILE_INVALID_SECURE_FILE,  /*passed in M_NULLPTR secureFileInfo structure
                                                     to a function that requires this.*/
                   SEC_FILE_FAILURE_CLOSING_FILE, /*a failure occured while trying to close the
                                                     file*/
                   SEC_FILE_BUFFER_TOO_SMALL,     /*provided buffer is too small for read/write*/
                   SEC_FILE_INVALID_PARAMETER,    /*this can be returned if a function (like
                                                     fread/fwrite) require a parameter that was
                                                     not provided*/
                   SEC_FILE_READ_WRITE_ERROR,     /*did not read or write as many bytes as
                                                     requested, cannot determine a more specific
                                                     reason*/
                   SEC_FILE_END_OF_FILE_REACHED,  /*reached the end of the file...can be a case
                                                     of success if this is expected*/
                   SEC_FILE_WRITE_DISK_FULL,      /*cannot write any more data due to an error from
                                                     running out of space*/
                   SEC_FILE_SEEK_FAILURE,         /*Cannot seek to the specified offset in the file*/
                   SEC_FILE_FLUSH_FAILURE,
                   SEC_FILE_CANNOT_REMOVE_FILE_STILL_OPEN, /* the ability to call a function
                                                              like POSIX's unlink is not
                                                              possible as there is no such
                                                              function to provide this behavior
                                                            */
                   SEC_FILE_FAILURE                        /*generic undefinable error*/
    );

    typedef struct ssecureFileInfo
    {
        eSecureFileError error;
        bool             isValid;
        FILE*            file;
        const char       fullpath[OPENSEA_PATH_MAX]; // contains the full canonicalized
                                                     // path and filename.
        const char* filename;                        // pointer to just the name section in the fullpath above
        int         fileno;                          // POSIX fileno. Translated using fileno(FILE *file);
        size_t      fileSize;                        // converted from the file attributes file size. May be
                                                     // smaller if size_t cannot represent whole file size. Ex:
                                                     // 4gig file on 32bit OS.
        fileAttributes*   attributes;
        fileUniqueIDInfo* uniqueID;
    } secureFileInfo; // do not free this manually. call the free_Secure_File_Info
                      // function!

    void free_Secure_File_Info(secureFileInfo** fileInfo);

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
                                                 const fileExt*    extList /*optional*/,
                                                 fileAttributes*   expectedFileInfo /*optional*/,
                                                 fileUniqueIDInfo* uniqueIdInfo /*optional*/);

    // M_NODISCARD secureFileInfo* secure_Reopen_File(secureFileInfo* fileInfo);

    // matching close for the open.
    M_NODISCARD eSecureFileError secure_Close_File(secureFileInfo* fileInfo);

    M_DECLARE_ENUM(eSecureFileRename, SEC_RENAME_DO_NOT_REPLACE_EXISTING, SEC_RENAME_REPLACE_EXISTING);

    // NOTE: This will convert the filename into a canonical path internally to
    // ensure a valid path is provided.
    //       Since the low-level APIs need a file name rather than FILE *, this
    //       takes the same kind of input.
    // eSecureFileError secure_Rename_File_By_Name(const char* filename,
    // eSecureFileRename renameInfo);

    M_NODISCARD eSecureFileError secure_Read_File(secureFileInfo* M_RESTRICT fileInfo,
                                                  void* M_RESTRICT           buffer,
                                                  size_t                     buffersize,
                                                  size_t                     elementsize,
                                                  size_t                     count,
                                                  size_t*                    numberread /*optional*/);
    M_NODISCARD eSecureFileError secure_Write_File(secureFileInfo* M_RESTRICT fileInfo,
                                                   void* M_RESTRICT           buffer,
                                                   size_t                     buffersize,
                                                   size_t                     elementsize,
                                                   size_t                     count,
                                                   size_t*                    numberwritten /*optional*/);
    M_NODISCARD eSecureFileError secure_Seek_File(secureFileInfo* fileInfo, offset_t offset, int initialPosition);
    M_NODISCARD eSecureFileError secure_Rewind_File(secureFileInfo* fileInfo);
    M_NODISCARD offset_t         secure_Tell_File(secureFileInfo* fileInfo);

    // This function will unlink the file if it is still open, otherwise it will
    // remove it.
    M_NODISCARD eSecureFileError secure_Remove_File(secureFileInfo* fileInfo);

    M_DECLARE_ENUM(eSecureFileDeleteNameAction, SEC_DELETE_NAME_FAIL_IF_OPEN, SEC_DELETE_NAME_UNLINK_IF_OPEN);

    M_NODISCARD eSecureFileError secure_Delete_File_By_Name(const char*                 filename,
                                                            eSecureFileDeleteNameAction deleteAction);

    eSecureFileError secure_Flush_File(secureFileInfo* fileInfo);

    eSecureFileError secure_GetPos_File(secureFileInfo* M_RESTRICT fileInfo, fpos_t* M_RESTRICT pos);

    eSecureFileError secure_SetPos_File(secureFileInfo* fileInfo, const fpos_t* pos);

    FUNC_ATTR_PRINTF(2, 0)
    eSecureFileError secure_vfprintf_File(secureFileInfo* M_RESTRICT fileInfo,
                                          const char* M_RESTRICT     format,
                                          va_list                    args);

    FUNC_ATTR_PRINTF(2, 3)
    eSecureFileError secure_fprintf_File(secureFileInfo* M_RESTRICT fileInfo, const char* M_RESTRICT format, ...);

    //-----------------------------------------------------------------------------
    //
    // bool os_Directory_Exits (const char * const pathToCheck)
    //
    // \brief   Description: Platform independent helper to check if path exists.
    //                       WARNING: May not work with UNICODE path.
    //
    // Entry:
    //      \param[in] pathToCheck The path that needs to be evaluated.
    //
    // Exit:
    //      \return true if exists, false if it does not.
    //
    //-----------------------------------------------------------------------------
    bool os_Directory_Exists(const char* const pathToCheck);

    //-----------------------------------------------------------------------------
    //
    // bool os_File_Exists (const char * const filetoCheck)
    //
    // \brief   Description: Platform independent helper to check if file exists.
    //                       WARNING: May not work with UNICODE path.
    //
    // Entry:
    //      \param[in] filetoCheck The file that needs to be evaluated.
    //
    // Exit:
    //      \return true if exists, false if it does not.
    //
    //-----------------------------------------------------------------------------
    bool os_File_Exists(const char* const filetoCheck);

    //-----------------------------------------------------------------------------
    //
    //  os_Create_Directory()
    //
    //! \brief   Description:  Creates a new directory in Windows
    //
    //  Entry:
    //!
    //  Exit:
    //!   \return SUCCESS on successful directory creation and FAILURE when
    //!   directory creation fails
    //
    //-----------------------------------------------------------------------------
    eReturnValues os_Create_Directory(const char* filePath);

    // windows and 'nix require a file to use for finding a path as far as I can
    // tell.-TJE
    eReturnValues get_Full_Path(const char* pathAndFile, char fullPath[OPENSEA_PATH_MAX]);

    eReturnValues replace_File_Name_In_Path(char fullPath[OPENSEA_PATH_MAX], char* newFileName);

    //-----------------------------------------------------------------------------
    //
    // int64_t os_Get_File_Size(FILE *filePtr)
    //
    // \brief   Description: returns the size of the file pointer to by the incoming
    // file pointer. This uses GetFileSizeEx in Windows and fstat64 in nix systems.
    //
    // Entry:
    //      \param[in] filePtr - pointer to a file that you wish to get the size of
    //
    // Exit:
    //      \return int64_t file size in bytes. -1 is returned if there is an error.
    //
    //-----------------------------------------------------------------------------
    int64_t os_Get_File_Size(FILE* filePtr);

#if defined(_WIN32)
    bool exact_Compare_SIDS_And_DACL_Strings(const char* sidsAndDACLstr1, const char* sidsAndDACLstr2);
#endif

    typedef enum eLogFileNamingConventionEnum
    {
        NAMING_SERIAL_NUMBER_DATE_TIME, // this should be used most of the time to
                                        // avoid collisions with existing files
        NAMING_SERIAL_NUMBER_ONLY,
        NAMING_BYUSER, // a way for the command line user to name the file
    } eLogFileNamingConvention;

    char* generate_Log_Name(eLogFileNamingConvention logFileNamingConvention, /*required*/
                            const char*              deviceIdentifier,        /*required*/
                            size_t                   deviceIDLen,             /*required*/
                            const char* logPath,    // optional /*requested path to output to. Will be checked for
                                                    // security. If NULL, current directory will be used*/
                            size_t      logPathLen, // may be 0
                            const char* logName,    // optional /*name of the log file from the drive,
                                                    // FARM, DST, etc*/
                            size_t      logNameLen, // may be 0
                            const char* logExt,     // optional /*extension for the log file. If NULL, set to .bin*/
                            size_t      logExtLen   // may be 0
    );

    //-----------------------------------------------------------------------------
    //
    //  create_And_Open_Log_File(tDevice *device, FILE *filePtr, char *logName, char
    //  *logExtension)
    //
    //! \brief   Description: This function will take the inputs given, generate a
    //! file name based off the serial number, time, and other inputs, and open the
    //! file for writing.
    //
    //  Entry:
    //!   \param[in] device = pointer to a valid device structure with a device
    //!   handle (used to get the serial number) \param[in,out] filePtr = File
    //!   pointer that will hold an open file handle upon successful completion
    //!   \param[in] logPath = this is a directory/folder for where the fle should
    //!   be created. M_NULLPTR if current directory. \param[in] logName = this is a
    //!   name to put into the name of the log. Examples: SMART, CEL \param[in]
    //!   logExtension = this is an ASCII representation of the extension to save
    //!   the file with. If unsure, use bin. No DOT required in this parameter
    //!   \param[in] logFileNamingConvention = enum value to specify which log file
    //!   naming convention to use \param[out] logFileNameUsed = sets this pointer
    //!   to the string of the name and extension used for the log file name. This
    //!   must be set to M_NULLPTR when calling. Memory should be OPENSEA_PATH_MAX
    //!   in size
    //!
    //  Exit:
    //!   \return SUCCESS = everything worked, !SUCCESS means something went wrong
    //
    //-----------------------------------------------------------------------------
    eReturnValues create_And_Open_Secure_Log_File(
        const char*              deviceIdentifier,        /*required*/
        size_t                   deviceIDLen,             /*required*/
        secureFileInfo**         file,                    /*required*/
        eLogFileNamingConvention logFileNamingConvention, /*required*/
        const char*              logPath,                 // optional /*requested path to output to. Will be checked for
                                                          // security. If NULL, current directory will be used*/
        size_t      logPathLen,                           // may be 0
        const char* logName,                              // optional /*name of the log file from the drive,
                                                          // FARM, DST, etc*/
        size_t      logNameLen,                           // may be 0
        const char* logExt,   // optional /*extension for the log file. If NULL, set to .bin*/
        size_t      logExtLen // may be 0
        // NOTE: This function does not return the name used as that is part of the
        // secureFileInfo -TJE
    );

#if defined(__cplusplus)
}
#endif
