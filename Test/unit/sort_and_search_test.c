#include"../framework/test_framework.h"
#include"../../include/sort_and_search.h"
#include"../testConstants.h"
#include <string.h>
#include <ctype.h>


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
    errno_t result = safe_qsort_context(arr, arr_size, sizeof(arr[0]), compare_ints_ctx, &ctx);
    TEST_ASSERT(result == 0, "safe_qsort_context correctly sorts the array in descending order");
    printf("Sorted array with context: ");
    for (size_t i = 0; i < arr_size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

static void test_safe_bsearch(void) {
    int arr[] = {1, 2, 5, 5, 6, 9};
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);
    int key = 5;
    int* found = (int*)safe_bsearch(&key, arr, arr_size, sizeof(arr[0]), compare_ints);
    TEST_ASSERT(found != NULL && *found == key, "safe_bsearch finds the key in the array");

    // Test searching for a non-existent key
    key = 10;
    found = (int*)safe_bsearch(&key, arr, arr_size, sizeof(arr[0]), compare_ints);
    TEST_ASSERT(found == NULL, "safe_bsearch returns NULL for a non-existent key");
}

typedef struct {
    int multiplier;
} search_ctx;

int compare_with_context(const void* a, const void* b, void* ctx)
{
    int key = *(const int*)a;
    int element = *(const int*)b;

    search_ctx* context = (search_ctx*)ctx;

    key *= context->multiplier;

    if (key < element) return -1;
    if (key > element) return 1;
    return 0;
}

static void test_safe_bsearch_context(void) {
    int arr[] = {1, 2, 5, 5, 6, 9, 10};
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);
    int key = 5;
    search_ctx ctx = {2};
    int* found = (int*)safe_bsearch_context(&key, arr, arr_size, sizeof(arr[0]), compare_with_context, &ctx);
    TEST_ASSERT(found != NULL && *found == 2*key, "safe_bsearch_context finds the key in the array with context");

    // Test searching for a non-existent key
    key = 11;
    found = (int*)safe_bsearch_context(&key, arr, arr_size, sizeof(arr[0]), compare_with_context, &ctx);
    TEST_ASSERT(found == NULL, "safe_bsearch_context returns NULL for a non-existent key with context");
}

static void test_safe_lsearch(void) {
    int arr[10] = {1, 2, 3, 4, 5};
    size_t nelp = 5;
    int key = 3;
    int* found = (int*)safe_lsearch(&key, arr, &nelp, sizeof(arr[0]), compare_ints);
    TEST_ASSERT(found != NULL && *found == key, "safe_lsearch finds the key in the array");

    // Test searching for a non-existent key
    key = 10;
    found = (int*)safe_lsearch(&key, arr, &nelp, sizeof(arr[0]), compare_ints);
    TEST_ASSERT(found != NULL && *found == key, "safe_lsearch inserts the non-existent key at the end of the array");
    TEST_ASSERT(nelp == 6, "safe_lsearch increments the number of elements when inserting a new key");
    printf("Array after safe_lsearch: ");
    for (size_t i = 0; i < nelp; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

typedef struct {
    int case_sensitive;
} StringContext;

int compare_strings_ctx(const void* a, const void* b, void* ctx)
{
    const char* key = *(const char**)a;
    const char* element = *(const char**)b;

    StringContext* context = (StringContext*)ctx;

    if (context->case_sensitive)
        return strcmp(key, element);

    // case-insensitive comparison 
    while (*key && *element) {
        char c1 = tolower(*key);
        char c2 = tolower(*element);

        if (c1 != c2)
            return c1 - c2;

        key++;
        element++;
    }

    return *key - *element;
}

static void test_safe_lsearch_context(void) {
    char* arr[10] = {"apple", "banana", "cherry"};
    size_t nelp = 3;
    char* key = "Banana";
    StringContext ctx = { .case_sensitive = 0 };
    char** found = (char**)safe_lsearch_context(&key, arr, NULL, sizeof(arr[0]), compare_strings_ctx, &ctx);
    TEST_ASSERT(found != NULL && strcmp(*found, "banana") == 0, "safe_lsearch_context finds the key in the array with context");

    // Test searching for a non-existent key
    key = "date";
    found = (char**)safe_lsearch_context(&key, arr, &nelp, sizeof(arr[0]), compare_strings_ctx, &ctx);
    TEST_ASSERT(found != NULL && strcmp(*found, "date") == 0, "safe_lsearch_context inserts the non-existent key at the end of the array with context");
    TEST_ASSERT(nelp == 4, "safe_lsearch_context increments the number of elements when inserting a new key with context");
    printf("Array after safe_lsearch_context: ");
    for (size_t i = 0; i < nelp; i++) {
        printf("%s ", arr[i]);
    }
    printf("\n");
}

static void test_safe_lfind(void) {
    int arr[] = {1, 2, 3, 4, 5};
    size_t nelp = sizeof(arr) / sizeof(arr[0]);
    int key = 3;
    int* found = (int*)safe_lfind(&key, arr, &nelp, sizeof(arr[0]), compare_ints);
    TEST_ASSERT(found != NULL && *found == key, "safe_lfind finds the key in the array");

    // Test searching for a non-existent key
    key = 10;
    found = (int*)safe_lfind(&key, arr, &nelp, sizeof(arr[0]), compare_ints);
    TEST_ASSERT(found == NULL, "safe_lfind returns NULL for a non-existent key");
}

void run_sort_and_search_tests(void) {
    test_safe_qsort();
    test_safe_qsort_context();
    test_safe_bsearch();
    test_safe_bsearch_context();
    test_safe_lsearch();
    test_safe_lsearch_context();
    test_safe_lfind();
}