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
    uint32_t upper_uint32bit = get_DWord1(value);

    int32_t lower_int32bit = M_DoubleWordInt0(value);
    int32_t upper_int32bit = M_DoubleWordInt1(value);

    uint16_t lower_uint16bit = get_Word0_uint64(value);
    uint16_t second_lower_uint16bit = get_Word1_uint64(value);
    uint16_t second_highest_uint16bit = get_Word2_uint64(value);
    uint16_t highest_uint16bit = get_Word3_uint64(value);

    int16_t lower_int16bit = M_WordInt0(value);
    int16_t second_lower_int16bit = M_WordInt1(value);
    int16_t second_highest_int16bit = M_WordInt2(value);
    int16_t highest_int16bit = M_WordInt3(value);

    uint8_t lowest_uint_byte = M_Byte0(value);
    uint8_t second_lowest_uint_byte = M_Byte1(value);
    uint8_t third_lowest_uint_byte = M_Byte2(value);
    uint8_t fourth_lowest_uint_byte = M_Byte3(value);
    uint8_t fifth_lowest_uint_byte = M_Byte4(value);
    uint8_t sixth_lowest_uint_byte = M_Byte5(value);
    uint8_t seventh_lowest_uint_byte = M_Byte6(value);
    uint8_t highest_uint_byte = M_Byte7(value);

    int8_t lowest_int_byte = M_ByteInt0(value);
    int8_t second_lowest_int_byte = M_ByteInt1(value);
    int8_t third_lowest_int_byte = M_ByteInt2(value);
    int8_t fourth_lowest_int_byte = M_ByteInt3(value);
    int8_t fifth_lowest_int_byte = M_ByteInt4(value);
    int8_t sixth_lowest_int_byte = M_ByteInt5(value);
    int8_t seventh_lowest_int_byte = M_ByteInt6(value);
    int8_t highest_int_byte = M_ByteInt7(value);

    uint8_t lowest_uint_nibble = M_Nibble0(value);
    uint8_t second_lowest_uint_nibble = M_Nibble1(value);
    uint8_t third_lowest_uint_nibble = M_Nibble2(value);
    uint8_t fourth_lowest_uint_nibble = M_Nibble3(value);
    uint8_t fifth_lowest_uint_nibble = M_Nibble4(value);
    uint8_t sixth_lowest_uint_nibble = M_Nibble5(value);
    uint8_t seventh_lowest_uint_nibble = M_Nibble6(value);
    uint8_t eighth_lowest_uint_nibble = M_Nibble7(value);
    uint8_t ninth_lowest_uint_nibble = M_Nibble8(value);
    uint8_t tenth_lowest_uint_nibble = M_Nibble9(value);
    uint8_t eleventh_lowest_uint_nibble = M_Nibble10(value);
    uint8_t twelfth_lowest_uint_nibble = M_Nibble11(value);
    uint8_t thirteenth_lowest_uint_nibble = M_Nibble12(value);
    uint8_t fourteenth_lowest_uint_nibble = M_Nibble13(value);
    uint8_t fifteenth_lowest_uint_nibble = M_Nibble14(value);
    uint8_t highest_uint_nibble = M_Nibble15(value);

    if (lower_uint32bit != (uint32_t)value || upper_uint32bit != (uint32_t)(value >> 32) ||
    lower_int32bit != (int32_t)value || upper_int32bit != (int32_t)(value >> 32) ||
    lower_uint16bit != (uint16_t)(value & UINT64_C(0x000000000000FFFF)) || second_lower_uint16bit !=
    (uint16_t)((value & UINT64_C(0x00000000FFFF0000)) >> 16) || second_highest_uint16bit !=
    (uint16_t)((value & UINT64_C(0x0000FFFF00000000)) >> 32) || highest_uint16bit !=
    (uint16_t)((value & UINT64_C(0xFFFF000000000000)) >> 48) || lower_int16bit !=
    (int16_t)(value & UINT64_C(0x000000000000FFFF)) || second_lower_int16bit !=
    (int16_t)((value & UINT64_C(0x00000000FFFF0000)) >> 16) || second_highest_int16bit !=
    (int16_t)((value & UINT64_C(0x0000FFFF00000000)) >> 32) || highest_int16bit !=
    (int16_t)((value & UINT64_C(0xFFFF000000000000)) >> 48) || lowest_uint_byte !=
    (uint8_t)value || second_lowest_uint_byte != (uint8_t)(value >> 8) || third_lowest_uint_byte !=
    (uint8_t)(value >> 16) || fourth_lowest_uint_byte != (uint8_t)(value >> 24) ||
    fifth_lowest_uint_byte != (uint8_t)(value >> 32) || sixth_lowest_uint_byte !=
    (uint8_t)(value >> 40) || seventh_lowest_uint_byte != (uint8_t)(value >> 48) ||
    highest_uint_byte != (uint8_t)(value >> 56) || lowest_int_byte !=
    (int8_t)value || second_lowest_int_byte != (int8_t)(value >> 8) || third_lowest_int_byte !=
    (int8_t)(value >> 16) || fourth_lowest_int_byte != (int8_t)(value >> 24) ||
    fifth_lowest_int_byte != (int8_t)(value >> 32) || sixth_lowest_int_byte !=
    (int8_t)(value >> 40) || seventh_lowest_int_byte != (int8_t)(value >> 48) ||
    highest_int_byte != (int8_t)(value >> 56) || lowest_uint_nibble != (uint8_t)(value & 0x0F) ||
    second_lowest_uint_nibble != (uint8_t)((value >> 4) & 0x0F) || third_lowest_uint_nibble !=
    (uint8_t)((value >> 8) & 0x0F) || fourth_lowest_uint_nibble != (uint8_t)((value >> 12) & 0x0F) ||
    fifth_lowest_uint_nibble != (uint8_t)((value >> 16) & 0x0F) || sixth_lowest_uint_nibble !=
    (uint8_t)((value >> 20) & 0x0F) || seventh_lowest_uint_nibble != (uint8_t)((value >> 24) & 0x0F) ||
    eighth_lowest_uint_nibble != (uint8_t)((value >> 28) & 0x0F) || ninth_lowest_uint_nibble !=
    (uint8_t)((value >> 32) & 0x0F) || tenth_lowest_uint_nibble != (uint8_t)((value >> 36) & 0x0F) ||
    eleventh_lowest_uint_nibble != (uint8_t)((value >> 40) & 0x0F) || twelfth_lowest_uint_nibble !=
    (uint8_t)((value >> 44) & 0x0F) || thirteenth_lowest_uint_nibble != (uint8_t)((value >> 48) & 0x0F)
    || fourteenth_lowest_uint_nibble != (uint8_t)((value >> 52) & 0x0F) || fifteenth_lowest_uint_nibble
    != (uint8_t)((value >> 56) & 0x0F) || highest_uint_nibble != (uint8_t)((value >> 60) & 0x0F)) {
        __builtin_trap();
    }

    return 0;
}
