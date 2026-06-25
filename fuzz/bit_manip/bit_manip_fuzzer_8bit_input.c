#include <stdint.h>
#include <stddef.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 16) {
        return 0;
    }

    uint8_t upperNibble = data[0];
    uint8_t lowerNibble = data[1];

    uint8_t msb16 = data[2];
    uint8_t lsb16 = data[3];

    uint8_t msb32 = data[4];
    uint8_t byte2_32 = data[5];
    uint8_t byte1_32 = data[6];
    uint8_t lsb32 = data[7];

    uint8_t msb64 = data[8];
    uint8_t byte6_64 = data[9];
    uint8_t byte5_64 = data[10];
    uint8_t byte4_64 = data[11];
    uint8_t byte3_64 = data[12];
    uint8_t byte2_64 = data[13];
    uint8_t byte1_64 = data[14];
    uint8_t lsb64 = data[15];

    uint8_t nibbles_To_Byte_result = nibbles_To_Byte(upperNibble, lowerNibble);
    if (nibbles_To_Byte_result != ((((upperNibble)&M_STATIC_CAST(uint8_t, 0x0F)) << 4) |(((lowerNibble)&M_STATIC_CAST(uint8_t, 0x0F)) << 0))) __builtin_trap();

    uint16_t bytes_To_Uint16_result = bytes_To_Uint16(msb16, lsb16);
    if (bytes_To_Uint16_result != ((M_STATIC_CAST(uint16_t, msb16) << 8) | (M_STATIC_CAST(uint16_t, lsb16) << 0))) __builtin_trap();

    uint32_t bytes_To_Uint32_result = bytes_To_Uint32(msb32, byte2_32, byte1_32, lsb32);
    if (bytes_To_Uint32_result != ((M_STATIC_CAST(uint32_t, msb32) << 24) | (M_STATIC_CAST(uint32_t, byte2_32) << 16) | (M_STATIC_CAST(uint32_t, byte1_32) << 8) | (M_STATIC_CAST(uint32_t, lsb32) << 0))) __builtin_trap();

    uint64_t bytes_To_Uint64_result = bytes_To_Uint64(msb64, byte6_64, byte5_64, byte4_64, byte3_64, byte2_64, byte1_64, lsb64);
    if (bytes_To_Uint64_result != ((M_STATIC_CAST(uint64_t, msb64) << 56) | (M_STATIC_CAST(uint64_t, byte6_64) << 48) | (M_STATIC_CAST(uint64_t, byte5_64) << 40) | (M_STATIC_CAST(uint64_t, byte4_64) << 32) | (M_STATIC_CAST(uint64_t, byte3_64) << 24) | (M_STATIC_CAST(uint64_t, byte2_64) << 16) | (M_STATIC_CAST(uint64_t, byte1_64) << 8) | (M_STATIC_CAST(uint64_t, lsb64) << 0))) __builtin_trap();

    uint8_t n_swap_8_result = n_swap_8(byte1_32);
    if (n_swap_8_result != ((M_STATIC_CAST(uint8_t, ((byte1_32 & UINT8_C(0x0F)) << 4))) | (M_STATIC_CAST(uint8_t, ((byte1_32 & UINT8_C(0xF0)) >> 4))))) __builtin_trap();

    uint8_t nibble_Swap_result = byte1_32;
    nibble_Swap(&nibble_Swap_result);
    if (nibble_Swap_result != n_swap_8_result) __builtin_trap();

    // Fuzzing count_leading_zeros_uc
    uint8_t count_leading_zeros_uc_result = count_leading_zeros_uc(byte1_32);
    if (count_leading_zeros_uc_result != (byte1_32 == 0U ? M_STATIC_CAST(unsigned int, UCHAR_WIDTH) : M_STATIC_CAST(unsigned int, __builtin_clz(byte1_32) - (UINT_WIDTH - UCHAR_WIDTH)))) __builtin_trap();

    // Fuzzing count_leading_ones_uc
    uint8_t count_leading_ones_uc_result = count_leading_ones_uc(byte1_32);
    if (count_leading_ones_uc_result != count_leading_zeros_uc(~byte1_32)) __builtin_trap();

    // Fuzzing count_trailing_zeros_uc
    uint8_t count_trailing_zeros_uc_result = count_trailing_zeros_uc(byte1_32);
    if (count_trailing_zeros_uc_result != (byte1_32 == 0U ? M_STATIC_CAST(unsigned int, UCHAR_WIDTH) : M_STATIC_CAST(unsigned int, __builtin_ctz(byte1_32)))) __builtin_trap();

    // Fuzzing count_trailing_ones_uc
    uint8_t count_trailing_ones_uc_result = count_trailing_ones_uc(byte1_32);
    if (count_trailing_ones_uc_result != count_trailing_zeros_uc(~byte1_32)) __builtin_trap();

    // Fuzzing first_leading_one_uc
    uint8_t first_leading_one_uc_result = first_leading_one_uc(byte1_32);
    if (first_leading_one_uc_result != (byte1_32 == 0U ? 0U : M_STATIC_CAST(unsigned int, __builtin_clz(byte1_32) - (UINT_WIDTH - UCHAR_WIDTH) + 1))) __builtin_trap();

    // Fuzzing first_leading_zero_uc
    uint8_t first_leading_zero_uc_result = first_leading_zero_uc(byte1_32);
    if (first_leading_zero_uc_result != first_leading_one_uc(~byte1_32)) __builtin_trap();

    // Fuzzing count_ones_uc
    uint8_t count_ones_uc_result = count_ones_uc(byte1_32);
    if (count_ones_uc_result != M_STATIC_CAST(unsigned int, __builtin_popcount(byte1_32))) __builtin_trap();

    // Fuzzing count_zeros_uc
    uint8_t count_zeros_uc_result = count_zeros_uc(byte1_32);
    if (count_zeros_uc_result != count_ones_uc(~byte1_32)) __builtin_trap();

    // Fuzzing has_single_bit_uc
    bool has_single_bit_uc_result = has_single_bit_uc(byte1_32);
    if (has_single_bit_uc_result != (byte1_32 != 0U && (byte1_32 & (byte1_32 - 1U)) == 0U)) __builtin_trap();

    // Fuzzing get_req_bit_width_uc
    uint32_t get_req_bit_width_uc_result = get_req_bit_width_uc(byte1_32);
    if (get_req_bit_width_uc_result != (UCHAR_WIDTH - (byte1_32 == 0U ? UCHAR_WIDTH : count_leading_zeros_uc(byte1_32)))) __builtin_trap();

    // Fuzzing bit_floor_uc
    uint8_t bit_floor_uc_result = bit_floor_uc(byte1_32);
    if (bit_floor_uc_result != (byte1_32 == 0U ? 0U : 1U << (UCHAR_WIDTH - 1U - count_leading_zeros_uc(byte1_32)))) __builtin_trap();

    // Fuzzing bit_ceil_uc
    uint8_t bit_ceil_uc_result = bit_ceil_uc(byte1_32);
    uint8_t expected_uc;

    if (byte1_32 <= 1U) {
        expected_uc = 1U;
    } else {
        uint8_t temp = (uint8_t)(byte1_32 - 1U);
        unsigned shift = UCHAR_WIDTH - 1U - count_leading_zeros_uc(temp);
        if (shift < UCHAR_WIDTH) {
        expected_uc = (uint8_t)(2U << shift);
        } else {
            expected_uc = 0;
        }
    }

    if (bit_ceil_uc_result != expected_uc) {
        __builtin_trap();
    }

    return 0;
}
