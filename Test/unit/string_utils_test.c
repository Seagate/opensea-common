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

char lowerCaseAlphabet[] = "abcdefghijklmnopqrstuvwxyz";
char upperCaseAlphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char numberChars[] = "0123456789";
char specialChars[] = "!@#$%^&*()_+-=~`";

static void test_safe_isalnum(void) {

    char msg[60];

    for(int i = 0; i < sizeof(lowerCaseAlphabet); i++) {
        snprintf(msg, sizeof(msg), "Alphanumeric character %c returns non-zero value", lowerCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isalnum(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet); i++) {
        snprintf(msg, sizeof(msg), "Alphanumeric character %c returns non-zero value", upperCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isalnum(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars); i++) {
        snprintf(msg, sizeof(msg), "Alphanumeric character %c returns non-zero value", numberChars[i]);
        TEST_ASSERT_NEQ(safe_isalnum(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars); i++) {
        snprintf(msg, sizeof(msg), "Non-alphanumeric character %c return 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_isalnum(specialChars[i]), 0, msg);
    }
}

static void test_safe_isalpha(void) {

    char msg[60];

    for(int i = 0; i < sizeof(lowerCaseAlphabet); i++) {
        snprintf(msg, sizeof(msg), "Alphabetic character %c returns non-zero value", lowerCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isalpha(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet); i++) {
        snprintf(msg, sizeof(msg), "Alphabetic character %c returns non-zero value", upperCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isalpha(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars); i++) {
        snprintf(msg, sizeof(msg), "Non-alphabetic character %c return 0", numberChars[i]);
        TEST_ASSERT_EQ(safe_isalpha(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars); i++) {
        snprintf(msg, sizeof(msg), "Non-alphabetic character %c return 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_isalpha(specialChars[i]), 0, msg);
    }
}

void run_string_utils_tests(void) {
    test_strcasecmp();
    test_strncasecmp();
    test_is_ASCII();
    test_safe_isascii();
    test_safe_isalnum();
    test_safe_isalpha();
}
