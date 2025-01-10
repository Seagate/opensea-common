// SPDX-License-Identifier: MPL-2.0

//! \file bit_manip.h
//! \brief Implements various functions and macros to manipulate bit fields,
//! manipulate bytes, swap bytes, etc
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "code_attributes.h"
#include "common_types.h"
#include "env_detect.h"
#include "memory_safety.h"
#include "type_conversion.h"

#if IS_MSVC_VERSION(MSVC_2005)
#    include <stdlib.h>
#    if !defined(HAVE_WIN_BSWAP)
#        define HAVE_WIN_BSWAP
#    endif //! HAVE_WIN_BSWAP
#endif     //_MSC_VER

#if defined(__has_include)
#    if __has_include(<stdbit.h>)
#        include <stdbit.h>
#        define HAVE_STDC_BIT
#    endif //__has_include(<stdbit.h>)
#endif     //__has_include

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \fn static M_INLINE uint32_t get_DWord0(uint64_t value)
    //! \brief Returns the lower 32bits in a 64bit value
    //!
    //! \param[in] value 64bit value to get the lower 32bits from
    //! \return lower 32bits of the input 64bit value
    static M_INLINE uint32_t get_DWord0(uint64_t value)
    {
        return M_STATIC_CAST(uint32_t, value & UINT64_C(0x00000000FFFFFFFF));
    }

    //! \def M_DoubleWord0(l)
    //! \brief Returns the lower 32bits in a 64bit value
    //!
    //! This is a wrapper for get_DWord0 for backwards compatibility.
    //!
    //! \param[in] l 64bit value to get the lower 32bits from
    //! \return lower 32bits of the input 64bit value
    //! \sa get_DWord0
#define M_DoubleWord0(l) get_DWord0(l)

    //! \fn static M_INLINE uint32_t get_DWord1(uint64_t value)
    //! \brief Returns the upper 32bits in a 64bit value
    //!
    //! \param[in] value 64bit value to get the upper 32bits from
    //! \return upper 32bits of the input 64bit value
    static M_INLINE uint32_t get_DWord1(uint64_t value)
    {
        return M_STATIC_CAST(uint32_t, (value & UINT64_C(0xFFFFFFFF00000000)) >> 32);
    }

    //! \def M_DoubleWord1(l)
    //! \brief Returns the upper 32bits in a 64bit value
    //!
    //! This is a wrapper for get_DWord1 for backwards compatibility.
    //!
    //! \param[in] l 64bit value to get the upper 32bits from
    //! \return upper 32bits of the input 64bit value
    //! \sa get_DWord1
#define M_DoubleWord1(l) get_DWord1(l)

//! \def M_DoubleWordInt0
//! \brief Extracts the lower 32 bits from a 64-bit integer and casts it to int32_t.
//!
//! This macro extracts the lower 32 bits from a 64-bit integer and casts it to int32_t.
//! \param l The 64-bit integer from which to extract the lower 32 bits.
#define M_DoubleWordInt0(l) (M_STATIC_CAST(int32_t, (((l)&UINT64_C(0x00000000FFFFFFFF)) >> 0)))

//! \def M_DoubleWordInt1
//! \brief Extracts the upper 32 bits from a 64-bit integer and casts it to int32_t.
//!
//! This macro extracts the upper 32 bits from a 64-bit integer and casts it to int32_t.
//! \param l The 64-bit integer from which to extract the upper 32 bits.
#define M_DoubleWordInt1(l) (M_STATIC_CAST(int32_t, (((l)&UINT64_C(0xFFFFFFFF00000000)) >> 32)))

//! \def M_Word0
//! \brief Extracts the lowest 16 bits from a 64-bit integer and casts it to uint16_t.
//!
//! This macro extracts the lowest 16 bits from a 64-bit integer and casts it to uint16_t.
//! \param l The 64-bit integer from which to extract the lowest 16 bits.
#define M_Word0(l) (M_STATIC_CAST(uint16_t, (((l)&UINT64_C(0x000000000000FFFF)) >> 0)))

//! \def M_Word1
//! \brief Extracts the second lowest 16 bits from a 64-bit integer and casts it to uint16_t.
//!
//! This macro extracts the second lowest 16 bits from a 64-bit integer and casts it to uint16_t.
//! \param l The 64-bit integer from which to extract the second lowest 16 bits.
#define M_Word1(l) (M_STATIC_CAST(uint16_t, (((l)&UINT64_C(0x00000000FFFF0000)) >> 16)))

//! \def M_Word2
//! \brief Extracts the second highest 16 bits from a 64-bit integer and casts it to uint16_t.
//!
//! This macro extracts the second highest 16 bits from a 64-bit integer and casts it to uint16_t.
//! \param l The 64-bit integer from which to extract the second highest 16 bits.
#define M_Word2(l) (M_STATIC_CAST(uint16_t, (((l)&UINT64_C(0x0000FFFF00000000)) >> 32)))

//! \def M_Word3
//! \brief Extracts the highest 16 bits from a 64-bit integer and casts it to uint16_t.
//!
//! This macro extracts the highest 16 bits from a 64-bit integer and casts it to uint16_t.
//! \param l The 64-bit integer from which to extract the highest 16 bits.
#define M_Word3(l) (M_STATIC_CAST(uint16_t, (((l)&UINT64_C(0xFFFF000000000000)) >> 48)))

//! \def M_WordInt0
//! \brief Extracts the lowest 16 bits from a 64-bit integer and casts it to int16_t.
//!
//! This macro extracts the lowest 16 bits from a 64-bit integer and casts it to int16_t.
//! \param l The 64-bit integer from which to extract the lowest 16 bits.
#define M_WordInt0(l) (M_STATIC_CAST(int16_t, (((l)&UINT64_C(0x000000000000FFFF)) >> 0)))

//! \def M_WordInt1
//! \brief Extracts the second lowest 16 bits from a 64-bit integer and casts it to int16_t.
//!
//! This macro extracts the second lowest 16 bits from a 64-bit integer and casts it to int16_t.
//! \param l The 64-bit integer from which to extract the second lowest 16 bits.
#define M_WordInt1(l) (M_STATIC_CAST(int16_t, (((l)&UINT64_C(0x00000000FFFF0000)) >> 16)))

//! \def M_WordInt2
//! \brief Extracts the second highest 16 bits from a 64-bit integer and casts it to int16_t.
//!
//! This macro extracts the second highest 16 bits from a 64-bit integer and casts it to int16_t.
//! \param l The 64-bit integer from which to extract the second highest 16 bits.
#define M_WordInt2(l) (M_STATIC_CAST(int16_t, (((l)&UINT64_C(0x0000FFFF00000000)) >> 32)))

//! \def M_WordInt3
//! \brief Extracts the highest 16 bits from a 64-bit integer and casts it to int16_t.
//!
//! This macro extracts the highest 16 bits from a 64-bit integer and casts it to int16_t.
//! \param l The 64-bit integer from which to extract the highest 16 bits.
#define M_WordInt3(l) (M_STATIC_CAST(int16_t, (((l)&UINT64_C(0xFFFF000000000000)) >> 48)))

//! \def BITSPERBYTE
//! \brief Defines the number of bits per byte.
//!
//! This macro defines the number of bits per byte as 8.
#define BITSPERBYTE UINT8_C(8)

//! \def M_ByteN
//! \brief Sets the nth byte to all 1s.
//!
//! This macro sets the nth byte to all 1s by shifting the maximum value of a byte (UINT8_MAX) left by n * 8 bits.
//! \param n The byte position to set to all 1s.
#define M_ByteN(n) ((UINT8_MAX << ((n)*BITSPERBYTE)))

//! \def M_Byte0
//! \brief Extracts the lowest byte from a 64-bit integer and casts it to uint8_t.
//!
//! This macro extracts the lowest byte from a 64-bit integer and casts it to uint8_t.
//! \param l The 64-bit integer from which to extract the lowest byte.
#define M_Byte0(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x00000000000000FF)) >> 0)))

//! \def M_Byte1
//! \brief Extracts the second lowest byte from a 64-bit integer and casts it to uint8_t.
//!
//! This macro extracts the second lowest byte from a 64-bit integer and casts it to uint8_t.
//! \param l The 64-bit integer from which to extract the second lowest byte.
#define M_Byte1(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x000000000000FF00)) >> 8)))

//! \def M_Byte2
//! \brief Extracts the third lowest byte from a 64-bit integer and casts it to uint8_t.
//!
//! This macro extracts the third lowest byte from a 64-bit integer and casts it to uint8_t.
//! \param l The 64-bit integer from which to extract the third lowest byte.
#define M_Byte2(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x0000000000FF0000)) >> 16)))

//! \def M_Byte3
//! \brief Extracts the fourth lowest byte from a 64-bit integer and casts it to uint8_t.
//!
//! This macro extracts the fourth lowest byte from a 64-bit integer and casts it to uint8_t.
//! \param l The 64-bit integer from which to extract the fourth lowest byte.
#define M_Byte3(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x00000000FF000000)) >> 24)))

//! \def M_Byte4
//! \brief Extracts the fifth lowest byte from a 64-bit integer and casts it to uint8_t.
//!
//! This macro extracts the fifth lowest byte from a 64-bit integer and casts it to uint8_t.
//! \param l The 64-bit integer from which to extract the fifth lowest byte.
#define M_Byte4(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x000000FF00000000)) >> 32)))

//! \def M_Byte5
//! \brief Extracts the sixth lowest byte from a 64-bit integer and casts it to uint8_t.
//!
//! This macro extracts the sixth lowest byte from a 64-bit integer and casts it to uint8_t.
//! \param l The 64-bit integer from which to extract the sixth lowest byte.
#define M_Byte5(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x0000FF0000000000)) >> 40)))

//! \def M_Byte6
//! \brief Extracts the seventh lowest byte from a 64-bit integer and casts it to uint8_t.
//!
//! This macro extracts the seventh lowest byte from a 64-bit integer and casts it to uint8_t.
//! \param l The 64-bit integer from which to extract the seventh lowest byte.
#define M_Byte6(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0x00FF000000000000)) >> 48)))

//! \def M_Byte7
//! \brief Extracts the highest byte from a 64-bit integer and casts it to uint8_t.
//!
//! This macro extracts the highest byte from a 64-bit integer and casts it to uint8_t.
//! \param l The 64-bit integer from which to extract the highest byte.
#define M_Byte7(l) (M_STATIC_CAST(uint8_t, (((l)&UINT64_C(0xFF00000000000000)) >> 56)))

//! \def M_ByteInt0
//! \brief Extracts the lowest byte from a 64-bit integer and casts it to int8_t.
//!
//! This macro extracts the lowest byte from a 64-bit integer and casts it to int8_t.
//! \param l The 64-bit integer from which to extract the lowest byte.
#define M_ByteInt0(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x00000000000000FF)) >> 0)))

//! \def M_ByteInt1
//! \brief Extracts the second lowest byte from a 64-bit integer and casts it to int8_t.
//!
//! This macro extracts the second lowest byte from a 64-bit integer and casts it to int8_t.
//! \param l The 64-bit integer from which to extract the second lowest byte.
#define M_ByteInt1(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x000000000000FF00)) >> 8)))

//! \def M_ByteInt2
//! \brief Extracts the third lowest byte from a 64-bit integer and casts it to int8_t.
//!
//! This macro extracts the third lowest byte from a 64-bit integer and casts it to int8_t.
//! \param l The 64-bit integer from which to extract the third lowest byte.
#define M_ByteInt2(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x0000000000FF0000)) >> 16)))

//! \def M_ByteInt3
//! \brief Extracts the fourth lowest byte from a 64-bit integer and casts it to int8_t.
//!
//! This macro extracts the fourth lowest byte from a 64-bit integer and casts it to int8_t.
//! \param l The 64-bit integer from which to extract the fourth lowest byte.
#define M_ByteInt3(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x00000000FF000000)) >> 24)))

//! \def M_ByteInt4
//! \brief Extracts the fifth lowest byte from a 64-bit integer and casts it to int8_t.
//!
//! This macro extracts the fifth lowest byte from a 64-bit integer and casts it to int8_t.
//! \param l The 64-bit integer from which to extract the fifth lowest byte.
#define M_ByteInt4(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x000000FF00000000)) >> 32)))

//! \def M_ByteInt5
//! \brief Extracts the sixth lowest byte from a 64-bit integer and casts it to int8_t.
//!
//! This macro extracts the sixth lowest byte from a 64-bit integer and casts it to int8_t.
//! \param l The 64-bit integer from which to extract the sixth lowest byte.
#define M_ByteInt5(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x0000FF0000000000)) >> 40)))

//! \def M_ByteInt6
//! \brief Extracts the seventh lowest byte from a 64-bit integer and casts it to int8_t.
//!
//! This macro extracts the seventh lowest byte from a 64-bit integer and casts it to int8_t.
//! \param l The 64-bit integer from which to extract the seventh lowest byte.
#define M_ByteInt6(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0x00FF000000000000)) >> 48)))

//! \def M_ByteInt7
//! \brief Extracts the highest byte from a 64-bit integer and casts it to int8_t.
//!
//! This macro extracts the highest byte from a 64-bit integer and casts it to int8_t.
//! \param l The 64-bit integer from which to extract the highest byte.
#define M_ByteInt7(l) (M_STATIC_CAST(int8_t, (((l)&UINT64_C(0xFF00000000000000)) >> 56)))

//! \def M_Nibble0
//! \brief Extracts the lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the lowest nibble.
#define M_Nibble0(l) M_STATIC_CAST(uint8_t, M_STATIC_CAST(uint8_t, l) & M_STATIC_CAST(uint8_t, UINT8_C(0x0F)))

//! \def M_Nibble1
//! \brief Extracts the second lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the second lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the second lowest nibble.
#define M_Nibble1(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint8_t, l) & UINT8_C(0xF0)) >> 4))

//! \def M_Nibble2
//! \brief Extracts the third lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the third lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the third lowest nibble.
#define M_Nibble2(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint16_t, l) & UINT16_C(0x0F00)) >> 8))

//! \def M_Nibble3
//! \brief Extracts the fourth lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the fourth lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the fourth lowest nibble.
#define M_Nibble3(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint16_t, l) & UINT16_C(0xF000)) >> 12))

//! \def M_Nibble4
//! \brief Extracts the fifth lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the fifth lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the fifth lowest nibble.
#define M_Nibble4(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint32_t, l) & UINT32_C(0x000F0000)) >> 16))

//! \def M_Nibble5
//! \brief Extracts the sixth lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the sixth lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the sixth lowest nibble.
#define M_Nibble5(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint32_t, l) & UINT32_C(0x00F00000)) >> 20))

//! \def M_Nibble6
//! \brief Extracts the seventh lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the seventh lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the seventh lowest nibble.
#define M_Nibble6(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint32_t, l) & UINT32_C(0x0F000000)) >> 24))

//! \def M_Nibble7
//! \brief Extracts the eighth lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the eighth lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the eighth lowest nibble.
#define M_Nibble7(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint32_t, l) & UINT32_C(0xF0000000)) >> 28))

//! \def M_Nibble8
//! \brief Extracts the ninth lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the ninth lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the ninth lowest nibble.
#define M_Nibble8(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x0000000F00000000)) >> 32))

//! \def M_Nibble9
//! \brief Extracts the tenth lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the tenth lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the tenth lowest nibble.
#define M_Nibble9(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x000000F000000000)) >> 36))

//! \def M_Nibble10
//! \brief Extracts the eleventh lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the eleventh lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the eleventh lowest nibble.
#define M_Nibble10(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x00000F0000000000)) >> 40))

//! \def M_Nibble11
//! \brief Extracts the twelfth lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the twelfth lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the twelfth lowest nibble.
#define M_Nibble11(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x0000F00000000000)) >> 44))

//! \def M_Nibble12
//! \brief Extracts the thirteenth lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the thirteenth lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the thirteenth lowest nibble.
#define M_Nibble12(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x000F000000000000)) >> 48))

//! \def M_Nibble13
//! \brief Extracts the fourteenth lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the fourteenth lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the fourteenth lowest nibble.
#define M_Nibble13(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x00F0000000000000)) >> 52))

//! \def M_Nibble14
//! \brief Extracts the fifteenth lowest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the fifteenth lowest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the fifteenth lowest nibble.
#define M_Nibble14(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0x0F00000000000000)) >> 56))

