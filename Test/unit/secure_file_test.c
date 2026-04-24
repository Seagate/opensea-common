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

    // Set some dummy values
    fileInfo->error = SEC_FILE_SUCCESS;
    fileInfo->isValid = true;
    fileInfo->file = NULL;
    fileInfo->filename = NULL;
    fileInfo->fileno = 0;
    fileInfo->fileSize = 0;
    fileInfo->attributes = NULL;
    fileInfo->uniqueID = NULL;
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
}

static void test_secure_Close_File(void) {
    const char* filename = "test_secure_close.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello");
    fclose(f);

    secureFileInfo* fileInfo = secure_Open_File(filename, "r", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    eSecureFileError closeResult = secure_Close_File(fileInfo);
    TEST_ASSERT(closeResult == SEC_FILE_SUCCESS, "secure_Close_File should succeed");

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

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);
}

static void test_secure_Write_File(void) {
    const char* filename = "test_secure_write.txt";
    secureFileInfo* fileInfo = secure_Open_File(filename, "w", NULL, NULL, NULL);
    TEST_ASSERT(fileInfo != NULL, "secure_Open_File should return a valid pointer");
    TEST_ASSERT(fileInfo->isValid, "secure_Open_File should return valid file info");

    const char* data = "hello";
    eSecureFileError writeResult = secure_Write_File(fileInfo, (void*)data, strlen(data), 1, strlen(data), NULL);
    TEST_ASSERT(writeResult == SEC_FILE_SUCCESS, "secure_Write_File should succeed");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);

    // Verify the file was written correctly
    FILE* f = fopen(filename, "r");
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

    // Move the file position to 6 (after "hello ")
    long pos = 6;
    eSecureFileError result = secure_Seek_File(fileInfo, pos, SEEK_SET);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Seek_File should succeed");

    // Read the next 5 characters ("world")
    char buffer[10] = {0};
    eSecureFileError readResult = secure_Read_File(fileInfo, buffer, sizeof(buffer), 1, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_SUCCESS, "secure_Read_File should succeed");
    TEST_ASSERT(strcmp(buffer, "world") == 0, "Buffer should contain 'world'");

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

    // Rewind the file back to the beginning
    eSecureFileError rewindResult = secure_Rewind_File(fileInfo);
    TEST_ASSERT(rewindResult == SEC_FILE_SUCCESS, "secure_Rewind_File should succeed");

    // Read the first 5 characters ("hello")
    char buffer[10] = {0};
    eSecureFileError readResult = secure_Read_File(fileInfo, buffer, sizeof(buffer), 1, 5, NULL);
    TEST_ASSERT(readResult == SEC_FILE_SUCCESS, "secure_Read_File should succeed");
    TEST_ASSERT(strcmp(buffer, "hello") == 0, "Buffer should contain 'hello'");

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

    // Move the file position to 6 (after "hello ")
    long pos = 6;
    eSecureFileError result = secure_Seek_File(fileInfo, pos, SEEK_SET);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Seek_File should succeed");
    oscoffset_t currentPos = secure_Tell_File(fileInfo);
    TEST_ASSERT(currentPos == pos, "secure_Tell_File should return the correct file position");

    secure_Close_File(fileInfo);
    free_Secure_File_Info(&fileInfo);
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
    const char* filename = "test_secure_delete_by_name.txt";
    FILE* f = fopen(filename, "w");
    fprintf(f, "hello world");
    fclose(f);

    // If file is open, SEC_DELETE_NAME_UNLINK_IF_OPEN should succeed by unlinking the file
    f = fopen(filename, "r");
    eSecureFileError result = secure_Delete_File_By_Name(filename, SEC_DELETE_NAME_UNLINK_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Delete_File_By_Name should succeed with unlink if file is open");
    TEST_ASSERT(!os_File_Exists(filename), "File should be unlinked and not exist");

    // SEC_DELETE_NAME_FAIL_IF_OPEN fails if the file is open

    FILE* f2 = fopen(filename, "r");
    result = secure_Delete_File_By_Name(filename, SEC_DELETE_NAME_FAIL_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_CANNOT_REMOVE_FILE_STILL_OPEN, "secure_Delete_File_By_Name should fail if file is open");

    fclose(f);

    result = secure_Delete_File_By_Name(filename, SEC_DELETE_NAME_FAIL_IF_OPEN);
    TEST_ASSERT(result == SEC_FILE_SUCCESS, "secure_Delete_File_By_Name should succeed");
    TEST_ASSERT(!os_File_Exists(filename), "File should be removed");
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
}