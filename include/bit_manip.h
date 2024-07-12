// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file bit_manip.h
// \brief Implements various functions and macros to manipulate bit fields, manipulate bytes, swap bytes, etc

#pragma once

#include "env_detect.h"
#include "code_attributes.h"
#include "common_types.h"
#include "type_conversion.h"


#if defined (__cplusplus)
extern "C"
{
#endif

    //get a specific double word
    #define M_DoubleWord0(l) ( M_STATIC_CAST(uint32_t, ( ( (l) & UINT64_C(0x00000000FFFFFFFF) ) >>  0 ) ) )
    #define M_DoubleWord1(l) ( M_STATIC_CAST(uint32_t, ( ( (l) & UINT64_C(0xFFFFFFFF00000000) ) >> 32 ) ) )

    //get a specific double word
    #define M_DoubleWordInt0(l) ( M_STATIC_CAST(int32_t, ( ( (l) & UINT64_C(0x00000000FFFFFFFF) ) >>  0 ) ) )
    #define M_DoubleWordInt1(l) ( M_STATIC_CAST(int32_t, ( ( (l) & UINT64_C(0xFFFFFFFF00000000) ) >> 32 ) ) )

    //get a specific word
    #define M_Word0(l) ( M_STATIC_CAST(uint16_t, ( ( (l) & UINT64_C(0x000000000000FFFF) ) >>  0 ) ) )
    #define M_Word1(l) ( M_STATIC_CAST(uint16_t, ( ( (l) & UINT64_C(0x00000000FFFF0000) ) >> 16 ) ) )
    #define M_Word2(l) ( M_STATIC_CAST(uint16_t, ( ( (l) & UINT64_C(0x0000FFFF00000000) ) >> 32 ) ) )
    #define M_Word3(l) ( M_STATIC_CAST(uint16_t, ( ( (l) & UINT64_C(0xFFFF000000000000) ) >> 48 ) ) )

    //get a specific word as int's
    #define M_WordInt0(l) ( M_STATIC_CAST(int16_t, ( ( (l) & UINT64_C(0x000000000000FFFF) ) >>  0 ) ) )
    #define M_WordInt1(l) ( M_STATIC_CAST(int16_t, ( ( (l) & UINT64_C(0x00000000FFFF0000) ) >> 16 ) ) )
    #define M_WordInt2(l) ( M_STATIC_CAST(int16_t, ( ( (l) & UINT64_C(0x0000FFFF00000000) ) >> 32 ) ) )
    #define M_WordInt3(l) ( M_STATIC_CAST(int16_t, ( ( (l) & UINT64_C(0xFFFF000000000000) ) >> 48 ) ) )

    //need to validate that this macro sets the correct bits on 32bit and 64bit
    #define BITSPERBYTE UINT8_C(8)
    #define M_ByteN(n) ((UINT8_MAX << (n * BITSPERBYTE)))

    //Get a specific byte
    #define M_Byte0(l) ( M_STATIC_CAST(uint8_t, ( ( (l) & UINT64_C(0x00000000000000FF) ) >>  0 ) ) )
    #define M_Byte1(l) ( M_STATIC_CAST(uint8_t, ( ( (l) & UINT64_C(0x000000000000FF00) ) >>  8 ) ) )
    #define M_Byte2(l) ( M_STATIC_CAST(uint8_t, ( ( (l) & UINT64_C(0x0000000000FF0000) ) >> 16 ) ) )
    #define M_Byte3(l) ( M_STATIC_CAST(uint8_t, ( ( (l) & UINT64_C(0x00000000FF000000) ) >> 24 ) ) )
    #define M_Byte4(l) ( M_STATIC_CAST(uint8_t, ( ( (l) & UINT64_C(0x000000FF00000000) ) >> 32 ) ) )
    #define M_Byte5(l) ( M_STATIC_CAST(uint8_t, ( ( (l) & UINT64_C(0x0000FF0000000000) ) >> 40 ) ) )
    #define M_Byte6(l) ( M_STATIC_CAST(uint8_t, ( ( (l) & UINT64_C(0x00FF000000000000) ) >> 48 ) ) )
    #define M_Byte7(l) ( M_STATIC_CAST(uint8_t, ( ( (l) & UINT64_C(0xFF00000000000000) ) >> 56 ) ) )

    //Get a specific byte int
    #define M_ByteInt0(l) ( M_STATIC_CAST(int8_t, ( ( (l) & UINT64_C(0x00000000000000FF) ) >>  0 ) ) )
    #define M_ByteInt1(l) ( M_STATIC_CAST(int8_t, ( ( (l) & UINT64_C(0x000000000000FF00) ) >>  8 ) ) )
    #define M_ByteInt2(l) ( M_STATIC_CAST(int8_t, ( ( (l) & UINT64_C(0x0000000000FF0000) ) >> 16 ) ) )
    #define M_ByteInt3(l) ( M_STATIC_CAST(int8_t, ( ( (l) & UINT64_C(0x00000000FF000000) ) >> 24 ) ) )
    #define M_ByteInt4(l) ( M_STATIC_CAST(int8_t, ( ( (l) & UINT64_C(0x000000FF00000000) ) >> 32 ) ) )
    #define M_ByteInt5(l) ( M_STATIC_CAST(int8_t, ( ( (l) & UINT64_C(0x0000FF0000000000) ) >> 40 ) ) )
    #define M_ByteInt6(l) ( M_STATIC_CAST(int8_t, ( ( (l) & UINT64_C(0x00FF000000000000) ) >> 48 ) ) )
    #define M_ByteInt7(l) ( M_STATIC_CAST(int8_t, ( ( (l) & UINT64_C(0xFF00000000000000) ) >> 56 ) ) )

    //get a specific nibble
    #define M_Nibble0(l)  M_STATIC_CAST(uint8_t, M_STATIC_CAST(uint8_t,l) & M_STATIC_CAST(uint8_t, UINT8_C(0x0F)) )
    #define M_Nibble1(l)  M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint8_t,l) & UINT8_C(0xF0) ) >>  4 ) )
    #define M_Nibble2(l)  M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint16_t,l) & UINT16_C(0x0F00) ) >>  8 ) )
    #define M_Nibble3(l)  M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint16_t,l) & UINT16_C(0xF000) ) >> 12 ) )
    #define M_Nibble4(l)  M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint32_t,l) & UINT32_C(0x000F0000) ) >> 16  ) )
    #define M_Nibble5(l)  M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint32_t,l) & UINT32_C(0x00F00000) ) >> 20  ) )
    #define M_Nibble6(l)  M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint32_t,l) & UINT32_C(0x0F000000) ) >> 24  ) )
    #define M_Nibble7(l)  M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint32_t,l) & UINT32_C(0xF0000000) ) >> 28  ) )
    #define M_Nibble8(l)  M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint64_t,l) & UINT64_C(0x0000000F00000000) ) >> 32 ) )
    #define M_Nibble9(l)  M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint64_t,l) & UINT64_C(0x000000F000000000) ) >> 36 ) )
    #define M_Nibble10(l) M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint64_t,l) & UINT64_C(0x00000F0000000000) ) >> 40 ) )
    #define M_Nibble11(l) M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint64_t,l) & UINT64_C(0x0000F00000000000) ) >> 44 ) )
    #define M_Nibble12(l) M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint64_t,l) & UINT64_C(0x000F000000000000) ) >> 48 ) )
    #define M_Nibble13(l) M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint64_t,l) & UINT64_C(0x00F0000000000000) ) >> 52 ) )
    #define M_Nibble14(l) M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint64_t,l) & UINT64_C(0x0F00000000000000) ) >> 56 ) )
    #define M_Nibble15(l) M_STATIC_CAST(uint8_t, ( ( M_STATIC_CAST(uint64_t,l) & UINT64_C(0xF000000000000000) ) >> 60 ) )

    #define M_NibblesTo1ByteValue(n1, n0) ( \
    M_STATIC_CAST(uint8_t, ( (M_STATIC_CAST(uint8_t, ((n1) & 0x0F) << 4) | (M_STATIC_CAST(uint8_t, ((n0) & 0x0F) << 0)) \
                                           ) ) ) )

    // Big endian parameter order, little endian value
    #define M_BytesTo4ByteValue(b3, b2, b1, b0)                    (        \
    M_STATIC_CAST(uint32_t, (  (M_STATIC_CAST(uint32_t, b3) << 24) | (M_STATIC_CAST(uint32_t, b2) << 16) |          \
                 (M_STATIC_CAST(uint32_t, b1) <<  8) | (M_STATIC_CAST(uint32_t, b0) <<  0)  )         \
                                                                   ) )
    // Big endian parameter order, little endian value
    #define M_BytesTo8ByteValue(b7, b6, b5, b4, b3, b2, b1, b0)    (        \
    M_STATIC_CAST(uint64_t, ( (M_STATIC_CAST(uint64_t, b7) << 56) | (M_STATIC_CAST(uint64_t, b6) << 48) |           \
                (M_STATIC_CAST(uint64_t, b5) << 40) | (M_STATIC_CAST(uint64_t, b4) << 32) |           \
                (M_STATIC_CAST(uint64_t, b3) << 24) | (M_STATIC_CAST(uint64_t, b2) << 16) |           \
                (M_STATIC_CAST(uint64_t, b1) <<  8) | (M_STATIC_CAST(uint64_t, b0) <<  0)  )          \
                                                                   ) )

    // Big endian parameter order, little endian value
    #define M_BytesTo2ByteValue(b1, b0)                            (        \
    M_STATIC_CAST(uint16_t, (  (M_STATIC_CAST(uint16_t, b1) << 8) | (M_STATIC_CAST(uint16_t, b0) <<  0)  )          \
                                                                   ) )

    // Big endian parameter order, little endian value
    #define M_WordsTo4ByteValue(w1, w0)                            (        \
    M_STATIC_CAST(uint32_t, (  (M_STATIC_CAST(uint32_t, w1) << 16) | (M_STATIC_CAST(uint32_t, w0) <<  0)  )         \
                                                                   ) )

    #define M_WordsTo8ByteValue(w3, w2, w1, w0)                    (   \
    M_STATIC_CAST(uint64_t, (  (M_STATIC_CAST(uint64_t, w3) << 48) | (M_STATIC_CAST(uint64_t, w2) << 32) |     \
                 (M_STATIC_CAST(uint64_t, w1) << 16) | (M_STATIC_CAST(uint64_t, w0) <<  0)  )    \
                                                                   ) )

    // Big endian parameter order, little endian value
    #define M_DWordsTo8ByteValue(d1, d0)                           (        \
    M_STATIC_CAST(uint64_t, (  (M_STATIC_CAST(uint64_t, d1) << 32) | (M_STATIC_CAST(uint64_t, d0) <<  0)  )         \
                                                                   ) )

    // MACRO to round the number of x so that it will not round up when formating the float
    #define ROUNDF(f, c) M_STATIC_CAST(float, (M_STATIC_CAST(int, (f) * (c))) / (c))

    #define M_GETBITRANGE(input, msb, lsb) (((input) >> (lsb)) & ~(~UINT64_C(0) << ((msb) - (lsb) + 1)))
    // get bit range for signed values
    #define M_IGETBITRANGE(input, msb, lsb) M_STATIC_CAST(int64_t, M_GETBITRANGE(input, msb, lsb))

    // Bit access macros

    #define M_BitN(n)   (UINT64_C(1) << n)

    #define M_BitN8(n)    (UINT8_C(1) << n)
    #define M_BitN16(n)   (UINT16_C(1) << n)
    #define M_BitN32(n)   (UINT32_C(1) << n)
    #define M_BitN64(n)   (UINT64_C(1) << n)


