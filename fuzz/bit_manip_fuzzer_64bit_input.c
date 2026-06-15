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

    if (lower_uint32bit != (uint32_t)value || upper_uint32bit != (uint32_t)(value >> 32) ||
    lower_int32bit != (int32_t)value || upper_int32bit != (int32_t)(value >> 32) ||
    lower_uint16bit != (value & UINT64_C(0x000000000000FFFF)) || second_lower_uint16bit !=
    ((value & UINT64_C(0x00000000FFFF0000)) >> 16) || second_highest_uint16bit !=
    ((value & UINT64_C(0x0000FFFF00000000)) >> 32) || highest_uint16bit !=
    ((value & UINT64_C(0xFFFF000000000000)) >> 48)) {
        __builtin_trap();
    }

    return 0;
}
