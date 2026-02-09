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
    TEST_ASSERT_EQ(get_Word1_uint32(LOWER4BYTE), (uint16_t)(0x0000) , "get higher 16 bits of uint32_t");
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

void run_bit_manip_tests(void)
{
    test_get_DWord0();
    test_get_DWord1();
    test_M_DoubleWordInt0();
    test_M_DoubleWordInt1(); 
    test_get_Word0_uint64();
    test_get_Word0_uint32();
    test_get_Word1_uint64();
    test_get_Word1_uint32();
    test_get_Word2_uint64();
    test_get_Word3_uint64();
    test_M_Word0();
    test_M_Word1();
    test_M_Word2();
    test_M_Word3();
}


