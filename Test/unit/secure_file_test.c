#include"../framework/test_framework.h"
#include"../../include/secure_file.h"
#include"../testConstants.h"
#include <sys/stat.h>
#include <stdlib.h>

static void test_compare_File_Unique_ID(void) {
    fileUniqueIDInfo a = {0}, b = {0};

    // The inode and deviceid must be equal for the unique IDs to be considered equal.
    a.inode = 100;
    a.deviceid = 200;

    b.inode = 100;
    b.deviceid = 200;

    TEST_ASSERT(compare_File_Unique_ID(&a, &b), "File unique IDs should be equal");

    // Test for different inodes
    b.inode = 101;
    TEST_ASSERT(!compare_File_Unique_ID(&a, &b), "File unique IDs should not be equal when inodes differ");

    // Test for different device IDs
    b.inode = 100;
    b.deviceid = 201;
    TEST_ASSERT(!compare_File_Unique_ID(&a, &b), "File unique IDs should not be equal when device IDs differ");
}

static void test_os_Get_File_Unique_Identifying_Information(void) {
    FILE *f = fopen("test.txt", "w");
    fprintf(f, "hello");
    fclose(f);

    f = fopen("test.txt", "r");

    fileUniqueIDInfo *id1 = os_Get_File_Unique_Identifying_Information(f);
    fileUniqueIDInfo *id2 = os_Get_File_Unique_Identifying_Information(f);

    TEST_ASSERT(compare_File_Unique_ID(id1, id2), "File unique IDs should be equal");

    // Test for different files
    FILE *f2 = fopen("test2.txt", "w");
    fprintf(f2, "hello");
    fclose(f2);

    f2 = fopen("test2.txt", "r");

    fileUniqueIDInfo *id3 = os_Get_File_Unique_Identifying_Information(f2);

    TEST_ASSERT(!compare_File_Unique_ID(id1, id3), "File unique IDs should not be equal for different files");

    fclose(f);
    fclose(f2);
}

static void test_os_Is_Directory_Secure(void) {
    mkdir("secure_dir", 0700);
    char *abs_path = realpath("secure_dir", NULL);

    if (!abs_path) {
        perror("realpath failed");
        return;
    }

    TEST_ASSERT(os_Is_Directory_Secure(abs_path, NULL), "Directory should be secure");

    free(abs_path);
    
    char* error = NULL;
    mkdir("insecure_dir", 0777); // umask removes the permissions for group and others, so we need to explicitly set them to make it insecure.
    chmod("insecure_dir", 0777); // Make the directory writable by others, which should make it insecure.
    char *abs_bad = realpath("insecure_dir", NULL);

    if (!abs_bad) {
        perror("realpath failed");
        return;
    }

    TEST_ASSERT(!os_Is_Directory_Secure(abs_bad, &error), "Directory should not be secure");
    TEST_ASSERT(error != NULL, "Error message should be set for insecure directory");
    free(abs_bad);

    // Test for nested directories
    mkdir("secure_dir/nested_secure", 0700);
    mkdir("secure_dir/nested_secure/inner_secure", 0700);
    char *abs_nested = realpath("secure_dir/nested_secure/inner_secure", NULL);

    if (!abs_nested) {
        perror("realpath failed");
        return;
    }

    TEST_ASSERT(os_Is_Directory_Secure(abs_nested, NULL), "Nested directory should be secure");
    free(abs_nested);

    rmdir("secure_dir/nested_secure/inner_secure");
    rmdir("secure_dir/nested_secure");
    rmdir("secure_dir");
    rmdir("insecure_dir");
}

static void test_free_File_Attributes(void) {
    fileAttributes* attributes = malloc(sizeof(fileAttributes));
    TEST_ASSERT(attributes != NULL, "Memory allocation for fileAttributes successful");

    // Set some dummy values
    attributes->deviceID = 123;
    attributes->inode = 456;
    attributes->filemode = 0644;
    attributes->numberOfLinks = 1;
    attributes->userID = 1000;
    attributes->groupID = 1000;

    free_File_Attributes(&attributes);
    TEST_ASSERT(attributes == NULL, "fileAttributes pointer should be set to NULL after freeing");
}

static void test_os_Get_File_Attributes_By_Name(void) {
    FILE* f = fopen("test_attributes.txt", "w");
    fprintf(f, "hello");
    fclose(f);

    fileAttributes* attributes = os_Get_File_Attributes_By_Name("test_attributes.txt");
    TEST_ASSERT(attributes != NULL, "Should retrieve file attributes successfully");
    TEST_ASSERT(attributes->filesize == 5, "File size should be 5 bytes");

    free_File_Attributes(&attributes);
}

static void test_os_Get_File_Attributes_By_File(void) {
    FILE* f = fopen("test_attributes_file.txt", "w");
    fprintf(f, "hello");
    fclose(f);

    f = fopen("test_attributes_file.txt", "r");
    fileAttributes* attributes = os_Get_File_Attributes_By_File(f);
    TEST_ASSERT(attributes != NULL, "Should retrieve file attributes successfully");
    TEST_ASSERT(attributes->filesize == 5, "File size should be 5 bytes");

    free_File_Attributes(&attributes);
    fclose(f);
}

static void test_free_Secure_File_Info(void) {
    secureFileInfo* fileInfo = malloc(sizeof(secureFileInfo));
    TEST_ASSERT(fileInfo != NULL, "Memory allocation for secureFileInfo successful");
    fileUniqueIDInfo* uniqueID = malloc(sizeof(fileUniqueIDInfo));
    TEST_ASSERT(uniqueID != NULL, "uniqueID allocation successful");

    uniqueID->inode = 123;
    uniqueID->deviceid = 456;

    // Set some dummy values
    fileInfo->error = SEC_FILE_SUCCESS;
    fileInfo->isValid = true;
    fileInfo->file = NULL;
    fileInfo->filename = NULL;
    fileInfo->uniqueID = uniqueID;
    fileInfo->fileno = 0;
    fileInfo->fileSize = 0;
    fileInfo->attributes = NULL;
    fileInfo->errorString = NULL;

    free_Secure_File_Info(&fileInfo);
    TEST_ASSERT(fileInfo == NULL, "secureFileInfo pointer should be set to NULL after freeing");
}

