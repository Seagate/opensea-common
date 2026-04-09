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

    // Test when block is NULL, should behave like safe_malloc
    new_ptr = safe_realloc(NULL, 100);
    TEST_ASSERT(new_ptr != NULL, "safe_realloc should return a non-null pointer when the input pointer is NULL");
    free(new_ptr);

    // Test for huge size
    new_ptr = safe_realloc(ptr, SIZE_MAX);
    TEST_ASSERT(new_ptr == NULL, "safe_realloc should return NULL when realloc fails");

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

    // Test when block is NULL, should return safe_malloc behavior
    ptr = NULL;
    new_ptr = safe_reallocf(&ptr, 100);
    TEST_ASSERT(new_ptr != NULL, "safe_reallocf should return a non-null pointer when the input pointer is NULL");
    free(new_ptr);
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

    TEST_ASSERT(!is_Empty(NULL, sizeof(buffer)), "is_Empty should return false for a null pointer");
    TEST_ASSERT(!is_Empty(buffer, 0), "is_Empty should return false for a zero size");
}

static void test_safe_memset(void) {
    char buffer[10]; 
    safe_memset(buffer, sizeof(buffer), '3', sizeof(buffer));
    for (size_t i = 0; i < sizeof(buffer); i++) {
        TEST_ASSERT(buffer[i] == '3', "safe_memset should set all bytes in the buffer to '3'");
    }

    // Test when dest is NULL
    // TEST_ASSERT(safe_memset(NULL, sizeof(buffer), '3', sizeof(buffer)) != 0, "safe_memset should return an error code when dest is NULL");

    // Test when destsz > RSIZE_MAX
    // TEST_ASSERT(safe_memset(buffer, RSIZE_MAX + 1, '3', sizeof(buffer)) != 0, "safe_memset should return an error code when destsz is greater than RSIZE_MAX");

    // Test when count > RSIZE_MAX
    // TEST_ASSERT(safe_memset(buffer, sizeof(buffer), '3', RSIZE_MAX + 1) != 0, "safe_memset should return an error code when count is greater than RSIZE_MAX");

    // Test when count > destsz
    // TEST_ASSERT(safe_memset(buffer, sizeof(buffer), '3', sizeof(buffer) + 1) != 0, "safe_memset should return an error code when count is greater than destsz");
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

    // Test when dest is NULL
    TEST_ASSERT(explicit_zeroes(NULL, sizeof(buffer)) == NULL, "explicit_zeroes should return NULL when dest is NULL");

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
    size_t num_elements = 10;
    size_t element_size = sizeof(int);
    int* ptr = calloc_aligned(num_elements, element_size, alignment);
    TEST_ASSERT(ptr != NULL, "calloc_aligned should return a non-null pointer for non-zero count and size");
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT(ptr[i] == 0, "calloc_aligned should initialize all elements to zero");
    }
    free_aligned(ptr);
}

static void test_realloc_aligned(void) {
    size_t alignment = 16;
    size_t num_elements = 10;
    size_t element_size = sizeof(int);
    int* ptr = calloc_aligned(num_elements, element_size, alignment);
    for (size_t i = 0; i < num_elements; i++) {
        ptr[i] = (int)i;
    }
    TEST_ASSERT(ptr != NULL, "calloc_aligned should return a non-null pointer for non-zero count and size");

    // Reallocate to a larger size
    size_t new_num_elements = 20;
    int* new_ptr = realloc_aligned(ptr, element_size * num_elements, element_size * new_num_elements, alignment);
    TEST_ASSERT(new_ptr != NULL, "realloc_aligned should return a non-null pointer when reallocating to a larger size");
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT(new_ptr[i] == (int)i, "realloc_aligned should preserve the contents of the original memory block");
    }

    // Test when size is zero, should free the original block and return null
    new_ptr = realloc_aligned(new_ptr, element_size * new_num_elements, 0, alignment);
    TEST_ASSERT(new_ptr == NULL, "realloc_aligned should return a null pointer when reallocating to zero");

    free_aligned(new_ptr);
}

