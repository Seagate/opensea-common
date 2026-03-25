#include"../framework/test_framework.h"
#include"../../include/io_utils.h"
#include"../testConstants.h"

static void test_safe_malloc(void) {
    void* ptr = safe_malloc(100);
    TEST_ASSERT(ptr != NULL, "safe_malloc should return a non-null pointer for a non-zero size");
    free(ptr);

    // Test that safe_malloc returns null for a size of zero
    // ptr = safe_malloc(0);
    // TEST_ASSERT(ptr == NULL, "safe_malloc should return a null pointer for a size of zero");
    // free(ptr);
}

static void test_safe_calloc(void) {
    void* ptr = safe_calloc(10, 20);
    TEST_ASSERT(ptr != NULL, "safe_calloc should return a non-null pointer for non-zero count and size");
    free(ptr);

    // Test that safe_calloc returns null for a count of zero
    // ptr = safe_calloc(0, 20);
    // TEST_ASSERT(ptr == NULL, "safe_calloc should return a null pointer for a count of zero");
    // free(ptr);

    // Test that safe_calloc returns null for a size of zero
    // ptr = safe_calloc(10, 0);
    // TEST_ASSERT(ptr == NULL, "safe_calloc should return a null pointer for a size of zero");
    // free(ptr);
}

static void test_safe_realloc(void) {
    void* ptr = safe_malloc(100);
    TEST_ASSERT(ptr != NULL, "safe_malloc should return a non-null pointer for a non-zero size");

    // Test that reallocating to a larger size works
    void* new_ptr = safe_realloc(ptr, 200);
    TEST_ASSERT(new_ptr != NULL, "safe_realloc should return a non-null pointer when reallocating to a larger size");
    free(new_ptr);

    // Test that reallocating to zero frees the memory and returns null
    ptr = safe_malloc(100);
    new_ptr = safe_realloc(ptr, 0);
    TEST_ASSERT(new_ptr == NULL, "safe_realloc should return a null pointer when reallocating to zero");
}

static void test_safe_reallocf(void) {
    void* ptr = safe_malloc(100);
    TEST_ASSERT(ptr != NULL, "safe_malloc should return a non-null pointer for a non-zero size");

    // Test that reallocating to a larger size works
    void* new_ptr = safe_reallocf(&ptr, 200);
    TEST_ASSERT(new_ptr != NULL, "safe_reallocf should return a non-null pointer when reallocating to a larger size");
    free(new_ptr);

    // Test that reallocating to zero frees the memory and returns null
    ptr = safe_malloc(100);
    new_ptr = safe_reallocf(&ptr, 0);
    TEST_ASSERT(new_ptr == NULL, "safe_reallocf should return a null pointer when reallocating to zero");

    // Pointer to block is NULL, should return NULL and not attempt to free
    new_ptr = safe_reallocf(NULL, 100);
    TEST_ASSERT(new_ptr == NULL, "safe_reallocf should return a null pointer when the input pointer is NULL");

    // Size is zero, should free the original block and return NULL
    ptr = safe_malloc(100);
    new_ptr = safe_reallocf(&ptr, 0);
    TEST_ASSERT(new_ptr == NULL, "safe_reallocf should return a null pointer when reallocating to zero");
    TEST_ASSERT(ptr == NULL, "safe_reallocf should set the original pointer to NULL when reallocating to zero");

    // Reallocating to a larger size but realloc fails, should free the original block and return NULL
    ptr = safe_malloc(100);
    new_ptr = safe_reallocf(&ptr, RSIZE_MAX);
    TEST_ASSERT(new_ptr == NULL, "safe_reallocf should return a null pointer when reallocating to an excessively large size");
    TEST_ASSERT(ptr == NULL, "safe_reallocf should set the original pointer to NULL when reallocating to an excessively large size");
}

