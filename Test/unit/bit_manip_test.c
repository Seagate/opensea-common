#include "../framework/test_framework.h"
#include "../../include/bit_manip.h"
#include <math.h>
#include "../testConstants.h"

static void test_get_DWord0(void) {
    TEST_ASSERT_EQ(get_DWord0(SERIAL_NUM), LOWER4BYTE, "get lower 32 bits of uint64_t");
}

static void test_get_DWord1(void) {
    TEST_ASSERT_EQ(get_DWord1(SERIAL_NUM), HIGHER4BYTE, "get upper 32 bits of uint64_t");
}

static void test_M_DoubleWordInt0(void) {
    TEST_ASSERT_EQ(M_DoubleWordInt0(SERIAL_NUM), (int32_t)(LOWER4BYTE), "get lower 32 bits of uint64_t");
}

static void test_M_DoubleWordInt1(void) {
    TEST_ASSERT_EQ(M_DoubleWordInt1(SERIAL_NUM), (int32_t)(HIGHER4BYTE), "get upper 32 bits of uint64_t");
}

static void test_get_Word0_uint64(void) {
    TEST_ASSERT_EQ(get_Word0_uint64(SERIAL_NUM), LOWER2BYTE, "get lower 16 bits of uint64_t");
}

static void test_get_Word0_uint32(void) {
    TEST_ASSERT_EQ(get_Word0_uint32(LOWER4BYTE), LOWER2BYTE, "get lower 16 bits of uint32_t");
}

static void test_M_Word0(void) {
    TEST_ASSERT_EQ(M_Word0((uint64_t)SERIAL_NUM), LOWER2BYTE, "get lower 16 bits of uint64_t");
}

// 0x00000000FFFF0000
static void test_get_Word1_uint64(void) {
    TEST_ASSERT_EQ(get_Word1_uint64(SERIAL_NUM), (uint16_t)((LOWER4BYTE >> 16) & 0xFFFF) , "get the second lowest 16 bits of uint64_t");
}

static void test_get_Word1_uint32(void) {
    TEST_ASSERT_EQ(get_Word1_uint32(LOWER4BYTE), (uint16_t)((LOWER4BYTE >> 16) & 0xFFFF) , "get higher 16 bits of uint32_t");
}

static void test_M_Word1(void) {
    TEST_ASSERT_EQ(M_Word1((uint64_t)SERIAL_NUM), (uint16_t)((LOWER4BYTE >> 16) & 0xFFFF) , "get the second lowest 16 bits of uint64_t");
}

// 0x0000FFFF00000000
static void test_get_Word2_uint64(void) {
    TEST_ASSERT_EQ(get_Word2_uint64(SERIAL_NUM), (uint16_t)(HIGHER4BYTE & 0xFFFF), "get second highest 16 bits of uint64_t");
}

static void test_M_Word2(void) {
    TEST_ASSERT_EQ(M_Word2((uint64_t)SERIAL_NUM), (uint16_t)(HIGHER4BYTE & 0xFFFF), "get second highest 16 bits of uint64_t");
}

// 0xFFFF000000000000
static void test_get_Word3_uint64(void) {
    TEST_ASSERT_EQ(get_Word3_uint64(SERIAL_NUM), HIGHER2BYTE, "get highest 16 bits of uint64_t");
}

static void test_M_Word3(void) {
    TEST_ASSERT_EQ(M_Word3((uint64_t)SERIAL_NUM), HIGHER2BYTE, "get highest 16 bits of uint64_t");
}

static void test_M_WordInt0(void) {
    TEST_ASSERT_EQ(M_WordInt0((uint64_t)SERIAL_NUM), (int16_t)(LOWER2BYTE), "get lower 16 bits of uint64_t");
}

static void test_M_WordInt1(void) {
    TEST_ASSERT_EQ(M_WordInt1((uint64_t)SERIAL_NUM), (int16_t)((LOWER4BYTE >> 16) & 0xFFFF), "get the second lowest 16 bits of uint64_t");
}

static void test_M_WordInt2(void) {
    TEST_ASSERT_EQ(M_WordInt2((uint64_t)SERIAL_NUM), (int16_t)(HIGHER4BYTE & 0xFFFF), "get second highest 16 bits of uint64_t");
}

static void test_M_WordInt3(void) {
    TEST_ASSERT_EQ(M_WordInt3((uint64_t)SERIAL_NUM), (int16_t)(HIGHER2BYTE), "get highest 16 bits of uint64_t as int16_t");
}

static void test_BITSPERBYTE(void) {
    TEST_ASSERT_EQ(BITSPERBYTE, 8, "BITSPERBYTE should be 8");
}

// UINT8_MAX = 0xFF
static void test_M_ByteN(void) {
    for (int n = 0; n < 8; n++) {
        char msg[50];
        snprintf(msg, sizeof(msg), "Set %dth byte to all 1s", n);
        TEST_ASSERT_EQ(M_ByteN(n), ((uint64_t)UINT8_MAX << (n * BITSPERBYTE)), msg);
    }
}

static void test_M_Byte0(void) {
    TEST_ASSERT_EQ(M_Byte0(SERIAL_NUM), (uint8_t)0xEF, "Extracts the lowest byte from a 64-bit integer and casts it to uint8_t");
}

static void test_M_Byte1(void) {
    TEST_ASSERT_EQ(M_Byte1(SERIAL_NUM), (uint8_t)0xCD, "Extracts the second lowest byte from a 64-bit integer and casts it to uint8_t");
}

static void test_M_Byte2(void) {
    TEST_ASSERT_EQ(M_Byte2(SERIAL_NUM), (uint8_t)0xAB, "Extracts the third lowest byte from a 64-bit integer and casts it to uint8_t");
}

static void test_M_Byte3(void) {
    TEST_ASSERT_EQ(M_Byte3(SERIAL_NUM), (uint8_t)0x90, "Extracts the fourth lowest byte from a 64-bit integer and casts it to uint8_t");
}

static void test_M_Byte4(void) {
    TEST_ASSERT_EQ(M_Byte4(SERIAL_NUM), (uint8_t)0x78, "Extracts the fifth lowest byte from a 64-bit integer and casts it to uint8_t");
}

static void test_M_Byte5(void) {
    TEST_ASSERT_EQ(M_Byte5(SERIAL_NUM), (uint8_t)0x56, "Extracts the sixth lowest byte from a 64-bit integer and casts it to uint8_t");
}

static void test_M_Byte6(void) {
    TEST_ASSERT_EQ(M_Byte6(SERIAL_NUM), (uint8_t)0x34, "Extracts the seventh lowest byte from a 64-bit integer and casts it to uint8_t");
}

static void test_M_Byte7(void) {
    TEST_ASSERT_EQ(M_Byte7(SERIAL_NUM), (uint8_t)0x12, "Extracts the highest byte from a 64-bit integer and casts it to uint8_t");
}

typedef struct {
    uint64_t input;
    int8_t expected;
    const char *description;
} M_ByteIntN_TestCase;

static void test_M_ByteInt0(void) {
    M_ByteIntN_TestCase cases[] = {
        {0x0000000000000000ULL, (int8_t)0x00, "Lowest byte 0x00 → 0"},
        {0x0000000000000001ULL, (int8_t)0x01, "Lowest byte 0x01 → 1"},
        {0x000000000000007FULL, (int8_t)0x7F, "Lowest byte 0x7F → 127"},
        {0x00000000000000FFULL, (int8_t)0xFF, "Lowest byte 0xFF → -1"},
        {0x00000000000000A5ULL, (int8_t)0xA5, "Lowest byte 0xA5 → -91"},
        {0x1234567890ABCDEFULL, (int8_t)0xEF, "Lowest byte 0xEF → -17"}
    };

    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
        TEST_ASSERT_EQ(
            M_ByteInt0(cases[i].input),
            cases[i].expected,
            cases[i].description
        );
    }
}


static void test_M_ByteInt1(void) {
    M_ByteIntN_TestCase cases[] = {
        {0x0000000000000000ULL, (int8_t)0x00, "Second lowest byte 0x00 → 0"},
        {0x0000000000000100ULL, (int8_t)0x01, "Second lowest byte 0x01 → 1"},
        {0x0000000000007F00ULL, (int8_t)0x7F, "Second lowest byte 0x7F → 127"},
        {0x000000000000FF00ULL, (int8_t)0xFF, "Second lowest byte 0xFF → -1"},
        {0x000000000000A500ULL, (int8_t)0xA5, "Second lowest byte 0xA5 → -91"},
        {0x1234567890ABCDEFULL, (int8_t)0xCD, "Second lowest byte 0xCD → -51"}
    };


    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
        TEST_ASSERT_EQ(
            M_ByteInt1(cases[i].input),
            cases[i].expected,
            cases[i].description
        );
    }
}

static void test_M_ByteInt2(void) {
    M_ByteIntN_TestCase cases[] = {
        {0x0000000000000000ULL, (int8_t)0x00, "Third lowest byte 0x00 → 0"},
        {0x0000000000010000ULL, (int8_t)0x01, "Third lowest byte 0x01 → 1"},
        {0x00000000007F0000ULL, (int8_t)0x7F, "Third lowest byte 0x7F → 127"},
        {0x0000000000FF0000ULL, (int8_t)0xFF, "Third lowest byte 0xFF → -1"},
        {0x0000000000A50000ULL, (int8_t)0xA5, "Third lowest byte 0xA5 → -91"},
        {0x1234567890ABCDEFULL, (int8_t)0xAB, "Third lowest byte 0xAB → -85"}
    };


    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
        TEST_ASSERT_EQ(
            M_ByteInt2(cases[i].input),
            cases[i].expected,
            cases[i].description
        );
    }
}

static void test_M_ByteInt3(void) {
    M_ByteIntN_TestCase cases[] = {
        {0x0000000000000000ULL, (int8_t)0x00, "Fourth lowest byte 0x00 → 0"},
        {0x0000000001000000ULL, (int8_t)0x01, "Fourth lowest byte 0x01 → 1"},
        {0x000000007F000000ULL, (int8_t)0x7F, "Fourth lowest byte 0x7F → 127"},
        {0x00000000FF000000ULL, (int8_t)0xFF, "Fourth lowest byte 0xFF → -1"},
        {0x00000000A5000000ULL, (int8_t)0xA5, "Fourth lowest byte 0xA5 → -91"},
        {0x1234567890ABCDEFULL, (int8_t)0x90, "Fourth lowest byte 0x90 → -112"}
    };


    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
        TEST_ASSERT_EQ(
            M_ByteInt3(cases[i].input),
            cases[i].expected,
            cases[i].description
        );
    }
}

static void test_M_ByteInt4(void) {
    M_ByteIntN_TestCase cases[] = {
        {0x0000000000000000ULL, (int8_t)0x00, "Fifth lowest byte 0x00 → 0"},
        {0x0000000100000000ULL, (int8_t)0x01, "Fifth lowest byte 0x01 → 1"},
        {0x0000007F00000000ULL, (int8_t)0x7F, "Fifth lowest byte 0x7F → 127"},
        {0x000000FF00000000ULL, (int8_t)0xFF, "Fifth lowest byte 0xFF → -1"},
        {0x000000A500000000ULL, (int8_t)0xA5, "Fifth lowest byte 0xA5 → -91"},
        {0x1234567890ABCDEFULL, (int8_t)0x78, "Fifth lowest byte 0x78 → 120"}
    };


    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
        TEST_ASSERT_EQ(
            M_ByteInt4(cases[i].input),
            cases[i].expected,
            cases[i].description
        );
    }
}

static void test_M_ByteInt5(void) {
    M_ByteIntN_TestCase cases[] = {
        {0x0000000000000000ULL, (int8_t)0x00, "Sixth lowest byte 0x00 → 0"},
        {0x0000010000000000ULL, (int8_t)0x01, "Sixth lowest byte 0x01 → 1"},
        {0x00007F0000000000ULL, (int8_t)0x7F, "Sixth lowest byte 0x7F → 127"},
        {0x0000FF0000000000ULL, (int8_t)0xFF, "Sixth lowest byte 0xFF → -1"},
        {0x0000A50000000000ULL, (int8_t)0xA5, "Sixth lowest byte 0xA5 → -91"},
        {0x1234567890ABCDEFULL, (int8_t)0x56, "Sixth lowest byte 0x56 → 86"}
    };


    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
        TEST_ASSERT_EQ(
            M_ByteInt5(cases[i].input),
            cases[i].expected,
            cases[i].description
        );
    }
}

static void test_M_ByteInt6(void) {
    M_ByteIntN_TestCase cases[] = {
        {0x0000000000000000ULL, (int8_t)0x00, "Seventh lowest byte 0x00 → 0"},
        {0x0001000000000000ULL, (int8_t)0x01, "Seventh lowest byte 0x01 → 1"},
        {0x007F000000000000ULL, (int8_t)0x7F, "Seventh lowest byte 0x7F → 127"},
        {0x00FF000000000000ULL, (int8_t)0xFF, "Seventh lowest byte 0xFF → -1"},
        {0x00A5000000000000ULL, (int8_t)0xA5, "Seventh lowest byte 0xA5 → -91"},
        {0x1234567890ABCDEFULL, (int8_t)0x34, "Seventh lowest byte 0x34 → 52"}
    };


    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
        TEST_ASSERT_EQ(
            M_ByteInt6(cases[i].input),
            cases[i].expected,
            cases[i].description
        );
    }
}

static void test_M_ByteInt7(void) {
    M_ByteIntN_TestCase cases[] = {
        {0x0000000000000000ULL, (int8_t)0x00, "Eighth lowest byte 0x00 → 0"},
        {0x0100000000000000ULL, (int8_t)0x01, "Eighth lowest byte 0x01 → 1"},
        {0x7F00000000000000ULL, (int8_t)0x7F, "Eighth lowest byte 0x7F → 127"},
        {0xFF00000000000000ULL, (int8_t)0xFF, "Eighth lowest byte 0xFF → -1"},
        {0xA500000000000000ULL, (int8_t)0xA5, "Eighth lowest byte 0xA5 → -91"},
        {0x1234567890ABCDEFULL, (int8_t)0x12, "Eighth lowest byte 0x12 → 18"}
    };


    for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
        TEST_ASSERT_EQ(
            M_ByteInt7(cases[i].input),
            cases[i].expected,
            cases[i].description
        );
    }
}

