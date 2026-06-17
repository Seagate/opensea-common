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

    if (f > 100000.0f || f < -100000.0f || c > 1000.0f) return 0;

    float result = ROUNDF(f, c);

    float max_allowed_distance = 1.0f / c;
    float actual_distance = fabsf(f - result);
    
    if (actual_distance > max_allowed_distance) {
        __builtin_trap(); // The macro rounded completely out of bounds!
    }

    if (fabsf(result) > fabsf(f) + 1e-5f) {
        __builtin_trap(); // The macro incorrectly rounded UP away from zero!
    }

    return 0;
}