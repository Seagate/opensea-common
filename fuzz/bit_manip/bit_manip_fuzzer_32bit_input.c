#include <stdint.h>
#include <stddef.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 2 * sizeof(uint16_t)) {
        return 0;
    }

    uint16_t val1 = 0;
    for (size_t i = 0; i < sizeof(uint16_t); i++) {
        val1 |= ((uint16_t)data[i]) << (8 * i);
    }

    uint16_t val2 = 0;
    for (size_t i = 0; i < sizeof(uint16_t); i++) {
        val2 |= ((uint16_t)data[i + sizeof(uint16_t)]) << (8 * i);
    }

    uint16_t lowest_16bits = get_Word0_uint32(val1);
    if (lowest_16bits != (uint16_t)(val1 & UINT32_C(0x0000FFFF))) __builtin_trap();

    uint16_t highest_16bits = get_Word1_uint32(val2);
    if (highest_16bits != (uint16_t)((val2 & UINT32_C(0xFFFF0000)) >> 16)) __builtin_trap();

    uint64_t dwords_To_Uint64_result = dwords_To_Uint64(val1, val2);
    if (dwords_To_Uint64_result != ((M_STATIC_CAST(uint64_t, val1) << 32) | (M_STATIC_CAST(uint64_t, val2) << 0))) __builtin_trap();

    return 0;
}
