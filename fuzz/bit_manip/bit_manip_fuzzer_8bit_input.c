#include <stdint.h>
#include <stddef.h>
#include "bit_manip.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 16) {
        return 0;
    }

    uint8_t upperNibble = data[0];
    uint8_t lowerNibble = data[1];

    uint8_t msb16 = data[2];
    uint8_t lsb16 = data[3];

    uint8_t msb32 = data[4];
    uint8_t byte2_32 = data[5];
    uint8_t byte1_32 = data[6];
    uint8_t lsb32 = data[7];

    uint8_t msb64 = data[8];
    uint8_t byte6_64 = data[9];
    uint8_t byte5_64 = data[10];
    uint8_t byte4_64 = data[11];
    uint8_t byte3_64 = data[12];
    uint8_t byte2_64 = data[13];
    uint8_t byte1_64 = data[14];
    uint8_t lsb64 = data[15];

    uint8_t nibbles_To_Byte_result = nibbles_To_Byte(upperNibble, lowerNibble);
    if (nibbles_To_Byte_result != ((((upperNibble)&M_STATIC_CAST(uint8_t, 0x0F)) << 4) |(((lowerNibble)&M_STATIC_CAST(uint8_t, 0x0F)) << 0))) __builtin_trap();

    uint16_t bytes_To_Uint16_result = bytes_To_Uint16(msb16, lsb16);
    if (bytes_To_Uint16_result != ((M_STATIC_CAST(uint16_t, msb16) << 8) | (M_STATIC_CAST(uint16_t, lsb16) << 0))) __builtin_trap();

    uint32_t bytes_To_Uint32_result = bytes_To_Uint32(msb32, byte2_32, byte1_32, lsb32);
    if (bytes_To_Uint32_result != ((M_STATIC_CAST(uint32_t, msb32) << 24) | (M_STATIC_CAST(uint32_t, byte2_32) << 16) | (M_STATIC_CAST(uint32_t, byte1_32) << 8) | (M_STATIC_CAST(uint32_t, lsb32) << 0))) __builtin_trap();

    uint64_t bytes_To_Uint64_result = bytes_To_Uint64(msb64, byte6_64, byte5_64, byte4_64, byte3_64, byte2_64, byte1_64, lsb64);
    if (bytes_To_Uint64_result != ((M_STATIC_CAST(uint64_t, msb64) << 56) | (M_STATIC_CAST(uint64_t, byte6_64) << 48) | (M_STATIC_CAST(uint64_t, byte5_64) << 40) | (M_STATIC_CAST(uint64_t, byte4_64) << 32) | (M_STATIC_CAST(uint64_t, byte3_64) << 24) | (M_STATIC_CAST(uint64_t, byte2_64) << 16) | (M_STATIC_CAST(uint64_t, byte1_64) << 8) | (M_STATIC_CAST(uint64_t, lsb64) << 0))) __builtin_trap();

    uint8_t n_swap_8_result = n_swap_8(byte1_32);
    if (n_swap_8_result != ((M_STATIC_CAST(uint8_t, ((byte1_32 & UINT8_C(0x0F)) << 4))) | (M_STATIC_CAST(uint8_t, ((byte1_32 & UINT8_C(0xF0)) >> 4))))) __builtin_trap();

    uint8_t nibble_Swap_result = byte1_32;
    nibble_Swap(&nibble_Swap_result);
    if (nibble_Swap_result != n_swap_8_result) __builtin_trap();

    // Fuzzing get_Bytes_To_16
    size_t fullDataLen = data[0] % 16;
    size_t msb = data[1];
    size_t lsb = data[2];

    uint8_t buffer[16] = {0};
    if (fullDataLen > 0) {
        size_t copyLen = (fullDataLen < size - 3) ? fullDataLen : size - 3;
        memcpy(buffer, data + 3, copyLen);
    }

    uint16_t outVal = 0xDEAD;
    bool result = get_Bytes_To_16(buffer, fullDataLen, msb, lsb, &outVal);

    bool valid = (fullDataLen > 0 &&
                  msb < fullDataLen &&
                  lsb < fullDataLen &&
                  &outVal != NULL);

    if (valid) {
        uint16_t expected;
        if (msb > lsb) {
            expected = ((uint16_t)buffer[msb] << 8) | buffer[lsb];
        } else {
            expected = ((uint16_t)buffer[lsb] << 8) | buffer[msb];
        }
        if (!result || outVal != expected) {
            fprintf(stderr, "Mismatch: msb=%zu lsb=%zu got=%04x expected=%04x\n",
                    msb, lsb, outVal, expected);
            __builtin_trap();
        }
    } else {
        if (result) {
            fprintf(stderr, "Function returned true for invalid input!\n");
            __builtin_trap();
        }
    }

    return 0;
}
