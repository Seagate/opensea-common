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
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint8("20 f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    printf("outputInteger: %u\n", outputInteger);
    TEST_ASSERT(get_And_Validate_Integer_Input_Uint8("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint8("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint8("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Uint8("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_Int64(void) {
    int64_t outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_Int64("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int64("5p", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int64("5f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int64("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int64("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int64("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int64("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_Int32(void) {
    int32_t outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_Int32("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int32("5p", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int32("5f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int32("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int32("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int32("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int32("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_Int16(void) {
    int16_t outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_Int16("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int16("5p", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int16("5f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int16("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int16("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int16("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int16("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_Int8(void) {
    int8_t outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_Int8("0x7F", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int8("5p", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int8("5f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_Int8("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int8("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int8("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_Int8("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_ULL(void) {
    unsigned long long outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_ULL("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_ULL("123mw", &unit, ALLOW_UNIT_POWER, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_ULL("12mv", &unit, ALLOW_UNIT_VOLTS, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_ULL("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_ULL("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_ULL("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_UL(void) {
    unsigned long outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_UL("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_UL("123mw", &unit, ALLOW_UNIT_POWER, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_UL("12mv", &unit, ALLOW_UNIT_VOLTS, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_UL("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_UL("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_UL("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_UI(void) {
    unsigned int outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_UI("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_UI("123w", &unit, ALLOW_UNIT_POWER, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_UI("12v", &unit, ALLOW_UNIT_VOLTS, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_UI("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_UI("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_UI("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_US(void) {
    unsigned short outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_US("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_US("12a", &unit, ALLOW_UNIT_AMPS, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_US("12v", &unit, ALLOW_UNIT_VOLTS, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_US("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_US("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_US("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_UC(void) {
    unsigned char outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_UC("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_UC("12w", &unit, ALLOW_UNIT_POWER, &outputInteger), "Converted string to integer successfully");
    // TEST_ASSERT(get_And_Validate_Integer_Input_UC("20a", &unit, ALLOW_UNIT_AMPS, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_UC("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_UC("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_UC("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_LL(void) {
    long long outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_LL("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_LL("5p", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_LL("50f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_LL("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_LL("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_LL("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_LL("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_L(void) {
    long outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_L("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_L("5p", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_L("50f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_L("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_L("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_L("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_L("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_I(void) {
    int outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_I("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_I("5p", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_I("50f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_I("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_I("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_I("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_I("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_S(void) {
    short outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_S("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_S("5p", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_S("5f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_S("12ms", &unit, ALLOW_UNIT_TIME, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_S("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_S("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_S("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Integer_Input_C(void) {
    char outputInteger;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Integer_Input_C("0x7F", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_C("5a", &unit, ALLOW_UNIT_AMPS, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_And_Validate_Integer_Input_C("20v", &unit, ALLOW_UNIT_VOLTS, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_C("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_C("123KB", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
    TEST_ASSERT(!get_And_Validate_Integer_Input_C("12.5", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_get_And_Validate_Float_Input(void) {
    float outputFloat;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Float_Input("12.34", NULL, ALLOW_UNIT_NONE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("20.34f", &unit, ALLOW_UNIT_TEMPERATURE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("56.78ms", &unit, ALLOW_UNIT_TIME, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(!get_And_Validate_Float_Input("xyz", NULL, ALLOW_UNIT_NONE, &outputFloat), "Could not convert string to float successfully");
    TEST_ASSERT(!get_And_Validate_Float_Input("123KB", &unit, ALLOW_UNIT_NONE, &outputFloat), "Could not convert string to float successfully");
}

static void test_get_And_Validate_Double_Input(void) {
    double outputFloat;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_Double_Input("20.34f", &unit, ALLOW_UNIT_TEMPERATURE, &outputFloat), "Converted string to double successfully");
    TEST_ASSERT(get_And_Validate_Double_Input("12.34", NULL, ALLOW_UNIT_NONE, &outputFloat), "Converted string to double successfully");
    TEST_ASSERT(get_And_Validate_Double_Input("56.78ms", &unit, ALLOW_UNIT_TIME, &outputFloat), "Converted string to double successfully");
    TEST_ASSERT(!get_And_Validate_Double_Input("xyz", NULL, ALLOW_UNIT_NONE, &outputFloat), "Could not convert string to double successfully");
    TEST_ASSERT(!get_And_Validate_Double_Input("123KB", &unit, ALLOW_UNIT_NONE, &outputFloat), "Could not convert string to double successfully");
}

static void test_get_And_Validate_LDouble_Input(void) {
    long double outputFloat;
    char* unit = NULL;
    TEST_ASSERT(get_And_Validate_LDouble_Input("20.34f", &unit, ALLOW_UNIT_TEMPERATURE, &outputFloat), "Converted string to long double successfully");
    TEST_ASSERT(get_And_Validate_LDouble_Input("12.34", NULL, ALLOW_UNIT_NONE, &outputFloat), "Converted string to long double successfully");
    TEST_ASSERT(get_And_Validate_LDouble_Input("56.78ms", &unit, ALLOW_UNIT_TIME, &outputFloat), "Converted string to long double successfully");
    TEST_ASSERT(!get_And_Validate_LDouble_Input("xyz", NULL, ALLOW_UNIT_NONE, &outputFloat), "Could not convert string to long double successfully");
    TEST_ASSERT(!get_And_Validate_LDouble_Input("123KB", &unit, ALLOW_UNIT_NONE, &outputFloat), "Could not convert string to long double successfully");
}

static void test_get_Valid_Integer_Input(void) {
    uint64_t outputInteger;
    float outputFloat;
    TEST_ASSERT(get_Valid_Integer_Input("12", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_Valid_Integer_Input("12345789", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_Valid_Integer_Input("0xFF", NULL, ALLOW_UNIT_NONE, &outputInteger), "Converted string to integer successfully");
    TEST_ASSERT(get_Valid_Integer_Input("12.5", NULL, ALLOW_UNIT_NONE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(!get_Valid_Integer_Input("xyz", NULL, ALLOW_UNIT_NONE, &outputInteger), "Could not convert string to integer successfully");
}

static void test_getline(void) {
    char *buffer = NULL;
    size_t size = 0;

    FILE* testFile = fopen("test_input.txt", "w");
    fprintf(testFile, "This is a test line.\n");
    fclose(testFile);

    testFile = fopen("test_input.txt", "r");
    TEST_ASSERT(testFile != NULL, "Failed to open test file");

    ssize_t len = getline(&buffer, &size, testFile);

    TEST_ASSERT(len != -1, "Read line from file successfully");
    TEST_ASSERT(strcmp(buffer, "This is a test line.\n") == 0,
                "Line read matches expected content");

    fclose(testFile);
    free(buffer);
}

static void test_getdelim(void) {
    char *buffer = NULL;
    size_t size = 0;

    FILE* testFile = fopen("test_input.txt", "w");
    fprintf(testFile, "This is a test line. This is another test line.\n");
    fclose(testFile);

    testFile = fopen("test_input.txt", "r");
    TEST_ASSERT(testFile != NULL, "Failed to open test file");

    ssize_t len = getdelim(&buffer, &size, '.', testFile);

    TEST_ASSERT(len != -1, "Read line from file successfully");
    TEST_ASSERT(strcmp(buffer, "This is a test line.") == 0,
                "Line read matches expected content");

    fclose(testFile);
    free(buffer);
}

void run_io_utils_tests(void) {
    test_get_And_Validate_Integer_Input();
    test_get_And_Validate_Integer_Input_Uint64();
    test_get_And_Validate_Integer_Input_Uint32();
    test_get_And_Validate_Integer_Input_Uint16();
    // test_get_And_Validate_Integer_Input_Uint8();
    test_get_And_Validate_Integer_Input_Int64();
    test_get_And_Validate_Integer_Input_Int32();
    test_get_And_Validate_Integer_Input_Int16();
    test_get_And_Validate_Integer_Input_Int8();
    test_get_And_Validate_Integer_Input_ULL();
    test_get_And_Validate_Integer_Input_UL();
    test_get_And_Validate_Integer_Input_UI();
    test_get_And_Validate_Integer_Input_US();
    test_get_And_Validate_Integer_Input_UC();
    test_get_And_Validate_Integer_Input_LL();
    test_get_And_Validate_Integer_Input_L();
    test_get_And_Validate_Integer_Input_I();
    test_get_And_Validate_Integer_Input_S();    
    test_get_And_Validate_Integer_Input_C();
    test_get_And_Validate_Float_Input();
    test_get_And_Validate_Double_Input();
    test_get_And_Validate_LDouble_Input();
    test_get_Valid_Integer_Input();
    test_getline();
    test_getdelim();
}