static void test_M_Nibble0(void) {
    TEST_ASSERT_EQ(M_Nibble0((uint64_t)SERIAL_NUM), (uint8_t)(SERIAL_NUM & 0xF), "Extracts the lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble1(void) {
    TEST_ASSERT_EQ(M_Nibble1((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 4) & 0xF), "Extracts the second lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble2(void) {
    TEST_ASSERT_EQ(M_Nibble2((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 8) & 0xF), "Extracts the third lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble3(void) {
    TEST_ASSERT_EQ(M_Nibble3((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 12) & 0xF), "Extracts the fourth lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble4(void) {
    TEST_ASSERT_EQ(M_Nibble4((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 16) & 0xF), "Extracts the fifth lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble5(void) {
    TEST_ASSERT_EQ(M_Nibble5((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 20) & 0xF), "Extracts the sixth lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble6(void) {
    TEST_ASSERT_EQ(M_Nibble6((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 24) & 0xF), "Extracts the seventh lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble7(void) {
    TEST_ASSERT_EQ(M_Nibble7((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 28) & 0xF), "Extracts the eighth lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble8(void) {
    TEST_ASSERT_EQ(M_Nibble8((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 32) & 0xF), "Extracts the ninth lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble9(void) {
    TEST_ASSERT_EQ(M_Nibble9((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 36) & 0xF), "Extracts the tenth lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble10(void) {
    TEST_ASSERT_EQ(M_Nibble10((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 40) & 0xF), "Extracts the eleventh lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble11(void) {
    TEST_ASSERT_EQ(M_Nibble11((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 44) & 0xF), "Extracts the twelfth lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble12(void) {
    TEST_ASSERT_EQ(M_Nibble12((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 48) & 0xF), "Extracts the thirteenth lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble13(void) {
    TEST_ASSERT_EQ(M_Nibble13((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 52) & 0xF), "Extracts the fourteenth lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble14(void) {
    TEST_ASSERT_EQ(M_Nibble14((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 56) & 0xF), "Extracts the fifteenth lowest nibble and casts it to uint8_t");
}

static void test_M_Nibble15(void) {
    TEST_ASSERT_EQ(M_Nibble15((uint64_t)SERIAL_NUM), (uint8_t)((SERIAL_NUM >> 60) & 0xF), "Extracts the highest nibble and casts it to uint8_t");
}

static void test_nibbles_To_Byte(void) {
    TEST_ASSERT_EQ(nibbles_To_Byte(NIBBLE1, NIBBLE0), 0xEF, "Combines 2 nibbles to form byte 0xEF");
}

static void test_M_NibblesTo1ByteValue(void) {
    TEST_ASSERT_EQ(M_NibblesTo1ByteValue(NIBBLE1, NIBBLE0), 0xEF, "Combines 2 nibbles to form byte 0xEF using M_NibblesTo1ByteValue");
}

static void test_bytes_To_Uint16(void) {
    TEST_ASSERT_EQ(bytes_To_Uint16(MSB, LSB), 0x12EF, "Combines 2 bytes to form uint16_t 0x12EF");
}

static void test_M_BytesTo2ByteValue(void) {
    TEST_ASSERT_EQ(M_BytesTo2ByteValue(MSB, LSB), 0x12EF, "Combines 2 bytes to form uint16_t 0x12EF using M_BytesTo2ByteValue");
}

static void test_bytes_To_Uint32(void) {
    TEST_ASSERT_EQ(bytes_To_Uint32(MSB, BYTE2, BYTE1, LSB), 0x12ABCDEF, "Combines 4 bytes to form uint32_t 0x12ABCDEF");
}

static void test_M_BytesTo4ByteValue(void) {
    TEST_ASSERT_EQ(M_BytesTo4ByteValue(MSB, BYTE2, BYTE1, LSB), 0x12ABCDEF, "Combines 4 bytes to form uint32_t 0x12ABCDEF using M_BytesTo4ByteValue");
}

static void test_bytes_To_Uint64(void) {
    TEST_ASSERT_EQ(bytes_To_Uint64(MSB, BYTE6, BYTE5, BYTE4, BYTE3, BYTE2, BYTE1, LSB),
                   0x1234567890ABCDEFULL,
                   "Combines 8 bytes to form uint64_t 0x1234567890ABCDEF");
}

static void test_M_BytesTo8ByteValue(void) {
    TEST_ASSERT_EQ(M_BytesTo8ByteValue(MSB, BYTE6, BYTE5, BYTE4, BYTE3, BYTE2, BYTE1, LSB),
                   0x1234567890ABCDEFULL,
                   "Combines 8 bytes to form uint64_t 0x1234567890ABCDEF using M_BytesTo8ByteValue");
}

static void test_words_To_Uint32(void) {
    TEST_ASSERT_EQ(words_To_Uint32(HIGHER2BYTE, LOWER2BYTE), 0x1234CDEF, "Combines 2 words to form uint32_t 0x1234CDEF");
}

static void test_M_WordsTo4ByteValue(void) {
    TEST_ASSERT_EQ(M_WordsTo4ByteValue(HIGHER2BYTE, LOWER2BYTE), 0xCDEF1234, "Combines 2 words to form uint32_t 0xCDEF1234 using M_WordsTo4ByteValue");
}

static void test_words_To_Uint64(void) {
    TEST_ASSERT_EQ(words_To_Uint64(HIGHER2BYTE, LOWER2BYTE, HIGHER2BYTE, LOWER2BYTE), 0x1234CDEF1234CDEFULL, "Combines 4 words to form uint64_t 0x1234CDEF1234CDEF");
}

static void test_M_WordsTo8ByteValue(void) {
    TEST_ASSERT_EQ(M_WordsTo8ByteValue(HIGHER2BYTE, LOWER2BYTE, HIGHER2BYTE, LOWER2BYTE), 0x1234CDEF1234CDEFULL, "Combines 4 words to form uint64_t 0x1234CDEF1234CDEF using M_WordsTo8ByteValue");
}

static void test_dwords_To_Uint64(void) {
    TEST_ASSERT_EQ(dwords_To_Uint64(HIGHER4BYTE, LOWER4BYTE), 0x1234567890ABCDEFULL, "Combines 2 double words to form uint64_t 0x1234567890ABCDEF");
}

static void test_M_DWordsTo8ByteValue(void) {
    TEST_ASSERT_EQ(M_DWordsTo8ByteValue(HIGHER4BYTE, LOWER4BYTE), 0x1234567890ABCDEFULL, "Combines 2 double words to form uint64_t 0x1234567890ABCDEF using M_DWordsTo8ByteValue");
}

static void test_ROUNDF(void) {
    TEST_ASSERT_FLOAT_EQ(ROUNDF(3.14159f, 100), 3.14f, 0.001f, "Rounds 3.14159 to 2 decimal places");
    TEST_ASSERT_FLOAT_EQ(ROUNDF(2.999f, 100), 3.00f, 0.001f, "Rounds 2.999 to 2 decimal places");
    TEST_ASSERT_FLOAT_EQ(ROUNDF(123.4561123f, 1000), 123.456f, 0.001f, "Rounds 123.456 to 3 decimal places");
    TEST_ASSERT_FLOAT_EQ(ROUNDF(-34.56f, 10), -34.6f, 0.001f, "Rounds 123.456 to 3 decimal places");
    TEST_ASSERT_FLOAT_EQ(ROUNDF(-126.113446f, 1000), -126.113f, 0.001f, "Rounds 123.456 to 3 decimal places");
}

struct sgenericint_t VALID_GENERIC_INT = {
    .issigned = true,
    .sizeoftype = sizeof(int32_t),
    .i32 = -100
};

struct sgenericint_t INVALID_GENERIC_INT_WITH_INVALID_SIZE = {
    .issigned = false,
    .sizeoftype = 12, // Invalid size
    .u32 = 0x12345678
};

// Only checks for valid and invalid sizeoftype
static void test_is_generic_int_valid(void) {
    TEST_ASSERT(is_generic_int_valid(VALID_GENERIC_INT), "VALID_GENERIC_INT should be valid");
    TEST_ASSERT(!is_generic_int_valid(INVALID_GENERIC_INT_WITH_INVALID_SIZE), "INVALID_GENERIC_INT_WITH_INVALID_SIZE should be invalid");
}

static void test_get_bit_range_uint8(void) {
    TEST_ASSERT_EQ(get_bit_range_uint8((uint8_t)HEX_RANDOM, 7, 4), (uint8_t)(0xF), "Extract bits 7 to 4 from 0xF0F0F0F0F0F0F0F0ULL");

    // Test for width > GENERIC_WIDTH_8 and lsb = 0
    TEST_ASSERT_EQ(get_bit_range_uint8((uint32_t)HEX_RANDOM, 7, 0), (uint8_t)HEX_RANDOM, "Extract bits 7 to 0 from 0xF0F0F0F0F0F0F0F0ULL");

    errno = 0;
    // Test for msb > GENERIC_INT_8BIT_MAX
    get_bit_range_uint8((uint8_t)HEX_RANDOM, 9, 4);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint8 should set errno to ERANGE when msb > 7");

    // Test for lsb > GENERIC_INT_8BIT_MAX
    get_bit_range_uint8((uint8_t)HEX_RANDOM, 7, 8);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint8 should set errno to ERANGE when lsb > 7");

    // Test for bitcount = 0
    errno = 0;
    get_bit_range_uint8((uint8_t)HEX_RANDOM, 5, 6);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint8 should set errno to ERANGE when bitcount = 0");

    // Test for bitcount > GENERIC_WIDTH_8 - Not possible
}

static void test_get_bit_range_uint16(void) {
    TEST_ASSERT_EQ(get_bit_range_uint16((uint16_t)HEX_RANDOM, 15, 8), (uint16_t)(0xF0), "Extract bits 15 to 8 from 0xF0F0F0F0F0F0F0F0ULL");

    // Test for width > GENERIC_WIDTH_16 and lsb = 0
    TEST_ASSERT_EQ(get_bit_range_uint16((uint32_t)HEX_RANDOM, 15, 0), (uint16_t)HEX_RANDOM, "Extract bits 15 to 0 from 0xF0F0F0F0F0F0F0F0ULL");

    errno = 0;
    // Test for msb > GENERIC_INT_16BIT_MAX
    get_bit_range_uint16((uint16_t)HEX_RANDOM, 16, 8);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint16 should set errno to ERANGE when msb > 15");

    // Test for lsb > GENERIC_INT_16BIT_MAX
    get_bit_range_uint16((uint16_t)HEX_RANDOM, 15, 16);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint16 should set errno to ERANGE when lsb > 15");

    // Test for bitcount = 0
    errno = 0;
    get_bit_range_uint16((uint16_t)HEX_RANDOM, 5, 6);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint16 should set errno to ERANGE when bitcount = 0");

    // Test for bitcount > GENERIC_WIDTH_16 - Not possible
}

static void test_get_8bit_range_uint16(void) {
    TEST_ASSERT_EQ(get_8bit_range_uint16((uint16_t)HEX_RANDOM, 7, 0), (uint8_t)(0xF0), "Extract bits 7 to 0 from 0xF0F0F0F0F0F0F0F0ULL");
}

static void test_get_bit_range_uint32(void) {
    TEST_ASSERT_EQ(get_bit_range_uint32((uint32_t)HEX_RANDOM, 7, 0), (uint32_t)(0xF0), "Extract bits 7 to 0 from 0xF0F0F0F0F0F0F0F0ULL");

    errno = 0;
    // Test for msb > GENERIC_INT_32BIT_MAX
    get_bit_range_uint32((uint32_t)HEX_RANDOM, 32, 8);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint32 should set errno to ERANGE when msb > 31");

    // Test for lsb > GENERIC_INT_32BIT_MAX
    get_bit_range_uint32((uint32_t)HEX_RANDOM, 31, 32);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint32 should set errno to ERANGE when lsb > 31");

    // Test for bitcount = 0
    errno = 0;
    get_bit_range_uint32((uint32_t)HEX_RANDOM, 5, 6);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint32 should set errno to ERANGE when bitcount = 0");

    // Test for width > GENERIC_WIDTH_32 and lsb = 0
    TEST_ASSERT_EQ(get_bit_range_uint32((uint32_t)HEX_RANDOM, 31, 0), (uint32_t)HEX_RANDOM, "Extract bits 31 to 0 from 0xF0F0F0F0F0F0F0F0ULL");

    // Test for bitcount > GENERIC_WIDTH_32 - Not possible
}

static void test_get_8bit_range_uint32(void) {
    TEST_ASSERT_EQ(get_8bit_range_uint32((uint32_t)HEX_RANDOM, 7, 0), (uint8_t)(0xF0), "Extract bits 7 to 0 from 0xF0F0F0F0F0F0F0F0ULL");

    // Test for bit_count > GENERIC_WIDTH_8
    errno = 0;
    get_8bit_range_uint32((uint32_t)HEX_RANDOM, 15, 0);
    TEST_ASSERT(errno == ERANGE, "get_8bit_range_uint32 should set errno to ERANGE when bit_count > 8");
}

static void test_get_16bit_range_uint32(void) {
    TEST_ASSERT_EQ(get_16bit_range_uint32((uint32_t)HEX_RANDOM, 15, 0), (uint16_t)(0xF0F0), "Extract bits 15 to 0 from 0xF0F0F0F0F0F0F0F0ULL");

    // Test for bit_count > GENERIC_WIDTH_16
    errno = 0;
    get_16bit_range_uint32((uint32_t)HEX_RANDOM, 31, 0);
    TEST_ASSERT(errno == ERANGE, "get_16bit_range_uint32 should set errno to ERANGE when bit_count > 16");
}

static void test_get_bit_range_uint64(void) {
    TEST_ASSERT_EQ(get_bit_range_uint64(HEX_RANDOM, 63, 32), (uint64_t)(0xF0F0F0F0), "Extract bits 63 to 32 from 0xF0F0F0F0F0F0F0F0ULL");

    errno = 0;
    // Test for msb > GENERIC_INT_64BIT_MAX
    get_bit_range_uint64(HEX_RANDOM, 64, 32);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint64 should set errno to ERANGE when msb > 63");

    // Test for lsb > GENERIC_INT_64BIT_MAX
    get_bit_range_uint64(HEX_RANDOM, 60, 64);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint64 should set errno to ERANGE when lsb > 63");

    // Test for bitcount = 0
    errno = 0;
    get_bit_range_uint64(HEX_RANDOM, 5, 6);
    TEST_ASSERT(errno == ERANGE, "get_bit_range_uint64 should set errno to ERANGE when bitcount = 0");

    // Test for bitcount > GENERIC_WIDTH_64 - Not possible
}

static void test_get_8bit_range_uint64(void) {
    TEST_ASSERT_EQ(get_8bit_range_uint64(HEX_RANDOM, 31, 24), (uint8_t)(0xF0), "Extract bits 31 to 24 from 0xF0F0F0F0F0F0F0F0ULL");

    // Test for bit_count > GENERIC_WIDTH_8
    errno = 0;
    get_8bit_range_uint64(HEX_RANDOM, 15, 0);
    TEST_ASSERT(errno == ERANGE, "get_8bit_range_uint64 should set errno to ERANGE when bit_count > 8");
}

static void test_get_16bit_range_uint64(void) {
    TEST_ASSERT_EQ(get_16bit_range_uint64(HEX_RANDOM, 39, 24), (uint16_t)(0xF0F0), "Extract bits 39 to 24 from 0xF0F0F0F0F0F0F0F0ULL");

    // Test for bit_count > GENERIC_WIDTH_16
    errno = 0;
    get_16bit_range_uint64(HEX_RANDOM, 31, 0);
    TEST_ASSERT(errno == ERANGE, "get_16bit_range_uint64 should set errno to ERANGE when bit_count > 16");
}

static void test_get_32bit_range_uint64(void) {
    TEST_ASSERT_EQ(get_32bit_range_uint64(HEX_RANDOM, 63, 32), (uint32_t)(0xF0F0F0F0), "Extract bits 63 to 32 from 0xF0F0F0F0F0F0F0F0ULL");

    // Test for bit_count > GENERIC_WIDTH_32
    errno = 0;
    get_32bit_range_uint64(HEX_RANDOM, 63, 0);
    TEST_ASSERT(errno == ERANGE, "get_32bit_range_uint64 should set errno to ERANGE when bit_count > 32");
}

static void test_get_bit_range_int8(void) {
    TEST_ASSERT_EQ(get_bit_range_int8((int8_t)(HEX_RANDOM), 7, 4), (int8_t)(0xF), "Extract bits 7 to 4 from int8_t 0xF0F0F0F0F0F0F0F0ULL");
}

static void test_get_bit_range_int16(void) {
    TEST_ASSERT_EQ(get_bit_range_int16((int16_t)(HEX_RANDOM), 15, 8), (int16_t)(0xF0), "Extract bits 15 to 8 from int16_t 0xF0F0F0F0F0F0F0F0ULL");
}

static void test_get_8bit_range_int16(void) {
    TEST_ASSERT_EQ(get_8bit_range_int16((int16_t)(HEX_RANDOM), 7, 0), (int8_t)(0xF0), "Extract bits 7 to 0 from int16_t 0xF0F0F0F0F0F0F0F0ULL");
}

static void test_get_bit_range_int32(void) {
    TEST_ASSERT_EQ(get_bit_range_int32((int32_t)(HEX_RANDOM), 7, 0), (int32_t)(0xF0), "Extract bits 7 to 0 from int32_t 0xF0F0F0F0F0F0F0F0ULL");
}

static void test_get_8bit_range_int32(void) {
    TEST_ASSERT_EQ(get_8bit_range_int32((int32_t)(HEX_RANDOM), 15, 8), (int8_t)(0xF0), "Extract bits 15 to 8 from int32_t 0xF0F0F0F0F0F0F0F0ULL");
}

static void test_get_16bit_range_int32(void) {
    TEST_ASSERT_EQ(get_16bit_range_int32((int32_t)(HEX_RANDOM), 31, 16), (int16_t)(0xF0F0), "Extract bits 31 to 16 from int32_t 0xF0F0F0F0F0F0F0F0ULL");
}

static void test_get_bit_range_int64(void) {
    TEST_ASSERT_EQ(get_bit_range_int64((int64_t)(HEX_RANDOM), 63, 32), (int64_t)(0xF0F0F0F0), "Extract bits 63 to 32 from int64_t 0xF0F0F0F0F0F0F0F0ULL");
    TEST_ASSERT_EQ(get_bit_range_int64((int64_t)0xFFF00000FFF00000ULL, 63, 0), (int64_t)(0xFFF00000FFF00000), "Extract bits 63 to 0 from int64_t 0xF0F0F0F0F0F0F0F0ULL"); // Check full range extraction
}

static void test_get_8bit_range_int64(void) {
    TEST_ASSERT_EQ(get_8bit_range_int64((int64_t)(HEX_RANDOM), 63, 56), (int8_t)(0xF0), "Extract bits 63 to 56 from int64_t 0xF0F0F0F0F0F0F0F0ULL");
}

static void test_get_16bit_range_int64(void) {
    TEST_ASSERT_EQ(get_16bit_range_int64((int64_t)(HEX_RANDOM), 63, 48), (int16_t)(0xF0F0), "Extract bits 63 to 48 from int64_t 0xF0F0F0F0F0F0F0F0ULL");
}

static void test_get_32bit_range_int64(void) {
    TEST_ASSERT_EQ(get_32bit_range_int64((int64_t)(HEX_RANDOM), 63, 32), (int32_t)(0xF0F0F0F0), "Extract bits 63 to 32 from int64_t 0xF0F0F0F0F0F0F0F0ULL");
}

static void test_M_GETBITRANGE(void) {
    TEST_ASSERT_EQ(M_GETBITRANGE(HEX_RANDOM, 7, 4), (uint8_t)(0xF), "Extract bits 7 to 4 from 0xF0F0F0F0F0F0F0F0ULL using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_GETBITRANGE(HEX_RANDOM, 15, 8), (uint8_t)(0xF0), "Extract bits 15 to 8 from 0xF0F0F0F0F0F0F0F0ULL using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_GETBITRANGE(HEX_RANDOM, 15, 0), (uint16_t)(0xF0F0), "Extract bits 15 to 0 from 0xF0F0F0F0F0F0F0F0ULL using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_GETBITRANGE(HEX_RANDOM, 31, 0), (uint32_t)(0xF0F0F0F0), "Extract bits 31 to 0 from 0xF0F0F0F0F0F0F0F0ULL using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_GETBITRANGE(HEX_RANDOM, 63, 0), (uint64_t)(HEX_RANDOM), "Extract bits 63 to 0 from 0xF0F0F0F0F0F0F0F0ULL using M_GETBITRANGE");
}

static void test_M_IGETBITRANGE(void) {
    TEST_ASSERT_EQ(M_IGETBITRANGE(HEX_RANDOM, 7, 4), (int8_t)(0xF), "Extract bits 7 to 4 from 0xF0F0F0F0F0F0F0F0ULL using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_IGETBITRANGE(HEX_RANDOM, 15, 8), (int8_t)(0xF0), "Extract bits 15 to 8 from 0xF0F0F0F0F0F0F0F0ULL using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_IGETBITRANGE(HEX_RANDOM, 15, 0), (int16_t)(0xF0F0), "Extract bits 15 to 0 from 0xF0F0F0F0F0F0F0F0ULL using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_IGETBITRANGE(HEX_RANDOM, 31, 0), (int32_t)(0xF0F0F0F0), "Extract bits 31 to 0 from 0xF0F0F0F0F0F0F0F0ULL using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_IGETBITRANGE(HEX_RANDOM, 63, 0), (int64_t)(HEX_RANDOM), "Extract bits 63 to 0 from 0xF0F0F0F0F0F0F0F0ULL using M_GETBITRANGE");
}

static void test_M_BitN(void) {
    for(uint64_t bit = 0; bit < 64; bit++) {
        
        uint64_t expected = (uint64_t)(1ULL << bit);

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %luth bit of uint64_t", bit);

        TEST_ASSERT_EQ(M_BitN(bit), expected, msg);
    }
}

static void test_M_BitN8(void) {
    for(uint8_t bit = 0; bit < 8; bit++) {
        
        uint8_t expected = (uint8_t)(1U << bit);

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %uth bit of uint8_t", bit);

        TEST_ASSERT_EQ(M_BitN8(bit), expected, msg);
    }
}

static void test_M_BitN16(void) {
    for(uint16_t bit = 0; bit < 16; bit++) {
        
        uint16_t expected = (uint16_t)(1U << bit);

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %uth bit of uint16_t", bit);

        TEST_ASSERT_EQ(M_BitN16(bit), expected, msg);
    }
}

static void test_M_BitN32(void) {
    for(uint32_t bit = 0; bit < 32; bit++) {
        
        uint32_t expected = (uint32_t)(1U << bit);

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %uth bit of uint32_t", bit);

        TEST_ASSERT_EQ(M_BitN32(bit), expected, msg);
    }
}

static void test_M_BitN64(void) {
    for(uint64_t bit = 0; bit < 64; bit++) {
        
        uint64_t expected = (uint64_t)(1ULL << bit);

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %luth bit of uint64_t", bit);

        TEST_ASSERT_EQ(M_BitN64(bit), expected, msg);
    }
}

static void test_BIT0(void) {
    TEST_ASSERT_EQ(BIT0, (uint8_t)1 << 0, "Set 0th bit of uint8_t");
}

static void test_BIT1(void) {
    TEST_ASSERT_EQ(BIT1, (uint8_t)1 << 1, "Set 1st bit of uint8_t");
}

static void test_BIT2(void) {
    TEST_ASSERT_EQ(BIT2, (uint8_t)1 << 2, "Set 2nd bit of uint8_t");
} 

static void test_BIT3(void) {
    TEST_ASSERT_EQ(BIT3, (uint8_t)1 << 3, "Set 3rd bit of uint8_t");
}

static void test_BIT4(void) {
    TEST_ASSERT_EQ(BIT4, (uint8_t)1 << 4, "Set 4th bit of uint8_t");
}

static void test_BIT5(void) {
    TEST_ASSERT_EQ(BIT5, (uint8_t)1 << 5, "Set 5th bit of uint8_t");
}

static void test_BIT6(void) {
    TEST_ASSERT_EQ(BIT6, (uint8_t)1 << 6, "Set 6th bit of uint8_t");
}

static void test_BIT7(void) {
    TEST_ASSERT_EQ(BIT7, (uint8_t)1 << 7, "Set 7th bit of uint8_t");
}

static void test_BIT8(void) {
    TEST_ASSERT_EQ(BIT8, (uint16_t)1 << 8, "Set 8th bit of uint16_t");
}

static void test_BIT9(void) {
    TEST_ASSERT_EQ(BIT9, (uint16_t)1 << 9, "Set 9th bit of uint16_t");
}

static void test_BIT10(void) {
    TEST_ASSERT_EQ(BIT10, (uint16_t)1 << 10, "Set 10th bit of uint16_t");
}

static void test_BIT11(void) {
    TEST_ASSERT_EQ(BIT11, (uint16_t)1 << 11, "Set 11th bit of uint16_t");
}

static void test_BIT12(void) {
    TEST_ASSERT_EQ(BIT12, (uint16_t)1 << 12, "Set 12th bit of uint16_t");
}

static void test_BIT13(void) {
    TEST_ASSERT_EQ(BIT13, (uint16_t)1 << 13, "Set 13th bit of uint16_t");
}

static void test_BIT14(void) {
    TEST_ASSERT_EQ(BIT14, (uint16_t)1 << 14, "Set 14th bit of uint16_t");
}

static void test_BIT15(void) {
    TEST_ASSERT_EQ(BIT15, (uint16_t)1 << 15, "Set 15th bit of uint16_t");
}

static void test_BIT16(void) {
    TEST_ASSERT_EQ(BIT16, (uint32_t)1 << 16, "Set 16th bit of uint32_t");
}

static void test_BIT17(void) {
    TEST_ASSERT_EQ(BIT17, (uint32_t)1 << 17, "Set 17th bit of uint32_t");
}

static void test_BIT18(void) {
    TEST_ASSERT_EQ(BIT18, (uint32_t)1 << 18, "Set 18th bit of uint32_t");
}

static void test_BIT19(void) {
    TEST_ASSERT_EQ(BIT19, (uint32_t)1 << 19, "Set 19th bit of uint32_t");
}

static void test_BIT20(void) {
    TEST_ASSERT_EQ(BIT20, (uint32_t)1 << 20, "Set 20th bit of uint32_t");
}

static void test_BIT21(void) {
    TEST_ASSERT_EQ(BIT21, (uint32_t)1 << 21, "Set 21st bit of uint32_t");
}

static void test_BIT22(void) {
    TEST_ASSERT_EQ(BIT22, (uint32_t)1 << 22, "Set 22nd bit of uint32_t");
}

static void test_BIT23(void) {
    TEST_ASSERT_EQ(BIT23, (uint32_t)1 << 23, "Set 23rd bit of uint32_t");
}

static void test_BIT24(void) {
    TEST_ASSERT_EQ(BIT24, (uint32_t)1 << 24, "Set 24th bit of uint32_t");
}

static void test_BIT25(void) {
    TEST_ASSERT_EQ(BIT25, (uint32_t)1 << 25, "Set 25th bit of uint32_t");
}

static void test_BIT26(void) {
    TEST_ASSERT_EQ(BIT26, (uint32_t)1 << 26, "Set 26th bit of uint32_t");
}

static void test_BIT27(void) {
    TEST_ASSERT_EQ(BIT27, (uint32_t)1 << 27, "Set 27th bit of uint32_t");
}

static void test_BIT28(void) {
    TEST_ASSERT_EQ(BIT28, (uint32_t)1 << 28, "Set 28th bit of uint32_t");
}

static void test_BIT29(void) {
    TEST_ASSERT_EQ(BIT29, (uint32_t)1 << 29, "Set 29th bit of uint32_t");
}

static void test_BIT30(void) {
    TEST_ASSERT_EQ(BIT30, (uint32_t)1 << 30, "Set 30th bit of uint32_t");
}

static void test_BIT31(void) {
    TEST_ASSERT_EQ(BIT31, (uint32_t)1 << 31, "Set 31st bit of uint32_t");
}

static void test_BIT32(void) {
    TEST_ASSERT_EQ(BIT32, (uint64_t)1 << 32, "Set 32nd bit of uint64_t");
}

static void test_BIT33(void) {
    TEST_ASSERT_EQ(BIT33, (uint64_t)1 << 33, "Set 33rd bit of uint64_t");
}

static void test_BIT34(void) {
    TEST_ASSERT_EQ(BIT34, (uint64_t)1 << 34, "Set 34th bit of uint64_t");
}

static void test_BIT35(void) {
    TEST_ASSERT_EQ(BIT35, (uint64_t)1 << 35, "Set 35th bit of uint64_t");
}

static void test_BIT36(void) {
    TEST_ASSERT_EQ(BIT36, (uint64_t)1 << 36, "Set 36th bit of uint64_t");
}

static void test_BIT37(void) {
    TEST_ASSERT_EQ(BIT37, (uint64_t)1 << 37, "Set 37th bit of uint64_t");
}

static void test_BIT38(void) {
    TEST_ASSERT_EQ(BIT38, (uint64_t)1 << 38, "Set 38th bit of uint64_t");
}

static void test_BIT39(void) {
    TEST_ASSERT_EQ(BIT39, (uint64_t)1 << 39, "Set 39th bit of uint64_t");
}

static void test_BIT40(void) {
    TEST_ASSERT_EQ(BIT40, (uint64_t)1 << 40, "Set 40th bit of uint64_t");
}

static void test_BIT41(void) {
    TEST_ASSERT_EQ(BIT41, (uint64_t)1 << 41, "Set 41st bit of uint64_t");
}

static void test_BIT42(void) {
    TEST_ASSERT_EQ(BIT42, (uint64_t)1 << 42, "Set 42nd bit of uint64_t");
}

static void test_BIT43(void) {
    TEST_ASSERT_EQ(BIT43, (uint64_t)1 << 43, "Set 43rd bit of uint64_t");
}

static void test_BIT44(void) {
    TEST_ASSERT_EQ(BIT44, (uint64_t)1 << 44, "Set 44th bit of uint64_t");
}

static void test_BIT45(void) {
    TEST_ASSERT_EQ(BIT45, (uint64_t)1 << 45, "Set 45th bit of uint64_t");
}

static void test_BIT46(void) {
    TEST_ASSERT_EQ(BIT46, (uint64_t)1 << 46, "Set 46th bit of uint64_t");
}

static void test_BIT47(void) {
    TEST_ASSERT_EQ(BIT47, (uint64_t)1 << 47, "Set 47th bit of uint64_t");
}

static void test_BIT48(void) {
    TEST_ASSERT_EQ(BIT48, (uint64_t)1 << 48, "Set 48th bit of uint64_t");
}

static void test_BIT49(void) {
    TEST_ASSERT_EQ(BIT49, (uint64_t)1 << 49, "Set 49th bit of uint64_t");
}

static void test_BIT50(void) {
    TEST_ASSERT_EQ(BIT50, (uint64_t)1 << 50, "Set 50th bit of uint64_t");
}

static void test_BIT51(void) {
    TEST_ASSERT_EQ(BIT51, (uint64_t)1 << 51, "Set 51st bit of uint64_t");
}

static void test_BIT52(void) {
    TEST_ASSERT_EQ(BIT52, (uint64_t)1 << 52, "Set 52nd bit of uint64_t");
}

static void test_BIT53(void) {
    TEST_ASSERT_EQ(BIT53, (uint64_t)1 << 53, "Set 53rd bit of uint64_t");
}

static void test_BIT54(void) {
    TEST_ASSERT_EQ(BIT54, (uint64_t)1 << 54, "Set 54th bit of uint64_t");
}

static void test_BIT55(void) {
    TEST_ASSERT_EQ(BIT55, (uint64_t)1 << 55, "Set 55th bit of uint64_t");
}

static void test_BIT56(void) {
    TEST_ASSERT_EQ(BIT56, (uint64_t)1 << 56, "Set 56th bit of uint64_t");
}

static void test_BIT57(void) {
    TEST_ASSERT_EQ(BIT57, (uint64_t)1 << 57, "Set 57th bit of uint64_t");
}

static void test_BIT58(void) {
    TEST_ASSERT_EQ(BIT58, (uint64_t)1 << 58, "Set 58th bit of uint64_t");
}

static void test_BIT59(void) {
    TEST_ASSERT_EQ(BIT59, (uint64_t)1 << 59, "Set 59th bit of uint64_t");
}

static void test_BIT60(void) {
    TEST_ASSERT_EQ(BIT60, (uint64_t)1 << 60, "Set 60th bit of uint64_t");
}

static void test_BIT61(void) {
    TEST_ASSERT_EQ(BIT61, (uint64_t)1 << 61, "Set 61st bit of uint64_t");
}

static void test_BIT62(void) {
    TEST_ASSERT_EQ(BIT62, (uint64_t)1 << 62, "Set 62nd bit of uint64_t");
}

static void test_BIT63(void) {
    TEST_ASSERT_EQ(BIT63, (uint64_t)1 << 63, "Set 63rd bit of uint64_t");
}

static void test_M_SET_BIT(void) {
    uint64_t value = HEX_ZERO;
    TEST_ASSERT_EQ(M_SET_BIT(value, 63), 0x8000000000000000ULL, "Set MSB of 0x00 using M_SET_BIT");
}

static void test_M_CLEAR_BIT(void) {
    uint64_t value = HEX_FULL;
    TEST_ASSERT_EQ(M_CLEAR_BIT(value, 63), 0x7FFFFFFFFFFFFFFFULL, "Clear MSB of 0xFF using M_CLEAR_BIT");
}

static void test_set_uint8_bit(void) {
    for(uint8_t bit = 0; bit < 8; bit++) {

        uint8_t expected = (uint8_t)(UINT8_MIN_VAL | (1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %uth bit of 0x00", bit);

        TEST_ASSERT_EQ(set_uint8_bit(UINT8_MIN_VAL, bit), expected, msg);
    }
}

static void test_set_uint16_bit(void) {
    for(uint16_t bit = 0; bit < 16; bit++) {

        uint16_t expected = (uint16_t)(UINT16_MIN_VAL | (1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %uth bit of 0x00", bit);

        TEST_ASSERT_EQ(set_uint16_bit(UINT16_MIN_VAL, bit), expected, msg);
    }
}

static void test_set_uint32_bit(void) {
    for(uint32_t bit = 0; bit < 32; bit++) {

        uint32_t expected = (uint32_t)(UINT32_MIN_VAL | (1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %uth bit of 0x00", bit);

        TEST_ASSERT_EQ(set_uint32_bit(UINT32_MIN_VAL, bit), expected, msg);
    }
}

static void test_set_uint64_bit(void) {
    for(uint64_t bit = 0; bit < 64; bit++) {

        uint64_t expected = (uint64_t)(UINT64_MIN_VAL | (1ULL << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %luth bit of 0x00", bit);

        TEST_ASSERT_EQ(set_uint64_bit(UINT64_MIN_VAL, bit), expected, msg);
    }
}

static void test_clear_uint8_bit(void) {
    for(uint8_t bit = 0; bit < 8; bit++) {

        uint8_t expected = (uint8_t)(UINT8_MAX_VAL & ~(1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Clear %uth bit of 0xFF", bit);

        TEST_ASSERT_EQ(clear_uint8_bit(UINT8_MAX_VAL, bit), expected, msg);
    }
}

static void test_clear_uint16_bit(void) {
    for(uint16_t bit = 0; bit < 16; bit++) {

        uint16_t expected = (uint16_t)(UINT16_MAX_VAL & ~(1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Clear %uth bit of 0xFF", bit);

        TEST_ASSERT_EQ(clear_uint16_bit(UINT16_MAX_VAL, bit), expected, msg);
    }
}

static void test_clear_uint32_bit(void) {
    for(uint32_t bit = 0; bit < 32; bit++) {

        uint32_t expected = (uint32_t)(UINT32_MAX_VAL & ~(1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Clear %uth bit of 0xFF", bit);

        TEST_ASSERT_EQ(clear_uint32_bit(UINT32_MAX_VAL, bit), expected, msg);
    }
}

static void test_clear_uint64_bit(void) {
    for(uint64_t bit = 0; bit < 64; bit++) {

        uint64_t expected = (uint64_t)(UINT64_MAX_VAL & ~(1ULL << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Clear %luth bit of 0xFF", bit);

        TEST_ASSERT_EQ(clear_uint64_bit(UINT64_MAX_VAL, bit), expected, msg);
    }
}

static void test_M_CLEAR_BIT8(void) {
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        uint8_t value = UINT8_MAX_VAL;
        uint8_t expected = (uint8_t)(UINT8_MAX_VAL & ~(1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Clear %uth bit of 0xFF using M_CLEAR_BIT8", bit);

        TEST_ASSERT_EQ(M_CLEAR_BIT8(value, bit), expected, msg);
    }
}

static void test_M_CLEAR_BIT16(void) {
    for (uint16_t bit = 0; bit < 16; bit++)
    {
        uint16_t value = UINT16_MAX_VAL;
        uint16_t expected = (uint16_t)(UINT16_MAX_VAL & ~(1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Clear %uth bit of 0xFF using M_CLEAR_BIT16", bit);

        TEST_ASSERT_EQ(M_CLEAR_BIT16(value, bit), expected, msg);
    }
}

static void test_M_CLEAR_BIT32(void) {
    for (uint32_t bit = 0; bit < 32; bit++)
    {
        uint32_t value = UINT32_MAX_VAL;
        uint32_t expected = (uint32_t)(UINT32_MAX_VAL & ~(1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Clear %uth bit of 0xFF using M_CLEAR_BIT32", bit);

        TEST_ASSERT_EQ(M_CLEAR_BIT32(value, bit), expected, msg);
    }
}

static void test_M_CLEAR_BIT64(void) {
    for (uint64_t bit = 0; bit < 64; bit++)
    {
        uint64_t value = UINT64_MAX_VAL;
        uint64_t expected = (uint64_t)(UINT64_MAX_VAL & ~(1ULL << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Clear %luth bit of 0xFF using M_CLEAR_BIT64", bit);

        TEST_ASSERT_EQ(M_CLEAR_BIT64(value, bit), expected, msg);
    }
}

static void test_M_SET_BIT8(void) {
    for (uint8_t bit = 0; bit < 8; bit++)
    {
        uint8_t value = UINT8_MIN_VAL;
        uint8_t expected = (uint8_t)(UINT8_MIN_VAL | (1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %uth bit of 0x00 using M_SET_BIT8", bit);

        TEST_ASSERT_EQ(M_SET_BIT8(value, bit), expected, msg);
    }
}

static void test_M_SET_BIT16(void) {
    for (uint16_t bit = 0; bit < 16; bit++)
    {
        uint16_t value = UINT16_MIN_VAL;
        uint16_t expected = (uint16_t)(UINT16_MIN_VAL | (1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %uth bit of 0x00 using M_SET_BIT16", bit);

        TEST_ASSERT_EQ(M_SET_BIT16(value, bit), expected, msg);
    }
}

static void test_M_SET_BIT32(void) {
    for (uint32_t bit = 0; bit < 32; bit++)
    {
        uint32_t value = UINT32_MIN_VAL;
        uint32_t expected = (uint32_t)(UINT32_MIN_VAL | (1U << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %uth bit of 0x00 using M_SET_BIT32", bit);

        TEST_ASSERT_EQ(M_SET_BIT32(value, bit), expected, msg);
    }
}

static void test_M_SET_BIT64(void) {
    for (uint64_t bit = 0; bit < 64; bit++)
    {
        uint64_t value = UINT64_MIN_VAL;
        uint64_t expected = (uint64_t)(UINT64_MIN_VAL | (1ULL << bit));

        char msg[80];
        snprintf(msg, sizeof(msg), "Set %luth bit of 0x00 using M_SET_BIT64", bit);

        TEST_ASSERT_EQ(M_SET_BIT64(value, bit), expected, msg);
    }
}

static void test_n_swap_8(void) {
    TEST_ASSERT_EQ(n_swap_8(MSB), (uint8_t)(0x21), "Swap nibbles in a byte");
}

static void test_nibble_Swap(void)
{
    uint8_t byte = MSB;           
    uint8_t expected = (uint8_t)(0x21);

    nibble_Swap(&byte);

    TEST_ASSERT_EQ(byte, expected, "Swap nibbles in a byte using nibble_Swap");
}

static void test_b_swap_16(void) {
    TEST_ASSERT_EQ(b_swap_16(HIGHER2BYTE), (uint16_t)(0x3412), "Swap bytes in a uint16_t");
}

static void test_b_swap_32(void) {
    TEST_ASSERT_EQ(b_swap_32(HIGHER4BYTE), (uint32_t)(0x78563412), "Swap bytes in a uint32_t");
}

static void test_b_swap_64(void) {
    TEST_ASSERT_EQ(b_swap_64(SERIAL_NUM), (uint64_t)(0xEFCDAB9078563412), "Swap bytes in a uint64_t");
}

static void test_by_swap_64(void) {
    TEST_ASSERT_EQ(by_swap_64(SERIAL_NUM), (uint64_t)(0xEFCDAB9078563412), "Swap bytes in a uint64_t using by_swap_64");
}

static void test_byte_Swap_16(void) {
    uint16_t word = HIGHER2BYTE;           
    uint16_t expected = (uint16_t)(0x3412);

    byte_Swap_16(&word);

    TEST_ASSERT_EQ(word, expected, "Swap bytes in a uint16_t");
}

static void test_byte_Swap_Int16(void) {
    int16_t word = (int16_t)HIGHER2BYTE;           
    int16_t expected = (int16_t)(0x3412);

    byte_Swap_Int16(&word);

    TEST_ASSERT_EQ(word, expected, "Swap bytes in a int16_t");
}

static void test_byte_Swap_32(void) {
    uint32_t dWord = HIGHER4BYTE;           
    uint32_t expected = (uint32_t)(0x78563412);

    byte_Swap_32(&dWord);

    TEST_ASSERT_EQ(dWord, expected, "Swap bytes in a uint32_t");
}

static void test_byte_Swap_Int32(void) {
    int32_t dWord = (int32_t)HIGHER4BYTE;           
    int32_t expected = (int32_t)(0x78563412);

    byte_Swap_Int32(&dWord);

    TEST_ASSERT_EQ(dWord, expected, "Swap bytes in a int32_t");
}

static void test_w_swap_32(void) {
    TEST_ASSERT_EQ(w_swap_32(HIGHER4BYTE), (uint32_t)(0x56781234), "Swap word within a dword");
}

static void test_word_Swap_32(void) {
    uint32_t dWord = HIGHER4BYTE;           
    uint32_t expected = (uint32_t)(0x56781234);

    word_Swap_32(&dWord);

    TEST_ASSERT_EQ(dWord, expected, "Swap word within a dword using word_Swap_32");
}

static void test_byte_Swap_64(void) {
    uint64_t qWord = SERIAL_NUM;           
    uint64_t expected = (uint64_t)(0xEFCDAB9078563412);

    byte_Swap_64(&qWord);

    TEST_ASSERT_EQ(qWord, expected, "Swap bytes in a uint64_t");
}

static void test_word_Swap_64(void) {
    uint64_t qWord = SERIAL_NUM;           
    uint64_t expected = (uint64_t)(0xCDEF90AB56781234);

    word_Swap_64(&qWord);

    TEST_ASSERT_EQ(qWord, expected, "Swap words in a uint64_t");
}

static void test_double_Word_Swap_64(void) {
    uint64_t qWord = SERIAL_NUM;           
    uint64_t expected = (uint64_t)(0x90ABCDEF12345678);

    double_Word_Swap_64(&qWord);

    TEST_ASSERT_EQ(qWord, expected, "Swap dwords in a uint64_t");
}

static void test_get_Bytes_To_16(void) {
    uint8_t buf[] = {0x12, 0x34, 0x56, 0x78};
    uint16_t out = 0;
    bool res;

    res = get_Bytes_To_16(buf, sizeof(buf), 0, 1, &out);
    TEST_ASSERT_EQ(out, (uint16_t)0x1234, "Big endian extraction of 2 bytes from a buffer");

    res = get_Bytes_To_16(buf, sizeof(buf), 1, 0, &out);
    TEST_ASSERT_EQ(out, (uint16_t)0x3412, "Little endian extraction of 2 bytes from a buffer");

    // Test to return false result
    res = get_Bytes_To_16(buf, sizeof(buf), 0, 3, &out);
    TEST_ASSERT_EQ(res, false, "get_Bytes_To_16 returns false for get_Bytes_Abs_Range(msb, lsb) > sizeof(uint16_t)");
}

static void test_get_Bytes_To_32(void) {
    uint8_t buf[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF};
    uint32_t out = 0;
    bool res;

    res = get_Bytes_To_32(buf, sizeof(buf), 0, 3, &out);
    TEST_ASSERT_EQ(out, (uint32_t)0x12345678, "Big endian extraction of 4 bytes from a buffer");

    res = get_Bytes_To_32(buf, sizeof(buf), 3, 0, &out);
    TEST_ASSERT_EQ(out, (uint32_t)0x78563412, "Little endian extraction of 4 bytes from a buffer");

    // Test to return false result
    res = get_Bytes_To_32(buf, sizeof(buf), 0, 6, &out);
    TEST_ASSERT_EQ(res, false, "get_Bytes_To_32 returns false for get_Bytes_Abs_Range(msb, lsb) > sizeof(uint32_t)");
}

static void test_get_Bytes_To_64(void) {
    uint8_t buf[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF};
    uint64_t out = 0;
    bool res;

    // msb == lsb
    res = get_Bytes_To_64(buf, sizeof(buf), 3, 3, &out);
    TEST_ASSERT_EQ(out, (uint64_t)0x78, "Extraction of a single byte from a buffer when msb == lsb");
    
    // lsb < msb
    res = get_Bytes_To_64(buf, sizeof(buf), 7, 0, &out);
    TEST_ASSERT_EQ(out, (uint64_t)0xEFCDAB9078563412, "Little endian extraction of 8 bytes from a buffer");
    
    // lsb > msb
    res = get_Bytes_To_64(buf, sizeof(buf), 0, 7, &out);
    TEST_ASSERT_EQ(out, (uint64_t)0x1234567890ABCDEF, "Big endian extraction of 8 bytes from a buffer");

    // Test for dataPtrBeginning == M_NULLPTR
    res = get_Bytes_To_64(M_NULLPTR, 0, 0, 7, &out);
    TEST_ASSERT_EQ(res, false, "get_Bytes_To_64 returns false for NULL pointer");

    // Test for out == M_NULLPTR
    res = get_Bytes_To_64(buf, sizeof(buf), 0, 7, M_NULLPTR);
    TEST_ASSERT_EQ(res, false, "get_Bytes_To_64 returns false for NULL output pointer");

    // Test for msb > fullDataLen
    res = get_Bytes_To_64(buf, sizeof(buf), 20, 0, &out);
    TEST_ASSERT_EQ(res, false, "get_Bytes_To_64 returns false for msb > fullDataLen");

    // Test for lsb > fullDataLen
    res = get_Bytes_To_64(buf, sizeof(buf), 0, 20, &out);
    TEST_ASSERT_EQ(res, false, "get_Bytes_To_64 returns false for lsb > fullDataLen");

    // Test for get_Bytes_Abs_Range(msb, lsb) > sizeof(uint64_t)
    res = get_Bytes_To_64(buf, sizeof(buf), 0, 9, &out);
    TEST_ASSERT_EQ(res, false, "get_Bytes_To_64 returns false for get_Bytes_Abs_Range(msb, lsb) > sizeof(uint64_t)");
}

static void test_be16_to_host(void) {
    uint16_t val = HIGHER2BYTE;
    uint16_t result = be16_to_host(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint16_t)0x1234, "Big endian host returns the same value");
    #else
        TEST_ASSERT_EQ(result, (uint16_t)0x3412, "Little endian host swaps to return little indian");
    #endif
}

static void test_be32_to_host(void) {
    uint32_t val = HIGHER4BYTE;
    uint32_t result = be32_to_host(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint32_t)0x12345678, "Big endian host returns the same value");
    #else
        TEST_ASSERT_EQ(result, (uint32_t)0x78563412, "Little endian host swaps to return little indian");
    #endif
}

static void test_be64_to_host(void) {
    uint64_t val = SERIAL_NUM;
    uint64_t result = be64_to_host(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint64_t)0x1234567890ABCDEF, "Big endian host returns the same value");
    #else
        TEST_ASSERT_EQ(result, (uint64_t)0xEFCDAB9078563412, "Little endian host swaps to return little indian");
    #endif
}

static void test_host_to_be16(void) {
    uint16_t val = HIGHER2BYTE;
    uint16_t result = host_to_be16(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint16_t)0x1234, "Big endian host returns the same value");
    #else
        TEST_ASSERT_EQ(result, (uint16_t)0x3412, "Little endian host swaps to big endian");
    #endif
}

static void test_host_to_be32(void) {
    uint32_t val = HIGHER4BYTE;
    uint32_t result = host_to_be32(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint32_t)0x12345678, "Big endian host returns the same value");
    #else
        TEST_ASSERT_EQ(result, (uint32_t)0x78563412, "Little endian host swaps to big endian");
    #endif
}

static void test_host_to_be64(void) {
    uint64_t val = SERIAL_NUM;
    uint64_t result = host_to_be64(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint64_t)0x1234567890ABCDEF, "Big endian host returns the same value");
    #else
        TEST_ASSERT_EQ(result, (uint64_t)0xEFCDAB9078563412, "Little endian host swaps to big endian");
    #endif
}

static void test_host_to_le16(void) {
    uint16_t val = HIGHER2BYTE;
    uint16_t result = host_to_le16(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint16_t)0x3412, "Big endian host swaps to little endian");
    #else
        TEST_ASSERT_EQ(result, (uint16_t)0x1234, "Little endian host returns the same value");
    #endif
}

static void test_host_to_le32(void) {
    uint32_t val = HIGHER4BYTE;
    uint32_t result = host_to_le32(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint32_t)0x78563412, "Big endian host swaps to little endian");
    #else
        TEST_ASSERT_EQ(result, (uint32_t)0x12345678, "Little endian host returns the same value");
    #endif
}

static void test_host_to_le64(void) {
    uint64_t val = SERIAL_NUM;
    uint64_t result = host_to_le64(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint64_t)0xEFCDAB9078563412, "Big endian host swaps to little endian");
    #else
        TEST_ASSERT_EQ(result, (uint64_t)0x1234567890ABCDEF, "Little endian host returns the same value");
    #endif
}

static void test_le16_to_host(void) {
    uint16_t val = HIGHER2BYTE;
    uint16_t result = le16_to_host(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint16_t)0x3412, "Big endian host swaps to little endian");
    #else
        TEST_ASSERT_EQ(result, (uint16_t)0x1234, "Little endian host returns the same value");
    #endif
}

static void test_le32_to_host(void) {
    uint32_t val = HIGHER4BYTE;
    uint32_t result = le32_to_host(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint32_t)0x78563412, "Big endian host swaps to little endian");
    #else
        TEST_ASSERT_EQ(result, (uint32_t)0x12345678, "Little endian host returns the same value");
    #endif
}

static void test_le64_to_host(void) {
    uint64_t val = SERIAL_NUM;
    uint64_t result = le64_to_host(val);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(result, (uint64_t)0xEFCDAB9078563412, "Big endian host swaps to little endian");
    #else
        TEST_ASSERT_EQ(result, (uint64_t)0x1234567890ABCDEF, "Little endian host returns the same value");
    #endif
}

static void test_big_To_Little_Endian_16(void) {
    uint16_t word = HIGHER2BYTE;          
    
    big_To_Little_Endian_16(&word);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(word, (uint16_t)(0x1234), "Big endian host returns the same value");
    #else
        TEST_ASSERT_EQ(word, (uint16_t)(0x3412), "Little endian host swaps to little endian");
    #endif
}

static void test_big_To_Little_Endian_32(void) {
    uint32_t dWord = HIGHER4BYTE;           
    
    big_To_Little_Endian_32(&dWord);

    #if defined(ENV_BIG_ENDIAN)
        TEST_ASSERT_EQ(dWord, (uint32_t)(0x12345678), "Big endian host returns the same value");
    #else
        TEST_ASSERT_EQ(dWord, (uint32_t)(0x78563412), "Little endian host swaps to little endian");
    #endif
}  

static void test_count_leading_zeros_uc(void) {
    uint8_t vals[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00};

    for(uint8_t i = 0; i < 9; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u leading zeros in unsigned char", i);
        TEST_ASSERT_EQ(count_leading_zeros_uc(vals[i]), i, msg);
    }
}

static void test_count_leading_zeros_us(void) {
    uint16_t vals[] = {
        0x8000, 
        0x4000, 
        0x2000, 
        0x1000, 
        0x0800, 
        0x0400, 
        0x0200, 
        0x0100, 
        0x0080, 
        0x0040, 
        0x0020, 
        0x0010, 
        0x0008, 
        0x0004, 
        0x0002, 
        0x0001, 
        0x0000
    };

    for(uint16_t i = 0; i < 17; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u leading zeros in unsigned short", i);
        TEST_ASSERT_EQ(count_leading_zeros_us(vals[i]), i, msg);
    }
}

static void test_count_leading_zeros_ui(void) {
    uint32_t vals[] = {
        0x80000000,
        0x40000000, 
        0x20000000, 
        0x10000000, 
        0x08000000, 
        0x04000000, 
        0x02000000, 
        0x01000000, 
        0x00800000, 
        0x00400000, 
        0x00200000, 
        0x00100000, 
        0x00080000, 
        0x00040000, 
        0x00020000, 
        0x00010000, 
        0x00008000, 
        0x00004000, 
        0x00002000, 
        0x00001000, 
        0x00000800, 
        0x00000400, 
        0x00000200, 
        0x00000100, 
        0x00000080, 
        0x00000040, 
        0x00000020, 
        0x00000010, 
        0x00000008, 
        0x00000004, 
        0x00000002, 
        0x00000001, 
        0x00000000
    };
    
    for(uint32_t i = 0; i < 33; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u leading zeros in unsigned int", i);
        TEST_ASSERT_EQ(count_leading_zeros_ui(vals[i]), i, msg);
    }
}

static void test_count_leading_zeros_ul(void) {
    uint64_t vals[] = {
        0x8000000000000000UL,
        0x4000000000000000UL,
        0x2000000000000000UL,
        0x1000000000000000UL,
        0x0800000000000000UL,
        0x0400000000000000UL,
        0x0200000000000000UL,
        0x0100000000000000UL,
        0x0080000000000000UL,
        0x0040000000000000UL,
        0x0020000000000000UL,
        0x0010000000000000UL,
        0x0008000000000000UL,
        0x0004000000000000UL,
        0x0002000000000000UL,
        0x0001000000000000UL,
        0x0000800000000000UL,
        0x0000400000000000UL,
        0x0000200000000000UL,
        0x0000100000000000UL,
        0x0000080000000000UL,
        0x0000040000000000UL,
        0x0000020000000000UL,
        0x0000010000000000UL,
        0x0000008000000000UL,
        0x0000004000000000UL,
        0x0000002000000000UL,
        0x0000001000000000UL,
        0x0000000800000000UL,
        0x0000000400000000UL,
        0x0000000200000000UL,
        0x0000000100000000UL,
        0x0000000080000000UL,
        0x0000000040000000UL,
        0x0000000020000000UL,
        0x0000000010000000UL,
        0x0000000008000000UL,
        0x0000000004000000UL,
        0x0000000002000000UL,
        0x0000000001000000UL,
        0x0000000000800000UL,
        0x0000000000400000UL,
        0x0000000000200000UL,
        0x0000000000100000UL,
        0x0000000000080000UL,
        0x0000000000040000UL,
        0x0000000000020000UL,
        0x0000000000010000UL,
        0x0000000000008000UL,
        0x0000000000004000UL,
        0x0000000000002000UL,
        0x0000000000001000UL,
        0x0000000000000800UL,
        0x0000000000000400UL,
        0x0000000000000200UL,
        0x0000000000000100UL,
        0x0000000000000080UL,
        0x0000000000000040UL,
        0x0000000000000020UL,
        0x0000000000000010UL,
        0x0000000000000008UL,
        0x0000000000000004UL,
        0x0000000000000002UL,
        0x0000000000000001UL,
        0x0000000000000000UL
    };
    
    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu leading zeros in unsigned long", i);
        TEST_ASSERT_EQ(count_leading_zeros_ul(vals[i]), i, msg);
    }
}

static void test_count_leading_zeros_ull(void) {
    uint64_t vals[] = {
        0x8000000000000000ULL,
        0x4000000000000000ULL,
        0x2000000000000000ULL,
        0x1000000000000000ULL,
        0x0800000000000000ULL,
        0x0400000000000000ULL,
        0x0200000000000000ULL,
        0x0100000000000000ULL,
        0x0080000000000000ULL,
        0x0040000000000000ULL,
        0x0020000000000000ULL,
        0x0010000000000000ULL,
        0x0008000000000000ULL,
        0x0004000000000000ULL,
        0x0002000000000000ULL,
        0x0001000000000000ULL,
        0x0000800000000000ULL,
        0x0000400000000000ULL,
        0x0000200000000000ULL,
        0x0000100000000000ULL,
        0x0000080000000000ULL,
        0x0000040000000000ULL,
        0x0000020000000000ULL,
        0x0000010000000000ULL,
        0x0000008000000000ULL,
        0x0000004000000000ULL,
        0x0000002000000000ULL,
        0x0000001000000000ULL,
        0x0000000800000000ULL,
        0x0000000400000000ULL,
        0x0000000200000000ULL,
        0x0000000100000000ULL,
        0x0000000080000000ULL,
        0x0000000040000000ULL,
        0x0000000020000000ULL,
        0x0000000010000000ULL,
        0x0000000008000000ULL,
        0x0000000004000000ULL,
        0x0000000002000000ULL,
        0x0000000001000000ULL,
        0x0000000000800000ULL,
        0x0000000000400000ULL,
        0x0000000000200000ULL,
        0x0000000000100000ULL,
        0x0000000000080000ULL,
        0x0000000000040000ULL,
        0x0000000000020000ULL,
        0x0000000000010000ULL,
        0x0000000000008000ULL,
        0x0000000000004000ULL,
        0x0000000000002000ULL,
        0x0000000000001000ULL,
        0x0000000000000800ULL,
        0x0000000000000400ULL,
        0x0000000000000200ULL,
        0x0000000000000100ULL,
        0x0000000000000080ULL,
        0x0000000000000040ULL,
        0x0000000000000020ULL,
        0x0000000000000010ULL,
        0x0000000000000008ULL,
        0x0000000000000004ULL,
        0x0000000000000002ULL,
        0x0000000000000001ULL,
        0x0000000000000000ULL
    };
    
    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu leading zeros in unsigned long long", i);
        TEST_ASSERT_EQ(count_leading_zeros_ull(vals[i]), i, msg);
    }
}

static void test_count_leading_ones_uc(void) {
    uint8_t vals[] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};

    for(uint8_t i = 0; i < 9; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u leading ones in unsigned char", i);
        TEST_ASSERT_EQ(count_leading_ones_uc(vals[i]), i, msg);
    }
}

static void test_count_leading_ones_us(void) {
    uint16_t vals[] = {
        0x0000,
    	0x8000,
    	0xC000,
    	0xE000,
    	0xF000,
    	0xF800,
    	0xFC00,
    	0xFE00,
    	0xFF00,
    	0xFF80,
    	0xFFC0,
    	0xFFE0,
    	0xFFF0,
    	0xFFF8,
    	0xFFFC,
    	0xFFFE,
    	0xFFFF
    };

    for(uint16_t i = 0; i < 17; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u leading ones in unsigned short", i);
        TEST_ASSERT_EQ(count_leading_ones_us(vals[i]), i, msg);
    }
}

static void test_count_leading_ones_ui(void) {
    uint32_t vals[] = {
    	0x00000000,
    	0x80000000,
    	0xC0000000,
    	0xE0000000,
    	0xF0000000,
    	0xF8000000,
    	0xFC000000,
    	0xFE000000,
    	0xFF000000,
    	0xFF800000,
    	0xFFC00000,
    	0xFFE00000,
    	0xFFF00000,
    	0xFFF80000,
    	0xFFFC0000,
    	0xFFFE0000,
    	0xFFFF0000,
    	0xFFFF8000,
    	0xFFFFC000,
    	0xFFFFE000,
    	0xFFFFF000,
    	0xFFFFF800,
    	0xFFFFFC00,
    	0xFFFFFE00,
    	0xFFFFFF00,
    	0xFFFFFF80,
    	0xFFFFFFC0,
    	0xFFFFFFE0,
    	0xFFFFFFF0,
    	0xFFFFFFF8,
    	0xFFFFFFFC,
    	0xFFFFFFFE,
    	0xFFFFFFFF
    };

    for(uint32_t i = 0; i < 33; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u leading ones in unsigned int", i);
        TEST_ASSERT_EQ(count_leading_ones_ui(vals[i]), i, msg);
    }
}

static void test_count_leading_ones_ul(void) {
    uint64_t vals[] = {
        0x0000000000000000UL,
    	0x8000000000000000UL,
    	0xC000000000000000UL,
    	0xE000000000000000UL,
    	0xF000000000000000UL,
    	0xF800000000000000UL,
    	0xFC00000000000000UL,
    	0xFE00000000000000UL,
    	0xFF00000000000000UL,
    	0xFF80000000000000UL,
    	0xFFC0000000000000UL,
    	0xFFE0000000000000UL,
    	0xFFF0000000000000UL,
    	0xFFF8000000000000UL,
    	0xFFFC000000000000UL,
    	0xFFFE000000000000UL,
    	0xFFFF000000000000UL,
    	0xFFFF800000000000UL,
    	0xFFFFC00000000000UL,
    	0xFFFFE00000000000UL,
    	0xFFFFF00000000000UL,
    	0xFFFFF80000000000UL,
    	0xFFFFFC0000000000UL,
    	0xFFFFFE0000000000UL,
    	0xFFFFFF0000000000UL,
    	0xFFFFFF8000000000UL,
    	0xFFFFFFC000000000UL,
    	0xFFFFFFE000000000UL,
    	0xFFFFFFF000000000UL,
    	0xFFFFFFF800000000UL,
    	0xFFFFFFFC00000000UL,
    	0xFFFFFFFE00000000UL,
    	0xFFFFFFFF00000000UL,
    	0xFFFFFFFF80000000UL,
    	0xFFFFFFFFC0000000UL,
    	0xFFFFFFFFE0000000UL,
    	0xFFFFFFFFF0000000UL,
    	0xFFFFFFFFF8000000UL,
    	0xFFFFFFFFFC000000UL,
    	0xFFFFFFFFFE000000UL,
    	0xFFFFFFFFFF000000UL,
    	0xFFFFFFFFFF800000UL,
    	0xFFFFFFFFFFC00000UL,
    	0xFFFFFFFFFFE00000UL,
    	0xFFFFFFFFFFF00000UL,
    	0xFFFFFFFFFFF80000UL,
    	0xFFFFFFFFFFFC0000UL,
    	0xFFFFFFFFFFFE0000UL,
    	0xFFFFFFFFFFFF0000UL,
    	0xFFFFFFFFFFFF8000UL,
    	0xFFFFFFFFFFFFC000UL,
    	0xFFFFFFFFFFFFE000UL,
    	0xFFFFFFFFFFFFF000UL,
    	0xFFFFFFFFFFFFF800UL,
    	0xFFFFFFFFFFFFFC00UL,
    	0xFFFFFFFFFFFFFE00UL,
    	0xFFFFFFFFFFFFFF00UL,
    	0xFFFFFFFFFFFFFF80UL,
    	0xFFFFFFFFFFFFFFC0UL,
    	0xFFFFFFFFFFFFFFE0UL,
    	0xFFFFFFFFFFFFFFF0UL,
    	0xFFFFFFFFFFFFFFF8UL,
    	0xFFFFFFFFFFFFFFFCUL,
    	0xFFFFFFFFFFFFFFFEUL,
    	0xFFFFFFFFFFFFFFFFUL
    };

    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu leading ones in unsigned long", i);
        TEST_ASSERT_EQ(count_leading_ones_ul(vals[i]), i, msg);
    }
}

static void test_count_leading_ones_ull(void) {
    uint64_t vals[] = {
        0x0000000000000000ULL,
    	0x8000000000000000ULL,
    	0xC000000000000000ULL,
    	0xE000000000000000ULL,
    	0xF000000000000000ULL,
    	0xF800000000000000ULL,
    	0xFC00000000000000ULL,
    	0xFE00000000000000ULL,
    	0xFF00000000000000ULL,
    	0xFF80000000000000ULL,
    	0xFFC0000000000000ULL,
    	0xFFE0000000000000ULL,
    	0xFFF0000000000000ULL,
    	0xFFF8000000000000ULL,
    	0xFFFC000000000000ULL,
    	0xFFFE000000000000ULL,
    	0xFFFF000000000000ULL,
    	0xFFFF800000000000ULL,
    	0xFFFFC00000000000ULL,
    	0xFFFFE00000000000ULL,
    	0xFFFFF00000000000ULL,
    	0xFFFFF80000000000ULL,
    	0xFFFFFC0000000000ULL,
    	0xFFFFFE0000000000ULL,
    	0xFFFFFF0000000000ULL,
    	0xFFFFFF8000000000ULL,
    	0xFFFFFFC000000000ULL,
    	0xFFFFFFE000000000ULL,
    	0xFFFFFFF000000000ULL,
    	0xFFFFFFF800000000ULL,
    	0xFFFFFFFC00000000ULL,
    	0xFFFFFFFE00000000ULL,
    	0xFFFFFFFF00000000ULL,
    	0xFFFFFFFF80000000ULL,
    	0xFFFFFFFFC0000000ULL,
    	0xFFFFFFFFE0000000ULL,
    	0xFFFFFFFFF0000000ULL,
    	0xFFFFFFFFF8000000ULL,
    	0xFFFFFFFFFC000000ULL,
    	0xFFFFFFFFFE000000ULL,
    	0xFFFFFFFFFF000000ULL,
    	0xFFFFFFFFFF800000ULL,
    	0xFFFFFFFFFFC00000ULL,
    	0xFFFFFFFFFFE00000ULL,
    	0xFFFFFFFFFFF00000ULL,
    	0xFFFFFFFFFFF80000ULL,
    	0xFFFFFFFFFFFC0000ULL,
    	0xFFFFFFFFFFFE0000ULL,
    	0xFFFFFFFFFFFF0000ULL,
    	0xFFFFFFFFFFFF8000ULL,
    	0xFFFFFFFFFFFFC000ULL,
    	0xFFFFFFFFFFFFE000ULL,
    	0xFFFFFFFFFFFFF000ULL,
    	0xFFFFFFFFFFFFF800ULL,
    	0xFFFFFFFFFFFFFC00ULL,
    	0xFFFFFFFFFFFFFE00ULL,
    	0xFFFFFFFFFFFFFF00ULL,
    	0xFFFFFFFFFFFFFF80ULL,
    	0xFFFFFFFFFFFFFFC0ULL,
    	0xFFFFFFFFFFFFFFE0ULL,
    	0xFFFFFFFFFFFFFFF0ULL,
    	0xFFFFFFFFFFFFFFF8ULL,
    	0xFFFFFFFFFFFFFFFCULL,
    	0xFFFFFFFFFFFFFFFEULL,
    	0xFFFFFFFFFFFFFFFFULL
    };

    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu leading ones in unsigned long long", i);
        TEST_ASSERT_EQ(count_leading_ones_ull(vals[i]), i, msg);
    }
}

static void test_count_trailing_zeros_uc(void) {
    uint8_t vals[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00};

    for(uint8_t i = 0; i < 9; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u trailing zeros in unsigned char", i);
        TEST_ASSERT_EQ(count_trailing_zeros_uc(vals[i]), i, msg);
    }
}

static void test_count_trailing_zeros_us(void) {
    uint16_t vals[] = {
        0x0001,
        0x0002,
        0x0004,
        0x0008,
        0x0010,
        0x0020,
        0x0040,
        0x0080,
        0x0100,
        0x0200,
        0x0400,
        0x0800,
        0x1000,
        0x2000,
        0x4000,
        0x8000,
        0x0000
    };

    for(uint16_t i = 0; i < 17; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u trailing zeros in unsigned short", i);
        TEST_ASSERT_EQ(count_trailing_zeros_us(vals[i]), i, msg);
    }
}

static void test_count_trailing_zeros_ui(void) {
    uint32_t vals[] = {
        0x00000001,
        0x00000002,
        0x00000004,
        0x00000008,
        0x00000010,
        0x00000020,
        0x00000040,
        0x00000080,
        0x00000100,
        0x00000200,
        0x00000400,
        0x00000800,
        0x00001000,
        0x00002000,
        0x00004000,
        0x00008000,
        0x00010000,
        0x00020000,
        0x00040000,
        0x00080000,
        0x00100000,
        0x00200000,
        0x00400000,
        0x00800000,
        0x01000000,
        0x02000000,
        0x04000000,
        0x08000000,
        0x10000000,
        0x20000000,
        0x40000000,
        0x80000000,
        0x00000000
    };
    
    for(uint32_t i = 0; i < 33; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u trailing zeros in unsigned int", i);
        TEST_ASSERT_EQ(count_trailing_zeros_ui(vals[i]), i, msg);
    }
}

static void test_count_trailing_zeros_ul(void) {
    uint64_t vals[] = {
        0x0000000000000001UL,
        0x0000000000000002UL,
        0x0000000000000004UL,
        0x0000000000000008UL,
        0x0000000000000010UL,
        0x0000000000000020UL,
        0x0000000000000040UL,
        0x0000000000000080UL,
        0x0000000000000100UL,
        0x0000000000000200UL,
        0x0000000000000400UL,
        0x0000000000000800UL,
        0x0000000000001000UL,
        0x0000000000002000UL,
        0x0000000000004000UL,
        0x0000000000008000UL,
        0x0000000000010000UL,
        0x0000000000020000UL,
        0x0000000000040000UL,
        0x0000000000080000UL,
        0x0000000000100000UL,
        0x0000000000200000UL,
        0x0000000000400000UL,
        0x0000000000800000UL,
        0x0000000001000000UL,
        0x0000000002000000UL,
        0x0000000004000000UL,
        0x0000000008000000UL,
        0x0000000010000000UL,
        0x0000000020000000UL,
        0x0000000040000000UL,
        0x0000000080000000UL,
        0x0000000100000000UL,
        0x0000000200000000UL,
        0x0000000400000000UL,
        0x0000000800000000UL,
        0x0000001000000000UL,
        0x0000002000000000UL,
        0x0000004000000000UL,
        0x0000008000000000UL,
        0x0000010000000000UL,
        0x0000020000000000UL,
        0x0000040000000000UL,
        0x0000080000000000UL,
        0x0000100000000000UL,
        0x0000200000000000UL,
        0x0000400000000000UL,
        0x0000800000000000UL,
        0x0001000000000000UL,
        0x0002000000000000UL,
        0x0004000000000000UL,
        0x0008000000000000UL,
        0x0010000000000000UL,
        0x0020000000000000UL,
        0x0040000000000000UL,
        0x0080000000000000UL,
        0x0100000000000000UL,
        0x0200000000000000UL,
        0x0400000000000000UL,
        0x0800000000000000UL,
        0x1000000000000000UL,
        0x2000000000000000UL,
        0x4000000000000000UL,
        0x8000000000000000UL,
        0x0000000000000000UL
    };

    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu trailing zeros in unsigned long", i);
        TEST_ASSERT_EQ(count_trailing_zeros_ul(vals[i]), i, msg);
    }
}

static void test_count_trailing_zeros_ull(void) {
    uint64_t vals[] = {
        0x0000000000000001ULL,
        0x0000000000000002ULL,
        0x0000000000000004ULL,
        0x0000000000000008ULL,
        0x0000000000000010ULL,
        0x0000000000000020ULL,
        0x0000000000000040ULL,
        0x0000000000000080ULL,
        0x0000000000000100ULL,
        0x0000000000000200ULL,
        0x0000000000000400ULL,
        0x0000000000000800ULL,
        0x0000000000001000ULL,
        0x0000000000002000ULL,
        0x0000000000004000ULL,
        0x0000000000008000ULL,
        0x0000000000010000ULL,
        0x0000000000020000ULL,
        0x0000000000040000ULL,
        0x0000000000080000ULL,
        0x0000000000100000ULL,
        0x0000000000200000ULL,
        0x0000000000400000ULL,
        0x0000000000800000ULL,
        0x0000000001000000ULL,
        0x0000000002000000ULL,
        0x0000000004000000ULL,
        0x0000000008000000ULL,
        0x0000000010000000ULL,
        0x0000000020000000ULL,
        0x0000000040000000ULL,
        0x0000000080000000ULL,
        0x0000000100000000ULL,
        0x0000000200000000ULL,
        0x0000000400000000ULL,
        0x0000000800000000ULL,
        0x0000001000000000ULL,
        0x0000002000000000ULL,
        0x0000004000000000ULL,
        0x0000008000000000ULL,
        0x0000010000000000ULL,
        0x0000020000000000ULL,
        0x0000040000000000ULL,
        0x0000080000000000ULL,
        0x0000100000000000ULL,
        0x0000200000000000ULL,
        0x0000400000000000ULL,
        0x0000800000000000ULL,
        0x0001000000000000ULL,
        0x0002000000000000ULL,
        0x0004000000000000ULL,
        0x0008000000000000ULL,
        0x0010000000000000ULL,
        0x0020000000000000ULL,
        0x0040000000000000ULL,
        0x0080000000000000ULL,
        0x0100000000000000ULL,
        0x0200000000000000ULL,
        0x0400000000000000ULL,
        0x0800000000000000ULL,
        0x1000000000000000ULL,
        0x2000000000000000ULL,
        0x4000000000000000ULL,
        0x8000000000000000ULL,
        0x0000000000000000ULL
    };

    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu trailing zeros in unsigned long long", i);
        TEST_ASSERT_EQ(count_trailing_zeros_ull(vals[i]), i, msg);
    }
}

static void test_count_trailing_ones_uc(void) {
    uint8_t vals[] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};

    for(uint8_t i = 0; i < 9; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u trailing ones in unsigned char", i);
        TEST_ASSERT_EQ(count_trailing_ones_uc(vals[i]), i, msg);
    }
}

static void test_count_trailing_ones_us(void) {
    uint16_t vals[] = {
        0x0000,
        0x0001,
        0x0003,
        0x0007,
        0x000F,
        0x001F,
        0x003F,
        0x007F,
        0x00FF,
        0x01FF,
        0x03FF,
        0x07FF,
        0x0FFF,
        0x1FFF,
        0x3FFF,
        0x7FFF,
        0xFFFF
    };

    for(uint16_t i = 0; i < 17; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u trailing ones in unsigned short", i);
        TEST_ASSERT_EQ(count_trailing_ones_us(vals[i]), i, msg);
    }
}

static void test_count_trailing_ones_ui(void) {
    uint32_t vals[] = {
        0x00000000,
        0x00000001,
        0x00000003,
        0x00000007,
        0x0000000F,
        0x0000001F,
        0x0000003F,
        0x0000007F,
        0x000000FF,
        0x000001FF,
        0x000003FF,
        0x000007FF,
        0x00000FFF,
        0x00001FFF,
        0x00003FFF,
        0x00007FFF,
        0x0000FFFF,
        0x0001FFFF,
        0x0003FFFF,
        0x0007FFFF,
        0x000FFFFF,
        0x001FFFFF,
        0x003FFFFF,
        0x007FFFFF,
        0x00FFFFFF,
        0x01FFFFFF,
        0x03FFFFFF,
        0x07FFFFFF,
        0x0FFFFFFF,
        0x1FFFFFFF,
        0x3FFFFFFF,
        0x7FFFFFFF,
        0xFFFFFFFF
    };

    for(uint32_t i = 0; i < 33; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u trailing ones in unsigned int", i);
        TEST_ASSERT_EQ(count_trailing_ones_ui(vals[i]), i, msg);
    }
}

static void test_count_trailing_ones_ul(void) {
    uint64_t vals[] = {
        0x0000000000000000UL,
        0x0000000000000001UL,
        0x0000000000000003UL,
        0x0000000000000007UL,
        0x000000000000000FUL,
        0x000000000000001FUL,
        0x000000000000003FUL,
        0x000000000000007FUL,
        0x00000000000000FFUL,
        0x00000000000001FFUL,
        0x00000000000003FFUL,
        0x00000000000007FFUL,
        0x0000000000000FFFUL,
        0x0000000000001FFFUL,
        0x0000000000003FFFUL,
        0x0000000000007FFFUL,
        0x000000000000FFFFUL,
        0x000000000001FFFFUL,
        0x000000000003FFFFUL,
        0x000000000007FFFFUL,
        0x00000000000FFFFFUL,
        0x00000000001FFFFFUL,
        0x00000000003FFFFFUL,
        0x00000000007FFFFFUL,
        0x0000000000FFFFFFUL,
        0x0000000001FFFFFFUL,
        0x0000000003FFFFFFUL,
        0x0000000007FFFFFFUL,
        0x000000000FFFFFFFUL,
        0x000000001FFFFFFFUL,
        0x000000003FFFFFFFUL,
        0x000000007FFFFFFFUL,
        0x00000000FFFFFFFFUL,
        0x00000001FFFFFFFFUL,
        0x00000003FFFFFFFFUL,
        0x00000007FFFFFFFFUL,
        0x0000000FFFFFFFFFUL,
        0x0000001FFFFFFFFFUL,
        0x0000003FFFFFFFFFUL,
        0x0000007FFFFFFFFFUL,
        0x000000FFFFFFFFFFUL,
        0x000001FFFFFFFFFFUL,
        0x000003FFFFFFFFFFUL,
        0x000007FFFFFFFFFFUL,
        0x00000FFFFFFFFFFFUL,
        0x00001FFFFFFFFFFFUL,
        0x00003FFFFFFFFFFFUL,
        0x00007FFFFFFFFFFFUL,
        0x0000FFFFFFFFFFFFUL,
        0x0001FFFFFFFFFFFFUL,
        0x0003FFFFFFFFFFFFUL,
        0x0007FFFFFFFFFFFFUL,
        0x000FFFFFFFFFFFFFUL,
        0x001FFFFFFFFFFFFFUL,
        0x003FFFFFFFFFFFFFUL,
        0x007FFFFFFFFFFFFFUL,
        0x00FFFFFFFFFFFFFFUL,
        0x01FFFFFFFFFFFFFFUL,
        0x03FFFFFFFFFFFFFFUL,
        0x07FFFFFFFFFFFFFFUL,
        0x0FFFFFFFFFFFFFFFUL,
        0x1FFFFFFFFFFFFFFFUL,
        0x3FFFFFFFFFFFFFFFUL,
        0x7FFFFFFFFFFFFFFFUL,
        0xFFFFFFFFFFFFFFFFUL
    };

    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu trailing ones in unsigned long", i);
        TEST_ASSERT_EQ(count_trailing_ones_ul(vals[i]), i, msg);
    }
}

static void test_count_trailing_ones_ull(void) {
    uint64_t vals[] = {
        0x0000000000000000ULL,
        0x0000000000000001ULL,
        0x0000000000000003ULL,
        0x0000000000000007ULL,
        0x000000000000000FULL,
        0x000000000000001FULL,
        0x000000000000003FULL,
        0x000000000000007FULL,
        0x00000000000000FFULL,
        0x00000000000001FFULL,
        0x00000000000003FFULL,
        0x00000000000007FFULL,
        0x0000000000000FFFULL,
        0x0000000000001FFFULL,
        0x0000000000003FFFULL,
        0x0000000000007FFFULL,
        0x000000000000FFFFULL,
        0x000000000001FFFFULL,
        0x000000000003FFFFULL,
        0x000000000007FFFFULL,
        0x00000000000FFFFFULL,
        0x00000000001FFFFFULL,
        0x00000000003FFFFFULL,
        0x00000000007FFFFFULL,
        0x0000000000FFFFFFULL,
        0x0000000001FFFFFFULL,
        0x0000000003FFFFFFULL,
        0x0000000007FFFFFFULL,
        0x000000000FFFFFFFULL,
        0x000000001FFFFFFFULL,
        0x000000003FFFFFFFULL,
        0x000000007FFFFFFFULL,
        0x00000000FFFFFFFFULL,
        0x00000001FFFFFFFFULL,
        0x00000003FFFFFFFFULL,
        0x00000007FFFFFFFFULL,
        0x0000000FFFFFFFFFULL,
        0x0000001FFFFFFFFFULL,
        0x0000003FFFFFFFFFULL,
        0x0000007FFFFFFFFFULL,
        0x000000FFFFFFFFFFULL,
        0x000001FFFFFFFFFFULL,
        0x000003FFFFFFFFFFULL,
        0x000007FFFFFFFFFFULL,
        0x00000FFFFFFFFFFFULL,
        0x00001FFFFFFFFFFFULL,
        0x00003FFFFFFFFFFFULL,
        0x00007FFFFFFFFFFFULL,
        0x0000FFFFFFFFFFFFULL,
        0x0001FFFFFFFFFFFFULL,
        0x0003FFFFFFFFFFFFULL,
        0x0007FFFFFFFFFFFFULL,
        0x000FFFFFFFFFFFFFULL,
        0x001FFFFFFFFFFFFFULL,
        0x003FFFFFFFFFFFFFULL,
        0x007FFFFFFFFFFFFFULL,
        0x00FFFFFFFFFFFFFFULL,
        0x01FFFFFFFFFFFFFFULL,
        0x03FFFFFFFFFFFFFFULL,
        0x07FFFFFFFFFFFFFFULL,
        0x0FFFFFFFFFFFFFFFULL,
        0x1FFFFFFFFFFFFFFFULL,
        0x3FFFFFFFFFFFFFFFULL,
        0x7FFFFFFFFFFFFFFFULL,
        0xFFFFFFFFFFFFFFFFULL
    };

    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu trailing ones in unsigned long long", i);
        TEST_ASSERT_EQ(count_trailing_ones_ull(vals[i]), i, msg);
    }
}

static void test_first_leading_one_uc(void) {
    uint8_t vals[] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x00};

    for(uint8_t i = 0; i < 9; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "First leading 1 on bit position %u in unsigned char", 8-i);
        TEST_ASSERT_EQ(first_leading_one_uc(vals[i]), 8-i, msg);
    }
}

static void test_first_leading_one_us(void) {
    uint16_t vals[] = {
        0x0001,
        0x0003,
        0x0007,
        0x000F,
        0x001F,
        0x003F,
        0x007F,
        0x00FF,
        0x01FF,
        0x03FF,
        0x07FF,
        0x0FFF,
        0x1FFF,
        0x3FFF,
        0x7FFF,
        0xFFFF,
        0x0000
    };

    for(uint16_t i = 1; i < 17; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "First leading 1 on bit position %u in unsigned short", 16-i);
        TEST_ASSERT_EQ(first_leading_one_us(vals[i]), 16-i, msg);
    }
}

static void test_first_leading_one_ui(void) {
    uint32_t vals[] = {
        0x00000001,
        0x00000003,
        0x00000007,
        0x0000000F,
        0x0000001F,
        0x0000003F,
        0x0000007F,
        0x000000FF,
        0x000001FF,
        0x000003FF,
        0x000007FF,
        0x00000FFF,
        0x00001FFF,
        0x00003FFF,
        0x00007FFF,
        0x0000FFFF,
        0x0001FFFF,
        0x0003FFFF,
        0x0007FFFF,
        0x000FFFFF,
        0x001FFFFF,
        0x003FFFFF,
        0x007FFFFF,
        0x00FFFFFF,
        0x01FFFFFF,
        0x03FFFFFF,
        0x07FFFFFF,
        0x0FFFFFFF,
        0x1FFFFFFF,
        0x3FFFFFFF,
        0x7FFFFFFF,
        0xFFFFFFFF,
        0x00000000
    };

    for(uint32_t i = 1; i < 33; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "First leading 1 on bit position %u in unsigned int", 32-i);
        TEST_ASSERT_EQ(first_leading_one_ui(vals[i]), 32-i, msg);
    }
}

static void test_first_leading_one_ul(void) {
    uint64_t vals[] = {
        0x0000000000000001UL,
        0x0000000000000003UL,
        0x0000000000000007UL,
        0x000000000000000FUL,
        0x000000000000001FUL,
        0x000000000000003FUL,
        0x000000000000007FUL,
        0x00000000000000FFUL,
        0x00000000000001FFUL,
        0x00000000000003FFUL,
        0x00000000000007FFUL,
        0x0000000000000FFFUL,
        0x0000000000001FFFUL,
        0x0000000000003FFFUL,
        0x0000000000007FFFUL,
        0x000000000000FFFFUL,
        0x000000000001FFFFUL,
        0x000000000003FFFFUL,
        0x000000000007FFFFUL,
        0x00000000000FFFFFUL,
        0x00000000001FFFFFUL,
        0x00000000003FFFFFUL,
        0x00000000007FFFFFUL,
        0x0000000000FFFFFFUL,
        0x0000000001FFFFFFUL,
        0x0000000003FFFFFFUL,
        0x0000000007FFFFFFUL,
        0x000000000FFFFFFFUL,
        0x000000001FFFFFFFUL,
        0x000000003FFFFFFFUL,
        0x000000007FFFFFFFUL,
        0x00000000FFFFFFFFUL,
        0x00000001FFFFFFFFUL,
        0x00000003FFFFFFFFUL,
        0x00000007FFFFFFFFUL,
        0x0000000FFFFFFFFFUL,
        0x0000001FFFFFFFFFUL,
        0x0000003FFFFFFFFFUL,
        0x0000007FFFFFFFFFUL,
        0x000000FFFFFFFFFFUL,
        0x000001FFFFFFFFFFUL,
        0x000003FFFFFFFFFFUL,
        0x000007FFFFFFFFFFUL,
        0x00000FFFFFFFFFFFUL,
        0x00001FFFFFFFFFFFUL,
        0x00003FFFFFFFFFFFUL,
        0x00007FFFFFFFFFFFUL,
        0x0000FFFFFFFFFFFFUL,
        0x0001FFFFFFFFFFFFUL,
        0x0003FFFFFFFFFFFFUL,
        0x0007FFFFFFFFFFFFUL,
        0x000FFFFFFFFFFFFFUL,
        0x001FFFFFFFFFFFFFUL,
        0x003FFFFFFFFFFFFFUL,
        0x007FFFFFFFFFFFFFUL,
        0x00FFFFFFFFFFFFFFUL,
        0x01FFFFFFFFFFFFFFUL,
        0x03FFFFFFFFFFFFFFUL,
        0x07FFFFFFFFFFFFFFUL,
        0x0FFFFFFFFFFFFFFFUL,
        0x1FFFFFFFFFFFFFFFUL,
        0x3FFFFFFFFFFFFFFFUL,
        0x7FFFFFFFFFFFFFFFUL,
        0xFFFFFFFFFFFFFFFFUL,
        0x0000000000000000UL
    };

    for(uint64_t i = 1; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "First leading 1 on bit position %lu in unsigned long", 64-i);
        TEST_ASSERT_EQ(first_leading_one_ul(vals[i]), 64-i, msg);
    }
}

static void test_first_leading_one_ull(void) {
    uint64_t vals[] = {
        0x0000000000000001ULL,
        0x0000000000000003ULL,
        0x0000000000000007ULL,
        0x000000000000000FULL,
        0x000000000000001FULL,
        0x000000000000003FULL,
        0x000000000000007FULL,
        0x00000000000000FFULL,
        0x00000000000001FFULL,
        0x00000000000003FFULL,
        0x00000000000007FFULL,
        0x0000000000000FFFULL,
        0x0000000000001FFFULL,
        0x0000000000003FFFULL,
        0x0000000000007FFFULL,
        0x000000000000FFFFULL,
        0x000000000001FFFFULL,
        0x000000000003FFFFULL,
        0x000000000007FFFFULL,
        0x00000000000FFFFFULL,
        0x00000000001FFFFFULL,
        0x00000000003FFFFFULL,
        0x00000000007FFFFFULL,
        0x0000000000FFFFFFULL,
        0x0000000001FFFFFFULL,
        0x0000000003FFFFFFULL,
        0x0000000007FFFFFFULL,
        0x000000000FFFFFFFULL,
        0x000000001FFFFFFFULL,
        0x000000003FFFFFFFULL,
        0x000000007FFFFFFFULL,
        0x00000000FFFFFFFFULL,
        0x00000001FFFFFFFFULL,
        0x00000003FFFFFFFFULL,
        0x00000007FFFFFFFFULL,
        0x0000000FFFFFFFFFULL,
        0x0000001FFFFFFFFFULL,
        0x0000003FFFFFFFFFULL,
        0x0000007FFFFFFFFFULL,
        0x000000FFFFFFFFFFULL,
        0x000001FFFFFFFFFFULL,
        0x000003FFFFFFFFFFULL,
        0x000007FFFFFFFFFFULL,
        0x00000FFFFFFFFFFFULL,
        0x00001FFFFFFFFFFFULL,
        0x00003FFFFFFFFFFFULL,
        0x00007FFFFFFFFFFFULL,
        0x0000FFFFFFFFFFFFULL,
        0x0001FFFFFFFFFFFFULL,
        0x0003FFFFFFFFFFFFULL,
        0x0007FFFFFFFFFFFFULL,
        0x000FFFFFFFFFFFFFULL,
        0x001FFFFFFFFFFFFFULL,
        0x003FFFFFFFFFFFFFULL,
        0x007FFFFFFFFFFFFFULL,
        0x00FFFFFFFFFFFFFFULL,
        0x01FFFFFFFFFFFFFFULL,
        0x03FFFFFFFFFFFFFFULL,
        0x07FFFFFFFFFFFFFFULL,
        0x0FFFFFFFFFFFFFFFULL,
        0x1FFFFFFFFFFFFFFFULL,
        0x3FFFFFFFFFFFFFFFULL,
        0x7FFFFFFFFFFFFFFFULL,
        0xFFFFFFFFFFFFFFFFULL,
        0x0000000000000000ULL
    };

    for(uint64_t i = 1; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "First leading 1 on bit position %lu in unsigned long long", 64-i);
        TEST_ASSERT_EQ(first_leading_one_ull(vals[i]), 64-i, msg);
    }
}

static void test_first_leading_zero_uc(void) {
    uint8_t vals[8] = {0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE};

    for(uint8_t i = 0; i < 8; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "First leading 0 on bit position %u in unsigned char", i+1);
        TEST_ASSERT_EQ(first_leading_zero_uc(vals[i]), i+1, msg);
    }
}

static void test_first_leading_zero_us(void) {
    uint16_t vals[16] = {
        0x7FFF,
        0xBFFF,
        0xDFFF,
        0xEFFF,
        0xF7FF,
        0xFBFF,
        0xFDFF,
        0xFEFF,
        0xFF7F,
        0xFFBF,
        0xFFDF,
        0xFFEF,
        0xFFF7,
        0xFFFB,
        0xFFFD,
        0xFFFE
    };

    for(uint16_t i = 0; i < 16; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "First leading 0 on bit position %u in unsigned short", i+1);
        TEST_ASSERT_EQ(first_leading_zero_us(vals[i]), i+1, msg);
    }
}

static void test_first_leading_zero_ui(void) {
    uint32_t vals[32] = {
        0x7FFFFFFF,
        0xBFFFFFFF,
        0xDFFFFFFF,
        0xEFFFFFFF,
        0xF7FFFFFF,
        0xFBFFFFFF,
        0xFDFFFFFF,
        0xFEFFFFFF,
        0xFF7FFFFF,
        0xFFBFFFFF,
        0xFFDFFFFF,
        0xFFEFFFFF,
        0xFFF7FFFF,
        0xFFFBFFFF,
        0xFFFDFFFF,
        0xFFFEFFFF,
        0xFFFF7FFF,
        0xFFFFBFFF,
        0xFFFFDFFF,
        0xFFFFEFFF,
        0xFFFFF7FF,
        0xFFFFFBFF,
        0xFFFFFDFF,
        0xFFFFFEFF,
        0xFFFFFF7F,
        0xFFFFFFBF,
        0xFFFFFFDF,
        0xFFFFFFEF,
        0xFFFFFFF7,
        0xFFFFFFFB,
        0xFFFFFFFD,
        0xFFFFFFFE
    };

    for(uint32_t i = 0; i < 32; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "First leading 0 on bit position %u in unsigned int", i+1);
        TEST_ASSERT_EQ(first_leading_zero_ui(vals[i]), i+1, msg);
    }
}

static void test_first_leading_zero_ul(void) {
    uint64_t vals[64] = {
        0x7FFFFFFFFFFFFFFFUL,
        0xBFFFFFFFFFFFFFFFUL,
        0xDFFFFFFFFFFFFFFFUL,
        0xEFFFFFFFFFFFFFFFUL,
        0xF7FFFFFFFFFFFFFFUL,
        0xFBFFFFFFFFFFFFFFUL,
        0xFDFFFFFFFFFFFFFFUL,
        0xFEFFFFFFFFFFFFFFUL,
        0xFF7FFFFFFFFFFFFFUL,
        0xFFBFFFFFFFFFFFFFUL,
        0xFFDFFFFFFFFFFFFFUL,
        0xFFEFFFFFFFFFFFFFUL,
        0xFFF7FFFFFFFFFFFFUL,
        0xFFFBFFFFFFFFFFFFUL,
        0xFFFDFFFFFFFFFFFFUL,
        0xFFFEFFFFFFFFFFFFUL,
        0xFFFF7FFFFFFFFFFFUL,
        0xFFFFBFFFFFFFFFFFUL,
        0xFFFFDFFFFFFFFFFFUL,
        0xFFFFEFFFFFFFFFFFUL,
        0xFFFFF7FFFFFFFFFFUL,
        0xFFFFFBFFFFFFFFFFUL,
        0xFFFFFDFFFFFFFFFFUL,
        0xFFFFFEFFFFFFFFFFUL,
        0xFFFFFF7FFFFFFFFFUL,
        0xFFFFFFBFFFFFFFFFUL,
        0xFFFFFFDFFFFFFFFFUL,
        0xFFFFFFEFFFFFFFFFUL,
        0xFFFFFFF7FFFFFFFFUL,
        0xFFFFFFFBFFFFFFFFUL,
        0xFFFFFFFDFFFFFFFFUL,
        0xFFFFFFFEFFFFFFFFUL,
        0xFFFFFFFF7FFFFFFFUL,
        0xFFFFFFFFBFFFFFFFUL,
        0xFFFFFFFFDFFFFFFFUL,
        0xFFFFFFFFEFFFFFFFUL,
        0xFFFFFFFFF7FFFFFFUL,
        0xFFFFFFFFFBFFFFFFUL,
        0xFFFFFFFFFDFFFFFFUL,
        0xFFFFFFFFFEFFFFFFUL,
        0xFFFFFFFFFF7FFFFFUL,
        0xFFFFFFFFFFBFFFFFUL,
        0xFFFFFFFFFFDFFFFFUL,
        0xFFFFFFFFFFEFFFFFUL,
        0xFFFFFFFFFFF7FFFFUL,
        0xFFFFFFFFFFFBFFFFUL,
        0xFFFFFFFFFFFDFFFFUL,
        0xFFFFFFFFFFFEFFFFUL,
        0xFFFFFFFFFFFF7FFFUL,
        0xFFFFFFFFFFFFBFFFUL,
        0xFFFFFFFFFFFFDFFFUL,
        0xFFFFFFFFFFFFEFFFUL,
        0xFFFFFFFFFFFFF7FFUL,
        0xFFFFFFFFFFFFFBFFUL,
        0xFFFFFFFFFFFFFDFFUL,
        0xFFFFFFFFFFFFFEFFUL,
        0xFFFFFFFFFFFFFF7FUL,
        0xFFFFFFFFFFFFFFBFUL,
        0xFFFFFFFFFFFFFFDFUL,
        0xFFFFFFFFFFFFFFEFUL,
        0xFFFFFFFFFFFFFFF7UL,
        0xFFFFFFFFFFFFFFFBUL,
        0xFFFFFFFFFFFFFFFDUL,
        0xFFFFFFFFFFFFFFFEUL
    };

    for(uint64_t i = 0; i < 64; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "First leading 0 on bit position %lu in unsigned long", i+1);
        TEST_ASSERT_EQ(first_leading_zero_ul(vals[i]), i+1, msg);
    }
}

static void test_first_leading_zero_ull(void) {
    uint64_t vals[64] = {
        0x7FFFFFFFFFFFFFFFULL,
        0xBFFFFFFFFFFFFFFFULL,
        0xDFFFFFFFFFFFFFFFULL,
        0xEFFFFFFFFFFFFFFFULL,
        0xF7FFFFFFFFFFFFFFULL,
        0xFBFFFFFFFFFFFFFFULL,
        0xFDFFFFFFFFFFFFFFULL,
        0xFEFFFFFFFFFFFFFFULL,
        0xFF7FFFFFFFFFFFFFULL,
        0xFFBFFFFFFFFFFFFFULL,
        0xFFDFFFFFFFFFFFFFULL,
        0xFFEFFFFFFFFFFFFFULL,
        0xFFF7FFFFFFFFFFFFULL,
        0xFFFBFFFFFFFFFFFFULL,
        0xFFFDFFFFFFFFFFFFULL,
        0xFFFEFFFFFFFFFFFFULL,
        0xFFFF7FFFFFFFFFFFULL,
        0xFFFFBFFFFFFFFFFFULL,
        0xFFFFDFFFFFFFFFFFULL,
        0xFFFFEFFFFFFFFFFFULL,
        0xFFFFF7FFFFFFFFFFULL,
        0xFFFFFBFFFFFFFFFFULL,
        0xFFFFFDFFFFFFFFFFULL,
        0xFFFFFEFFFFFFFFFFULL,
        0xFFFFFF7FFFFFFFFFULL,
        0xFFFFFFBFFFFFFFFFULL,
        0xFFFFFFDFFFFFFFFFULL,
        0xFFFFFFEFFFFFFFFFULL,
        0xFFFFFFF7FFFFFFFFULL,
        0xFFFFFFFBFFFFFFFFULL,
        0xFFFFFFFDFFFFFFFFULL,
        0xFFFFFFFEFFFFFFFFULL,
        0xFFFFFFFF7FFFFFFFULL,
        0xFFFFFFFFBFFFFFFFULL,
        0xFFFFFFFFDFFFFFFFULL,
        0xFFFFFFFFEFFFFFFFULL,
        0xFFFFFFFFF7FFFFFFULL,
        0xFFFFFFFFFBFFFFFFULL,
        0xFFFFFFFFFDFFFFFFULL,
        0xFFFFFFFFFEFFFFFFULL,
        0xFFFFFFFFFF7FFFFFULL,
        0xFFFFFFFFFFBFFFFFULL,
        0xFFFFFFFFFFDFFFFFULL,
        0xFFFFFFFFFFEFFFFFULL,
        0xFFFFFFFFFFF7FFFFULL,
        0xFFFFFFFFFFFBFFFFULL,
        0xFFFFFFFFFFFDFFFFULL,
        0xFFFFFFFFFFFEFFFFULL,
        0xFFFFFFFFFFFF7FFFULL,
        0xFFFFFFFFFFFFBFFFULL,
        0xFFFFFFFFFFFFDFFFULL,
        0xFFFFFFFFFFFFEFFFULL,
        0xFFFFFFFFFFFFF7FFULL,
        0xFFFFFFFFFFFFFBFFULL,
        0xFFFFFFFFFFFFFDFFULL,
        0xFFFFFFFFFFFFFEFFULL,
        0xFFFFFFFFFFFFFF7FULL,
        0xFFFFFFFFFFFFFFBFULL,
        0xFFFFFFFFFFFFFFDFULL,
        0xFFFFFFFFFFFFFFEFULL,
        0xFFFFFFFFFFFFFFF7ULL,
        0xFFFFFFFFFFFFFFFBULL,
        0xFFFFFFFFFFFFFFFDULL,
        0xFFFFFFFFFFFFFFFEULL
    };

    for(uint64_t i = 0; i < 64; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "First leading 0 on bit position %lu in unsigned long long", i+1);
        TEST_ASSERT_EQ(first_leading_zero_ull(vals[i]), i+1, msg);
    }
}

static void test_count_ones_uc(void) {
    uint8_t vals[] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};

    for(uint8_t i = 0; i < 9; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u ones in unsigned char", i);
        TEST_ASSERT_EQ(count_ones_uc(vals[i]), i, msg);
    }
}

static void test_count_ones_us(void) {
    uint16_t vals[] = {
        0x0000,
    	0x8000,
    	0xC000,
    	0xE000,
    	0xF000,
    	0xF800,
    	0xFC00,
    	0xFE00,
    	0xFF00,
    	0xFF80,
    	0xFFC0,
    	0xFFE0,
    	0xFFF0,
    	0xFFF8,
    	0xFFFC,
    	0xFFFE,
    	0xFFFF
    };

    for(uint16_t i = 0; i < 17; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u ones in unsigned short", i);
        TEST_ASSERT_EQ(count_ones_us(vals[i]), i, msg);
    }
}

static void test_count_ones_ui(void) {
    uint32_t vals[] = {
    	0x00000000,
    	0x80000000,
    	0xC0000000,
    	0xE0000000,
    	0xF0000000,
    	0xF8000000,
    	0xFC000000,
    	0xFE000000,
    	0xFF000000,
    	0xFF800000,
    	0xFFC00000,
    	0xFFE00000,
    	0xFFF00000,
    	0xFFF80000,
    	0xFFFC0000,
    	0xFFFE0000,
    	0xFFFF0000,
    	0xFFFF8000,
    	0xFFFFC000,
    	0xFFFFE000,
    	0xFFFFF000,
    	0xFFFFF800,
    	0xFFFFFC00,
    	0xFFFFFE00,
    	0xFFFFFF00,
    	0xFFFFFF80,
    	0xFFFFFFC0,
    	0xFFFFFFE0,
    	0xFFFFFFF0,
    	0xFFFFFFF8,
    	0xFFFFFFFC,
    	0xFFFFFFFE,
    	0xFFFFFFFF
    };

    for(uint32_t i = 0; i < 33; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u ones in unsigned int", i);
        TEST_ASSERT_EQ(count_ones_ui(vals[i]), i, msg);
    }
}

static void test_count_ones_ul(void) {
    uint64_t vals[] = {
        0x0000000000000000UL,
    	0x8000000000000000UL,
    	0xC000000000000000UL,
    	0xE000000000000000UL,
    	0xF000000000000000UL,
    	0xF800000000000000UL,
    	0xFC00000000000000UL,
    	0xFE00000000000000UL,
    	0xFF00000000000000UL,
    	0xFF80000000000000UL,
    	0xFFC0000000000000UL,
    	0xFFE0000000000000UL,
    	0xFFF0000000000000UL,
    	0xFFF8000000000000UL,
    	0xFFFC000000000000UL,
    	0xFFFE000000000000UL,
    	0xFFFF000000000000UL,
    	0xFFFF800000000000UL,
    	0xFFFFC00000000000UL,
    	0xFFFFE00000000000UL,
    	0xFFFFF00000000000UL,
    	0xFFFFF80000000000UL,
    	0xFFFFFC0000000000UL,
    	0xFFFFFE0000000000UL,
    	0xFFFFFF0000000000UL,
    	0xFFFFFF8000000000UL,
    	0xFFFFFFC000000000UL,
    	0xFFFFFFE000000000UL,
    	0xFFFFFFF000000000UL,
    	0xFFFFFFF800000000UL,
    	0xFFFFFFFC00000000UL,
    	0xFFFFFFFE00000000UL,
    	0xFFFFFFFF00000000UL,
    	0xFFFFFFFF80000000UL,
    	0xFFFFFFFFC0000000UL,
    	0xFFFFFFFFE0000000UL,
    	0xFFFFFFFFF0000000UL,
    	0xFFFFFFFFF8000000UL,
    	0xFFFFFFFFFC000000UL,
    	0xFFFFFFFFFE000000UL,
    	0xFFFFFFFFFF000000UL,
    	0xFFFFFFFFFF800000UL,
    	0xFFFFFFFFFFC00000UL,
    	0xFFFFFFFFFFE00000UL,
    	0xFFFFFFFFFFF00000UL,
    	0xFFFFFFFFFFF80000UL,
    	0xFFFFFFFFFFFC0000UL,
    	0xFFFFFFFFFFFE0000UL,
    	0xFFFFFFFFFFFF0000UL,
    	0xFFFFFFFFFFFF8000UL,
    	0xFFFFFFFFFFFFC000UL,
    	0xFFFFFFFFFFFFE000UL,
    	0xFFFFFFFFFFFFF000UL,
    	0xFFFFFFFFFFFFF800UL,
    	0xFFFFFFFFFFFFFC00UL,
    	0xFFFFFFFFFFFFFE00UL,
    	0xFFFFFFFFFFFFFF00UL,
    	0xFFFFFFFFFFFFFF80UL,
    	0xFFFFFFFFFFFFFFC0UL,
    	0xFFFFFFFFFFFFFFE0UL,
    	0xFFFFFFFFFFFFFFF0UL,
    	0xFFFFFFFFFFFFFFF8UL,
    	0xFFFFFFFFFFFFFFFCUL,
    	0xFFFFFFFFFFFFFFFEUL,
    	0xFFFFFFFFFFFFFFFFUL
    };

    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu ones in unsigned long", i);
        TEST_ASSERT_EQ(count_ones_ul(vals[i]), i, msg);
    }
}

static void test_count_ones_ull(void) {
    uint64_t vals[] = {
        0x0000000000000000ULL,
    	0x8000000000000000ULL,
    	0xC000000000000000ULL,
    	0xE000000000000000ULL,
    	0xF000000000000000ULL,
    	0xF800000000000000ULL,
    	0xFC00000000000000ULL,
    	0xFE00000000000000ULL,
    	0xFF00000000000000ULL,
    	0xFF80000000000000ULL,
    	0xFFC0000000000000ULL,
    	0xFFE0000000000000ULL,
    	0xFFF0000000000000ULL,
    	0xFFF8000000000000ULL,
    	0xFFFC000000000000ULL,
    	0xFFFE000000000000ULL,
    	0xFFFF000000000000ULL,
    	0xFFFF800000000000ULL,
    	0xFFFFC00000000000ULL,
    	0xFFFFE00000000000ULL,
    	0xFFFFF00000000000ULL,
    	0xFFFFF80000000000ULL,
    	0xFFFFFC0000000000ULL,
    	0xFFFFFE0000000000ULL,
    	0xFFFFFF0000000000ULL,
    	0xFFFFFF8000000000ULL,
    	0xFFFFFFC000000000ULL,
    	0xFFFFFFE000000000ULL,
    	0xFFFFFFF000000000ULL,
    	0xFFFFFFF800000000ULL,
    	0xFFFFFFFC00000000ULL,
    	0xFFFFFFFE00000000ULL,
    	0xFFFFFFFF00000000ULL,
    	0xFFFFFFFF80000000ULL,
    	0xFFFFFFFFC0000000ULL,
    	0xFFFFFFFFE0000000ULL,
    	0xFFFFFFFFF0000000ULL,
    	0xFFFFFFFFF8000000ULL,
    	0xFFFFFFFFFC000000ULL,
    	0xFFFFFFFFFE000000ULL,
    	0xFFFFFFFFFF000000ULL,
    	0xFFFFFFFFFF800000ULL,
    	0xFFFFFFFFFFC00000ULL,
    	0xFFFFFFFFFFE00000ULL,
    	0xFFFFFFFFFFF00000ULL,
    	0xFFFFFFFFFFF80000ULL,
    	0xFFFFFFFFFFFC0000ULL,
    	0xFFFFFFFFFFFE0000ULL,
    	0xFFFFFFFFFFFF0000ULL,
    	0xFFFFFFFFFFFF8000ULL,
    	0xFFFFFFFFFFFFC000ULL,
    	0xFFFFFFFFFFFFE000ULL,
    	0xFFFFFFFFFFFFF000ULL,
    	0xFFFFFFFFFFFFF800ULL,
    	0xFFFFFFFFFFFFFC00ULL,
    	0xFFFFFFFFFFFFFE00ULL,
    	0xFFFFFFFFFFFFFF00ULL,
    	0xFFFFFFFFFFFFFF80ULL,
    	0xFFFFFFFFFFFFFFC0ULL,
    	0xFFFFFFFFFFFFFFE0ULL,
    	0xFFFFFFFFFFFFFFF0ULL,
    	0xFFFFFFFFFFFFFFF8ULL,
    	0xFFFFFFFFFFFFFFFCULL,
    	0xFFFFFFFFFFFFFFFEULL,
    	0xFFFFFFFFFFFFFFFFULL
    };

    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu ones in unsigned long long", i);
        TEST_ASSERT_EQ(count_ones_ull(vals[i]), i, msg);
    }
}

static void test_count_zeros_uc(void) {
    uint8_t vals[] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};

    for(uint8_t i = 0; i < 9; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u zeros in unsigned char", 8-i);
        TEST_ASSERT_EQ(count_zeros_uc(vals[i]), 8-i, msg);
    }
}

static void test_count_zeros_us(void) {
    uint16_t vals[] = {
        0x0000,
    	0x8000,
    	0xC000,
    	0xE000,
    	0xF000,
    	0xF800,
    	0xFC00,
    	0xFE00,
    	0xFF00,
    	0xFF80,
    	0xFFC0,
    	0xFFE0,
    	0xFFF0,
    	0xFFF8,
    	0xFFFC,
    	0xFFFE,
    	0xFFFF
    };

    for(uint16_t i = 0; i < 17; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u zeros in unsigned short", 16-i);
        TEST_ASSERT_EQ(count_zeros_us(vals[i]), 16-i, msg);
    }
}

static void test_count_zeros_ui(void) {
    uint32_t vals[] = {
    	0x00000000,
    	0x80000000,
    	0xC0000000,
    	0xE0000000,
    	0xF0000000,
    	0xF8000000,
    	0xFC000000,
    	0xFE000000,
    	0xFF000000,
    	0xFF800000,
    	0xFFC00000,
    	0xFFE00000,
    	0xFFF00000,
    	0xFFF80000,
    	0xFFFC0000,
    	0xFFFE0000,
    	0xFFFF0000,
    	0xFFFF8000,
    	0xFFFFC000,
    	0xFFFFE000,
    	0xFFFFF000,
    	0xFFFFF800,
    	0xFFFFFC00,
    	0xFFFFFE00,
    	0xFFFFFF00,
    	0xFFFFFF80,
    	0xFFFFFFC0,
    	0xFFFFFFE0,
    	0xFFFFFFF0,
    	0xFFFFFFF8,
    	0xFFFFFFFC,
    	0xFFFFFFFE,
    	0xFFFFFFFF
    };

    for(uint32_t i = 0; i < 33; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%u zeros in unsigned int", 32-i);
        TEST_ASSERT_EQ(count_zeros_ui(vals[i]), 32-i, msg);
    }
}

static void test_count_zeros_ul(void) {
    uint64_t vals[] = {
        0x0000000000000000UL,
    	0x8000000000000000UL,
    	0xC000000000000000UL,
    	0xE000000000000000UL,
    	0xF000000000000000UL,
    	0xF800000000000000UL,
    	0xFC00000000000000UL,
    	0xFE00000000000000UL,
    	0xFF00000000000000UL,
    	0xFF80000000000000UL,
    	0xFFC0000000000000UL,
    	0xFFE0000000000000UL,
    	0xFFF0000000000000UL,
    	0xFFF8000000000000UL,
    	0xFFFC000000000000UL,
    	0xFFFE000000000000UL,
    	0xFFFF000000000000UL,
    	0xFFFF800000000000UL,
    	0xFFFFC00000000000UL,
    	0xFFFFE00000000000UL,
    	0xFFFFF00000000000UL,
    	0xFFFFF80000000000UL,
    	0xFFFFFC0000000000UL,
    	0xFFFFFE0000000000UL,
    	0xFFFFFF0000000000UL,
    	0xFFFFFF8000000000UL,
    	0xFFFFFFC000000000UL,
    	0xFFFFFFE000000000UL,
    	0xFFFFFFF000000000UL,
    	0xFFFFFFF800000000UL,
    	0xFFFFFFFC00000000UL,
    	0xFFFFFFFE00000000UL,
    	0xFFFFFFFF00000000UL,
    	0xFFFFFFFF80000000UL,
    	0xFFFFFFFFC0000000UL,
    	0xFFFFFFFFE0000000UL,
    	0xFFFFFFFFF0000000UL,
    	0xFFFFFFFFF8000000UL,
    	0xFFFFFFFFFC000000UL,
    	0xFFFFFFFFFE000000UL,
    	0xFFFFFFFFFF000000UL,
    	0xFFFFFFFFFF800000UL,
    	0xFFFFFFFFFFC00000UL,
    	0xFFFFFFFFFFE00000UL,
    	0xFFFFFFFFFFF00000UL,
    	0xFFFFFFFFFFF80000UL,
    	0xFFFFFFFFFFFC0000UL,
    	0xFFFFFFFFFFFE0000UL,
    	0xFFFFFFFFFFFF0000UL,
    	0xFFFFFFFFFFFF8000UL,
    	0xFFFFFFFFFFFFC000UL,
    	0xFFFFFFFFFFFFE000UL,
    	0xFFFFFFFFFFFFF000UL,
    	0xFFFFFFFFFFFFF800UL,
    	0xFFFFFFFFFFFFFC00UL,
    	0xFFFFFFFFFFFFFE00UL,
    	0xFFFFFFFFFFFFFF00UL,
    	0xFFFFFFFFFFFFFF80UL,
    	0xFFFFFFFFFFFFFFC0UL,
    	0xFFFFFFFFFFFFFFE0UL,
    	0xFFFFFFFFFFFFFFF0UL,
    	0xFFFFFFFFFFFFFFF8UL,
    	0xFFFFFFFFFFFFFFFCUL,
    	0xFFFFFFFFFFFFFFFEUL,
    	0xFFFFFFFFFFFFFFFFUL
    };

    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu zeros in unsigned long", 64-i);
        TEST_ASSERT_EQ(count_zeros_ul(vals[i]), 64-i, msg);
    }
}

static void test_count_zeros_ull(void) {
    uint64_t vals[] = {
        0x0000000000000000ULL,
    	0x8000000000000000ULL,
    	0xC000000000000000ULL,
    	0xE000000000000000ULL,
    	0xF000000000000000ULL,
    	0xF800000000000000ULL,
    	0xFC00000000000000ULL,
    	0xFE00000000000000ULL,
    	0xFF00000000000000ULL,
    	0xFF80000000000000ULL,
    	0xFFC0000000000000ULL,
    	0xFFE0000000000000ULL,
    	0xFFF0000000000000ULL,
    	0xFFF8000000000000ULL,
    	0xFFFC000000000000ULL,
    	0xFFFE000000000000ULL,
    	0xFFFF000000000000ULL,
    	0xFFFF800000000000ULL,
    	0xFFFFC00000000000ULL,
    	0xFFFFE00000000000ULL,
    	0xFFFFF00000000000ULL,
    	0xFFFFF80000000000ULL,
    	0xFFFFFC0000000000ULL,
    	0xFFFFFE0000000000ULL,
    	0xFFFFFF0000000000ULL,
    	0xFFFFFF8000000000ULL,
    	0xFFFFFFC000000000ULL,
    	0xFFFFFFE000000000ULL,
    	0xFFFFFFF000000000ULL,
    	0xFFFFFFF800000000ULL,
    	0xFFFFFFFC00000000ULL,
    	0xFFFFFFFE00000000ULL,
    	0xFFFFFFFF00000000ULL,
    	0xFFFFFFFF80000000ULL,
    	0xFFFFFFFFC0000000ULL,
    	0xFFFFFFFFE0000000ULL,
    	0xFFFFFFFFF0000000ULL,
    	0xFFFFFFFFF8000000ULL,
    	0xFFFFFFFFFC000000ULL,
    	0xFFFFFFFFFE000000ULL,
    	0xFFFFFFFFFF000000ULL,
    	0xFFFFFFFFFF800000ULL,
    	0xFFFFFFFFFFC00000ULL,
    	0xFFFFFFFFFFE00000ULL,
    	0xFFFFFFFFFFF00000ULL,
    	0xFFFFFFFFFFF80000ULL,
    	0xFFFFFFFFFFFC0000ULL,
    	0xFFFFFFFFFFFE0000ULL,
    	0xFFFFFFFFFFFF0000ULL,
    	0xFFFFFFFFFFFF8000ULL,
    	0xFFFFFFFFFFFFC000ULL,
    	0xFFFFFFFFFFFFE000ULL,
    	0xFFFFFFFFFFFFF000ULL,
    	0xFFFFFFFFFFFFF800ULL,
    	0xFFFFFFFFFFFFFC00ULL,
    	0xFFFFFFFFFFFFFE00ULL,
    	0xFFFFFFFFFFFFFF00ULL,
    	0xFFFFFFFFFFFFFF80ULL,
    	0xFFFFFFFFFFFFFFC0ULL,
    	0xFFFFFFFFFFFFFFE0ULL,
    	0xFFFFFFFFFFFFFFF0ULL,
    	0xFFFFFFFFFFFFFFF8ULL,
    	0xFFFFFFFFFFFFFFFCULL,
    	0xFFFFFFFFFFFFFFFEULL,
    	0xFFFFFFFFFFFFFFFFULL
    };

    for(uint64_t i = 0; i < 65; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu zeros in unsigned long long", 64-i);
        TEST_ASSERT_EQ(count_zeros_ull(vals[i]), 64-i, msg);
    }
}

static void test_has_single_bit_uc(void) {
    TEST_ASSERT(!has_single_bit_uc(UINT8_MIN_VAL), "Returns false as no bit is set");
    TEST_ASSERT(!has_single_bit_uc(UINT8_MAX_VAL), "Returns false as more than 1 bit is set");
    TEST_ASSERT(has_single_bit_uc(0x01), "Returns true as a single bit is set");
}

static void test_has_single_bit_us(void) {
    TEST_ASSERT(!has_single_bit_us(UINT16_MIN_VAL), "Returns false as no bit is set");
    TEST_ASSERT(!has_single_bit_us(UINT16_MAX_VAL), "Returns false as more than 1 bit is set");
    TEST_ASSERT(has_single_bit_us((uint16_t)0x01), "Returns true as a single bit is set");
}

static void test_has_single_bit_ui(void) {
    TEST_ASSERT(!has_single_bit_ui(UINT32_MIN_VAL), "Returns false as no bit is set");
    TEST_ASSERT(!has_single_bit_ui(UINT32_MAX_VAL), "Returns false as more than 1 bit is set");
    TEST_ASSERT(has_single_bit_ui((uint32_t)0x01), "Returns true as a single bit is set");
}

static void test_has_single_bit_ul(void) {
    TEST_ASSERT(!has_single_bit_ul(UINT64_MIN_VAL), "Returns false as no bit is set");
    TEST_ASSERT(!has_single_bit_ul(UINT64_MAX_VAL), "Returns false as more than 1 bit is set");
    TEST_ASSERT(has_single_bit_ul((uint64_t)0x01), "Returns true as a single bit is set");
}

static void test_has_single_bit_ull(void) {
    TEST_ASSERT(!has_single_bit_ull(UINT64_MIN_VAL), "Returns false as no bit is set");
    TEST_ASSERT(!has_single_bit_ull(UINT64_MAX_VAL), "Returns false as more than 1 bit is set");
    TEST_ASSERT(has_single_bit_ull((uint64_t)0x01), "Returns true as a single bit is set");
}

static void test_get_req_bit_width_uc(void) {
    uint8_t vals[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00};

    for(uint8_t i = 0; i < 9; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "The smallest number of bits needed to represent the given value is %u", 8-i);
        TEST_ASSERT_EQ(get_req_bit_width_uc(vals[i]), 8-i, msg);
    }
}

static void test_get_req_bit_width_us(void) {
    uint16_t vals[] = {
        0x8000, 
        0x4000, 
        0x2000, 
        0x1000, 
        0x0800, 
        0x0400, 
        0x0200, 
        0x0100, 
        0x0080, 
        0x0040, 
        0x0020, 
        0x0010, 
        0x0008, 
        0x0004, 
        0x0002, 
        0x0001, 
        0x0000
    };

    for(uint16_t i = 0; i < 17; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "The smallest number of bits needed to represent the given value is %u", 16-i);
        TEST_ASSERT_EQ(get_req_bit_width_us(vals[i]), 16-i, msg);
    }
}

static void test_get_req_bit_width_ui(void) {
    uint32_t vals[] = {
        0x80000000,
        0x40000000, 
        0x20000000, 
        0x10000000, 
        0x08000000, 
        0x04000000, 
        0x02000000, 
        0x01000000, 
        0x00800000, 
        0x00400000, 
        0x00200000, 
        0x00100000, 
        0x00080000, 
        0x00040000, 
        0x00020000, 
        0x00010000, 
        0x00008000, 
        0x00004000, 
        0x00002000, 
        0x00001000, 
        0x00000800, 
        0x00000400, 
        0x00000200, 
        0x00000100, 
        0x00000080, 
        0x00000040, 
        0x00000020, 
        0x00000010, 
        0x00000008, 
        0x00000004, 
        0x00000002, 
        0x00000001, 
        0x00000000
    };
    
    for(uint32_t i = 0; i < 33; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "The smallest number of bits needed to represent the given value is %u", 32-i);
        TEST_ASSERT_EQ(get_req_bit_width_ui(vals[i]), 32-i, msg);
    }
}

static void test_get_req_bit_width_ul(void) {
    uint64_t vals[] = {
        0x8000000000000000UL,
        0x4000000000000000UL,
        0x2000000000000000UL,
        0x1000000000000000UL,
        0x0800000000000000UL,
        0x0400000000000000UL,
        0x0200000000000000UL,
        0x0100000000000000UL,
        0x0080000000000000UL,
        0x0040000000000000UL,
        0x0020000000000000UL,
        0x0010000000000000UL,
        0x0008000000000000UL,
        0x0004000000000000UL,
        0x0002000000000000UL,
        0x0001000000000000UL,
        0x0000800000000000UL,
        0x0000400000000000UL,
        0x0000200000000000UL,
        0x0000100000000000UL,
        0x0000080000000000UL,
        0x0000040000000000UL,
        0x0000020000000000UL,
        0x0000010000000000UL,
        0x0000008000000000UL,
        0x0000004000000000UL,
        0x0000002000000000UL,
        0x0000001000000000UL,
        0x0000000800000000UL,
        0x0000000400000000UL,
        0x0000000200000000UL,
        0x0000000100000000UL,
        0x0000000080000000UL,
        0x0000000040000000UL,
        0x0000000020000000UL,
        0x0000000010000000UL,
        0x0000000008000000UL,
        0x0000000004000000UL,
        0x0000000002000000UL,
        0x0000000001000000UL,
        0x0000000000800000UL,
        0x0000000000400000UL,
        0x0000000000200000UL,
        0x0000000000100000UL,
        0x0000000000080000UL,
        0x0000000000040000UL,
        0x0000000000020000UL,
        0x0000000000010000UL,
        0x0000000000008000UL,
        0x0000000000004000UL,
        0x0000000000002000UL,
        0x0000000000001000UL,
        0x0000000000000800UL,
        0x0000000000000400UL,
        0x0000000000000200UL,
        0x0000000000000100UL,
        0x0000000000000080UL,
        0x0000000000000040UL,
        0x0000000000000020UL,
        0x0000000000000010UL,
        0x0000000000000008UL,
        0x0000000000000004UL,
        0x0000000000000002UL,
        0x0000000000000001UL,
        0x0000000000000000UL
    };
    
    for(uint64_t i = 0; i < 65; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "The smallest number of bits needed to represent the given value is %u", 64-i);
        TEST_ASSERT_EQ(get_req_bit_width_ul(vals[i]), 64-i, msg);
    }
}

static void test_get_req_bit_width_ull(void) {
    uint64_t vals[] = {
        0x8000000000000000ULL,
        0x4000000000000000ULL,
        0x2000000000000000ULL,
        0x1000000000000000ULL,
        0x0800000000000000ULL,
        0x0400000000000000ULL,
        0x0200000000000000ULL,
        0x0100000000000000ULL,
        0x0080000000000000ULL,
        0x0040000000000000ULL,
        0x0020000000000000ULL,
        0x0010000000000000ULL,
        0x0008000000000000ULL,
        0x0004000000000000ULL,
        0x0002000000000000ULL,
        0x0001000000000000ULL,
        0x0000800000000000ULL,
        0x0000400000000000ULL,
        0x0000200000000000ULL,
        0x0000100000000000ULL,
        0x0000080000000000ULL,
        0x0000040000000000ULL,
        0x0000020000000000ULL,
        0x0000010000000000ULL,
        0x0000008000000000ULL,
        0x0000004000000000ULL,
        0x0000002000000000ULL,
        0x0000001000000000ULL,
        0x0000000800000000ULL,
        0x0000000400000000ULL,
        0x0000000200000000ULL,
        0x0000000100000000ULL,
        0x0000000080000000ULL,
        0x0000000040000000ULL,
        0x0000000020000000ULL,
        0x0000000010000000ULL,
        0x0000000008000000ULL,
        0x0000000004000000ULL,
        0x0000000002000000ULL,
        0x0000000001000000ULL,
        0x0000000000800000ULL,
        0x0000000000400000ULL,
        0x0000000000200000ULL,
        0x0000000000100000ULL,
        0x0000000000080000ULL,
        0x0000000000040000ULL,
        0x0000000000020000ULL,
        0x0000000000010000ULL,
        0x0000000000008000ULL,
        0x0000000000004000ULL,
        0x0000000000002000ULL,
        0x0000000000001000ULL,
        0x0000000000000800ULL,
        0x0000000000000400ULL,
        0x0000000000000200ULL,
        0x0000000000000100ULL,
        0x0000000000000080ULL,
        0x0000000000000040ULL,
        0x0000000000000020ULL,
        0x0000000000000010ULL,
        0x0000000000000008ULL,
        0x0000000000000004ULL,
        0x0000000000000002ULL,
        0x0000000000000001ULL,
        0x0000000000000000ULL
    };
    
    for(uint64_t i = 0; i < 65; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "The smallest number of bits needed to represent the given value is %u", 64-i);
        TEST_ASSERT_EQ(get_req_bit_width_ull(vals[i]), 64-i, msg);
    }
}

static void test_bit_floor_uc(void) {
    uint8_t vals[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00};

    for(uint8_t i = 0; i < 8; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "the largest integral power of two not greater than the given value is %u", 1U << (7-i));
        TEST_ASSERT_EQ(bit_floor_uc(vals[i]), 1U << (7-i), msg);
    }
}

