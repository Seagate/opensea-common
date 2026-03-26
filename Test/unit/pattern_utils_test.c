#include"../framework/test_framework.h"
#include"../../include/pattern_utils.h"
#include"../testConstants.h"

static void test_fill_Random_Pattern_In_Buffer(void) {
    // Test with valid parameters
    uint8_t buffer[10];
    eReturnValues result = fill_Random_Pattern_In_Buffer(buffer, sizeof(buffer));
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

static void test_fill_Hex_Pattern_In_Buffer(void) {
    // Test with valid parameters
    uint8_t buffer[10];
    uint32_t hexPattern = 0xAABBCCDD;
    eReturnValues result = fill_Hex_Pattern_In_Buffer(hexPattern, buffer, sizeof(buffer));
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS for valid parameters");
    printf("Buffer after fill_Hex_Pattern_In_Buffer: ");
    for (size_t i = 0; i < sizeof(buffer); i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");

    // Test with zero data length
    // result = fill_Hex_Pattern_In_Buffer(hexPattern, buffer, 0);
    // TEST_ASSERT(result == BAD_PARAMETER, "Gives error for zero data length");

    // Test with null pointer
    // result = fill_Hex_Pattern_In_Buffer(hexPattern, NULL, sizeof(buffer));
    // TEST_ASSERT(result == BAD_PARAMETER, "Gives error for null pointer");
}

static void test_fill_Incrementing_Pattern_In_Buffer(void) {
    // Test with valid parameters
    uint8_t buffer[10];
    uint8_t startValue = 0x00;
    eReturnValues result = fill_Incrementing_Pattern_In_Buffer(startValue, buffer, sizeof(buffer));
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS for valid parameters");
    printf("Buffer after fill_Incrementing_Pattern_In_Buffer: ");
    for (size_t i = 0; i < sizeof(buffer); i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");

    // Test with zero data length
    result = fill_Incrementing_Pattern_In_Buffer(startValue, buffer, 0);
    printf("Result for zero data length: %d\n", result);

    // Test with null pointer
    result = fill_Incrementing_Pattern_In_Buffer(startValue, NULL, sizeof(buffer));
    TEST_ASSERT(result == BAD_PARAMETER, "Returns BAD_PARAMETER for null pointer");
}

static void test_fill_ASCII_Pattern_In_Buffer(void) {
    // Test with valid parameters
    uint8_t buffer[20];
    const char* asciiPattern = "Hello";
    eReturnValues result = fill_ASCII_Pattern_In_Buffer(asciiPattern, strlen(asciiPattern), buffer, sizeof(buffer));
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS for valid parameters");
    printf("Buffer after fill_ASCII_Pattern_In_Buffer: ");
    for (size_t i = 0; i < sizeof(buffer); i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");

    // Test with zero data length
    result = fill_ASCII_Pattern_In_Buffer(asciiPattern, strlen(asciiPattern), buffer, 0);
    printf("Result for zero data length: %d\n", result);

    // Test with null pointer
//     result = fill_ASCII_Pattern_In_Buffer(asciiPattern, strlen(asciiPattern), NULL, sizeof(buffer));
//     TEST_ASSERT(result == BAD_PARAMETER, "Returns BAD_PARAMETER for null pointer");
}

void run_pattern_utils_tests(void) {
    test_fill_Random_Pattern_In_Buffer();
    test_fill_Hex_Pattern_In_Buffer();
    test_fill_Incrementing_Pattern_In_Buffer();
    test_fill_ASCII_Pattern_In_Buffer();
}