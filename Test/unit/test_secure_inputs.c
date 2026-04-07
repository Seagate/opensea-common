#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"../framework/test_framework.h"
#include"../../include/io_utils.h"
#include"../testConstants.h"

static void test_get_Secure_User_Input(void)
{
    char *input = NULL;
    size_t len = 0;

    printf("Testing function get_Secure_User_Input\n");

    eReturnValues ret = get_Secure_User_Input("Enter password:", &input, &len);

    TEST_ASSERT(ret == 0, "get_Secure_User_Input succeeded");
    TEST_ASSERT(input != NULL, "Input is not NULL");

    input[strcspn(input, "\n")] = '\0';

    TEST_ASSERT(len > 0, "Input length is greater than 0");
    TEST_ASSERT(strcmp(input, "mypassword") == 0, "Input matches expected value");

    free(input);
}

void run_test_secure_inputs(void) {
    test_get_Secure_User_Input();
}