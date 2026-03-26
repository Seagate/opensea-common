#include"../framework/test_framework.h"
#include"../../include/pattern_utils.h"
#include"../testConstants.h"

static void test_fill_Random_Pattern_In_Buffer(void) {
    // Test with valid parameters
    uint8_t buffer[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    eReturnValues result = fill_Random_Pattern_In_Buffer(buffer, RSIZE_MAX);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS for valid parameters");
    printf("Buffer after fill_Random_Pattern_In_Buffer: ");
    for (size_t i = 0; i < sizeof(buffer); i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");

    // Test with zero data length
    // result = fill_Random_Pattern_In_Buffer(buffer, 0);
    // TEST_ASSERT(result == BAD_PARAMETER, "Gives error for zero data length");

    // Test with null pointer
    // result = fill_Random_Pattern_In_Buffer(NULL, sizeof(buffer));
    // TEST_ASSERT(result == BAD_PARAMETER, "Gives error for null pointer");
}

void run_pattern_utils_tests(void) {
    test_fill_Random_Pattern_In_Buffer();
}