static void test_bit_floor_us(void) {
    uint16_t vals[] = {
        0x8000, 
        0x4000, 
        0x2000, 
        0x1000, 
        0x0800, 
        0x0400, 
        0x0200, 
        0x0100, 
        0x0080, 
        0x0040, 
        0x0020, 
        0x0010, 
        0x0008, 
        0x0004, 
        0x0002, 
        0x0001, 
        0x0000
    };

    for(uint16_t i = 0; i < 16; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "the largest integral power of two not greater than the given value is %u", 1U << (15-i));
        TEST_ASSERT_EQ(bit_floor_us(vals[i]), 1U << (15-i), msg);
    }
}

static void test_bit_floor_ui(void) {
    uint32_t vals[] = {
        0x80000000,
        0x40000000, 
        0x20000000, 
        0x10000000, 
        0x08000000, 
        0x04000000, 
        0x02000000, 
        0x01000000, 
        0x00800000, 
        0x00400000, 
        0x00200000, 
        0x00100000, 
        0x00080000, 
        0x00040000, 
        0x00020000, 
        0x00010000, 
        0x00008000, 
        0x00004000, 
        0x00002000, 
        0x00001000, 
        0x00000800, 
        0x00000400, 
        0x00000200, 
        0x00000100, 
        0x00000080, 
        0x00000040, 
        0x00000020, 
        0x00000010, 
        0x00000008, 
        0x00000004, 
        0x00000002, 
        0x00000001, 
        0x00000000
    };
    
    for(uint32_t i = 0; i < 32; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "the largest integral power of two not greater than the given value is %u", 1U << (31-i));
        TEST_ASSERT_EQ(bit_floor_ui(vals[i]), 1U << (31-i), msg);
    }
}

