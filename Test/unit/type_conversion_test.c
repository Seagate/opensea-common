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
    TEST_ASSERT(bool_negative == false, "M_ToBool should convert negative to false");

    // Test with an expression
    bool bool_expression = M_ToBool(5 > 3);
    TEST_ASSERT(bool_expression == true, "M_ToBool should convert true expression to true");

    bool bool_false_expression = M_ToBool(2 > 3);
    TEST_ASSERT(bool_false_expression == false, "M_ToBool should convert false expression to false");
}

static void test_int8_to_sizet(void) {
    // Test with minimum int8_t value
    int8_t min_val = INT8_MIN_VAL;
    errno = 0; 
    size_t sizet_min = int8_to_sizet(min_val);
    TEST_ASSERT(sizet_min == 0, "int8_to_sizet should convert INT8_MIN to 0");
    TEST_ASSERT(errno == ERANGE, "int8_to_sizet should set errno to ERANGE for negative input");

    // Test with maximum int8_t value
    int8_t max_val = INT8_MAX_VAL;
    size_t sizet_max = int8_to_sizet(max_val);
    TEST_ASSERT(sizet_max == (size_t)INT8_MAX_VAL, "int8_to_sizet should convert INT8_MAX correctly");

    // Test with a negative value
    int8_t negative_val = -5;
    errno = 0;
    size_t sizet_negative = int8_to_sizet(negative_val);
    TEST_ASSERT(sizet_negative == 0, "int8_to_sizet should convert negative values to 0");
    TEST_ASSERT(errno == ERANGE, "int8_to_sizet should set errno to ERANGE for negative input");

    // Test with a positive value
    int8_t positive_val = 10;
    size_t sizet_positive = int8_to_sizet(positive_val);
    TEST_ASSERT(sizet_positive == (size_t)positive_val, "int8_to_sizet should convert positive values correctly");

    // Test with a value greater than SIZE_MAX - Not possible for real systems as SIZE_MAX is typically much larger than INT8_MAX.  
    errno = 0;
    size_t sizet_large = int8_to_sizet(SIZE_MAX + 1);

    // Test for val > INT8_MAX
    int val = 130;
    errno = 0;
    size_t sizet_overflow = int8_to_sizet(val);
    TEST_ASSERT(sizet_overflow == 0, "int8_to_sizet should convert the values to int8 first for values greater than INT8_MAX");
    TEST_ASSERT(errno == ERANGE, "int8_to_sizet should set errno to ERANGE for values greater than INT8_MAX");
}

static void test_uint8_to_sizet(void) {
    // Test with minimum uint8_t value
    uint8_t min_val = UINT8_MIN_VAL;
    size_t sizet_min = uint8_to_sizet(min_val);
    TEST_ASSERT(sizet_min == 0, "uint8_to_sizet should convert UINT8_MIN to 0");

    // Test with maximum uint8_t value
    uint8_t max_val = UINT8_MAX_VAL;
    size_t sizet_max = uint8_to_sizet(max_val);
    TEST_ASSERT(sizet_max == (size_t)UINT8_MAX_VAL, "uint8_to_sizet should convert UINT8_MAX correctly");

    // Test with a negative value
    int8_t negative_val = -5;
    errno = 0;
    size_t sizet_negative = uint8_to_sizet(negative_val);
    TEST_ASSERT(sizet_negative == 251, "uint8_to_sizet should convert negative values to positive values");

    // Test with a positive value
    int8_t positive_val = 10;
    size_t sizet_positive = uint8_to_sizet(positive_val);
    TEST_ASSERT(sizet_positive == (size_t)positive_val, "uint8_to_sizet should convert positive values correctly");

    // Test with a value greater than SIZE_MAX - Not possible for real systems as SIZE_MAX is typically much larger than UINT8_MAX.  
    errno = 0;
    size_t sizet_large = uint8_to_sizet(SIZE_MAX + 1);
}