static void test_secure_Open_File(void) {
    const char* filename = "test_secure_open.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello");
    fclose(f);
    fileExt extList[] = {{"txt"}, {"log"}, {NULL}};

    secureFileInfo* fileInfo = secure_Open_File(filename, "r", extList, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");
    TEST_ASSERT(fileInfo->file != NULL, "secure_Open_File should have a valid FILE pointer");
    TEST_ASSERT(fileInfo->fileSize == 5, "File size should be 5 bytes");
    free_Secure_File_Info(&fileInfo);

    // Test for invalid mode
    fileInfo = secure_Open_File(filename, "x", extList, NULL, NULL);
    TEST_ASSERT(fileInfo->error == SEC_FILE_INVALID_MODE, "Should return invalid mode error for unsupported mode");
    free_Secure_File_Info(&fileInfo);

    // Test when filename is NULL
    fileInfo = secure_Open_File(NULL, "r", extList, NULL, NULL);
    TEST_ASSERT(fileInfo->error == SEC_FILE_FAILURE, "Should return failure when filename is NULL");
    free_Secure_File_Info(&fileInfo);
}

static void test_secure_Close_File(void) {
    const char* filename = "test_secure_close.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello");
    fclose(f);

    secureFileInfo* fileInfo = secure_Open_File(filename, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    // Test when fileInfo = NULL
    eSecureFileError closeResult = secure_Close_File(NULL);
    TEST_ASSERT(closeResult == SEC_FILE_INVALID_SECURE_FILE, "secure_Close_File should return invalid secure file error when fileInfo is NULL");

    closeResult = secure_Close_File(fileInfo);
    TEST_ASSERT(closeResult == SEC_FILE_SUCCESS, "secure_Close_File should succeed");

    // Change fileInfo state to simulate failure closing file
    // fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    // closeResult = secure_Close_File(fileInfo);
    // TEST_ASSERT(closeResult == SEC_FILE_FAILURE_CLOSING_FILE, "secure_Close_File should return failure closing file error if fileInfo is in that state");

    free_Secure_File_Info(&fileInfo);
}

static void test_secure_Read_File(void) {
    const char* filename = "test_secure_read.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello");
    fclose(f);

    secureFileInfo* fileInfo = secure_Open_File(filename, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    char buffer[10] = {0};
    eSecureFileError readResult = secure_Read_File(fileInfo, buffer, sizeof(buffer), 1, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_SUCCESS, "secure_Read_File should succeed");
    TEST_ASSERT(strcmp(buffer, "hello") == 0, "Buffer should contain 'hello'");

    // Test when buffer = NULL
    readResult = secure_Read_File(fileInfo, NULL, sizeof(buffer), 1, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_INVALID_PARAMETER, "Should return invalid parameter error when buffer is NULL");
    TEST_ASSERT(fileInfo->error == SEC_FILE_INVALID_PARAMETER, "fileInfo error should be set to invalid parameter when buffer is NULL");

    // Test when buffersize < elementsize * count
    char smallBuffer[2] = {0};
    readResult = secure_Read_File(fileInfo, smallBuffer, sizeof(smallBuffer), 1, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_BUFFER_TOO_SMALL, "Should return buffer too small error when buffersize is insufficient");
    TEST_ASSERT(fileInfo->error == SEC_FILE_BUFFER_TOO_SMALL, "fileInfo error should be set to buffer too small when buffersize is insufficient");

    // Test when numberread is provided
    size_t numberRead = 0;
    secure_Seek_File(fileInfo, 0, SEEK_SET);
    readResult = secure_Read_File(fileInfo, buffer, sizeof(buffer), 1, 5, &numberRead);
    TEST_ASSERT(readResult == SEC_FILE_SUCCESS, "secure_Read_File should succeed when numberread is provided");
    TEST_ASSERT(numberRead == 5, "numberread should be set to the number of elements read");

    // Test when count is 0
    readResult = secure_Read_File(fileInfo, buffer, sizeof(buffer), 1, 0, NULL);
    TEST_ASSERT(readResult == SEC_FILE_SUCCESS, "secure_Read_File should succeed when count is 0");

    // Test when elementsize is 0
    readResult = secure_Read_File(fileInfo, buffer, sizeof(buffer), 0, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_SUCCESS, "secure_Read_File should succeed when elementsize is 0");

    // Test when readResult is less than count (partial read)
    numberRead = 0;
    secure_Seek_File(fileInfo, 0, SEEK_SET);
    readResult = secure_Read_File(fileInfo, buffer, sizeof(buffer), 1, 6, &numberRead);
    TEST_ASSERT(readResult == SEC_FILE_END_OF_FILE_REACHED, "secure_Read_File should return end of file error when reading past the end of the file");

    fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    readResult = secure_Read_File(fileInfo, buffer, sizeof(buffer), 1, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_FAILURE_CLOSING_FILE, "secure_Read_File should return failure closing file error if fileInfo is in that state");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    // Test for an empty file
    const char* emptyFilename = "test_secure_read_empty.txt";
    f = fopen(emptyFilename, "w");
    fclose(f);
    secureFileInfo* emptyFileInfo = secure_Open_File(emptyFilename, "r", NULL, NULL, NULL);
    readResult = secure_Read_File(emptyFileInfo, buffer, sizeof(buffer), 1, 5, &numberRead);
    TEST_ASSERT(readResult == SEC_FILE_READ_WRITE_ERROR, "secure_Read_File should return read/write error when reading from an empty file");
    secure_Close_File(emptyFileInfo);
    free_Secure_File_Info(&emptyFileInfo);

    // Test when reading from a write-only file
    secureFileInfo* writeOnlyInfo = secure_Open_File(filename, "w", NULL, NULL, NULL);
    readResult = secure_Read_File(writeOnlyInfo, buffer, sizeof(buffer), 1, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_READ_WRITE_ERROR, "secure_Read_File should return read/write error when reading from a write-only file");
    secure_Close_File(writeOnlyInfo);
    free_Secure_File_Info(&writeOnlyInfo);

    // Test when fileInfo = NULL
    readResult = secure_Read_File(NULL, buffer, sizeof(buffer), 1, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_INVALID_SECURE_FILE, "secure_Read_File should return invalid secure file error when fileInfo is NULL");
}