static void test_bit_floor_ul(void) {
    uint64_t vals[] = {
        0x8000000000000000UL,
        0x4000000000000000UL,
        0x2000000000000000UL,
        0x1000000000000000UL,
        0x0800000000000000UL,
        0x0400000000000000UL,
        0x0200000000000000UL,
        0x0100000000000000UL,
        0x0080000000000000UL,
        0x0040000000000000UL,
        0x0020000000000000UL,
        0x0010000000000000UL,
        0x0008000000000000UL,
        0x0004000000000000UL,
        0x0002000000000000UL,
        0x0001000000000000UL,
        0x0000800000000000UL,
        0x0000400000000000UL,
        0x0000200000000000UL,
        0x0000100000000000UL,
        0x0000080000000000UL,
        0x0000040000000000UL,
        0x0000020000000000UL,
        0x0000010000000000UL,
        0x0000008000000000UL,
        0x0000004000000000UL,
        0x0000002000000000UL,
        0x0000001000000000UL,
        0x0000000800000000UL,
        0x0000000400000000UL,
        0x0000000200000000UL,
        0x0000000100000000UL,
        0x0000000080000000UL,
        0x0000000040000000UL,
        0x0000000020000000UL,
        0x0000000010000000UL,
        0x0000000008000000UL,
        0x0000000004000000UL,
        0x0000000002000000UL,
        0x0000000001000000UL,
        0x0000000000800000UL,
        0x0000000000400000UL,
        0x0000000000200000UL,
        0x0000000000100000UL,
        0x0000000000080000UL,
        0x0000000000040000UL,
        0x0000000000020000UL,
        0x0000000000010000UL,
        0x0000000000008000UL,
        0x0000000000004000UL,
        0x0000000000002000UL,
        0x0000000000001000UL,
        0x0000000000000800UL,
        0x0000000000000400UL,
        0x0000000000000200UL,
        0x0000000000000100UL,
        0x0000000000000080UL,
        0x0000000000000040UL,
        0x0000000000000020UL,
        0x0000000000000010UL,
        0x0000000000000008UL,
        0x0000000000000004UL,
        0x0000000000000002UL,
        0x0000000000000001UL,
        0x0000000000000000UL
    };
    
    for(uint64_t i = 0; i < 64; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "the largest integral power of two not greater than the given value is %u", 1U << (63-i));
        TEST_ASSERT_EQ(bit_floor_ul(vals[i]), 1UL << (63-i), msg);
    }
}

