#include"../framework/test_framework.h"
#include"../../include/math_utils.h"
#include <math.h>
#include"../testConstants.h"

static void test_M_Min(void) {
    TEST_ASSERT_EQ(M_Min(INT_MIN, INT_MAX), INT_MIN, "M_Min of INT_MIN and INT_MAX");
    TEST_ASSERT_EQ(M_Min(INT8_MAX_VAL, INT8_MIN_VAL), INT8_MIN_VAL, "M_Min of INT8_MAX_VAL and INT8_MIN_VAL");
    TEST_ASSERT_EQ(M_Min(INT16_MAX_VAL, INT16_MIN_VAL), INT16_MIN_VAL, "M_Min of INT16_MAX_VAL and INT16_MIN_VAL");
    TEST_ASSERT_EQ(M_Min(INT32_MAX_VAL, INT32_MIN_VAL), INT32_MIN_VAL, "M_Min of INT32_MAX_VAL and INT32_MIN_VAL");
    TEST_ASSERT_EQ(M_Min(INT64_MAX_VAL, INT64_MIN_VAL), INT64_MIN_VAL, "M_Min of INT64_MAX_VAL and INT64_MIN_VAL");
    TEST_ASSERT_EQ(M_Min(UINT_MAX_VAL, UINT_MIN_VAL), UINT_MIN_VAL, "M_Min of UINT_MAX_VAL and UINT_MIN_VAL");
    TEST_ASSERT_EQ(M_Min(UINT8_MAX_VAL, UINT8_MIN_VAL), UINT8_MIN_VAL, "M_Min of UINT8_MAX_VAL and UINT8_MIN_VAL");
    TEST_ASSERT_EQ(M_Min(UINT16_MAX_VAL, UINT16_MIN_VAL), UINT16_MIN_VAL, "M_Min of UINT16_MAX_VAL and UINT16_MIN_VAL");        
    TEST_ASSERT_EQ(M_Min(UINT32_MAX_VAL, UINT32_MIN_VAL), UINT32_MIN_VAL, "M_Min of UINT32_MAX_VAL and UINT32_MIN_VAL");
    TEST_ASSERT_EQ(M_Min(UINT64_MAX_VAL, UINT64_MIN_VAL), UINT64_MIN_VAL, "M_Min of UINT64_MAX_VAL and UINT64_MIN_VAL");
    TEST_ASSERT_EQ(M_Min(FLOAT32_MAX_VAL, FLOAT32_MIN_VAL), FLOAT32_MIN_VAL, "M_Min of FLOAT32_MAX_VAL and FLOAT32_MIN_VAL");
    TEST_ASSERT_EQ(M_Min(FLOAT64_MIN_VAL, FLOAT64_MAX_VAL), FLOAT64_MIN_VAL, "M_Min of FLOAT64_MIN_VAL and FLOAT64_MAX_VAL");
}

static void test_M_Max(void) {
    TEST_ASSERT_EQ(M_Max(INT_MIN, INT_MAX), INT_MAX, "M_Max of INT_MIN and INT_MAX");
    TEST_ASSERT_EQ(M_Max(INT8_MAX_VAL, INT8_MIN_VAL), INT8_MAX_VAL, "M_Max of INT8_MAX_VAL and INT8_MIN_VAL");
    TEST_ASSERT_EQ(M_Max(INT16_MAX_VAL, INT16_MIN_VAL), INT16_MAX_VAL, "M_Max of INT16_MAX_VAL and INT16_MIN_VAL");
    TEST_ASSERT_EQ(M_Max(INT32_MAX_VAL, INT32_MIN_VAL), INT32_MAX_VAL, "M_Max of INT32_MAX_VAL and INT32_MIN_VAL");
    TEST_ASSERT_EQ(M_Max(INT64_MAX_VAL, INT64_MIN_VAL), INT64_MAX_VAL, "M_Max of INT64_MAX_VAL and INT64_MIN_VAL");
    TEST_ASSERT_EQ(M_Max(UINT_MAX_VAL, UINT_MIN_VAL), UINT_MAX_VAL, "M_Max of UINT_MAX_VAL and UINT_MIN_VAL");
    TEST_ASSERT_EQ(M_Max(UINT8_MAX_VAL, UINT8_MIN_VAL), UINT8_MAX_VAL, "M_Max of UINT8_MAX_VAL and UINT8_MIN_VAL");
    TEST_ASSERT_EQ(M_Max(UINT16_MAX_VAL, UINT16_MIN_VAL), UINT16_MAX_VAL, "M_Max of UINT16_MAX_VAL and UINT16_MIN_VAL");        
    TEST_ASSERT_EQ(M_Max(UINT32_MAX_VAL, UINT32_MIN_VAL), UINT32_MAX_VAL, "M_Max of UINT32_MAX_VAL and UINT32_MIN_VAL");
    TEST_ASSERT_EQ(M_Max(UINT64_MAX_VAL, UINT64_MIN_VAL), UINT64_MAX_VAL, "M_Max of UINT64_MAX_VAL and UINT64_MIN_VAL");
    TEST_ASSERT_EQ(M_Max(FLOAT32_MAX_VAL, FLOAT32_MIN_VAL), FLOAT32_MAX_VAL, "M_Max of FLOAT32_MAX_VAL and FLOAT32_MIN_VAL");
    TEST_ASSERT_EQ(M_Max(FLOAT64_MIN_VAL, FLOAT64_MAX_VAL), FLOAT64_MAX_VAL, "M_Max of FLOAT64_MIN_VAL and FLOAT64_MAX_VAL");
}

