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
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint64("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint64("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint64("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint64("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint64("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_Uint32(void) {
    uint32_t outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint32("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint32("123f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint32("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint32("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint32("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint32("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_Uint16(void) {
    uint16_t outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint16("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint16("123f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint16("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint16("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint16("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint16("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_Uint8(void) {
    uint8_t outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint8("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint8("123f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint8("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint8("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint8("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint8("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

void run_io_utils_tests(void) {
    test_get_And_Validate_Integer_Input();
    test_get_And_Validate_Integer_Input_Uint64();
    test_get_And_Validate_Integer_Input_Uint32();
    test_get_And_Validate_Integer_Input_Uint16();
    test_get_And_Validate_Integer_Input_Uint8();
}