static void test_secure_Write_File(void) {
    const char* filename = "test_secure_write.txt";
    secureFileInfo* fileInfo = secure_Open_File(filename, "w", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    const char* data = "hello";
    // Test when fileInfo = NULL
    eSecureFileError writeResult = secure_Write_File(NULL, (void*)data, strlen(data), 1, strlen(data), NULL);
    TEST_ASSERT(writeResult == SEC_FILE_INVALID_SECURE_FILE, "secure_Write_File should return invalid secure file error when fileInfo is NULL");

    // Test when buffer = NULL
    writeResult = secure_Write_File(fileInfo, NULL, strlen(data), 1, strlen(data), NULL);
    TEST_ASSERT(writeResult == SEC_FILE_INVALID_PARAMETER, "secure_Write_File should return invalid parameter error when buffer is NULL");

    // Test when buffersize < elementsize * count
    writeResult = secure_Write_File(fileInfo, (void*)data, 2, 1, strlen(data), NULL);
    TEST_ASSERT(writeResult == SEC_FILE_BUFFER_TOO_SMALL, "secure_Write_File should return buffer too small error when buffersize is insufficient");

    // Test when numberwritten is provided
    size_t numberWritten = 0;
    writeResult = secure_Write_File(fileInfo, (void*)data, strlen(data), 1, strlen(data), &numberWritten);
    TEST_ASSERT(writeResult == SEC_FILE_SUCCESS, "secure_Write_File should succeed when numberwritten is provided");
    TEST_ASSERT(numberWritten == strlen(data), "numberwritten should be set to the number of elements written");

    // Test when count is 0
    writeResult = secure_Write_File(fileInfo, (void*)data, strlen(data), 1, 0, NULL);
    TEST_ASSERT(writeResult == SEC_FILE_SUCCESS, "secure_Write_File should succeed when count is 0");

    // Test when elementsize is 0
    writeResult = secure_Write_File(fileInfo, (void*)data, strlen(data), 0, strlen(data), NULL);
    TEST_ASSERT(writeResult == SEC_FILE_SUCCESS, "secure_Write_File should succeed when elementsize is 0");

    fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    writeResult = secure_Write_File(fileInfo, (void*)data, strlen(data), 1, strlen(data), NULL);
    TEST_ASSERT(writeResult == SEC_FILE_FAILURE_CLOSING_FILE, "secure_Write_File should return failure closing file error if fileInfo is in that state");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    // Test for writing to a read-only file
    secureFileInfo* readOnlyInfo = secure_Open_File(filename, "r", NULL, NULL, NULL);
    writeResult = secure_Write_File(readOnlyInfo, (void*)data, strlen(data), 1, strlen(data), NULL);
    TEST_ASSERT(writeResult == SEC_FILE_READ_WRITE_ERROR, "secure_Write_File should return read/write error when writing to a read-only file");
    secure_Close_File(readOnlyInfo);
    free_Secure_File_Info(&readOnlyInfo);

    // Test for writing to a file and then reading it back to verify the contents
    const char* filename2 = "test_secure_write2.txt";
    fileInfo = secure_Open_File(filename2, "w", NULL, NULL, NULL);

    writeResult = secure_Write_File(fileInfo, (void*)data, strlen(data), 1, strlen(data), NULL);
    TEST_ASSERT(writeResult == SEC_FILE_SUCCESS, "secure_Write_File should succeed");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    // Verify the file was written correctly
    FILE* f = fopen(filename2, "r");
    char buffer[10] = {0};
    fread(buffer, 1, sizeof(buffer), f);
    fclose(f);
    TEST_ASSERT(strcmp(buffer, "hello") == 0, "File should contain 'hello'");
}

static void test_secure_Seek_File(void) {
    const char* filename = "test_secure_seek.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello world");
    fclose(f);

    secureFileInfo* fileInfo = secure_Open_File(filename, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    // Test when fileInfo = NULL
    eSecureFileError result = secure_Seek_File(NULL, 0, SEEK_SET);
    TEST_ASSERT(result == SEC_FILE_INVALID_SECURE_FILE, "secure_Seek_File should return invalid secure file error when fileInfo is NULL");

    // Test when initialPosition is invalid
    result = secure_Seek_File(fileInfo, 6, -1);
    TEST_ASSERT(result == SEC_FILE_SEEK_FAILURE, "secure_Seek_File should return seek failure error when initialPosition is invalid");

    // Move the file position to 6 (after "hello ")
    long pos = 6;
    result = secure_Seek_File(fileInfo, pos, SEEK_SET);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Seek_File should succeed");

    // Read the next 5 characters ("world")
    char buffer[10] = {0};
    eSecureFileError readResult = secure_Read_File(fileInfo, buffer, sizeof(buffer), 1, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_SUCCESS, "secure_Read_File should succeed");
    TEST_ASSERT(strcmp(buffer, "world") == 0, "Buffer should contain 'world'");

    fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    result = secure_Seek_File(fileInfo, 0, SEEK_SET);
    TEST_ASSERT(result == SEC_FILE_FAILURE_CLOSING_FILE, "secure_Seek_File should return failure closing file error if fileInfo is in that state");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);
}

