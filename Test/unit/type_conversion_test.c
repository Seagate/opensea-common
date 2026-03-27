#include"../framework/test_framework.h"
#include"../../include/type_conversion.h"
#include"../testConstants.h"

static void test_C_CAST(void) {
    // Cast int to double
    int int_val = 42;
    double double_val = C_CAST(double, int_val);
    TEST_ASSERT(double_val == 42.0, "C_CAST should convert int to double correctly");

    // Cast double to int
    double_val = 3.14;
    int_val = C_CAST(int, double_val);
    TEST_ASSERT(int_val == 3, "C_CAST should convert double to int correctly");

    // Cast float to int
    float float_val = 2.718f;
    int_val = C_CAST(int, float_val);
    TEST_ASSERT(int_val == 2, "C_CAST should convert float to int correctly");

    // Cast char to int
    char char_val = 'A';
    int_val = C_CAST(int, char_val);
    TEST_ASSERT(int_val == 65, "C_CAST should convert char to int correctly");
}

void run_type_conversion_tests(void) {
    printf("Running type conversion tests...\n");
    test_C_CAST();
}