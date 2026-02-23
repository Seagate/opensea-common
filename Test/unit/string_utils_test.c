#include"../framework/test_framework.h"
#include"../../include/string_utils.h"
#include"../testConstants.h"

static void test_strcasecmp(void) {
    TEST_ASSERT_EQ(strcasecmp("Pratiksha", "pratiksha"), 0, "Case insensitive strings are equal");
    TEST_ASSERT(strcasecmp("abc", "adc") < 0, "abc < adc");
    TEST_ASSERT(strcasecmp("adc", "abc") > 0, "adc > abc");
}

void run_string_utils_tests(void) {
    test_strcasecmp();
}
