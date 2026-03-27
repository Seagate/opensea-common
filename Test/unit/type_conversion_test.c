#include"../framework/test_framework.h"
#include"../../include/type_conversion.h"
#include"../testConstants.h"
#include <limits.h>

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

    // Cast int pointer to void pointer and back
    int x = 10;
    void *ptr = C_CAST(void*, &x);
    int *int_ptr = C_CAST(int*, ptr);
    TEST_ASSERT(*int_ptr == 10, "C_CAST should cast void* back to int*");

    // Test truncation when casting a large long long to int
    long long big = LLONG_MAX;
    int truncated = C_CAST(int, big);
    TEST_ASSERT(truncated == (int)big, "C_CAST handles truncation correctly");
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

    // Cast int pointer to void pointer
    void* void_ptr = M_REINTERPRET_CAST(void*, &int_val);
    TEST_ASSERT(void_ptr == &int_val, "M_REINTERPRET_CAST should reinterpret int pointer as void pointer correctly");
}

static void test_M_CONST_CAST(void) {
    // Cast const int pointer to int pointer
    const int const_int_val = 42;
    int* int_ptr = M_CONST_CAST(int*, &const_int_val);
    TEST_ASSERT(*int_ptr == 42, "M_CONST_CAST should cast const int pointer to int pointer correctly");

    // Cast int pointer back to const int pointer
    const int* const_int_ptr = M_CONST_CAST(const int*, int_ptr);
    TEST_ASSERT(*const_int_ptr == 42, "M_CONST_CAST should cast int pointer back to const int pointer correctly");

    // Cast volatile int pointer to int pointer
    volatile int volatile_int_val = 100;
    int* volatile_int_ptr = M_CONST_CAST(int*, &volatile_int_val);
    TEST_ASSERT(*volatile_int_ptr == 100, "M_CONST_CAST should cast volatile int pointer to int pointer correctly");
}

static void test_M_ToBool(void) {
    // Test with zero value
    int zero = 0;
    bool bool_zero = M_ToBool(zero);
    TEST_ASSERT(bool_zero == false, "M_ToBool should convert zero to false");

    // Test with non-zero value
    int non_zero = 42;
    bool bool_non_zero = M_ToBool(non_zero);
    TEST_ASSERT(bool_non_zero == true, "M_ToBool should convert non-zero to true");

    // Test with negative value
    int negative = -1;
    bool bool_negative = M_ToBool(negative);
    TEST_ASSERT(bool_negative == true, "M_ToBool should convert negative to true");

    // Test with an expression
    bool bool_expression = M_ToBool(5 > 3);
    TEST_ASSERT(bool_expression == true, "M_ToBool should convert true expression to true");

    bool bool_false_expression = M_ToBool(2 > 3);
    TEST_ASSERT(bool_false_expression == false, "M_ToBool should convert false expression to false");
}

void run_type_conversion_tests(void) {
    test_C_CAST();
    test_M_STATIC_CAST();   
    test_M_REINTERPRET_CAST();
    test_M_CONST_CAST();
    test_M_ToBool();
}