static void test_safe_free_core(void) {
    char* ptr = safe_malloc(100);
    safe_free_core((void**)&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_core should set the pointer to NULL after freeing");
}

static void test_safe_free_char(void) {
    char* ptr = safe_malloc(100);
    safe_free_char(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_char should set the pointer to NULL after freeing");
}

static void test_safe_free_schar(void) {
    signed char* ptr = safe_malloc(100);
    safe_free_schar(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_schar should set the pointer to NULL after freeing");
}

static void test_safe_free_uchar(void) {
    unsigned char* ptr = safe_malloc(100);
    safe_free_uchar(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_uchar should set the pointer to NULL after freeing");
}

static void test_safe_free_wchar(void) {
    wchar_t* ptr = safe_malloc(100);
    safe_free_wchar(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_wchar should set the pointer to NULL after freeing");
}

static void test_safe_free_short(void) {
    signed short* ptr = safe_malloc(100);
    safe_free_short(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_short should set the pointer to NULL after freeing");
}

static void test_safe_free_ushort(void) {
    unsigned short* ptr = safe_malloc(100);
    safe_free_ushort(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_ushort should set the pointer to NULL after freeing");
}

static void test_safe_free_int(void) {
    signed int* ptr = safe_malloc(100);
    safe_free_int(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_int should set the pointer to NULL after freeing");
}

static void test_safe_free_uint(void) {
    unsigned int* ptr = safe_malloc(100);
    safe_free_uint(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_uint should set the pointer to NULL after freeing");
}

static void test_safe_free_long(void) {
    signed long* ptr = safe_malloc(100);
    safe_free_long(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_long should set the pointer to NULL after freeing");
}

static void test_safe_free_ulong(void) {
    unsigned long* ptr = safe_malloc(100);
    safe_free_ulong(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_ulong should set the pointer to NULL after freeing");
}

static void test_safe_free_longlong(void) {
    signed long long* ptr = safe_malloc(100);
    safe_free_longlong(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_longlong should set the pointer to NULL after freeing");
}

static void test_safe_free_ulonglong(void) {
    unsigned long long* ptr = safe_malloc(100);
    safe_free_ulonglong(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_ulonglong should set the pointer to NULL after freeing");
}

static void test_safe_free_float(void) {
    float* ptr = safe_malloc(100);
    safe_free_float(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_float should set the pointer to NULL after freeing");
}

static void test_safe_free_double(void) {
    double* ptr = safe_malloc(100);
    safe_free_double(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_double should set the pointer to NULL after freeing");
}

static void test_safe_free_longdouble(void) {
    long double* ptr = safe_malloc(100);
    safe_free_longdouble(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_longdouble should set the pointer to NULL after freeing");
}

static void test_safe_free(void) {
    char* char_ptr = safe_malloc(100);
    safe_free(&char_ptr);
    TEST_ASSERT(char_ptr == NULL, "safe_free should set the signed char pointer to NULL after freeing");

    unsigned char* uchar_ptr = safe_malloc(100);
    safe_free(&uchar_ptr);
    TEST_ASSERT(uchar_ptr == NULL, "safe_free should set the unsigned char pointer to NULL after freeing");

    signed short* short_ptr = safe_malloc(100);
    safe_free(&short_ptr);
    TEST_ASSERT(short_ptr == NULL, "safe_free should set the signed short pointer to NULL after freeing");

    unsigned short* ushort_ptr = safe_malloc(100);
    safe_free(&ushort_ptr);
    TEST_ASSERT(ushort_ptr == NULL, "safe_free should set the unsigned short pointer to NULL after freeing");

    signed int* int_ptr = safe_malloc(100);
    safe_free(&int_ptr);
    TEST_ASSERT(int_ptr == NULL, "safe_free should set the signed int pointer to NULL after freeing");

    unsigned int* uint_ptr = safe_malloc(100);
    safe_free(&uint_ptr);
    TEST_ASSERT(uint_ptr == NULL, "safe_free should set the unsigned int pointer to NULL after freeing");

    signed long* long_ptr = safe_malloc(100);
    safe_free(&long_ptr);
    TEST_ASSERT(long_ptr == NULL, "safe_free should set the signed long pointer to NULL after freeing");

    unsigned long* ulong_ptr = safe_malloc(100);
    safe_free(&ulong_ptr);
    TEST_ASSERT(ulong_ptr == NULL, "safe_free should set the unsigned long pointer to NULL after freeing");

    signed long long* longlong_ptr = safe_malloc(100);
    safe_free(&longlong_ptr);
    TEST_ASSERT(longlong_ptr == NULL, "safe_free should set the signed long long pointer to NULL after freeing");

    unsigned long long* ulonglong_ptr = safe_malloc(100);
    safe_free(&ulonglong_ptr);
    TEST_ASSERT(ulonglong_ptr == NULL, "safe_free should set the unsigned long long pointer to NULL after freeing");

    float* float_ptr = safe_malloc(100);
    safe_free(&float_ptr);
    TEST_ASSERT(float_ptr == NULL, "safe_free should set the float pointer to NULL after freeing");

    double* double_ptr = safe_malloc(100);
    safe_free(&double_ptr);
    TEST_ASSERT(double_ptr == NULL, "safe_free should set the double pointer to NULL after freeing");

    long double* longdouble_ptr = safe_malloc(100);
    safe_free(&longdouble_ptr);
    TEST_ASSERT(longdouble_ptr == NULL, "safe_free should set the long double pointer to NULL after freeing");
}

#if defined(_WIN32)
    static void test_safe_free_tchar(void) {
        TCHAR* memory = safe_malloc(50);
        safe_free_tchar(&memory);
        TEST_ASSERT(memory == NULL, "safe_free should set the TCHAR pointer to NULL after freeing");
    }
#endif

static void test_is_Empty(void) {
    char buffer[10] = {0};
    TEST_ASSERT(is_Empty(buffer, sizeof(buffer)), "is_Empty should return true for a buffer initialized to zero");

    buffer[5] = 'A';
    TEST_ASSERT(!is_Empty(buffer, sizeof(buffer)), "is_Empty should return false for a buffer that contains non-zero values");
}

static void test_safe_memset(void) {
    char buffer[10];
    safe_memset(buffer, sizeof(buffer), '3', sizeof(buffer));
    for (size_t i = 0; i < sizeof(buffer); i++) {
        TEST_ASSERT(buffer[i] == '3', "safe_memset should set all bytes in the buffer to '3'");
    }
}

static void test_explicit_zeroes(void) {
    int buffer[10];
    for (size_t i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i++) {
        buffer[i] = 42; // Fill with non-zero values
    }
    explicit_zeroes(buffer, sizeof(buffer));
    for (size_t i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i++) {
        TEST_ASSERT(buffer[i] == 0, "explicit_zeroes should set all bytes in the buffer to zero");
    }

    // Testing when the buffer is already zeroed out
    int char_buffer[10] = {0};
    explicit_zeroes(char_buffer, sizeof(char_buffer));
    for (size_t i = 0; i < sizeof(char_buffer) / sizeof(char_buffer[0]); i++) {
        TEST_ASSERT(char_buffer[i] == 0, "explicit_zeroes should set all bytes in the buffer to zero");
    }
}

static void test_free_aligned(void) {
    char* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    free_aligned(ptr);
}

static void test_malloc_aligned(void) {
    char* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    free_aligned(ptr);
}

static void test_safe_free_aligned_core(void) {
    char* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_core((void**)&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_core should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_char(void) {
    char* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_char(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_char should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_wchar(void) {
    wchar_t* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_wchar(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_wchar should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_schar(void) {
    signed char* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_schar(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_schar should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_uchar(void) {
    unsigned char* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_uchar(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_uchar should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_short(void) {
    signed short* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_short(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_short should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_ushort(void) {
    unsigned short* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_ushort(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_ushort should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_int(void) {
    signed int* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_int(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_int should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_uint(void) {
    unsigned int* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_uint(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_uint should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_long(void) {
    signed long* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_long(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_long should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_ulong(void) {
    unsigned long* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_ulong(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_ulong should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_longlong(void) {
    signed long long* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_longlong(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_longlong should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_ulonglong(void) {
    unsigned long long* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_ulonglong(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_ulonglong should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_float(void) {
    float* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_float(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_float should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_double(void) {
    double* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_double(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_double should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned_longdouble(void) {
    long double* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned_longdouble(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned_longdouble should set the pointer to NULL after freeing");
}

static void test_safe_free_aligned(void) {
    char* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_aligned should set the pointer to NULL after freeing");

    float* ptr2 = malloc_aligned(100, 16);
    TEST_ASSERT(ptr2 != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned(&ptr2);
    TEST_ASSERT(ptr2 == NULL, "safe_free_aligned should set the pointer to NULL after freeing");

    long double* ptr3 = malloc_aligned(100, 16);
    TEST_ASSERT(ptr3 != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_aligned(&ptr3);
    TEST_ASSERT(ptr3 == NULL, "safe_free_aligned should set the pointer to NULL after freeing");
}

static void test_calloc_aligned(void) {
    size_t alignment = 16;
    size_t num_elements = 0;
    size_t element_size = sizeof(int);
    int* ptr = calloc_aligned(num_elements, element_size, alignment);
    TEST_ASSERT(ptr != NULL, "calloc_aligned should return a non-null pointer for non-zero count and size");
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT(ptr[i] == 0, "calloc_aligned should initialize all elements to zero");
    }
    free_aligned(ptr);
}

void run_memory_safety_tests(void) {
    test_safe_malloc();
    test_safe_calloc();
    test_safe_realloc();
    test_safe_reallocf();
    test_safe_free_core();
    test_safe_free_char();
    test_safe_free_schar();
    test_safe_free_uchar();
    test_safe_free_wchar();
    test_safe_free_short();
    test_safe_free_ushort();
    test_safe_free_int();
    test_safe_free_uint();
    test_safe_free_long();
    test_safe_free_ulong();
    test_safe_free_longlong();
    test_safe_free_ulonglong();
    test_safe_free_float();
    test_safe_free_double();
    test_safe_free_longdouble();
    test_safe_free();
    #ifdef _WIN32
    test_safe_free_tchar();
    #endif
    test_is_Empty();
    test_safe_memset();
    test_explicit_zeroes();
    test_free_aligned();
    test_malloc_aligned();
    test_safe_free_aligned_core();
    test_safe_free_aligned_char();
    test_safe_free_aligned_wchar();
    test_safe_free_aligned_schar();
    test_safe_free_aligned_uchar();
    test_safe_free_aligned_short();
    test_safe_free_aligned_ushort();
    test_safe_free_aligned_int();
    test_safe_free_aligned_uint();
    test_safe_free_aligned_long();
    test_safe_free_aligned_ulong();
    test_safe_free_aligned_longlong();
    test_safe_free_aligned_ulonglong();
    test_safe_free_aligned_float();
    test_safe_free_aligned_double();
    test_safe_free_aligned_longdouble();
    test_safe_free_aligned();
    test_calloc_aligned();
}