static void test_bit_floor_ull(void) {
    uint64_t vals[] = {
        0x8000000000000000ULL,
        0x4000000000000000ULL,
        0x2000000000000000ULL,
        0x1000000000000000ULL,
        0x0800000000000000ULL,
        0x0400000000000000ULL,
        0x0200000000000000ULL,
        0x0100000000000000ULL,
        0x0080000000000000ULL,
        0x0040000000000000ULL,
        0x0020000000000000ULL,
        0x0010000000000000ULL,
        0x0008000000000000ULL,
        0x0004000000000000ULL,
        0x0002000000000000ULL,
        0x0001000000000000ULL,
        0x0000800000000000ULL,
        0x0000400000000000ULL,
        0x0000200000000000ULL,
        0x0000100000000000ULL,
        0x0000080000000000ULL,
        0x0000040000000000ULL,
        0x0000020000000000ULL,
        0x0000010000000000ULL,
        0x0000008000000000ULL,
        0x0000004000000000ULL,
        0x0000002000000000ULL,
        0x0000001000000000ULL,
        0x0000000800000000ULL,
        0x0000000400000000ULL,
        0x0000000200000000ULL,
        0x0000000100000000ULL,
        0x0000000080000000ULL,
        0x0000000040000000ULL,
        0x0000000020000000ULL,
        0x0000000010000000ULL,
        0x0000000008000000ULL,
        0x0000000004000000ULL,
        0x0000000002000000ULL,
        0x0000000001000000ULL,
        0x0000000000800000ULL,
        0x0000000000400000ULL,
        0x0000000000200000ULL,
        0x0000000000100000ULL,
        0x0000000000080000ULL,
        0x0000000000040000ULL,
        0x0000000000020000ULL,
        0x0000000000010000ULL,
        0x0000000000008000ULL,
        0x0000000000004000ULL,
        0x0000000000002000ULL,
        0x0000000000001000ULL,
        0x0000000000000800ULL,
        0x0000000000000400ULL,
        0x0000000000000200ULL,
        0x0000000000000100ULL,
        0x0000000000000080ULL,
        0x0000000000000040ULL,
        0x0000000000000020ULL,
        0x0000000000000010ULL,
        0x0000000000000008ULL,
        0x0000000000000004ULL,
        0x0000000000000002ULL,
        0x0000000000000001ULL,
        0x0000000000000000ULL
    };
    
    for(uint64_t i = 0; i < 64; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "the largest integral power of two not greater than the given value is %u", 1U << (63-i));
        TEST_ASSERT_EQ(bit_floor_ull(vals[i]), 1ULL << (63-i), msg);
    }
}