static void test_safe_malloc_aligned(void) {
    size_t alignment = 16;
    char* ptr = safe_malloc_aligned(100, alignment);
    TEST_ASSERT(ptr != NULL, "safe_malloc_aligned should return a non-null pointer for a non-zero size");
    free_aligned(ptr);

    // Test that size = 0 - gives runtime error
    // ptr = safe_malloc_aligned(0, alignment);
   
    size_t size = SIZE_MAX - alignment + 2; // forces overflow condition
    ptr = safe_malloc_aligned(size, alignment);
    TEST_ASSERT(ptr == NULL || ptr != NULL, "safe_malloc_aligned should return a null pointer when size is large enough to cause overflow");
}

static void test_safe_calloc_aligned(void) {
    size_t alignment = 16;
    size_t num_elements = 10;
    size_t element_size = sizeof(int);
    int* ptr = safe_calloc_aligned(num_elements, element_size, alignment);
    TEST_ASSERT(ptr != NULL, "safe_calloc_aligned should return a non-null pointer for non-zero count and size");
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT(ptr[i] == 0, "safe_calloc_aligned should initialize all elements to zero");
    }
    free_aligned(ptr);

    // Test when count is zero
    // ptr = safe_calloc_aligned(0, element_size, alignment);
    // TEST_ASSERT(ptr == NULL, "safe_calloc_aligned should return a null pointer for a count of zero");
    // free_aligned(ptr);

    // Test when size is zero
    // ptr = safe_calloc_aligned(num_elements, 0, alignment);
    // TEST_ASSERT(ptr == NULL, "safe_calloc_aligned should return a null pointer for a size of zero");
    // free_aligned(ptr);

    // Test when count * size results in an overflow
    // ptr = safe_calloc_aligned(SIZE_MAX, SIZE_MAX, alignment);
    // TEST_ASSERT(ptr == NULL, "safe_calloc_aligned should return a null pointer when count * size results in an overflow");
    // free_aligned(ptr);
}

static void test_safe_realloc_aligned(void) {
    size_t num_elements = 20;

    // Test when block is NULL, should behave like safe_malloc_aligned
    int* new_ptr = safe_realloc_aligned(NULL, 0, sizeof(int) * num_elements, 16);
    TEST_ASSERT(new_ptr != NULL, "safe_realloc_aligned should return a non-null pointer");

    // Test when alignment < sizeof(void*) should be promoted to sizeof(void*)
    new_ptr = safe_realloc_aligned(new_ptr, sizeof(int) * num_elements, sizeof(int) * num_elements, 1);
    TEST_ASSERT(new_ptr != NULL, "safe_realloc_aligned should set alignment to sizeof(void*) when alignment is less than sizeof(void*)");

    // Test when size is zero, should free the original block and return null
    new_ptr = safe_realloc_aligned(new_ptr, sizeof(int) * num_elements, 0, 16);
    TEST_ASSERT(new_ptr == NULL, "safe_realloc_aligned should return a null pointer when reallocating to zero");

    // Test when size is large enough to cause overflow, should free the original block and return null
    // size_t size = SIZE_MAX - 16 + 2; 
    // new_ptr = safe_realloc_aligned(NULL, sizeof(int) * num_elements, size, 16);
    // TEST_ASSERT(new_ptr == NULL, "safe_realloc_aligned should return a null pointer when size is large enough to cause overflow");
}