//! \def M_Nibble15
//! \brief Extracts the highest nibble (4 bits) from a value and casts it to uint8_t.
//!
//! This macro extracts the highest nibble (4 bits) from a value and casts it to uint8_t.
//! \param l The value from which to extract the highest nibble.
#define M_Nibble15(l) M_STATIC_CAST(uint8_t, ((M_STATIC_CAST(uint64_t, l) & UINT64_C(0xF000000000000000)) >> 60))

    //! \fn static M_INLINE uint8_t nibbles_To_Byte(uint8_t upperNibble, uint8_t lowerNibble)
    //! \brief Combines two nibbles into a single byte
    //!
    //! \param[in] upperNibble upper 4 bits
    //! \param[in] lowerNibble lower 4 bits
    //! \return byte
    static M_INLINE uint8_t nibbles_To_Byte(uint8_t upperNibble, uint8_t lowerNibble)
    {
        return M_STATIC_CAST(uint8_t, (((upperNibble)&M_STATIC_CAST(uint8_t, 0x0F)) << 4) |
                                          (((lowerNibble)&M_STATIC_CAST(uint8_t, 0x0F)) << 0));
    }

    //! \def M_NibblesTo1ByteValue(n1, n0)
    //! \brief Combines two nibbles into a single byte
    //!
    //! This is a convenience wrapper for nibbles_To_Byte for backwards compatibility
    //!
    //! \param[in] upperNibble upper 4 bits
    //! \param[in] lowerNibble lower 4 bits
    //! \return byte
    //! \sa nibbles_To_Byte
#define M_NibblesTo1ByteValue(n1, n0) nibbles_To_Byte(n1, n0)

    //! \fn static M_INLINE uint16_t bytes_To_Uint16(uint8_t msb, uint8_t lsb)
    //! \brief Combines two bytes into a word (uint16_t)
    //!
    //! \param[in] msb most significant byte
    //! \param[in] lsb least significant byte
    //! \return word from combined values
    static M_INLINE uint16_t bytes_To_Uint16(uint8_t msb, uint8_t lsb)
    {
        return M_STATIC_CAST(uint16_t, (M_STATIC_CAST(uint16_t, msb) << 8) | (M_STATIC_CAST(uint16_t, lsb) << 0));
    }

    //! \def M_BytesTo2ByteValue(b1, b0)
    //! \brief Combines two bytes into a word (uint16_t)
    //!
    //! This is a convenience macro for bytes_To_Uint16 for backwards compatibility
    //! \param[in] b1 most significant byte
    //! \param[in] b0 least significant byte
    //! \return word from combined values
    //! \sa bytes_To_Uint16
#define M_BytesTo2ByteValue(b1, b0) bytes_To_Uint16(b1, b0)

    //! \fn static M_INLINE uint32_t bytes_To_Uint32(uint8_t msb, uint8_t byte2, uint8_t byte1, uint8_t lsb)
    //! \brief Combines four bytes into a dword (uint32_t)
    //!
    //! \param[in] msb most significant byte (bits 31:24)
    //! \param[in] byte2 next most significant byte (bits 23:16)
    //! \param[in] byte1 next byte (bits 15:8)
    //! \param[in] lsb least significant byte (bits 7:0)
    //! \return dword from combined values
    static M_INLINE uint32_t bytes_To_Uint32(uint8_t msb, uint8_t byte2, uint8_t byte1, uint8_t lsb)
    {
        return (M_STATIC_CAST(uint32_t, msb) << 24) | (M_STATIC_CAST(uint32_t, byte2) << 16) |
               (M_STATIC_CAST(uint32_t, byte1) << 8) | (M_STATIC_CAST(uint32_t, lsb) << 0);
    }

    //! \def M_BytesTo4ByteValue(b3, b2, b1, b0)
    //! \brief Combines four bytes into a dword (uint32_t)
    //!
    //! This is a convenience macro for bytes_To_Uint32 for backwards compatibility
    //!
    //! \param[in] b3 most significant byte (bits 31:24)
    //! \param[in] b2 next most significant byte (bits 23:16)
    //! \param[in] b1 next byte (bits 15:8)
    //! \param[in] b0 least significant byte (bits 7:0)
    //! \return dword from combined values
    //! \sa bytes_To_Uint32
#define M_BytesTo4ByteValue(b3, b2, b1, b0) bytes_To_Uint32(b3, b2, b1, b0)

    //! \fn static M_INLINE uint64_t bytes_To_Uint64(uint8_t msb, uint8_t byte6, uint8_t byte5, uint8_t byte4,
    //!                                                  uint8_t byte2, uint8_t byte3, uint8_t byte1, uint8_t lsb)
    //! \brief Combines eight bytes into a qword (uint64_t)
    //!
    //! \param[in] msb most significant byte (bits 63:56)
    //! \param[in] byte6 (bits 55:48)
    //! \param[in] byte5 (bits 47:40)
    //! \param[in] byte4 (bits 39:32)
    //! \param[in] byte3 (bits 31:24)
    //! \param[in] byte2 (bits 23:16)
    //! \param[in] byte1 (bits 15:8)
    //! \param[in] lsb least significant byte (bits 7:0)
    //! \return qword from combined values
    static M_INLINE uint64_t bytes_To_Uint64(uint8_t msb,
                                             uint8_t byte6,
                                             uint8_t byte5,
                                             uint8_t byte4,
                                             uint8_t byte3,
                                             uint8_t byte2,
                                             uint8_t byte1,
                                             uint8_t lsb)
    {
        return (M_STATIC_CAST(uint64_t, msb) << 56) | (M_STATIC_CAST(uint64_t, byte6) << 48) |
               (M_STATIC_CAST(uint64_t, byte5) << 40) | (M_STATIC_CAST(uint64_t, byte4) << 32) |
               (M_STATIC_CAST(uint64_t, byte3) << 24) | (M_STATIC_CAST(uint64_t, byte2) << 16) |
               (M_STATIC_CAST(uint64_t, byte1) << 8) | (M_STATIC_CAST(uint64_t, lsb) << 0);
    }

    //! \def M_BytesTo8ByteValue(b7, b6, b5, b4, b3, b2, b1, b0)
    //! \brief Combines eight bytes into a qword (uint64_t)
    //!
    //! This is a convenience macro for bytes_To_Uint64 for backwards compatibility
    //!
    //! \param[in] msb most significant byte (bits 63:56)
    //! \param[in] byte6 (bits 55:48)
    //! \param[in] byte5 (bits 47:40)
    //! \param[in] byte4 (bits 39:32)
    //! \param[in] byte3 (bits 31:24)
    //! \param[in] byte2 (bits 23:16)
    //! \param[in] byte1 (bits 15:8)
    //! \param[in] lsb least significant byte (bits 7:0)
    //! \return qword from combined values
    //! \sa bytes_To_Uint64
#define M_BytesTo8ByteValue(b7, b6, b5, b4, b3, b2, b1, b0) bytes_To_Uint64(b7, b6, b5, b4, b3, b2, b1, b0)

    //! \fn static M_INLINE uint32_t words_To_Uint32(uint16_t msw, uint16_t lsw)
    //! \brief Combines two words into a dword (uint32_t)
    //!
    //! \param[in] msw most significant word
    //! \param[in] lsw least significant word
    //! \return dword from combined values
    static M_INLINE uint32_t words_To_Uint32(uint16_t msw, uint16_t lsw)
    {
        return (M_STATIC_CAST(uint32_t, msw) << 16) | (M_STATIC_CAST(uint32_t, lsw) << 0);
    }

    //! \def M_WordsTo4ByteValue(w1, w0)
    //! \brief Combines two words into a dword (uint32_t)
    //!
    //! This is a convenience macro for words_To_Uint32 for backwards compatibility
    //!
    //! \param[in] w1 most significant word
    //! \param[in] w0 least significant word
    //! \return dword from combined values
    //! \sa words_To_Uint32
#define M_WordsTo4ByteValue(w1, w0) words_To_Uint32(w0, w1)

    //! \fn static M_INLINE uint64_t words_To_Uint64(uint16_t msw, uint16_t word2, uint16_t word1, uint16_t lsw)
    //! \brief Combines four words into a qword (uint64_t)
    //!
    //! \param[in] msw most significant word (bits 63:48)
    //! \param[in] word2 next most significant word (bits 47:32)
    //! \param[in] word1 next word (bits 31:16)
    //! \param[in] lsw least significant word (bits 15:0)
    //! \return qword from combined values
    static M_INLINE uint64_t words_To_Uint64(uint16_t msw, uint16_t word2, uint16_t word1, uint16_t lsw)
    {
        return (M_STATIC_CAST(uint64_t, msw) << 48) | (M_STATIC_CAST(uint64_t, word2) << 32) |
               (M_STATIC_CAST(uint64_t, word1) << 16) | (M_STATIC_CAST(uint64_t, lsw) << 0);
    }

    //! \def M_WordsTo8ByteValue(w3, w2, w1, w0)
    //! \brief Combines four words into a qword (uint64_t)
    //!
    //! This is a convenience macro for words_To_Uint64 for backwards compatibility
    //!
    //! \param[in] w3 most significant word (bits 63:48)
    //! \param[in] w2 next most significant word (bits 47:32)
    //! \param[in] w1 next word (bits 31:16)
    //! \param[in] w0 least significant word (bits 15:0)
    //! \return qword from combined values
    //! \sa words_To_Uint64
#define M_WordsTo8ByteValue(w3, w2, w1, w0) words_To_Uint64(w3, w2, w1, w0)

    //! static M_INLINE uint64_t dwords_To_Uint64(uint32_t msdw, uint32_t lsdw)
    //!
    //! \fn static M_INLINE uint64_t dwords_To_Uint64(uint32_t msdw, uint32_t lsdw)
    //! \brief Combines two dwords into a qword (uint64_t)
    //!
    //! \param[in] msdw most significant dword (bits 63:32)
    //! \param[in] lsdw least significant dword (bits 31:0)
    //! \return qword from combined values
    static M_INLINE uint64_t dwords_To_Uint64(uint32_t msdw, uint32_t lsdw)
    {
        return (M_STATIC_CAST(uint64_t, msdw) << 32) | (M_STATIC_CAST(uint64_t, lsdw) << 0);
    }

    //! \def M_DWordsTo8ByteValue(d1, d0)
    //! \brief Combines two dwords into a qword (uint64_t)
    //!
    //! This is a convenience macro for dwords_To_Uint64 for backwards compatibility
    //!
    //! \param[in] msdw most significant dword (bits 63:32)
    //! \param[in] lsdw least significant dword (bits 31:0)
    //! \return qword from combined values
    //! \sa dwords_To_Uint64
#define M_DWordsTo8ByteValue(d1, d0) dwords_To_Uint64(d1, d0)

    //! \def ROUNDF(f, c)
    //! \brief Rounds a float so it does not get rounded up when formatting
    //!
    //! \param[in] f float to round
    //! \param[in] c nearest value to round to
    //! \return rounded float value
#define ROUNDF(f, c) M_STATIC_CAST(float, (M_STATIC_CAST(int, (f) * (c))) / (c))

    //! \struct genericint_t
    //! \brief Structure used to help generically retrieve a bit range from
    //!
    //! This structure contains the information necessary to assist with
    //! a generic way to extract a range of bits from any size value
    typedef struct sgenericint_t
    {
        bool   issigned;   /*!< set to true for a signed value, false for unsigned value */
        size_t sizeoftype; /*!< set to sizeof(var) when using this structure */
        union
        {
            uint8_t  u8;  /*!< set/get as a uint8_t value */
            uint16_t u16; /*!< set/get as a uint16_t value */
            uint32_t u32; /*!< set/get as a uint32_t value */
            uint64_t u64; /*!< set/get as a uint64_t value */
            int8_t   i8;  /*!< set/get as a int8_t value */
            int16_t  i16; /*!< set/get as a int16_t value */
            int32_t  i32; /*!< set/get as a int32_t value */
            int64_t  i64; /*!< set/get as a int64_t value */
        };                /*< Annonymous union for setting/getting different integer types without lots of casting */
    } genericint_t;

    //! \fn static M_INLINE bool is_generic_int_valid(genericint_t genint)
    //! \brief validates the contents of the genericint_t
    //!
    //! \param[in] genint genericint_t structure to validate
    //! \return true = valid, false = invalid
    static M_INLINE bool is_generic_int_valid(genericint_t genint)
    {
        bool good = false;
        switch (genint.sizeoftype)
        {
        case sizeof(uint8_t):
        case sizeof(uint16_t):
        case sizeof(uint32_t):
        case sizeof(uint64_t):
            good = true;
            break;
        }
        return good;
    }

    //! \fn genericint_t generic_Get_Bit_Range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb)
    //! \brief returns a genericint_t with the range of bits requested from the input value
    //!
    //! This is the "main" worker function to get a range of bits from "any" sized type value.
    //! It is recommended not to call this directly but to use the other inline helper functions
    //! below.
    //! \param[in] input genericint_t structure to retrieve data from
    //! \param[in] outputsize requested output integer size (sizeof(uint8_t), sizeof(int16_t), etc)
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return genericint_t structure with requested data range
    //! \sa get_bit_range_uint8
    //! \sa get_bit_range_uint16
    //! \sa get_8bit_range_uint16
    //! \sa get_bit_range_uint32
    //! \sa get_8bit_range_uint32
    //! \sa get_16bit_range_uint32
    //! \sa get_bit_range_uint64
    //! \sa get_8bit_range_uint64
    //! \sa get_16bit_range_uint64
    //! \sa get_32bit_range_uint64
    //! \sa get_bit_range_int8
    //! \sa get_bit_range_int16
    //! \sa get_8bit_range_int16
    //! \sa get_bit_range_int32
    //! \sa get_8bit_range_int32
    //! \sa get_16bit_range_int32
    //! \sa get_bit_range_int64
    //! \sa get_8bit_range_int64
    //! \sa get_16bit_range_int64
    //! \sa get_32bit_range_int64
    genericint_t generic_Get_Bit_Range(genericint_t input, size_t outputsize, uint8_t msb, uint8_t lsb);

    //! \fn static M_INLINE uint8_t get_bit_range_uint8(uint8_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for uint8_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint8_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE uint8_t get_bit_range_uint8(uint8_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = false;
        genint.u8         = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint8_t), msb, lsb);
        return genint.u8;
    }

    //! \fn static M_INLINE uint16_t get_bit_range_uint16(uint16_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for uint16_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint16_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE uint16_t get_bit_range_uint16(uint16_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = false;
        genint.u16        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint16_t), msb, lsb);
        return genint.u16;
    }

    //! \fn static M_INLINE uint8_t get_8bit_range_uint16(uint16_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for uint16_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint8_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE uint8_t get_8bit_range_uint16(uint16_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = false;
        genint.u16        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint8_t), msb, lsb);
        return genint.u8;
    }

    //! \fn static M_INLINE uint32_t get_bit_range_uint32(uint32_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for uint32_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint32_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE uint32_t get_bit_range_uint32(uint32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = false;
        genint.u32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint32_t), msb, lsb);
        return genint.u32;
    }

    //! \fn static M_INLINE uint8_t get_8bit_range_uint32(uint32_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for uint32_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint8_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE uint8_t get_8bit_range_uint32(uint32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = false;
        genint.u32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint8_t), msb, lsb);
        return genint.u8;
    }

    //! \fn static M_INLINE uint16_t get_16bit_range_uint32(uint32_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for uint32_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint16_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE uint16_t get_16bit_range_uint32(uint32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = false;
        genint.u32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint16_t), msb, lsb);
        return genint.u16;
    }

    //! \fn static M_INLINE uint64_t get_bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for uint64_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint64_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE uint64_t get_bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.issigned   = false;
        genint.u64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint64_t), msb, lsb);
        return genint.u64;
    }

    //! \fn static M_INLINE uint8_t get_8bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for uint64_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint8_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE uint8_t get_8bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.issigned   = false;
        genint.u64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint8_t), msb, lsb);
        return genint.u8;
    }

    //! \fn static M_INLINE uint16_t get_16bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for uint64_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint16_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE uint16_t get_16bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.issigned   = false;
        genint.u64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint16_t), msb, lsb);
        return genint.u16;
    }

    //! \fn static M_INLINE uint32_t get_32bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for uint64_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint32_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE uint32_t get_32bit_range_uint64(uint64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.issigned   = false;
        genint.u64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(uint32_t), msb, lsb);
        return genint.u32;
    }

    //! \fn static M_INLINE uint8_t get_bit_range_int8(uint8_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for int8_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return int8_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE int8_t get_bit_range_int8(int8_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = true;
        genint.i8         = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int8_t), msb, lsb);
        return genint.i8;
    }

    //! \fn static M_INLINE int16_t get_bit_range_int16(int16_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for int16_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return int16_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE int16_t get_bit_range_int16(int16_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = true;
        genint.i16        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int16_t), msb, lsb);
        return genint.i16;
    }

    //! \fn static M_INLINE int8_t get_8bit_range_int16(int16_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for int16_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return int8_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE int8_t get_8bit_range_int16(int16_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = true;
        genint.i16        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int8_t), msb, lsb);
        return genint.i8;
    }

    //! \fn static M_INLINE int32_t get_bit_range_int32(int32_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for int32_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return int32_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE int32_t get_bit_range_int32(int32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = true;
        genint.i32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int32_t), msb, lsb);
        return genint.i32;
    }

    //! \fn static M_INLINE int8_t get_8bit_range_int32(int32_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for int32_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return int8_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE int8_t get_8bit_range_int32(int32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = true;
        genint.i32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int8_t), msb, lsb);
        return genint.i8;
    }

    //! \fn static M_INLINE int16_t get_16bit_range_int32(int32_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for int32_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return int16_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE int16_t get_16bit_range_int32(int32_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.u64        = UINT64_C(0); // to ensure all bits of the anonymous union are zeroed
        genint.issigned   = true;
        genint.i32        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int16_t), msb, lsb);
        return genint.i16;
    }

    //! \fn static M_INLINE int64_t get_bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for int64_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return uint64_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE int64_t get_bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.issigned   = true;
        genint.i64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int64_t), msb, lsb);
        return genint.i64;
    }

    //! \fn static M_INLINE int8_t get_8bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for int64_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return int8_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE int8_t get_8bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.issigned   = true;
        genint.i64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int8_t), msb, lsb);
        return genint.i8;
    }

    //! \fn static M_INLINE int16_t get_16bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for int64_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return int16_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE int16_t get_16bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.issigned   = true;
        genint.i64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int16_t), msb, lsb);
        return genint.i16;
    }

    //! \fn static M_INLINE int32_t get_32bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    //! \brief Extracts a value from a range of bits and returns it
    //!
    //! This is a helper function for generic_Get_Bit_Range for int64_t values.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return int32_t with the requested bit range. Will be shifted so input lsb is at bit 0 of this output.
    static M_INLINE int32_t get_32bit_range_int64(int64_t value, uint8_t msb, uint8_t lsb)
    {
        genericint_t genint;
        genint.issigned   = true;
        genint.i64        = value;
        genint.sizeoftype = sizeof(value);
        genint            = generic_Get_Bit_Range(genint, sizeof(int32_t), msb, lsb);
        return genint.i32;
    }

    //! \def M_GETBITRANGE(input, msb, lsb)
    //! \brief Extracts a value from a range of bits and returns it as an unsigned value
    //!
    //! This is a backwards compatible convenience macro. It is recommended to use one of the other
    //! inline functions in this file instead of this macro.
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return appropriately sized integer based on bit range for msb and lsb.
    //! Will be shifted so input lsb is at bit 0 of this output.
    //! \sa get_bit_range_uint8
    //! \sa get_bit_range_uint16
    //! \sa get_8bit_range_uint16
    //! \sa get_bit_range_uint32
    //! \sa get_8bit_range_uint32
    //! \sa get_16bit_range_uint32
    //! \sa get_bit_range_uint64
    //! \sa get_8bit_range_uint64
    //! \sa get_16bit_range_uint64
    //! \sa get_32bit_range_uint64
    //! \sa get_bit_range_int8
    //! \sa get_bit_range_int16
    //! \sa get_8bit_range_int16
    //! \sa get_bit_range_int32
    //! \sa get_8bit_range_int32
    //! \sa get_16bit_range_int32
    //! \sa get_bit_range_int64
    //! \sa get_8bit_range_int64
    //! \sa get_16bit_range_int64
    //! \sa get_32bit_range_int64
