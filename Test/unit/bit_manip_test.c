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

static void test_M_ByteInt0(void) {
    TEST_ASSERT_EQ(M_ByteInt0(0), (int8_t)(UINT8_MAX << (0 * BITSPERBYTE)), "Set 0th byte to all 1s as int8_t");
}

static void test_M_ByteInt1(void) {
    TEST_ASSERT_EQ(M_ByteInt1(1), (int8_t)(UINT8_MAX << (1 * BITSPERBYTE)), "Set 1st byte to all 1s as int8_t");
}

static void test_M_ByteInt2(void) {
    TEST_ASSERT_EQ(M_ByteInt2(2), (int8_t)(UINT8_MAX << (2 * BITSPERBYTE)), "Set 2nd byte to all 1s as int8_t");
}

static void test_M_ByteInt3(void) {
    TEST_ASSERT_EQ(M_ByteInt3(3), (int8_t)(UINT8_MAX << (3 * BITSPERBYTE)), "Set 3rd byte to all 1s as int8_t");
}

static void test_M_ByteInt4(void) {
    TEST_ASSERT_EQ(M_ByteInt4(4), (int8_t)(UINT8_MAX << (4 * BITSPERBYTE)), "Set 4th byte to all 1s as int8_t");
}

static void test_M_ByteInt5(void) {
    TEST_ASSERT_EQ(M_ByteInt5(5), (int8_t)(UINT8_MAX << (5 * BITSPERBYTE)), "Set 5th byte to all 1s as int8_t");
}

static void test_M_ByteInt6(void) {
    TEST_ASSERT_EQ(M_ByteInt6(6), (int8_t)(UINT8_MAX << (6 * BITSPERBYTE)), "Set 6th byte to all 1s as int8_t");
}

static void test_M_ByteInt7(void) {
    TEST_ASSERT_EQ(M_ByteInt7(7), (int8_t)(UINT8_MAX << (7 * BITSPERBYTE)), "Set 7th byte to all 1s as int8_t");
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
}


