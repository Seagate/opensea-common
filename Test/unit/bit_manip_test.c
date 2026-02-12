#include "../framework/test_framework.h"
#include "../../include/bit_manip.h"
#include "../testConstants.h"

static void test_get_DWord0(void) {
    TEST_ASSERT_EQ(get_DWord0(SERIAL_NUM), LOWER4BYTE, "get lower 32 bits of uint64_t");
}

static void test_get_DWord1(void) {
    TEST_ASSERT_EQ(get_DWord1(SERIAL_NUM), HIGHER4BYTE, "get upper 32 bits of uint64_t");
}

static void test_M_DoubleWordInt0(void) {
    TEST_ASSERT_EQ(M_DoubleWordInt0(SERIAL_NUM), (int32_t)(LOWER4BYTE), "get lower 32 bits of uint64_t as int32_t");
}

static void test_M_DoubleWordInt1(void) {
    TEST_ASSERT_EQ(M_DoubleWordInt1(SERIAL_NUM), (int32_t)(HIGHER4BYTE), "get upper 32 bits of uint64_t as int32_t");
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
    TEST_ASSERT_EQ(M_WordInt0((uint64_t)SERIAL_NUM), (int16_t)(LOWER2BYTE), "get lower 16 bits of uint64_t as int16_t");
}

static void test_M_WordInt1(void) {
    TEST_ASSERT_EQ(M_WordInt1((uint64_t)SERIAL_NUM), (int16_t)((LOWER4BYTE >> 16) & 0xFFFF), "get the second lowest 16 bits of uint64_t as int16_t");
}

static void test_M_WordInt2(void) {
    TEST_ASSERT_EQ(M_WordInt2((uint64_t)SERIAL_NUM), (int16_t)(HIGHER4BYTE & 0xFFFF), "get second highest 16 bits of uint64_t as int16_t");
}

static void test_M_WordInt3(void) {
    TEST_ASSERT_EQ(M_WordInt3((uint64_t)SERIAL_NUM), (int16_t)(HIGHER2BYTE), "get highest 16 bits of uint64_t as int16_t");
}

static void test_BITSPERBYTE(void) {
    TEST_ASSERT_EQ(BITSPERBYTE, 8, "BITSPERBYTE should be 8");
}

// Assuming 64-bit integer for M_ByteN tests
// UINT8_MAX = 0xFF
static void test_M_ByteN(void) {
    for (int n = 0; n < 8; n++) {
        char msg[50];
        snprintf(msg, sizeof(msg), "Set %dth byte to all 1s", n);
        TEST_ASSERT_EQ(M_ByteN(n), (UINT8_MAX << (n * BITSPERBYTE)), msg);
    }
}

static void test_M_Byte0(void) {
    TEST_ASSERT_EQ(M_ByteN(0), (uint8_t)(UINT8_MAX << (0 * BITSPERBYTE)), "Set 0th byte to all 1s");
}

static void test_M_Byte1(void) {
    TEST_ASSERT_EQ(M_Byte1(1), (uint8_t)(UINT8_MAX << (1 * BITSPERBYTE)), "Set 1st byte to all 1s");
}

static void test_M_Byte2(void) {
    TEST_ASSERT_EQ(M_Byte2(2), (uint8_t)(UINT8_MAX << (2 * BITSPERBYTE)), "Set 2nd byte to all 1s");
}

static void test_M_Byte3(void) {
    TEST_ASSERT_EQ(M_Byte3(3), (uint8_t)(UINT8_MAX << (3 * BITSPERBYTE)), "Set 3rd byte to all 1s");
}

static void test_M_Byte4(void) {
    TEST_ASSERT_EQ(M_Byte4(4), (uint8_t)(UINT8_MAX << (4 * BITSPERBYTE)), "Set 4th byte to all 1s");
}

static void test_M_Byte5(void) {
    TEST_ASSERT_EQ(M_Byte5(5), (uint8_t)(UINT8_MAX << (5 * BITSPERBYTE)), "Set 5th byte to all 1s");
}