#if !defined(UEFI_C_SOURCE)//defined in EDK2 MdePkg and causes conflicts, so checking this define for now to avoid conflicts

    #define BIT0      (M_BitN8(0))
    #define BIT1      (M_BitN8(1))
    #define BIT2      (M_BitN8(2))
    #define BIT3      (M_BitN8(3))
    #define BIT4      (M_BitN8(4))
    #define BIT5      (M_BitN8(5))
    #define BIT6      (M_BitN8(6))
    #define BIT7      (M_BitN8(7))
    #define BIT8      (M_BitN16(8))
    #define BIT9      (M_BitN16(9))
    #define BIT10     (M_BitN16(10))
    #define BIT11     (M_BitN16(11))
    #define BIT12     (M_BitN16(12))
    #define BIT13     (M_BitN16(13))
    #define BIT14     (M_BitN16(14))
    #define BIT15     (M_BitN16(15))
    #define BIT16     (M_BitN32(16))
    #define BIT17     (M_BitN32(17))
    #define BIT18     (M_BitN32(18))
    #define BIT19     (M_BitN32(19))
    #define BIT20     (M_BitN32(20))
    #define BIT21     (M_BitN32(21))
    #define BIT22     (M_BitN32(22))
    #define BIT23     (M_BitN32(23))
    #define BIT24     (M_BitN32(24))
    #define BIT25     (M_BitN32(25))
    #define BIT26     (M_BitN32(26))
    #define BIT27     (M_BitN32(27))
    #define BIT28     (M_BitN32(28))
    #define BIT29     (M_BitN32(29))
    #define BIT30     (M_BitN32(30))
    #define BIT31     (M_BitN32(31))
    #define BIT32     (M_BitN64(32))
    #define BIT33     (M_BitN64(33))
    #define BIT34     (M_BitN64(34))
    #define BIT35     (M_BitN64(35))
    #define BIT36     (M_BitN64(36))
    #define BIT37     (M_BitN64(37))
    #define BIT38     (M_BitN64(38))
    #define BIT39     (M_BitN64(39))
    #define BIT40     (M_BitN64(40))
    #define BIT41     (M_BitN64(41))
    #define BIT42     (M_BitN64(42))
    #define BIT43     (M_BitN64(43))
    #define BIT44     (M_BitN64(44))
    #define BIT45     (M_BitN64(45))
    #define BIT46     (M_BitN64(46))
    #define BIT47     (M_BitN64(47))
    #define BIT48     (M_BitN64(48))
    #define BIT49     (M_BitN64(49))
    #define BIT50     (M_BitN64(50))
    #define BIT51     (M_BitN64(51))
    #define BIT52     (M_BitN64(52))
    #define BIT53     (M_BitN64(53))
    #define BIT54     (M_BitN64(54))
    #define BIT55     (M_BitN64(55))
    #define BIT56     (M_BitN64(56))
    #define BIT57     (M_BitN64(57))
    #define BIT58     (M_BitN64(58))
    #define BIT59     (M_BitN64(59))
    #define BIT60     (M_BitN64(60))
    #define BIT61     (M_BitN64(61))
    #define BIT62     (M_BitN64(62))
    #define BIT63     (M_BitN64(63))

