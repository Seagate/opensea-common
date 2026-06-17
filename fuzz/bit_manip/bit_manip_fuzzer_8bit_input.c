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
    (void)nibbles_To_Byte_result;

    uint16_t bytes_To_Uint16_result = bytes_To_Uint16(msb16, lsb16);
    (void)bytes_To_Uint16_result;

    uint32_t bytes_To_Uint32_result = bytes_To_Uint32(msb32, byte2_32, byte1_32, lsb32);
    (void)bytes_To_Uint32_result;

    uint64_t bytes_To_Uint64_result = bytes_To_Uint64(msb64, byte6_64, byte5_64, byte4_64, byte3_64, byte2_64, byte1_64, lsb64);
    (void)bytes_To_Uint64_result;

    return 0;
}
