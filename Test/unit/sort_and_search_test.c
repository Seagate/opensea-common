#include"../framework/test_framework.h"
#include"../../include/sort_and_search.h"
#include"../testConstants.h"

static int compare_ints(const void* a, const void* b)
{
    int x = *(const int*)a;
    int y = *(const int*)b;

    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

static int compare_chars(const void* a, const void* b)
{
    char x = *(const char*)a;
    char y = *(const char*)b;

    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

static void test_safe_qsort(void) {
    int arr[] = {5, 2, 9, 1, 5, 6};
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);
    errno_t result = safe_qsort(arr, arr_size, sizeof(arr[0]), compare_ints);
    TEST_ASSERT(result == 0, "safe_qsort correctly sorts the array");
    printf("Sorted array: ");
    for (size_t i = 0; i < arr_size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    char char_arr[] = {'z', 'a', 'q', 'b', 'm'};
    size_t char_arr_size = sizeof(char_arr) / sizeof(char_arr[0]);
    result = safe_qsort(char_arr, char_arr_size, sizeof(char_arr[0]), compare_chars);
    TEST_ASSERT(result == 0, "safe_qsort correctly sorts the character array");
    printf("Sorted character array: ");
    for (size_t i = 0; i < char_arr_size; i++) {
        printf("%c ", char_arr[i]);
    }
    printf("\n");
}

typedef struct {
    int descending ;
} sort_ctx;

static int compare_ints_ctx(const void* a, const void* b, void* context)
{
    int x = *(const int*)a;
    int y = *(const int*)b;

    sort_ctx* ctx = (sort_ctx*)context;

    if (ctx->descending)
    {
        if (x < y) return 1;
        if (x > y) return -1;
        return 0;
    }
    else
    {
        if (x < y) return -1;
        if (x > y) return 1;
        return 0;
    }
}

static int compare_ints_no_ctx(const void* a, const void* b, void* context)
{
    (void)context;  // context not used

    int x = *(const int*)a;
    int y = *(const int*)b;

    if (x < y) return 1;
    if (x > y) return -1;
    return 0;
}

static void test_safe_qsort_context(void) {
    int arr[] = {5, 2, 9, 1, 5, 6};
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);
    sort_ctx ctx = { .descending = 1 };
    errno_t result = safe_qsort_context(arr, arr_size, 0, compare_ints_ctx, &ctx);
    TEST_ASSERT(result == 0, "safe_qsort_context correctly sorts the array in descending order");
    printf("Sorted array with context: ");
    for (size_t i = 0; i < arr_size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void run_sort_and_search_tests(void) {
    test_safe_qsort();
    test_safe_qsort_context();
}