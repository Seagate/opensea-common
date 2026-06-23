#include <stdint.h>
#include <stddef.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < sizeof(uint64_t)) {
        return 0;
    }

    uint64_t value = 0;
    for (size_t i = 0; i < sizeof(uint64_t); i++) {
        value |= ((uint64_t)data[i]) << (8 * i);
    }

    uint32_t lower_uint32bit = get_DWord0(value);
    if (lower_uint32bit != (uint32_t)value) __builtin_trap();

    uint32_t upper_uint32bit = get_DWord1(value);
    if (upper_uint32bit != (uint32_t)(value >> 32)) __builtin_trap();

    int32_t lower_int32bit = M_DoubleWordInt0(value);
    if (lower_int32bit != (int32_t)value) __builtin_trap();

    int32_t upper_int32bit = M_DoubleWordInt1(value);
    if (upper_int32bit != (int32_t)(value >> 32)) __builtin_trap();

    uint16_t lower_uint16bit = get_Word0_uint64(value);
    if (lower_uint16bit != (uint16_t)(value & UINT64_C(0x000000000000FFFF))) __builtin_trap();

    uint16_t second_lower_uint16bit = get_Word1_uint64(value);
    if (second_lower_uint16bit != (uint16_t)((value & UINT64_C(0x00000000FFFF0000)) >> 16)) __builtin_trap();

    uint16_t second_highest_uint16bit = get_Word2_uint64(value);
    if (second_highest_uint16bit != (uint16_t)((value & UINT64_C(0x0000FFFF00000000)) >> 32)) __builtin_trap();

    uint16_t highest_uint16bit = get_Word3_uint64(value);
    if (highest_uint16bit != (uint16_t)((value & UINT64_C(0xFFFF000000000000)) >> 48)) __builtin_trap();

    int16_t lower_int16bit = M_WordInt0(value);
    if (lower_int16bit != (int16_t)(value & UINT64_C(0x000000000000FFFF))) __builtin_trap();

    int16_t second_lower_int16bit = M_WordInt1(value);
    if (second_lower_int16bit != (int16_t)((value & UINT64_C(0x00000000FFFF0000)) >> 16)) __builtin_trap();

    int16_t second_highest_int16bit = M_WordInt2(value);
    if (second_highest_int16bit != (int16_t)((value & UINT64_C(0x0000FFFF00000000)) >> 32)) __builtin_trap();

    int16_t highest_int16bit = M_WordInt3(value);
    if (highest_int16bit != (int16_t)((value & UINT64_C(0xFFFF000000000000)) >> 48)) __builtin_trap();

    uint8_t lowest_uint_byte = M_Byte0(value);
    if (lowest_uint_byte != (uint8_t)value) __builtin_trap();

    uint8_t second_lowest_uint_byte = M_Byte1(value);
    if (second_lowest_uint_byte != (uint8_t)(value >> 8)) __builtin_trap();

    uint8_t third_lowest_uint_byte = M_Byte2(value);
    if (third_lowest_uint_byte != (uint8_t)(value >> 16)) __builtin_trap();

    uint8_t fourth_lowest_uint_byte = M_Byte3(value);
    if (fourth_lowest_uint_byte != (uint8_t)(value >> 24)) __builtin_trap();

    uint8_t fifth_lowest_uint_byte = M_Byte4(value);
    if (fifth_lowest_uint_byte != (uint8_t)(value >> 32)) __builtin_trap();

    uint8_t sixth_lowest_uint_byte = M_Byte5(value);
    if (sixth_lowest_uint_byte != (uint8_t)(value >> 40)) __builtin_trap();

    uint8_t seventh_lowest_uint_byte = M_Byte6(value);
    if (seventh_lowest_uint_byte != (uint8_t)(value >> 48)) __builtin_trap();

    uint8_t highest_uint_byte = M_Byte7(value);
    if (highest_uint_byte != (uint8_t)(value >> 56)) __builtin_trap();

    int8_t lowest_int_byte = M_ByteInt0(value);
    if (lowest_int_byte != (int8_t)value) __builtin_trap();

    int8_t second_lowest_int_byte = M_ByteInt1(value);
    if (second_lowest_int_byte != (int8_t)(value >> 8)) __builtin_trap();

    int8_t third_lowest_int_byte = M_ByteInt2(value);
    if (third_lowest_int_byte != (int8_t)(value >> 16)) __builtin_trap();

    int8_t fourth_lowest_int_byte = M_ByteInt3(value);
    if (fourth_lowest_int_byte != (int8_t)(value >> 24)) __builtin_trap();

    int8_t fifth_lowest_int_byte = M_ByteInt4(value);
    if (fifth_lowest_int_byte != (int8_t)(value >> 32)) __builtin_trap();

    int8_t sixth_lowest_int_byte = M_ByteInt5(value);
    if (sixth_lowest_int_byte != (int8_t)(value >> 40)) __builtin_trap();

    int8_t seventh_lowest_int_byte = M_ByteInt6(value);
    if (seventh_lowest_int_byte != (int8_t)(value >> 48)) __builtin_trap();

    int8_t highest_int_byte = M_ByteInt7(value);
    if (highest_int_byte != (int8_t)(value >> 56)) __builtin_trap();

    uint8_t lowest_uint_nibble = M_Nibble0(value);
    if (lowest_uint_nibble != (uint8_t)(value & 0x0F)) __builtin_trap();

    uint8_t second_lowest_uint_nibble = M_Nibble1(value);
    if (second_lowest_uint_nibble != (uint8_t)((value >> 4) & 0x0F)) __builtin_trap();

    uint8_t third_lowest_uint_nibble = M_Nibble2(value);
    if (third_lowest_uint_nibble != (uint8_t)((value >> 8) & 0x0F)) __builtin_trap();

    uint8_t fourth_lowest_uint_nibble = M_Nibble3(value);
    if (fourth_lowest_uint_nibble != (uint8_t)((value >> 12) & 0x0F)) __builtin_trap();

    uint8_t fifth_lowest_uint_nibble = M_Nibble4(value);
    if (fifth_lowest_uint_nibble != (uint8_t)((value >> 16) & 0x0F)) __builtin_trap();

    uint8_t sixth_lowest_uint_nibble = M_Nibble5(value);
    if (sixth_lowest_uint_nibble != (uint8_t)((value >> 20) & 0x0F)) __builtin_trap();

    uint8_t seventh_lowest_uint_nibble = M_Nibble6(value);
    if (seventh_lowest_uint_nibble != (uint8_t)((value >> 24) & 0x0F)) __builtin_trap();

    uint8_t eighth_lowest_uint_nibble = M_Nibble7(value);
    if (eighth_lowest_uint_nibble != (uint8_t)((value >> 28) & 0x0F)) __builtin_trap();

    uint8_t ninth_lowest_uint_nibble = M_Nibble8(value);
    if (ninth_lowest_uint_nibble != (uint8_t)((value >> 32) & 0x0F)) __builtin_trap();

    uint8_t tenth_lowest_uint_nibble = M_Nibble9(value);
    if (tenth_lowest_uint_nibble != (uint8_t)((value >> 36) & 0x0F)) __builtin_trap();

    uint8_t eleventh_lowest_uint_nibble = M_Nibble10(value);
    if (eleventh_lowest_uint_nibble != (uint8_t)((value >> 40) & 0x0F)) __builtin_trap();

    uint8_t twelfth_lowest_uint_nibble = M_Nibble11(value);
    if (twelfth_lowest_uint_nibble != (uint8_t)((value >> 44) & 0x0F)) __builtin_trap();

    uint8_t thirteenth_lowest_uint_nibble = M_Nibble12(value);
    if (thirteenth_lowest_uint_nibble != (uint8_t)((value >> 48) & 0x0F)) __builtin_trap();

    uint8_t fourteenth_lowest_uint_nibble = M_Nibble13(value);
    if (fourteenth_lowest_uint_nibble != (uint8_t)((value >> 52) & 0x0F)) __builtin_trap();

    uint8_t fifteenth_lowest_uint_nibble = M_Nibble14(value);
    if (fifteenth_lowest_uint_nibble != (uint8_t)((value >> 56) & 0x0F)) __builtin_trap();

    uint8_t highest_uint_nibble = M_Nibble15(value);
    if (highest_uint_nibble != (uint8_t)((value >> 60) & 0x0F)) __builtin_trap();

    uint64_t b_swap_64_result = b_swap_64(value);
    if (b_swap_64_result != (((value & UINT64_C(0xFF00000000000000)) >> 56) |
                         ((value & UINT64_C(0x00FF000000000000)) >> 40) |
                         ((value & UINT64_C(0x0000FF0000000000)) >> 24) |
                         ((value & UINT64_C(0x000000FF00000000)) >> 8)  |
                         ((value & UINT64_C(0x00000000FF000000)) << 8)  |
                         ((value & UINT64_C(0x0000000000FF0000)) << 24) |
                         ((value & UINT64_C(0x000000000000FF00)) << 40) |
                         ((value & UINT64_C(0x00000000000000FF)) << 56))) {
        __builtin_trap();
    }

    uint64_t byte_Swap_64_result = value;
    byte_Swap_64(&byte_Swap_64_result);
    if (byte_Swap_64_result != b_swap_64_result) __builtin_trap();

    uint64_t word_Swap_64_result = value;
    word_Swap_64(&word_Swap_64_result);
    if (word_Swap_64_result != (((value & UINT64_C(0x000000000000FFFF)) << 48) |
        ((value & UINT64_C(0x00000000FFFF0000)) << 16) |
        ((value & UINT64_C(0x0000FFFF00000000)) >> 16) |
        ((value & UINT64_C(0xFFFF000000000000)) >> 48))) {
        __builtin_trap();
    }

    uint64_t double_Word_Swap_64_result = value;
    double_Word_Swap_64(&double_Word_Swap_64_result);
    if (double_Word_Swap_64_result != (((value & UINT64_C(0x00000000FFFFFFFF)) << 32) |
        ((value & UINT64_C(0xFFFFFFFF00000000)) >> 32))) {
        __builtin_trap();
    }

    // Fuzzing be64_to_host
    uint64_t be64_to_host_result = be64_to_host(value);
    #if defined(ENV_BIG_ENDIAN)
        if (be64_to_host_result != value) __builtin_trap();
    #else
        if (be64_to_host_result != b_swap_64(value)) __builtin_trap();
    #endif

    // Fuzzing host_to_be64
    uint64_t host_to_be64_result = host_to_be64(value);
    #if defined(ENV_BIG_ENDIAN)
        if (host_to_be64_result != value) __builtin_trap();
    #else
        if (host_to_be64_result != b_swap_64(value)) __builtin_trap();
    #endif    

    return 0;
}