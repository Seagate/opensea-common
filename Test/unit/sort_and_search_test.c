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

static void test_safe_qsort(void) {
    int arr[] = {5, 2, 9, 1, 5, 6};
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);
    errno_t result = safe_qsort(arr, arr_size, sizeof(arr[0]), compare_ints);
    TEST_ASSERT(result == 0, "safe_qsort correctly sorts the array");
}

void run_sort_and_search_tests(void) {
    test_safe_qsort();
}