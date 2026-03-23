#include"../framework/test_framework.h"
#include"../../include/io_utils.h"
#include"../testConstants.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define SUCCESS 0
#define FAILURE 1
#define EFI_SUCCESS 0

typedef unsigned long long UINTN;
typedef int EFI_STATUS;

//console color constants
#define EFI_BLACK        0
#define EFI_BLUE         1
#define EFI_GREEN        2
#define EFI_CYAN         3
#define EFI_RED          4
#define EFI_MAGENTA      5
#define EFI_BROWN        6
#define EFI_LIGHTGRAY    7
#define EFI_DARKGRAY     8
#define EFI_LIGHTBLUE    9
#define EFI_LIGHTGREEN   10
#define EFI_LIGHTCYAN    11
#define EFI_LIGHTRED     12
#define EFI_LIGHTMAGENTA 13
#define EFI_YELLOW       14
#define EFI_WHITE        15

#define EFI_TEXT_ATTR(fg, bg) ((fg) | ((bg) << 4))

#define M_Nibble0(x) ((x) & 0xF)
#define M_Nibble1(x) (((x) >> 4) & 0xF)

#define C_CAST(type, val) ((type)(val))

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
    TEST_ASSERT(testFile != NULL, "Test file opened successfully");

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
    TEST_ASSERT(testFile != NULL, "Test file opened successfully");

    ssize_t len = getdelim(&buffer, &size, '.', testFile);

    TEST_ASSERT(len != -1, "Read line from file successfully");
    TEST_ASSERT(strcmp(buffer, "This is a test line.") == 0,
                "Line read matches expected content");

    fclose(testFile);
    free(buffer);
}

static void test_asprintf(void) {
    char* str;
    int result = asprintf(&str, "Hello, %s!", "world");
    TEST_ASSERT(result != -1, "asprintf succeeded");
    TEST_ASSERT(strcmp(str, "Hello, world!") == 0, "asprintf produced expected string");
    free(str);
}

static int test_vasprintf_wrapper(char **out, const char *fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = vasprintf(out, fmt, args);
    va_end(args);

    return ret;
}

static void test_vasprintf(void) {
    char* str = NULL;
    
    int result = test_vasprintf_wrapper(&str, "Hello, %s! It's a %s", "world", "beautiful day");
    TEST_ASSERT(result != -1, "vasprintf succeeded");
    TEST_ASSERT(strcmp(str, "Hello, world! It's a beautiful day") == 0, "vasprintf produced expected string");
    free(str);
}

static void test_snprintf(void) {
    char buffer[50] = {0};
    int result = snprintf(buffer, sizeof(buffer), "Hello, %s!", "world");
    TEST_ASSERT(result >= 0 && result < sizeof(buffer), "snprintf succeeded and did not truncate");
    TEST_ASSERT(strcmp(buffer, "Hello, world!") == 0, "snprintf produced expected string");
}

static int test_snprintf_wrapper(char *buffer, size_t size, const char *fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = vsnprintf(buffer, size, fmt, args);
    va_end(args);

    return ret;
}

static void test_vsnprintf(void) {
    char buffer[50] = {0};
    int result = test_snprintf_wrapper(buffer, sizeof(buffer), "Hello, %s! It's a %s", "world", "beautiful day");
    TEST_ASSERT(result >= 0 && result < sizeof(buffer), "vsnprintf succeeded and did not truncate");
    TEST_ASSERT(strcmp(buffer, "Hello, world! It's a beautiful day") == 0, "vsnprintf produced expected string");
}

// Handles error correctly - calls abort_handler
static void test_snprintf_err_handle(void) {
    char buffer[10] = {0};
    int result = snprintf_err_handle(buffer, sizeof(buffer), "Hello, %s!", "world");
    TEST_ASSERT(result >= 0, "snprintf_err_handle succeeded");
    TEST_ASSERT(result >= sizeof(buffer), "snprintf_err_handle correctly identified truncation");
    TEST_ASSERT(strcmp(buffer, "Hello, wo") == 0, "snprintf_err_handle produced truncated string as expected");
}

static int test_verify_Format_String_And_Args_wrapper(const char* fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);
    ret = verify_Format_String_And_Args(fmt, args);
    va_end(args);

    return ret;
}

static void test_verify_Format_String_And_Args(void)
{
    const char* formatString = "Hello, %s!";
    
    int result = test_verify_Format_String_And_Args_wrapper(formatString, "world");
    TEST_ASSERT(result != -1, "Format string and arguments are valid");

    int result2 = test_verify_Format_String_And_Args_wrapper("Hello %s", NULL);
    TEST_ASSERT(result2 == -1, "Returns -1 for invalid argument");
}