static void test_secure_Rewind_File(void) {
    const char* filename = "test_secure_rewind.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello world");
    fclose(f);

    secureFileInfo* fileInfo = secure_Open_File(filename, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    // Move the file position to 6 (after "hello ")
    long pos = 6;
    eSecureFileError result = secure_Seek_File(fileInfo, pos, SEEK_SET);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Seek_File should succeed");

    // Test when fileInfo = NULL
    result = secure_Rewind_File(NULL);
    TEST_ASSERT(result == SEC_FILE_INVALID_SECURE_FILE, "secure_Rewind_File should return invalid secure file error when fileInfo is NULL");

    // Rewind the file back to the beginning
    eSecureFileError rewindResult = secure_Rewind_File(fileInfo);
    TEST_ASSERT(rewindResult == SEC_FILE_SUCCESS, "secure_Rewind_File should succeed");

    // Read the first 5 characters ("hello")
    char buffer[10] = {0};
    eSecureFileError readResult = secure_Read_File(fileInfo, buffer, sizeof(buffer), 1, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_SUCCESS, "secure_Read_File should succeed");
    TEST_ASSERT(strcmp(buffer, "hello") == 0, "Buffer should contain 'hello'");

    // Test when file is in failure closing state
    fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    result = secure_Rewind_File(fileInfo);
    TEST_ASSERT(result == SEC_FILE_FAILURE_CLOSING_FILE, "secure_Rewind_File should return failure closing file error if file is in that state");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);
}

static void test_secure_Tell_File(void) {
    const char* filename = "test_secure_tell.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello world");
    fclose(f);

    secureFileInfo* fileInfo = secure_Open_File(filename, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    // Test when fileInfo = NULL
    oscoffset_t tellResult = secure_Tell_File(NULL);
    TEST_ASSERT(tellResult == -1, "secure_Tell_File should return -1 when fileInfo is NULL");

    // Move the file position to 6 (after "hello ")
    long pos = 6;
    eSecureFileError result = secure_Seek_File(fileInfo, pos, SEEK_SET);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Seek_File should succeed");
    oscoffset_t currentPos = secure_Tell_File(fileInfo);
    TEST_ASSERT(currentPos == pos, "secure_Tell_File should return the correct file position");

    fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    tellResult = secure_Tell_File(fileInfo);
    TEST_ASSERT(tellResult == -1, "secure_Tell_File should return -1 when fileInfo is in failure closing state");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    const char* filename2 = "test_secure_tell2.txt";
    FILE* f2 = fopen(filename2, "w");
    fprintf(f2, "hello world");
    fclose(f2);

    secureFileInfo* fileInfo2 = secure_Open_File(filename2, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo2 != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo2->isValid, "secure_Open_File should return valid file info");

    // Test when FILE pointer is NULL
    fileInfo2->file = NULL;
    tellResult = secure_Tell_File(fileInfo2);
    TEST_ASSERT(tellResult == -1, "secure_Tell_File should return -1 when FILE pointer is NULL");

    secure_Close_File(fileInfo2);
    free_Secure_File_Info(&fileInfo2);
}

static void test_secure_Remove_File(void) {
    const char* filename = "test_secure_remove.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello world");
    fclose(f);

    secureFileInfo* fileInfo = secure_Open_File(filename, "r+", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    secure_Close_File(fileInfo);

    eSecureFileError result = secure_Remove_File(fileInfo);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Remove_File should succeed");
    TEST_ASSERT(!os_File_Exists(filename), "File should be removed");
    free_Secure_File_Info(&fileInfo);
}

static void test_secure_Delete_File_By_Name(void) {
    const char* filename = "test_secure_delete_by_name1.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello world");
    fclose(f);

    // Test when filename = NULL
    eSecureFileError result = secure_Delete_File_By_Name(NULL, SEC_DELETE_NAME_FAIL_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_INVALID_PATH, "secure_Delete_File_By_Name should return invalid parameter error when filename is NULL");

    // If file is open, SEC_DELETE_NAME_UNLINK_IF_OPEN should succeed by unlinking the file
    f = fopen(filename, "r");
    result = secure_Delete_File_By_Name(filename, SEC_DELETE_NAME_UNLINK_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Delete_File_By_Name should succeed with unlink if file is open");
    TEST_ASSERT(!os_File_Exists(filename), "File should be unlinked and not exist");
    fclose(f);

    // Create the file again for the next test
    f = fopen(filename, "w");
    fprintf(f, "hello world");
    fclose(f);

    // Test when path is invalid
    const char* badPath = "invalid_path_###";
    result = secure_Delete_File_By_Name(badPath, SEC_DELETE_NAME_FAIL_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_INVALID_PATH, "Should fail when full path resolution fails");

    result = secure_Delete_File_By_Name(filename, SEC_DELETE_NAME_FAIL_IF_OPEN);
    printf("Result of deleting non-existent file: %d\n", result);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Delete_File_By_Name should succeed");
    TEST_ASSERT(!os_File_Exists(filename), "File should be removed");

    // Test for a not secure directory
    system("mkdir insecure_dir");
    system("chmod 777 insecure_dir");

    const char* filename2 = "insecure_dir/test.txt";
    FILE* f2 = fopen(filename2, "w");
    fprintf(f2, "Hello world");
    fclose(f2);

    result = secure_Delete_File_By_Name(filename2, SEC_DELETE_NAME_FAIL_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_INSECURE_PATH, "Should fail when parent directory is not secure");

    remove(filename2);
    rmdir("insecure_dir");

    // Test when the directory does not have write permissions
    system("mkdir secure_dir");

    const char* filename3 = "secure_dir/test_secure_delete_by_name3.txt";
    FILE* f3 = fopen(filename3, "w");
    fprintf(f3, "hello world");
    fclose(f3);

    system("chmod 555 secure_dir");

    result = secure_Delete_File_By_Name(filename3, SEC_DELETE_NAME_FAIL_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_FAILURE, "Should return failure when remove fails");

    // Test when read permissions are removed from the file
    system("chmod 000 secure_dir/test_secure_delete_by_name3.txt");
    result = secure_Delete_File_By_Name(filename3, SEC_DELETE_NAME_UNLINK_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_FAILURE, "Should return failure when unlink also fails due to permissions");

    system("chmod 755 secure_dir");

    result = secure_Delete_File_By_Name(filename3, SEC_DELETE_NAME_FAIL_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_CANNOT_REMOVE_FILE_STILL_OPEN, "Should return cannot remove file still open when remove fails due to permissions");

    result = secure_Delete_File_By_Name(filename3, SEC_DELETE_NAME_UNLINK_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "Should succeed with unlink if remove fails due to permissions");

    remove(filename3);
    rmdir("secure_dir");
}

