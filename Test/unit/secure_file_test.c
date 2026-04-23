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

void run_secure_file_tests(void) {
    test_compare_File_Unique_ID();
    // test_os_Get_File_Unique_Identifying_Information();
    test_os_Is_Directory_Secure();
    test_free_File_Attributes();
    test_os_Get_File_Attributes_By_Name();
    test_os_Get_File_Attributes_By_File();
}