#include"../framework/test_framework.h"
#include"../../include/io_utils.h"
#include"../testConstants.h"

static void test_get_And_Validate_Integer_Input(void) {
    uint64_t outputInteger;
    TEST_ASSERT(get_And_Validate_Integer_Input("12345", &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input("0xFF", &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input("xyz", &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input("12.5", &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_Uint64(void) {
    uint64_t outputInteger;
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint64("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint64("12ms", NULL, ALLOW_UNIT_SECTOR_TYPE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint64("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint64("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint64("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

void run_io_utils_tests(void) {
    test_get_And_Validate_Integer_Input();
    test_get_And_Validate_Integer_Input_Uint64();
}