#define M_GETBITRANGE(input, msb, lsb)                                                                                 \
    (((msb) - (lsb) + 1) <= 8                                                                                          \
         ? get_8bit_range_uint64(input, msb, lsb)                                                                      \
         : (((msb) - (lsb) + 1) <= 16                                                                                  \
                ? get_16bit_range_uint64(input, msb, lsb)                                                              \
                : (((msb) - (lsb) + 1) <= 32                                                                           \
                       ? get_32bit_range_uint64(input, msb, lsb)                                                       \
                       : (((msb) - (lsb) + 1) <= 64 ? get_bit_range_uint64(input, msb, lsb) : UINTMAX_MAX))))

    //! \def M_IGETBITRANGE(input, msb, lsb)
    //! \brief Extracts a value from a range of bits and returns it as a signed value
    //!
    //! This is a backwards compatible convenience macro. It is recommended to use one of the other
    //! inline functions in this file instead of this macro
    //!
    //! \param[in] value Input value to extract a range of bits from
    //! \param[in] msb most significant bit value
    //! \param[in] lsb least significant bit value
    //! \return appropriately sized integer based on bit range for msb and lsb.
    //! Will be shifted so input lsb is at bit 0 of this output.
    //! \sa get_bit_range_uint8
    //! \sa get_bit_range_uint16
    //! \sa get_8bit_range_uint16
    //! \sa get_bit_range_uint32
    //! \sa get_8bit_range_uint32
    //! \sa get_16bit_range_uint32
    //! \sa get_bit_range_uint64
    //! \sa get_8bit_range_uint64
    //! \sa get_16bit_range_uint64
    //! \sa get_32bit_range_uint64
    //! \sa get_bit_range_int8
    //! \sa get_bit_range_int16
    //! \sa get_8bit_range_int16
    //! \sa get_bit_range_int32
    //! \sa get_8bit_range_int32
    //! \sa get_16bit_range_int32
    //! \sa get_bit_range_int64
    //! \sa get_8bit_range_int64
    //! \sa get_16bit_range_int64
    //! \sa get_32bit_range_int64
#define M_IGETBITRANGE(input, msb, lsb)                                                                                \
    (((msb) - (lsb) + 1) <= 8                                                                                          \
         ? get_8bit_range_int64(input, msb, lsb)                                                                       \
         : (((msb) - (lsb) + 1) <= 16                                                                                  \
                ? get_16bit_range_int64(input, msb, lsb)                                                               \
                : (((msb) - (lsb) + 1) <= 32                                                                           \
                       ? get_32bit_range_int64(input, msb, lsb)                                                        \
                       : (((msb) - (lsb) + 1) <= 64 ? get_bit_range_int64(input, msb, lsb) : INTMAX_MAX))))

    // Bit access macros

    //! \def M_BitN(n)
    //! \brief Sets a specific bit number to 1 and all other bits are zero.
    //! Recommend using the other macros with specific widths over this one
    //! \sa M_BitN8
    //! \sa M_BitN16
    //! \sa M_BitN32
    //! \sa M_BitN64
#define M_BitN(n) (UINT64_C(1) << (n))

    //! \def M_BitN8(n)
    //! \brief Sets a specific bit number to 1 and all other bits are zero.
#define M_BitN8(n) (UINT8_C(1) << (n))

    //! \def M_BitN16(n)
    //! \brief Sets a specific bit number to 1 and all other bits are zero.
#define M_BitN16(n) (UINT16_C(1) << (n))

    //! \def M_BitN32(n)
    //! \brief Sets a specific bit number to 1 and all other bits are zero.
#define M_BitN32(n) (UINT32_C(1) << (n))

    //! \def M_BitN64(n)
    //! \brief Sets a specific bit number to 1 and all other bits are zero.
#define M_BitN64(n) (UINT64_C(1) << (n))

// defined in EDK2 MdePkg and causes conflicts, so checking this define for now
// to avoid conflicts
#if !defined(UEFI_C_SOURCE)

//! \def BIT0
//! \brief Sets the 0th bit (0x01).
#    define BIT0 (M_BitN8(0))

//! \def BIT1
//! \brief Sets the 1st bit (0x02).
#    define BIT1 (M_BitN8(1))

//! \def BIT2
//! \brief Sets the 2nd bit (0x04).
#    define BIT2 (M_BitN8(2))

//! \def BIT3
//! \brief Sets the 3rd bit (0x08).
#    define BIT3 (M_BitN8(3))

//! \def BIT4
//! \brief Sets the 4th bit (0x10).
#    define BIT4 (M_BitN8(4))

//! \def BIT5
//! \brief Sets the 5th bit (0x20).
#    define BIT5 (M_BitN8(5))

//! \def BIT6
//! \brief Sets the 6th bit (0x40).
#    define BIT6 (M_BitN8(6))

//! \def BIT7
//! \brief Sets the 7th bit (0x80).
#    define BIT7 (M_BitN8(7))

//! \def BIT8
//! \brief Sets the 8th bit (0x0100).
#    define BIT8 (M_BitN16(8))

//! \def BIT9
//! \brief Sets the 9th bit (0x0200).
#    define BIT9 (M_BitN16(9))

//! \def BIT10
//! \brief Sets the 10th bit (0x0400).
#    define BIT10 (M_BitN16(10))

//! \def BIT11
//! \brief Sets the 11th bit (0x0800).
#    define BIT11 (M_BitN16(11))

//! \def BIT12
//! \brief Sets the 12th bit (0x1000).
#    define BIT12 (M_BitN16(12))

//! \def BIT13
//! \brief Sets the 13th bit (0x2000).
#    define BIT13 (M_BitN16(13))

//! \def BIT14
//! \brief Sets the 14th bit (0x4000).
#    define BIT14 (M_BitN16(14))

//! \def BIT15
//! \brief Sets the 15th bit (0x8000).
#    define BIT15 (M_BitN16(15))

//! \def BIT16
//! \brief Sets the 16th bit (0x00010000).
#    define BIT16 (M_BitN32(16))

//! \def BIT17
//! \brief Sets the 17th bit (0x00020000).
#    define BIT17 (M_BitN32(17))

//! \def BIT18
//! \brief Sets the 18th bit (0x00040000).
#    define BIT18 (M_BitN32(18))

//! \def BIT19
//! \brief Sets the 19th bit (0x00080000).
#    define BIT19 (M_BitN32(19))

//! \def BIT20
//! \brief Sets the 20th bit (0x00100000).
#    define BIT20 (M_BitN32(20))

//! \def BIT21
//! \brief Sets the 21st bit (0x00200000).
#    define BIT21 (M_BitN32(21))

//! \def BIT22
//! \brief Sets the 22nd bit (0x00400000).
#    define BIT22 (M_BitN32(22))

//! \def BIT23
//! \brief Sets the 23rd bit (0x00800000).
#    define BIT23 (M_BitN32(23))

//! \def BIT24
//! \brief Sets the 24th bit (0x01000000).
#    define BIT24 (M_BitN32(24))

//! \def BIT25
//! \brief Sets the 25th bit (0x02000000).
#    define BIT25 (M_BitN32(25))

//! \def BIT26
//! \brief Sets the 26th bit (0x04000000).
#    define BIT26 (M_BitN32(26))

//! \def BIT27
//! \brief Sets the 27th bit (0x08000000).
#    define BIT27 (M_BitN32(27))

//! \def BIT28
//! \brief Sets the 28th bit (0x10000000).
#    define BIT28 (M_BitN32(28))

//! \def BIT29
//! \brief Sets the 29th bit (0x20000000).
#    define BIT29 (M_BitN32(29))

//! \def BIT30
//! \brief Sets the 30th bit (0x40000000).
#    define BIT30 (M_BitN32(30))

//! \def BIT31
//! \brief Sets the 31st bit (0x80000000).
#    define BIT31 (M_BitN32(31))

//! \def BIT32
//! \brief Sets the 32nd bit (0x0000000100000000).
#    define BIT32 (M_BitN64(32))

//! \def BIT33
//! \brief Sets the 33rd bit (0x0000000200000000).
#    define BIT33 (M_BitN64(33))

//! \def BIT34
//! \brief Sets the 34th bit (0x0000000400000000).
#    define BIT34 (M_BitN64(34))

//! \def BIT35
//! \brief Sets the 35th bit (0x0000000800000000).
#    define BIT35 (M_BitN64(35))

//! \def BIT36
//! \brief Sets the 36th bit (0x0000001000000000).
#    define BIT36 (M_BitN64(36))

//! \def BIT37
//! \brief Sets the 37th bit (0x0000002000000000).
#    define BIT37 (M_BitN64(37))

//! \def BIT38
//! \brief Sets the 38th bit (0x0000004000000000).
#    define BIT38 (M_BitN64(38))

//! \def BIT39
//! \brief Sets the 39th bit (0x0000008000000000).
#    define BIT39 (M_BitN64(39))

//! \def BIT40
//! \brief Sets the 40th bit (0x0000010000000000).
#    define BIT40 (M_BitN64(40))

//! \def BIT41
//! \brief Sets the 41st bit (0x0000020000000000).
#    define BIT41 (M_BitN64(41))

//! \def BIT42
//! \brief Sets the 42nd bit (0x0000040000000000).
#    define BIT42 (M_BitN64(42))

//! \def BIT43
//! \brief Sets the 43rd bit (0x0000080000000000).
#    define BIT43 (M_BitN64(43))

//! \def BIT44
//! \brief Sets the 44th bit (0x0000100000000000).
#    define BIT44 (M_BitN64(44))

//! \def BIT45
//! \brief Sets the 45th bit (0x0000200000000000).
#    define BIT45 (M_BitN64(45))

//! \def BIT46
//! \brief Sets the 46th bit (0x0000400000000000).
#    define BIT46 (M_BitN64(46))

//! \def BIT47
//! \brief Sets the 47th bit (0x0000800000000000).
#    define BIT47 (M_BitN64(47))

//! \def BIT48
//! \brief Sets the 48th bit (0x0001000000000000).
#    define BIT48 (M_BitN64(48))

//! \def BIT49
//! \brief Sets the 49th bit (0x0002000000000000).
#    define BIT49 (M_BitN64(49))

//! \def BIT50
//! \brief Sets the 50th bit (0x0004000000000000).
#    define BIT50 (M_BitN64(50))

//! \def BIT51
//! \brief Sets the 51st bit (0x0008000000000000).
#    define BIT51 (M_BitN64(51))

//! \def BIT52
//! \brief Sets the 52nd bit (0x0010000000000000).
#    define BIT52 (M_BitN64(52))

//! \def BIT53
//! \brief Sets the 53rd bit (0x0020000000000000).
#    define BIT53 (M_BitN64(53))

//! \def BIT54
//! \brief Sets the 54th bit (0x0040000000000000).
#    define BIT54 (M_BitN64(54))

//! \def BIT55
//! \brief Sets the 55th bit (0x0080000000000000).
#    define BIT55 (M_BitN64(55))

//! \def BIT56
//! \brief Sets the 56th bit (0x0100000000000000).
#    define BIT56 (M_BitN64(56))

//! \def BIT57
//! \brief Sets the 57th bit (0x0200000000000000).
#    define BIT57 (M_BitN64(57))

//! \def BIT58
//! \brief Sets the 58th bit (0x0400000000000000).
#    define BIT58 (M_BitN64(58))

//! \def BIT59
//! \brief Sets the 59th bit (0x0800000000000000).
#    define BIT59 (M_BitN64(59))

//! \def BIT60
//! \brief Sets the 60th bit (0x1000000000000000).
#    define BIT60 (M_BitN64(60))

//! \def BIT61
//! \brief Sets the 61st bit (0x2000000000000000).
#    define BIT61 (M_BitN64(61))

//! \def BIT62
//! \brief Sets the 62nd bit (0x4000000000000000).
#    define BIT62 (M_BitN64(62))

//! \def BIT63
//! \brief Sets the 63rd bit (0x8000000000000000).
#    define BIT63 (M_BitN64(63))

#endif // UEFI_C_SOURCE

//! \def M_SET_BIT(val, bitNum)
//! \brief sets the specified bit in the input value.
//!
//! Recommended to use one of the other functions in this file
//! instead!
//! \sa set_uint8_bit
//! \sa set_uint16_bit
//! \sa set_uint32_bit
//! \sa set_uint64_bit
#define M_SET_BIT(val, bitNum) ((val) |= M_BitN(bitNum))