// function test_get_Secure_User_Input added to file test_secure_inputs.c as 
// it is an interactive test function and requires user input from terminal

typedef struct {
    UINTN Attribute;
} SIMPLE_TEXT_OUTPUT_MODE;

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    EFI_STATUS (*SetAttribute)(
        struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
        UINTN Attribute);
    SIMPLE_TEXT_OUTPUT_MODE *Mode;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

static UINTN last_attribute = 0;
static int set_attribute_called = 0;

EFI_STATUS MockSetAttribute(
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
    UINTN Attribute)
{
    last_attribute = Attribute;
    set_attribute_called++;
    // Update protocol state
    This->Mode->Attribute = Attribute;

    return EFI_SUCCESS;
}

static SIMPLE_TEXT_OUTPUT_MODE mockMode = {
    .Attribute = EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK)
};

static EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL mockProtocol = {
    .SetAttribute = MockSetAttribute,
    .Mode = &mockMode
};

int get_Simple_Text_Output_Protocol_Ptr(void **ptr)
{
    *ptr = &mockProtocol;
    return SUCCESS;
}

void close_Simple_Text_Output_Protocol_Ptr(void **ptr)
{
    *ptr = NULL;
}

UINTN get_Default_Console_Colors(void)
{
    return EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK);
}

typedef struct
{
    eConsoleColors color;
    uint8_t expectedEFIColor;
} ColorTestCase;

// Skip for now as it is complicated to test
static void test_set_Console_Colors(void) {
    ColorTestCase tests[] =
    {
        { CONSOLE_COLOR_BLUE,  EFI_LIGHTBLUE },
        { CONSOLE_COLOR_GREEN, EFI_LIGHTGREEN },
        { CONSOLE_COLOR_RED,   EFI_LIGHTRED },
        { CONSOLE_COLOR_GRAY,  EFI_LIGHTGRAY }
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++)
    {
        /* ---- Test Foreground ---- */
        set_attribute_called = 0;

        set_Console_Colors(true, tests[i].color);
        printf("set_attribute_called for fg: %d\n", set_attribute_called);

        uint8_t fg = M_Nibble0(last_attribute);

        printf("fg: %u, expected: %u\n", fg, tests[i].expectedEFIColor);
        TEST_ASSERT_EQ(fg,
                       tests[i].expectedEFIColor,
                       "Foreground color mismatch");

        /* ---- Test Background ---- */
        set_attribute_called = 0;

        set_Console_Colors(false, tests[i].color);
        printf("set_attribute_called for bg: %d\n", set_attribute_called);

        uint8_t bg = M_Nibble1(last_attribute);

        printf("bg: %u, expected: %u\n", bg, tests[i].expectedEFIColor);
        TEST_ASSERT_EQ(bg,
                       tests[i].expectedEFIColor,
                       "Background color mismatch");
    }
}

static void test_print_Data_Buffer(void) {
    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x09};

    FILE *fp = fopen("output.txt", "w+");
    if (!fp)
    {
        perror("fopen failed");
        return;
    }

    /* Save original stdout */
    int saved_stdout = dup(fileno(stdout));

    /* Redirect stdout to file */
    dup2(fileno(fp), fileno(stdout));

    print_Data_Buffer(data, sizeof(data), true);

    fflush(stdout);

    /* Close the file */
    fclose(fp);

    /* Restore stdout BEFORE reading */
    dup2(saved_stdout, fileno(stdout));
    close(saved_stdout);

    /* Reopen the file for reading */
    fp = fopen("output.txt", "r");
    if (!fp)
    {
        perror("fopen failed");
        return;
    }

    fseek(fp, 0, SEEK_SET);

    char buffer[512] = {0};
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[n] = '\0';

    TEST_ASSERT(strstr(buffer, "DE AD BE EF 09") != NULL, "Hex bytes printed correctly");

    TEST_ASSERT(strstr(buffer, ".....") != NULL, "Non-printable character representation printed");

    printf("Captured output:\n%s\n", buffer);

    fclose(fp);
}

static void test_print_Pipe_Data(void) {
    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x09};
    print_Pipe_Data(data, sizeof(data));
}

static void test_print_Return_Enum(void) {
    char *input = NULL;
    size_t len = 0;

    eReturnValues ret = get_Secure_User_Input("Enter a number: ", &input, &len);

    print_Return_Enum("get_Secure_User_Input", ret);
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
    test_asprintf();
    test_vasprintf();
    test_snprintf();
    test_vsnprintf();
    // test_snprintf_err_handle(); needs to be commented
    test_verify_Format_String_And_Args();
    // test_set_Console_Colors(); Skip for now as it is complicated to test
    test_print_Data_Buffer();
    // test_print_Pipe_Data();
    test_print_Return_Enum();
}
