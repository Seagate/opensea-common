#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 8) return 0;

    float f;
    float c;
    
    memcpy(&f, data, 4);
    memcpy(&c, data + 4, 4);

    if (!isfinite(c) || c == 0.0f) return 0;
    if (!isfinite(f)) return 0;

    c = fabsf(c);

    if (f > 100000.0f || f < -100000.0f || c > 1000.0f || c < 0.001f) return 0;

    float result = ROUNDF(f, c);

    float max_allowed_distance = (0.5f / c) + 1e-4f;
    float actual_distance = fabsf(f - result);
    
    if (actual_distance > max_allowed_distance) {
        __builtin_trap();
    }

    float double_round_result = ROUNDF(result, c);
    if (result != double_round_result) {
        __builtin_trap();
    }

    return 0;
}