//! \def M_CLEAR_BIT(val, bitNum)
//! \brief Clears the specified bit in the input value.
//!
//! Recommended to use one of the other functions in this file
//! instead!
//! \sa clear_uint8_bit
//! \sa clear_uint16_bit
//! \sa clear_uint32_bit
//! \sa clear_uint64_bit
#define M_CLEAR_BIT(val, bitNum) ((val) &= (~M_BitN(bitNum)))

    //! \fn static M_INLINE uint8_t set_uint8_bit(uint8_t val, uint8_t bitNum)
    //! \brief sets the specified bit to one in the input value.
    //!
    //! \param[in] val Original value to set a bit to 1 in.
    //! \param[in] bitNum The bit number to set to 1. Value values are 0 - 7
    //! \return returns \a val with the specified bit set to 1
    static M_INLINE uint8_t set_uint8_bit(uint8_t val, uint8_t bitNum)
    {
        return M_STATIC_CAST(uint8_t, val | M_STATIC_CAST(uint8_t, UINT8_C(1) << bitNum));
    }

    //! \fn static M_INLINE uint16_t set_uint16_bit(uint16_t val, uint16_t bitNum)
    //! \brief sets the specified bit to one in the input value.
    //!
    //! \param[in] val Original value to set a bit to 1 in.
    //! \param[in] bitNum The bit number to set to 1. Value values are 0 - 15
    //! \return returns \a val with the specified bit set to 1
    static M_INLINE uint16_t set_uint16_bit(uint16_t val, uint16_t bitNum)
    {
        return M_STATIC_CAST(uint16_t, val | M_STATIC_CAST(uint16_t, UINT16_C(1) << bitNum));
    }

    //! \fn static M_INLINE uint32_t set_uint32_bit(uint32_t val, uint32_t bitNum)
    //! \brief sets the specified bit to one in the input value.
    //!
    //! \param[in] val Original value to set a bit to 1 in.
    //! \param[in] bitNum The bit number to set to 1. Value values are 0 - 31
    //! \return returns \a val with the specified bit set to 1
    static M_INLINE uint32_t set_uint32_bit(uint32_t val, uint32_t bitNum)
    {
        return val | M_STATIC_CAST(uint32_t, UINT32_C(1) << bitNum);
    }

    //! \fn static M_INLINE uint64_t set_uint64_bit(uint64_t val, uint64_t bitNum)
    //! \brief sets the specified bit to one in the input value.
    //!
    //! \param[in] val Original value to set a bit to 1 in.
    //! \param[in] bitNum The bit number to set to 1. Value values are 0 - 63
    //! \return returns \a val with the specified bit set to 1
    static M_INLINE uint64_t set_uint64_bit(uint64_t val, uint64_t bitNum)
    {
        return val | M_STATIC_CAST(uint64_t, UINT64_C(1) << bitNum);
    }

    //! \fn static M_INLINE uint8_t clear_uint8_bit(uint8_t val, uint8_t bitNum)
    //! \brief clears the specified bit to zero in the input value.
    //!
    //! \param[in] val Original value to clear a bit to 0 in.
    //! \param[in] bitNum The bit number to clear to 0. Value values are 0 - 7
    //! \return returns \a val with the specified bit cleared to 0
    static M_INLINE uint8_t clear_uint8_bit(uint8_t val, uint8_t bitNum)
    {
        return M_STATIC_CAST(uint8_t, val & M_STATIC_CAST(uint8_t, ~(UINT8_C(1) << bitNum)));
    }

    //! \fn static M_INLINE uint16_t clear_uint16_bit(uint16_t val, uint16_t bitNum)
    //! \brief clears the specified bit to zero in the input value.
    //!
    //! \param[in] val Original value to clear a bit to 0 in.
    //! \param[in] bitNum The bit number to clear to 0. Value values are 0 - 15
    //! \return returns \a val with the specified cleared set to 0
    static M_INLINE uint16_t clear_uint16_bit(uint16_t val, uint16_t bitNum)
    {
        return M_STATIC_CAST(uint16_t, val & M_STATIC_CAST(uint16_t, ~(UINT16_C(1) << bitNum)));
    }

    //! \fn static M_INLINE uint32_t clear_uint32_bit(uint32_t val, uint32_t bitNum)
    //! \brief clears the specified bit to zero in the input value.
    //!
    //! \param[in] val Original value to clear a bit to 0 in.
    //! \param[in] bitNum The bit number to clear to 0. Value values are 0 - 31
    //! \return returns \a val with the specified cleared set to 0
    static M_INLINE uint32_t clear_uint32_bit(uint32_t val, uint32_t bitNum)
    {
        return val & M_STATIC_CAST(uint32_t, ~(UINT32_C(1) << bitNum));
    }

    //! \fn static M_INLINE uint64_t clear_uint64_bit(uint64_t val, uint64_t bitNum)
    //! \brief clears the specified bit to zero in the input value.
    //!
    //! \param[in] val Original value to clear a bit to 0 in.
    //! \param[in] bitNum The bit number to clear to 0. Value values are 0 - 63
    //! \return returns \a val with the specified cleared set to 0
    static M_INLINE uint64_t clear_uint64_bit(uint64_t val, uint64_t bitNum)
    {
        return val & M_STATIC_CAST(uint64_t, ~(UINT64_C(1) << bitNum));
    }

    //! \def M_CLEAR_BIT8(val, bitNum)
    //! \brief clears the specified bit to zero in the input value.
    //!
    //! This is a convenience macro for backwards compatibility
    //!
    //! \param[in] val Original value to clear a bit to 0 in.
    //! \param[in] bitNum The bit number to clear to 0. Value values are 0 - 7
    //! \return returns \a val with the specified cleared set to 0
    //! \sa clear_uint8_bit
#define M_CLEAR_BIT8(val, bitNum) ((val) = clear_uint8_bit(val, bitNum))

    //! \def M_CLEAR_BIT16(val, bitNum)
    //! \brief clears the specified bit to zero in the input value.
    //!
    //! This is a convenience macro for backwards compatibility
    //!
    //! \param[in] val Original value to clear a bit to 0 in.
    //! \param[in] bitNum The bit number to clear to 0. Value values are 0 - 15
    //! \return returns \a val with the specified cleared set to 0
    //! \sa clear_uint16_bit
#define M_CLEAR_BIT16(val, bitNum) ((val) = clear_uint16_bit(val, bitNum))

    //! \def M_CLEAR_BIT32(val, bitNum)
    //! \brief clears the specified bit to zero in the input value.
    //!
    //! This is a convenience macro for backwards compatibility
    //!
    //! \param[in] val Original value to clear a bit to 0 in.
    //! \param[in] bitNum The bit number to clear to 0. Value values are 0 - 31
    //! \return returns \a val with the specified cleared set to 0
    //! \sa clear_uint32_bit
#define M_CLEAR_BIT32(val, bitNum) ((val) = clear_uint32_bit(val, bitNum))

    //! \def M_CLEAR_BIT64(val, bitNum)
    //! \brief clears the specified bit to zero in the input value.
    //!
    //! This is a convenience macro for backwards compatibility
    //!
    //! \param[in] val Original value to clear a bit to 0 in.
    //! \param[in] bitNum The bit number to clear to 0. Value values are 0 - 63
    //! \return returns \a val with the specified cleared set to 0
    //! \sa clear_uint64_bit
#define M_CLEAR_BIT64(val, bitNum) ((val) = clear_uint64_bit(val, bitNum))

    //! \def M_SET_BIT8(val, bitNum)
    //! \brief sets the specified bit to one in the input value.
    //!
    //! This is a convenience macro for backwards compatibility
    //!
    //! \param[in] val Original value to set a bit to 1 in.
    //! \param[in] bitNum The bit number to set to 1. Value values are 0 - 7
    //! \return returns \a val with the specified bit set to 1
    //! \sa set_uint8_bit
#define M_SET_BIT8(val, bitNum) ((val) = set_uint8_bit(val, bitNum))

    //! \def M_SET_BIT16(val, bitNum)
    //! \brief sets the specified bit to one in the input value.
    //!
    //! This is a convenience macro for backwards compatibility
    //!
    //! \param[in] val Original value to set a bit to 1 in.
    //! \param[in] bitNum The bit number to set to 1. Value values are 0 - 15
    //! \return returns \a val with the specified bit set to 1
    //! \sa set_uint16_bit
#define M_SET_BIT16(val, bitNum) ((val) = set_uint16_bit(val, bitNum))

    //! \def M_SET_BIT32(val, bitNum)
    //! \brief sets the specified bit to one in the input value.
    //!
    //! This is a convenience macro for backwards compatibility
    //!
    //! \param[in] val Original value to set a bit to 1 in.
    //! \param[in] bitNum The bit number to set to 1. Value values are 0 - 31
    //! \return returns \a val with the specified bit set to 1
    //! \sa set_uint32_bit
#define M_SET_BIT32(val, bitNum) ((val) = set_uint32_bit(val, bitNum))

    //! \def M_SET_BIT64(val, bitNum)
    //! \brief sets the specified bit to one in the input value.
    //!
    //! This is a convenience macro for backwards compatibility
    //!
    //! \param[in] val Original value to set a bit to 1 in.
    //! \param[in] bitNum The bit number to set to 1. Value values are 0 - 63
    //! \return returns \a val with the specified bit set to 1
    //! \sa set_uint64_bit
