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

    uint32_t lower_32bit = get_DWord0(value);
    uint32_t upper_32bit = get_DWord1(value);

    if (lower_32bit != (uint32_t)value || upper_32bit != (uint32_t)(value >> 32)) {
        __builtin_trap();
    }

    return 0;
}
