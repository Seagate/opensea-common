#include"../framework/test_framework.h"
#include"../../include/string_utils.h"
#include"../testConstants.h"
#include"../../include/memory_safety.h"

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
    // handles eof
    TEST_ASSERT_EQ(safe_isascii(EOF), 0, "EOF returns 0 using safe_isascii");
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_isalnum(-1), 0, "safe_isalnum returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_isalnum sets errno to ERANGE for negative values");
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_isalpha(-1), 0, "safe_isalpha returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_isalpha sets errno to ERANGE for negative values");
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_islower(-1), 0, "safe_islower returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_islower sets errno to ERANGE for negative values");
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_isupper(-1), 0, "safe_isupper returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_isupper sets errno to ERANGE for negative values");
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_isdigit(-1), 0, "safe_isdigit returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_isdigit sets errno to ERANGE for negative values");
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_isxdigit(-1), 0, "safe_isxdigit returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_isxdigit sets errno to ERANGE for negative values");
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_iscntrl(-1), 0, "safe_iscntrl returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_iscntrl sets errno to ERANGE for negative values");
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
        snprintf(msg, sizeof(msg), "Whitespace character returns 0", space_chars[i]);
        TEST_ASSERT_EQ(safe_isgraph(space_chars[i]), 0, msg);
    }

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_isgraph(-1), 0, "safe_isgraph returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_isgraph sets errno to ERANGE for negative values");
}

static void test_safe_isspace(void) {
    char msg[60];

    for(int i = 0; i < sizeof(space_chars)-1; i++) {
        snprintf(msg, sizeof(msg), "Whitespace character returns non-zero value", space_chars[i]);
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_isspace(-1), 0, "safe_isspace returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_isspace sets errno to ERANGE for negative values");
}

static void test_safe_isblank(void) {
    char msg[60];

    for(int i = 0; i < sizeof(space_chars)-1; i++) {
        if(space_chars[i] == ' ' || space_chars[i] == '\t') {
            snprintf(msg, sizeof(msg), "Blank character ' ' and '\t' return non-zero value");
            TEST_ASSERT_NEQ(safe_isblank(space_chars[i]), 0, msg);
        } else {
            snprintf(msg, sizeof(msg), "Non-blank character returns 0", space_chars[i]);
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_isblank(-1), 0, "safe_isblank returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_isblank sets errno to ERANGE for negative values");
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_isprint(-1), 0, "safe_isprint returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_isprint sets errno to ERANGE for negative values");
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

    // Test for invalid unsigned char values
    TEST_ASSERT_EQ(safe_ispunct(-1), 0, "safe_ispunct returns 0 for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_ispunct sets errno to ERANGE for negative values");
}

static void test_safe_tolower(void) {
    char msg[60];

    for(int i = 0; i < sizeof(upperCaseAlphabet)-1; i++) {
        snprintf(msg, sizeof(msg), "tolower converts %c to %c", upperCaseAlphabet[i], lowerCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_tolower(upperCaseAlphabet[i]), lowerCaseAlphabet[i], msg);
    }

    // Test for input = EOF
    TEST_ASSERT_EQ(safe_tolower(EOF), EOF, "safe_tolower returns EOF for input = EOF");
    TEST_ASSERT(errno == 0, "safe_tolower does not set errno for input = EOF");

    // Test for  invalid unsigned char values
    TEST_ASSERT_EQ(safe_tolower(300), 300, "safe_tolower returns EOF for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_tolower sets errno to ERANGE for negative values");
}