#define M_SET_BIT64(val, bitNum) ((val) = set_uint64_bit(val, bitNum))

    //! \fn M_NODISCARD static M_INLINE uint8_t n_swap_8(uint8_t byte)
    //! \brief swaps nibbles within a byte and returns it
    //!
    //! \param[in] byte byte to swap nibbles in
    //! \return returns \a byte with the nibbles swapped
    M_NODISCARD static M_INLINE uint8_t n_swap_8(uint8_t byte)
    {
        return M_STATIC_CAST(uint8_t, ((byte & UINT8_C(0x0F)) << 4)) |
               M_STATIC_CAST(uint8_t, ((byte & UINT8_C(0xF0)) >> 4));
    }

    //! \fn void nibble_Swap(uint8_t* byteToSwap)
    //! \brief swaps nibbles within a byte in place
    //!
    //! Recommend migration to n_swap_8
    //!
    //! \param[in,out] byteToSwap pointer to byte to swap nibbles
    //! \sa n_swap_8
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void nibble_Swap(uint8_t* byteToSwap)
    {
        DISABLE_NONNULL_COMPARE
        if (byteToSwap != M_NULLPTR)
        {
            *byteToSwap = n_swap_8(*byteToSwap);
        }
        RESTORE_NONNULL_COMPARE
    }

    //! \fn M_NODISCARD static M_INLINE uint16_t b_swap_16(uint16_t value)
    //! \brief byte swaps a uint16_t
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value value to byte swap
    //! \return returns \a value with bytes swapped
    M_NODISCARD static M_INLINE uint16_t b_swap_16(uint16_t value)
    {
#if defined(HAVE_BUILTIN_BSWAP)
        return __builtin_bswap16(value);
#elif defined(HAVE_WIN_BSWAP)
    return _byteswap_ushort(value);
#else
    return (((value & UINT16_C(0x00FF)) << 8) | ((value & UINT16_C(0xFF00)) >> 8));
#endif
    }

    //! \fn M_NODISCARD static M_INLINE uint32_t b_swap_32(uint32_t value)
    //! \brief byte swaps a uint32_t
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value value to byte swap
    //! \return returns \a value with bytes swapped
    M_NODISCARD static M_INLINE uint32_t b_swap_32(uint32_t value)
    {
#if defined(HAVE_BUILTIN_BSWAP)
        return __builtin_bswap32(value);
#elif defined(HAVE_WIN_BSWAP) && defined(UINT_MAX) && defined(ULONG_MAX) && UINT_MAX == ULONG_MAX
    // Windows defines uint32 as unsigned int, not unsigned long as this
    // function expects. So we have a more elaborate check in order to make sure
    // these have the same range before casting and returning that result It is
    // unlikely this will change in Windows, but it does not hurt to be safe and
    // verify everything before casting types that can de defined as different
    // widths-TJE
    return M_STATIC_CAST(uint32_t, _byteswap_ulong(M_STATIC_CAST(unsigned long, value)));
#else
    return ((value & UINT32_C(0xFF000000) >> 24) | (value & UINT32_C(0x00FF0000) >> 8) |
            (value & UINT32_C(0x0000FF00) << 8) | (value & UINT32_C(0x000000FF) << 24));
#endif
    }

    //! \fn M_NODISCARD static M_INLINE uint64_t b_swap_64(uint64_t value)
    //! \brief byte swaps a uint64_t
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value value to byte swap
    //! \return returns \a value with bytes swapped
    M_NODISCARD static M_INLINE uint64_t by_swap_64(uint64_t value)
    {
#if defined(HAVE_BUILTIN_BSWAP)
        return __builtin_bswap64(value);
#elif defined(HAVE_WIN_BSWAP)
    return _byteswap_uint64(value);
#else
    return ((value & UINT64_C(0xFF00000000000000) >> 56) | (value & UINT64_C(0x00FF000000000000) >> 40) |
            (value & UINT64_C(0x0000FF0000000000) >> 24) | (value & UINT64_C(0x000000FF00000000) >> 8) |
            (value & UINT64_C(0x00000000FF000000) << 8) | (value & UINT64_C(0x0000000000FF0000) << 24) |
            (value & UINT64_C(0x000000000000FF00) << 40) | (value & UINT64_C(0x00000000000000FF) << 56));
#endif
    }

    //! \fn void byte_Swap_16(uint16_t* wordToSwap)
    //! \brief swaps bytes within a word in place
    //!
    //! Recommend migrating to b_swap_16
    //!
    //! \param[in,out] wordToSwap pointer to word to swap bytes
    //! \sa b_swap_16
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void byte_Swap_16(uint16_t* wordToSwap)
    {
        DISABLE_NONNULL_COMPARE
        if (wordToSwap != M_NULLPTR)
        {
            *wordToSwap = b_swap_16(*wordToSwap);
        }
        RESTORE_NONNULL_COMPARE
    }

    //! \fn void byte_Swap_Int16(int16_t* signedWordToSwap)
    //! \brief swaps bytes within a signed word in place
    //!
    //! \param[in,out] signedWordToSwap pointer to word to swap bytes
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void byte_Swap_Int16(int16_t* signedWordToSwap)
    {
        DISABLE_NONNULL_COMPARE
        if (signedWordToSwap != M_NULLPTR)
        {
            *signedWordToSwap = M_STATIC_CAST(int16_t, ((*signedWordToSwap & UINT16_C(0x00FF)) << 8)) |
                                M_STATIC_CAST(int16_t, ((*signedWordToSwap & UINT16_C(0xFF00)) >> 8));
        }
        RESTORE_NONNULL_COMPARE
    }

    //! \fn void byte_Swap_32(uint32_t* doubleWordToSwap)
    //! \brief swaps bytes within a dword in place
    //!
    //! Recommend migrating to b_swap_32
    //!
    //! \param[in,out] doubleWordToSwap pointer to dword to swap bytes
    //! \sa b_swap_32
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void byte_Swap_32(uint32_t* doubleWordToSwap)
    {
        DISABLE_NONNULL_COMPARE
        if (doubleWordToSwap != M_NULLPTR)
        {
            *doubleWordToSwap = b_swap_32(*doubleWordToSwap);
        }
        RESTORE_NONNULL_COMPARE
    }

    //! \fn void byte_Swap_Int32(int32_t* signedDWord)
    //! \brief swaps bytes within a signed dword in place
    //!
    //! \param[in,out] signedDWord pointer to signed dword to swap bytes
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void byte_Swap_Int32(int32_t* signedDWord)
    {
        DISABLE_NONNULL_COMPARE
        if (signedDWord != M_NULLPTR)
        {
            *signedDWord =
                M_STATIC_CAST(int32_t, ((*signedDWord & M_STATIC_CAST(int32_t, INT32_C(0x0000FFFF))) << 16)) |
                M_STATIC_CAST(int32_t, ((*signedDWord & M_STATIC_CAST(int32_t, INT32_C(0xFFFF0000))) >> 16));
            *signedDWord = M_STATIC_CAST(int32_t, ((*signedDWord & M_STATIC_CAST(int32_t, INT32_C(0x00FF00FF))) << 8)) |
                           M_STATIC_CAST(int32_t, ((*signedDWord & M_STATIC_CAST(int32_t, INT32_C(0xFF00FF00))) >> 8));
        }
        RESTORE_NONNULL_COMPARE
    }

    //! \fn void word_Swap_32(uint32_t* doubleWordToSwap)
    //! \brief swaps words within a dword in place
    //!
    //! \param[in,out] doubleWordToSwap pointer to dword to swap words
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void word_Swap_32(uint32_t* doubleWordToSwap)
    {
        DISABLE_NONNULL_COMPARE
        if (doubleWordToSwap != M_NULLPTR)
        {
            *doubleWordToSwap =
                ((*doubleWordToSwap & UINT32_C(0x0000FFFF)) << 16) | ((*doubleWordToSwap & UINT32_C(0xFFFF0000)) >> 16);
        }
        RESTORE_NONNULL_COMPARE
    }

    //! \fn void byte_Swap_64(uint64_t* quadWordToSwap)
    //! \brief swaps bytes within a qword in place
    //!
    //! Recommend migrating to b_swap_64
    //!
    //! \param[in,out] quadWordToSwap pointer to qword to swap bytes
    //! \sa b_swap_64
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void byte_Swap_64(uint64_t* quadWordToSwap)
    {
        DISABLE_NONNULL_COMPARE
        if (quadWordToSwap != M_NULLPTR)
        {
            *quadWordToSwap = by_swap_64(*quadWordToSwap);
        }
        RESTORE_NONNULL_COMPARE
    }

    //! \fn void word_Swap_64(uint64_t* quadWordToSwap)
    //! \brief swaps words within a qword in place
    //!
    //! \param[in,out] quadWordToSwap pointer to qword to swap words
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void word_Swap_64(uint64_t* quadWordToSwap)
    {
        DISABLE_NONNULL_COMPARE
        if (quadWordToSwap != M_NULLPTR)
        {
            *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00000000FFFFFFFF)) << 32) |
                              ((*quadWordToSwap & UINT64_C(0xFFFFFFFF00000000)) >> 32);
            *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x0000FFFF0000FFFF)) << 16) |
                              ((*quadWordToSwap & UINT64_C(0xFFFF0000FFFF0000)) >> 16);
        }
        RESTORE_NONNULL_COMPARE
    }

    //! \fn void double_Word_Swap_64(uint64_t* quadWordToSwap)
    //! \brief swaps dwords within a qword in place
    //!
    //! \param[in,out] quadWordToSwap pointer to qword to swap dwords
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void double_Word_Swap_64(uint64_t* quadWordToSwap)
    {
        DISABLE_NONNULL_COMPARE
        if (quadWordToSwap != M_NULLPTR)
        {
            *quadWordToSwap = ((*quadWordToSwap & UINT64_C(0x00000000FFFFFFFF)) << 32) |
                              ((*quadWordToSwap & UINT64_C(0xFFFFFFFF00000000)) >> 32);
        }
        RESTORE_NONNULL_COMPARE
    }

    //! \fn bool get_Bytes_To_16(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint16_t*
    //! out) \brief takes a data pointer and byte offsets to output a uint16_t
    //!
    //! Reads from a data buffer pointer to output a uint16_t. MSB and LSB can
    //! be in little or big endian (MSB > LSB or MSB < LSB) when it reads.
    //! The data pointer should be to the beginning of the buffer and msb/lsb are offsets from the
    //! first byte the pointer is pointing to.
    //!
    //! \param[in] dataPtrBeginning pointer to beginning of a data buffer
    //! \param[in] fullDataLen full length of the data buffer starting from \a dataPtrBeginning
    //! \param[in] msb most significant byte offset
    //! \param[in] lsb least significant byte offset
    //! \param[out] out uint16_t output based on the input parameters
    //! \return true when this function succeeds, false on error.
    bool get_Bytes_To_16(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint16_t* out);

    //! \fn bool get_Bytes_To_32(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint32_t*
    //! out) \brief takes a data pointer and byte offsets to output a uint32_t
    //!
    //! Reads from a data buffer pointer to output a uint32_t. MSB and LSB can
    //! be in little or big endian (MSB > LSB or MSB < LSB) when it reads.
    //! The data pointer should be to the beginning of the buffer and msb/lsb are offsets from the
    //! first byte the pointer is pointing to.
    //!
    //! \param[in] dataPtrBeginning pointer to beginning of a data buffer
    //! \param[in] fullDataLen full length of the data buffer starting from \a dataPtrBeginning
    //! \param[in] msb most significant byte offset
    //! \param[in] lsb least significant byte offset
    //! \param[out] out uint32_t output based on the input parameters
    //! \return true when this function succeeds, false on error.
    bool get_Bytes_To_32(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint32_t* out);

    //! \fn bool get_Bytes_To_32(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint64_t*
    //! out) \brief takes a data pointer and byte offsets to output a uint64_t
    //!
    //! Reads from a data buffer pointer to output a uint64_t. MSB and LSB can
    //! be in little or big endian (MSB > LSB or MSB < LSB) when it reads.
    //! The data pointer should be to the beginning of the buffer and msb/lsb are offsets from the
    //! first byte the pointer is pointing to.
    //!
    //! \param[in] dataPtrBeginning pointer to beginning of a data buffer
    //! \param[in] fullDataLen full length of the data buffer starting from \a dataPtrBeginning
    //! \param[in] msb most significant byte offset
    //! \param[in] lsb least significant byte offset
    //! \param[out] out uint64_t output based on the input parameters
    //! \return true when this function succeeds, false on error.
    bool get_Bytes_To_64(const uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint64_t* out);

    //! \fn uint16_t be16_to_host(uint16_t value)
    //! \brief takes a big endian uint16_t and returns it in host endianness
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value big endian value to convert to host endianness
    //! \return returns \a value in the host CPU's endianness
    M_NODISCARD static M_INLINE uint16_t be16_to_host(uint16_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return value;
#else // Assume little endian
    return b_swap_16(value);
#endif
    }

    //! \fn uint32_t be32_to_host(uint32_t value)
    //! \brief takes a big endian uint32_t and returns it in host endianness
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value big endian value to convert to host endianness
    //! \return returns \a value in the host CPU's endianness
    M_NODISCARD static M_INLINE uint32_t be32_to_host(uint32_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return value;
#else // Assume little endian
    return b_swap_32(value);
#endif
    }

    //! \fn uint64_t be64_to_host(uint64_t value)
    //! \brief takes a big endian uint64_t and returns it in host endianness
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value big endian value to convert to host endianness
    //! \return returns \a value in the host CPU's endianness
    M_NODISCARD static M_INLINE uint64_t be64_to_host(uint64_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return value;
#else // Assume little endian
    return by_swap_64(value);
#endif
    }

    //! \fn uint16_t host_to_be16(uint16_t value)
    //! \brief takes a host endian uint16_t and returns it in big endian
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value host endian value to convert to big endian
    //! \return returns \a value in big endian
    M_NODISCARD static M_INLINE uint16_t host_to_be16(uint16_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return value;
#else // Assume little endian
    return b_swap_16(value);
#endif
    }

    //! \fn uint32_t host_to_be16(uint32_t value)
    //! \brief takes a host endian uint32_t and returns it in big endian
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value host endian value to convert to big endian
    //! \return returns \a value in big endian
    M_NODISCARD static M_INLINE uint32_t host_to_be32(uint32_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return value;
#else // Assume little endian
    return b_swap_32(value);
#endif
    }

    //! \fn uint64_t host_to_be64(uint64_t value)
    //! \brief takes a host endian uint64_t and returns it in big endian
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value host endian value to convert to big endian
    //! \return returns \a value in big endian
    M_NODISCARD static M_INLINE uint64_t host_to_be64(uint64_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return value;
#else // Assume little endian
    return by_swap_64(value);
#endif
    }

    //! \fn uint16_t host_to_le16(uint16_t value)
    //! \brief takes a host endian uint16_t and returns it in little endian
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value host endian value to convert to little endian
    //! \return returns \a value in little endian
    M_NODISCARD static M_INLINE uint16_t host_to_le16(uint16_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return b_swap_16(value);
#else // Assume little endian
    return value;
#endif
    }

    //! \fn uint32_t host_to_le32(uint32_t value)
    //! \brief takes a host endian uint32_t and returns it in little endian
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value host endian value to convert to little endian
    //! \return returns \a value in little endian
    M_NODISCARD static M_INLINE uint32_t host_to_le32(uint32_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return b_swap_32(value);
#else // Assume little endian
    return value;
#endif
    }

    //! \fn uint64_t host_to_le64(uint64_t value)
    //! \brief takes a host endian uint64_t and returns it in little endian
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value host endian value to convert to little endian
    //! \return returns \a value in little endian
    M_NODISCARD static M_INLINE uint64_t host_to_le64(uint64_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return by_swap_64(value);
#else // Assume little endian
    return value;
#endif
    }

    //! \fn uint16_t le16_to_host(uint16_t value)
    //! \brief takes a little endian uint16_t and returns it in host endianness
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value little endian value to convert to host endianness
    //! \return returns \a value in the host CPU's endianness
    M_NODISCARD static M_INLINE uint16_t le16_to_host(uint16_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return b_swap_16(value);
#else // Assume little endian
    return value;
#endif
    }

    //! \fn uint32_t le32_to_host(uint32_t value)
    //! \brief takes a little endian uint32_t and returns it in host endianness
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value little endian value to convert to host endianness
    //! \return returns \a value in the host CPU's endianness
    M_NODISCARD static M_INLINE uint32_t le32_to_host(uint32_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return b_swap_32(value);
#else // Assume little endian
    return value;
#endif
    }

    //! \fn uint64_t le64_to_host(uint64_t value)
    //! \brief takes a little endian uint64_t and returns it in host endianness
    //!
    //! Named similarly to functions found in ?/endian.h files in various POSIX systems.
    //! Due to these headers residing in different places and some variable function names,
    //! these are reimplemented here and the names have underscores unlike POSIX 2024 implementation,
    //! but work identically. This was done to avoid collisions and provide a consistent API for all systems
    //!
    //! \param[in] value little endian value to convert to host endianness
    //! \return returns \a value in the host CPU's endianness
    M_NODISCARD static M_INLINE uint64_t le64_to_host(uint64_t value)
    {
#if defined(ENV_BIG_ENDIAN)
        return by_swap_64(value);
#else // Assume little endian
    return value;
#endif
    }

    //! \fn static M_INLINE void big_To_Little_Endian_16(uint16_t* wordToSwap)
    //! \brief takes a big endian uint16_t and returns it in host endianness
    //!
    //! recommend migrating to be16_to_host
    //!
    //! \param[in] value big endian value to convert to host endianness
    //! \return returns \a value in the host CPU's endianness
    //! \sa be16_to_host
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void big_To_Little_Endian_16(uint16_t* wordToSwap)
    {
        DISABLE_NONNULL_COMPARE
        if (wordToSwap != M_NULLPTR)
        {
            *wordToSwap = be16_to_host(*wordToSwap);
        }
        RESTORE_NONNULL_COMPARE
    }

    //! \fn static M_INLINE void big_To_Little_Endian_32(uint32_t* doubleWordToSwap)
    //! \brief takes a big endian uint32_t and returns it in host endianness
    //!
    //! recommend migrating to be32_to_host
    //!
    //! \param[in] value big endian value to convert to host endianness
    //! \return returns \a value in the host CPU's endianness
    //! \sa be32_to_host
    M_NONNULL_PARAM_LIST(1) M_PARAM_RW(1)
    static M_INLINE void big_To_Little_Endian_32(uint32_t* doubleWordToSwap)
    {
        DISABLE_NONNULL_COMPARE
        if (doubleWordToSwap != M_NULLPTR)
        {
            *doubleWordToSwap = be32_to_host(*doubleWordToSwap);
        }
        RESTORE_NONNULL_COMPARE
    }

    // C23-like bit functions. These have similar names so they do not collide with the standard implementation

    //! \fn static M_INLINE unsigned int count_leading_zeros_uc(unsigned char value)
    //! \brief counts the number of consecutive ​0​ bits, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return number of leading zeros in \a value
    static M_INLINE unsigned int count_leading_zeros_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_LEADING_ZEROS)
        return __builtin_stdc_leading_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_leading_zeros(value);
#elif defined(HAVE_BUILT_IN_CLZ)
    return value == 0U ? M_STATIC_CAST(unsigned int, UCHAR_WIDTH)
                       : M_STATIC_CAST(unsigned int, __builtin_clz(value) - (UINT_WIDTH - UCHAR_WIDTH));
#else
    unsigned int count = 0U;
    while (value > 0U)
    {
        if (value & UCHAR_MSB)
        {
            break;
        }
        ++count;
        value <<= 1;
    }
    return count;
#endif
    }

    //! \fn static M_INLINE unsigned int count_leading_zeros_us(unsigned short value)
    //! \brief counts the number of consecutive ​0​ bits, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return number of leading zeros in \a value
    static M_INLINE unsigned int count_leading_zeros_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_LEADING_ZEROS)
        return __builtin_stdc_leading_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_leading_zeros(value);
#elif defined(HAVE_BUILT_IN_CLZ)
    return value == 0U ? M_STATIC_CAST(unsigned int, USHRT_WIDTH)
                       : M_STATIC_CAST(unsigned int, __builtin_clz(value) - (UINT_WIDTH - USHRT_WIDTH));
#else
    unsigned int count = 0U;
    while (value > 0U)
    {
        if (value & USHRT_MSB)
        {
            break;
        }
        ++count;
        value <<= 1;
    }
    return count;
#endif
    }

    //! \fn static M_INLINE unsigned int count_leading_zeros_ui(unsigned int value)
    //! \brief counts the number of consecutive ​0​ bits, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return number of leading zeros in \a value
    static M_INLINE unsigned int count_leading_zeros_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_LEADING_ZEROS)
        return __builtin_stdc_leading_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_leading_zeros(value);
#elif defined(HAVE_BUILT_IN_CLZ)
    return value == 0U ? M_STATIC_CAST(unsigned int, UINT_WIDTH) : M_STATIC_CAST(unsigned int, __builtin_clz(value));
#else
    unsigned int count = 0U;
    while (value > 0U)
    {
        if (value & UINT_MSB)
        {
            break;
        }
        ++count;
        value <<= 1;
    }
    return count;
#endif
    }

    //! \fn static M_INLINE unsigned int count_leading_zeros_ul(unsigned long value)
    //! \brief counts the number of consecutive ​0​ bits, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return number of leading zeros in \a value
    static M_INLINE unsigned int count_leading_zeros_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_LEADING_ZEROS)
        return __builtin_stdc_leading_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_leading_zeros(value);
#elif defined(HAVE_BUILT_IN_CLZL)
    return value == 0UL ? M_STATIC_CAST(unsigned int, ULONG_WIDTH) : M_STATIC_CAST(unsigned int, __builtin_clzl(value));
#else
    unsigned int count = 0U;
    while (value > 0UL)
    {
        if (value & ULONG_MSB)
        {
            break;
        }
        ++count;
        value <<= 1;
    }
    return count;
#endif
    }

    //! \fn static M_INLINE unsigned int count_leading_zeros_ull(unsigned long long value)
    //! \brief counts the number of consecutive ​0​ bits, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return number of leading zeros in \a value
    static M_INLINE unsigned int count_leading_zeros_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_LEADING_ZEROS)
        return __builtin_stdc_leading_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_leading_zeros(value);
#elif defined(HAVE_BUILT_IN_CLZLL)
    return value == 0ULL ? M_STATIC_CAST(unsigned int, ULLONG_WIDTH)
                         : M_STATIC_CAST(unsigned int, __builtin_clzll(value));
#else
    unsigned int count = 0U;
    while (value > 0ULL)
    {
        if (value & ULLONG_MSB)
        {
            break;
        }
        ++count;
        value <<= 1;
    }
    return count;
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define count_leading_zeros(value)                                                                                 \
        _Generic((value), unsigned char                                                                                \
                 : count_leading_zeros_uc, unsigned short                                                              \
                 : count_leading_zeros_us, unsigned int                                                                \
                 : count_leading_zeros_ui, unsigned long                                                               \
                 : count_leading_zeros_ul, unsigned long long                                                          \
                 : count_leading_zeros_ull)(value)
