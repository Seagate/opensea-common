#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 2 * sizeof(uint32_t)) {
        return 0;
    }

    uint32_t val1 = 0;
    for (size_t i = 0; i < sizeof(uint32_t); i++) {
        val1 |= ((uint32_t)data[i]) << (8 * i);
    }

    uint32_t val2 = 0;
    for (size_t i = 0; i < sizeof(uint32_t); i++) {
        val2 |= ((uint32_t)data[i + sizeof(uint32_t)]) << (8 * i);
    }

    uint16_t lowest_16bits = get_Word0_uint32(val1);
    if (lowest_16bits != (uint16_t)(val1 & UINT32_C(0x0000FFFF))) __builtin_trap();

    uint16_t highest_16bits = get_Word1_uint32(val2);
    if (highest_16bits != (uint16_t)((val2 & UINT32_C(0xFFFF0000)) >> 16)) __builtin_trap();

    uint64_t dwords_To_Uint64_result = dwords_To_Uint64(val1, val2);
    if (dwords_To_Uint64_result != ((M_STATIC_CAST(uint64_t, val1) << 32) | (M_STATIC_CAST(uint64_t, val2) << 0))) __builtin_trap();

    uint32_t b_swap_32_result = b_swap_32(val1);
    if (b_swap_32_result != (((val1 & UINT32_C(0xFF000000)) >> 24) |
        ((val1 & UINT32_C(0x00FF0000)) >> 8)  |
        ((val1 & UINT32_C(0x0000FF00)) << 8)  |
        ((val1 & UINT32_C(0x000000FF)) << 24))) __builtin_trap();

    uint32_t byte_Swap_32_result = val1;
    byte_Swap_32(&byte_Swap_32_result);
    if (byte_Swap_32_result != b_swap_32_result) __builtin_trap();

    uint32_t signed_val1 = (int32_t)val1;
    uint32_t byte_Swap_Int32_result = signed_val1;
    byte_Swap_Int32(&byte_Swap_Int32_result);
    if (byte_Swap_Int32_result != (uint32_t)b_swap_32_result) __builtin_trap();
        
    uint32_t w_swap_32_result = w_swap_32(val1);
    if (w_swap_32_result != ((((val1 & UINT32_C(0x0000FFFF)) << 16) |
        ((val1 & UINT32_C(0xFFFF0000)) >> 16)))) __builtin_trap();

    uint32_t word_Swap_32_result = val1;
    word_Swap_32(&word_Swap_32_result);
    if (word_Swap_32_result != w_swap_32_result) __builtin_trap();

    // Fuzzing get_Bytes_To_32
    // size_t fullDataLen = data[0] % 16;
    // size_t msb = data[1];
    // size_t lsb = data[2];

    // uint8_t buffer[16] = {0};
    // if (fullDataLen > 0) {
    //     size_t copyLen = (fullDataLen < size - 3) ? fullDataLen : size - 3;
    //     memcpy(buffer, data + 3, copyLen);
    // }

    // uint32_t outVal = 0xDEAD;
    // bool result = get_Bytes_To_32(buffer, fullDataLen, msb, lsb, &outVal);

    // bool expectedValid =
    // (msb <= fullDataLen) &&
    // (lsb <= fullDataLen) &&
    // (abs(msb - lsb) <= sizeof(uint32_t));

    // if (result) {
    //     if (!expectedValid) {
    //         fprintf(stderr, "Function succeeded on invalid input!\n");
    //         return 0;
    //     }

    //     uint64_t temp = 0;
    //     if (lsb <= msb) {
    //         for (size_t iter = msb; iter >= lsb; --iter) {
    //             temp <<= 8;
    //             temp |= buffer[iter];
    //             if (iter == 0) break;
    //         }
    //     } else {
    //         for (size_t iter = msb; iter <= lsb; ++iter) {
    //             temp <<= 8;
    //             temp |= buffer[iter];
    //         }
    //     }

    //     uint32_t expected = (uint32_t)temp;
    //     if (outVal != expected) {
    //         fprintf(stderr, "Mismatch: got=%08x expected=%08x\n", outVal, expected);
    //         return 0;
    //     }
    // } else {
    //     if (expectedValid) {
    //         fprintf(stderr, "Function failed on valid input!\n");
    //         return 0;
    //     }
    // }

    // Fuzzing be32_to_host
    uint32_t be32_to_host_result = be32_to_host(val1);
    #if defined(ENV_BIG_ENDIAN)
        if (be32_to_host_result != val1) __builtin_trap();
    #else
        if (be32_to_host_result != b_swap_32(val1)) __builtin_trap();
    #endif

    // Fuzzing host_to_be32
    uint32_t host_to_be32_result = host_to_be32(val1);
    #if defined(ENV_BIG_ENDIAN)
        if (host_to_be32_result != val1) __builtin_trap();
    #else
        if (host_to_be32_result != b_swap_32(val1)) __builtin_trap();
    #endif
    
    // Fuzzing host_to_le32
    uint32_t host_to_le32_result = host_to_le32(val1);
    #if defined(ENV_LITTLE_ENDIAN) 
        if (host_to_le32_result != val1) __builtin_trap();
    #else
        if (host_to_le32_result != b_swap_32(val1)) __builtin_trap();
    #endif

    // Fuzzing le32_to_host
    uint32_t le32_to_host_result = le32_to_host(val1);
    #if defined(ENV_LITTLE_ENDIAN)
        if (le32_to_host_result != val1) __builtin_trap();
    #else
        if (le32_to_host_result != b_swap_32(val1)) __builtin_trap();
    #endif

    // Fuzzing big_To_Little_Endian_32
    uint32_t big_To_Little_Endian_32_result = val1;
    big_To_Little_Endian_32(&big_To_Little_Endian_32_result);
    if (big_To_Little_Endian_32_result != be32_to_host(val1)) __builtin_trap();

    // Fuzzing count_leading_zeros_ui
    uint32_t count_leading_zeros_ui_result = count_leading_zeros_ui(val1);
    if (count_leading_zeros_ui_result != (val1 == 0U ? M_STATIC_CAST(unsigned int, UINT_WIDTH) : M_STATIC_CAST(unsigned int, __builtin_clz(val1)))) __builtin_trap();

    // Fuzzing count_leading_ones_ui
    uint32_t count_leading_ones_ui_result = count_leading_ones_ui(val1);
    if (count_leading_ones_ui_result != count_leading_zeros_ui(~val1)) __builtin_trap();

    // Fuzzing count_trailing_zeros_ui
    uint32_t count_trailing_zeros_ui_result = count_trailing_zeros_ui(val1);
    if (count_trailing_zeros_ui_result != (val1 == 0U ? M_STATIC_CAST(unsigned int, UINT_WIDTH) : M_STATIC_CAST(unsigned int, __builtin_ctz(val1)))) __builtin_trap();

    // Fuzzing count_trailing_ones_ui
    uint32_t count_trailing_ones_ui_result = count_trailing_ones_ui(val1);
    if (count_trailing_ones_ui_result != count_trailing_zeros_ui(~val1)) __builtin_trap();

    // Fuzzing first_leading_one_ui
    uint32_t first_leading_one_ui_result = first_leading_one_ui(val1);
    if (first_leading_one_ui_result != (val1 == 0U ? 0U : M_STATIC_CAST(unsigned int, __builtin_clz(val1) + 1))) __builtin_trap();

    // Fuzzing first_leading_zero_ui
    uint32_t first_leading_zero_ui_result = first_leading_zero_ui(val1);
    if (first_leading_zero_ui_result != first_leading_one_ui(~val1)) __builtin_trap();

    // Fuzzing count_ones_ui
    uint32_t count_ones_ui_result = count_ones_ui(val1);
    if (count_ones_ui_result != M_STATIC_CAST(unsigned int, __builtin_popcount(val1))) __builtin_trap();

    // Fuzzing count_zeros_ui
    uint32_t count_zeros_ui_result = count_zeros_ui(val1);
    if (count_zeros_ui_result != count_ones_ui(~val1)) __builtin_trap();

    // Fuzzing has_single_bit_ui
    bool has_single_bit_ui_result = has_single_bit_ui(val1);
    if (has_single_bit_ui_result != (val1 != 0U && (val1 & (val1 - 1U)) == 0U)) __builtin_trap();

    // Fuzzing get_req_bit_width_ui
    uint32_t get_req_bit_width_ui_result = get_req_bit_width_ui(val1);
    if (get_req_bit_width_ui_result != (UINT_WIDTH - (val1 == 0U ? UINT_WIDTH : count_leading_zeros_ui(val1)))) __builtin_trap();

    // Fuzzing bit_floor_ui
    uint32_t bit_floor_ui_result = bit_floor_ui(val1);
    if (bit_floor_ui_result != (val1 == 0U ? 0U : 1U << (UINT_WIDTH - 1U - count_leading_zeros_ui(val1)))) __builtin_trap();

    // Fuzzing bit_ceil_ui
    uint32_t bit_ceil_ui_result = bit_ceil_ui(val1);
    if (bit_ceil_ui_result != (val1 <= 1U ? 1U : 2U << (UINT_WIDTH - 1U - count_leading_zeros_ui(val1 - 1U)))) __builtin_trap();


    // Fuzzing rotate_left_ui
    uint32_t rotate_left_ui_result = rotate_left_ui(val1, 3);
    uint32_t expected_rotate_left_ui_result;
    unsigned left_count  = 3 % UINT_WIDTH;
    unsigned right_count = (UINT_WIDTH - left_count) % UINT_WIDTH;

    expected_rotate_left_ui_result = (val1 << left_count) | (val1 >> right_count);

    if (rotate_left_ui_result != expected_rotate_left_ui_result) {
        __builtin_trap();
    }

    // Fuzzing rotate_right_ui
    uint32_t rotate_right_ui_result = rotate_right_ui(val1, 3);
    unsigned left_count  = (UINT_WIDTH - (3 % UINT_WIDTH)) % UINT_WIDTH;
    unsigned right_count = 3 % UINT_WIDTH;

    uint32_t expected_rotate_right_ui_result = (uint32_t)((val1 >> right_count) | (val1 << left_count));

    if (rotate_right_ui_result != expected_rotate_right_ui_result) {
        __builtin_trap();
    }

    return 0;
}
