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

    // Cast long long to int
    long long long_long_val = 123456789012345LL;
    int_val = C_CAST(int, long_long_val);
    TEST_ASSERT(int_val == (int)long_long_val, "C_CAST should convert long long to int correctly");
}

static void test_M_STATIC_CAST(void) {
    // Cast int to double
    int int_val = 42;
    double double_val = M_STATIC_CAST(double, int_val);
    TEST_ASSERT(double_val == 42.0, "M_STATIC_CAST should convert int to double correctly");

    // Cast double to int
    double_val = 3.14;
    int_val = M_STATIC_CAST(int, double_val);
    TEST_ASSERT(int_val == 3, "M_STATIC_CAST should convert double to int correctly");

    // Cast float to int
    float float_val = 2.718f;
    int_val = M_STATIC_CAST(int, float_val);
    TEST_ASSERT(int_val == 2, "M_STATIC_CAST should convert float to int correctly");

    // Cast char to int
    char char_val = 'A';
    int_val = M_STATIC_CAST(int, char_val);
    TEST_ASSERT(int_val == 65, "M_STATIC_CAST should convert char to int correctly");

    // Cast long long to int
    long long long_long_val = 123456789012345LL;
    int_val = M_STATIC_CAST(int, long_long_val);
    TEST_ASSERT(int_val == (int)long_long_val, "M_STATIC_CAST should convert long long to int correctly");
}

static void test_M_REINTERPRET_CAST(void) {
    // Cast int pointer to char pointer
    int int_val = 0x12345678;
    char* char_ptr = M_REINTERPRET_CAST(char*, &int_val);
    TEST_ASSERT(*char_ptr == 0x78, "M_REINTERPRET_CAST should reinterpret int pointer as char pointer correctly");

    // Cast char pointer back to int pointer
    int* int_ptr = M_REINTERPRET_CAST(int*, char_ptr);
    TEST_ASSERT(*int_ptr == 0x12345678, "M_REINTERPRET_CAST should reinterpret char pointer back to int pointer correctly");
}

void run_type_conversion_tests(void) {
    test_C_CAST();
    test_M_STATIC_CAST();   
    test_M_REINTERPRET_CAST();
}