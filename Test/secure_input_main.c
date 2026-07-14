#include <stdio.h>
#include "test_framework.h"

void run_test_secure_inputs(void);

int main(void)
{
    printf("Starting secure input tests...\n\n");

    run_test_secure_inputs();

    print_test_summary();

    return (failed_tests == 0) ? 0 : 1;
}