static void test_bit_ceil_uc(void) {
    uint8_t vals[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00};

    for(uint8_t i = 0; i < 9; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "the smallest integral power of two not less than the given value is %u", 2U << (7-(i-1)));
        TEST_ASSERT_EQ(bit_ceil_uc(vals[i]), 2U << (7-(i-1)), msg);
    }
}

static void test_bit_ceil_us(void) {
    uint16_t vals[] = {
        0x8000, 
        0x4000, 
        0x2000, 
        0x1000, 
        0x0800, 
        0x0400, 
        0x0200, 
        0x0100, 
        0x0080, 
        0x0040, 
        0x0020, 
        0x0010, 
        0x0008, 
        0x0004, 
        0x0002, 
        0x0001, 
        0x0000
    };

    for(uint16_t i = 0; i < 17; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "the smallest integral power of two not less than the given value is %u", 2U << (15-(i-1)));
        TEST_ASSERT_EQ(bit_ceil_us(vals[i]), 2U << (15-(i-1)), msg);
    }
}

static void test_bit_ceil_ui(void) {
    uint32_t vals[] = {
        0x80000000,
        0x40000000, 
        0x20000000, 
        0x10000000, 
        0x08000000, 
        0x04000000, 
        0x02000000, 
        0x01000000, 
        0x00800000, 
        0x00400000, 
        0x00200000, 
        0x00100000, 
        0x00080000, 
        0x00040000, 
        0x00020000, 
        0x00010000, 
        0x00008000, 
        0x00004000, 
        0x00002000, 
        0x00001000, 
        0x00000800, 
        0x00000400, 
        0x00000200, 
        0x00000100, 
        0x00000080, 
        0x00000040, 
        0x00000020, 
        0x00000010, 
        0x00000008, 
        0x00000004, 
        0x00000002, 
        0x00000001, 
        0x00000000
    };
    
    for(uint32_t i = 0; i < 33; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "the smallest integral power of two not less than the given value is %u", 2U << (31-(i-1)));
        TEST_ASSERT_EQ(bit_ceil_ui(vals[i]), 2U << (31-(i-1)), msg);
    }
}