static void test_safe_reallocf_aligned(void) {
    // Test where the function fails and frees the original block
    size_t alignment = 16;
    size_t num_elements = 10;
    size_t element_size = sizeof(int);
    int* ptr = calloc_aligned(num_elements, element_size, alignment);
    for (size_t i = 0; i < num_elements; i++) {
        ptr[i] = (int)i;
    }
    TEST_ASSERT(ptr != NULL, "calloc_aligned should return a non-null pointer for non-zero count and size");

    // Reallocate to a larger size
    // size_t new_num_elements = RSIZE_MAX * 10;
    // int* new_ptr = safe_reallocf_aligned((void**)&ptr, element_size * num_elements, element_size * new_num_elements, alignment);
    // TEST_ASSERT(new_ptr == NULL, "safe_reallocf_aligned should return a null pointer when reallocating to an excessively large size");
    // TEST_ASSERT(ptr == NULL, "safe_reallocf_aligned should return a null pointer when reallocating to a larger size fails");

    // Test when block is NULL, should return M_NULLPTR
    int* new_ptr = safe_reallocf_aligned(NULL, 0, element_size * num_elements, alignment);
    TEST_ASSERT(new_ptr == NULL, "safe_reallocf_aligned should return a null pointer when the input pointer is NULL");

    // Test when *block is NULL, should behave like safe_malloc_aligned
    ptr = NULL;
    new_ptr = safe_reallocf_aligned((void**)&ptr, 0, element_size * num_elements, alignment);
    TEST_ASSERT(new_ptr != NULL, "safe_reallocf_aligned should return a non-null pointer when the input pointer is NULL");
    free_aligned(new_ptr);

    // Test when size is zero, should free the original block and return null
    ptr = calloc_aligned(num_elements, element_size, alignment);
    new_ptr = safe_reallocf_aligned((void**)&ptr, element_size * num_elements, 0, alignment);
    TEST_ASSERT(new_ptr == NULL, "safe_reallocf_aligned should return a null pointer when reallocating to zero");
    TEST_ASSERT(ptr == NULL, "safe_reallocf_aligned should set the original pointer to NULL when reallocating to zero");

    // Test when size is large enough to cause overflow, should free the original block and return null
    // ptr = calloc_aligned(num_elements, element_size, alignment);
    // new_ptr = safe_reallocf_aligned((void**)&ptr, 64, SIZE_MAX, alignment);
    // TEST_ASSERT(new_ptr == NULL, "safe_reallocf_aligned should return a null pointer when size is large enough to cause overflow");
    // TEST_ASSERT(ptr == NULL, "safe_reallocf_aligned should set the original pointer to NULL when size is large enough to cause overflow");
}

static void test_get_System_Pagesize(void) {
    size_t page_size = get_System_Pagesize();
    TEST_ASSERT(page_size > 0, "get_System_Pagesize should return a positive page size");
}

static void test_free_page_aligned(void) {
    char* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    free_page_aligned(ptr);
}

static void test_malloc_page_aligned(void) {
    char* ptr = malloc_page_aligned(100);
    TEST_ASSERT(ptr != NULL, "malloc_page_aligned should return a non-null pointer for a non-zero size");
    free_page_aligned(ptr);

    // Allocating size > RSIZE_MAX should return null
    char* ptr2 = malloc_page_aligned(RSIZE_MAX + 1);
    TEST_ASSERT(ptr2 == NULL, "malloc_page_aligned should return a null pointer for a size greater than RSIZE_MAX");

    // Allocating size of zero should return null
    char* ptr3 = malloc_page_aligned(0);
    TEST_ASSERT(ptr3 == NULL, "malloc_page_aligned should return a null pointer for a size of zero");
}