static void test_secure_Flush_File(void) {
    const char* filename = "test_secure_flush.txt";
    secureFileInfo* fileInfo = secure_Open_File(filename, "w", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    const char* data = "hello";
    eSecureFileError writeResult = secure_Write_File(fileInfo, (void*)data, strlen(data), 1, strlen(data), NULL);
    TEST_ASSERT(writeResult == SEC_FILE_SUCCESS, "secure_Write_File should succeed");

    // Test when fileInfo = NULL
    eSecureFileError flushResult = secure_Flush_File(NULL);
    TEST_ASSERT(flushResult == SEC_FILE_INVALID_SECURE_FILE, "secure_Flush_File should return invalid secure file error when fileInfo is NULL");

    flushResult = secure_Flush_File(fileInfo);
    TEST_ASSERT(flushResult == SEC_FILE_SUCCESS, "secure_Flush_File should succeed");

    fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    flushResult = secure_Flush_File(fileInfo);
    TEST_ASSERT(flushResult == SEC_FILE_FAILURE_CLOSING_FILE, "secure_Flush_File should return failure closing file error if fileInfo is in that state");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    // Verify the file was written correctly
    FILE* f = fopen(filename, "r");
    char buffer[10] = {0};
    fread(buffer, 1, sizeof(buffer), f);
    fclose(f);
    TEST_ASSERT(strcmp(buffer, "hello") == 0, "File should contain 'hello'");
}

static void test_secure_GetPos_File(void) {
    const char* filename = "test_secure_getpos.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello world");
    fclose(f);

    secureFileInfo* fileInfo = secure_Open_File(filename, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    fpos_t pos;
    eSecureFileError result = secure_GetPos_File(fileInfo, &pos);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_GetPos_File should succeed");
    long current = ftell(fileInfo->file);
    TEST_ASSERT(current == 0, "Initial file position should be 0");

    // Move the file position to 6 (after "hello ")
    long offset = 6;
    result = secure_Seek_File(fileInfo, offset, SEEK_SET);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Seek_File should succeed");

    result = secure_GetPos_File(fileInfo, &pos);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_GetPos_File should succeed");
    current = ftell(fileInfo->file);
    TEST_ASSERT(current == offset, "File position should be updated to the offset");

    // Test when pos == NULL
    result = secure_GetPos_File(fileInfo, NULL);
    TEST_ASSERT(result == SEC_FILE_INVALID_PARAMETER, "secure_GetPos_File should return invalid parameter error when pos is NULL");
    TEST_ASSERT(fileInfo->error == SEC_FILE_INVALID_PARAMETER, "fileInfo error should be set to invalid parameter when pos is NULL");

    // Test when fileInfo = NULL
    result = secure_GetPos_File(NULL, &pos);
    TEST_ASSERT(result == SEC_FILE_INVALID_SECURE_FILE, "secure_GetPos_File should return invalid secure file error when fileInfo is NULL");

    fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    result = secure_GetPos_File(fileInfo, &pos);
    TEST_ASSERT(result == SEC_FILE_FAILURE_CLOSING_FILE, "secure_GetPos_File should return failure closing file error if fileInfo is in that state");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    const char* filename1 = "test_secure_getpos1.txt";
    FILE* f1 = fopen(filename1, "w");
    fprintf(f1, "hello world");
    fclose(f1);

    secureFileInfo* fileInfo1 = secure_Open_File(filename1, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo1 != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo1->isValid, "secure_Open_File should return valid file info");

    fclose(fileInfo1->file);

    // Test when file is closed
    result = secure_GetPos_File(fileInfo1, &pos);
    TEST_ASSERT(result == SEC_FILE_FAILURE, "secure_GetPos_File should return failure when file is closed");
    free_Secure_File_Info(&fileInfo1);
}

static void test_secure_SetPos_File(void) {
    const char* filename = "test_secure_setpos.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello world");
    fclose(f);

    secureFileInfo* fileInfo = secure_Open_File(filename, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    // Move to position 6
    long offset = 6;
    eSecureFileError result = secure_Seek_File(fileInfo, offset, SEEK_SET);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Seek_File should succeed");

    fpos_t pos;
    result = secure_GetPos_File(fileInfo, &pos);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_GetPos_File should succeed");

    result = secure_Seek_File(fileInfo, 0, SEEK_SET);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Seek_File should succeed");

    long current = ftell(fileInfo->file);
    TEST_ASSERT(current == 0, "Position should now be 0");

    // Restore position using fpos_t
    result = secure_SetPos_File(fileInfo, &pos);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_SetPos_File should succeed");
    current = ftell(fileInfo->file);
    TEST_ASSERT(current == offset, "File position should be restored to 6");

    // Test when pos == NULL
    result = secure_SetPos_File(fileInfo, NULL);
    TEST_ASSERT(result == SEC_FILE_INVALID_PARAMETER, "secure_SetPos_File should return invalid parameter error when pos is NULL");
    TEST_ASSERT(fileInfo->error == SEC_FILE_INVALID_PARAMETER, "fileInfo error should be set to invalid parameter when pos is NULL");

    // Test when fileInfo = NULL
    result = secure_SetPos_File(NULL, &pos);
    TEST_ASSERT(result == SEC_FILE_INVALID_SECURE_FILE, "secure_SetPos_File should return invalid secure file error when fileInfo is NULL");

    fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    result = secure_SetPos_File(fileInfo, &pos);
    TEST_ASSERT(result == SEC_FILE_FAILURE_CLOSING_FILE, "secure_SetPos_File should return failure closing file error if fileInfo is in that state");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    const char* filename1 = "test_secure_setpos1.txt";
    FILE* f1 = fopen(filename1, "w");
    fprintf(f1, "hello world");
    fclose(f1);

    secureFileInfo* fileInfo1 = secure_Open_File(filename1, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo1 != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo1->isValid, "secure_Open_File should return valid file info");

    fclose(fileInfo1->file);

    // Test when file is closed
    result = secure_SetPos_File(fileInfo1, &pos);
    TEST_ASSERT(result == SEC_FILE_FAILURE, "secure_SetPos_File should return failure when file is closed");
    free_Secure_File_Info(&fileInfo1);
}

static eSecureFileError test_wrapper_vfprintf(secureFileInfo* fileInfo, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    eSecureFileError result = secure_vfprintf_File(fileInfo, fmt, args);

    va_end(args);
    return result;
}

static void test_secure_vfprintf_File(void) {
    const char* filename = "test_secure_vfprintf.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello");
    fclose(f);

    secureFileInfo* fileInfo = secure_Open_File(filename, "w", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    eSecureFileError result = test_wrapper_vfprintf(fileInfo, "Hello %s!", "world");
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_vfprintf_File should succeed");

   // Test for incomplete format
    result = test_wrapper_vfprintf(fileInfo, "Hello %");
    TEST_ASSERT(result == SEC_FILE_READ_WRITE_ERROR, "secure_vfprintf_File should return read/write error for invalid format string");
    printf("Result of invalid format string: %d\n", result);

    // Test when fileInfo = NULL
    result = test_wrapper_vfprintf(NULL, "This should fail");
    TEST_ASSERT(result == SEC_FILE_INVALID_SECURE_FILE, "secure_vfprintf_File should return invalid secure file error when fileInfo is NULL");

    // Test when format = NULL
    result = test_wrapper_vfprintf(fileInfo, NULL);
    TEST_ASSERT(result == SEC_FILE_INVALID_PARAMETER, "secure_vfprintf_File should return invalid parameter error when format is NULL");
    TEST_ASSERT(fileInfo->error == SEC_FILE_INVALID_PARAMETER, "fileInfo error should be set to invalid parameter when format is NULL");

    fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    result = test_wrapper_vfprintf(fileInfo, "This should fail");
    TEST_ASSERT(result == SEC_FILE_FAILURE_CLOSING_FILE, "secure_vfprintf_File should return failure closing file error if fileInfo is in that state");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    // Verify the file was written correctly
    f = fopen(filename, "r");
    char buffer[20] = {0};
    fread(buffer, 1, sizeof(buffer), f);
    fclose(f);
    TEST_ASSERT(strcmp(buffer, "Hello world!") == 0, "File should contain 'Hello world!'");
}

static void test_secure_fprintf_File(void) {
    const char* filename = "test_secure_fprintf.txt";
    secureFileInfo* fileInfo = secure_Open_File(filename, "w", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    eSecureFileError result = secure_fprintf_File(fileInfo, "Hello %s!", "world");
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_fprintf_File should succeed");

    // Test when format = NULL
    result = secure_fprintf_File(fileInfo, NULL);
    TEST_ASSERT(result == SEC_FILE_INVALID_PARAMETER, "secure_fprintf_File should return invalid parameter error when format is NULL");
    TEST_ASSERT(fileInfo->error == SEC_FILE_INVALID_PARAMETER, "fileInfo error should be set to invalid parameter when format is NULL");

    // Test when fileInfo = NULL
    result = secure_fprintf_File(NULL, "This should fail");
    TEST_ASSERT(result == SEC_FILE_INVALID_SECURE_FILE, "secure_fprintf_File should return invalid secure file error when fileInfo is NULL");

    fileInfo->error = SEC_FILE_FAILURE_CLOSING_FILE;
    result = secure_fprintf_File(fileInfo, "This should fail");
    TEST_ASSERT(result == SEC_FILE_FAILURE_CLOSING_FILE, "secure_fprintf_File should return failure closing file error if fileInfo is in that state");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    // Verify the file was written correctly
    FILE* f = fopen(filename, "r");
    char buffer[20] = {0};
    fread(buffer, 1, sizeof(buffer), f);
    fclose(f);
    TEST_ASSERT(strcmp(buffer, "Hello world!") == 0, "File should contain 'Hello world!'");
}

static void test_os_Directory_Exists(void) {
    mkdir("test_dir_exists", 0700);
    char* abs_path = realpath("test_dir_exists", NULL);
    TEST_ASSERT(os_Directory_Exists(abs_path), "Directory should exist");
    rmdir("test_dir_exists");
    TEST_ASSERT(!os_Directory_Exists(abs_path), "Directory should not exist");
    free(abs_path);
}

static void test_os_File_Exists(void) {
    const char* filename = "test_file_exists.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello");
    fclose(f);

    TEST_ASSERT(os_File_Exists(filename), "File should exist");

    remove(filename);
    TEST_ASSERT(!os_File_Exists(filename), "File should not exist");
}

static void test_os_Create_Directory(void) {
    const char* dirname = "test_create_directory";
    eSecureFileError result = os_Create_Directory(dirname);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "os_Create_Directory should succeed");
    TEST_ASSERT(os_Directory_Exists(dirname), "Directory should exist after creation");

    rmdir(dirname);
}

