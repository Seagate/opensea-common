#include<stdio.h>
#include "framework/test_framework.h"

// Forward declarations of test functions
void run_bit_manip_tests(void);
void run_math_utils_tests(void);
void run_string_utils_tests(void);
void run_io_utils_tests(void);
void run_memory_safety_tests(void);
void run_pattern_utils_tests(void);
void run_sort_and_search_tests(void);
void run_type_conversion_tests(void);
void run_unit_conversion_tests(void);

int main(void)
{
    printf("Starting unit tests...\n\n");

    // Run individual test suites
    run_bit_manip_tests();
    run_math_utils_tests();
    run_string_utils_tests();
    run_io_utils_tests();
    run_memory_safety_tests();
    run_pattern_utils_tests();
    run_sort_and_search_tests();
    run_type_conversion_tests();
    run_unit_conversion_tests();
    
    // Print summary of test results
    print_test_summary();

    return (failed_tests == 0) ? 0 : 1;
}