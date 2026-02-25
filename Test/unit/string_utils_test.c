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

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Alphanumeric character %c returns non-zero value", lowerCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isalnum(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Alphanumeric character %c returns non-zero value", upperCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isalnum(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Alphanumeric character %c returns non-zero value", numberChars[i]);
        TEST_ASSERT_NEQ(safe_isalnum(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-alphanumeric character %c return 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_isalnum(specialChars[i]), 0, msg);
    }
}

static void test_safe_isalpha(void) {
    char msg[60];

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Alphabetic character %c returns non-zero value", lowerCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isalpha(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Alphabetic character %c returns non-zero value", upperCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isalpha(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-alphabetic character %c return 0", numberChars[i]);
        TEST_ASSERT_EQ(safe_isalpha(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-alphabetic character %c return 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_isalpha(specialChars[i]), 0, msg);
    }
}

static void test_safe_islower(void) {
    char msg[60];

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Lowercase character %c returns non-zero value", lowerCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_islower(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Uppercase character %c returns 0", upperCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_islower(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-lowercase character %c return 0", numberChars[i]);
        TEST_ASSERT_EQ(safe_islower(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-lowercase character %c return 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_islower(specialChars[i]), 0, msg);
    }
}

static void test_safe_isupper(void) {
    char msg[60];

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Uppercase character %c returns non-zero value", upperCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isupper(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Lowercase character %c returns 0", lowerCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_isupper(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-uppercase character %c return 0", numberChars[i]);
        TEST_ASSERT_EQ(safe_isupper(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-uppercase character %c return 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_isupper(specialChars[i]), 0, msg);
    }
}

static void test_safe_isdigit(void) {
    char msg[60];

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Digit character %c returns non-zero value", numberChars[i]);
        TEST_ASSERT_NEQ(safe_isdigit(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Non-digit character %c returns 0", lowerCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_isdigit(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-digit character %c returns 0", upperCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_isdigit(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-digit character %c return 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_isdigit(specialChars[i]), 0, msg);
    }
}

static void test_safe_isxdigit(void) {
    char msg[60];

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Hexadecimal digit character %c returns non-zero value", numberChars[i]);
        TEST_ASSERT_NEQ(safe_isxdigit(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Hexadecimal digit character %c returns non-zero value if it is a-f", lowerCaseAlphabet[i]);
        if(lowerCaseAlphabet[i] >= 'a' && lowerCaseAlphabet[i] <= 'f') {
            TEST_ASSERT_NEQ(safe_isxdigit(lowerCaseAlphabet[i]), 0, msg);
        } else {
            TEST_ASSERT_EQ(safe_isxdigit(lowerCaseAlphabet[i]), 0, msg);
        }
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Hexadecimal digit character %c returns non-zero value if it is A-F", upperCaseAlphabet[i]);
        if(upperCaseAlphabet[i] >= 'A' && upperCaseAlphabet[i] <= 'F') {
            TEST_ASSERT_NEQ(safe_isxdigit(upperCaseAlphabet[i]), 0, msg);
        } else {
            TEST_ASSERT_EQ(safe_isxdigit(upperCaseAlphabet[i]), 0, msg);
        }
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-hexadecimal digit character %c return 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_isxdigit(specialChars[i]), 0, msg);
    }
}

uint8_t cntrl_chars[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x7F
};

static void test_safe_iscntrl(void) {
    char msg[60];

    for(int i = 0; i < sizeof(cntrl_chars); i++) {
        snprintf(msg, sizeof(msg), "Control character 0x%02X returns non-zero value", cntrl_chars[i]);
        TEST_ASSERT_NEQ(safe_iscntrl(cntrl_chars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Non-control character %c returns 0", lowerCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_iscntrl(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-control character %c returns 0", upperCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_iscntrl(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-control character %c return 0", numberChars[i]);
        TEST_ASSERT_EQ(safe_iscntrl(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-control character %c return 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_iscntrl(specialChars[i]), 0, msg);
    }
}

char space_chars[] = " \t\n\r\f\v";

static void test_safe_isgraph(void) {
    char msg[60];

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Graphical character %c returns non-zero value", lowerCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isgraph(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Graphical character %c returns non-zero value", upperCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isgraph(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Graphical character %c return non-zero value", numberChars[i]);
        TEST_ASSERT_NEQ(safe_isgraph(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Graphical character %c return non-zero value", specialChars[i]);
        TEST_ASSERT_NEQ(safe_isgraph(specialChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(cntrl_chars); i++) {
        snprintf(msg, sizeof(msg), "Control character 0x%02X returns 0", cntrl_chars[i]);
        TEST_ASSERT_EQ(safe_isgraph(cntrl_chars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(space_chars)-1; i++) {
        snprintf(msg, sizeof(msg), "Whitespace character %c returns 0", space_chars[i]);
        TEST_ASSERT_EQ(safe_isgraph(space_chars[i]), 0, msg);
    }
}

static void test_safe_isspace(void) {
    char msg[60];

    for(int i = 0; i < sizeof(space_chars)-1; i++) {
        snprintf(msg, sizeof(msg), "Whitespace character %c returns non-zero value", space_chars[i]);
        TEST_ASSERT_NEQ(safe_isspace(space_chars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Non-whitespace character %c returns 0", lowerCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_isspace(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-whitespace character %c returns 0", upperCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_isspace(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-whitespace character %c return 0", numberChars[i]);
        TEST_ASSERT_EQ(safe_isspace(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-whitespace character %c return 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_isspace(specialChars[i]), 0, msg);
    }
}

static void test_safe_isblank(void) {
    char msg[60];

    for(int i = 0; i < sizeof(space_chars)-1; i++) {
        if(space_chars[i] == ' ' || space_chars[i] == '\t') {
            snprintf(msg, sizeof(msg), "Blank character %c returns non-zero value", space_chars[i]);
            TEST_ASSERT_NEQ(safe_isblank(space_chars[i]), 0, msg);
        } else {
            snprintf(msg, sizeof(msg), "Non-blank character %c returns 0", space_chars[i]);
            TEST_ASSERT_EQ(safe_isblank(space_chars[i]), 0, msg);
        }
    }

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Non-blank character %c returns 0", lowerCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_isblank(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-blank character %c returns 0", upperCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_isblank(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-blank character %c returns 0", numberChars[i]);
        TEST_ASSERT_EQ(safe_isblank(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-blank character %c returns 0", specialChars[i]);
        TEST_ASSERT_EQ(safe_isblank(specialChars[i]), 0, msg);
    }
}

static void test_safe_isprint(void) {
    char msg[60];

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Printable character %c returns non-zero value", lowerCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isprint(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Printable character %c returns non-zero value", upperCaseAlphabet[i]);
        TEST_ASSERT_NEQ(safe_isprint(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Printable character %c return non-zero value", numberChars[i]);
        TEST_ASSERT_NEQ(safe_isprint(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Printable character %c return non-zero value", specialChars[i]);
        TEST_ASSERT_NEQ(safe_isprint(specialChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(cntrl_chars); i++) {
        snprintf(msg, sizeof(msg), "Control character 0x%02X returns 0", cntrl_chars[i]);
        TEST_ASSERT_EQ(safe_isprint(cntrl_chars[i]), 0, msg);
    }
}

static void test_safe_ispunct(void) {
    char msg[60];

    for(int i = 0; i < sizeof(specialChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Punctuation character %c returns non-zero value", specialChars[i]);
        TEST_ASSERT_NEQ(safe_ispunct(specialChars[i]), 0, msg);
    }   

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "Non-punctuation character %c returns 0", lowerCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_ispunct(lowerCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-punctuation character %c returns 0", upperCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_ispunct(upperCaseAlphabet[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(numberChars)-1; i++) {
        snprintf(msg, sizeof(msg), "Non-punctuation character %c returns 0", numberChars[i]);
        TEST_ASSERT_EQ(safe_ispunct(numberChars[i]), 0, msg);
    }

    for(int i = 0; i < sizeof(cntrl_chars); i++) {
        snprintf(msg, sizeof(msg), "Non-punctuation character 0x%02X returns 0", cntrl_chars[i]);
        TEST_ASSERT_EQ(safe_ispunct(cntrl_chars[i]), 0, msg);
    }
}

static void test_safe_tolower(void) {
    char msg[60];

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "tolower converts %c to %c", upperCaseAlphabet[i], lowerCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_tolower(upperCaseAlphabet[i]), lowerCaseAlphabet[i], msg);
    }
}

static void test_safe_toupper(void) {
    char msg[60];

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "toupper converts %c to %c", lowerCaseAlphabet[i], upperCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_toupper(lowerCaseAlphabet[i]), upperCaseAlphabet[i], msg);
    }
}

static void test_safe_strnlen(void) {
    TEST_ASSERT_EQ(safe_strnlen("Hello, World!", 20), 13, "String length is correctly calculated when null terminator is within the string length");
    TEST_ASSERT_EQ(safe_strnlen("Hello, World!", 10), 10, "Returns n when null terminator is not found within the first n characters");
    TEST_ASSERT_EQ(safe_strnlen("", 10), 0, "Returns 0 for empty string");
}

static void test_safe_strlen(void) {
    TEST_ASSERT_EQ(safe_strlen("Hello, World!"), 13, "String length is correctly calculated");
    TEST_ASSERT_EQ(safe_strlen(""), 0, "Returns 0 for empty string");
    TEST_ASSERT_EQ(safe_strlen(NULL), 0, "Returns 0 for null pointer input");
}

void run_string_utils_tests(void) {
    test_strcasecmp();
    test_strncasecmp();
    test_is_ASCII();
    test_safe_isascii();
    test_safe_isalnum();
    test_safe_isalpha();
    test_safe_islower();
    test_safe_isupper();
    test_safe_isdigit();
    test_safe_isxdigit();
    test_safe_iscntrl();
    test_safe_isgraph();
    test_safe_isspace();
    test_safe_isblank();
    test_safe_isprint();
    test_safe_ispunct();
    test_safe_tolower();
    test_safe_toupper();
    test_safe_strnlen();
    test_safe_strlen();
}