static void test_os_Create_Secure_Directory(void) {
    const char* dirname = "test_create_secure_directory";
    eSecureFileError result = os_Create_Secure_Directory(dirname);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "os_Create_Secure_Directory should succeed");
    TEST_ASSERT(os_Directory_Exists(dirname), "Directory should exist after creation");

    // Verify the directory is secure
    char* abs_path = realpath(dirname, NULL);
    TEST_ASSERT(os_Is_Directory_Secure(abs_path, NULL), "Directory should be secure");
    free(abs_path);

    rmdir(dirname);
}

static void test_get_Full_Path(void) {
    const char* filename = "test_full_path.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello");
    fclose(f);

    char fullpath[260] = {0};
    char* abs_path = realpath(filename, NULL);
    TEST_ASSERT(abs_path != NULL, "realpath should succeed");
    eReturnValues result = get_Full_Path(abs_path, fullpath);
    TEST_ASSERT(result == SUCCESS, "get_Full_Path should succeed");
    TEST_ASSERT(strlen(fullpath) > 0, "Full path should not be empty");
    free(abs_path);

    remove(filename);
}

static void test_replace_File_Name_In_Path(void) {
    const char* filename = "test_replace_filename.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello");
    fclose(f);

    // When fullpath is invalid, replace_File_Name_In_Path should fail
    eReturnValues result = replace_File_Name_In_Path("testfile.txt", "newfile.txt");
    TEST_ASSERT(result == FAILURE, "replace_File_Name_In_Path should fail with invalid fullpath");

    char fullpath[260] = {0};
    char* abs_path = realpath(filename, NULL);
    TEST_ASSERT(abs_path != NULL, "realpath should succeed");
    result = get_Full_Path(abs_path, fullpath);
    TEST_ASSERT(result == SUCCESS, "get_Full_Path should succeed");

    char newPath[260] = {0};
    result = replace_File_Name_In_Path(fullpath, "newfile.txt");
    TEST_ASSERT(result == SUCCESS, "replace_File_Name_In_Path should succeed");
    TEST_ASSERT(strstr(fullpath, "newfile.txt") != NULL, "New path should contain the new filename");

    free(abs_path);
    remove(filename);
}

