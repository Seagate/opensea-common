#include <stdint.h>
#include <stddef.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < (4 * sizeof(uint16_t))) {
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

    return 0;
}