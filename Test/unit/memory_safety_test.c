#include"../framework/test_framework.h"
#include"../../include/io_utils.h"
#include"../testConstants.h"

static void test_safe_malloc(void) {
    void* ptr = safe_malloc(100);
    TEST_ASSERT(ptr != NULL, "safe_malloc should return a non-null pointer for a non-zero size");
    free(ptr);

    // Test that safe_malloc returns null for a size of zero
    // ptr = safe_malloc(0);
    // TEST_ASSERT(ptr == NULL, "safe_malloc should return a null pointer for a size of zero");
    // free(ptr);
}

static void test_safe_calloc(void) {
    void* ptr = safe_calloc(10, 20);
    TEST_ASSERT(ptr != NULL, "safe_calloc should return a non-null pointer for non-zero count and size");
    free(ptr);

    // Test that safe_calloc returns null for a count of zero
    // ptr = safe_calloc(0, 20);
    // TEST_ASSERT(ptr == NULL, "safe_calloc should return a null pointer for a count of zero");
    // free(ptr);

    // Test that safe_calloc returns null for a size of zero
    // ptr = safe_calloc(10, 0);
    // TEST_ASSERT(ptr == NULL, "safe_calloc should return a null pointer for a size of zero");
    // free(ptr);
}

static void test_safe_realloc(void) {
    void* ptr = safe_malloc(100);
    TEST_ASSERT(ptr != NULL, "safe_malloc should return a non-null pointer for a non-zero size");

    // Test that reallocating to a larger size works
    void* new_ptr = safe_realloc(ptr, 200);
    TEST_ASSERT(new_ptr != NULL, "safe_realloc should return a non-null pointer when reallocating to a larger size");
    free(new_ptr);

    // Test that reallocating to zero frees the memory and returns null
    ptr = safe_malloc(100);
    new_ptr = safe_realloc(ptr, 0);
    TEST_ASSERT(new_ptr == NULL, "safe_realloc should return a null pointer when reallocating to zero");
}

static void test_safe_reallocf(void) {
    void* ptr = safe_malloc(100);
    TEST_ASSERT(ptr != NULL, "safe_malloc should return a non-null pointer for a non-zero size");

    // Test that reallocating to a larger size works
    void* new_ptr = safe_reallocf(&ptr, 200);
    TEST_ASSERT(new_ptr != NULL, "safe_reallocf should return a non-null pointer when reallocating to a larger size");
    free(new_ptr);

    // Test that reallocating to zero frees the memory and returns null
    ptr = safe_malloc(100);
    new_ptr = safe_reallocf(&ptr, 0);
    TEST_ASSERT(new_ptr == NULL, "safe_reallocf should return a null pointer when reallocating to zero");

    // Pointer to block is NULL, should return NULL and not attempt to free
    new_ptr = safe_reallocf(NULL, 100);
    TEST_ASSERT(new_ptr == NULL, "safe_reallocf should return a null pointer when the input pointer is NULL");

    // Size is zero, should free the original block and return NULL
    ptr = safe_malloc(100);
    new_ptr = safe_reallocf(&ptr, 0);
    TEST_ASSERT(new_ptr == NULL, "safe_reallocf should return a null pointer when reallocating to zero");
    TEST_ASSERT(ptr == NULL, "safe_reallocf should set the original pointer to NULL when reallocating to zero");

    // Reallocating to a larger size but realloc fails, should free the original block and return NULL
    ptr = safe_malloc(100);
    new_ptr = safe_reallocf(&ptr, RSIZE_MAX);
    TEST_ASSERT(new_ptr == NULL, "safe_reallocf should return a null pointer when reallocating to an excessively large size");
    TEST_ASSERT(ptr == NULL, "safe_reallocf should set the original pointer to NULL when reallocating to an excessively large size");
}

static void test_safe_free_core(void) {
    char* ptr = safe_malloc(100);
    safe_free_core((void**)&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_core should set the pointer to NULL after freeing");
}

static void test_safe_free_char(void) {
    char* ptr = safe_malloc(100);
    safe_free_char(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_char should set the pointer to NULL after freeing");
}

static void test_safe_free_schar(void) {
    signed char* ptr = safe_malloc(100);
    safe_free_schar(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_schar should set the pointer to NULL after freeing");
}

static void test_safe_free_uchar(void) {
    unsigned char* ptr = safe_malloc(100);
    safe_free_uchar(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_uchar should set the pointer to NULL after freeing");
}

static void test_safe_free_wchar(void) {
    wchar_t* ptr = safe_malloc(100);
    safe_free_wchar(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_wchar should set the pointer to NULL after freeing");
}

static void test_safe_free_short(void) {
    signed short* ptr = safe_malloc(100);
    safe_free_short(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_short should set the pointer to NULL after freeing");
}

static void test_safe_free_ushort(void) {
    unsigned short* ptr = safe_malloc(100);
    safe_free_ushort(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_ushort should set the pointer to NULL after freeing");
}

static void test_safe_free_int(void) {
    signed int* ptr = safe_malloc(100);
    safe_free_int(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_int should set the pointer to NULL after freeing");
}

static void test_safe_free_uint(void) {
    unsigned int* ptr = safe_malloc(100);
    safe_free_uint(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_uint should set the pointer to NULL after freeing");
}

void run_memory_safety_tests(void) {
    test_safe_malloc();
    test_safe_calloc();
    test_safe_realloc();
    test_safe_reallocf();
    test_safe_free_core();
    test_safe_free_char();
    test_safe_free_schar();
    test_safe_free_uchar();
    test_safe_free_wchar();
    test_safe_free_short();
    test_safe_free_ushort();
    test_safe_free_int();
    test_safe_free_uint();
}