static void test_M_Byte6(void) {
    TEST_ASSERT_EQ(M_Byte6(6), (uint8_t)(UINT8_MAX << (6 * BITSPERBYTE)), "Set 6th byte to all 1s");
}

static void test_M_Byte7(void) {
    TEST_ASSERT_EQ(M_Byte7(7), (uint8_t)(UINT8_MAX << (7 * BITSPERBYTE)), "Set 7th byte to all 1s");
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

// static void test_ROUNDF(void) {
//     TEST_ASSERT_FLOAT_EQ(ROUNDF(3.14159f, 100), 3.14f, 0.001f, "Rounds 3.14159 to 2 decimal places");
//     TEST_ASSERT_FLOAT_EQ(ROUNDF(2.999f, 100), 2.99f, 0.001f, "Rounds 2.999 to 2 decimal places");
//     TEST_ASSERT_FLOAT_EQ(ROUNDF(-5.678f, 10), -5.6f, 0.001f, "Rounds -5.678 to 1 decimal place");
//     TEST_ASSERT_FLOAT_EQ(ROUNDF(123.456f, 1000), 123.456f, 0.001f, "Rounds 123.456 to 3 decimal places");
// }

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
    TEST_ASSERT_EQ(get_bit_range_uint8(0b11110000, 7, 4), (uint8_t)(0b1111), "Extract bits 7 to 4 from 0b11110000");
}

static void test_get_bit_range_uint16(void) {
    TEST_ASSERT_EQ(get_bit_range_uint16(0b1111000011110000, 15, 8), (uint16_t)(0b11110000), "Extract bits 15 to 8 from 0b1111000011110000");
}

static void test_get_8bit_range_uint16(void) {
    TEST_ASSERT_EQ(get_8bit_range_uint16(0b1111000011110000, 7, 0), (uint8_t)(0b11110000), "Extract bits 7 to 0 from 0b1111000011110000");
}

static void test_get_bit_range_uint32(void) {
    TEST_ASSERT_EQ(get_bit_range_uint32(0b11110000111100001111000011110000, 7, 0), (uint32_t)(0b11110000), "Extract bits 7 to 0 from 0b11110000111100001111000011110000");
}

static void test_get_8bit_range_uint32(void) {
    TEST_ASSERT_EQ(get_8bit_range_uint32(0b11110000111100001111000011110000, 7, 0), (uint8_t)(0b11110000), "Extract bits 7 to 0 from 0b11110000111100001111000011110000");
}

static void test_get_16bit_range_uint32(void) {
    TEST_ASSERT_EQ(get_16bit_range_uint32(0b11110000111100001111000011110000, 15, 0), (uint16_t)(0b1111000011110000), "Extract bits 15 to 0 from 0b11110000111100001111000011110000");
}

static void test_get_bit_range_uint64(void) {
    TEST_ASSERT_EQ(get_bit_range_uint64(0xFFF00000FFF00000ULL, 63, 32), (uint64_t)(0xFFF00000), "Extract bits 63 to 32 from 0xFFF00000FFF00000");
}

static void test_get_8bit_range_uint64(void) {
    TEST_ASSERT_EQ(get_8bit_range_uint64(0xFFF00000FFF00000ULL, 31, 24), (uint8_t)(0xFF), "Extract bits 31 to 24 from 0xFFF00000FFF00000");
}

static void test_get_16bit_range_uint64(void) {
    TEST_ASSERT_EQ(get_16bit_range_uint64(0xFFF00000FFF00000ULL, 39, 24), (uint16_t)(0x00FF), "Extract bits 39 to 24 from 0xFFF00000FFF00000");
}

static void test_get_32bit_range_uint64(void) {
    TEST_ASSERT_EQ(get_32bit_range_uint64(0xFFF00000FFF00000ULL, 63, 32), (uint32_t)(0xFFF00000), "Extract bits 63 to 32 from 0xFFF00000FFF00000");
}

static void test_get_bit_range_int8(void) {
    TEST_ASSERT_EQ(get_bit_range_int8((int8_t)0b11110000, 7, 4), (int8_t)(0b1111), "Extract bits 7 to 4 from int8_t 0b11110000");
}