#else
#    define count_leading_zeros(value)                                                                                 \
        (sizeof(value) == sizeof(unsigned char)    ? count_leading_zeros_uc(value)                                     \
         : sizeof(value) == sizeof(unsigned short) ? count_leading_zeros_us(value)                                     \
         : sizeof(value) == sizeof(unsigned int)   ? count_leading_zeros_ui(value)                                     \
         : sizeof(value) == sizeof(unsigned long)  ? count_leading_zeros_ul(value)                                     \
                                                   : count_leading_zeros_ull(value))
#endif

    //! \fn static M_INLINE unsigned int count_leading_ones_uc(unsigned char value)
    //! \brief counts the number of consecutive 1 bits, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return number of leading ones in \a value
    static M_INLINE unsigned int count_leading_ones_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_LEADING_ONES)
        return __builtin_stdc_leading_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_leading_ones(value);
#else
    return count_leading_zeros_uc(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_leading_ones_us(unsigned short value)
    //! \brief counts the number of consecutive 1 bits, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return number of leading ones in \a value
    static M_INLINE unsigned int count_leading_ones_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_LEADING_ONES)
        return __builtin_stdc_leading_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_leading_ones(value);
#else
    return count_leading_zeros_us(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_leading_ones_ui(unsigned int value)
    //! \brief counts the number of consecutive 1 bits, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return number of leading ones in \a value
    static M_INLINE unsigned int count_leading_ones_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_LEADING_ONES)
        return __builtin_stdc_leading_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_leading_ones(value);
#else
    return count_leading_zeros_ui(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_leading_ones_ul(unsigned long value)
    //! \brief counts the number of consecutive 1 bits, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return number of leading ones in \a value
    static M_INLINE unsigned int count_leading_ones_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_LEADING_ONES)
        return __builtin_stdc_leading_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_leading_ones(value);
#else
    return count_leading_zeros_ul(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_leading_ones_ull(unsigned long long value)
    //! \brief counts the number of consecutive 1 bits, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return number of leading ones in \a value
    static M_INLINE unsigned int count_leading_ones_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_LEADING_ONES)
        return __builtin_stdc_leading_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_leading_ones(value);
#else
    return count_leading_zeros_ull(~value);
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define count_leading_ones(value)                                                                                  \
        _Generic((value), unsigned char                                                                                \
                 : count_leading_ones_uc, unsigned short                                                               \
                 : count_leading_ones_us, unsigned int                                                                 \
                 : count_leading_ones_ui, unsigned long                                                                \
                 : count_leading_ones_ul, unsigned long long                                                           \
                 : count_leading_ones_ull)(value)
#else
#    define count_leading_ones(value)                                                                                  \
        (sizeof(value) == sizeof(unsigned char)    ? count_leading_ones_uc(value)                                      \
         : sizeof(value) == sizeof(unsigned short) ? count_leading_ones_us(value)                                      \
         : sizeof(value) == sizeof(unsigned int)   ? count_leading_ones_ui(value)                                      \
         : sizeof(value) == sizeof(unsigned long)  ? count_leading_ones_ul(value)                                      \
                                                   : count_leading_ones_ull(value))
#endif

    //! \fn static M_INLINE unsigned int count_trailing_zeros_uc(unsigned char value)
    //! \brief counts the number of consecutive ​0 bits, starting from the least significant bit
    //! \param[in] value the value to assess
    //! \return number of trailing zeroes in \a value
    static M_INLINE unsigned int count_trailing_zeros_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_TRAILING_ZEROS)
        return __builtin_stdc_trailing_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_trailing_zeros(value);
#elif defined(HAVE_BUILT_IN_CTZ)
    return value == 0U ? UCHAR_WIDTH : M_STATIC_CAST(unsigned int, __builtin_ctz(value));
#else
    unsigned int count = 0U;
    while (value > 0U)
    {
        if (value & 0x1U)
        {
            break;
        }
        else
        {
            ++count;
        }
        value >>= 1;
    }
    return count;
#endif
    }

    //! \fn static M_INLINE unsigned int count_trailing_zeros_us(unsigned short value)
    //! \brief counts the number of consecutive ​0 bits, starting from the least significant bit
    //! \param[in] value the value to assess
    //! \return number of trailing zeroes in \a value
    static M_INLINE unsigned int count_trailing_zeros_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_TRAILING_ZEROS)
        return __builtin_stdc_trailing_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_trailing_zeros(value);
#elif defined(HAVE_BUILT_IN_CTZ)
    return value == 0U ? USHRT_WIDTH : M_STATIC_CAST(unsigned int, __builtin_ctz(value));
#else
    unsigned int count = 0U;
    while (value > 0U)
    {
        if (value & 0x1U)
        {
            break;
        }
        else
        {
            ++count;
        }
        value >>= 1;
    }
    return count;
#endif
    }

    //! \fn static M_INLINE unsigned int count_trailing_zeros_ui(unsigned int value)
    //! \brief counts the number of consecutive ​0 bits, starting from the least significant bit
    //! \param[in] value the value to assess
    //! \return number of trailing zeroes in \a value
    static M_INLINE unsigned int count_trailing_zeros_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_TRAILING_ZEROS)
        return __builtin_stdc_trailing_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_trailing_zeros(value);
#elif defined(HAVE_BUILT_IN_CTZ)
    return value == 0U ? UINT_WIDTH : M_STATIC_CAST(unsigned int, __builtin_ctz(value));
#else
    unsigned int count = 0U;
    while (value > 0U)
    {
        if (value & 0x1U)
        {
            break;
        }
        else
        {
            ++count;
        }
        value >>= 1;
    }
    return count;
#endif
    }

    //! \fn static M_INLINE unsigned int count_trailing_zeros_ul(unsigned long value)
    //! \brief counts the number of consecutive ​0 bits, starting from the least significant bit
    //! \param[in] value the value to assess
    //! \return number of trailing zeroes in \a value
    static M_INLINE unsigned int count_trailing_zeros_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_TRAILING_ZEROS)
        return __builtin_stdc_trailing_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_trailing_zeros(value);
#elif defined(HAVE_BUILT_IN_CTZL)
    return value == 0UL ? ULONG_WIDTH : M_STATIC_CAST(unsigned int, __builtin_ctzl(value));
#else
    unsigned int count = 0U;
    while (value > 0UL)
    {
        if (value & 0x1UL)
        {
            break;
        }
        else
        {
            ++count;
        }
        value >>= 1;
    }
    return count;
#endif
    }

    //! \fn static M_INLINE unsigned int count_trailing_zeros_ull(unsigned long long value)
    //! \brief counts the number of consecutive ​0 bits, starting from the least significant bit
    //! \param[in] value the value to assess
    //! \return number of trailing zeroes in \a value
    static M_INLINE unsigned int count_trailing_zeros_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_TRAILING_ZEROS)
        return __builtin_stdc_trailing_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_trailing_zeros(value);
#elif defined(HAVE_BUILT_IN_CTZLL)
    return value == 0ULL ? ULLONG_WIDTH : M_STATIC_CAST(unsigned int, __builtin_ctzll(value));
#else
    unsigned int count = 0U;
    while (value > 0ULL)
    {
        if (value & 0x1ULL)
        {
            break;
        }
        else
        {
            ++count;
        }
        value >>= 1;
    }
    return count;
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define count_trailing_zeros(value)                                                                                \
        _Generic((value), unsigned char                                                                                \
                 : count_trailing_zeros_uc, unsigned short                                                             \
                 : count_trailing_zeros_us, unsigned int                                                               \
                 : count_trailing_zeros_ui, unsigned long                                                              \
                 : count_trailing_zeros_ul, unsigned long long                                                         \
                 : count_trailing_zeros_ull)(value)
#else
#    define count_trailing_zeros(value)                                                                                \
        (sizeof(value) == sizeof(unsigned char)    ? count_trailing_zeros_uc(value)                                    \
         : sizeof(value) == sizeof(unsigned short) ? count_trailing_zeros_us(value)                                    \
         : sizeof(value) == sizeof(unsigned int)   ? count_trailing_zeros_ui(value)                                    \
         : sizeof(value) == sizeof(unsigned long)  ? count_trailing_zeros_ul(value)                                    \
                                                   : count_trailing_zeros_ull(value))
#endif

    //! \fn static M_INLINE unsigned int count_trailing_ones_uc(unsigned char value)
    //! \brief counts the number of consecutive 1 bits, starting from the least significant bit
    //! \param[in] value the value to assess
    //! \return number of trailing ones in \a value
    static M_INLINE unsigned int count_trailing_ones_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_TRAILING_ONES)
        return __builtin_stdc_trailing_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_trailing_ones(value);
#else
    return count_trailing_zeros_uc(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_trailing_ones_us(unsigned short value)
    //! \brief counts the number of consecutive 1 bits, starting from the least significant bit
    //! \param[in] value the value to assess
    //! \return number of trailing ones in \a value
    static M_INLINE unsigned int count_trailing_ones_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_TRAILING_ONES)
        return __builtin_stdc_trailing_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_trailing_ones(value);
#else
    return count_trailing_zeros_us(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_trailing_ones_ui(unsigned int value)
    //! \brief counts the number of consecutive 1 bits, starting from the least significant bit
    //! \param[in] value the value to assess
    //! \return number of trailing ones in \a value
    static M_INLINE unsigned int count_trailing_ones_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_TRAILING_ONES)
        return __builtin_stdc_trailing_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_trailing_ones(value);
#else
    return count_trailing_zeros_ui(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_trailing_ones_ul(unsigned long value)
    //! \brief counts the number of consecutive 1 bits, starting from the least significant bit
    //! \param[in] value the value to assess
    //! \return number of trailing ones in \a value
    static M_INLINE unsigned int count_trailing_ones_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_TRAILING_ONES)
        return __builtin_stdc_trailing_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_trailing_ones(value);
#else
    return count_trailing_zeros_ul(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_trailing_ones_ull(unsigned long long value)
    //! \brief counts the number of consecutive 1 bits, starting from the least significant bit
    //! \param[in] value the value to assess
    //! \return number of trailing ones in \a value
    static M_INLINE unsigned int count_trailing_ones_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_TRAILING_ONES)
        return __builtin_stdc_trailing_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_trailing_ones(value);
#else
    return count_trailing_zeros_ull(~value);
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define count_trailing_ones(value)                                                                                 \
        _Generic((value), unsigned char                                                                                \
                 : count_trailing_ones_uc, unsigned short                                                              \
                 : count_trailing_ones_us, unsigned int                                                                \
                 : count_trailing_ones_ui, unsigned long                                                               \
                 : count_trailing_ones_ul, unsigned long long                                                          \
                 : count_trailing_ones_ull)(value)
#else
#    define count_trailing_ones(value)                                                                                 \
        (sizeof(value) == sizeof(unsigned char)    ? count_trailing_ones_uc(value)                                     \
         : sizeof(value) == sizeof(unsigned short) ? count_trailing_ones_us(value)                                     \
         : sizeof(value) == sizeof(unsigned int)   ? count_trailing_ones_ui(value)                                     \
         : sizeof(value) == sizeof(unsigned long)  ? count_trailing_ones_ul(value)                                     \
                                                   : count_trailing_ones_ull(value))
#endif

    //! \fn static M_INLINE unsigned int first_leading_one_uc(unsigned char value)
    //! \brief finds the first position of 1 bit, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return the position of the first bit set to one in \a value
    static M_INLINE unsigned int first_leading_one_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_FIRST_LEADING_ONE)
        return __builtin_stdc_first_leading_one(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_first_leading_one(value);
#elif defined(HAVE_BUILT_IN_CLZ)
    return value == 0U ? 0U : M_STATIC_CAST(unsigned int, __builtin_clz(value) - (UINT_WIDTH - UCHAR_WIDTH) + 1);
#else
    unsigned int pos = UCHAR_WIDTH;
    while (value > 0U)
    {
        if (value & UCHAR_MSB)
        {
            break;
        }
        --pos;
        value <<= 1;
    }
    return pos;
#endif
    }

    //! \fn static M_INLINE unsigned int first_leading_one_us(unsigned short value)
    //! \brief finds the first position of 1 bit, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return the position of the first bit set to one in \a value
    static M_INLINE unsigned int first_leading_one_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_FIRST_LEADING_ONE)
        return __builtin_stdc_first_leading_one(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_first_leading_one(value);
#elif defined(HAVE_BUILT_IN_CLZ)
    return value == 0U ? 0U : M_STATIC_CAST(unsigned int, __builtin_clz(value) - (UINT_WIDTH - USHRT_WIDTH) + 1);
#else
    unsigned int pos = USHRT_WIDTH;
    while (value > 0U)
    {
        if (value & USHRT_MSB)
        {
            break;
        }
        --pos;
        value <<= 1;
    }
    return pos;
#endif
    }

    //! \fn static M_INLINE unsigned int first_leading_one_ui(unsigned int value)
    //! \brief finds the first position of 1 bit, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return the position of the first bit set to one in \a value
    static M_INLINE unsigned int first_leading_one_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_FIRST_LEADING_ONE)
        return __builtin_stdc_first_leading_one(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_first_leading_one(value);
#elif defined(HAVE_BUILT_IN_CLZ)
    return value == 0U ? 0U : M_STATIC_CAST(unsigned int, __builtin_clz(value) + 1);
#else
    unsigned int pos = UINT_WIDTH;
    while (value > 0U)
    {
        if (value & UINT_MSB)
        {
            break;
        }
        --pos;
        value <<= 1;
    }
    return pos;
#endif
    }

    //! \fn static M_INLINE unsigned int first_leading_one_ul(unsigned long value)
    //! \brief finds the first position of 1 bit, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return the position of the first bit set to one in \a value
    static M_INLINE unsigned int first_leading_one_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_FIRST_LEADING_ONE)
        return __builtin_stdc_first_leading_one(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_first_leading_one(value);
#elif defined(HAVE_BUILT_IN_CLZL)
    return value == 0UL ? 0U : M_STATIC_CAST(unsigned int, __builtin_clzl(value) + 1);
#else
    unsigned int pos = ULONG_WIDTH;
    while (value > 0UL)
    {
        if (value & ULONG_MSB)
        {
            break;
        }
        --pos;
        value <<= 1;
    }
    return pos;
#endif
    }

    //! \fn static M_INLINE unsigned int first_leading_one_ull(unsigned long long value)
    //! \brief finds the first position of 1 bit, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return the position of the first bit set to one in \a value
    static M_INLINE unsigned int first_leading_one_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_FIRST_LEADING_ONE)
        return __builtin_stdc_first_leading_one(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_first_leading_one(value);
#elif defined(HAVE_BUILT_IN_CLZLL)
    return value == 0ULL ? 0U : M_STATIC_CAST(unsigned int, __builtin_clzll(value) + 1);
#else
    unsigned int pos = ULLONG_WIDTH;
    while (value > 0ULL)
    {
        if (value & ULLONG_MSB)
        {
            break;
        }
        --pos;
        value <<= 1;
    }
    return pos;
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define first_leading_one(value)                                                                                   \
        _Generic((value), unsigned char                                                                                \
                 : first_leading_one_uc, unsigned short                                                                \
                 : first_leading_one_us, unsigned int                                                                  \
                 : first_leading_one_ui, unsigned long                                                                 \
                 : first_leading_one_ul, unsigned long long                                                            \
                 : first_leading_one_ull)(value)
#else
#    define first_leading_one(value)                                                                                   \
        (sizeof(value) == sizeof(unsigned char)    ? first_leading_one_uc(value)                                       \
         : sizeof(value) == sizeof(unsigned short) ? first_leading_one_us(value)                                       \
         : sizeof(value) == sizeof(unsigned int)   ? first_leading_one_ui(value)                                       \
         : sizeof(value) == sizeof(unsigned long)  ? first_leading_one_ul(value)                                       \
                                                   : first_leading_one_ull(value))
#endif

    //! \fn static M_INLINE unsigned int first_leading_zero_uc(unsigned char value)
    //! \brief finds the first position of 0 bit, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return the position of the highest bit set to one in \a value
    static M_INLINE unsigned int first_leading_zero_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_FIRST_LEADING_ZERO)
        return __builtin_stdc_first_leading_zero(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_first_leading_zero(value);
#else
    return first_leading_one_uc(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int first_leading_zero_us(unsigned short value)
    //! \brief finds the first position of 0 bit, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return the position of the highest bit set to one in \a value
    static M_INLINE unsigned int first_leading_zero_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_FIRST_LEADING_ZERO)
        return __builtin_stdc_first_leading_zero(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_first_leading_zero(value);
#else
    return first_leading_one_us(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int first_leading_zero_ui(unsigned int value)
    //! \brief finds the first position of 0 bit, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return the position of the highest bit set to one in \a value
    static M_INLINE unsigned int first_leading_zero_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_FIRST_LEADING_ZERO)
        return __builtin_stdc_first_leading_zero(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_first_leading_zero(value);
#else
    return first_leading_one_ui(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int first_leading_zero_ul(unsigned long value)
    //! \brief finds the first position of 0 bit, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return the position of the highest bit set to one in \a value
    static M_INLINE unsigned int first_leading_zero_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_FIRST_LEADING_ZERO)
        return __builtin_stdc_first_leading_zero(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_first_leading_zero(value);
#else
    return first_leading_one_ul(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int first_leading_zero_ull(unsigned long long value)
    //! \brief finds the first position of 0 bit, starting from the most significant bit
    //! \param[in] value the value to assess
    //! \return the position of the highest bit set to one in \a value
    static M_INLINE unsigned int first_leading_zero_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_FIRST_LEADING_ZERO)
        return __builtin_stdc_first_leading_zero(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_first_leading_zero(value);
#else
    return first_leading_one_ull(~value);
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define first_leading_zero(value)                                                                                  \
        _Generic((value), unsigned char                                                                                \
                 : first_leading_zero_uc, unsigned short                                                               \
                 : first_leading_zero_us, unsigned int                                                                 \
                 : first_leading_zero_ui, unsigned long                                                                \
                 : first_leading_zero_ul, unsigned long long                                                           \
                 : first_leading_zero_ull)(value)
#else
#    define first_leading_zero(value)                                                                                  \
        (sizeof(value) == sizeof(unsigned char)    ? first_leading_zero_uc(value)                                      \
         : sizeof(value) == sizeof(unsigned short) ? first_leading_zero_us(value)                                      \
         : sizeof(value) == sizeof(unsigned int)   ? first_leading_zero_ui(value)                                      \
         : sizeof(value) == sizeof(unsigned long)  ? first_leading_zero_ul(value)                                      \
                                                   : first_leading_zero_ull(value))
#endif

    //! \fn static M_INLINE unsigned int count_ones_uc(unsigned char value)
    //! \brief counts the number of 1 bits in an unsigned integer
    //! \param[in] value the value to assess
    //! \return the number of bits set to 1 in \a value
    static M_INLINE unsigned int count_ones_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_COUNT_ONES)
        return __builtin_stdc_count_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_count_ones(value);
#elif defined(HAVE_BUILT_IN_POPCOUNT)
    return M_STATIC_CAST(unsigned int, __builtin_popcount(value));
#else
    value -= (value >> 1) & 0x55U;
    value = (value & 0x33U) + ((value >> 2) & 0x33U);
    value = (value + (value >> 4)) & 0x0FU;
    return value;
#endif
    }

    //! \fn static M_INLINE unsigned int count_ones_us(unsigned short value)
    //! \brief counts the number of 1 bits in an unsigned integer
    //! \param[in] value the value to assess
    //! \return the number of bits set to 1 in \a value
    static M_INLINE unsigned int count_ones_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_COUNT_ONES)
        return __builtin_stdc_count_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_count_ones(value);
#elif defined(HAVE_BUILT_IN_POPCOUNT)
    return M_STATIC_CAST(unsigned int, __builtin_popcount(value));
#else
    value -= (value >> 1) & 0x5555U;
    value = (value & 0x3333U) + ((value >> 2) & 0x3333U);
    value = (value + (value >> 4)) & 0x0F0FU;
    value += value >> 8;
    return value & 0x1FU;
#endif
    }

    //! \fn static M_INLINE unsigned int count_ones_ui(unsigned int value)
    //! \brief counts the number of 1 bits in an unsigned integer
    //! \param[in] value the value to assess
    //! \return the number of bits set to 1 in \a value
    static M_INLINE unsigned int count_ones_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_COUNT_ONES)
        return __builtin_stdc_count_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_count_ones(value);
