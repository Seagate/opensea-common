#include"../framework/test_framework.h"
#include"../../include/math_utils.h"
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

static void test_INT_ROUND_UP(void) {
    TEST_ASSERT_EQ(INT_ROUND_UP((uint8_t)0xC, 5), (uint8_t)0xA, "Round 0xC to the nearest multiple of 5");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint16_t)0x7B3D, 4), (uint16_t)0x7B3C, "Round 0x7B3D to the nearest multiple of 4");
    TEST_ASSERT_EQ(INT_ROUND_UP((uint32_t)0xA73C91D6, 8), (uint32_t)0xA73C91D8, "Round 0xA73C91D6 to the nearest multiple of 8");
    TEST_ASSERT_EQ(INT_ROUND_UP(0x9F3A7C5D8E21B4C7UL, 2), 0x9F3A7C5D8E21B4C8UL, "Round 0x9F3A7C5D8E21B4C7UL to the nearest multiple of 2");
    TEST_ASSERT_EQ(INT_ROUND_UP(0x9F3A7C5D8E21B4C7ULL, 2), 0x9F3A7C5D8E21B4C7ULL, "Round 0x9F3A7C5D8E21B4C7ULL to the nearest multiple of 2");
}

static void test_uint8_round_up_generic(void) {
    TEST_ASSERT_EQ(uint8_round_up_generic((uint8_t)8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(uint8_round_up_generic((uint8_t)5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(uint8_round_up_generic((uint8_t)12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(uint8_round_up_generic((uint8_t)251, 10), 4, "Round 251 up to nearest multiple of 10: Overflow case");
}

static void test_int8_round_up_generic(void) {
    TEST_ASSERT_EQ(int8_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(int8_round_up_generic(5, 7), 7, "Round 5 up to nearest multiple of 7");
    TEST_ASSERT_EQ(int8_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(int8_round_up_generic(121, 10), -126, "Round 121 up to nearest multiple of 10: Overflow case");
}

static void test_signed_char_round_up_generic(void) {
    TEST_ASSERT_EQ(signed_char_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_char_round_up_generic(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_char_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_char_round_up_generic(121, 10), -126, "Round 121 up to nearest multiple of 10: Overflow case");
}

static void test_signed_char_round_down_generic(void) {
    TEST_ASSERT_EQ(signed_char_round_down_generic(19, 4), 16, "Round 19 down to nearest multiple of 4: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_char_round_down_generic(5, 3), 3, "Round 5 down to nearest multiple of 3");
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
    TEST_ASSERT_EQ(signed_short_round_up_generic(8, 2), 8, "Round 8 up to nearest multiple of 2: Roundto is power of 2");
    TEST_ASSERT_EQ(signed_short_round_up_generic(5, 3), 6, "Round 5 up to nearest multiple of 3");
    TEST_ASSERT_EQ(signed_short_round_up_generic(12, 1), 12, "Round 12 up to nearest multiple of 1: no-op operation");
    TEST_ASSERT_EQ(signed_short_round_up_generic(243, 5), 245, "Round 243 up to nearest multiple of 5");
    TEST_ASSERT_EQ(signed_short_round_up_generic(1643, 12), 1644, "Round 1643 up to nearest multiple of 12");
    TEST_ASSERT_EQ(signed_short_round_up_generic(32761, 10), -32766, "Round 32761 up to nearest multiple of 10: Overflow case");
}

void run_math_utils_tests(void) {
    test_M_Min();
    test_M_Max();
    // test_INT_ROUND_UP();
    test_uint8_round_up_generic();
    test_int8_round_up_generic();
    test_signed_char_round_up_generic();
    test_signed_char_round_down_generic();
    test_unsigned_char_round_up_generic();
    test_unsigned_char_round_down_generic();
    test_signed_short_round_up_generic();
}