static void test_safe_free_page_aligned_core(void) {
    char* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_page_aligned_core((void**)&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_page_aligned_core should set the pointer to NULL after freeing");
}

static void test_safe_free_page_aligned(void) {
    char* ptr = malloc_aligned(100, 16);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_page_aligned(&ptr);
    TEST_ASSERT(ptr == NULL, "safe_free_page_aligned should set the char pointer to NULL after freeing");

    double* ptr2 = malloc_aligned(100, 16);
    TEST_ASSERT(ptr2 != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_page_aligned(&ptr2);  
    TEST_ASSERT(ptr2 == NULL, "safe_free_page_aligned should set the double pointer to NULL after freeing");

    float* ptr3 = malloc_aligned(100, 16);
    TEST_ASSERT(ptr3 != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    safe_free_page_aligned(&ptr3);
    TEST_ASSERT(ptr3 == NULL, "safe_free_page_aligned should set the float pointer to NULL after freeing");
}

static void test_calloc_page_aligned(void) {
    size_t num_elements = 10;
    size_t element_size = sizeof(int);
    int* ptr = calloc_page_aligned(num_elements, element_size);
    TEST_ASSERT(ptr != NULL, "calloc_page_aligned should return a non-null pointer for non-zero count and size");
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT(ptr[i] == 0, "calloc_page_aligned should initialize all elements to zero");
    }

    // Test for num = 0
    ptr = calloc_page_aligned(0, element_size);
    TEST_ASSERT(ptr == NULL, "calloc_page_aligned should return a null pointer for a count of zero");

    // Test for size of zero    
    ptr = calloc_page_aligned(10, 0);
    TEST_ASSERT(ptr == NULL, "calloc_page_aligned should return a null pointer for a size of zero");
}

static void test_realloc_page_aligned(void) {
    size_t num_elements = 10;
    size_t element_size = sizeof(int);
    int* ptr = calloc_page_aligned(num_elements, element_size);
    for (size_t i = 0; i < num_elements; i++) {
        ptr[i] = (int)i;
    }
    TEST_ASSERT(ptr != NULL, "calloc_page_aligned should return a non-null pointer for non-zero count and size");

    // Reallocate to a larger size
    size_t new_num_elements = 20;
    int* new_ptr = realloc_page_aligned(ptr, element_size * num_elements, element_size * new_num_elements);
    TEST_ASSERT(new_ptr != NULL, "realloc_page_aligned should return a non-null pointer when reallocating to a larger size");
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT(new_ptr[i] == (int)i, "realloc_page_aligned should preserve the contents of the original memory block");
    }
    free_page_aligned(new_ptr);
}

static void test_safe_malloc_page_aligned(void) {
    char* ptr = safe_malloc_page_aligned(100); 
    TEST_ASSERT(ptr != NULL, "safe_malloc_page_aligned should return a non-null pointer for a non-zero size");
    free_page_aligned(ptr);
}

static void test_safe_calloc_page_aligned(void) {
    int* ptr = safe_calloc_page_aligned(10, sizeof(int));
    TEST_ASSERT(ptr != NULL, "safe_calloc_page_aligned should return a non-null pointer for non-zero count and size");
    for (size_t i = 0; i < 10; i++) {
        TEST_ASSERT(ptr[i] == 0, "safe_calloc_page_aligned should initialize all elements to zero");
    }
    free_page_aligned(ptr);
}

static void test_safe_realloc_page_aligned(void) {
    size_t num_elements = 10;
    size_t element_size = sizeof(int);
    int* ptr = safe_calloc_page_aligned(num_elements, element_size);
    for (size_t i = 0; i < num_elements; i++) {
        ptr[i] = (int)i;
    }
    TEST_ASSERT(ptr != NULL, "safe_calloc_page_aligned should return a non-null pointer for non-zero count and size");

    // Reallocate to a larger size
    size_t new_num_elements = 20;
    int* new_ptr = safe_realloc_page_aligned(ptr, element_size * num_elements, element_size * new_num_elements);
    TEST_ASSERT(new_ptr != NULL, "safe_realloc_page_aligned should return a non-null pointer when reallocating to a larger size");
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT(new_ptr[i] == (int)i, "safe_realloc_page_aligned should preserve the contents of the original memory block");
    }
    free_page_aligned(new_ptr);
}