static void test_safe_toupper(void) {
    char msg[60];

    for(int i = 0; i < sizeof(lowerCaseAlphabet)-1 ; i++) {
        snprintf(msg, sizeof(msg), "toupper converts %c to %c", lowerCaseAlphabet[i], upperCaseAlphabet[i]);
        TEST_ASSERT_EQ(safe_toupper(lowerCaseAlphabet[i]), upperCaseAlphabet[i], msg);
    }

    // Test for input = EOF
    TEST_ASSERT_EQ(safe_toupper(EOF), EOF, "safe_toupper returns EOF for input = EOF");
    TEST_ASSERT(errno == 0, "safe_toupper does not set errno for input = EOF");

    // Test for  invalid unsigned char values
    TEST_ASSERT_EQ(safe_toupper(300), 300, "safe_toupper returns EOF for negative values");
    TEST_ASSERT(errno == ERANGE, "safe_toupper sets errno to ERANGE for negative values");
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

static void test_safe_strcpy(void) {
    char dest[20];
    const char* src = "Hello, World!";
    safe_strcpy(dest, sizeof(dest), src);
    TEST_ASSERT_EQ(strcmp(dest, src), 0, "String is correctly copied to destination buffer");

    // Test for buffer overflow protection
    // char smallDest[5];
    // errno = 0;
    // safe_strcpy(smallDest, sizeof(smallDest), src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcpy sets errno to ERANGE when destination buffer is too small");

    // Test for null pointer protection
    // errno = 0;
    // safe_strcpy(NULL, sizeof(dest), src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcpy sets errno to ERANGE when destination pointer is null");
    // errno = 0;
    // src = NULL;
    // safe_strcpy(dest, sizeof(dest), NULL);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcpy sets errno to ERANGE when source pointer is null");

    // Test for zero and too large destsz
    // errno = 0;
    // safe_strcpy(dest, 0, src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcpy sets errno to ERANGE when destsz is zero");
    // errno = 0;
    // safe_strcpy(dest, RSIZE_MAX + 1, src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcpy sets errno to ERANGE when destsz is greater than RSIZE_MAX");
}

static void test_safe_strmove(void) {
    char dest[20];
    const char* src = "Hello, World!";
    safe_strmove(dest, sizeof(dest), src);
    TEST_ASSERT_EQ(strcmp(dest, src), 0, "String is correctly moved to destination buffer");

    // Test for overlapping
    char str[20] = "This String";
    // Move "String" one position left (overwrite space)
    errno_t err = safe_strmove(str + 4, sizeof(str) - 4, str + 5);
    TEST_ASSERT_EQ(err, 0, "Move should succeed");
    TEST_ASSERT_EQ(strcmp(str, "ThisString"), 0, "String should be shifted left correctly");

    // Test for buffer overflow protection
    // char smallDest[5];
    // errno = 0;
    // safe_strcpy(smallDest, sizeof(smallDest), src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcpy sets errno to ERANGE when destination buffer is too small");

    // Test for null pointer protection
    // errno = 0;
    // safe_strcpy(NULL, sizeof(dest), src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcpy sets errno to ERANGE when destination pointer is null");
    // errno = 0;
    // src = NULL;
    // safe_strcpy(dest, sizeof(dest), NULL);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcpy sets errno to ERANGE when source pointer is null");

    //Test for zero and too large destsz
    // errno = 0;
    // safe_strcpy(dest, 0, src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcpy sets errno to ERANGE when destsz is zero");
    // errno = 0;
    // safe_strcpy(dest, RSIZE_MAX + 1, src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcpy sets errno to ERANGE when destsz is greater than RSIZE_MAX");
}

static void test_safe_strncpy(void) {
    char dest[20];
    const char* src = "Hello, World!";
    safe_strncpy(dest, sizeof(dest), src, 5);
    TEST_ASSERT_EQ(strncmp(dest, src, 5), 0, "First n characters are correctly copied to destination buffer");
    TEST_ASSERT_EQ(dest[5], '\0', "Destination buffer is null-terminated after copying n characters");

    // Test when srclen < count
    char shortSrc[] = "Hi";
    safe_strncpy(dest, sizeof(dest), shortSrc, 10);
    TEST_ASSERT(strcmp(dest, "Hi") == 0, "Source shorter than count copied correctly");
    TEST_ASSERT(dest[2] == '\0', "Null terminator placed after source");

    // Test for buffer overflow protection
    // char smallDest[5];
    // errno = 0;
    // safe_strncpy(smallDest, sizeof(smallDest), src, 5);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when destination buffer is too small");

    // Test for null pointer protection
    // errno = 0;
    // safe_strncpy(NULL, sizeof(dest), src, 5);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when destination pointer is null");
    // errno = 0;
    // src = NULL;
    // safe_strncpy(dest, sizeof(dest), NULL, 5);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when source pointer is null");

    // Test for zero and too large destsz
    // errno = 0;
    // safe_strncpy(dest, 0, src, 5);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when destsz is zero");
    // errno = 0;
    // safe_strncpy(dest, RSIZE_MAX + 1, src, 5);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when destsz is greater than RSIZE_MAX");

    // Count greater than RSIZE_MAX
    // errno = 0;
    // safe_strncpy(dest, sizeof(dest), src, RSIZE_MAX + 1);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when count is greater than RSIZE_MAX");

    // Count greater than or equal to destsz, but destsz is less than or equal to strnlen_s(src, count); truncation would occur
    // errno = 0;
    // safe_strncpy(dest, 5, src, 10);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when count is greater than or equal to destsz and destsz is less than or equal to strnlen_s(src, count)");
}

static void test_safe_strnmove(void) {
    char dest[20];
    const char* src = "Hello, World!";
    safe_strnmove(dest, sizeof(dest), src, 5);
    TEST_ASSERT_EQ(strncmp(dest, src, 5), 0, "First n characters are correctly moved to destination buffer");
    TEST_ASSERT_EQ(dest[5], '\0', "Destination buffer is null-terminated after moving n characters");

    // Test for overlapping
    char str[20] = "This String";
    // Move "String" one position left (overwrite space)
    errno_t err = safe_strnmove(str + 4, sizeof(str) - 4, str + 5, 5);
    TEST_ASSERT_EQ(err, 0, "Move should succeed");
    TEST_ASSERT_EQ(strcmp(str, "ThisStrin"), 0, "String should be shifted left correctly");

    // Test for buffer overflow protection
    // char smallDest[5];
    // errno = 0;
    // safe_strncpy(smallDest, sizeof(smallDest), src, 5);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when destination buffer is too small");

    // Test for null pointer protection
    // errno = 0;
    // safe_strncpy(NULL, sizeof(dest), src, 5);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when destination pointer is null");
    // errno = 0;
    // src = NULL;
    // safe_strncpy(dest, sizeof(dest), NULL, 5);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when source pointer is null");

    // Test for zero and too large destsz
    // errno = 0;
    // safe_strncpy(dest, 0, src, 5);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when destsz is zero");
    // errno = 0;
    // safe_strncpy(dest, RSIZE_MAX + 1, src, 5);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when destsz is greater than RSIZE_MAX");

    // Count greater than RSIZE_MAX
    // errno = 0;
    // safe_strncpy(dest, sizeof(dest), src, RSIZE_MAX + 1);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when count is greater than RSIZE_MAX");

    // Count greater than or equal to destsz, but destsz is less than or equal to strnlen_s(src, count); truncation would occur
    // errno = 0;
    // safe_strncpy(dest, 5, src, 10);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncpy sets errno to ERANGE when count is greater than or equal to destsz and destsz is less than or equal to strnlen_s(src, count)");
}

static void test_safe_strcat(void) {
    char dest[20] = "Hello ";
    const char* src = "World";
    safe_strcat(dest, sizeof(dest), src);
    TEST_ASSERT_EQ(strcmp(dest, "Hello World"), 0, "String is correctly concatenated to destination buffer");

    // Test for null pointer protection
    // errno = 0;
    // safe_strcat(NULL, sizeof(dest), src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcat sets errno to ERANGE when destination pointer is null");
    // errno = 0;   
    // safe_strcat(dest, sizeof(dest), NULL);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcat sets errno to ERANGE when source pointer is null");

    // Test for zero and too large destsz
    // errno = 0;
    // safe_strcat(dest, 0, src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcat sets errno to ERANGE when destsz is zero");
    // errno = 0;
    // safe_strcat(dest, RSIZE_MAX + 1, src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcat sets errno to ERANGE when destsz is greater than RSIZE_MAX");
    
    // Test for no null terminator in the first destsz bytes of dest
    // char str[20] = "This string is too long for the buffer";
    // errno = 0;   
    // safe_strcat(str, sizeof(str), src);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strcat sets errno to ERANGE when there is no null terminator in the first destsz bytes of dest");
}

static void test_safe_strncat(void) {
    char dest[20] = "Hello ";
    const char* src = "World";
    safe_strncat(dest, sizeof(dest), src, 3);
    TEST_ASSERT_EQ(strcmp(dest, "Hello Wor"), 0, "First n characters are correctly concatenated to destination buffer");
    TEST_ASSERT_EQ(dest[9], '\0', "Destination buffer is null-terminated after concatenating n characters");
    
    // Test for null pointer protection
    // errno = 0;
    // safe_strncat(NULL, sizeof(dest), src, 3);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncat sets errno to ERANGE when destination pointer is null");
    // errno = 0;   
    // safe_strncat(dest, sizeof(dest), NULL, 3);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncat sets errno to ERANGE when source pointer is null");
    
    // Test for zero and too large destsz
    // errno = 0;
    // safe_strncat(dest, 0, src, 3);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncat sets errno to ERANGE when destsz is zero");
    // errno = 0;
    // safe_strncat(dest, RSIZE_MAX + 1, src, 3);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncat sets errno to ERANGE when destsz is greater than RSIZE_MAX");
    
    // Test for no null terminator in the first destsz bytes of dest
    // char str[20] = "This string is too long for the buffer";
    // errno = 0;   
    // safe_strncat(str, sizeof(str), src, 3);
    // TEST_ASSERT_EQ(errno, ERANGE, "safe_strncat sets errno to ERANGE when there is no null terminator in the first destsz bytes of dest");
}

static void test_safe_strcpy_no_overlap(void) {
    char dest[20];
    const char* src = "Hello, World!";
    safe_strcpy_no_overlap(dest, sizeof(dest), src);
    TEST_ASSERT_EQ(strcmp(dest, src), 0, "String is correctly copied to destination buffer without overlap");

    // Test for overlapping - aborts the test
    // char str[20] = "This String";
    // Attempt to copy "String" one position left (overwrite space)
    // errno_t err = safe_strcpy_no_overlap(str + 4, sizeof(str) - 4, str + 5);
    // printf("str after attempted overlapping copy: %s\n", str);
    // printf("errno after attempted overlapping copy: %d\n", err);
    // TEST_ASSERT_EQ(err, ERANGE, "safe_strcpy_no_overlap should fail with overlapping buffers");
}

static void test_safe_strcat_no_overlap(void) {
    char dest[20] = "Hello ";
    const char* src = "World";
    safe_strcat_no_overlap(dest, sizeof(dest), src);
    TEST_ASSERT_EQ(strcmp(dest, "Hello World"), 0, "String is correctly concatenated to destination buffer without overlap");

    // Test for overlapping - aborts the test
    // char str[20] = "This String";
    // Attempt to concatenate "String" one position left (overwrite space)
    // errno_t err = safe_strcat_no_overlap(str + 4, sizeof(str) - 4, str + 5);
    // printf("str after attempted overlapping concatenation: %s\n", str);
    // printf("errno after attempted overlapping concatenation: %d\n", err);
    // TEST_ASSERT_EQ(err, ERANGE, "safe_strcat_no_overlap should fail with overlapping buffers");
}

static void test_safe_strncpy_no_overlap(void) {
    char dest[20];
    const char* src = "Hello, World!";
    safe_strncpy_no_overlap(dest, sizeof(dest), src, 5);
    TEST_ASSERT_EQ(strncmp(dest, src, 5), 0, "First n characters are correctly copied to destination buffer without overlap");
    TEST_ASSERT_EQ(dest[5], '\0', "Destination buffer is null-terminated after copying n characters without overlap");

    // Test for overlapping - aborts the test
    // char str[20] = "This String";
    // Attempt to copy "String" one position left (overwrite space)
    // errno_t err = safe_strncpy_no_overlap(str + 4, sizeof(str) - 4, str + 5, 5);
    // printf("str after attempted overlapping copy: %s\n", str);
    // printf("errno after attempted overlapping copy: %d\n", err);
    // TEST_ASSERT_EQ(err, ERANGE, "safe_strncpy_no_overlap should fail with overlapping buffers");
}

static void test_safe_strncat_no_overlap(void) {
    char dest[20] = "Hello ";
    const char* src = "World";
    safe_strncat_no_overlap(dest, sizeof(dest), src, 3);
    TEST_ASSERT_EQ(strcmp(dest, "Hello Wor"), 0, "First n characters are correctly concatenated to destination buffer without overlap");
    TEST_ASSERT_EQ(dest[9], '\0', "Destination buffer is null-terminated after concatenating n characters without overlap");

    // Test for overlapping - aborts the test
    // char str[20] = "This String";
    // Attempt to concatenate "String" one position left (overwrite space)
    // errno_t err = safe_strncat_no_overlap(str + 4, sizeof(str) - 4, str + 5, 5);
    // printf("str after attempted overlapping concatenation: %s\n", str);
    // printf("errno after attempted overlapping concatenation: %d\n", err);
    // TEST_ASSERT_EQ(err, ERANGE, "safe_strncat_no_overlap should fail with overlapping buffers");
}

static void test_common_String_Concat(void) {
    char dest[20] = "Hello "; 
    const char* src = "World";
    TEST_ASSERT_EQ(common_String_Concat(dest, sizeof(dest), src), dest, "String is correctly concatenated to destination buffer");

    // Test for null pointer protection
    // errno = 0;
    // TEST_ASSERT_EQ(common_String_Concat(NULL, sizeof(dest), src), NULL, "common_String_Concat returns NULL when destination pointer is null");
    // errno = 0;   
    // TEST_ASSERT_EQ(common_String_Concat(dest, sizeof(dest), NULL), NULL, "common_String_Concat returns NULL when source pointer is null");

    // Test for zero and too large destsz
    // errno = 0;;
    // TEST_ASSERT_EQ(common_String_Concat(dest, 0, src), NULL, "common_String_Concat returns NULL when destsz is zero");
    // errno = 0;
    // TEST_ASSERT_EQ(common_String_Concat(dest, RSIZE_MAX + 1, src), NULL, "common_String_Concat returns NULL when destsz is greater than RSIZE_MAX");
    
    // Test for no null terminator in the first destsz bytes of dest
    // char str[20] = "This string is too long for the buffer";
    // errno = 0;   
    // TEST_ASSERT_EQ(common_String_Concat(str, sizeof(str), src), NULL, "common_String_Concat returns NULL when there is no null terminator in the first destsz bytes of dest");
}

static void test_common_String_Concat_Len(void) {
    char dest[20] = "Hello ";
    const char* src = "World";
    char* result = common_String_Concat_Len(dest, sizeof(dest), src, 3);
    TEST_ASSERT_EQ(result, dest, "First n characters are correctly concatenated to destination buffer");
    TEST_ASSERT_EQ(dest[9], '\0', "Destination buffer is null-terminated after concatenating n characters");

    // Test for null pointer protection
    // errno = 0;
    //result = common_String_Concat_Len(NULL, sizeof(dest), src, 3);
    // TEST_ASSERT_EQ(result, NULL, "common_String_Concat_Len returns NULL when destination pointer is null");
    // errno = 0;   
    //result = common_String_Concat_Len(dest, sizeof(dest), NULL, 3);
    // TEST_ASSERT_EQ(result, NULL, "common_String_Concat_Len returns NULL when source pointer is null");
    
    // Test for zero and too large destsz
    // errno = 0;
    //result = common_String_Concat_Len(dest, 0, src, 3);
    // TEST_ASSERT_EQ(result, NULL, "common_String_Concat_Len returns NULL when destsz is zero");
    // errno = 0;
    //result = common_String_Concat_Len(dest, RSIZE_MAX + 1, src, 3);
    // TEST_ASSERT_EQ(result, NULL, "common_String_Concat_Len returns NULL when destsz is greater than RSIZE_MAX");
    
    // Test for no null terminator in the first destsz bytes of dest
    // char str[20] = "This string is too long for the buffer";
    // errno = 0;   
    //result = common_String_Concat_Len(str, sizeof(str), src, 3);
    // TEST_ASSERT_EQ(result, NULL, "common_String_Concat_Len returns NULL when there is no null terminator in the first destsz bytes of dest");
}

static void test_safe_strtok(void) {
    char str[] = "one,two,three";
    rsize_t max = sizeof(str);
    char* saveptr;

    char* token;

    token = safe_strtok(str, &max, ",", &saveptr);
    TEST_ASSERT_NEQ(token, NULL, "First token should not be NULL");
    TEST_ASSERT_EQ(strcmp(token, "one"), 0, "First token should be 'one'");

    token = safe_strtok(NULL, &max, ",", &saveptr);
    TEST_ASSERT_NEQ(token, NULL, "Second token should not be NULL");
    TEST_ASSERT_EQ(strcmp(token, "two"), 0, "Second token should be 'two'");

    token = safe_strtok(NULL, &max, ",", &saveptr);
    TEST_ASSERT_NEQ(token, NULL, "Third token should not be NULL");
    TEST_ASSERT_EQ(strcmp(token, "three"), 0, "Third token should be 'three'");

    token = safe_strtok(NULL, &max, ",", &saveptr);
    TEST_ASSERT_EQ(token, NULL, "No more tokens expected");
}

static void test_common_String_Token(void) {
    char str[] = "one,two,three";
    rsize_t max = sizeof(str);
    char* saveptr;

    char* token;

    token = common_String_Token(str, &max, ",", &saveptr);
    TEST_ASSERT_NEQ(token, NULL, "First token should not be NULL");
    TEST_ASSERT_EQ(strcmp(token, "one"), 0, "First token should be 'one'");

    token = common_String_Token(NULL, &max, ",", &saveptr);
    TEST_ASSERT_NEQ(token, NULL, "Second token should not be NULL");
    TEST_ASSERT_EQ(strcmp(token, "two"), 0, "Second token should be 'two'");

    token = common_String_Token(NULL, &max, ",", &saveptr);
    TEST_ASSERT_NEQ(token, NULL, "Third token should not be NULL");
    TEST_ASSERT_EQ(strcmp(token, "three"), 0, "Third token should be 'three'");

    token = common_String_Token(NULL, &max, ",", &saveptr);
    TEST_ASSERT_EQ(token, NULL, "No more tokens expected");
}

static void test_safe_String_Token(void) {
    char str[] = "one,two,three";
    rsize_t max = sizeof(str);
    char* saveptr;

    char* token;

    token = safe_String_Token(str, &max, ",", &saveptr);
    TEST_ASSERT_NEQ(token, NULL, "First token should not be NULL");
    TEST_ASSERT_EQ(strcmp(token, "one"), 0, "First token should be 'one'");

    token = safe_String_Token(NULL, &max, ",", &saveptr);
    TEST_ASSERT_NEQ(token, NULL, "Second token should not be NULL");
    TEST_ASSERT_EQ(strcmp(token, "two"), 0, "Second token should be 'two'");

    token = safe_String_Token(NULL, &max, ",", &saveptr);
    TEST_ASSERT_NEQ(token, NULL, "Third token should not be NULL");
    TEST_ASSERT_EQ(strcmp(token, "three"), 0, "Third token should be 'three'");

    token = safe_String_Token(NULL, &max, ",", &saveptr);
    TEST_ASSERT_EQ(token, NULL, "No more tokens expected");
}

static void test_strndup(void) {
    const char* str = "Hello, World!";
    char* dup = strndup(str, 5);
    TEST_ASSERT_NEQ(dup, NULL, "strndup should return a non-NULL pointer");
    TEST_ASSERT_EQ(strncmp(dup, str, 5), 0, "First n characters should be correctly duplicated");
    free(dup);
}

static void test_safe_strdup(void) {
    const char* str = "Hello, World!";
    char* dup;

    errno_t err = safe_strdup(&dup, str);

    TEST_ASSERT_EQ(err, 0, "Duplication should succeed");
    TEST_ASSERT_EQ(strcmp(dup, str), 0, "String should be correctly duplicated");

    free(dup);
}

static void test_safe_strndup(void) {
    const char* str = "Hello, World!";
    char* dup;

    errno_t err = safe_strndup(&dup, str, 5);

    TEST_ASSERT_EQ(err, 0, "Duplication should succeed");
    TEST_ASSERT_EQ(strncmp(dup, str, 5), 0, "First n characters should be correctly duplicated");

    free(dup);
}

static void test_byte_Swap_String(void) {
    char str[] = "Hello World";
    byte_Swap_String(str);
    TEST_ASSERT_EQ(strcmp(str, "eHll ooWlrd"), 0, "Swap bytes in the string");
}

static void test_byte_Swap_String_Len(void) {
    char str[] = "Hello World";
    byte_Swap_String_Len(str, 3);
    TEST_ASSERT_EQ(strcmp(str, "eHllo World"), 0, "Swap bytes in the string up to the specified length");
}

static void test_remove_Whitespace_Left(void) {
    char str[] = "   Hello World";
    remove_Whitespace_Left(str);
    TEST_ASSERT_EQ(strcmp(str, "Hello World"), 0, "Leading whitespace should be removed");
}

static void test_remove_Trailing_Whitespace(void) {
    char str[] = "Hello World   ";
    remove_Trailing_Whitespace(str);
    TEST_ASSERT_EQ(strcmp(str, "Hello World"), 0, "Trailing whitespace should be removed");
    char str2[] = "Hello World";
    remove_Trailing_Whitespace(str2);
    TEST_ASSERT_EQ(strcmp(str2, "Hello World"), 0, "String without trailing whitespace should remain unchanged");
    char str3[] = "    Hello World";
    remove_Trailing_Whitespace(str3);
    TEST_ASSERT_EQ(strcmp(str3, "    Hello World"), 0, "String without leading whitespace should remain unchanged");
}

static void test_remove_Trailing_Whitespace_Len(void) {
    char str[] = "Hello World        ";

    // Test when string is NULL
    remove_Trailing_Whitespace_Len(NULL, 0);
    TEST_ASSERT_EQ(strcmp(str, "Hello World        "), 0, "String should remain unchanged when input is NULL for remove_Trailing_Whitespace_Len");

    remove_Trailing_Whitespace_Len(str, SIZE_OF_STACK_ARRAY(str));
    TEST_ASSERT_EQ(strcmp(str, "Hello World"), 0, "Trailing whitespace should be removed using remove_Trailing_Whitespace_Len");
}

static void test_remove_Leading_Whitespace(void) {
    char str[] = "      Hello World";
    remove_Leading_Whitespace(str);
    TEST_ASSERT_EQ(strcmp(str, "Hello World"), 0, "Leading whitespace should be removed");
    char str2[] = "Hello World   ";
    remove_Leading_Whitespace(str2);
    TEST_ASSERT_EQ(strcmp(str2, "Hello World   "), 0, "String with leading whitespaces should remain unchanged");
    char str3[] = "Hello World";
    remove_Leading_Whitespace(str3);
    TEST_ASSERT_EQ(strcmp(str3, "Hello World"), 0, "String without leading whitespace should remain unchanged");
}

static void test_remove_Leading_Whitespace_Len(void) {
    char str[] = "         Hello World";

    // Test when string is NULL
    remove_Leading_Whitespace_Len(NULL, 0);
    TEST_ASSERT_EQ(strcmp(str, "         Hello World"), 0, "String should remain unchanged when input is NULL for remove_Leading_Whitespace_Len");

    remove_Leading_Whitespace_Len(str, SIZE_OF_STACK_ARRAY(str));
    TEST_ASSERT_EQ(strcmp(str, "Hello World"), 0, "Leading whitespace should be removed using remove_Leading_Whitespace_Len");
}

static void test_remove_Leading_And_Trailing_Whitespace(void) {
    char str[] = "   Hello World   ";
    remove_Leading_And_Trailing_Whitespace(str);
    TEST_ASSERT_EQ(strcmp(str, "Hello World"), 0, "Leading and trailing whitespace should be removed");
}

static void test_remove_Leading_And_Trailing_Whitespace_Len(void) {
    char str[] = "   Hello World        ";

    // Test when string is NULL
    remove_Leading_And_Trailing_Whitespace_Len(NULL, 0);
    TEST_ASSERT_EQ(strcmp(str, "   Hello World        "), 0, "String should remain unchanged when input is NULL for remove_Leading_And_Trailing_Whitespace_Len");

    remove_Leading_And_Trailing_Whitespace_Len(str, SIZE_OF_STACK_ARRAY(str));
    TEST_ASSERT_EQ(strcmp(str, "Hello World"), 0, "Leading and trailing whitespace should be removed using remove_Leading_And_Trailing_Whitespace_Len");

    char str2[] = "        ";
    remove_Leading_And_Trailing_Whitespace_Len(str2, SIZE_OF_STACK_ARRAY(str2));
    TEST_ASSERT_EQ(strcmp(str2, ""), 0, "String with only whitespaces should become an empty string after removing leading and trailing whitespace using remove_Leading_And_Trailing_Whitespace_Len");

    // Test when no change is needed
    char str3[] = "Hello World";
    remove_Leading_And_Trailing_Whitespace_Len(str3, SIZE_OF_STACK_ARRAY(str3));
    TEST_ASSERT_EQ(strcmp(str3, "Hello World"), 0, "String without leading or trailing whitespace should remain unchanged when using remove_Leading_And_Trailing_Whitespace_Len");
}

static void test_remove_Leading_And_Trailing_Control_Char(void) {
    char str[] = "\n\tHello\n\t";
    remove_Leading_And_Trailing_Control_Char(str);
    TEST_ASSERT_EQ(strcmp(str, "Hello"), 0, "Leading and trailing control characters removed");
}

static void test_remove_Leading_And_Trailing_Control_Char_Len(void) {
    char str[] = "\n\tHello\n\t";
    remove_Leading_And_Trailing_Control_Char_Len(str, SIZE_OF_STACK_ARRAY(str));
    TEST_ASSERT_EQ(strcmp(str, "Hello"), 0, "Leading and trailing control characters removed using remove_Leading_And_Trailing_Control_Char_Len");
}

static void test_convert_String_To_Upper_Case(void) {
    char str[] = "HEllO World!";

    // Test when string is NULL
    convert_String_To_Upper_Case(NULL);
    TEST_ASSERT_EQ(strcmp(str, "HEllO World!"), 0, "String should remain unchanged when input is NULL for convert_String_To_Upper_Case");

    convert_String_To_Upper_Case(str);
    TEST_ASSERT_EQ(strcmp(str, "HELLO WORLD!"), 0, "String should be converted to upper case");
}

static void test_convert_String_To_Upper_Case_Len(void) {
    char str[] = "HEllO World!";

    // Test when string is NULL
    convert_String_To_Upper_Case_Len(NULL, 0);
    TEST_ASSERT_EQ(strcmp(str, "HEllO World!"), 0, "String should remain unchanged when input is NULL for convert_String_To_Upper_Case_Len");

    convert_String_To_Upper_Case_Len(str, SIZE_OF_STACK_ARRAY(str));
    TEST_ASSERT_EQ(strcmp(str, "HELLO WORLD!"), 0, "String should be converted to upper case using convert_String_To_Upper_Case_Len");
}

static void test_convert_String_To_Lower_Case(void) {
    char str[] = "HEllO World!";

    // Test when string is NULL
    convert_String_To_Lower_Case(NULL);
    TEST_ASSERT_EQ(strcmp(str, "HEllO World!"), 0, "String should remain unchanged when input is NULL for convert_String_To_Lower_Case");

    convert_String_To_Lower_Case(str);
    TEST_ASSERT_EQ(strcmp(str, "hello world!"), 0, "String should be converted to lower case");
}

static void test_convert_String_To_Lower_Case_Len(void) {
    char str[] = "HEllO World!";

    // Test when string is NULL
    convert_String_To_Lower_Case_Len(NULL, 0);
    TEST_ASSERT_EQ(strcmp(str, "HEllO World!"), 0, "String should remain unchanged when input is NULL for convert_String_To_Lower_Case_Len");

    convert_String_To_Lower_Case_Len(str, SIZE_OF_STACK_ARRAY(str));
    TEST_ASSERT_EQ(strcmp(str, "hello world!"), 0, "String should be converted to lower case using convert_String_To_Lower_Case_Len");
}

static void test_convert_String_To_Inverse_Case(void) {
    char str[] = "hello world!";

    // Test when string is NULL
    convert_String_To_Inverse_Case(NULL);
    TEST_ASSERT_EQ(strcmp(str, "hello world!"), 0, "String should remain unchanged when input is NULL for convert_String_To_Inverse_Case");

    convert_String_To_Inverse_Case(str);
    TEST_ASSERT_EQ(strcmp(str, "HELLO WORLD!"), 0, "Lower case string should be converted to upper case");

    char str2[] = "HELLO WORLD!";
    convert_String_To_Inverse_Case(str2);
    TEST_ASSERT_EQ(strcmp(str2, "hello world!"), 0, "Upper case string should be converted to lower case");
}

static void test_convert_String_To_Inverse_Case_Len(void) {
    char str[] = "hello world!";

    // Test when string is NULL
    convert_String_To_Inverse_Case_Len(NULL, 0);
    TEST_ASSERT_EQ(strcmp(str, "hello world!"), 0, "String should remain unchanged when input is NULL for convert_String_To_Inverse_Case_Len");

    convert_String_To_Inverse_Case_Len(str, SIZE_OF_STACK_ARRAY(str));
    TEST_ASSERT_EQ(strcmp(str, "HELLO WORLD!"), 0, "Lower case string should be converted to upper case using convert_String_To_Inverse_Case_Len");

    char str2[] = "HELLO WORLD!";
    convert_String_To_Inverse_Case_Len(str2, SIZE_OF_STACK_ARRAY(str2));
    TEST_ASSERT_EQ(strcmp(str2, "hello world!"), 0, "Upper case string should be converted to lower case using convert_String_To_Inverse_Case_Len");
}

static void test_find_last_occurrence_in_string(void) {
    const char* str = "This test string contains the word test multiple times to test the function.";
    const char* substr = "test";
    const char* substr2 = "the";

    // Test if str is NULL
    size_t offset = find_last_occurrence_in_string(NULL, substr);
    TEST_ASSERT_EQ(offset, SIZE_MAX, "Should return SIZE_MAX when input string is NULL for find_last_occurrence_in_string");

    // Test if substr is NULL
    size_t offset2 = find_last_occurrence_in_string(str, NULL);
    TEST_ASSERT_EQ(offset2, SIZE_MAX, "Should return SIZE_MAX when substring is NULL for find_last_occurrence_in_string");

    size_t offset3 = find_last_occurrence_in_string(str, substr);
    size_t offset4 = find_last_occurrence_in_string(str, substr2);
    TEST_ASSERT_EQ(offset3, 18, "Last occurrence of substring should be at offset 18 from end of string");
    TEST_ASSERT_EQ(offset4, 13, "Last occurrence of substring 'the' should be at offset 13 from end of string");

    // Test for substring not found
    const char* substr3 = "notfound";
    size_t offset5 = find_last_occurrence_in_string(str, substr3);
    TEST_ASSERT_EQ(offset5, strlen(str), "Should return length of string when substring is not found");

    // Test for substr = 0 length string
    const char* substr4 = "";
    size_t offset6 = find_last_occurrence_in_string(str, substr4);
    TEST_ASSERT_EQ(offset6, SIZE_MAX, "Should return SIZE_MAX when substring is an empty string");
}

static void test_find_first_occurrence_in_string(void) {
    const char* str = "This test string contains the word test multiple times to test the function.";
    const char* substr = "test";

    // Test if str is NULL
    size_t offset = find_first_occurrence_in_string(NULL, substr);
    TEST_ASSERT_EQ(offset, SIZE_MAX, "Should return SIZE_MAX when input string is NULL for find_first_occurrence_in_string");

    // Test if substr is NULL
    size_t offset2 = find_first_occurrence_in_string(str, NULL);
    TEST_ASSERT_EQ(offset2, SIZE_MAX, "Should return SIZE_MAX when substring is NULL for find_first_occurrence_in_string");

    size_t offset3 = find_first_occurrence_in_string(str, substr);
    TEST_ASSERT_EQ(offset3, 5, "First occurrence of substring should be at offset 5 from start of string");

    // Test for substring not found
    const char* substr2 = "notfound";
    size_t offset4 = find_first_occurrence_in_string(str, substr2);
    TEST_ASSERT_EQ(offset4, SIZE_MAX , "Should return SIZE_MAX when substring is not found");
}

static void test_wildcard_match(void) {
    TEST_ASSERT(!wildcard_match(NULL, "string"), "Null pattern should not match any string");
    TEST_ASSERT(wildcard_match("he*o", "hello"), "Pattern with wildcard should match string");
    TEST_ASSERT(wildcard_match("h?llo", "hello"), "Pattern with single character wildcard should match string");
    TEST_ASSERT(wildcard_match("*", "anything"), "Wildcard pattern should match any string");
    TEST_ASSERT(wildcard_match("h*o", "ho"), "Pattern with wildcard should match string with zero characters in place of wildcard");
    TEST_ASSERT(!wildcard_match("he*o", "hi"), "Pattern with wildcard should not match non-matching string");
    TEST_ASSERT(!wildcard_match("h*o", "hold"), "Pattern with wildcard should not match string that does not fit the pattern");
    TEST_ASSERT(!wildcard_match("h*o", "Ho"), "Case insensitive matching returns false");
}

static void test_wildcard_Match(void) {
    TEST_ASSERT(wildcard_Match("he*o", "hello"), "Pattern with wildcard should match string");
    TEST_ASSERT(wildcard_Match("h?llo", "hello"), "Pattern with single character wildcard should match string");
    TEST_ASSERT(wildcard_Match("*", "anything"), "Wildcard pattern should match any string");
    TEST_ASSERT(wildcard_Match("h*o", "ho"), "Pattern with wildcard should match string with zero characters in place of wildcard");
    TEST_ASSERT(!wildcard_Match("he*o", "hi"), "Pattern with wildcard should not match non-matching string");
    TEST_ASSERT(!wildcard_Match("h*o", "hold"), "Pattern with wildcard should not match string that does not fit the pattern");
    TEST_ASSERT(!wildcard_Match("h*o", "Ho"), "Case insensitive matching returns false");
}

static void test_wildcard_case_match(void) {
    TEST_ASSERT(wildcard_case_match("he*o", "hello"), "Pattern with wildcard should match string");
    TEST_ASSERT(wildcard_case_match("h?llo", "hello"), "Pattern with single character wildcard should match string");
    TEST_ASSERT(wildcard_case_match("*", "anything"), "Wildcard pattern should match any string");
    TEST_ASSERT(wildcard_case_match("h*o", "ho"), "Pattern with wildcard should match string with zero characters in place of wildcard");
    TEST_ASSERT(wildcard_case_match("h*o", "Ho"), "Case insensitive matching returns true");
    TEST_ASSERT(!wildcard_case_match("he*o", "hi"), "Pattern with wildcard should not match non-matching string");
    TEST_ASSERT(!wildcard_case_match("h*o", "hold"), "Pattern with wildcard should not match string that does not fit the pattern");
}

static void test_string_version_compare(void) {
    TEST_ASSERT(string_version_compare("1.0.0", "1.0.0") == 0, "Identical versions should compare equal");
    TEST_ASSERT(string_version_compare("1.0.1", "1.0.0") > 0, "Version 1.0.1 should be greater than 1.0.0");
    TEST_ASSERT(string_version_compare("1.0.0", "1.0.1") < 0, "Version 1.0.0 should be less than 1.0.1");
    TEST_ASSERT(string_version_compare("2.0", "10.0") < 0, "Version 2.0 should be less than 10.0");
    TEST_ASSERT(string_version_compare("file9.txt", "file10.txt") < 0, "Version file9.txt should be less than file10.txt");
    TEST_ASSERT(string_version_compare("file01.txt", "file1.txt") < 0, "Version file01.txt should be less than file1.txt");
    TEST_ASSERT(string_version_compare("abc", "abd") < 0, "Version abc should be less than abd");
    TEST_ASSERT(string_version_compare("v123", "v1234") < 0, "Longer numeric segment should win");
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
    test_safe_strcpy();
    test_safe_strmove();
    test_safe_strncpy();
    test_safe_strnmove();
    test_safe_strcat();
    test_safe_strncat();
    test_safe_strcpy_no_overlap();
    test_safe_strcat_no_overlap();
    test_safe_strncpy_no_overlap();
    test_safe_strncat_no_overlap();
    test_common_String_Concat();
    test_common_String_Concat_Len();
    test_safe_strtok();
    test_safe_String_Token();
    test_common_String_Token();
    test_strndup();
    test_safe_strdup();
    test_safe_strndup();
    test_byte_Swap_String();
    test_byte_Swap_String_Len();
    test_remove_Whitespace_Left();
    test_remove_Trailing_Whitespace();
    test_remove_Trailing_Whitespace_Len();
    test_remove_Leading_Whitespace();
    test_remove_Leading_Whitespace_Len();
    test_remove_Leading_And_Trailing_Whitespace();
    test_remove_Leading_And_Trailing_Whitespace_Len();
    test_remove_Leading_And_Trailing_Control_Char();
    test_remove_Leading_And_Trailing_Control_Char_Len();
    test_convert_String_To_Upper_Case();
    test_convert_String_To_Upper_Case_Len();
    test_convert_String_To_Lower_Case();
    test_convert_String_To_Lower_Case_Len();
    test_convert_String_To_Inverse_Case();
    test_convert_String_To_Inverse_Case_Len();
    test_find_last_occurrence_in_string();
    test_find_first_occurrence_in_string();
    test_wildcard_match();
    test_wildcard_Match();
    test_wildcard_case_match();
    test_string_version_compare();
}
