#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 4) return 0;

    genericint_t in = {0};
    in.issigned = (data[0] & 1) != 0;

    static const size_t widths[] = { sizeof(uint8_t), sizeof(uint16_t), sizeof(uint32_t), sizeof(uint64_t) };
    in.sizeoftype = widths[data[1] % 4];

    uint8_t msb = data[size - 2];
    uint8_t lsb = data[size - 1];
    if (lsb > msb) {
        uint8_t temp = lsb;
        lsb = msb;
        msb = temp;
    }

    size_t payload_size = size - 4; 
    uint64_t raw = 0;
    size_t copy_n = (payload_size < sizeof(raw)) ? payload_size : sizeof(raw);
    
    if (copy_n > 0) {
        memcpy(&raw, data + 2, copy_n);
    }

    switch (in.sizeoftype) {
        case sizeof(uint8_t):  in.u8  = (uint8_t)raw; break;
        case sizeof(uint16_t): in.u16 = (uint16_t)raw; break;
        case sizeof(uint32_t): in.u32 = (uint32_t)raw; break;
        case sizeof(uint64_t): in.u64 = raw; break;
    }

    size_t outsz = widths[(data[0] >> 1) % 4];

    if (!is_generic_int_valid(in)) return 0;

    msb = msb % (in.sizeoftype * 8);
    lsb = lsb % (in.sizeoftype * 8);
    if (lsb > msb) {
        uint8_t temp = lsb;
        lsb = msb;
        msb = temp;
    }

    genericint_t result = generic_Get_Bit_Range(in, outsz, msb, lsb);
    (void)result;

    return 0;
}