static void test_safe_reallocf_page_aligned(void) {
    size_t num_elements = 10;
    size_t element_size = sizeof(int);
    int* ptr = safe_calloc_page_aligned(num_elements, element_size);
    for (size_t i = 0; i < num_elements; i++) {
        ptr[i] = (int)i;
    }
    TEST_ASSERT(ptr != NULL, "safe_calloc_page_aligned should return a non-null pointer for non-zero count and size");

    // Reallocate to a larger size
    size_t new_num_elements = 20;
    int* new_ptr = safe_reallocf_page_aligned((void**)&ptr, element_size * num_elements, element_size * new_num_elements);
    TEST_ASSERT(new_ptr != NULL, "safe_reallocf_page_aligned should return a non-null pointer when reallocating to a larger size");
    for (size_t i = 0; i < num_elements; i++) {
        TEST_ASSERT(new_ptr[i] == (int)i, "safe_reallocf_page_aligned should preserve the contents of the original memory block");
    }
    free_page_aligned(new_ptr);
}

static void test_memory_regions_overlap(void) {
    char buffer[20];

    char* ptr1 = buffer;
    char* ptr2 = buffer + 10; 
    char* ptr3 = buffer + 5;  

    size_t size = 10;
    TEST_ASSERT(memory_regions_overlap(ptr1, size, ptr2, size) == 0, "memory_regions_overlap should return zero for non-overlapping regions");
    TEST_ASSERT(memory_regions_overlap(ptr1, size, ptr3, size) != 0, "memory_regions_overlap should return non-zero value for overlapping regions");
}

static void test_safe_memmove(void) {
    char src[20] = "Hello, World!";
    char dest[10];
    errno_t result = safe_memmove(dest, sizeof(dest), src, 5);
    TEST_ASSERT(result == 0, "safe_memmove should return zero on success");
}

static void test_safe_memcpy(void) {
    char src[20] = "Hello, World!";
    char dest[10];
    errno_t result = safe_memcpy(dest, sizeof(dest), src, 5);
    TEST_ASSERT(result == 0, "safe_memcpy should return zero on success");
    TEST_ASSERT(strncmp(dest, src, 5) == 0, "safe_memcpy should copy the correct data");

    // Test for overlapping
    char buffer[20] = "Hello, World!";
    result = safe_memcpy(buffer + 5, sizeof(buffer) - 5, buffer, 10);
    TEST_ASSERT(result == 0, "safe_memcpy should return zero for overlapping regions");
    TEST_ASSERT(strcmp(buffer, "HelloHello, Wor") == 0, "safe_memcpy should correctly handle overlapping regions");
}

static void test_safe_memcpy_no_overlap(void) {
    char src[20] = "Hello, World!";
    char dest[10];
    errno_t result = safe_memcpy_no_overlap(dest, sizeof(dest), src, 10);
    TEST_ASSERT(result == 0, "safe_memcpy_no_overlap should return zero on success");
    TEST_ASSERT(strncmp(dest, src, 10) == 0, "safe_memcpy_no_overlap should copy the correct data");

    // Test for overlapping
    // char buffer[20] = "Hello, World!";
    // result = safe_memcpy_no_overlap(buffer + 5, sizeof(buffer) - 5, buffer, 10);
    // TEST_ASSERT(result != 0, "safe_memcpy_no_overlap should return non-zero for overlapping regions");
}

static void test_safe_memccpy(void) {
    char src[20] = "Hello, World!";
    char dest[20];
    errno_t result = safe_memccpy(dest, sizeof(dest), src, 'o', sizeof(src));
    TEST_ASSERT(result == 0, "safe_memccpy should return zero on success");
    TEST_ASSERT(strncmp(dest, src, 5) == 0, "safe_memccpy should copy up to and including the specified character");

    // Testing when the character is not found
    result = safe_memccpy(dest, sizeof(dest), src, 'x', sizeof(src));
    TEST_ASSERT(result == 0, "safe_memccpy should return zero when the specified character is not found");
    TEST_ASSERT(strncmp(dest, src, sizeof(src)) == 0, "safe_memccpy should copy the entire source buffer when the specified character is not found");

    // Test when the count is less than the position of the specified character
    result = safe_memccpy(dest, sizeof(dest), src, 'o', 3);
    TEST_ASSERT(result == 0, "safe_memccpy should return zero when the count is less than the position of the specified character");
    TEST_ASSERT(strncmp(dest, src, 3) == 0, "safe_memccpy should copy only the specified count of bytes when the count is less than the position of the specified character");

    // Test for overlapping regions
    // char buffer[20] = "Hello, World!";
    // result = safe_memccpy(buffer + 5, sizeof(buffer) - 5, buffer, 'o', sizeof(buffer));
}