static void test_get_bit_range_int16(void) {
    TEST_ASSERT_EQ(get_bit_range_int16((int16_t)0b1111000011110000, 15, 8), (int16_t)(0b11110000), "Extract bits 15 to 8 from int16_t 0b1111000011110000");
}

static void test_get_8bit_range_int16(void) {
    TEST_ASSERT_EQ(get_8bit_range_int16((int16_t)0b1111000011110000, 7, 0), (int8_t)(0b11110000), "Extract bits 7 to 0 from int16_t 0b1111000011110000");
}

static void test_get_bit_range_int32(void) {
    TEST_ASSERT_EQ(get_bit_range_int32((int32_t)0b11110000111100001111000011110000, 7, 0), (int32_t)(0b11110000), "Extract bits 7 to 0 from int32_t 0b11110000111100001111000011110000");
}

static void test_get_8bit_range_int32(void) {
    TEST_ASSERT_EQ(get_8bit_range_int32((int32_t)0b11110000111100001111000011110000, 15, 8), (int8_t)(0b11110000), "Extract bits 15 to 8 from int32_t 0b11110000111100001111000011110000");
}

static void test_get_16bit_range_int32(void) {
    TEST_ASSERT_EQ(get_16bit_range_int32((int32_t)0b11110000111100001111000011110000, 31, 16), (int16_t)(0b1111000011110000), "Extract bits 31 to 16 from int32_t 0b11110000111100001111000011110000");
}

static void test_get_bit_range_int64(void) {
    TEST_ASSERT_EQ(get_bit_range_int64((int64_t)0xFFF00000FFF00000ULL, 63, 32), (int64_t)(0xFFF00000), "Extract bits 63 to 32 from int64_t 0xFFF00000FFF00000");
    // TEST_ASSERT_EQ(get_bit_range_int64((int64_t)0xFFF00000FFF00000ULL, 63, 0), (int64_t)(0xFFF00000FFF00000), "Extract bits 63 to 0 from int64_t 0xFFF00000FFF00000"); // Check full range extraction
}

static void test_get_8bit_range_int64(void) {
    TEST_ASSERT_EQ(get_8bit_range_int64((int64_t)0xFFF00000FFF00000ULL, 63, 56), (int8_t)(0xFF), "Extract bits 63 to 56 from int64_t 0xFFF00000FFF00000");
}

static void test_get_16bit_range_int64(void) {
    TEST_ASSERT_EQ(get_16bit_range_int64((int64_t)0xFFF00000FFF00000ULL, 63, 48), (int16_t)(0xFFF0), "Extract bits 63 to 48 from int64_t 0xFFF00000FFF00000");
}

static void test_get_32bit_range_int64(void) {
    TEST_ASSERT_EQ(get_32bit_range_int64((int64_t)0xFFF00000FFF00000ULL, 63, 32), (int32_t)(0xFFF00000), "Extract bits 63 to 32 from int64_t 0xFFF00000FFF00000");
}

