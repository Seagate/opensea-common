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

    // Fuzzing is_generic_int_valid
    if (!is_generic_int_valid(in)) {
        __builtin_trap();
    }

    msb = msb % (in.sizeoftype * 8);
    lsb = lsb % (in.sizeoftype * 8);
    if (lsb > msb) {
        uint8_t temp = lsb;
        lsb = msb;
        msb = temp;
    }

    // Fuzzing generic_Get_Bit_Range
    genericint_t generic_Get_Bit_Range_result = generic_Get_Bit_Range(in, outsz, msb, lsb);
    
    // Extract the exact input integer configuration as a baseline uint64_t
    uint64_t oracle_input_val = 0;
    switch (in.sizeoftype) {
        case sizeof(uint8_t):  oracle_input_val = in.u8;  break;
        case sizeof(uint16_t): oracle_input_val = in.u16; break;
        case sizeof(uint32_t): oracle_input_val = in.u32; break;
        case sizeof(uint64_t): oracle_input_val = in.u64; break;
    }

    // Slice bits out one by one manually using a clean, unoptimized loop
    uint64_t generic_reference = 0;
    size_t bit_counter = 0;
    for (uint8_t i = lsb; i <= msb; i++) {
        uint64_t bit = (oracle_input_val >> i) & 1;
        generic_reference |= (bit << bit_counter);
        bit_counter++;
    }

    // Truncate the reference value to match the requested output container width
    if (outsz < sizeof(uint64_t)) {
        uint64_t size_mask = ((uint64_t)1 << (outsz * 8)) - 1;
        generic_reference &= size_mask;
    } else {
        // If outsz is 8 bytes, no masking is required (full 64-bit capacity)
        generic_reference &= UINT64_MAX;
    }

    // Unpack what your function returned
    uint64_t generic_function_val = 0;
    switch (outsz) {
        case sizeof(uint8_t):  generic_function_val = generic_Get_Bit_Range_result.u8;  break;
        case sizeof(uint16_t): generic_function_val = generic_Get_Bit_Range_result.u16; break;
        case sizeof(uint32_t): generic_function_val = generic_Get_Bit_Range_result.u32; break;
        case sizeof(uint64_t): generic_function_val = generic_Get_Bit_Range_result.u64; break;
    }

    if (generic_function_val != generic_reference) {
        __builtin_trap();
    }

    // Fuzzing the wrapper functions for specific input/output types
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

    // Fuzzing M_BitN
    uint8_t raw_bit_position = data[0]; 

    volatile uint64_t M_BitN_result = M_BitN(raw_bit_position);
    (void)M_BitN_result;

    // Fuzzing M_BitN8
    volatile uint8_t M_BitN8_result = M_BitN8(raw_bit_position);
    (void)M_BitN8_result;

    // Fuzzing M_BitN16
    volatile uint16_t M_BitN16_result = M_BitN16(raw_bit_position);
    (void)M_BitN16_result;

    // Fuzzing M_BitN32
    volatile uint32_t M_BitN32_result = M_BitN32(raw_bit_position);
    (void)M_BitN32_result;

    // Fuzzing M_BitN64
    volatile uint64_t M_BitN64_result = M_BitN64(raw_bit_position);
    (void)M_BitN64_result;

    return 0;
}