static void test_int16_to_sizet(void) {
    // Test with minimum int16_t value
    int16_t min_val = INT16_MIN_VAL;
    errno = 0; 
    size_t sizet_min = int16_to_sizet(min_val);
    TEST_ASSERT(sizet_min == 0, "int16_to_sizet should convert INT16_MIN to 0");
    TEST_ASSERT(errno == ERANGE, "int16_to_sizet should set errno to ERANGE for negative input");

    // Test with maximum int16_t value
    int16_t max_val = INT16_MAX_VAL;
    size_t sizet_max = int16_to_sizet(max_val);
    TEST_ASSERT(sizet_max == (size_t)INT16_MAX_VAL, "int16_to_sizet should convert INT16_MAX correctly");

    // Test with a negative value
    int16_t negative_val = -5;
    errno = 0;
    size_t sizet_negative = int16_to_sizet(negative_val);
    TEST_ASSERT(sizet_negative == 0, "int16_to_sizet should convert negative values to 0");
    TEST_ASSERT(errno == ERANGE, "int16_to_sizet should set errno to ERANGE for negative input");

    // Test with a positive value
    int16_t positive_val = 10;
    size_t sizet_positive = int16_to_sizet(positive_val);
    TEST_ASSERT(sizet_positive == (size_t)positive_val, "int16_to_sizet should convert positive values correctly");
}

static void test_uint16_to_sizet(void) {
    // Test with minimum uint16_t value
    uint16_t min_val = UINT16_MIN_VAL;
    size_t sizet_min = uint16_to_sizet(min_val);
    TEST_ASSERT(sizet_min == 0, "uint16_to_sizet should convert UINT16_MIN to 0");

    // Test with maximum uint16_t value
    uint16_t max_val = UINT16_MAX_VAL;
    size_t sizet_max = uint16_to_sizet(max_val);
    TEST_ASSERT(sizet_max == (size_t)UINT16_MAX_VAL, "uint16_to_sizet should convert UINT16_MAX correctly");

    // Test with a negative value
    int16_t negative_val = -5;
    errno = 0;
    size_t sizet_negative = uint16_to_sizet(negative_val);
    TEST_ASSERT(sizet_negative == 65531, "uint16_to_sizet should convert negative values to positive values");

    // Test with a positive value
    int16_t positive_val = 10;
    size_t sizet_positive = uint16_to_sizet(positive_val);
    TEST_ASSERT(sizet_positive == (size_t)positive_val, "uint16_to_sizet should convert positive values correctly");
}

static void test_int32_to_sizet(void) {
    // Test with minimum int32_t value
    int32_t min_val = INT32_MIN_VAL;
    errno = 0; 
    size_t sizet_min = int32_to_sizet(min_val);
    TEST_ASSERT(sizet_min == 0, "int32_to_sizet should convert INT32_MIN to 0");
    TEST_ASSERT(errno == ERANGE, "int32_to_sizet should set errno to ERANGE for negative input");

    // Test with maximum int32_t value
    int32_t max_val = INT32_MAX_VAL;
    size_t sizet_max = int32_to_sizet(max_val);
    TEST_ASSERT(sizet_max == (size_t)INT32_MAX_VAL, "int32_to_sizet should convert INT32_MAX correctly");

    // Test with a negative value
    int32_t negative_val = -5;
    errno = 0;
    size_t sizet_negative = int32_to_sizet(negative_val);
    TEST_ASSERT(sizet_negative == 0, "int32_to_sizet should convert negative values to 0");
    TEST_ASSERT(errno == ERANGE, "int32_to_sizet should set errno to ERANGE for negative input");

    // Test with a positive value
    int32_t positive_val = 10;
    size_t sizet_positive = int32_to_sizet(positive_val);
    TEST_ASSERT(sizet_positive == (size_t)positive_val, "int32_to_sizet should convert positive values correctly");

    // Test for val > INT32_MAX
    int val = (int64_t)INT32_MAX + 1;
    errno = 0;
    size_t sizet_overflow = int32_to_sizet(val);
    TEST_ASSERT(sizet_overflow == 0, "int32_to_sizet should convert the values to int32 first for values greater than INT32_MAX");
    TEST_ASSERT(errno == ERANGE, "int32_to_sizet should set errno to ERANGE for values greater than INT32_MAX");
}