static void test_bit_ceil_ul(void) {
    uint64_t vals[] = {
        0x8000000000000000UL,
        0x4000000000000000UL,
        0x2000000000000000UL,
        0x1000000000000000UL,
        0x0800000000000000UL,
        0x0400000000000000UL,
        0x0200000000000000UL,
        0x0100000000000000UL,
        0x0080000000000000UL,
        0x0040000000000000UL,
        0x0020000000000000UL,
        0x0010000000000000UL,
        0x0008000000000000UL,
        0x0004000000000000UL,
        0x0002000000000000UL,
        0x0001000000000000UL,
        0x0000800000000000UL,
        0x0000400000000000UL,
        0x0000200000000000UL,
        0x0000100000000000UL,
        0x0000080000000000UL,
        0x0000040000000000UL,
        0x0000020000000000UL,
        0x0000010000000000UL,
        0x0000008000000000UL,
        0x0000004000000000UL,
        0x0000002000000000UL,
        0x0000001000000000UL,
        0x0000000800000000UL,
        0x0000000400000000UL,
        0x0000000200000000UL,
        0x0000000100000000UL,
        0x0000000080000000UL,
        0x0000000040000000UL,
        0x0000000020000000UL,
        0x0000000010000000UL,
        0x0000000008000000UL,
        0x0000000004000000UL,
        0x0000000002000000UL,
        0x0000000001000000UL,
        0x0000000000800000UL,
        0x0000000000400000UL,
        0x0000000000200000UL,
        0x0000000000100000UL,
        0x0000000000080000UL,
        0x0000000000040000UL,
        0x0000000000020000UL,
        0x0000000000010000UL,
        0x0000000000008000UL,
        0x0000000000004000UL,
        0x0000000000002000UL,
        0x0000000000001000UL,
        0x0000000000000800UL,
        0x0000000000000400UL,
        0x0000000000000200UL,
        0x0000000000000100UL,
        0x0000000000000080UL,
        0x0000000000000040UL,
        0x0000000000000020UL,
        0x0000000000000010UL,
        0x0000000000000008UL,
        0x0000000000000004UL,
        0x0000000000000002UL,
        0x0000000000000001UL,
        0x0000000000000000UL
    };
    
    for(uint64_t i = 0; i < 65; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "the smallest integral power of two not less than the given value is %u", 2UL << (63-(i-1)));
        TEST_ASSERT_EQ(bit_ceil_ul(vals[i]), 2UL << (63-(i-1)), msg);
    }
}

static void test_bit_ceil_ull(void) {
    uint64_t vals[] = {
        0x8000000000000000ULL,
        0x4000000000000000ULL,
        0x2000000000000000ULL,
        0x1000000000000000ULL,
        0x0800000000000000ULL,
        0x0400000000000000ULL,
        0x0200000000000000ULL,
        0x0100000000000000ULL,
        0x0080000000000000ULL,
        0x0040000000000000ULL,
        0x0020000000000000ULL,
        0x0010000000000000ULL,
        0x0008000000000000ULL,
        0x0004000000000000ULL,
        0x0002000000000000ULL,
        0x0001000000000000ULL,
        0x0000800000000000ULL,
        0x0000400000000000ULL,
        0x0000200000000000ULL,
        0x0000100000000000ULL,
        0x0000080000000000ULL,
        0x0000040000000000ULL,
        0x0000020000000000ULL,
        0x0000010000000000ULL,
        0x0000008000000000ULL,
        0x0000004000000000ULL,
        0x0000002000000000ULL,
        0x0000001000000000ULL,
        0x0000000800000000ULL,
        0x0000000400000000ULL,
        0x0000000200000000ULL,
        0x0000000100000000ULL,
        0x0000000080000000ULL,
        0x0000000040000000ULL,
        0x0000000020000000ULL,
        0x0000000010000000ULL,
        0x0000000008000000ULL,
        0x0000000004000000ULL,
        0x0000000002000000ULL,
        0x0000000001000000ULL,
        0x0000000000800000ULL,
        0x0000000000400000ULL,
        0x0000000000200000ULL,
        0x0000000000100000ULL,
        0x0000000000080000ULL,
        0x0000000000040000ULL,
        0x0000000000020000ULL,
        0x0000000000010000ULL,
        0x0000000000008000ULL,
        0x0000000000004000ULL,
        0x0000000000002000ULL,
        0x0000000000001000ULL,
        0x0000000000000800ULL,
        0x0000000000000400ULL,
        0x0000000000000200ULL,
        0x0000000000000100ULL,
        0x0000000000000080ULL,
        0x0000000000000040ULL,
        0x0000000000000020ULL,
        0x0000000000000010ULL,
        0x0000000000000008ULL,
        0x0000000000000004ULL,
        0x0000000000000002ULL,
        0x0000000000000001ULL,
        0x0000000000000000ULL
    };
    
    for(uint64_t i = 0; i < 65; i++) {
        char msg[100];
        snprintf(msg, sizeof(msg), "the smallest integral power of two not less than the given value is %u", 2ULL << (63-(i-1)));
        TEST_ASSERT_EQ(bit_ceil_ull(vals[i]), 2ULL << (63-(i-1)), msg);
    }
}

