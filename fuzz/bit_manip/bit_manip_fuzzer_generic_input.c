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

    // Fuzzing is_generic_int_valid
    bool is_generic_int_valid_result = is_generic_int_valid(in);
    (void)is_generic_int_valid_result;

    // Fuzzing generic_Get_Bit_Range
    genericint_t generic_Get_Bit_Range_result = generic_Get_Bit_Range(in, outsz, msb, lsb);
    (void)generic_Get_Bit_Range_result;

    if (!in.issigned && in.sizeoftype == sizeof(uint8_t) && outsz == sizeof(uint8_t)) {
        uint8_t wrapper_result = get_bit_range_uint8(in.u8, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u8) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint16_t) && outsz == sizeof(uint16_t)) {
        uint16_t wrapper_result = get_bit_range_uint16(in.u16, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u16) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint16_t) && outsz == sizeof(uint8_t)) {
        uint8_t wrapper_result = get_8bit_range_uint16(in.u16, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u8) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint32_t) && outsz == sizeof(uint32_t)) {
        uint32_t wrapper_result = get_bit_range_uint32(in.u32, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u32) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint32_t) && outsz == sizeof(uint8_t)) {
        uint8_t wrapper_result = get_8bit_range_uint32(in.u32, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u8) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint32_t) && outsz == sizeof(uint16_t)) {
        uint16_t wrapper_result = get_16bit_range_uint32(in.u32, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u16) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint64_t) && outsz == sizeof(uint64_t)) {
        uint64_t wrapper_result = get_bit_range_uint64(in.u64, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u64) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint64_t) && outsz == sizeof(uint8_t)) {
        uint8_t wrapper_result = get_8bit_range_uint64(in.u64, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u8) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint64_t) && outsz == sizeof(uint16_t)) {
        uint16_t wrapper_result = get_16bit_range_uint64(in.u64, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u16) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint64_t) && outsz == sizeof(uint32_t)) {
        uint32_t wrapper_result = get_32bit_range_uint64(in.u64, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u32) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint8_t) && outsz == sizeof(uint8_t)) {
        uint8_t wrapper_result = get_bit_range_int8(in.u8, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u8) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint16_t) && outsz == sizeof(uint16_t)) {
        uint16_t wrapper_result = get_bit_range_int16(in.u16, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u16) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint16_t) && outsz == sizeof(uint8_t)) {
        uint8_t wrapper_result = get_8bit_range_int16(in.u16, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u8) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint32_t) && outsz == sizeof(uint32_t)) {
        uint32_t wrapper_result = get_bit_range_int32(in.u32, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u32) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint32_t) && outsz == sizeof(uint8_t)) {
        uint8_t wrapper_result = get_8bit_range_int32(in.u32, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u8) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint32_t) && outsz == sizeof(uint16_t)) {
        uint16_t wrapper_result = get_16bit_range_int32(in.u32, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u16) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint64_t) && outsz == sizeof(uint64_t)) {
        uint64_t wrapper_result = get_bit_range_int64(in.u64, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u64) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint64_t) && outsz == sizeof(uint8_t)) {
        uint8_t wrapper_result = get_8bit_range_int64(in.u64, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u8) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint64_t) && outsz == sizeof(uint16_t)) {
        uint16_t wrapper_result = get_16bit_range_int64(in.u64, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u16) {
            __builtin_trap();
        }
    }

    if (!in.issigned && in.sizeoftype == sizeof(uint64_t) && outsz == sizeof(uint32_t)) {
        uint32_t wrapper_result = get_32bit_range_int64(in.u64, msb, lsb);
        
        if (wrapper_result != generic_Get_Bit_Range_result.u32) {
            __builtin_trap();
        }
    }

    // Fuzzing macro M_GETBITRANGE
    uint64_t M_GETBITRANGE_input = 0;
    switch (in.sizeoftype) {
        case sizeof(uint8_t):  M_GETBITRANGE_input = in.u8;  break;
        case sizeof(uint16_t): M_GETBITRANGE_input = in.u16; break;
        case sizeof(uint32_t): M_GETBITRANGE_input = in.u32; break;
        case sizeof(uint64_t): M_GETBITRANGE_input = in.u64; break;
    }

    volatile uint64_t M_GETBITRANGE_result = M_GETBITRANGE(M_GETBITRANGE_input, msb, lsb);
    (void)M_GETBITRANGE_result;

    // Fuzzing M_IGETBITRANGE
    int64_t signed_macro_input = (int64_t)M_GETBITRANGE_input;

    volatile int64_t M_IGETBITRANGE_result = M_IGETBITRANGE(signed_macro_input, msb, lsb);
    (void)M_IGETBITRANGE_result;

    return 0;
}