static void test_os_Get_File_Size(void) {
    FILE* f = fopen("test_file_size.txt", "w");
    fprintf(f, "hello world");
    fclose(f);

    // Test when file pointer is valid
    size_t fileSize = os_Get_File_Size(f);
    TEST_ASSERT(fileSize == (size_t)-1, "File size should be (size_t)-1 when file pointer is invalid");

    // Reopen the file to get a valid file pointer
    f = fopen("test_file_size.txt", "r");
    fileSize = os_Get_File_Size(f);
    TEST_ASSERT(fileSize == 11, "File size should be 11 bytes");
    fclose(f);
    remove("test_file_size.txt");
}

#if defined(_WIN32)
static void test_exact_Compare_SIDS_And_DACL_Strings(void) {
    const char* sidAndDACL1 = "S-1-5-21-3623811015-3361044348-30300820-1013:D:(A;;FA;;;S-1-1-0)";
    const char* sidAndDACL2 = "S-1-5-21-3623811015-3361044348-30300820-1013:D:(A;;FA;;;S-1-1-0)";
    const char* sidAndDACL3 = "S-1-5-21-3623811015-3361044348-30300820-1014:D:(A;;FA;;;S-1-1-0)";

    TEST_ASSERT(exact_Compare_SIDS_And_DACL_Strings(sidAndDACL1, sidAndDACL2), "SID and DACL strings should be considered equal");
    TEST_ASSERT(!exact_Compare_SIDS_And_DACL_Strings(sidAndDACL1, sidAndDACL3), "SID and DACL strings should not be considered equal");
}
#endif

