#include "test_framework.h"

int total_tests = 0;
int failed_tests = 0;

void test_record(int condition, const char* msg, const char *file, int line) {
    if (!condition) {
        printf("TEST FAILED: %s (at %s:%d)\n", msg, file, line);
    } else {
        printf("TEST PASSED: %s\n", msg);
    }
}

void print_test_summary(void)
{
    int passed = total_tests - failed_tests;
    printf("\n=== TEST SUMMARY ===\n");
    printf("Total: %d\n", total_tests);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed_tests);
}
