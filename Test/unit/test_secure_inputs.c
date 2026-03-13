#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/io_utils.h"


int main(void)
{
    char *input = NULL;
    size_t len = 0;

    /* Test function from io_utils.h */
    printf("Testing function get_Secure_User_Input\n");
    
    eReturnValues ret = get_Secure_User_Input("Enter password:", &input, &len);
    
    if (!input)
    {
        TEST_ASSERT(1, "get_Secure_User_Input failed when input is NULL");
    }
    input[strcspn(input, "\n")] = '\0';

    TEST_ASSERT(ret == SUCCESS, "get_Secure_User_Input succeeded");
    TEST_ASSERT(input != NULL, "Input is not NULL");
    TEST_ASSERT(len > 0, "Input length is greater than 0");
    TEST_ASSERT(strcmp(input, "mypassword") == 0, "Input matches expected value");
    printf("Input: %s\n", input);
    free(input);

    return 0;
}