static void test_generate_Log_Name(void) {
    FILE* f = fopen("test_log_name.txt", "w");
    fprintf(f, "hello");
    fclose(f);

    char* logName = generate_Log_Name(NAMING_SERIAL_NUMBER_DATE_TIME, "device123", strlen("device123"), NULL, 0, "logfile", strlen("logfile"), "txt", strlen("txt"));
    TEST_ASSERT(logName != NULL, "generate_Log_Name should return a valid pointer");
    TEST_ASSERT(strstr(logName, "device123") != NULL, "Log name should contain the device identifier");
    TEST_ASSERT(strstr(logName, "logfile") != NULL, "Log name should contain the log name");
    TEST_ASSERT(strstr(logName, "txt") != NULL, "Log name should contain the log extension");
    free(logName);

    // Naming convention with serial number only
    logName = generate_Log_Name(NAMING_SERIAL_NUMBER_ONLY, "device123", strlen("device123"), NULL, 0, "logfile", strlen("logfile"), ".txt", strlen(".txt"));
    TEST_ASSERT(logName != NULL, "generate_Log_Name should return a valid pointer");
    TEST_ASSERT(strstr(logName, "device123") != NULL, "Log name should contain the device identifier");
    free(logName);

    // Naming convention = NAMING_BYUSER
    // logName = generate_Log_Name(NAMING_BYUSER, "device123", strlen("device123"), NULL, 0, "logfile", strlen("logfile"), "txt", strlen("txt"));
    // TEST_ASSERT(logName != NULL, "generate_Log_Name should return a valid pointer");
    // free(logName);

    // Test when logName is NULL
    logName = generate_Log_Name(NAMING_SERIAL_NUMBER_DATE_TIME, "device123", strlen("device123"), NULL, 0, NULL, 0, "txt", strlen("txt"));
    TEST_ASSERT(logName != NULL, "generate_Log_Name should return a valid pointer when log name is NULL");
    TEST_ASSERT(strstr(logName, "unknown") != NULL, "Log name should contain 'unknown' when log name is NULL");
    free(logName);

    // Test when logPath != NULL and > 0
    logName = generate_Log_Name(NAMING_SERIAL_NUMBER_DATE_TIME, "device123", strlen("device123"), "/logs", strlen("/logs"), "logfile", strlen("logfile"), "txt", strlen("txt"));
    TEST_ASSERT(logName != NULL, "generate_Log_Name should return a valid pointer when logPath is provided");
    TEST_ASSERT(strstr(logName, "/logs") != NULL, "Log name should contain the log path");
    free(logName);

    // Test when logExt is NULL
    logName = generate_Log_Name(NAMING_SERIAL_NUMBER_DATE_TIME, "device123", strlen("device123"), NULL, 0, "logfile", strlen("logfile"), NULL, 0);
    TEST_ASSERT(logName != NULL, "generate_Log_Name should return a valid pointer when log extension is NULL");
    free(logName);

    // Test when device identifier is NULL
    logName = generate_Log_Name(NAMING_SERIAL_NUMBER_DATE_TIME, NULL, 0, NULL, 0, "logfile", strlen("logfile"), "txt", strlen("txt"));
    TEST_ASSERT(logName == NULL, "generate_Log_Name should return NULL when device identifier is NULL");
}

static void test_create_And_Open_Secure_Log_File(void) {
    secureFileInfo* fileInfo = NULL;

    // Test when deviceIdentifier is NULL
    eReturnValues result = create_And_Open_Secure_Log_File(NULL, 0, &fileInfo, NAMING_SERIAL_NUMBER_DATE_TIME, NULL, 0, "logfile", strlen("logfile"), "txt", strlen("txt"));
    TEST_ASSERT(result == BAD_PARAMETER, "Function should return BAD_PARAMETER when device identifier is NULL");

    // Test for insecure path
    system("mkdir insecure_dir");
    system("chmod 777 insecure_dir");
    result = create_And_Open_Secure_Log_File("device123", strlen("device123"), &fileInfo, NAMING_SERIAL_NUMBER_DATE_TIME, "insecure_dir", strlen("insecure_dir"), "logfile", strlen("logfile"), "txt", strlen("txt"));
    TEST_ASSERT(result == INSECURE_PATH, "Function should return INSECURE_PATH when log path is not secure");
    rmdir("insecure_dir");

    result = create_And_Open_Secure_Log_File("device123", strlen("device123"), &fileInfo, NAMING_SERIAL_NUMBER_DATE_TIME, NULL, 0, "logfile", strlen("logfile"), "txt", strlen("txt"));
    TEST_ASSERT(result == SUCCESS, "Function should succeed");
    TEST_ASSERT(fileInfo != NULL, "fileInfo should be allocated");
    TEST_ASSERT(fileInfo->isValid, "File info should be valid");
    TEST_ASSERT(fileInfo->file != NULL, "File pointer should be valid");

    TEST_ASSERT(os_File_Exists(fileInfo->filename), "Generated log file should exist");
    TEST_ASSERT(strstr(fileInfo->filename, ".txt") != NULL, "File should have .txt extension");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    // Test with pre-created file
    FILE* f = fopen("testfile.bin", "wb");
    fclose(f);

    secureFileInfo* file = NULL;

    result = create_And_Open_Secure_Log_File("testfile", strlen("testfile"), &file, NAMING_SERIAL_NUMBER_ONLY, NULL, 0, NULL, 0, ".bin", 4);
    TEST_ASSERT(result == FAILURE, "Function should fail when file already exists");
}

void run_secure_file_tests(void) {
    test_compare_File_Unique_ID();
    // test_os_Get_File_Unique_Identifying_Information();
    test_os_Is_Directory_Secure();
    test_free_File_Attributes();
    test_os_Get_File_Attributes_By_Name();
    test_os_Get_File_Attributes_By_File();
    test_free_Secure_File_Info();
    test_secure_Open_File();
    test_secure_Close_File();
    test_secure_Read_File();
    test_secure_Write_File();
    test_secure_Seek_File();
    test_secure_Rewind_File();
    test_secure_Tell_File();
    // test_secure_Remove_File();
    test_secure_Delete_File_By_Name();
    test_secure_Flush_File();
    test_secure_GetPos_File();
    test_secure_SetPos_File();
    test_secure_vfprintf_File();
    test_secure_fprintf_File();
    test_os_Directory_Exists();
    test_os_File_Exists();
    test_os_Create_Directory();
    test_os_Create_Secure_Directory();
    test_get_Full_Path();
    test_replace_File_Name_In_Path();
    test_os_Get_File_Size();
    #if defined(_WIN32)
    test_exact_Compare_SIDS_And_DACL_Strings();
    #endif
    test_generate_Log_Name();
    test_create_And_Open_Secure_Log_File();
}