static void test_rotate_left_uc(void) {
    TEST_ASSERT_EQ(rotate_left_uc((uint8_t)0xB7, 0), (uint8_t)0xB7, "Shift the value left by 0 bits");
    TEST_ASSERT_EQ(rotate_left_uc((uint8_t)0xB7, 3), (uint8_t)0xBD, "Shift the value left by 3 bits");
    TEST_ASSERT_EQ(rotate_left_uc((uint8_t)0xB7, 8), (uint8_t)0xB7, "Shift the value left by 8 bits");
}

static void test_rotate_left_us(void) {
    TEST_ASSERT_EQ(rotate_left_us((uint16_t)0x7C9E, 0), (uint16_t)0x7C9E, "Shift the value left by 0 bits");
    TEST_ASSERT_EQ(rotate_left_us((uint16_t)0x7C9E, 3), (uint16_t)0xE4F3, "Shift the value left by 3 bits");
    TEST_ASSERT_EQ(rotate_left_us((uint16_t)0x7C9E, 8), (uint16_t)0x9E7C, "Shift the value left by 8 bits");
    TEST_ASSERT_EQ(rotate_left_us((uint16_t)0x7C9E, 12), (uint16_t)0xE7C9, "Shift the value left by 12 bits");
    TEST_ASSERT_EQ(rotate_left_us((uint16_t)0x7C9E, 16), (uint16_t)0x7C9E, "Shift the value left by 16 bits");
}

static void test_rotate_left_ui(void) {
    TEST_ASSERT_EQ(rotate_left_ui((uint32_t)0xC4A9F27D, 0), (uint32_t)0xC4A9F27D, "Shift the value left by 0 bits");
    TEST_ASSERT_EQ(rotate_left_ui((uint32_t)0xC4A9F27D, 3), (uint32_t)0x254F93EE, "Shift the value left by 3 bits");
    TEST_ASSERT_EQ(rotate_left_ui((uint32_t)0xC4A9F27D, 8), (uint32_t)0xA9F27DC4, "Shift the value left by 8 bits");
    TEST_ASSERT_EQ(rotate_left_ui((uint32_t)0xC4A9F27D, 12), (uint32_t)0x9F27DC4A, "Shift the value left by 12 bits");
    TEST_ASSERT_EQ(rotate_left_ui((uint32_t)0xC4A9F27D, 16), (uint32_t)0xF27DC4A9, "Shift the value left by 16 bits");
    TEST_ASSERT_EQ(rotate_left_ui((uint32_t)0xC4A9F27D, 24), (uint32_t)0x7DC4A9F2, "Shift the value left by 24 bits");
    TEST_ASSERT_EQ(rotate_left_ui((uint32_t)0xC4A9F27D, 32), (uint32_t)0xC4A9F27D, "Shift the value left by 32 bits");
}

static void test_rotate_left_ul(void) {
    TEST_ASSERT_EQ(rotate_left_ul(0x9A7F3C5D82B1E6A4UL, 0), 0x9A7F3C5D82B1E6A4UL, "Shift the value left by 0 bits");
    TEST_ASSERT_EQ(rotate_left_ul(0x9A7F3C5D82B1E6A4UL, 3), 0xD3F9E2EC158F3524UL, "Shift the value left by 3 bits");
    TEST_ASSERT_EQ(rotate_left_ul(0x9A7F3C5D82B1E6A4UL, 8), 0x7F3C5D82B1E6A49AUL, "Shift the value left by 8 bits");
    TEST_ASSERT_EQ(rotate_left_ul(0x9A7F3C5D82B1E6A4UL, 12), 0xF3C5D82B1E6A49A7UL, "Shift the value left by 12 bits");
    TEST_ASSERT_EQ(rotate_left_ul(0x9A7F3C5D82B1E6A4UL, 16), 0x3C5D82B1E6A49A7FUL, "Shift the value left by 16 bits");
    TEST_ASSERT_EQ(rotate_left_ul(0x9A7F3C5D82B1E6A4UL, 24), 0x5D82B1E6A49A7F3CUL, "Shift the value left by 24 bits");
    TEST_ASSERT_EQ(rotate_left_ul(0x9A7F3C5D82B1E6A4UL, 32), 0x82B1E6A49A7F3C5DUL, "Shift the value left by 32 bits");
    TEST_ASSERT_EQ(rotate_left_ul(0x9A7F3C5D82B1E6A4UL, 46), 0x79A9269FCF1760ACUL, "Shift the value left by 46 bits");
    TEST_ASSERT_EQ(rotate_left_ul(0x9A7F3C5D82B1E6A4UL, 64), 0x9A7F3C5D82B1E6A4UL, "Shift the value left by 64 bits");
}

static void test_rotate_left_ull(void) {
    TEST_ASSERT_EQ(rotate_left_ull(0x9A7F3C5D82B1E6A4ULL, 0), 0x9A7F3C5D82B1E6A4ULL, "Shift the value left by 0 bits");
    TEST_ASSERT_EQ(rotate_left_ull(0x9A7F3C5D82B1E6A4ULL, 3), 0xD3F9E2EC158F3524ULL, "Shift the value left by 3 bits");
    TEST_ASSERT_EQ(rotate_left_ull(0x9A7F3C5D82B1E6A4ULL, 8), 0x7F3C5D82B1E6A49AULL, "Shift the value left by 8 bits");
    TEST_ASSERT_EQ(rotate_left_ull(0x9A7F3C5D82B1E6A4ULL, 12), 0xF3C5D82B1E6A49A7ULL, "Shift the value left by 12 bits");
    TEST_ASSERT_EQ(rotate_left_ull(0x9A7F3C5D82B1E6A4ULL, 16), 0x3C5D82B1E6A49A7FULL, "Shift the value left by 16 bits");
    TEST_ASSERT_EQ(rotate_left_ull(0x9A7F3C5D82B1E6A4ULL, 24), 0x5D82B1E6A49A7F3CULL, "Shift the value left by 24 bits");
    TEST_ASSERT_EQ(rotate_left_ull(0x9A7F3C5D82B1E6A4ULL, 32), 0x82B1E6A49A7F3C5DULL, "Shift the value left by 32 bits");
    TEST_ASSERT_EQ(rotate_left_ull(0x9A7F3C5D82B1E6A4ULL, 46), 0x79A9269FCF1760ACULL, "Shift the value left by 46 bits");
    TEST_ASSERT_EQ(rotate_left_ull(0x9A7F3C5D82B1E6A4ULL, 64), 0x9A7F3C5D82B1E6A4ULL, "Shift the value left by 64 bits");
}

static void test_rotate_right_uc(void) {
    TEST_ASSERT_EQ(rotate_right_uc((uint8_t)0xB7, 0), (uint8_t)0xB7, "Shift the value right by 0 bits");
    TEST_ASSERT_EQ(rotate_right_uc((uint8_t)0xB7, 3), (uint8_t)0xF6, "Shift the value right by 3 bits");
    TEST_ASSERT_EQ(rotate_right_uc((uint8_t)0xB7, 8), (uint8_t)0xB7, "Shift the value right by 8 bits");
}

static void test_rotate_right_us(void) {
    TEST_ASSERT_EQ(rotate_right_us((uint16_t)0x7C9E, 0), (uint16_t)0x7C9E, "Shift the value right by 0 bits");
    TEST_ASSERT_EQ(rotate_right_us((uint16_t)0x7C9E, 3), (uint16_t)0xCF93, "Shift the value right by 3 bits");
    TEST_ASSERT_EQ(rotate_right_us((uint16_t)0x7C9E, 8), (uint16_t)0x9E7C, "Shift the value right by 8 bits");
    TEST_ASSERT_EQ(rotate_right_us((uint16_t)0x7C9E, 12), (uint16_t)0xC9E7, "Shift the value right by 12 bits");
    TEST_ASSERT_EQ(rotate_right_us((uint16_t)0x7C9E, 16), (uint16_t)0x7C9E, "Shift the value right by 16 bits");
}

static void test_rotate_right_ui(void) {
    TEST_ASSERT_EQ(rotate_right_ui((uint32_t)0xC4A9F27D, 0), (uint32_t)0xC4A9F27D, "Shift the value right by 0 bits");
    TEST_ASSERT_EQ(rotate_right_ui((uint32_t)0xC4A9F27D, 3), (uint32_t)0xB8953E4F, "Shift the value right by 3 bits");
    TEST_ASSERT_EQ(rotate_right_ui((uint32_t)0xC4A9F27D, 8), (uint32_t)0x7DC4A9F2, "Shift the value right by 8 bits");
    TEST_ASSERT_EQ(rotate_right_ui((uint32_t)0xC4A9F27D, 12), (uint32_t)0x27DC4A9F, "Shift the value right by 12 bits");
    TEST_ASSERT_EQ(rotate_right_ui((uint32_t)0xC4A9F27D, 16), (uint32_t)0xF27DC4A9, "Shift the value right by 16 bits");
    TEST_ASSERT_EQ(rotate_right_ui((uint32_t)0xC4A9F27D, 24), (uint32_t)0xA9F27DC4, "Shift the value right by 24 bits");
    TEST_ASSERT_EQ(rotate_right_ui((uint32_t)0xC4A9F27D, 32), (uint32_t)0xC4A9F27D, "Shift the value right by 32 bits");
}

static void test_rotate_right_ul(void) {
    TEST_ASSERT_EQ(rotate_right_ul(0x9A7F3C5D82B1E6A4UL, 0), 0x9A7F3C5D82B1E6A4UL, "Shift the value right by 0 bits");
    TEST_ASSERT_EQ(rotate_right_ul(0x9A7F3C5D82B1E6A4UL, 3), 0x934FE78BB0563CD4UL, "Shift the value right by 3 bits");
    TEST_ASSERT_EQ(rotate_right_ul(0x9A7F3C5D82B1E6A4UL, 8), 0xA49A7F3C5D82B1E6UL, "Shift the value right by 8 bits");
    TEST_ASSERT_EQ(rotate_right_ul(0x9A7F3C5D82B1E6A4UL, 12), 0x6A49A7F3C5D82B1EUL, "Shift the value right by 12 bits");
    TEST_ASSERT_EQ(rotate_right_ul(0x9A7F3C5D82B1E6A4UL, 16), 0xE6A49A7F3C5D82B1UL, "Shift the value right by 16 bits");
    TEST_ASSERT_EQ(rotate_right_ul(0x9A7F3C5D82B1E6A4UL, 24), 0xB1E6A49A7F3C5D82UL, "Shift the value right by 24 bits");
    TEST_ASSERT_EQ(rotate_right_ul(0x9A7F3C5D82B1E6A4UL, 32), 0x82B1E6A49A7F3C5DUL, "Shift the value right by 32 bits");
    TEST_ASSERT_EQ(rotate_right_ul(0x9A7F3C5D82B1E6A4UL, 46), 0xF1760AC79A9269FCUL, "Shift the value right by 46 bits");
    TEST_ASSERT_EQ(rotate_right_ul(0x9A7F3C5D82B1E6A4UL, 64), 0x9A7F3C5D82B1E6A4UL, "Shift the value right by 64 bits");
}

static void test_rotate_right_ull(void) {
    TEST_ASSERT_EQ(rotate_right_ull(0x9A7F3C5D82B1E6A4UL, 0), 0x9A7F3C5D82B1E6A4UL, "Shift the value right by 0 bits");
    TEST_ASSERT_EQ(rotate_right_ull(0x9A7F3C5D82B1E6A4UL, 3), 0x934FE78BB0563CD4UL, "Shift the value right by 3 bits");
    TEST_ASSERT_EQ(rotate_right_ull(0x9A7F3C5D82B1E6A4UL, 8), 0xA49A7F3C5D82B1E6UL, "Shift the value right by 8 bits");
    TEST_ASSERT_EQ(rotate_right_ull(0x9A7F3C5D82B1E6A4UL, 12), 0x6A49A7F3C5D82B1EUL, "Shift the value right by 12 bits");
    TEST_ASSERT_EQ(rotate_right_ull(0x9A7F3C5D82B1E6A4UL, 16), 0xE6A49A7F3C5D82B1UL, "Shift the value right by 16 bits");
    TEST_ASSERT_EQ(rotate_right_ull(0x9A7F3C5D82B1E6A4UL, 24), 0xB1E6A49A7F3C5D82UL, "Shift the value right by 24 bits");
    TEST_ASSERT_EQ(rotate_right_ull(0x9A7F3C5D82B1E6A4UL, 32), 0x82B1E6A49A7F3C5DUL, "Shift the value right by 32 bits");
    TEST_ASSERT_EQ(rotate_right_ull(0x9A7F3C5D82B1E6A4UL, 46), 0xF1760AC79A9269FCUL, "Shift the value right by 46 bits");
    TEST_ASSERT_EQ(rotate_right_ull(0x9A7F3C5D82B1E6A4UL, 64), 0x9A7F3C5D82B1E6A4UL, "Shift the value right by 64 bits");
}

void run_bit_manip_tests(void)
{
    test_get_DWord0();
    test_get_DWord1();
    test_M_DoubleWordInt0();
    test_M_DoubleWordInt1(); 
    test_get_Word0_uint64();
    test_get_Word0_uint32();
    test_M_Word0();
    test_get_Word1_uint64();
    test_get_Word1_uint32();
    test_get_Word2_uint64();
    test_get_Word3_uint64();
    test_M_Word1();
    test_M_Word2();
    test_M_Word3(); 
    test_M_WordInt0();
    test_M_WordInt1();
    test_M_WordInt2();
    test_M_WordInt3();
    test_BITSPERBYTE();
    test_M_ByteN();
    test_M_Byte0();
    test_M_Byte1();
    test_M_Byte2();
    test_M_Byte3();
    test_M_Byte4();
    test_M_Byte5();
    test_M_Byte6();
    test_M_Byte7();
    test_M_ByteInt0();
    test_M_ByteInt1();
    test_M_ByteInt2();
    test_M_ByteInt3();
    test_M_ByteInt4();
    test_M_ByteInt5();
    test_M_ByteInt6();
    test_M_ByteInt7();
    test_M_Nibble0();
    test_M_Nibble1();
    test_M_Nibble2();
    test_M_Nibble3();
    test_M_Nibble4();
    test_M_Nibble5();
    test_M_Nibble6();
    test_M_Nibble7();
    test_M_Nibble8();
    test_M_Nibble9();
    test_M_Nibble10();
    test_M_Nibble11();
    test_M_Nibble12();
    test_M_Nibble13();
    test_M_Nibble14();
    test_M_Nibble15();
    test_nibbles_To_Byte();
    test_M_NibblesTo1ByteValue();
    test_bytes_To_Uint16();
    test_M_BytesTo2ByteValue();
    test_bytes_To_Uint32();
    test_M_BytesTo4ByteValue();
    test_bytes_To_Uint64();
    test_M_BytesTo8ByteValue();
    test_words_To_Uint32();
    test_M_WordsTo4ByteValue();
    test_words_To_Uint64();
    test_M_WordsTo8ByteValue();
    test_dwords_To_Uint64();
    test_M_DWordsTo8ByteValue();
    test_ROUNDF();
    test_is_generic_int_valid();
    test_get_bit_range_uint8();
    test_get_bit_range_uint16();
    test_get_8bit_range_uint16();
    test_get_bit_range_uint32();
    test_get_8bit_range_uint32();
    test_get_16bit_range_uint32();
    test_get_bit_range_uint64();
    test_get_8bit_range_uint64();
    test_get_16bit_range_uint64();
    test_get_32bit_range_uint64();
    test_get_bit_range_int8();
    test_get_bit_range_int16();
    test_get_8bit_range_int16();
    test_get_bit_range_int32();
    test_get_8bit_range_int32();
    test_get_16bit_range_int32();
    test_get_bit_range_int64();
    test_get_8bit_range_int64();
    test_get_16bit_range_int64();
    test_get_32bit_range_int64();
    test_M_GETBITRANGE();
    test_M_IGETBITRANGE();
    test_M_BitN();
    test_M_BitN8();
    test_M_BitN16();
    test_M_BitN32();
    test_M_BitN64();
    test_BIT0();
    test_BIT1();
    test_BIT2();
    test_BIT3();
    test_BIT4();
    test_BIT5();
    test_BIT6();
    test_BIT7();
    test_BIT8();
    test_BIT9();
    test_BIT10();
    test_BIT11();
    test_BIT12();
    test_BIT13();
    test_BIT14();
    test_BIT15();
    test_BIT16();
    test_BIT17();
    test_BIT18();
    test_BIT19();
    test_BIT20();
    test_BIT21();
    test_BIT22();
    test_BIT23();
    test_BIT24();
    test_BIT25();
    test_BIT26();
    test_BIT27();
    test_BIT28();
    test_BIT29();
    test_BIT30();
    test_BIT31();
    test_BIT32();
    test_BIT33();
    test_BIT34();
    test_BIT35();
    test_BIT36();
    test_BIT37();
    test_BIT38();
    test_BIT39();
    test_BIT40();
    test_BIT41();
    test_BIT42();
    test_BIT43();
    test_BIT44();
    test_BIT45();
    test_BIT46();
    test_BIT47();
    test_BIT48();
    test_BIT49();
    test_BIT50();
    test_BIT51();
    test_BIT52();
    test_BIT53();
    test_BIT54();
    test_BIT55();
    test_BIT56();
    test_BIT57();
    test_BIT58();
    test_BIT59();
    test_BIT60();
    test_BIT61();
    test_BIT62();
    test_BIT63();
    test_M_SET_BIT();
    test_M_CLEAR_BIT();
    test_set_uint8_bit();
    test_set_uint16_bit();
    test_set_uint32_bit();
    test_set_uint64_bit();
    test_clear_uint8_bit();
    test_clear_uint16_bit();
    test_clear_uint32_bit();
    test_clear_uint64_bit();
    test_M_CLEAR_BIT8();
    test_M_CLEAR_BIT16();
    test_M_CLEAR_BIT32();
    test_M_CLEAR_BIT64();
    test_M_SET_BIT8();
    test_M_SET_BIT16();
    test_M_SET_BIT32();
    test_M_SET_BIT64();
    test_n_swap_8();
    test_nibble_Swap();
    test_b_swap_16();
    test_b_swap_32();
    test_b_swap_64();
    test_by_swap_64();
    test_byte_Swap_16();
    test_byte_Swap_Int16();
    test_byte_Swap_32();
    test_byte_Swap_Int32();
    test_w_swap_32();
    test_word_Swap_32();
    test_byte_Swap_64();
    test_word_Swap_64();
    test_double_Word_Swap_64();
    test_get_Bytes_To_16();
    test_get_Bytes_To_32();
    test_get_Bytes_To_64();
    test_be16_to_host();
    test_be32_to_host();
    test_be64_to_host();
    test_host_to_be16();
    test_host_to_be32();
    test_host_to_be64();
    test_host_to_le16();
    test_host_to_le32();
    test_host_to_le64();
    test_le16_to_host();
    test_le32_to_host();
    test_le64_to_host();
    test_big_To_Little_Endian_16();
    test_big_To_Little_Endian_32();
    test_count_leading_zeros_uc();
    test_count_leading_zeros_us();
    test_count_leading_zeros_ui();
    test_count_leading_zeros_ul();
    test_count_leading_zeros_ull();
    test_count_leading_ones_uc();
    test_count_leading_ones_us();
    test_count_leading_ones_ui();
    test_count_leading_ones_ul();
    test_count_leading_ones_ull();
    test_count_trailing_zeros_uc();
    test_count_trailing_zeros_us();
    test_count_trailing_zeros_ui();
    test_count_trailing_zeros_ul();
    test_count_trailing_zeros_ull();
    test_count_trailing_ones_uc();
    test_count_trailing_ones_us();
    test_count_trailing_ones_ui();
    test_count_trailing_ones_ul();
    test_count_trailing_ones_ull();
    test_first_leading_one_uc();
    test_first_leading_one_us();
    test_first_leading_one_ui();
    test_first_leading_one_ul();
    test_first_leading_one_ull();
    test_first_leading_zero_uc();
    test_first_leading_zero_us();
    test_first_leading_zero_ui();
    test_first_leading_zero_ul();
    test_first_leading_zero_ull();
    test_count_ones_uc();
    test_count_ones_us();
    test_count_ones_ui();
    test_count_ones_ul();
    test_count_ones_ull();
    test_count_zeros_uc();
    test_count_zeros_us();
    test_count_zeros_ui();
    test_count_zeros_ul();
    test_count_zeros_ull();
    test_has_single_bit_uc();
    test_has_single_bit_us();
    test_has_single_bit_ui();
    test_has_single_bit_ul();
    test_has_single_bit_ull();
    test_get_req_bit_width_uc();
    test_get_req_bit_width_us();
    test_get_req_bit_width_ui();
    test_get_req_bit_width_ul();
    test_get_req_bit_width_ull();
    test_bit_floor_uc();
    test_bit_floor_us();
    test_bit_floor_ui();
    test_bit_floor_ul();
    test_bit_floor_ull();
    test_rotate_left_uc();
    test_rotate_left_us();
    test_rotate_left_ui();
    test_rotate_left_ul();
    test_rotate_left_ull();
    test_rotate_right_uc();
    test_rotate_right_us();
    test_rotate_right_ui();
    test_rotate_right_ul();
    test_rotate_right_ull();
}