static void test_uint32_to_sizet(void) {
    // Test with minimum uint32_t value
    uint32_t min_val = UINT32_MIN_VAL;
    size_t sizet_min = uint32_to_sizet(min_val);
    TEST_ASSERT(sizet_min == 0, "uint32_to_sizet should convert UINT32_MIN to 0");

    // Test with maximum uint32_t value
    uint32_t max_val = UINT32_MAX_VAL;
    size_t sizet_max = uint32_to_sizet(max_val);
    TEST_ASSERT(sizet_max == (size_t)UINT32_MAX_VAL, "uint32_to_sizet should convert UINT32_MAX correctly");

    // Test with a negative value
    int64_t negative_val = -5;
    errno = 0;
    size_t sizet_negative = uint32_to_sizet(negative_val);
    TEST_ASSERT(sizet_negative == 4294967291, "uint32_to_sizet should convert negative values to positive values");

    // Test with a positive value
    int64_t positive_val = 10;
    size_t sizet_positive = uint32_to_sizet(positive_val);
    TEST_ASSERT(sizet_positive == (size_t)positive_val, "uint32_to_sizet should convert positive values correctly");

    // Test for val > UINT32_MAX
    uint64_t val = (uint64_t)UINT32_MAX + 1;
    errno = 0;
    size_t sizet_overflow = uint32_to_sizet(val);
    TEST_ASSERT(sizet_overflow == 0, "uint32_to_sizet should convert the values to uint32 first for values greater than UINT32_MAX");
}

static void test_int64_to_sizet(void) {
    int64_t min_val = INT64_MIN_VAL;
    errno = 0; 
    size_t sizet_min = int64_to_sizet(min_val);
    TEST_ASSERT(sizet_min == 0, "int64_to_sizet should convert INT64_MIN to 0");
    TEST_ASSERT(errno == ERANGE, "int64_to_sizet should set errno to ERANGE for negative input");

    int64_t max_val = INT64_MAX_VAL;
    size_t sizet_max = int64_to_sizet(max_val);
    TEST_ASSERT(sizet_max == (size_t)INT64_MAX_VAL, "int64_to_sizet should convert INT64_MAX correctly");

    int64_t negative_val = -5;
    errno = 0;
    size_t sizet_negative = int64_to_sizet(negative_val);
    TEST_ASSERT(sizet_negative == 0, "int64_to_sizet should convert negative values to 0");
    TEST_ASSERT(errno == ERANGE, "int64_to_sizet should set errno to ERANGE for negative input");

    int64_t positive_val = 10;
    size_t sizet_positive = int64_to_sizet(positive_val);
    TEST_ASSERT(sizet_positive == (size_t)positive_val, "int64_to_sizet should convert positive values correctly");
}

static void test_uint64_to_sizet(void) {
    uint64_t min_val = UINT64_MIN_VAL;
    size_t sizet_min = ulonglong_to_sizet(min_val);
    TEST_ASSERT(sizet_min == 0, "uint64_to_sizet should convert UINT64_MIN to 0");

    uint64_t max_val = UINT64_MAX_VAL;
    size_t sizet_max = ulonglong_to_sizet(max_val);
    TEST_ASSERT(sizet_max == (size_t)UINT64_MAX_VAL, "uint64_to_sizet should convert UINT64_MAX correctly");

    int64_t negative_val = -2000;
    errno = 0;
    size_t sizet_negative = uint64_to_sizet(negative_val);
    TEST_ASSERT(sizet_negative == 18446744073709549616, "uint64_to_sizet should convert negative values to positive values");
    TEST_ASSERT(errno == ERANGE, "uint64_to_sizet should set errno to ERANGE for negative input");

    int64_t positive_val = 10;
    size_t sizet_positive = uint64_to_sizet(positive_val);
    TEST_ASSERT(sizet_positive == (size_t)positive_val, "uint64_to_sizet should convert positive values correctly");
}


void run_type_conversion_tests(void) {
    test_C_CAST();
    test_M_STATIC_CAST();   
    test_M_REINTERPRET_CAST();
    test_M_CONST_CAST();
    test_M_ToBool();
    test_int8_to_sizet();
    test_uint8_to_sizet();
    test_int16_to_sizet();
    test_uint16_to_sizet();
    test_int32_to_sizet();
    test_uint32_to_sizet();
    test_int64_to_sizet();
    test_uint64_to_sizet();
}