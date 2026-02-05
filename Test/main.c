#include<stdio.h>
#include "framework/test_framework.h"

// Forward declarations of test functions
void run_bit_manip_tests(void);

int main(void)
{
    printf("Starting unit tests...\n\n");

    // Run individual test suites
    run_bit_manip_tests();

    // Print summary of test results
    print_test_summary();

    return (failed_tests == 0) ? 0 : 1;
}