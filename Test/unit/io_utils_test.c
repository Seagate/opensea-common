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
    // TEST_ASSERT(get_And_Validate_Integer_Input_Uint8("20 f", &unit, ALLOW_UNIT_TEMPERATURE, &outputInteger), "Converted string to integer successfully");
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
    TEST_ASSERT(!get_And_Validate_Integer_Input_UI("4294967296", NULL, ALLOW_UNIT_NONE, &outputInteger),"Value larger than UINT_MAX should fail");
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

    // unittype - ALLOW_UNIT_DATASIZE
    TEST_ASSERT(get_And_Validate_Float_Input("123B", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123KB", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123KiB", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123MB", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123MiB", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123GB", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123GiB", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123TB", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123TiB", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123BLOCKS", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123SECTORS", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123", &unit, ALLOW_UNIT_DATASIZE, &outputFloat), "Converted string to float successfully");

    // unittype - ALLOW_UNIT_SECTOR_TYPE
    TEST_ASSERT(get_And_Validate_Float_Input("5p", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("5l", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("5logical", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("5physical", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("5", &unit, ALLOW_UNIT_SECTOR_TYPE, &outputFloat), "Converted string to float successfully");

    // unittype - ALLOW_UNIT_TIME
    TEST_ASSERT(get_And_Validate_Float_Input("12ns", &unit, ALLOW_UNIT_TIME, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("12us", &unit, ALLOW_UNIT_TIME, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("12ms", &unit, ALLOW_UNIT_TIME, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("12s", &unit, ALLOW_UNIT_TIME, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("12m", &unit, ALLOW_UNIT_TIME, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("12h", &unit, ALLOW_UNIT_TIME, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("12", &unit, ALLOW_UNIT_TIME, &outputFloat), "Converted string to float successfully");

    // unittype - ALLOW_UNIT_POWER
    TEST_ASSERT(get_And_Validate_Float_Input("123w", &unit, ALLOW_UNIT_POWER, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123mw", &unit, ALLOW_UNIT_POWER, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("123", &unit, ALLOW_UNIT_POWER, &outputFloat), "Converted string to float successfully");

    // unittype - ALLOW_UNIT_VOLTS
    TEST_ASSERT(get_And_Validate_Float_Input("12v", &unit, ALLOW_UNIT_VOLTS, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("12mv", &unit, ALLOW_UNIT_VOLTS, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("12", &unit, ALLOW_UNIT_VOLTS, &outputFloat), "Converted string to float successfully");

    TEST_ASSERT(get_And_Validate_Float_Input("20.34", NULL, ALLOW_UNIT_NONE, &outputFloat), "Converted string to float successfully");
    TEST_ASSERT(get_And_Validate_Float_Input("20.34f", &unit, ALLOW_UNIT_TEMPERATURE, &outputFloat), "Converted string to float successfully");
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

/* Handles error correctly - calls abort_handler
static void test_snprintf_err_handle(void) {
    char buffer[10] = {0};
    int result = snprintf_err_handle(buffer, sizeof(buffer), "Hello, %s!", "world");
    TEST_ASSERT(result >= 0, "snprintf_err_handle succeeded");
    TEST_ASSERT(result >= sizeof(buffer), "snprintf_err_handle correctly identified truncation");
    TEST_ASSERT(strcmp(buffer, "Hello, wo") == 0, "snprintf_err_handle produced truncated string as expected");
} */

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

/* function test_get_Secure_User_Input added to file test_secure_inputs.c as 
it is an interactive test function and requires user input from terminal

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

Skip for now as it is complicated to test
The test can differ as the OS or the terminal changes. Can be tested for windows.

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
        //Test Foreground 
        set_attribute_called = 0;

        set_Console_Colors(true, tests[i].color);
        printf("set_attribute_called for fg: %d\n", set_attribute_called);

        uint8_t fg = M_Nibble0(last_attribute);

        printf("fg: %u, expected: %u\n", fg, tests[i].expectedEFIColor);
        TEST_ASSERT_EQ(fg,
                       tests[i].expectedEFIColor,
                       "Foreground color mismatch");

        //Test Background
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
*/

static void test_print_Data_Buffer(void) {
    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x09};

    FILE *fp = fopen("output.txt", "w+");

    fflush(stdout);                      

    int saved_stdout = dup(fileno(stdout));
    dup2(fileno(fp), fileno(stdout));

    print_Data_Buffer(data, sizeof(data), true);

    fflush(stdout);                     

    dup2(saved_stdout, fileno(stdout));
    close(saved_stdout);

    fflush(stdout);                   

    fseek(fp, 0, SEEK_SET);

    char buffer[512] = {0};
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[n] = '\0';

    printf("Captured output:\n%s\n", buffer);

    TEST_ASSERT(strstr(buffer, "DE AD BE EF 09") != NULL, "Hex bytes printed correctly");
    TEST_ASSERT(strstr(buffer, ".....") != NULL, "Non-printable character representation printed");

    fclose(fp);
}

static void test_print_Pipe_Data(void)
{
    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};

    FILE *fp = fopen("pipe_output.bin", "w+");

    fflush(stdout);

    int saved_stdout = dup(fileno(stdout));
    dup2(fileno(fp), fileno(stdout));

    print_Pipe_Data(data, sizeof(data));

    fflush(stdout);

    dup2(saved_stdout, fileno(stdout));
    close(saved_stdout);

    fflush(stdout);

    fseek(fp, 0, SEEK_SET);

    char buffer[256] = {0};
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[n] = '\0';

    printf("Captured output:\n%s\n", buffer);

    TEST_ASSERT(strstr(buffer, "DE AD BE EF") != NULL,
                "Pipe data printed correctly");

    fclose(fp);
}

static void test_print_Return_Enum(void) {
    char *input = NULL;
    size_t len = 0;

    FILE *fp = fopen("pipe_output.bin", "w+");

    fflush(stdout);

    int saved_stdout = dup(fileno(stdout));
    dup2(fileno(fp), fileno(stdout));

    eReturnValues ret = get_Secure_User_Input("Enter a number: ", &input, &len);

    print_Return_Enum("get_Secure_User_Input", ret);

    fflush(stdout);

    dup2(saved_stdout, fileno(stdout));
    close(saved_stdout);

    fflush(stdout);

    fseek(fp, 0, SEEK_SET);
    char buffer[256] = {0};
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[n] = '\0';

    printf("Captured output:\n%s\n", buffer);

    TEST_ASSERT(strstr(buffer, "get_Secure_User_Input returning: FAILURE") != NULL, "Return enum printed correctly");
    fclose(fp);
}

static void test_flush_stdout(void) {
    FILE *fp = fopen("flush_output.txt", "w+");

    fflush(stdout);

    int saved_stdout = dup(fileno(stdout));
    dup2(fileno(fp), fileno(stdout));

    printf("This is a test for flush_stdout.");
    flush_stdout();

    fflush(stdout);

    dup2(saved_stdout, fileno(stdout));
    close(saved_stdout);

    fflush(stdout);

    fseek(fp, 0, SEEK_SET);

    char buffer[256] = {0};
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[n] = '\0';

    printf("Captured output:\n%s\n", buffer);

    TEST_ASSERT(strstr(buffer, "This is a test for flush_stdout.") != NULL, "flush_stdout flushed the output correctly");

    fclose(fp);
}

static void test_flush_stderr(void) {
    FILE *fp = fopen("flush_stderr_output.txt", "w+");

    fflush(stderr);

    int saved_stderr = dup(fileno(stderr));
    dup2(fileno(fp), fileno(stderr));

    fprintf(stderr, "This is a test for flush_stderr.");
    flush_stderr();

    fflush(stderr);

    dup2(saved_stderr, fileno(stderr));
    close(saved_stderr);

    fflush(stderr);

    fseek(fp, 0, SEEK_SET);

    char buffer[256] = {0};
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[n] = '\0';

    printf("Captured output:\n%s\n", buffer);

    TEST_ASSERT(strstr(buffer, "This is a test for flush_stderr.") != NULL, "flush_stderr flushed the output correctly");

    fclose(fp);
}

static void test_safe_fopen(void) {
    FILE* file;
    errno_t err = safe_fopen(&file,"test_safe_fopen.txt", "w");
    TEST_ASSERT(file != NULL, "safe_fopen opened the file successfully");
    fprintf(file, "Testing safe_fopen.\n");
    fclose(file);

    err = safe_fopen(&file, "test_safe_fopen.txt", "r");
    TEST_ASSERT(file != NULL, "safe_fopen opened the file successfully for reading");

    char buffer[256] = {0};
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[n] = '\0';

    printf("Read from file:\n%s\n", buffer);
    TEST_ASSERT(strstr(buffer, "Testing safe_fopen.") != NULL, "safe_fopen read the correct content");

    fclose(file);
}

static void test_safe_freopen(void)
{
    printf("Testing safe_freopen. This should not be captured in the file.\n");

    FILE *file = NULL;

    int saved_stdout = dup(fileno(stdout));

    errno_t err = safe_freopen(&file, "test_safe_freopen.txt", "w", stdout);

    TEST_ASSERT(err == 0, "safe_freopen returned success");
    TEST_ASSERT(file != NULL, "safe_freopen redirected stdout successfully");

    printf("Testing safe_freopen.\n");
    fflush(stdout);

    dup2(saved_stdout, fileno(stdout));
    close(saved_stdout);

    fflush(stdout);

    char buffer[256] = {0};
    FILE *readFile = fopen("test_safe_freopen.txt", "r");

    TEST_ASSERT(readFile != NULL, "output file opened");

    size_t n = fread(buffer, 1, sizeof(buffer) - 1, readFile);
    buffer[n] = '\0';

    printf("Read from file:\n%s\n", buffer);

    TEST_ASSERT(strstr(buffer, "Testing safe_freopen.") != NULL, "safe_freopen wrote the correct content to the file");

    fclose(readFile);
}

static void test_safe_tmpfile(void) {
    FILE* file;
    errno_t err = safe_tmpfile(&file);
    TEST_ASSERT(file != NULL, "safe_tmpfile created a temporary file successfully");
    fprintf(file, "Testing safe_tmpfile.\n");
    fflush(file);

    fseek(file, 0, SEEK_SET);

    char buffer[256] = {0};
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, file);
    buffer[n] = '\0';

    printf("Read from temporary file:\n%s\n", buffer);
    TEST_ASSERT(strstr(buffer, "Testing safe_tmpfile.") != NULL, "safe_tmpfile wrote the correct content to the temporary file");

    fclose(file);
}

static void test_safe_gets(void)
{
    char buffer[64] = {0};

    FILE *fp = fopen("test_input.txt", "w");
    TEST_ASSERT(fp != NULL, "input file created");

    fprintf(fp, "hello world\n");
    fclose(fp);

    fp = fopen("test_input.txt", "r");
    TEST_ASSERT(fp != NULL, "input file opened");

    int saved_stdin = dup(fileno(stdin));

    dup2(fileno(fp), fileno(stdin));

    char *res = safe_gets(buffer, sizeof(buffer));

    dup2(saved_stdin, fileno(stdin));
    close(saved_stdin);

    fclose(fp);

    printf("Captured input: %s\n", buffer);

    TEST_ASSERT(res != NULL, "safe_gets returned non-null");

    TEST_ASSERT(strcmp(buffer, "hello world") == 0,
                "safe_gets read correct string");
}

static void test_safe_strtol(void) {
    long result;
    char *endptr;
    errno_t err = safe_strtol(&result, "12345", &endptr, 10);
    TEST_ASSERT(result == 12345, "safe_strtol converted string to long correctly for base 10");
    TEST_ASSERT(*endptr == '\0', "safe_strtol consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtol did not set errno for valid input");

    err = safe_strtol(&result, "0xFF", &endptr, 0);
    TEST_ASSERT(result == 255, "safe_strtol converted string to long correctly for base 0 (hexadecimal)");
    TEST_ASSERT(*endptr == '\0', "safe_strtol consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtol did not set errno for valid input");

    err = safe_strtol(&result, "10101", &endptr, 2);
    TEST_ASSERT(result == 21, "safe_strtol converted string to long correctly for base 2");
    TEST_ASSERT(*endptr == '\0', "safe_strtol consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtol did not set errno for valid input");

    err = safe_strtol(&result, "17", &endptr, 8);
    TEST_ASSERT(result == 15, "safe_strtol converted string to long correctly for base 8");
    TEST_ASSERT(*endptr == '\0', "safe_strtol consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtol did not set errno for valid input");

    err = safe_strtol(&result, "FF", &endptr, 16);
    TEST_ASSERT(result == 255, "safe_strtol converted string to long correctly for base 16");
    TEST_ASSERT(*endptr == '\0', "safe_strtol consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtol did not set errno for valid input");

    err = safe_strtol(&result, "V", &endptr, 32);
    TEST_ASSERT(result == 31, "safe_strtol converted string to long correctly for base 32");
    TEST_ASSERT(*endptr == '\0', "safe_strtol consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtol did not set errno for valid input");

    err = safe_strtol(&result, "   -42abc", &endptr, 10);
    TEST_ASSERT(result == -42, "safe_strtol converted string to long correctly with leading whitespace and sign");
    TEST_ASSERT(strcmp(endptr, "abc") == 0, "safe_strtol set endptr to the correct position");
    TEST_ASSERT(errno == 0, "safe_strtol did not set errno for valid input");

    err = safe_strtol(&result, "99999999999999999999", &endptr, 10);
    TEST_ASSERT(result == LONG_MAX, "safe_strtol returned LONG_MAX for overflow");
    TEST_ASSERT(errno == ERANGE, "safe_strtol set errno to ERANGE for overflow");

    err = safe_strtol(&result, "-99999999999999999999", &endptr, 10);
    TEST_ASSERT(result == LONG_MIN, "safe_strtol returned LONG_MIN for underflow");
    TEST_ASSERT(errno == ERANGE, "safe_strtol set errno to ERANGE for underflow");

    err = safe_strtol(&result, "abc", &endptr, 10);
    TEST_ASSERT(result == 0, "safe_strtol returned 0 for invalid input");
    TEST_ASSERT(endptr == NULL || *endptr == 'a', "safe_strtol set endptr correctly for invalid input");
    TEST_ASSERT(errno == EINVAL || errno == 0, "safe_strtol set errno to EINVAL or left it unchanged for invalid input");
}

static void test_safe_strtoll(void) {
    long long result;
    char *endptr;
    errno_t err = safe_strtoll(&result, "12345", &endptr, 10);
    TEST_ASSERT(result == 12345, "safe_strtoll converted string to long long correctly for base 10");
    TEST_ASSERT(*endptr == '\0', "safe_strtoll consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoll did not set errno for valid input");

    err = safe_strtoll(&result, "0xFFFF", &endptr, 0);
    TEST_ASSERT(result == 65535, "safe_strtoll converted string to long long correctly for base 0 (hexadecimal)");
    TEST_ASSERT(*endptr == '\0', "safe_strtoll consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoll did not set errno for valid input");

    err = safe_strtoll(&result, "10101", &endptr, 2);
    TEST_ASSERT(result == 21, "safe_strtoll converted string to long long correctly for base 2");
    TEST_ASSERT(*endptr == '\0', "safe_strtoll consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoll did not set errno for valid input");

    err = safe_strtoll(&result, "17", &endptr, 8);
    TEST_ASSERT(result == 15, "safe_strtoll converted string to long long correctly for base 8");
    TEST_ASSERT(*endptr == '\0', "safe_strtoll consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoll did not set errno for valid input");

    err = safe_strtoll(&result, "FFFF", &endptr, 16);
    TEST_ASSERT(result == 65535, "safe_strtoll converted string to long long correctly for base 16");
    TEST_ASSERT(*endptr == '\0', "safe_strtoll consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoll did not set errno for valid input");

    err = safe_strtoll(&result, "V", &endptr, 32);
    TEST_ASSERT(result == 31, "safe_strtoll converted string to long long correctly for base 32");
    TEST_ASSERT(*endptr == '\0', "safe_strtoll consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoll did not set errno for valid input");

    err = safe_strtoll(&result, "   -42abc", &endptr, 10);
    TEST_ASSERT(result == -42, "safe_strtoll converted string to long long correctly with leading whitespace and sign");
    TEST_ASSERT(strcmp(endptr, "abc") == 0, "safe_strtoll set endptr to the correct position");
    TEST_ASSERT(errno == 0, "safe_strtoll did not set errno for valid input");
}

static void test_safe_strtoul(void) {
    unsigned long result;
    char *endptr;
    errno_t err = safe_strtoul(&result, "12345", &endptr, 10);
    TEST_ASSERT(result == 12345, "safe_strtoul converted string to unsigned long correctly for base 10");
    TEST_ASSERT(*endptr == '\0', "safe_strtoul consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoul did not set errno for valid input");

    err = safe_strtoul(&result, "0xFFFF", &endptr, 0);
    TEST_ASSERT(result == 65535, "safe_strtoul converted string to unsigned long correctly for base 0 (hexadecimal)");
    TEST_ASSERT(*endptr == '\0', "safe_strtoul consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoul did not set errno for valid input");

    err = safe_strtoul(&result, "10101", &endptr, 2);
    TEST_ASSERT(result == 21, "safe_strtoul converted string to unsigned long correctly for base 2");
    TEST_ASSERT(*endptr == '\0', "safe_strtoul consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoul did not set errno for valid input");

    err = safe_strtoul(&result, "17", &endptr, 8);
    TEST_ASSERT(result == 15, "safe_strtoul converted string to unsigned long correctly for base 8");
    TEST_ASSERT(*endptr == '\0', "safe_strtoul consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoul did not set errno for valid input");

    err = safe_strtoul(&result, "FFFF", &endptr, 16);
    TEST_ASSERT(result == 65535, "safe_strtoul converted string to unsigned long correctly for base 16");
    TEST_ASSERT(*endptr == '\0', "safe_strtoul consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoul did not set errno for valid input");

    err = safe_strtoul(&result, "V", &endptr, 32);
    TEST_ASSERT(result == 31, "safe_strtoul converted string to unsigned long correctly for base 32");
    TEST_ASSERT(*endptr == '\0', "safe_strtoul consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoul did not set errno for valid input");

    err = safe_strtoul(&result, "   -42abc", &endptr, 10);
    TEST_ASSERT(strcmp(endptr, "abc") == 0, "safe_strtoul set endptr to the correct position");
    TEST_ASSERT(errno == 0, "safe_strtoul did not set errno for valid input");
}

static void test_safe_strtoull(void) {
    unsigned long long result;
    char *endptr;
    errno_t err = safe_strtoull(&result, "12345", &endptr, 10);
    TEST_ASSERT(result == 12345, "safe_strtoull converted string to unsigned long long correctly for base 10");
    TEST_ASSERT(*endptr == '\0', "safe_strtoull consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoull did not set errno for valid input");

    err = safe_strtoull(&result, "0xFFFFFFFF", &endptr, 0);
    TEST_ASSERT(result == 4294967295ULL, "safe_strtoull converted string to unsigned long long correctly for base 0 (hexadecimal)");
    TEST_ASSERT(*endptr == '\0', "safe_strtoull consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoull did not set errno for valid input");

    err = safe_strtoull(&result, "10101", &endptr, 2);
    TEST_ASSERT(result == 21, "safe_strtoull converted string to unsigned long long correctly for base 2");
    TEST_ASSERT(*endptr == '\0', "safe_strtoull consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoull did not set errno for valid input");

    err = safe_strtoull(&result, "17", &endptr, 8);
    TEST_ASSERT(result == 15, "safe_strtoull converted string to unsigned long long correctly for base 8");
    TEST_ASSERT(*endptr == '\0', "safe_strtoull consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoull did not set errno for valid input");

    err = safe_strtoull(&result, "FFFFFFFF", &endptr, 16);
    TEST_ASSERT(result == 4294967295ULL, "safe_strtoull converted string to unsigned long long correctly for base 16");
    TEST_ASSERT(*endptr == '\0', "safe_strtoull consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoull did not set errno for valid input");

    err = safe_strtoull(&result, "V", &endptr, 32);
    TEST_ASSERT(result == 31, "safe_strtoull converted string to unsigned long long correctly for base 32");
    TEST_ASSERT(*endptr == '\0', "safe_strtoull consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoull did not set errno for valid input"); 
}

static void test_safe_strtoimax(void) {
    intmax_t result;
    char *endptr;
    errno_t err = safe_strtoimax(&result, "12345", &endptr, 10);
    TEST_ASSERT(result == 12345, "safe_strtoimax converted string to intmax_t correctly for base 10");
    TEST_ASSERT(*endptr == '\0', "safe_strtoimax consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoimax did not set errno for valid input");

    err = safe_strtoimax(&result, "0x7FFFFFFFFFFFFFFF", &endptr, 0);
    TEST_ASSERT(result == INTMAX_MAX, "safe_strtoimax converted string to intmax_t correctly for base 0 (hexadecimal)");
    TEST_ASSERT(*endptr == '\0', "safe_strtoimax consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoimax did not set errno for valid input");

    err = safe_strtoimax(&result, "-99999999999999999999", &endptr, 10);
    TEST_ASSERT(result == INTMAX_MIN, "safe_strtoimax returned INTMAX_MIN for underflow");
    TEST_ASSERT(errno == ERANGE, "safe_strtoimax set errno to ERANGE for underflow");

    err = safe_strtoimax(&result, "abc", &endptr, 10);
    TEST_ASSERT(err == EINVAL, "safe_strtoimax returned EINVAL for invalid input");
    TEST_ASSERT(endptr == NULL || *endptr == 'a', "safe_strtoimax set endptr correctly for invalid input");
    TEST_ASSERT(errno == EINVAL || errno == 0, "safe_strtoimax set errno to EINVAL or left it unchanged for invalid input");
}

static void test_safe_strtoumax(void) {
    uintmax_t result;
    char *endptr;
    errno_t err = safe_strtoumax(&result, "12345", &endptr, 10);
    TEST_ASSERT(result == 12345, "safe_strtoumax converted string to uintmax_t correctly for base 10");
    TEST_ASSERT(*endptr == '\0', "safe_strtoumax consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoumax did not set errno for valid input");

    err = safe_strtoumax(&result, "0xFFFFFFFFFFFFFFFF", &endptr, 0);
    TEST_ASSERT(result == UINTMAX_MAX, "safe_strtoumax converted string to uintmax_t correctly for base 0 (hexadecimal)");
    TEST_ASSERT(*endptr == '\0', "safe_strtoumax consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtoumax did not set errno for valid input");

    err = safe_strtoumax(&result, "abc", &endptr, 10);
    TEST_ASSERT(err == EINVAL, "safe_strtoumax returned EINVAL for invalid input");
    TEST_ASSERT(endptr == NULL || *endptr == 'a', "safe_strtoumax set endptr correctly for invalid input");
    TEST_ASSERT(errno == EINVAL || errno == 0, "safe_strtoumax set errno to EINVAL or left it unchanged for invalid input");
}

static void test_safe_strtof(void) {
    float result;
    char *endptr;
    errno_t err = safe_strtof(&result, "123.45", &endptr);
    TEST_ASSERT(result == 123.45f, "safe_strtof converted string to float correctly");
    TEST_ASSERT(*endptr == '\0', "safe_strtof consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtof did not set errno for valid input");

    err = safe_strtof(&result, "12", &endptr);
    TEST_ASSERT(result == 12.0f, "safe_strtof converted string to float correctly");
    TEST_ASSERT(*endptr == '\0', "safe_strtof consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtof did not set errno for valid input");

    err = safe_strtof(&result, "1e-10", &endptr);
    TEST_ASSERT(result == 1e-10f, "safe_strtof converted string to float correctly for scientific notation");
    TEST_ASSERT(*endptr == '\0', "safe_strtof consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtof did not set errno for valid input");

    err = safe_strtof(&result, "abc", &endptr);
    TEST_ASSERT(err == EINVAL, "safe_strtof returned EINVAL for invalid input");
    TEST_ASSERT(endptr == NULL || *endptr == 'a', "safe_strtof set endptr correctly for invalid input");
    TEST_ASSERT(errno == EINVAL || errno == 0, "safe_strtof set errno to EINVAL or left it unchanged for invalid input");
}

static void test_safe_strtod(void) {
    double result;
    char *endptr;
    errno_t err = safe_strtod(&result, "123.45", &endptr);
    TEST_ASSERT(result == 123.45, "safe_strtod converted string to double correctly");
    TEST_ASSERT(*endptr == '\0', "safe_strtod consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtod did not set errno for valid input");

    err = safe_strtod(&result, "12", &endptr);
    TEST_ASSERT(result == 12.0, "safe_strtod converted string to double correctly");
    TEST_ASSERT(*endptr == '\0', "safe_strtod consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtod did not set errno for valid input");

    err = safe_strtod(&result, "1e-10", &endptr);
    TEST_ASSERT(result == 1e-10, "safe_strtod converted string to double correctly for scientific notation");
    TEST_ASSERT(*endptr == '\0', "safe_strtod consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtod did not set errno for valid input");

    err = safe_strtod(&result, "abc", &endptr);
    TEST_ASSERT(err == EINVAL, "safe_strtod returned EINVAL for invalid input");
    TEST_ASSERT(endptr == NULL || *endptr == 'a', "safe_strtod set endptr correctly for invalid input");
    TEST_ASSERT(errno == EINVAL || errno == 0, "safe_strtod set errno to EINVAL or left it unchanged for invalid input");
}

static void test_safe_strtold(void) {
    long double result;
    char *endptr;
    errno_t err = safe_strtold(&result, "123.45", &endptr);
    TEST_ASSERT(result == 123.45L, "safe_strtold converted string to long double correctly");
    TEST_ASSERT(*endptr == '\0', "safe_strtold consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtold did not set errno for valid input");

    err = safe_strtold(&result, "12", &endptr);
    TEST_ASSERT(result == 12.0L, "safe_strtold converted string to long double correctly");
    TEST_ASSERT(*endptr == '\0', "safe_strtold consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtold did not set errno for valid input");

    err = safe_strtold(&result, "1e-10", &endptr);
    TEST_ASSERT(result == 1e-10L, "safe_strtold converted string to long double correctly for scientific notation");
    TEST_ASSERT(*endptr == '\0', "safe_strtold consumed the entire string");
    TEST_ASSERT(errno == 0, "safe_strtold did not set errno for valid input");

    err = safe_strtold(&result, "abc", &endptr);
    TEST_ASSERT(err == EINVAL, "safe_strtold returned EINVAL for invalid input");
    TEST_ASSERT(endptr == NULL || *endptr == 'a', "safe_strtold set endptr correctly for invalid input");
    TEST_ASSERT(errno == EINVAL || errno == 0, "safe_strtold set errno to EINVAL or left it unchanged for invalid input");
}

// Value is set to 0 on error
static void test_safe_atoi(void) {
    int result;
    errno_t err = safe_atoi(&result, "12345");
    TEST_ASSERT(result == 12345, "safe_atoi converted string to int correctly");
    TEST_ASSERT(errno == 0, "safe_atoi did not set errno for valid input");

    err = safe_atoi(&result, "   -42abc");
    TEST_ASSERT(result == 0, "safe_atoi converted string to int correctly with leading whitespace and sign");
    TEST_ASSERT(errno == EINVAL, "safe_atoi set errno for invalid input");

    err = safe_atoi(&result, "99999999999999999999");
    TEST_ASSERT(result == 0, "safe_atoi returned INT_MAX for overflow");
    TEST_ASSERT(errno == ERANGE, "safe_atoi set errno to ERANGE for overflow");

    err = safe_atoi(&result, "-99999999999999999999");
    TEST_ASSERT(result == 0, "safe_atoi returned INT_MIN for underflow");
    TEST_ASSERT(errno == ERANGE, "safe_atoi set errno to ERANGE for underflow");
}

static void test_safe_atol(void) {
    long result;
    errno_t err = safe_atol(&result, "12345");
    TEST_ASSERT(result == 12345, "safe_atol converted string to long correctly");
    TEST_ASSERT(errno == 0, "safe_atol did not set errno for valid input");

    err = safe_atol(&result, "   -42abc");
    TEST_ASSERT(result == 0, "safe_atol returns 0 for string with leading whitespace and sign");
    TEST_ASSERT(errno == EINVAL, "safe_atol set errno for invalid input");

    err = safe_atol(&result, "99999999999999999999");
    TEST_ASSERT(result == LONG_MAX, "safe_atol returned LONG_MAX for overflow");
    TEST_ASSERT(errno == ERANGE, "safe_atol set errno to ERANGE for overflow");

    err = safe_atol(&result, "-99999999999999999999");
    TEST_ASSERT(result == LONG_MIN, "safe_atol returned LONG_MIN for underflow");
    TEST_ASSERT(errno == ERANGE, "safe_atol set errno to ERANGE for underflow");
}

static void test_safe_atoll(void) {
    long long result;
    errno_t err = safe_atoll(&result, "12345");
    TEST_ASSERT(result == 12345, "safe_atoll converted string to long long correctly");
    TEST_ASSERT(errno == 0, "safe_atoll did not set errno for valid input");

    err = safe_atoll(&result, "   -42abc");
    TEST_ASSERT(result == 0, "safe_atoll returns 0 for string with leading whitespace and sign");
    TEST_ASSERT(errno == EINVAL, "safe_atoll set errno for invalid input");

    err = safe_atoll(&result, "99999999999999999999");
    TEST_ASSERT(result == LLONG_MAX, "safe_atoll returned LLONG_MAX for overflow");
    TEST_ASSERT(errno == ERANGE, "safe_atoll set errno to ERANGE for overflow");

    err = safe_atoll(&result, "-99999999999999999999");
    TEST_ASSERT(result == LLONG_MIN, "safe_atoll returned LLONG_MIN for underflow");
    TEST_ASSERT(errno == ERANGE, "safe_atoll set errno to ERANGE for underflow");
}

static void test_safe_atof(void) {
    double result;
    errno_t err = safe_atof(&result, "123.45");
    TEST_ASSERT(result == 123.45, "safe_atof converted string to double correctly");
    TEST_ASSERT(errno == 0, "safe_atof did not set errno for valid input");

    err = safe_atof(&result, "12");
    TEST_ASSERT(result == 12.0, "safe_atof converted string to double correctly");
    TEST_ASSERT(errno == 0, "safe_atof did not set errno for valid input");

    err = safe_atof(&result, "1e-10");
    TEST_ASSERT(result == 1e-10, "safe_atof converted string to double correctly for scientific notation");
    TEST_ASSERT(errno == 0, "safe_atof did not set errno for valid input");

    err = safe_atof(&result, "abc");
    TEST_ASSERT(err == EINVAL, "safe_atof returned EINVAL for invalid input");
    TEST_ASSERT(errno == EINVAL || errno == 0, "safe_atof set errno to EINVAL or left it unchanged for invalid input");
}

static void test_checked_fputs(void) {
    FILE *fp = fopen("test_checked_fputs.txt", "w");
    TEST_ASSERT(fp != NULL, "File opened successfully for checked_fputs test");

    errno_t err = checked_fputs("Hello, World!\n", fp);
    TEST_ASSERT(err == 0, "checked_fputs wrote to file successfully");

    fclose(fp);

    fp = fopen("test_checked_fputs.txt", "r");
    TEST_ASSERT(fp != NULL, "File opened successfully for reading in checked_fputs test");

    char buffer[256] = {0};
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[n] = '\0';

    printf("Read from file:\n%s\n", buffer);
    TEST_ASSERT(strstr(buffer, "Hello, World!") != NULL, "checked_fputs wrote the correct content to the file");

    fclose(fp);
}

static void test_print_str(void) {
    char *testStr = "This is a test string.";
    FILE *fp = fopen("test_print_str.txt", "w+");
    TEST_ASSERT(fp != NULL, "File opened successfully for test_print_str");

    fflush(stdout);

    int saved_stdout = dup(fileno(stdout));
    dup2(fileno(fp), fileno(stdout));

    print_str(testStr);

    fflush(stdout);

    dup2(saved_stdout, fileno(stdout));
    close(saved_stdout);

    fflush(stdout);

    fseek(fp, 0, SEEK_SET);

    char buffer[256] = {0};
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, fp);
    buffer[n] = '\0';

    printf("Captured output:\n%s\n", buffer);
    TEST_ASSERT(strstr(buffer, testStr) != NULL, "print_str printed the correct string");

    fclose(fp);
}

void run_io_utils_tests(void) {
    test_get_And_Validate_Integer_Input();
    test_get_And_Validate_Integer_Input_Uint64();
    test_get_And_Validate_Integer_Input_Uint32();
    test_get_And_Validate_Integer_Input_Uint16();
    test_get_And_Validate_Integer_Input_Uint8();
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
    test_print_Pipe_Data();
    test_print_Return_Enum();
    test_flush_stdout();
    test_flush_stderr();
    test_safe_fopen();
    test_safe_freopen();
    test_safe_tmpfile();
    test_safe_gets();
    test_safe_strtol();
    test_safe_strtoll();
    test_safe_strtoul();
    test_safe_strtoull();
    test_safe_strtoimax();
    test_safe_strtoumax();
    test_safe_strtof();
    test_safe_strtod();
    test_safe_strtold();
    test_safe_atoi();
    test_safe_atol();
    test_safe_atoll();
    test_safe_atof();
    test_checked_fputs();
    test_print_str();
}
