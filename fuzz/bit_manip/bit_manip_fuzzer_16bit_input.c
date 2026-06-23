#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 8) {
        return 0;
    }

    uint16_t msw = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
    uint16_t word2 = (uint16_t)data[2] | ((uint16_t)data[3] << 8);
    uint16_t word1 = (uint16_t)data[4] | ((uint16_t)data[5] << 8);
    uint16_t lsw = (uint16_t)data[6] | ((uint16_t)data[7] << 8);

    uint32_t words_To_Uint32_result = words_To_Uint32(msw, lsw);
    if (words_To_Uint32_result != ((M_STATIC_CAST(uint32_t, msw) << 16) | (M_STATIC_CAST(uint32_t, lsw) << 0))) __builtin_trap();

    uint64_t words_To_Uint64_result = words_To_Uint64(msw, word2, word1, lsw);
    if (words_To_Uint64_result != ((M_STATIC_CAST(uint64_t, msw) << 48) | (M_STATIC_CAST(uint64_t, word2) << 32) | (M_STATIC_CAST(uint64_t, word1) << 16) | (M_STATIC_CAST(uint64_t, lsw) << 0))) __builtin_trap();

    uint16_t b_swap_16_result = b_swap_16(msw);
    if (b_swap_16_result != (((msw & UINT16_C(0x00FF)) << 8) | ((msw & UINT16_C(0xFF00)) >> 8))) __builtin_trap();

    uint16_t byte_Swap_16_result = msw;
    byte_Swap_16(&byte_Swap_16_result);
    if (byte_Swap_16_result != b_swap_16_result) __builtin_trap();

    int16_t signed_value = (int16_t)msw;
    int16_t byte_Swap_Int16_result = signed_value;
    byte_Swap_Int16(&byte_Swap_Int16_result);
    if (byte_Swap_Int16_result != (int16_t)b_swap_16_result) __builtin_trap();

    // Fuzzing get_Bytes_To_16
    // size_t fullDataLen = data[0] % 16;
    // size_t msb = data[1];
    // size_t lsb = data[2];

    // uint8_t buffer[16] = {0};
    // if (fullDataLen > 0) {
    //     size_t copyLen = (fullDataLen < size - 3) ? fullDataLen : size - 3;
    //     memcpy(buffer, data + 3, copyLen);
    // }

    // uint16_t outVal = 0xDEAD;
    // bool result = get_Bytes_To_16(buffer, fullDataLen, msb, lsb, &outVal);

    // bool expectedValid =
    //     (&outVal != NULL) &&
    //     (abs(msb - lsb) <= sizeof(uint16_t)) && 
    //     (msb < fullDataLen) &&
    //     (lsb < fullDataLen);

    // if (result) {
    //     if (!expectedValid) {
    //         fprintf(stderr, "Function succeeded on invalid input!\n");
    //         return 0;
    //     }
    //     uint16_t expected = ((uint16_t)buffer[msb] << 8) | buffer[lsb];
    //     if (outVal != expected) {
    //         fprintf(stderr, "Mismatch: got=%04x expected=%04x\n", outVal, expected);
    //         return 0;
    //     }
    // } else {
    //     if (expectedValid) {
    //         fprintf(stderr, "Function failed on valid input!\n");
    //         return 0;
    //     }
    // }

    // Fuzzing be16_to_host
    uint16_t be16_to_host_result = be16_to_host(msw);
    #if defined(ENV_BIG_ENDIAN)
        if (be16_to_host_result != msw) __builtin_trap();
    #else
        if (be16_to_host_result != b_swap_16(msw)) __builtin_trap();
    #endif

    // Fuzzing host_to_be16
    uint16_t host_to_be16_result = host_to_be16(msw);
    #if defined(ENV_BIG_ENDIAN)
        if (host_to_be16_result != msw) __builtin_trap();
    #else
        if (host_to_be16_result != b_swap_16(msw)) __builtin_trap();
    #endif

    // Fuzzing host_to_le16
    uint16_t host_to_le16_result = host_to_le16(msw);
    #if defined(ENV_LITTLE_ENDIAN) 
        if (host_to_le16_result != msw) __builtin_trap();
    #else
        if (host_to_le16_result != b_swap_16(msw)) __builtin_trap();
    #endif

    // Fuzzing le16_to_host
    uint16_t le16_to_host_result = le16_to_host(msw);
    #if defined(ENV_LITTLE_ENDIAN)
        if (le16_to_host_result != msw) __builtin_trap();
    #else
        if (le16_to_host_result != b_swap_16(msw)) __builtin_trap();
    #endif

    // Fuzzing big_To_Little_Endian_16
    uint16_t big_To_Little_Endian_16_result = msw;
    big_To_Little_Endian_16(&big_To_Little_Endian_16_result);
    if (big_To_Little_Endian_16_result != be16_to_host(msw)) __builtin_trap();

    return 0;
}