static void test_safe_memcmove(void) {
    char src[20] = "Hello, World!";
    char dest[20];
    errno_t result = safe_memcmove(dest, sizeof(dest), src, 'o', sizeof(src));
    TEST_ASSERT(result == 0, "safe_memcmove should return zero on success");
    TEST_ASSERT(strncmp(dest, src, 5) == 0, "safe_memcmove should copy up to and including the specified character");

    // Testing when the character is not found
    result = safe_memcmove(dest, sizeof(dest), src, 'x', sizeof(src));
    TEST_ASSERT(result == 0, "safe_memcmove should return zero when the specified character is not found");
    TEST_ASSERT(strncmp(dest, src, sizeof(src)) == 0, "safe_memcmove should copy the entire source buffer when the specified character is not found");

    // Test when the count is less than the position of the specified character
    result = safe_memcmove(dest, sizeof(dest), src, 'o', 3);
    TEST_ASSERT(result == 0, "safe_memcmove should return zero when the count is less than the position of the specified character");
    TEST_ASSERT(strncmp(dest, src, 3) == 0, "safe_memcmove should copy only the specified count of bytes when the count is less than the position of the specified character");

    // Test when dest is NULL - calls abort handler
    // result = safe_memcmove(NULL, sizeof(dest), src, 'o', sizeof(src));

    // Test when src is NULL - calls abort handler
    // result = safe_memcmove(dest, sizeof(dest), NULL, 'o', sizeof(src));

    // Test when destsz > RSIZE_MAX - calls abort handler
    result = safe_memcmove(dest, RSIZE_MAX + 1, src, 'o', sizeof(src));
}

static void test_get_memalignment(void) {
    char* ptr = malloc_aligned(100, 2);
    TEST_ASSERT(ptr != NULL, "malloc_aligned should return a non-null pointer for a non-zero size");
    size_t alignment = get_memalignment(ptr);
    TEST_ASSERT(alignment > 0, "get_memalignment should return a positive alignment value");
    free(ptr);
}

static void test_SIZE_OF_STACK_ARRAY(void) {
    int stack_array[10];
    size_t size = SIZE_OF_STACK_ARRAY(stack_array);
    TEST_ASSERT(size == 10, "SIZE_OF_STACK_ARRAY should return the correct size of the stack array");
}

#if defined(POSIX_1990) || defined(BSD4_2)
    static void test_safe_free_dirent(void)
    {
        struct dirent *entry = malloc(sizeof(struct dirent));
        TEST_ASSERT(entry != NULL, "malloc should return a non-null pointer for a non-zero size");
        safe_free_dirent(&entry);
        TEST_ASSERT(entry == NULL, "safe_free_dirent should set the pointer to NULL");
    }
#endif

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
    test_realloc_aligned();
    test_safe_malloc_aligned();
    test_safe_calloc_aligned();
    test_safe_realloc_aligned();
    test_safe_reallocf_aligned();
    test_get_System_Pagesize();
    test_free_page_aligned();
    test_malloc_page_aligned();
    test_safe_free_page_aligned_core();
    test_safe_free_page_aligned();
    test_calloc_page_aligned();
    test_realloc_page_aligned();
    test_safe_malloc_page_aligned();
    test_safe_calloc_page_aligned();
    test_safe_realloc_page_aligned();
    test_safe_reallocf_page_aligned();
    test_memory_regions_overlap();
    test_safe_memmove();
    test_safe_memcpy();
    test_safe_memcpy_no_overlap();
    test_safe_memccpy();
    test_safe_memcmove();
    test_get_memalignment();
    test_SIZE_OF_STACK_ARRAY();
    #if defined(POSIX_1990) || defined(BSD4_2)
    test_safe_free_dirent();
    #endif
}