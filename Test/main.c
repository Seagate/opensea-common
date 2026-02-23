#include<stdio.h>
#include "framework/test_framework.h"

// Forward declarations of test functions
void run_bit_manip_tests(void);
void run_math_utils_tests(void);

int main(void)
{
    printf("Starting unit tests...\n\n");

    // Run individual test suites
    run_bit_manip_tests();
    run_math_utils_tests();
    run_string_utils_tests();

    // Print summary of test results
    print_test_summary();

    return (failed_tests == 0) ? 0 : 1;
}