#include "../framework/test_framework.h"
#include "../../include/bit_manip.h"
#include "../testConstants.h"

static void test_get_DWord0(void) {
    TEST_ASSERT_EQ(get_DWord0_uint64(SERIAL_NUM), LOWER8BYTE);
}

static void test_get_DWord1(void) {
    TEST_ASSERT_EQ(get_DWord1(SERIAL_NUM), UPPER8BYTE);
}

static void test_M_DoubleWordInt0(void) {
    TEST_ASSERT_EQ(M_DoubleWordInt0(SERIAL_NUM), (int32_t)(LOWER8BYTE));
}

static void test_M_DoubleWordInt1(void) {
    TEST_ASSERT_EQ(M_DoubleWordInt1(SERIAL_NUM), (int32_t)(UPPER8BYTE));
}

static void test_get_Word0_uint64(void) {
    TEST_ASSERT_EQ(get_Word0_uint64(SERIAL_NUM), LOWER4BYTE);
}

static void test_get_Word0_uint32(void) {
    TEST_ASSERT_EQ(get_Word0_uint32(LOWER8BYTE), LOWER4BYTE);
}

static void 

void run_bit_manip_tests(void)
{
   test_get_DWord0();
   test_get_DWord1();
   test_M_DoubleWordInt0();
   test_M_DoubleWordInt1(); 
   test_get_Word0_uint64();
   test_get_Word0_uint32();
}


