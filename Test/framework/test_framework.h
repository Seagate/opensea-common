#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include<stdio.h>
#include<string.h>

extern int total_tests;
extern int failed_tests;

void test_record(int condition,const char* msg, const char *file, int line);
void print_test_summary(void);

#define TEST_ASSERT(cond, msg) \
    do { \
        total_tests++; \
        if (!(cond)) { \
            failed_tests++; \
        } else { \
        } \
        test_record((cond), (msg), __FILE__, __LINE__); \
    } while(0)

#define TEST_ASSERT_EQ(a, b, msg) TEST_ASSERT((a) == (b), (msg))
#define TEST_ASSERT_STR_EQ(a, b, msg) TEST_ASSERT(strcmp((a), (b)) == 0, (msg))

#endif // TEST_FRAMEWORK_H