#elif defined(HAVE_BUILT_IN_POPCOUNT)
    return M_STATIC_CAST(unsigned int, __builtin_popcount(value));
#else
    value -= (value >> 1) & 0x55555555U;
    value = (value & 0x33333333U) + ((value >> 2) & 0x33333333U);
    value = (value + (value >> 4)) & 0x0F0F0F0FU;
    value += value >> 8;
    value += value >> 16;
    return value & 0x3FU;
#endif
    }

    //! \fn static M_INLINE unsigned int count_ones_ul(unsigned long value)
    //! \brief counts the number of 1 bits in an unsigned integer
    //! \param[in] value the value to assess
    //! \return the number of bits set to 1 in \a value
    static M_INLINE unsigned int count_ones_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_COUNT_ONES)
        return __builtin_stdc_count_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_count_ones(value);
#elif defined(HAVE_BUILT_IN_POPCOUNTL)
    return M_STATIC_CAST(unsigned int, __builtin_popcountl(value));
#else
#    if defined(LP64_DATA_MODEL) || defined(ILP64_DATA_MODEL)
    value -= (value >> 1) & 0x5555555555555555UL;
    value = (value & 0x3333333333333333UL) + ((value >> 2) & 0x3333333333333333UL);
    value = (value + (value >> 4)) & 0x0F0F0F0F0F0F0F0FUL;
    value += value >> 8;
    value += value >> 16;
    value += value >> 32;
    return value & 0x7FUL;
#    else
    value -= (value >> 1) & 0x55555555UL;
    value = (value & 0x33333333UL) + ((value >> 2) & 0x33333333UL);
    value = (value + (value >> 4)) & 0x0F0F0F0FUL;
    value += value >> 8;
    value += value >> 16;
    return value & 0x3FUL;
#    endif
#endif
    }

    //! \fn static M_INLINE unsigned int count_ones_ull(unsigned long long value)
    //! \brief counts the number of 1 bits in an unsigned integer
    //! \param[in] value the value to assess
    //! \return the number of bits set to 1 in \a value
    static M_INLINE unsigned int count_ones_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_COUNT_ONES)
        return __builtin_stdc_count_ones(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_count_ones(value);
#elif defined(HAVE_BUILT_IN_POPCOUNTLL)
    return M_STATIC_CAST(unsigned int, __builtin_popcountll(value));
#else
    value -= (value >> 1) & 0x5555555555555555ULL;
    value = (value & 0x3333333333333333ULL) + ((value >> 2) & 0x3333333333333333ULL);
    value = (value + (value >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
    value += value >> 8;
    value += value >> 16;
    value += value >> 32;
    return value & 0x7FULL;
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define count_ones(value)                                                                                          \
        _Generic((value), unsigned char                                                                                \
                 : count_ones_uc, unsigned short                                                                       \
                 : count_ones_us, unsigned int                                                                         \
                 : count_ones_ui, unsigned long                                                                        \
                 : count_ones_ul, unsigned long long                                                                   \
                 : count_ones_ull)(value)
#else
#    define count_ones(value)                                                                                          \
        (sizeof(value) == sizeof(unsigned char)    ? count_ones_uc(value)                                              \
         : sizeof(value) == sizeof(unsigned short) ? count_ones_us(value)                                              \
         : sizeof(value) == sizeof(unsigned int)   ? count_ones_ui(value)                                              \
         : sizeof(value) == sizeof(unsigned long)  ? count_ones_ul(value)                                              \
                                                   : count_ones_ull(value))
#endif

    //! \fn static M_INLINE unsigned int count_zeros_uc(unsigned char value)
    //! \brief counts the number of ​0​ bits in an unsigned integer
    //! \param[in] value the value to assess
    //! \return the number of bits set to 0 in \a value
    static M_INLINE unsigned int count_zeros_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_COUNT_ZEROS)
        return __builtin_stdc_count_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_count_zeros(value);
#else
    return count_ones_uc(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_zeros_us(unsigned short value)
    //! \brief counts the number of ​0​ bits in an unsigned integer
    //! \param[in] value the value to assess
    //! \return the number of bits set to 0 in \a value
    static M_INLINE unsigned int count_zeros_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_COUNT_ZEROS)
        return __builtin_stdc_count_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_count_zeros(value);
#else
    return count_ones_us(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_zeros_ui(unsigned int value)
    //! \brief counts the number of ​0​ bits in an unsigned integer
    //! \param[in] value the value to assess
    //! \return the number of bits set to 0 in \a value
    static M_INLINE unsigned int count_zeros_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_COUNT_ZEROS)
        return __builtin_stdc_count_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_count_zeros(value);
#else
    return count_ones_ui(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_zeros_ul(unsigned long value)
    //! \brief counts the number of ​0​ bits in an unsigned integer
    //! \param[in] value the value to assess
    //! \return the number of bits set to 0 in \a value
    static M_INLINE unsigned int count_zeros_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_COUNT_ZEROS)
        return __builtin_stdc_count_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_count_zeros(value);
#else
    return count_ones_ul(~value);
#endif
    }

    //! \fn static M_INLINE unsigned int count_zeros_ull(unsigned long long value)
    //! \brief counts the number of ​0​ bits in an unsigned integer
    //! \param[in] value the value to assess
    //! \return the number of bits set to 0 in \a value
    static M_INLINE unsigned int count_zeros_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_COUNT_ZEROS)
        return __builtin_stdc_count_zeros(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_count_zeros(value);
#else
    return count_ones_ull(~value);
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define count_zeros(value)                                                                                         \
        _Generic((value), unsigned char                                                                                \
                 : count_zeros_uc, unsigned short                                                                      \
                 : count_zeros_us, unsigned int                                                                        \
                 : count_zeros_ui, unsigned long                                                                       \
                 : count_zeros_ul, unsigned long long                                                                  \
                 : count_zeros_ull)(value)
#else
#    define count_zeros(value)                                                                                         \
        (sizeof(value) == sizeof(unsigned char)    ? count_zeros_uc(value)                                             \
         : sizeof(value) == sizeof(unsigned short) ? count_zeros_us(value)                                             \
         : sizeof(value) == sizeof(unsigned int)   ? count_zeros_ui(value)                                             \
         : sizeof(value) == sizeof(unsigned long)  ? count_zeros_ul(value)                                             \
                                                   : count_zeros_ull(value))
#endif

    //! \fn static M_INLINE bool has_single_bit_uc(unsigned char value)
    //! \brief checks if a number is an integral power of 2 or only one bit is set to 1
    //! \param[in] value the value to assess
    //! \return true when only one bit is set to one, otherwise false
    static M_INLINE bool has_single_bit_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_HAS_SINGLE_BIT)
        return __builtin_stdc_has_single_bit(value);
#else
    return value != 0U && (value & (value - 1U)) == 0U;
#endif
    }

    //! \fn static M_INLINE bool has_single_bit_us(unsigned short value)
    //! \brief checks if a number is an integral power of 2 or only one bit is set to 1
    //! \param[in] value the value to assess
    //! \return true when only one bit is set to one, otherwise false
    static M_INLINE bool has_single_bit_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_HAS_SINGLE_BIT)
        return __builtin_stdc_has_single_bit(value);
#else
    return value != 0U && (value & (value - 1U)) == 0U;
#endif
    }

    //! \fn static M_INLINE bool has_single_bit_ui(unsigned int value)
    //! \brief checks if a number is an integral power of 2 or only one bit is set to 1
    //! \param[in] value the value to assess
    //! \return true when only one bit is set to one, otherwise false
    static M_INLINE bool has_single_bit_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_HAS_SINGLE_BIT)
        return __builtin_stdc_has_single_bit(value);
#else
    return value != 0U && (value & (value - 1U)) == 0U;
#endif
    }

    //! \fn static M_INLINE bool has_single_bit_ul(unsigned long value)
    //! \brief checks if a number is an integral power of 2 or only one bit is set to 1
    //! \param[in] value the value to assess
    //! \return true when only one bit is set to one, otherwise false
    static M_INLINE bool has_single_bit_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_HAS_SINGLE_BIT)
        return __builtin_stdc_has_single_bit(value);
#else
    return value != 0UL && (value & (value - 1UL)) == 0UL;
#endif
    }

    //! \fn static M_INLINE bool has_single_bit_ull(unsigned long long value)
    //! \brief checks if a number is an integral power of 2 or only one bit is set to 1
    //! \param[in] value the value to assess
    //! \return true when only one bit is set to one, otherwise false
    static M_INLINE bool has_single_bit_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_HAS_SINGLE_BIT)
        return __builtin_stdc_has_single_bit(value);
#else
    return value != 0ULL && (value & (value - 1ULL)) == 0ULL;
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define has_single_bit(value)                                                                                      \
        _Generic((value), unsigned char                                                                                \
                 : has_single_bit_uc, unsigned short                                                                   \
                 : has_single_bit_us, unsigned int                                                                     \
                 : has_single_bit_ui, unsigned long                                                                    \
                 : has_single_bit_ul, unsigned long long                                                               \
                 : has_single_bit_ull)(value)
#else
#    define has_single_bit(value)                                                                                      \
        (sizeof(value) == sizeof(unsigned char)    ? has_single_bit_uc(value)                                          \
         : sizeof(value) == sizeof(unsigned short) ? has_single_bit_us(value)                                          \
         : sizeof(value) == sizeof(unsigned int)   ? has_single_bit_ui(value)                                          \
         : sizeof(value) == sizeof(unsigned long)  ? has_single_bit_ul(value)                                          \
                                                   : has_single_bit_ull(value))
