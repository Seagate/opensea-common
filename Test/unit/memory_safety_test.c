#include"../framework/test_framework.h"
#include"../../include/io_utils.h"
#include"../testConstants.h"

static void test_safe_malloc(void) {
    void* ptr = safe_malloc(100);
    TEST_ASSERT(ptr != NULL, "safe_malloc should return a non-null pointer for a non-zero size");
    free(ptr);

    // Test that safe_malloc returns null for a size of zero
    ptr = safe_malloc(0);
    TEST_ASSERT(ptr == NULL, "safe_malloc should return a null pointer for a size of zero");
}

void run_memory_safety_tests(void) {
    test_safe_malloc();
}