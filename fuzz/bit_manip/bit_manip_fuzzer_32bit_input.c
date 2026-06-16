#include <stdint.h>
#include <stddef.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < sizeof(uint32_t)) {
        return 0;
    }

    uint32_t value = 0;
    for (size_t i = 0; i < sizeof(uint32_t); i++) {
        value |= ((uint32_t)data[i]) << (8 * i);
    }

    uint16_t lowest_16bits = get_Word0_uint32(value);
    uint16_t highest_16bits = get_Word1_uint32(value);

    return 0;
}
