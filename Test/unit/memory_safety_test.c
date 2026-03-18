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

void run_memory_safety_tests(void) {
    test_safe_malloc();
    test_safe_calloc();
}