#endif

    //! \fn static M_INLINE unsigned int get_req_bit_width_uc(unsigned char value)
    //! \brief finds the smallest number of bits needed to represent the given value
    //! \param[in] value the value to assess
    //! \return the number of bits required to represent \a value
    static M_INLINE unsigned int get_req_bit_width_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_WIDTH)
        return __builtin_stdc_bit_width(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_width(value);
#else
    return UCHAR_WIDTH - (value == 0U ? UCHAR_WIDTH : count_leading_zeros_uc(value));
#endif
    }

    //! \fn static M_INLINE unsigned int get_req_bit_width_us(unsigned short value)
    //! \brief finds the smallest number of bits needed to represent the given value
    //! \param[in] value the value to assess
    //! \return the number of bits required to represent \a value
    static M_INLINE unsigned int get_req_bit_width_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_WIDTH)
        return __builtin_stdc_bit_width(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_width(value);
#else
    return USHRT_WIDTH - (value == 0U ? USHRT_WIDTH : count_leading_zeros_us(value));
#endif
    }

    //! \fn static M_INLINE unsigned int get_req_bit_width_ui(unsigned int value)
    //! \brief finds the smallest number of bits needed to represent the given value
    //! \param[in] value the value to assess
    //! \return the number of bits required to represent \a value
    static M_INLINE unsigned int get_req_bit_width_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_WIDTH)
        return __builtin_stdc_bit_width(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_width(value);
#else
    return UINT_WIDTH - (value == 0U ? UINT_WIDTH : count_leading_zeros_ui(value));
#endif
    }

    //! \fn static M_INLINE unsigned int get_req_bit_width_ul(unsigned long value)
    //! \brief finds the smallest number of bits needed to represent the given value
    //! \param[in] value the value to assess
    //! \return the number of bits required to represent \a value
    static M_INLINE unsigned int get_req_bit_width_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_WIDTH)
        return __builtin_stdc_bit_width(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_width(value);
#else
    return ULONG_WIDTH - (value == 0UL ? ULONG_WIDTH : count_leading_zeros_ul(value));
#endif
    }

    //! \fn static M_INLINE unsigned int get_req_bit_width_ull(unsigned long long value)
    //! \brief finds the smallest number of bits needed to represent the given value
    //! \param[in] value the value to assess
    //! \return the number of bits required to represent \a value
    static M_INLINE unsigned int get_req_bit_width_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_WIDTH)
        return __builtin_stdc_bit_width(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_width(value);
#else
    return ULLONG_WIDTH - (value == 0ULL ? ULLONG_WIDTH : count_leading_zeros_ull(value));
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define get_req_bit_width(value)                                                                                   \
        _Generic((value), unsigned char                                                                                \
                 : get_req_bit_width_uc, unsigned short                                                                \
                 : get_req_bit_width_us, unsigned int                                                                  \
                 : get_req_bit_width_ui, unsigned long                                                                 \
                 : get_req_bit_width_ul, unsigned long long                                                            \
                 : get_req_bit_width_ull)(value)
#else
#    define get_req_bit_width(value)                                                                                   \
        (sizeof(value) == sizeof(unsigned char)    ? get_req_bit_width_uc(value)                                       \
         : sizeof(value) == sizeof(unsigned short) ? get_req_bit_width_us(value)                                       \
         : sizeof(value) == sizeof(unsigned int)   ? get_req_bit_width_ui(value)                                       \
         : sizeof(value) == sizeof(unsigned long)  ? get_req_bit_width_ul(value)                                       \
                                                   : get_req_bit_width_ull(value))
#endif

    //! \fn static M_INLINE unsigned char bit_floor_uc(unsigned char value)
    //! \brief finds the largest integral power of two not greater than the given \a value
    //! \param[in] value the value to assess
    //! \return power of two <= \a value
    static M_INLINE unsigned char bit_floor_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_FLOOR)
        return __builtin_stdc_bit_floor(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_floor(value);
#else
    return value == 0U ? 0U : 1U << (UCHAR_WIDTH - 1U - count_leading_zeros_uc(value));
#endif
    }

    //! \fn static M_INLINE unsigned short bit_floor_us(unsigned short value)
    //! \brief finds the largest integral power of two not greater than the given \a value
    //! \param[in] value the value to assess
    //! \return power of two <= \a value
    static M_INLINE unsigned short bit_floor_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_FLOOR)
        return __builtin_stdc_bit_floor(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_floor(value);
#else
    return value == 0U ? 0U : 1U << (USHRT_WIDTH - 1U - count_leading_zeros_us(value));
#endif
    }

    //! \fn static M_INLINE unsigned int bit_floor_ui(unsigned int value)
    //! \brief finds the largest integral power of two not greater than the given \a value
    //! \param[in] value the value to assess
    //! \return power of two <= \a value
    static M_INLINE unsigned int bit_floor_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_FLOOR)
        return __builtin_stdc_bit_floor(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_floor(value);
#else
    return value == 0U ? 0U : 1U << (UINT_WIDTH - 1U - count_leading_zeros_ui(value));
#endif
    }

    //! \fn static M_INLINE unsigned long bit_floor_ul(unsigned long value)
    //! \brief finds the largest integral power of two not greater than the given \a value
    //! \param[in] value the value to assess
    //! \return power of two <= \a value
    static M_INLINE unsigned long bit_floor_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_FLOOR)
        return __builtin_stdc_bit_floor(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_floor(value);
#else
    return value == 0UL ? 0UL : 1UL << (ULONG_WIDTH - 1UL - count_leading_zeros_ul(value));
#endif
    }

    //! \fn static M_INLINE unsigned long long bit_floor_ull(unsigned long long value)
    //! \brief finds the largest integral power of two not greater than the given \a value
    //! \param[in] value the value to assess
    //! \return power of two <= \a value
    static M_INLINE unsigned long long bit_floor_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_FLOOR)
        return __builtin_stdc_bit_floor(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_floor(value);
#else
    return value == 0ULL ? 0ULL : 1ULL << (ULLONG_WIDTH - 1ULL - count_leading_zeros_ull(value));
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define bit_floor(value)                                                                                           \
        _Generic((value), unsigned char                                                                                \
                 : bit_floor_uc, unsigned short                                                                        \
                 : bit_floor_us, unsigned int                                                                          \
                 : bit_floor_ui, unsigned long                                                                         \
                 : bit_floor_ul, unsigned long long                                                                    \
                 : bit_floor_ull)(value)
#else
#    define bit_floor(value)                                                                                           \
        (sizeof(value) == sizeof(unsigned char)    ? bit_floor_uc(value)                                               \
         : sizeof(value) == sizeof(unsigned short) ? bit_floor_us(value)                                               \
         : sizeof(value) == sizeof(unsigned int)   ? bit_floor_ui(value)                                               \
         : sizeof(value) == sizeof(unsigned long)  ? bit_floor_ul(value)                                               \
                                                   : bit_floor_ull(value))
#endif

    //! \fn static M_INLINE unsigned char bit_ceil_uc(unsigned char value)
    //! \brief finds the smallest integral power of two not less than the given \a value
    //! \param[in] value the value to assess
    //! \return power of two >= \a value
    static M_INLINE unsigned char bit_ceil_uc(unsigned char value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_CEIL)
        return __builtin_stdc_bit_ceil(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_ceil(value);
#else
    return value <= 1U ? 1U : 2U << (UCHAR_WIDTH - 1U - count_leading_zeros_uc(value - 1U));
#endif
    }

    //! \fn static M_INLINE unsigned short bit_ceil_us(unsigned short value)
    //! \brief finds the smallest integral power of two not less than the given \a value
    //! \param[in] value the value to assess
    //! \return power of two >= \a value
    static M_INLINE unsigned short bit_ceil_us(unsigned short value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_CEIL)
        return __builtin_stdc_bit_ceil(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_ceil(value);
#else
    return value <= 1U ? 1U : 2U << (USHRT_WIDTH - 1U - count_leading_zeros_us(value - 1U));
#endif
    }

    //! \fn static M_INLINE unsigned int bit_ceil_ui(unsigned int value)
    //! \brief finds the smallest integral power of two not less than the given \a value
    //! \param[in] value the value to assess
    //! \return power of two >= \a value
    static M_INLINE unsigned int bit_ceil_ui(unsigned int value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_CEIL)
        return __builtin_stdc_bit_ceil(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_ceil(value);
#else
    return value <= 1U ? 1U : 2U << (UINT_WIDTH - 1U - count_leading_zeros_ui(value - 1U));
#endif
    }

    //! \fn static M_INLINE unsigned long bit_ceil_ul(unsigned long value)
    //! \brief finds the smallest integral power of two not less than the given \a value
    //! \param[in] value the value to assess
    //! \return power of two >= \a value
    static M_INLINE unsigned long bit_ceil_ul(unsigned long value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_CEIL)
        return __builtin_stdc_bit_ceil(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_ceil(value);
#else
    return value <= 1UL ? 1UL : 2UL << (ULONG_WIDTH - 1UL - count_leading_zeros_ul(value - 1UL));
#endif
    }

    //! \fn static M_INLINE unsigned long long bit_ceil_ull(unsigned long long value)
    //! \brief finds the smallest integral power of two not less than the given \a value
    //! \param[in] value the value to assess
    //! \return power of two >= \a value
    static M_INLINE unsigned long long bit_ceil_ull(unsigned long long value)
    {
#if defined(HAVE_BUILT_IN_STDC_BIT_CEIL)
        return __builtin_stdc_bit_ceil(value);
#elif defined(HAVE_STDC_BIT)
    return stdc_bit_ceil(value);
#else
    return value <= 1ULL ? 1ULL : 2ULL << (ULLONG_WIDTH - 1ULL - count_leading_zeros_ull(value - 1ULL));
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define bit_ceil(value)                                                                                            \
        _Generic((value), unsigned char                                                                                \
                 : bit_ceil_uc, unsigned short                                                                         \
                 : bit_ceil_us, unsigned int                                                                           \
                 : bit_ceil_ui, unsigned long                                                                          \
                 : bit_ceil_ul, unsigned long long                                                                     \
                 : bit_ceil_ull)(value)
#else
#    define bit_ceil(value)                                                                                            \
        (sizeof(value) == sizeof(unsigned char)    ? bit_ceil_uc(value)                                                \
         : sizeof(value) == sizeof(unsigned short) ? bit_ceil_us(value)                                                \
         : sizeof(value) == sizeof(unsigned int)   ? bit_ceil_ui(value)                                                \
         : sizeof(value) == sizeof(unsigned long)  ? bit_ceil_ul(value)                                                \
                                                   : bit_ceil_ull(value))
#endif

    //! \fn static M_INLINE unsigned char rotate_left_uc(unsigned char value, unsigned int count)
    //! \brief rotates the value to the left by specified \a count
    //! \param[in] value the value to rotate
    //! \param[in] count number of bit positions to rotate to the left
    //! \return rotated \a value
    static M_INLINE unsigned char rotate_left_uc(unsigned char value, unsigned int count)
    {
#if defined(HAVE_BUILT_IN_STDC_ROTATE_LEFT)
        return __builtin_stdc_rotate_left(value, count);
#else
    return ((value << (count % UCHAR_WIDTH)) | (value >> (((~count) + 1U) % UCHAR_WIDTH)));
#endif
    }

    //! \fn static M_INLINE unsigned short rotate_left_us(unsigned short value, unsigned int count)
    //! \brief rotates the value to the left by specified \a count
    //! \param[in] value the value to rotate
    //! \param[in] count number of bit positions to rotate to the left
    //! \return rotated \a value
    static M_INLINE unsigned short rotate_left_us(unsigned short value, unsigned int count)
    {
#if defined(HAVE_BUILT_IN_STDC_ROTATE_LEFT)
        return __builtin_stdc_rotate_left(value, count);
#else
    return ((value << (count % USHRT_WIDTH)) | (value >> (((~count) + 1U) % USHRT_WIDTH)));
#endif
    }

    //! \fn static M_INLINE unsigned int rotate_left_ui(unsigned int value, unsigned int count)
    //! \brief rotates the value to the left by specified \a count
    //! \param[in] value the value to rotate
    //! \param[in] count number of bit positions to rotate to the left
    //! \return rotated \a value
    static M_INLINE unsigned int rotate_left_ui(unsigned int value, unsigned int count)
    {
#if defined(HAVE_BUILT_IN_STDC_ROTATE_LEFT)
        return __builtin_stdc_rotate_left(value, count);
#else
    return ((value << (count % UINT_WIDTH)) | (value >> (((~count) + 1U) % UINT_WIDTH)));
#endif
    }

    //! \fn static M_INLINE unsigned long rotate_left_ul(unsigned long value, unsigned int count)
    //! \brief rotates the value to the left by specified \a count
    //! \param[in] value the value to rotate
    //! \param[in] count number of bit positions to rotate to the left
    //! \return rotated \a value
    static M_INLINE unsigned long rotate_left_ul(unsigned long value, unsigned int count)
    {
#if defined(HAVE_BUILT_IN_STDC_ROTATE_LEFT)
        return __builtin_stdc_rotate_left(value, count);
#else
    return ((value << (count % ULONG_WIDTH)) | (value >> (((~count) + 1U) % ULONG_WIDTH)));
#endif
    }

    //! \fn static M_INLINE unsigned long long rotate_left_ull(unsigned long long value, unsigned int count)
    //! \brief rotates the value to the left by specified \a count
    //! \param[in] value the value to rotate
    //! \param[in] count number of bit positions to rotate to the left
    //! \return rotated \a value
    static M_INLINE unsigned long long rotate_left_ull(unsigned long long value, unsigned int count)
    {
#if defined(HAVE_BUILT_IN_STDC_ROTATE_LEFT)
        return __builtin_stdc_rotate_left(value, count);
#else
    return ((value << (count % ULLONG_WIDTH)) | (value >> (((~count) + 1U) % ULLONG_WIDTH)));
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define rotate_left(value, count)                                                                                  \
        _Generic((value), unsigned char                                                                                \
                 : rotate_left_uc, unsigned short                                                                      \
                 : rotate_left_us, unsigned int                                                                        \
                 : rotate_left_ui, unsigned long                                                                       \
                 : rotate_left_ul, unsigned long long                                                                  \
                 : rotate_left_ull)(value, count)
#else
#    define rotate_left(value, count)                                                                                  \
        (sizeof(value) == sizeof(unsigned char)    ? rotate_left_uc((unsigned char)(value), count)                     \
         : sizeof(value) == sizeof(unsigned short) ? rotate_left_us((unsigned short)(value), count)                    \
         : sizeof(value) == sizeof(unsigned int)   ? rotate_left_ui((unsigned int)(value), count)                      \
         : sizeof(value) == sizeof(unsigned long)  ? rotate_left_ul((unsigned long)(value), count)                     \
                                                   : rotate_left_ull((unsigned long long)(value), count))
#endif

    //! \fn static M_INLINE unsigned char rotate_right_uc(unsigned char value, unsigned int count)
    //! \brief rotates the value to the right by specified \a count
    //! \param[in] value the value to rotate
    //! \param[in] count number of bit positions to rotate to the right
    //! \return rotated \a value
    static M_INLINE unsigned char rotate_right_uc(unsigned char value, unsigned int count)
    {
#if defined(HAVE_BUILT_IN_STDC_ROTATE_RIGHT)
        return __builtin_stdc_rotate_right(value, count);
#else
    return ((value >> (count % UCHAR_WIDTH)) | (value << (((~count) + 1U) % UCHAR_WIDTH)));
#endif
    }

    //! \fn static M_INLINE unsigned short rotate_right_us(unsigned short value, unsigned int count)
    //! \brief rotates the value to the right by specified \a count
    //! \param[in] value the value to rotate
    //! \param[in] count number of bit positions to rotate to the right
    //! \return rotated \a value
    static M_INLINE unsigned short rotate_right_us(unsigned short value, unsigned int count)
    {
#if defined(HAVE_BUILT_IN_STDC_ROTATE_RIGHT)
        return __builtin_stdc_rotate_right(value, count);
#else
    return ((value >> (count % USHRT_WIDTH)) | (value << (((~count) + 1U) % USHRT_WIDTH)));
#endif
    }

    //! \fn static M_INLINE unsigned int rotate_right_ui(unsigned int value, unsigned int count)
    //! \brief rotates the value to the right by specified \a count
    //! \param[in] value the value to rotate
    //! \param[in] count number of bit positions to rotate to the right
    //! \return rotated \a value
    static M_INLINE unsigned int rotate_right_ui(unsigned int value, unsigned int count)
    {
#if defined(HAVE_BUILT_IN_STDC_ROTATE_RIGHT)
        return __builtin_stdc_rotate_right(value, count);
#else
    return ((value >> (count % UINT_WIDTH)) | (value << (((~count) + 1U) % UINT_WIDTH)));
#endif
    }

    //! \fn static M_INLINE unsigned long rotate_right_ul(unsigned long value, unsigned int count)
    //! \brief rotates the value to the right by specified \a count
    //! \param[in] value the value to rotate
    //! \param[in] count number of bit positions to rotate to the right
    //! \return rotated \a value
    static M_INLINE unsigned long rotate_right_ul(unsigned long value, unsigned int count)
    {
#if defined(HAVE_BUILT_IN_STDC_ROTATE_RIGHT)
        return __builtin_stdc_rotate_right(value, count);
#else
    return ((value >> (count % ULONG_WIDTH)) | (value << (((~count) + 1U) % ULONG_WIDTH)));
#endif
    }

    //! \fn static M_INLINE unsigned long long rotate_right_ull(unsigned long long value, unsigned int count)
    //! \brief rotates the value to the right by specified \a count
    //! \param[in] value the value to rotate
    //! \param[in] count number of bit positions to rotate to the right
    //! \return rotated \a value
    static M_INLINE unsigned long long rotate_right_ull(unsigned long long value, unsigned int count)
    {
#if defined(HAVE_BUILT_IN_STDC_ROTATE_RIGHT)
        return __builtin_stdc_rotate_right(value, count);
#else
    return ((value >> (count % ULLONG_WIDTH)) | (value << (((~count) + 1U) % ULLONG_WIDTH)));
#endif
    }

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
#    define rotate_right(value, count)                                                                                 \
        _Generic((value), unsigned char                                                                                \
                 : rotate_right_uc, unsigned short                                                                     \
                 : rotate_right_us, unsigned int                                                                       \
                 : rotate_right_ui, unsigned long                                                                      \
                 : rotate_right_ul, unsigned long long                                                                 \
                 : rotate_right_ull)(value, count)
#else
#    define rotate_right(value, count)                                                                                 \
        (sizeof(value) == sizeof(unsigned char)    ? rotate_right_uc((unsigned char)(value), count)                    \
         : sizeof(value) == sizeof(unsigned short) ? rotate_right_us((unsigned short)(value), count)                   \
         : sizeof(value) == sizeof(unsigned int)   ? rotate_right_ui((unsigned int)(value), count)                     \
         : sizeof(value) == sizeof(unsigned long)  ? rotate_right_ul((unsigned long)(value), count)                    \
                                                   : rotate_right_ull((unsigned long long)(value), count))
#endif

#if defined(__cplusplus)
}
#endif
