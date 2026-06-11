#include <cstddef>
#include <cstdint>
#include <cstring>
#include "bit_manip.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 4) return 0;

    genericint_t in{};
    in.issigned = (data[0] & 1) != 0;

    static const size_t widths[] = { sizeof(uint8_t), sizeof(uint16_t), sizeof(uint32_t), sizeof(uint64_t) };
    in.sizeoftype = widths[data[1] % 4];

    uint64_t raw = 0;
    size_t copy_n = (size - 2) < sizeof(raw) ? (size - 2) : sizeof(raw);
    memcpy(&raw, data + 2, copy_n);

    switch (in.sizeoftype) {
        case sizeof(uint8_t):  in.u8  = static_cast<uint8_t>(raw); break;
        case sizeof(uint16_t): in.u16 = static_cast<uint16_t>(raw); break;
        case sizeof(uint32_t): in.u32 = static_cast<uint32_t>(raw); break;
        case sizeof(uint64_t): in.u64 = raw; break;
    }

    if (size < copy_n + 4) return 0;
    uint8_t msb = data[size - 2];
    uint8_t lsb = data[size - 1];

    if (lsb > msb) {
        uint8_t tmp = lsb;
        lsb = msb;
        msb = tmp;
    }

    size_t outsz = widths[(data[0] >> 1) % 4];

    if (!is_generic_int_valid(in)) return 0;

    genericint_t result = generic_Get_Bit_Range(in, outsz, msb, lsb);

    (void)generic_Get_Bit_Range(result, outsz, msb, lsb);

    return 0;
}