static void test_uint8_round_up_generic(void) { 
    TEST_ASSERT_EQ(uint8_round_up_generic((uint8_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(uint8_round_up_generic((uint8_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(uint8_round_up_generic((uint8_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(uint8_round_up_generic((uint8_t)251, 10), 4, "Round 251 up to nearest multiple of 10: Overflow case");
    // TEST_ASSERT_EQ(uint8_round_up_generic((uint8_t)16, 0), 16, "Round 16 up to nearest multiple of 0: returns value"); 
}

static void test_int8_round_up_generic(void) {
    TEST_ASSERT_EQ(int8_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(int8_round_up_generic(5, 7), 7, "Round 5 up to nearest multiple of 7");
    TEST_ASSERT_EQ(int8_round_up_generic(-5, 3), -3, "Round -5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(int8_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(int8_round_up_generic(121, 10), -126, "Round 121 up to nearest multiple of 10: Overflow case");
}

static void test_signed_char_round_up_generic(void) {
    TEST_ASSERT_EQ(signed_char_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_char_round_up_generic(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_char_round_up_generic(-5, 3), -3, "Round -5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_char_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_char_round_up_generic(121, 10), -126, "Round 121 up to nearest multiple of 10: Overflow case");
}

static void test_signed_char_round_down_generic(void) {
    TEST_ASSERT_EQ(signed_char_round_down_generic(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_char_round_down_generic(5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_char_round_down_generic(-5, 3), -10, "Round -5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_char_round_down_generic(12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
}

static void test_unsigned_char_round_up_generic(void) {
    TEST_ASSERT_EQ(unsigned_char_round_up_generic((uint8_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(unsigned_char_round_up_generic((uint8_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(unsigned_char_round_up_generic((uint8_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(unsigned_char_round_up_generic((uint8_t)251, 10), 4, "Round 251 up to nearest multiple of 10: Overflow case");
}

static void test_unsigned_char_round_down_generic(void) {
    TEST_ASSERT_EQ(unsigned_char_round_down_generic((uint8_t)19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(unsigned_char_round_down_generic((uint8_t)5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(unsigned_char_round_down_generic((uint8_t)12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
}

static void test_signed_short_round_up_generic(void) {
    TEST_ASSERT_EQ(signed_short_round_up_generic(-5, 3), -3, "Round -5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_short_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_short_round_up_generic(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_short_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_short_round_up_generic(243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(signed_short_round_up_generic(1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_short_round_up_generic(32761, 10), -32766, "Round 32761 up to nearest multiple of 10: Overflow case");
}

static void test_signed_short_round_down_generic(void) {
    TEST_ASSERT_EQ(signed_short_round_down_generic(-5, 3), -10, "Round -5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_short_round_down_generic(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_short_round_down_generic(5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_short_round_down_generic(12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_short_round_down_generic(243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(signed_short_round_down_generic(1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_short_round_down_generic(32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
}

static void test_unsigned_short_round_up_generic(void) {
    TEST_ASSERT_EQ(unsigned_short_round_up_generic((uint16_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(unsigned_short_round_up_generic((uint16_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(unsigned_short_round_up_generic((uint16_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(unsigned_short_round_up_generic((uint16_t)243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(unsigned_short_round_up_generic((uint16_t)1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(unsigned_short_round_up_generic((uint16_t)65531, 10), 4, "Round 65531 up to nearest multiple of 10: Overflow case");
}

static void test_unsigned_short_round_down_generic(void) {
    TEST_ASSERT_EQ(unsigned_short_round_down_generic((uint16_t)19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(unsigned_short_round_down_generic((uint16_t)5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(unsigned_short_round_down_generic((uint16_t)12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(unsigned_short_round_down_generic((uint16_t)243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(unsigned_short_round_down_generic((uint16_t)1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(unsigned_short_round_down_generic((uint16_t)32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_short_round_down_generic((uint16_t)65535, 10), 65530, "Round 65535 down to nearest multiple of 10");
}

static void test_signed_int_round_up_generic(void) {
    TEST_ASSERT_EQ(signed_int_round_up_generic(-5, 3), -3, "Round -5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_int_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_int_round_up_generic(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_int_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_int_round_up_generic(243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(signed_int_round_up_generic(1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_int_round_up_generic(9496729, 12), 9496740, "Round 9496729 up to nearest multiple of 12");
}

static void test_signed_int_round_down_generic(void) {
    TEST_ASSERT_EQ(signed_int_round_down_generic(-5, 3), -10, "Round -5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_int_round_down_generic(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_int_round_down_generic(5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_int_round_down_generic(12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_int_round_down_generic(243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(signed_int_round_down_generic(1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_int_round_down_generic(32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(signed_int_round_down_generic(9496729, 10), 9496720, "Round 9496729 down to nearest multiple of 10");
}

static void test_unsigned_int_round_up_generic(void) {
    TEST_ASSERT_EQ(unsigned_int_round_up_generic((uint32_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(unsigned_int_round_up_generic((uint32_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(unsigned_int_round_up_generic((uint32_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(unsigned_int_round_up_generic((uint32_t)243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(unsigned_int_round_up_generic((uint32_t)1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(unsigned_int_round_up_generic((uint32_t)65531, 10), 65540, "Round 65531 up to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_int_round_up_generic((uint32_t)2147483637, 10), 2147483640, "Round 2147483637 up to nearest multiple of 10");
}

static void test_unsigned_int_round_down_generic(void) {
    TEST_ASSERT_EQ(unsigned_int_round_down_generic((uint32_t)19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(unsigned_int_round_down_generic((uint32_t)5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(unsigned_int_round_down_generic((uint32_t)12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(unsigned_int_round_down_generic((uint32_t)243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(unsigned_int_round_down_generic((uint32_t)1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(unsigned_int_round_down_generic((uint32_t)32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_int_round_down_generic((uint32_t)65535, 10), 65530, "Round 65535 down to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_int_round_down_generic((uint32_t)4294967295, 10), 4294967290, "Round 4294967295 down to nearest multiple of 10");
}

static void test_signed_long_round_up_generic(void) {
    TEST_ASSERT_EQ(signed_long_round_up_generic(-5, 3), -3, "Round -5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_long_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_long_round_up_generic(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_long_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_long_round_up_generic(243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(signed_long_round_up_generic(1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_long_round_up_generic(9496729, 12), 9496740, "Round 9496729 up to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_long_round_up_generic(23372036854775808, 7), 23372036854775812, "Round 23372036854775808 up to nearest multiple of 7");
}

static void test_signed_long_round_down_generic(void) {
    TEST_ASSERT_EQ(signed_long_round_down_generic(-5, 3), -10, "Round -5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_long_round_down_generic(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_long_round_down_generic(5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_long_round_down_generic(12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_long_round_down_generic(243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(signed_long_round_down_generic(1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_long_round_down_generic(32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(signed_long_round_down_generic(9496729, 10), 9496720, "Round 9496729 down to nearest multiple of 10");
    TEST_ASSERT_EQ(signed_long_round_down_generic(23372036854775808, 7), 23372036854775805, "Round 23372036854775808 down to nearest multiple of 7");
}

static void test_unsigned_long_round_up_generic(void) {
    TEST_ASSERT_EQ(unsigned_long_round_up_generic((uint64_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(unsigned_long_round_up_generic((uint64_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(unsigned_long_round_up_generic((uint64_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(unsigned_long_round_up_generic((uint64_t)243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(unsigned_long_round_up_generic((uint64_t)1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(unsigned_long_round_up_generic((uint64_t)65531, 10), 65540, "Round 65531 up to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_round_up_generic((uint64_t)2147483637, 10), 2147483640, "Round 2147483637 up to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_round_up_generic((uint64_t)23372036854775808, 7), 23372036854775812, "Round 23372036854775808 up to nearest multiple of 7");
}

static void test_unsigned_long_round_down_generic(void) {
    TEST_ASSERT_EQ(unsigned_long_round_down_generic((uint64_t)19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(unsigned_long_round_down_generic((uint64_t)5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(unsigned_long_round_down_generic((uint64_t)12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(unsigned_long_round_down_generic((uint64_t)243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(unsigned_long_round_down_generic((uint64_t)1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(unsigned_long_round_down_generic((uint64_t)32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_round_down_generic((uint64_t)65535, 10), 65530, "Round 65535 down to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_round_down_generic((uint64_t)4294967295, 10), 4294967290, "Round 4294967295 down to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_round_down_generic((uint64_t)9223372036854775808, 10), 9223372036854775800, "Round 9223372036854775808 down to nearest multiple of 10");
}

static void test_signed_long_long_round_up_generic(void) {
    TEST_ASSERT_EQ(signed_long_long_round_up_generic(-5, 3), -3, "Round -5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_long_long_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_long_long_round_up_generic(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_long_long_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_long_long_round_up_generic(243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(signed_long_long_round_up_generic(1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_long_long_round_up_generic(9496729, 12), 9496740, "Round 9496729 up to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_long_long_round_up_generic(23372036854775808, 7), 23372036854775812, "Round 23372036854775808 up to nearest multiple of 7");
    TEST_ASSERT_EQ(signed_long_long_round_up_generic(223372036854775807, 6), 223372036854775812, "Round 223372036854775807 up to nearest multiple of 6");
}

static void test_signed_long_long_round_down_generic(void) {
    TEST_ASSERT_EQ(signed_long_long_round_down_generic(-5, 3), -10, "Round -5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_long_long_round_down_generic(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_long_long_round_down_generic(5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_long_long_round_down_generic(12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_long_long_round_down_generic(243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(signed_long_long_round_down_generic(1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_long_long_round_down_generic(32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(signed_long_long_round_down_generic(9496729, 10), 9496720, "Round 9496729 down to nearest multiple of 10");
    TEST_ASSERT_EQ(signed_long_long_round_down_generic(23372036854775808, 7), 23372036854775805, "Round 23372036854775808 down to nearest multiple of 7");
    TEST_ASSERT_EQ(signed_long_long_round_down_generic(223372036854775807, 6), 223372036854775806, "Round 223372036854775807 down to nearest multiple of 6");
}

static void test_unsigned_long_long_round_up_generic(void) {
    TEST_ASSERT_EQ(unsigned_long_long_round_up_generic((uint64_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_generic((uint64_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_generic((uint64_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_generic((uint64_t)243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_generic((uint64_t)1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_generic((uint64_t)65531, 10), 65540, "Round 65531 up to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_generic((uint64_t)2147483637, 10), 2147483640, "Round 2147483637 up to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_generic((uint64_t)23372036854775808, 7), 23372036854775812, "Round 23372036854775808 up to nearest multiple of 7");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_generic((uint64_t)1844674407370955134, 9), 1844674407370955142, "Round 1844674407370955134 up to nearest multiple of 9");
}

static void test_unsigned_long_long_round_down_generic(void) {
    TEST_ASSERT_EQ(unsigned_long_long_round_down_generic((uint64_t)19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_generic((uint64_t)5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_generic((uint64_t)12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_generic((uint64_t)243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_generic((uint64_t)1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_generic((uint64_t)32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_generic((uint64_t)65535, 10), 65530, "Round 65535 down to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_generic((uint64_t)4294967295, 10), 4294967290, "Round 4294967295 down to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_generic((uint64_t)9223372036854775808, 10), 9223372036854775800, "Round 9223372036854775808 down to nearest multiple of 10");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_generic((uint64_t)1844674407370955134, 9), 1844674407370955133, "Round 1844674407370955134 down to nearest multiple of 9");
}

static void test_int16_round_up_generic(void) {
    TEST_ASSERT_EQ(int16_round_up_generic(-5, 3), -3, "Round -5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(int16_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(int16_round_up_generic(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(int16_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(int16_round_up_generic(243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(int16_round_up_generic(1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(int16_round_up_generic(32761, 10), -32766, "Round 32761 up to nearest multiple of 10: Overflow case");
}

static void test_uint16_round_up_generic(void) {
    TEST_ASSERT_EQ(uint16_round_up_generic((uint16_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(uint16_round_up_generic((uint16_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(uint16_round_up_generic((uint16_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(uint16_round_up_generic((uint16_t)243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(uint16_round_up_generic((uint16_t)1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(uint16_round_up_generic((uint16_t)65531, 10), 4, "Round 65531 up to nearest multiple of 10: Overflow case");
}

static void test_int32_round_up_generic(void) {
    TEST_ASSERT_EQ(int32_round_up_generic(-5, 3), -3, "Round -5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(int32_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(int32_round_up_generic(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(int32_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(int32_round_up_generic(243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(int32_round_up_generic(1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(int32_round_up_generic(9496729, 12), 9496740, "Round 9496729 up to nearest multiple of 12");
}

static void test_uint32_round_up_generic(void) {
    TEST_ASSERT_EQ(uint32_round_up_generic((uint32_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(uint32_round_up_generic((uint32_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(uint32_round_up_generic((uint32_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(uint32_round_up_generic((uint32_t)243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(uint32_round_up_generic((uint32_t)1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(uint32_round_up_generic((uint32_t)65531, 10), 65540, "Round 65531 up to nearest multiple of 10");
    TEST_ASSERT_EQ(uint32_round_up_generic((uint32_t)2147483637, 10), 2147483640, "Round 2147483637 up to nearest multiple of 10");
}

static void test_int64_round_up_generic(void) {
    TEST_ASSERT_EQ(int64_round_up_generic(-5, 3), -3, "Round -5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(int64_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(int64_round_up_generic(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(int64_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(int64_round_up_generic(243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(int64_round_up_generic(1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(int64_round_up_generic(9496729, 12), 9496740, "Round 9496729 up to nearest multiple of 12");
    TEST_ASSERT_EQ(int64_round_up_generic(23372036854775808, 7), 23372036854775812, "Round 23372036854775808 up to nearest multiple of 7");
}

static void test_uint64_round_up_generic(void) {
    TEST_ASSERT_EQ(uint64_round_up_generic((uint64_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(uint64_round_up_generic((uint64_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(uint64_round_up_generic((uint64_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(uint64_round_up_generic((uint64_t)243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(uint64_round_up_generic((uint64_t)1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(uint64_round_up_generic((uint64_t)65531, 10), 65540, "Round 65531 up to nearest multiple of 10");
    TEST_ASSERT_EQ(uint64_round_up_generic((uint64_t)2147483637, 10), 2147483640, "Round 2147483637 up to nearest multiple of 10");
    TEST_ASSERT_EQ(uint64_round_up_generic((uint64_t)23372036854775808, 7), 23372036854775812, "Round 23372036854775808 up to nearest multiple of 7");
}

static void test_int8_round_down_generic(void) {
    TEST_ASSERT_EQ(int8_round_down_generic(-5, 3), -6, "Round -5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(int8_round_down_generic(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(int8_round_down_generic(5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(int8_round_down_generic(12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
}

static void test_uint8_round_down_generic(void) {
    TEST_ASSERT_EQ(uint8_round_down_generic((uint8_t)19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(uint8_round_down_generic((uint8_t)5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(uint8_round_down_generic((uint8_t)12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
}

static void test_int16_round_down_generic(void) {
    TEST_ASSERT_EQ(int16_round_down_generic(-5, 3), -6, "Round -5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(int16_round_down_generic(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(int16_round_down_generic(5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(int16_round_down_generic(12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(int16_round_down_generic(243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(int16_round_down_generic(1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(int16_round_down_generic(32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
}

static void test_uint16_round_down_generic(void) {
    TEST_ASSERT_EQ(uint16_round_down_generic((uint16_t)19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(uint16_round_down_generic((uint16_t)5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(uint16_round_down_generic((uint16_t)12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(uint16_round_down_generic((uint16_t)243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(uint16_round_down_generic((uint16_t)1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(uint16_round_down_generic((uint16_t)32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(uint16_round_down_generic((uint16_t)65535, 10), 65530, "Round 65535 down to nearest multiple of 10");
}

static void test_int32_round_down_generic(void) {
    TEST_ASSERT_EQ(int32_round_down_generic(-5, 3), -6, "Round -5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(int32_round_down_generic(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(int32_round_down_generic(5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(int32_round_down_generic(12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(int32_round_down_generic(243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(int32_round_down_generic(1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(int32_round_down_generic(32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(int32_round_down_generic(9496729, 10), 9496720, "Round 9496729 down to nearest multiple of 10");
}

static void test_uint32_round_down_generic(void) {
    TEST_ASSERT_EQ(uint32_round_down_generic((uint32_t)19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(uint32_round_down_generic((uint32_t)5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(uint32_round_down_generic((uint32_t)12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(uint32_round_down_generic((uint32_t)243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(uint32_round_down_generic((uint32_t)1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(uint32_round_down_generic((uint32_t)32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(uint32_round_down_generic((uint32_t)65535, 10), 65530, "Round 65535 down to nearest multiple of 10");
    TEST_ASSERT_EQ(uint32_round_down_generic((uint32_t)4294967295, 10), 4294967290, "Round 4294967295 down to nearest multiple of 10");
}

static void test_int64_round_down_generic(void) {
    TEST_ASSERT_EQ(int64_round_down_generic(-5, 3), -6, "Round -5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(int64_round_down_generic(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(int64_round_down_generic(5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(int64_round_down_generic(12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(int64_round_down_generic(243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(int64_round_down_generic(1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(int64_round_down_generic(32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(int64_round_down_generic(9496729, 10), 9496720, "Round 9496729 down to nearest multiple of 10");
    TEST_ASSERT_EQ(int64_round_down_generic(23372036854775808, 7), 23372036854775805, "Round 23372036854775808 down to nearest multiple of 7");
}

static void test_uint64_round_down_generic(void) {
    TEST_ASSERT_EQ(uint64_round_down_generic((uint64_t)19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(uint64_round_down_generic((uint64_t)5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(uint64_round_down_generic((uint64_t)12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(uint64_round_down_generic((uint64_t)243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(uint64_round_down_generic((uint64_t)1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(uint64_round_down_generic((uint64_t)32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(uint64_round_down_generic((uint64_t)65535, 10), 65530, "Round 65535 down to nearest multiple of 10");
    TEST_ASSERT_EQ(uint64_round_down_generic((uint64_t)4294967295, 10), 4294967290, "Round 4294967295 down to nearest multiple of 10");
    TEST_ASSERT_EQ(uint64_round_down_generic((uint64_t)9223372036854775808, 10), 9223372036854775800, "Round 9223372036854775808 down to nearest multiple of 10");
}

static void test_uint8_round_up_power2(void) {
    TEST_ASSERT_EQ(uint8_round_up_power2((uint8_t)8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(uint8_round_up_power2((uint8_t)5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(uint8_round_up_power2((uint8_t)12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(uint8_round_up_power2((uint8_t)251, 2), 252, "Round 251 up to nearest multiple of 2");
}

static void test_int8_round_up_power2(void) {
    TEST_ASSERT_EQ(int8_round_up_power2(8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(int8_round_up_power2(5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(int8_round_up_power2(12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(int8_round_up_power2(121, 2), 122, "Round 121 up to nearest multiple of 2");
}

static void test_int16_round_up_power2(void) {
    TEST_ASSERT_EQ(int16_round_up_power2(8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(int16_round_up_power2(5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(int16_round_up_power2(12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(int16_round_up_power2(243, 2), 244, "Round 243 up to nearest multiple of 2");
    TEST_ASSERT_EQ(int16_round_up_power2(1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(int16_round_up_power2(32761, 4), 32764, "Round 32761 up to nearest multiple of 4");
}

static void test_uint16_round_up_power2(void) {
    TEST_ASSERT_EQ(uint16_round_up_power2((uint16_t)8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(uint16_round_up_power2((uint16_t)5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(uint16_round_up_power2((uint16_t)12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(uint16_round_up_power2((uint16_t)243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(uint16_round_up_power2((uint16_t)1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(uint16_round_up_power2((uint16_t)65531, 2), 65532, "Round 65531 up to nearest multiple of 2");
}

static void test_int32_round_up_power2(void) {
    TEST_ASSERT_EQ(int32_round_up_power2(8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(int32_round_up_power2(5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(int32_round_up_power2(12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(int32_round_up_power2(243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(int32_round_up_power2(1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(int32_round_up_power2(9496729, 2), 9496730, "Round 9496729 up to nearest multiple of 2");
}

static void test_uint32_round_up_power2(void) {
    TEST_ASSERT_EQ(uint32_round_up_power2((uint32_t)8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(uint32_round_up_power2((uint32_t)5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(uint32_round_up_power2((uint32_t)12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(uint32_round_up_power2((uint32_t)243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(uint32_round_up_power2((uint32_t)1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(uint32_round_up_power2((uint32_t)65531, 2), 65532, "Round 65531 up to nearest multiple of 2");
    TEST_ASSERT_EQ(uint32_round_up_power2((uint32_t)2147483637, 2), 2147483638, "Round 2147483637 up to nearest multiple of 2");
}

static void test_int64_round_up_power2(void) {
    TEST_ASSERT_EQ(int64_round_up_power2(8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(int64_round_up_power2(5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(int64_round_up_power2(12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(int64_round_up_power2(243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(int64_round_up_power2(1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(int64_round_up_power2(9496729, 2), 9496730, "Round 9496729 up to nearest multiple of 2");
    TEST_ASSERT_EQ(int64_round_up_power2(23372036854775808, 2), 23372036854775808, "Round 23372036854775808 up to nearest multiple of 2");
}

static void test_uint64_round_up_power2(void) {
    TEST_ASSERT_EQ(uint64_round_up_power2((uint64_t)8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(uint64_round_up_power2((uint64_t)5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(uint64_round_up_power2((uint64_t)12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(uint64_round_up_power2((uint64_t)243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(uint64_round_up_power2((uint64_t)1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(uint64_round_up_power2((uint64_t)65531, 2), 65532, "Round 65531 up to nearest multiple of 2");
    TEST_ASSERT_EQ(uint64_round_up_power2((uint64_t)2147483637, 4), 2147483640, "Round 2147483637 up to nearest multiple of 4");
    TEST_ASSERT_EQ(uint64_round_up_power2((uint64_t)23372036854775808, 2), 23372036854775808, "Round 23372036854775808 up to nearest multiple of 2");
}

static void test_int8_round_down_power2(void) {
    TEST_ASSERT_EQ(int8_round_down_power2(19, 2), 18, "Round 19 down to nearest multiple of 2");
    TEST_ASSERT_EQ(int8_round_down_power2(5, 4), 4, "Round 5 down to nearest multiple of 4");
    TEST_ASSERT_EQ(int8_round_down_power2(12, 4), 12, "Round 12 down to nearest multiple of 4");
}

static void test_uint8_round_down_power2(void) {
    TEST_ASSERT_EQ(uint8_round_down_power2((uint8_t)19, 2), 18, "Round 19 down to nearest multiple of 2");
    TEST_ASSERT_EQ(uint8_round_down_power2((uint8_t)5, 4), 4, "Round 5 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint8_round_down_power2((uint8_t)12, 4), 12, "Round 12 down to nearest multiple of 4");
}

static void test_int16_round_down_power2(void) {
    TEST_ASSERT_EQ(int16_round_down_power2(19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(int16_round_down_power2(5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(int16_round_down_power2(12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(int16_round_down_power2(243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(int16_round_down_power2(1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(int16_round_down_power2(32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
}

static void test_uint16_round_down_power2(void) {
    TEST_ASSERT_EQ(uint16_round_down_power2((uint16_t)19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint16_round_down_power2((uint16_t)5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(uint16_round_down_power2((uint16_t)12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint16_round_down_power2((uint16_t)243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(uint16_round_down_power2((uint16_t)1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(uint16_round_down_power2((uint16_t)32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint16_round_down_power2((uint16_t)65535, 2), 65534, "Round 65535 down to nearest multiple of 2");
}

static void test_int32_round_down_power2(void) {
    TEST_ASSERT_EQ(int32_round_down_power2(19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(int32_round_down_power2(5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(int32_round_down_power2(12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(int32_round_down_power2(243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(int32_round_down_power2(1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(int32_round_down_power2(32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(int32_round_down_power2(9496729, 2), 9496728, "Round 9496729 down to nearest multiple of 2");
}

static void test_uint32_round_down_power2(void) {
    TEST_ASSERT_EQ(uint32_round_down_power2((uint32_t)19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint32_round_down_power2((uint32_t)5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(uint32_round_down_power2((uint32_t)12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint32_round_down_power2((uint32_t)243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(uint32_round_down_power2((uint32_t)1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(uint32_round_down_power2((uint32_t)32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint32_round_down_power2((uint32_t)65535, 2), 65534, "Round 65535 down to nearest multiple of 2");
    TEST_ASSERT_EQ(uint32_round_down_power2((uint32_t)4294967295, 4), 4294967292, "Round 4294967295 down to nearest multiple of 4");
}

static void test_int64_round_down_power2(void) {
    TEST_ASSERT_EQ(int64_round_down_power2(19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(int64_round_down_power2(5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(int64_round_down_power2(12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(int64_round_down_power2(243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(int64_round_down_power2(1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(int64_round_down_power2(32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(int64_round_down_power2(9496729, 2), 9496728, "Round 9496729 down to nearest multiple of 2");
    TEST_ASSERT_EQ(int64_round_down_power2(23372036854775808, 4), 23372036854775808, "Round 23372036854775808 down to nearest multiple of 4");
}

static void test_uint64_round_down_power2(void) {
    TEST_ASSERT_EQ(uint64_round_down_power2((uint64_t)19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint64_round_down_power2((uint64_t)5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(uint64_round_down_power2((uint64_t)12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint64_round_down_power2((uint64_t)243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(uint64_round_down_power2((uint64_t)1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(uint64_round_down_power2((uint64_t)32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint64_round_down_power2((uint64_t)65535, 2), 65534, "Round 65535 down to nearest multiple of 2");
    TEST_ASSERT_EQ(uint64_round_down_power2((uint64_t)4294967295, 4), 4294967292, "Round 4294967295 down to nearest multiple of 4");
    TEST_ASSERT_EQ(uint64_round_down_power2((uint64_t)9223372036854775808, 2), 9223372036854775808, "Round 9223372036854775808 down to nearest multiple of 2");
}

static void test_signed_char_round_up_power2(void) {
    TEST_ASSERT_EQ(signed_char_round_up_power2(8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_char_round_up_power2(5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_char_round_up_power2(12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_char_round_up_power2(121, 2), 122, "Round 121 up to nearest multiple of 2");
}

static void test_signed_char_round_down_power2(void) {
    TEST_ASSERT_EQ(signed_char_round_down_power2(19, 2), 18, "Round 19 down to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_char_round_down_power2(5, 4), 4, "Round 5 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_char_round_down_power2(12, 4), 12, "Round 12 down to nearest multiple of 4");
}

static void test_unsigned_char_round_up_power2(void) {
    TEST_ASSERT_EQ(unsigned_char_round_up_power2((uint8_t)8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_char_round_up_power2((uint8_t)5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_char_round_up_power2((uint8_t)12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_char_round_up_power2((uint8_t)251, 2), 252, "Round 251 up to nearest multiple of 2");
}

static void test_unsigned_char_round_down_power2(void) {
    TEST_ASSERT_EQ(unsigned_char_round_down_power2((uint8_t)19, 2), 18, "Round 19 down to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_char_round_down_power2((uint8_t)5, 4), 4, "Round 5 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_char_round_down_power2((uint8_t)12, 4), 12, "Round 12 down to nearest multiple of 4");
}

static void test_signed_short_round_up_power2(void) {
    TEST_ASSERT_EQ(signed_short_round_up_power2(8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_short_round_up_power2(5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_short_round_up_power2(12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_short_round_up_power2(243, 2), 244, "Round 243 up to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_short_round_up_power2(1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_short_round_up_power2(32761, 4), 32764, "Round 32761 up to nearest multiple of 4");
}

static void test_signed_short_round_down_power2(void) {
    TEST_ASSERT_EQ(signed_short_round_down_power2(19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_short_round_down_power2(5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_short_round_down_power2(12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_short_round_down_power2(243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_short_round_down_power2(1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_short_round_down_power2(32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
}

static void test_unsigned_short_round_up_power2(void) {
    TEST_ASSERT_EQ(unsigned_short_round_up_power2((uint16_t)8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_short_round_up_power2((uint16_t)5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_short_round_up_power2((uint16_t)12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_short_round_up_power2((uint16_t)243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_short_round_up_power2((uint16_t)1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_short_round_up_power2((uint16_t)65531, 2), 65532, "Round 65531 up to nearest multiple of 2");
}

static void test_unsigned_short_round_down_power2(void) {
    TEST_ASSERT_EQ(unsigned_short_round_down_power2((uint16_t)19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_short_round_down_power2((uint16_t)5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_short_round_down_power2((uint16_t)12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_short_round_down_power2((uint16_t)243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_short_round_down_power2((uint16_t)1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_short_round_down_power2((uint16_t)32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_short_round_down_power2((uint16_t)65535, 2), 65534, "Round 65535 down to nearest multiple of 2");
}

static void test_signed_int_round_up_power2(void) {
    TEST_ASSERT_EQ(signed_int_round_up_power2(8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_int_round_up_power2(5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_int_round_up_power2(12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_int_round_up_power2(243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_int_round_up_power2(1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_int_round_up_power2(9496729, 2), 9496730, "Round 9496729 up to nearest multiple of 2");
}

static void test_signed_int_round_down_power2(void) {
    TEST_ASSERT_EQ(signed_int_round_down_power2(19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_int_round_down_power2(5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_int_round_down_power2(12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_int_round_down_power2(243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_int_round_down_power2(1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_int_round_down_power2(32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_int_round_down_power2(9496729, 2), 9496728, "Round 9496729 down to nearest multiple of 2");
}

static void test_unsigned_int_round_up_power2(void) {
    TEST_ASSERT_EQ(unsigned_int_round_up_power2((uint32_t)8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_int_round_up_power2((uint32_t)5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_int_round_up_power2((uint32_t)12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_int_round_up_power2((uint32_t)243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_int_round_up_power2((uint32_t)1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_int_round_up_power2((uint32_t)65531, 2), 65532, "Round 65531 up to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_int_round_up_power2((uint32_t)2147483637, 2), 2147483638, "Round 2147483637 up to nearest multiple of 2");
}

static void test_unsigned_int_round_down_power2(void) {
    TEST_ASSERT_EQ(unsigned_int_round_down_power2((uint32_t)19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_int_round_down_power2((uint32_t)5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_int_round_down_power2((uint32_t)12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_int_round_down_power2((uint32_t)243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_int_round_down_power2((uint32_t)1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_int_round_down_power2((uint32_t)32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_int_round_down_power2((uint32_t)65535, 2), 65534, "Round 65535 down to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_int_round_down_power2((uint32_t)4294967295, 4), 4294967292, "Round 4294967295 down to nearest multiple of 4");
}

static void test_signed_long_round_up_power2(void) {
    TEST_ASSERT_EQ(signed_long_round_up_power2(8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_long_round_up_power2(5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_long_round_up_power2(12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_long_round_up_power2(243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_long_round_up_power2(1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_long_round_up_power2(9496729, 2), 9496730, "Round 9496729 up to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_long_round_up_power2(23372036854775808, 2), 23372036854775808, "Round 23372036854775808 up to nearest multiple of 2");
}

static void test_signed_long_round_down_power2(void) {
    TEST_ASSERT_EQ(signed_long_round_down_power2(19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_long_round_down_power2(5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_long_round_down_power2(12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_long_round_down_power2(243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_long_round_down_power2(1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_long_round_down_power2(32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_long_round_down_power2(9496729, 2), 9496728, "Round 9496729 down to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_long_round_down_power2(23372036854775808, 4), 23372036854775808, "Round 23372036854775808 down to nearest multiple of 4");
}

static void test_unsigned_long_round_up_power2(void) {
    TEST_ASSERT_EQ(unsigned_long_round_up_power2((uint64_t)8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_long_round_up_power2((uint64_t)5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_round_up_power2((uint64_t)12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_long_round_up_power2((uint64_t)243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_long_round_up_power2((uint64_t)1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_round_up_power2((uint64_t)65531, 2), 65532, "Round 65531 up to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_long_round_up_power2((uint64_t)2147483637, 4), 2147483640, "Round 2147483637 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_round_up_power2((uint64_t)23372036854775808, 2), 23372036854775808, "Round 23372036854775808 up to nearest multiple of 2");
}

static void test_unsigned_long_round_down_power2(void) {
    TEST_ASSERT_EQ(unsigned_long_round_down_power2((uint64_t)19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_round_down_power2((uint64_t)5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_long_round_down_power2((uint64_t)12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_round_down_power2((uint64_t)243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_long_round_down_power2((uint64_t)1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_long_round_down_power2((uint64_t)32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_round_down_power2((uint64_t)65535, 2), 65534, "Round 65535 down to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_long_round_down_power2((uint64_t)4294967295, 4), 4294967292, "Round 4294967295 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_round_down_power2((uint64_t)9223372036854775808, 2), 9223372036854775808, "Round 9223372036854775808 down to nearest multiple of 2");
}

static void test_signed_long_long_round_up_power2(void) {
    TEST_ASSERT_EQ(signed_long_long_round_up_power2(8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_long_long_round_up_power2(5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_long_long_round_up_power2(12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_long_long_round_up_power2(243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_long_long_round_up_power2(1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_long_long_round_up_power2(9496729, 2), 9496730, "Round 9496729 up to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_long_long_round_up_power2(23372036854775808, 2), 23372036854775808, "Round 23372036854775808 up to nearest multiple of 2");
}

static void test_signed_long_long_round_down_power2(void) {
    TEST_ASSERT_EQ(signed_long_long_round_down_power2(19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_long_long_round_down_power2(5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_long_long_round_down_power2(12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_long_long_round_down_power2(243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_long_long_round_down_power2(1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(signed_long_long_round_down_power2(32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(signed_long_long_round_down_power2(9496729, 2), 9496728, "Round 9496729 down to nearest multiple of 2");
    TEST_ASSERT_EQ(signed_long_long_round_down_power2(23372036854775808, 4), 23372036854775808, "Round 23372036854775808 down to nearest multiple of 4");
}

static void test_unsigned_long_long_round_up_power2(void) {
    TEST_ASSERT_EQ(unsigned_long_long_round_up_power2((uint64_t)8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_power2((uint64_t)5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_power2((uint64_t)12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_power2((uint64_t)243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_power2((uint64_t)1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_power2((uint64_t)65531, 2), 65532, "Round 65531 up to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_power2((uint64_t)2147483637, 4), 2147483640, "Round 2147483637 up to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_long_round_up_power2((uint64_t)23372036854775808, 2), 23372036854775808, "Round 23372036854775808 up to nearest multiple of 2");
}

static void test_unsigned_long_long_round_down_power2(void) {
    TEST_ASSERT_EQ(unsigned_long_long_round_down_power2((uint64_t)19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_power2((uint64_t)5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_power2((uint64_t)12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_power2((uint64_t)243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_power2((uint64_t)1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_power2((uint64_t)32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_power2((uint64_t)65535, 2), 65534, "Round 65535 down to nearest multiple of 2");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_power2((uint64_t)4294967295, 4), 4294967292, "Round 4294967295 down to nearest multiple of 4");
    TEST_ASSERT_EQ(unsigned_long_long_round_down_power2((uint64_t)9223372036854775808, 2), 9223372036854775808, "Round 9223372036854775808 down to nearest multiple of 2");
}

static void test_INT_ROUND_UP(void) {
    TEST_ASSERT_EQ(INT_ROUND_UP(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(INT_ROUND_UP(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(INT_ROUND_UP(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(INT_ROUND_UP(243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(INT_ROUND_UP(1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(INT_ROUND_UP(9496729, 12), 9496740, "Round 9496729 up to nearest multiple of 12");
    TEST_ASSERT_EQ(INT_ROUND_UP(23372036854775808, 7), 23372036854775812, "Round 23372036854775808 up to nearest multiple of 7");
    TEST_ASSERT_EQ(INT_ROUND_UP(223372036854775807, 6), 223372036854775812, "Round 223372036854775807 up to nearest multiple of 6");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint64_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint64_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint64_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint64_t)243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint64_t)1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint64_t)65531, 10), 65540, "Round 65531 up to nearest multiple of 10");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint64_t)2147483637, 10), 2147483640, "Round 2147483637 up to nearest multiple of 10");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint64_t)23372036854775808, 7), 23372036854775812, "Round 23372036854775808 up to nearest multiple of 7");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint64_t)1844674407370955134, 9), 1844674407370955142, "Round 1844674407370955134 up to nearest multiple of 9");
}

static void test_INT_ROUND_DOWN(void) {
    TEST_ASSERT_EQ(INT_ROUND_DOWN(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(INT_ROUND_DOWN(5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(INT_ROUND_DOWN(12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(INT_ROUND_DOWN(243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(INT_ROUND_DOWN(1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(INT_ROUND_DOWN(32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(INT_ROUND_DOWN(9496729, 10), 9496720, "Round 9496729 down to nearest multiple of 10");
    TEST_ASSERT_EQ(INT_ROUND_DOWN(23372036854775808, 7), 23372036854775805, "Round 23372036854775808 down to nearest multiple of 7");
    TEST_ASSERT_EQ(INT_ROUND_DOWN(223372036854775807, 6), 223372036854775806, "Round 223372036854775807 down to nearest multiple of 6");
    TEST_ASSERT_EQ(INT_ROUND_DOWN((uint64_t)19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(INT_ROUND_DOWN((uint64_t)5, 3), 3, "Round 5 down to nearest multiple of 3");
    TEST_ASSERT_EQ(INT_ROUND_DOWN((uint64_t)12, 1), 12, "Round 12 down to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(INT_ROUND_DOWN((uint64_t)243, 5), 240, "Round 243 down to nearest multiple of 5");
    TEST_ASSERT_EQ(INT_ROUND_DOWN((uint64_t)1643, 12), 1632, "Round 1643 down to nearest multiple of 12");
    TEST_ASSERT_EQ(INT_ROUND_DOWN((uint64_t)32761, 10), 32760, "Round 32761 down to nearest multiple of 10");
    TEST_ASSERT_EQ(INT_ROUND_DOWN((uint64_t)65535, 10), 65530, "Round 65535 down to nearest multiple of 10");
    TEST_ASSERT_EQ(INT_ROUND_DOWN((uint64_t)4294967295, 10), 4294967290, "Round 4294967295 down to nearest multiple of 10");
    TEST_ASSERT_EQ(INT_ROUND_DOWN((uint64_t)9223372036854775808, 10), 9223372036854775800, "Round 9223372036854775808 down to nearest multiple of 10");
    TEST_ASSERT_EQ(INT_ROUND_DOWN((uint64_t)1844674407370955134, 9), 1844674407370955133, "Round 1844674407370955134 down to nearest multiple of 9");
}

static void test_INT_ROUND_UP_POWER2(void) {
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2(8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2(5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2(12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2(243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2(1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2(9496729, 2), 9496730, "Round 9496729 up to nearest multiple of 2");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2(23372036854775808, 2), 23372036854775808, "Round 23372036854775808 up to nearest multiple of 2");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2((uint64_t)8, 2), 8, "Round 8 up to nearest multiple of 2");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2((uint64_t)5, 4), 8, "Round 5 up to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2((uint64_t)12, 8), 16, "Round 12 up to nearest multiple of 8");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2((uint64_t)243, 8), 248, "Round 243 up to nearest multiple of 8");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2((uint64_t)1643, 4), 1644, "Round 1643 up to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2((uint64_t)65531, 2), 65532, "Round 65531 up to nearest multiple of 2");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2((uint64_t)2147483637, 4), 2147483640, "Round 2147483637 up to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_UP_POWER2((uint64_t)23372036854775808, 2), 23372036854775808, "Round 23372036854775808 up to nearest multiple of 2");
}

static void test_INT_ROUND_DOWN_POWER2(void) {
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2(19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2(5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2(12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2(243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2(1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2(32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2(9496729, 2), 9496728, "Round 9496729 down to nearest multiple of 2");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2(23372036854775808, 4), 23372036854775808, "Round 23372036854775808 down to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2((uint64_t)19, 4), 16, "Round 19 down to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2((uint64_t)5, 2), 4, "Round 5 down to nearest multiple of 2");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2((uint64_t)12, 4), 12, "Round 12 down to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2((uint64_t)243, 8), 240, "Round 243 down to nearest multiple of 8");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2((uint64_t)1643, 8), 1640, "Round 1643 down to nearest multiple of 8");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2((uint64_t)32761, 4), 32760, "Round 32761 down to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2((uint64_t)65535, 2), 65534, "Round 65535 down to nearest multiple of 2");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2((uint64_t)4294967295, 4), 4294967292, "Round 4294967295 down to nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_DOWN_POWER2((uint64_t)9223372036854775808, 2), 9223372036854775808, "Round 9223372036854775808 down to nearest multiple of 2");
}

static void test_power_Of_Two(void) {
    for(uint16_t i = 0; i < 64; i++) {

        uint64_t expected = (uint64_t)1 << i;

        char msg[40];
        snprintf(msg, sizeof(msg), "%luth power of 2", i);

        TEST_ASSERT_EQ(power_Of_Two(i), expected, msg);
    }
}

static void test_log2_power2(void) {
    uint64_t powers_of_2[] = {
        1ULL,
        2ULL,
        4ULL,
        8ULL,
        16ULL,
        32ULL,
        64ULL,
        128ULL,
        256ULL,
        512ULL,
        1024ULL,
        2048ULL,
        4096ULL,
        8192ULL,
        16384ULL,
        32768ULL,
        65536ULL,
        131072ULL,
        262144ULL,
        524288ULL,
        1048576ULL,
        2097152ULL,
        4194304ULL,
        8388608ULL,
        16777216ULL,
        33554432ULL,
        67108864ULL,
        134217728ULL,
        268435456ULL,
        536870912ULL,
        1073741824ULL,
        2147483648ULL,
        4294967296ULL,
        8589934592ULL,
        17179869184ULL,
        34359738368ULL,
        68719476736ULL,
        137438953472ULL,
        274877906944ULL,
        549755813888ULL,
        1099511627776ULL,
        2199023255552ULL,
        4398046511104ULL,
        8796093022208ULL,
        17592186044416ULL,
        35184372088832ULL,
        70368744177664ULL,
        140737488355328ULL,
        281474976710656ULL,
        562949953421312ULL,
        1125899906842624ULL,
        2251799813685248ULL,
        4503599627370496ULL,
        9007199254740992ULL,
        18014398509481984ULL,
        36028797018963968ULL,
        72057594037927936ULL,
        144115188075855872ULL,
        288230376151711744ULL,
        576460752303423488ULL,
        1152921504606846976ULL,
        2305843009213693952ULL,
        4611686018427387904ULL,
        9223372036854775808ULL
    };
    for(uint64_t i = 0; i < 64; i++) {
        char msg[60];
        snprintf(msg, sizeof(msg), "%lu is the %luth power of 2", powers_of_2[i], i);

        TEST_ASSERT_EQ(log2_power2(powers_of_2[i]), i, msg);
    }
}

static void test_raise_to_power(void) {
    TEST_ASSERT(fabs(raise_to_power(4, 3) - 64) < EPSILON, "4 to power 3");
    TEST_ASSERT(fabs(raise_to_power(12, 5) - 248832) < EPSILON, "12 to power 5");
    TEST_ASSERT(fabs(raise_to_power(-2, 3) + 8) < EPSILON, "-2 to power 3");
    TEST_ASSERT(fabs(raise_to_power(-2, 4) - 16) < EPSILON, "-2 to power 4");
    TEST_ASSERT(fabs(raise_to_power(2, -4) - 0.0625) < EPSILON, "2 raised to power -4");
    TEST_ASSERT(fabs(raise_to_power(3, -2) - 0.11111) < EPSILON, "3 raised to power -2");
    TEST_ASSERT(isinf(raise_to_power(0, -4)), "0 raised to power -4 returns infinity");
    TEST_ASSERT(fabs(raise_to_power(4, 0) - 1) < EPSILON, "4 to power 0 is 1");
}

void run_math_utils_tests(void) {
    test_M_Min();
    test_M_Max();
    test_uint8_round_up_generic();
    test_int8_round_up_generic();
    test_signed_char_round_up_generic();
    test_signed_char_round_down_generic();
    test_unsigned_char_round_up_generic();
    test_unsigned_char_round_down_generic();
    test_signed_short_round_up_generic();
    test_signed_short_round_down_generic();
    test_unsigned_short_round_up_generic();
    test_unsigned_short_round_down_generic();
    test_signed_int_round_up_generic();
    test_signed_int_round_down_generic();
    test_unsigned_int_round_up_generic();
    test_unsigned_int_round_down_generic();
    test_signed_long_round_up_generic();
    test_signed_long_round_down_generic();
    test_unsigned_long_round_up_generic();
    test_unsigned_long_round_down_generic();
    test_signed_long_long_round_up_generic();
    test_signed_long_long_round_down_generic();
    test_unsigned_long_long_round_up_generic();
    test_unsigned_long_long_round_down_generic();
    test_int16_round_up_generic();
    test_uint16_round_up_generic();
    test_int32_round_up_generic();
    test_uint32_round_up_generic();
    test_int64_round_up_generic();
    test_uint64_round_up_generic();
    test_int8_round_down_generic();
    test_uint8_round_down_generic();
    test_int16_round_down_generic();
    test_uint16_round_down_generic();
    test_int32_round_down_generic();
    test_uint32_round_down_generic();
    test_int64_round_down_generic();
    test_uint64_round_down_generic();
    test_uint8_round_up_power2();
    test_int8_round_up_power2();
    test_uint16_round_up_power2();
    test_int16_round_up_power2();
    test_uint32_round_up_power2();
    test_int32_round_up_power2();
    test_uint64_round_up_power2();
    test_int64_round_up_power2();
    test_int8_round_down_power2();
    test_uint8_round_down_power2();
    test_int16_round_down_power2();
    test_uint16_round_down_power2();
    test_int32_round_down_power2();
    test_uint32_round_down_power2();
    test_int64_round_down_power2();
    test_uint64_round_down_power2();
    test_signed_char_round_up_power2();
    test_signed_char_round_down_power2();
    test_unsigned_char_round_up_power2();
    test_unsigned_char_round_down_power2();
    test_signed_short_round_up_power2();
    test_signed_short_round_down_power2();
    test_unsigned_short_round_up_power2();
    test_unsigned_short_round_down_power2();
    test_signed_int_round_up_power2();
    test_signed_int_round_down_power2();
    test_unsigned_int_round_up_power2();
    test_unsigned_int_round_down_power2();
    test_signed_long_round_up_power2();
    test_signed_long_round_down_power2();
    test_unsigned_long_round_up_power2();
    test_unsigned_long_round_down_power2();
    test_signed_long_long_round_up_power2();
    test_signed_long_long_round_down_power2();
    test_unsigned_long_long_round_up_power2();
    test_unsigned_long_long_round_down_power2();
    test_INT_ROUND_UP();
    test_INT_ROUND_DOWN();
    test_INT_ROUND_UP_POWER2();
    test_INT_ROUND_DOWN_POWER2();
    test_power_Of_Two();
    test_log2_power2();
    test_raise_to_power();
}