#endif //UEFI_C_SOURCE

    //set a bit to 1 within a value
    #define M_SET_BIT(val, bitNum) (val |= M_BitN(bitNum))

    #define M_SET_BIT8(val, bitNum) (val |= M_BitN8(bitNum))
    #define M_SET_BIT16(val, bitNum) (val |= M_BitN16(bitNum))
    #define M_SET_BIT32(val, bitNum) (val |= M_BitN32(bitNum))
    #define M_SET_BIT64(val, bitNum) (val |= M_BitN64(bitNum))

    //clear a bit to 0 within a value
    #define M_CLEAR_BIT(val, bitNum) (val &= (~M_BitN(bitNum)))

    #define M_CLEAR_BIT8(val, bitNum) (val &= C_CAST(uint8_t, (~M_BitN8(bitNum)))) //Cast added because UINT8_C() macro most of the time does not add additional qualifiers, so it ends up thinking this is a sign conversion issue.-TJE
    #define M_CLEAR_BIT16(val, bitNum) (val &= C_CAST(uint16_t, (~M_BitN16(bitNum)))) //Cast added because UINT16_C() macro most of the time does not add additional qualifiers, so it ends up thinking this is a sign conversion issue.-TJE
    #define M_CLEAR_BIT32(val, bitNum) (val &= (~M_BitN32(bitNum)))
    #define M_CLEAR_BIT64(val, bitNum) (val &= (~M_BitN64(bitNum)))

    //-----------------------------------------------------------------------------
    //
    //  nibble_Swap()
    //
    //! \brief   Description:  swap the nibbles in a byte
    //
    //  Entry:
    //!   \param[out] byteToSwap = a pointer to the byte containing the data in which to have the nibbles swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void nibble_Swap(uint8_t *byteToSwap);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_16()
    //
    //! \brief   Description:  swap the bytes in a word
    //
    //  Entry:
    //!   \param[out] wordToSwap = a pointer to the word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_16(uint16_t *wordToSwap);
    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_Int16()
    //
    //! \brief   Description:  swap the bytes in a singned word 
    //
    //  Entry:
    //!   \param[out] signedWordToSwap = a pointer to the signed word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_Int16(int16_t *signedWordToSwap);
    //-----------------------------------------------------------------------------
    //
    //  big_To_Little_Endian_16()
    //
    //! \brief   Description:  swap the bytes in a word only if on little endian system. 
    //
    //  Entry:
    //!   \param[out] wordToSwap = a pointer to the word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void big_To_Little_Endian_16(uint16_t *wordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_32()
    //
    //! \brief   Description:  swap the bytes in a double word - useful for converting between endianness
    //
    //  Entry:
    //!   \param[out] doubleWordToSwap = a pointer to the double word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_32(uint32_t *doubleWordToSwap);
    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_Int32()
    //
    //! \brief   Description:  swap the bytes in a Signed double word - useful for converting between endianness
    //
    //  Entry:
    //!   \param[out] signedDWord = a pointer to the Signed double word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_Int32(int32_t *signedDWord);
    //-----------------------------------------------------------------------------
    //
    //  big_To_Little_Endian_32()
    //
    //! \brief   Description:  swap the bytes in a double word only if running on little endian system
    //
    //  Entry:
    //!   \param[out] doubleWordToSwap = a pointer to the double word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void big_To_Little_Endian_32(uint32_t *doubleWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  word_Swap_32()
    //
    //! \brief   Description:  swap the words in a double word
    //
    //  Entry:
    //!   \param[out] doubleWordToSwap = a pointer to the double word containing the data in which to have the words swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void word_Swap_32(uint32_t *doubleWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  byte_Swap_64()
    //
    //! \brief   Description:  swap the bytes in a quad word - useful for conversion between endianness
    //
    //  Entry:
    //!   \param[out] quadWordToSwap = a pointer to the quad word containing the data in which to have the bytes swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void byte_Swap_64(uint64_t *quadWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  word_Swap_64()
    //
    //! \brief   Description:  swap the words in a quad word
    //
    //  Entry:
    //!   \param[out] quadWordToSwap = a pointer to the quad word containing the data in which to have the words swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void word_Swap_64(uint64_t *quadWordToSwap);

    //-----------------------------------------------------------------------------
    //
    //  double_Word_Swap_64()
    //
    //! \brief   Description:  swap the double words in a quad word
    //
    //  Entry:
    //!   \param[out] quadWordToSwap = a pointer to the quad word containing the data in which to have the double words swapped
    //!
    //  Exit:
    //
    //-----------------------------------------------------------------------------
    void double_Word_Swap_64(uint64_t *quadWordToSwap);

    bool get_Bytes_To_16(uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint16_t* out);

    bool get_Bytes_To_32(uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint32_t* out);

    bool get_Bytes_To_64(uint8_t* dataPtrBeginning, size_t fullDataLen, size_t msb, size_t lsb, uint64_t* out);

#if defined (__cplusplus)
}
#endif
