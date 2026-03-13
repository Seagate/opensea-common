#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/io_utils.h"

int main(void)
{
    char *input = NULL;
    size_t len = 0;

    printf("Testing function get_Secure_User_Input\n");

    eReturnValues ret = get_Secure_User_Input("Enter password:", &input, &len);

    TEST_ASSERT(ret == SUCCESS ? 0 : 1, "get_Secure_User_Input succeeded");
    TEST_ASSERT(input != NULL ? 0 : 1, "Input is not NULL");

    if (input == NULL)
        return 1;

    input[strcspn(input, "\n")] = '\0';

    TEST_ASSERT(len > 0 ? 0 : 1, "Input length is greater than 0");
    TEST_ASSERT(strcmp(input, "mypassword") == 0 ? 0 : 1,
                "Input matches expected value");

    printf("Input: %s\n", input);

    free(input);

    return 0;
}