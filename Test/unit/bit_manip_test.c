#include "../framework/test_framework.h"
#include "../../include/bit_manip.h"

void run_bit_manip_tests(void)
{
    TEST_ASSERT_EQ(get_DWord0(0x123456789ABCDEF0ULL), 0x9ABCDEF0U, "get_DWord0: lower 32 bits");
    TEST_ASSERT_EQ(get_DWord1(0x123456789ABCDEF0ULL), 0x12345678U, "get_DWord1: upper 32 bits");
}
