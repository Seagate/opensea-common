#include"../framework/test_framework.h"
#include"../../include/string_utils.h"
#include"../testConstants.h"

static void test_strcasecmp(void) {
    TEST_ASSERT_EQ(strcasecmp("Test", "test"), 0, "Case insensitive strings are equal");
    TEST_ASSERT(strcasecmp("abc", "adc") < 0, "String 1 is less than string 2");
    TEST_ASSERT(strcasecmp("adc", "abc") > 0, "String 1 is greater than string 2");
}

static void test_strncasecmp(void) {
    TEST_ASSERT_EQ(strncasecmp("Test", "testing", 4), 0, "Case insensitive strings are equal till given length");
    TEST_ASSERT(strncasecmp("test", "testing", 5) < 0, "String 1 is less than string 2");
    TEST_ASSERT(strncasecmp("testing", "test", 5) > 0, "String 1 is greater than string 2");
}

static void test_is_ASCII(void) {
    TEST_ASSERT_EQ(is_ASCII('A'), 1, "ASCII characters return 1");
    TEST_ASSERT_EQ(is_ASCII(290), 0, "non-ASCII characters return 0");
    TEST_ASSERT_EQ(is_ASCII(-1), 0, "EOF returns 0");
}

static void test_safe_isascii(void) {
    TEST_ASSERT_EQ(safe_isascii('A'), 1, "ASCII characters return non-zero value using safe_isascii");
    TEST_ASSERT_EQ(safe_isascii(290), 0, "non-ASCII characters return 0 using safe_isascii");
    TEST_ASSERT_EQ(safe_isascii(-1), 0, "EOF returns 0 using safe_isascii");
}

static void test_safe_isalnum(void) {
    TEST_ASSERT_NEQ(safe_isalnum('A'), 0, "Alphanumeric character returns non-zero value");
    TEST_ASSERT_EQ(safe_isalnum('#'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('!'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('@'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('$'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('%'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('^'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('&'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('*'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('('), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum(')'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('_'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('+'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('-'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('='), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('`'), 0, "Non-alphanumeric character return 0");
    TEST_ASSERT_EQ(safe_isalnum('~'), 0, "Non-alphanumeric character return 0");
}

static void test_safe_isalpha(void) {
    TEST_ASSERT_NEQ(safe_isalpha('z'), 0, "Alphabetic character returns non-zero value");
    TEST_ASSERT_EQ(safe_isalpha('#'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('!'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('@'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('$'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('%'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('^'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('&'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('*'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('('), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha(')'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('_'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('+'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('-'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('='), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('`'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('~'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('0'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('1'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('2'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('3'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('4'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('5'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('6'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('7'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('8'), 0, "Non-alphabetic character return 0");
    TEST_ASSERT_EQ(safe_isalpha('9'), 0, "Non-alphabetic character return 0");
}

void run_string_utils_tests(void) {
    test_strcasecmp();
    test_strncasecmp();
    test_is_ASCII();
    test_safe_isascii();
    test_safe_isalnum();
    test_safe_isalpha();
}