static void test_M_GETBITRANGE(void) {
    TEST_ASSERT_EQ(M_GETBITRANGE(0b11110000, 7, 4), (uint8_t)(0b1111), "Extract bits 7 to 4 from 0b11110000 using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_GETBITRANGE(0b1111000011110000, 15, 8), (uint8_t)(0b11110000), "Extract bits 15 to 8 from 0b1111000011110000 using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_GETBITRANGE(0b1111000011110000, 15, 0), (uint16_t)(0b1111000011110000), "Extract bits 15 to 0 from 0b11110000111100001111000011110000 using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_GETBITRANGE(0b11110000111100001111000011110000, 31, 0), (uint32_t)(0b11110000111100001111000011110000), "Extract bits 31 to 0 from 0b1111000011110000111100001111000 using M_GETBITRANGE");
    // TEST_ASSERT_EQ(M_GETBITRANGE(0xFFF00000FFF00000, 63, 0), (uint64_t)(0xFFF00000FFF00000), "Extract bits 63 to 0 from 0xFFF00000FFF00000 using M_GETBITRANGE");
}

static void test_M_IGETBITRANGE(void) {
    TEST_ASSERT_EQ(M_IGETBITRANGE(0b11110000, 7, 4), (int8_t)(0b1111), "Extract bits 7 to 4 from 0b11110000 using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_IGETBITRANGE(0b1111000011110000, 15, 8), (int8_t)(0b11110000), "Extract bits 15 to 8 from 0b1111000011110000 using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_IGETBITRANGE(0b1111000011110000, 15, 0), (int16_t)(0b1111000011110000), "Extract bits 15 to 0 from 0b11110000111100001111000011110000 using M_GETBITRANGE");
    TEST_ASSERT_EQ(M_IGETBITRANGE(0b11110000111100001111000011110000, 31, 0), (int32_t)(0b11110000111100001111000011110000), "Extract bits 31 to 0 from 0b1111000011110000111100001111000 using M_GETBITRANGE");
    // TEST_ASSERT_EQ(M_IGETBITRANGE(0xFFF00000FFF00000, 63, 0), (int64_t)(0xFFF00000FFF00000), "Extract bits 63 to 0 from 0xFFF00000FFF00000 using M_GETBITRANGE");
}

static void test_M_BitN(void) {
    TEST_ASSERT_EQ(M_BitN(0), (uint64_t)1 << 0, "Set 0th bit of uint64_t");
    TEST_ASSERT_EQ(M_BitN(4), (uint64_t)1 << 4, "Set 4th bit of uint64_t");
    TEST_ASSERT_EQ(M_BitN(63), (uint64_t)1 << 63, "Set 63rd bit of uint64_t");
}

static void test_M_BitN8(void) {
    TEST_ASSERT_EQ(M_BitN8(0), (uint8_t)1 << 0, "Set 0th bit of uint8_t");
    TEST_ASSERT_EQ(M_BitN8(4), (uint8_t)1 << 4, "Set 4th bit of uint8_t");
    TEST_ASSERT_EQ(M_BitN8(63), (uint8_t)1 << 63, "Set 63rd bit of uint8_t");
}

static void test_M_BitN16(void) {
    TEST_ASSERT_EQ(M_BitN16(0), (uint16_t)1 << 0, "Set 0th bit of uint16_t");
    TEST_ASSERT_EQ(M_BitN16(4), (uint16_t)1 << 4, "Set 4th bit of uint16_t");
    TEST_ASSERT_EQ(M_BitN16(63), (uint16_t)1 << 63, "Set 63rd bit of uint16_t");
}

static void test_M_BitN32(void) {
    TEST_ASSERT_EQ(M_BitN32(0), (uint32_t)1 << 0, "Set 0th bit of uint32_t");
    TEST_ASSERT_EQ(M_BitN32(4), (uint32_t)1 << 4, "Set 4th bit of uint32_t");
    TEST_ASSERT_EQ(M_BitN16(63), (uint32_t)1 << 63, "Set 63rd bit of uint32_t");
}

static void test_M_BitN64(void) {
    TEST_ASSERT_EQ(M_BitN64(0), (uint64_t)1 << 0, "Set 0th bit of uint64_t");
    TEST_ASSERT_EQ(M_BitN64(4), (uint64_t)1 << 4, "Set 4th bit of uint64_t");
    TEST_ASSERT_EQ(M_BitN64(63), (uint64_t)1 << 63, "Set 63rd bit of uint64_t");
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
    uint64_t value = 0x00;
    TEST_ASSERT_EQ(M_SET_BIT(value, 63), 0x80, "Set 3rd bit of 0x00 using M_SET_BIT");
}

static void test_M_CLEAR_BIT(void) {
    uint64_t value = 0xFF;
    TEST_ASSERT_EQ(M_CLEAR_BIT(value, 63), 0x7F, "Clear 3rd bit of 0xFF using M_CLEAR_BIT");
}

void run_bit_manip_tests(void)
{
    printf("%.20f\n", ROUNDF(2.999f,100));
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
    // test_ROUNDF();
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
}


