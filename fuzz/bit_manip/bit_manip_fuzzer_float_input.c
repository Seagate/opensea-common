#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size);

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 8) return 0;

    float f;
    float c;
    
    memcpy(&f, data, 4);
    memcpy(&c, data + 4, 4);

    if (!isfinite(c) || c == 0.0f) return 0;
    if (!isfinite(f)) return 0;

    volatile float result = ROUNDF(f, c);
    (void)result;

    return 0;
}
