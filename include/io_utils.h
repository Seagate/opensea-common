// SPDX-License-Identifier: MPL-2.0

//! \file io_utils.h
//! \brief Implements various functions to work with IO (printf, snprintf, etc)
//!
//!        This file also has functionality for changing output colors and
//!        reading user input. If working with files, it is recommended
//!        secure_file.h is used instead.
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
#include "constraint_handling.h"
#include "env_detect.h"
#include "impl_io_utils.h"
#include "type_conversion.h"
#include <stdarg.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    //! \fn bool get_And_Validate_Integer_Input(const char* strToConvert, uint64_t* outputInteger)
    //! \brief Validates an input as an unsigned integer and converts it to an unsigned type.
    //!
    //! This function supports hex values such as 0xFF, AEh, etc.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_DEPRECATED /*use the bit width specific versions instead!*/
        M_NONNULL_PARAM_LIST(1, 2) M_PARAM_RO(1) M_NULL_TERM_STRING(1)
            M_PARAM_WO(2) bool get_And_Validate_Integer_Input(const char* strToConvert, uint64_t* outputInteger);

    //! \enum eAllowedUnitInput
    //! \brief Enum specifying which units are allowed at the end of the user's input.
    //!
    //! This enum must match exactly the units described in the comments. If the list of allowed units needs to be
    //! expanded, all callers should use the output unit as needed, and any unknown unit must be treated as an error.
    M_DECLARE_ENUM(eAllowedUnitInput,
                   /*!< No units allowed. */
                   ALLOW_UNIT_NONE,
                   /*!< Data size units: BLOCKS, SECTORS, B, KB, KiB, MB, MiB, GB, GiB, TB, TiB. */
                   ALLOW_UNIT_DATASIZE,
                   /*!< Sector type units: l, p, logical, physical. */
                   ALLOW_UNIT_SECTOR_TYPE,
                   /*!< Time units: h, m, s, ms, us, ns. */
                   ALLOW_UNIT_TIME,
                   /*!< Power units: w, mw. */
                   ALLOW_UNIT_POWER,
                   /*!< Voltage units: v, mv. */
                   ALLOW_UNIT_VOLTS,
                   /*!< Current units: a, ma. */
                   ALLOW_UNIT_AMPS,
                   /*!< Temperature units: f, c, k. */
                   ALLOW_UNIT_TEMPERATURE);

    //! \fn bool get_And_Validate_Integer_Input_Uint64(const char* strToConvert, char** unit, eAllowedUnitInput
    //! unittype, uint64_t* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_Uint64(const char*       strToConvert,
                                                                 char**            unit,
                                                                 eAllowedUnitInput unittype,
                                                                 uint64_t*         outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_Uint32(const char* strToConvert, char** unit, eAllowedUnitInput
    //! unittype, uint32_t* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_Uint32(const char*       strToConvert,
                                                                 char**            unit,
                                                                 eAllowedUnitInput unittype,
                                                                 uint32_t*         outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_Uint16(const char* strToConvert, char** unit, eAllowedUnitInput
    //! unittype, uint16_t* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_Uint16(const char*       strToConvert,
                                                                 char**            unit,
                                                                 eAllowedUnitInput unittype,
                                                                 uint16_t*         outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_Uint8(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! uint8_t* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_Uint8(const char*       strToConvert,
                                                                char**            unit,
                                                                eAllowedUnitInput unittype,
                                                                uint8_t*          outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_Int64(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! int64_t* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_Int64(const char*       strToConvert,
                                                                char**            unit,
                                                                eAllowedUnitInput unittype,
                                                                int64_t*          outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_Int32(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! int32_t* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_Int32(const char*       strToConvert,
                                                                char**            unit,
                                                                eAllowedUnitInput unittype,
                                                                int32_t*          outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_Int16(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! int16_t* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_Int16(const char*       strToConvert,
                                                                char**            unit,
                                                                eAllowedUnitInput unittype,
                                                                int16_t*          outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_Int8(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! int8_t* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_Int8(const char*       strToConvert,
                                                               char**            unit,
                                                               eAllowedUnitInput unittype,
                                                               int8_t*           outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_ULL(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! unsigned long long* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_ULL(const char*         strToConvert,
                                                              char**              unit,
                                                              eAllowedUnitInput   unittype,
                                                              unsigned long long* outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_UL(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! unsigned long* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_UL(const char*       strToConvert,
                                                             char**            unit,
                                                             eAllowedUnitInput unittype,
                                                             unsigned long*    outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_UI(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! unsigned int* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_UI(const char*       strToConvert,
                                                             char**            unit,
                                                             eAllowedUnitInput unittype,
                                                             unsigned int*     outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_US(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! unsigned short* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_US(const char*       strToConvert,
                                                             char**            unit,
                                                             eAllowedUnitInput unittype,
                                                             unsigned short*   outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_UC(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! unsigned char* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_UC(const char*       strToConvert,
                                                             char**            unit,
                                                             eAllowedUnitInput unittype,
                                                             unsigned char*    outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_LL(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! long long* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_LL(const char*       strToConvert,
                                                             char**            unit,
                                                             eAllowedUnitInput unittype,
                                                             long long*        outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_L(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! long* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_L(const char*       strToConvert,
                                                            char**            unit,
                                                            eAllowedUnitInput unittype,
                                                            long*             outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_I(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! int* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_I(const char*       strToConvert,
                                                            char**            unit,
                                                            eAllowedUnitInput unittype,
                                                            int*              outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_S(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! short* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_S(const char*       strToConvert,
                                                            char**            unit,
                                                            eAllowedUnitInput unittype,
                                                            short*            outputInteger);

    //! \fn bool get_And_Validate_Integer_Input_C(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! char* outputInteger)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to an integer.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputInteger Pointer to the integer to store the output.
    //! \return true if able to read an integer number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Integer_Input_C(const char*       strToConvert,
                                                            char**            unit,
                                                            eAllowedUnitInput unittype,
                                                            char*             outputInteger);

    //! \fn bool get_And_Validate_Float_Input(const char* strToConvert, char** unit, eAllowedUnitInput unittype, float*
    //! outputFloat)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to a float.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputFloat Pointer to the float to store the output.
    //! \return true if able to read a float number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Float_Input(const char*       strToConvert,
                                                        char**            unit,
                                                        eAllowedUnitInput unittype,
                                                        float*            outputFloat);

    //! \fn bool get_And_Validate_Double_Input(const char* strToConvert, char** unit, eAllowedUnitInput unittype,
    //! double* outputFloat)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to a double.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputFloat Pointer to the double to store the output.
    //! \return true if able to read a double number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_Double_Input(const char*       strToConvert,
                                                         char**            unit,
                                                         eAllowedUnitInput unittype,
                                                         double*           outputFloat);

    //! \fn bool get_And_Validate_LDouble_Input(const char* strToConvert, char** unit, eAllowedUnitInput unittype, long
    //! double* outputFloat)
    //! \brief Converts a string to an integer from decimal or hex input and validates it converted correctly.
    //!
    //! When a unit is specified, it passes that out to be used for further use. If no unit is allowed, but
    //! more data is in the string, it will fail due to unexpected input remaining.
    //!
    //! \param[in] strToConvert The buffer to convert to a long double.
    //! \param[out] unit Pointer to the unit string.
    //! \param[in] unittype The type of unit allowed.
    //! \param[out] outputFloat Pointer to the long double to store the output.
    //! \return true if able to read a long double number, false if the format is invalid.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 4) M_NULL_TERM_STRING(1) M_PARAM_RO(1) M_PARAM_WO(2)
        M_PARAM_WO(4) bool get_And_Validate_LDouble_Input(const char*       strToConvert,
                                                          char**            unit,
                                                          eAllowedUnitInput unittype,
                                                          long double*      outputFloat);

#if defined(USING_C11) && defined(HAVE_C11_GENERIC_SELECTION)
//! \def get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger)
//! \brief Generic selection macro for validating and converting integer inputs.
//!
//! This macro uses C11 generic selection to call the appropriate function based on the type of \a outputInteger.
//! It is similar to an overloaded function in C++ for different types.
//!
//! \param[in] strToConvert The buffer to convert to an integer.
//! \param[out] unit Pointer to the unit string.
//! \param[in] unittype The type of unit allowed.
//! \param[out] outputInteger Pointer to the integer to store the output.
//!
//! \note This macro does not use \c intX_t or \c uintX_t types since these are typedefs to one of the types in the
//! macro below.
//! \note There is no default case, so a warning will be generated if this macro does not expand correctly.
//!
//! \code
//! get_Valid_Integer_Input("123", &unit, ALLOW_UNIT_NONE, &outputInteger);
//! \endcode
// clang-format off
#define get_Valid_Integer_Input(strToConvert, unit, unittype, outputInteger)                                           \
    _Generic((outputInteger),                                                                                          \
             char*                  : get_And_Validate_Integer_Input_C,                                                \
             unsigned char*         : get_And_Validate_Integer_Input_UC,                                               \
             short*                 : get_And_Validate_Integer_Input_S,                                                \
             unsigned short*        : get_And_Validate_Integer_Input_US,                                               \
             int*                   : get_And_Validate_Integer_Input_I,                                                \
             unsigned int*          : get_And_Validate_Integer_Input_UI,                                               \
             long*                  : get_And_Validate_Integer_Input_L,                                                \
             unsigned long*         : get_And_Validate_Integer_Input_UL,                                               \
             long long*             : get_And_Validate_Integer_Input_LL,                                               \
             unsigned long long*    : get_And_Validate_Integer_Input_ULL,                                              \
             float*                 : get_And_Validate_Float_Input,                                                    \
             double*                : get_And_Validate_Double_Input,                                                   \
             long double*           : get_And_Validate_LDouble_Input                                                   \
            )(strToConvert, unit, unittype, outputInteger)
// clang-format on
#endif // C11

#if !defined(__STDC_ALLOC_LIB__) && !defined(POSIX_2008) && !defined(USING_C23)
    //! \fn ssize_t getline(char** lineptr, size_t* n, FILE* stream)
    //! \brief Reads an entire line from a stream.
    //!
    //! This function reads an entire line from the specified stream, storing the address of the buffer containing the
    //! text into *lineptr. The buffer is null-terminated and includes the newline character, if one was found.
    //!
    //! \param[in, out] lineptr A pointer to the buffer containing the read line. If *lineptr is NULL, a buffer will be
    //! allocated.
    //! \param[in, out] n A pointer to the size of the buffer. If *lineptr is NULL, *n will be set to the size of the
    //! allocated buffer.
    //! \param[in] stream The input stream to read from.
    //! \return The number of characters read, including the delimiter, but not including the null terminator.
    //! Returns -1 on failure or when end of file is reached.
    M_NONNULL_PARAM_LIST(2, 3)
    M_PARAM_RW(1) M_PARAM_RW(2) M_PARAM_RO(3) ssize_t getline(char** lineptr, size_t* n, FILE* stream);

    //! \fn ssize_t getdelim(char** M_RESTRICT lineptr, size_t* M_RESTRICT n, int delimiter, FILE* stream)
    //! \brief Reads a line from a stream, stopping at a specified delimiter.
    //!
    //! This function reads a line from the specified stream, stopping when the specified delimiter character is found,
    //! storing the address of the buffer containing the text into *lineptr. The buffer is null-terminated and includes
    //! the delimiter character, if one was found.
    //!
    //! \param[in, out] lineptr A pointer to the buffer containing the read line. If *lineptr is NULL, a buffer will be
    //! allocated.
    //! \param[in, out] n A pointer to the size of the buffer. If *lineptr is NULL, *n will be set to the size of the
    //! allocated buffer.
    //! \param[in] delimiter The character to stop reading at.
    //! \param[in] stream The input stream to read from.
    //! \return The number of characters read, including the delimiter, but not including the null terminator.
    //! Returns -1 on failure or when end of file is reached.
    M_NONNULL_PARAM_LIST(2, 4)
    M_PARAM_RW(1)
    M_PARAM_RW(2)
    M_PARAM_RO(4) ssize_t getdelim(char** M_RESTRICT lineptr, size_t* M_RESTRICT n, int delimiter, FILE* stream);
#endif //!__STDC_ALLOC_LIB__ && (POSIX < 2008)

// Defining asprintf and vasprintf for all systems that do not have these
// already. asprintf and vasprintf were in GNU C library, added in FreeBSD 2.2,
// and OpenBSD 2.3
#if !defined(__STDC_ALLOC_LIB__) && !defined(_GNU_SOURCE) && !IS_FREEBSD_VERSION(2, 2, 0) &&                           \
    !(defined(__OpenBSD__) && defined(OpenBSD2_3)) && !defined(HAVE_VASPRINTF)

    //! \fn int asprintf(char** M_RESTRICT strp, const char* M_RESTRICT fmt, ...)
    //! \brief Allocates a formatted string.
    //!
    //! This function allocates a formatted string and stores the address of the buffer in \a strp.
    //! The buffer is allocated using malloc and should be freed by the caller.
    //!
    //! \param[out] strp Pointer to the buffer that will hold the formatted string.
    //! \param[in] fmt The format string.
    //! \param[in] ... Additional arguments specifying data to format.
    //! \return The number of characters printed (excluding the null terminator), or -1 if an error occurs.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 2) M_PARAM_RW(1) M_PARAM_RO(2)
        FUNC_ATTR_PRINTF(2, 3) int asprintf(char** M_RESTRICT strp, const char* M_RESTRICT fmt, ...);

    //! \fn int vasprintf(char** M_RESTRICT strp, const char* M_RESTRICT fmt, va_list arg)
    //! \brief Allocates a formatted string using a va_list.
    //!
    //! This function allocates a formatted string using a va_list and stores the address of the buffer in \a strp.
    //! The buffer is allocated using malloc and should be freed by the caller.
    //!
    //! \param[out] strp Pointer to the buffer that will hold the formatted string.
    //! \param[in] fmt The format string.
    //! \param[in] arg A va_list specifying data to format.
    //! \return The number of characters printed (excluding the null terminator), or -1 if an error occurs.
    M_NODISCARD M_NONNULL_PARAM_LIST(1, 2) M_PARAM_RW(1) M_PARAM_RO(2)
        FUNC_ATTR_PRINTF(2, 0) int vasprintf(char** M_RESTRICT strp, const char* M_RESTRICT fmt, va_list arg);

#endif // asprintf, vasprintf

// Microsoft doesn't have snprintf compliant with C99 until VS2015, so make our
// own definition using what Microsoft does have available.
#if defined(_MSC_VER) && _MSC_VER <= MSVC_2013 && defined _WIN32

    //! \fn int snprintf(char* buffer, size_t bufsz, const char* format, ...)
    //! \brief Writes formatted data to a string.
    //!
    //! This function writes formatted data to a string, ensuring that the buffer is not overflowed.
    //!
    //! \param[out] buffer The buffer to write the formatted string to.
    //! \param[in] bufsz The size of the buffer.
    //! \param[in] format The format string.
    //! \param[in] ... Additional arguments specifying data to format.
    //! \return The number of characters that would have been written if the buffer had been sufficiently large, not
    //! including the null terminator.
    int snprintf(char* buffer, size_t bufsz, const char* format, ...);

    //! \fn int vsnprintf(char* buffer, size_t bufsz, const char* format, va_list args)
    //! \brief Writes formatted data to a string using a va_list.
    //!
    //! This function writes formatted data to a string using a va_list, ensuring that the buffer is not overflowed.
    //!
    //! \param[out] buffer The buffer to write the formatted string to.
    //! \param[in] bufsz The size of the buffer.
    //! \param[in] format The format string.
    //! \param[in] args A va_list specifying data to format.
    //! \return The number of characters that would have been written if the buffer had been sufficiently large, not
    //! including the null terminator.
    int vsnprintf(char* buffer, size_t bufsz, const char* format, va_list args);

#endif

    //! \fn FUNC_ATTR_PRINTF(3, 4) static M_INLINE int snprintf_err_handle(char* buf, size_t bufsize,
    //! const char* format, ...)
    //! \brief Not exactly the same as snprintf_s, just checking that the return code is as expected when using this
    //! function to detect errors or truncation.
    //! \details This function does NOT do bounds checking.
    //! \see https://wiki.sei.cmu.edu/confluence/display/c/ERR33-C.+Detect+and+handle+standard+library+errors
    //! \todo Implement a safe_snprintf that works as C11 annex k specifies.
    //! \param[out] buf TThe buffer to write the formatted string to.
    //! \param[in] bufsize The size of the buffer.
    //! \param[in] format The format string.
    //! \param[in] ... Additional arguments for the format string.
    //! \return The number of characters that would have been written if the buffer had been sufficiently large, not
    //! including the null terminator. On error, this will always add a null terminator at the end of the buffer.
    //! \note Invokes the constraint handler on error.
    M_NONNULL_PARAM_LIST(1, 3)
    M_PARAM_RW(1)
    M_PARAM_RO(3)
    FUNC_ATTR_PRINTF(3, 4) static M_INLINE int snprintf_err_handle(char* buf, size_t bufsize, const char* format, ...)
    {
        int     n = 0;
        va_list args;
        va_start(args, format);
        // Disabling this warning in GCC and Clang for now. It only seems to show in Windows at the moment-TJE
        DISABLE_WARNING_FORMAT_NONLITERAL
        // NOLINTBEGIN(clang-analyzer-valist.Uninitialized,clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        // - false positive
        n = vsnprintf(buf, bufsize, format, args);
        // NOLINTEND(clang-analyzer-valist.Uninitialized,clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        // - false positive
        RESTORE_WARNING_FORMAT_NONLITERAL
        va_end(args);

        if (n < 0 || int_to_sizet(n) >= bufsize)
        {
            buf[bufsize - 1] = 0;
            constraintEnvInfo envInfo;
            errno = EINVAL;
            invoke_Constraint_Handler("snprintf_error_handler_macro: error in snprintf",
                                      set_Env_Info(&envInfo, __FILE__, __func__, M_NULLPTR, __LINE__), EINVAL);
        }
        return n;
    }

    //! \fn int verify_Format_String_And_Args(const char* M_RESTRICT format, va_list formatargs)
    //! \brief Checks for the same conditions as printf_s in C11 annex K.
    //! \details Ensures that there are no newline characters (\n) and that no NULL pointers are passed for strings.
    //! \param[in] format The format string.
    //! \param[in] formatargs The list of arguments for the format string.
    //! \return An integer indicating the result of the verification.
    M_NONNULL_PARAM_LIST(1)
    M_PARAM_RO(1) int verify_Format_String_And_Args(const char* M_RESTRICT format, va_list formatargs);

    //! \fn eReturnValues get_Secure_User_Input(const char* prompt, char** userInput, size_t* inputDataLen)
    //! \brief Provides a prompt and takes a password or other input without echoing to the screen.
    //! \details This function disables the echo of typed characters while reading input. Once reading is complete,
    //! the echo is restored to normal. If you wish to confirm the user input, call this function again after asking
    //! for confirmation and compare the strings. This function allocates memory that should be freed with free().
    //! It is recommended to call explicit_zeroes before freeing to ensure data is securely removed.
    //! \note inputDataLen is the allocated buffer size, not necessarily the number of characters typed. This can vary
    //! depending on the low-level OS's implementation of getting a password without echoing.
    //! \note On really old systems, this may be limited to 8 characters. Most modern systems should allow at least
    //! 128 characters. As long as POSIX2001 is supported, there should be no limitation to the number of characters
    //! read from this function.
    //! \param[in] prompt The prompt to display to the user.
    //! \param[out] userInput A pointer to the buffer where the user input will be stored.
    //! \param[in] inputDataLen A pointer to the size of the allocated buffer.
    //! \return An eReturnValues indicating the result of the input operation.
    M_NONNULL_PARAM_LIST(1, 2, 3)
    M_PARAM_RO(1)
    M_PARAM_RW(2)
    M_PARAM_RW(3) eReturnValues get_Secure_User_Input(const char* prompt, char** userInput, size_t* inputDataLen);

    //! \enum eConsoleColors
    //! \brief Enum representing console colors.
    //!
    //! These colors may appear slightly different in each console, but will be close to the requested color.
    //!
    //! \see https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit
    M_DECLARE_ENUM(eConsoleColors,
                   /*! Default console color */
                   CONSOLE_COLOR_DEFAULT,
                   /*! Black console color */
                   CONSOLE_COLOR_BLACK,
                   /*! Red console color */
                   CONSOLE_COLOR_RED,
                   /*! Green console color */
                   CONSOLE_COLOR_GREEN,
                   /*! Yellow console color */
                   CONSOLE_COLOR_YELLOW,
                   /*! Blue console color */
                   CONSOLE_COLOR_BLUE,
                   /*! Magenta console color */
                   CONSOLE_COLOR_MAGENTA,
                   /*! Cyan console color */
                   CONSOLE_COLOR_CYAN,
                   /*! White console color */
                   CONSOLE_COLOR_WHITE,
                   /*! Gray console color */
                   CONSOLE_COLOR_GRAY,
                   /*! Bright black console color (same as gray) */
                   CONSOLE_COLOR_BRIGHT_BLACK = CONSOLE_COLOR_GRAY,
                   /*! Bright red console color */
                   CONSOLE_COLOR_BRIGHT_RED,
                   /*! Bright green console color */
                   CONSOLE_COLOR_BRIGHT_GREEN,
                   /*! Bright yellow console color */
                   CONSOLE_COLOR_BRIGHT_YELLOW,
                   /*! Bright blue console color */
                   CONSOLE_COLOR_BRIGHT_BLUE,
                   /*! Bright magenta console color */
                   CONSOLE_COLOR_BRIGHT_MAGENTA,
                   /*! Bright cyan console color */
                   CONSOLE_COLOR_BRIGHT_CYAN,
                   /*! Bright white console color */
                   CONSOLE_COLOR_BRIGHT_WHITE,
                   /*! Current console color */
                   CONSOLE_COLOR_CURRENT);

    //! \fn void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor)
    //! \brief Recommend using set_Console_Foreground_Background_Colors() instead!
    //!
    //! Set the foreground or background color in the console output.
    //!
    //! \param[in] foregroundBackground True for foreground, false for background.
    //! \param[in] consoleColor One of the enum values defined in this header for the desired color.
    void set_Console_Colors(bool foregroundBackground, eConsoleColors consoleColor);

    //! \fn void set_Console_Foreground_Background_Colors(eConsoleColors foregroundColor,
    //!                                                   eConsoleColors backgroundColor)
    //! \brief Set the foreground and background color in the console output.
    //!
    //! \param[in] foregroundColor Requested foreground color.
    //! \param[in] backgroundColor Requested background color.
    void set_Console_Foreground_Background_Colors(eConsoleColors foregroundColor, eConsoleColors backgroundColor);

    //! \fn void print_Data_Buffer(uint8_t* dataBuffer, uint32_t bufferLen, bool showPrint)
    //! \brief Prints out a data buffer to the screen.
    //!
    //! This function prints out a data buffer to the screen. If showPrint is set to true, printable characters will be
    //! shown on the side of the hex output for the buffer. Non-printable characters will be represented as dots.
    //!
    //! \param[in] dataBuffer A pointer to the data buffer you want to print out.
    //! \param[in] bufferLen The length that you want to print out. This can be the length of the buffer, or anything
    //! less than that. \param[in] showPrint Set to true to show printable characters on the side of the hex output for
    //! the buffer. Non-printable characters will be represented as dots.
    M_NONNULL_IF_NONZERO_PARAM(1, 2)
    M_PARAM_RO_SIZE(1, 2) void print_Data_Buffer(const uint8_t* dataBuffer, uint32_t bufferLen, bool showPrint);

    //! \fn void print_Pipe_Data(uint8_t* dataBuffer, uint32_t bufferLen)
    //! \brief Prints out a data buffer for piping to the next executable to the screen.
    //!
    //! This function prints out a data buffer for piping to the next executable to the screen.
    //!
    //! \param[in] dataBuffer A pointer to the data buffer you want to print out.
    //! \param[in] bufferLen The length that you want to print out. This can be the length of the buffer, or anything
    //! less than that.
    M_NONNULL_IF_NONZERO_PARAM(1, 2)
    M_PARAM_RO_SIZE(1, 2) void print_Pipe_Data(const uint8_t* dataBuffer, uint32_t bufferLen);

    //! \fn void print_Return_Enum(const char* funcName, eReturnValues ret)
    //! \brief Prints humanized eReturnValue for a given return value.
    //!
    //! This function prints a human-readable eReturnValue for a given return value.
    //!
    //! \param[in] funcName Name of the function value returning from.
    //! \param[in] ret Value to humanize.
    M_NONNULL_PARAM_LIST(1)
    M_PARAM_RO(1) M_NULL_TERM_STRING(1) void print_Return_Enum(const char* funcName, eReturnValues ret);

    //! \fn static M_INLINE void flush_stdout(void)
    //! \brief Flushes the standard output stream.
    //!
    //! This function flushes the standard output stream to ensure all buffered data is written out.
    static M_INLINE void flush_stdout(void)
    {
        M_STATIC_CAST(void, fflush(stdout));
    }

    //! \fn static M_INLINE void flush_stderr(void)
    //! \brief Flushes the standard error stream.
    //!
    //! This function flushes the standard error stream to ensure all buffered data is written out.
    static M_INLINE void flush_stderr(void)
    {
        M_STATIC_CAST(void, fflush(stderr));
    }

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_fopen(FILE* M_RESTRICT* M_RESTRICT streamptr, const char* M_RESTRICT filename, const char*
    //! M_RESTRICT mode) \brief Opens a file with the specified mode.
    //!
    //! This function opens a file with the specified mode, similar to fopen_s in C11 annex K.
    //!
    //! \param[out] streamptr Pointer to the FILE pointer that will be set to the opened file.
    //! \param[in] filename The name of the file to open.
    //! \param[in] mode The mode in which to open the file.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the currently installed constraint handler function:
    //!
    //! - \a streamptr is a null pointer
    //!
    //! - \a filename is a null pointer
    //!
    //! - \a mode is a null pointer
    M_INLINE errno_t safe_fopen(FILE* M_RESTRICT* M_RESTRICT streamptr,
                                const char* M_RESTRICT       filename,
                                const char* M_RESTRICT       mode)
    {
        return safe_fopen_impl(streamptr, filename, mode, __FILE__, __func__, __LINE__,
                               "safe_fopen(streamptr, filename, mode)");
    }
#else
//! \def safe_fopen(streamptr, filename, mode)
//! \brief Opens a file with the specified mode using a macro.
//!
//! This macro opens a file with the specified mode, similar to fopen_s in C11 annex K. This macro passes __FILE__,
//! __func__, __LINE__ to the internal implementation to provide more debug information at compile time to the
//! constraint handler.
//!
//! \param[out] streamptr Pointer to the FILE pointer that will be set to the opened file.
//! \param[in] filename The name of the file to open.
//! \param[in] mode The mode in which to open the file.
//! \return Zero on success, or an error code on failure.
//!
//! \note The following errors are detected at runtime and call the currently installed constraint handler function:
//!
//! - \a streamptr is a null pointer
//!
//! - \a filename is a null pointer
//!
//! - \a mode is a null pointer
#    define safe_fopen(streamptr, filename, mode)                                                                      \
        safe_fopen_impl(streamptr, filename, mode, __FILE__, __func__, __LINE__,                                       \
                        "safe_fopen(" #streamptr ", " #filename ", " #mode ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_freopen(FILE* M_RESTRICT* M_RESTRICT newstreamptr, const char* M_RESTRICT filename,
    //!                          const char* M_RESTRICT mode, FILE* M_RESTRICT stream)
    //! \brief Reopens a file with the specified mode.
    //!
    //! This function reopens a file with the specified mode, similar to freopen_s in C11 annex K.
    //!
    //! \param[out] newstreamptr Pointer to the FILE pointer that will be set to the reopened file.
    //! \param[in] filename The name of the file to reopen.
    //! \param[in] mode The mode in which to reopen the file.
    //! \param[in] stream The existing FILE pointer to be reopened.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the currently installed constraint handler function:
    //!
    //! - \a newstreamptr is a null pointer
    //!
    //! - \a filename is a null pointer
    //!
    //! - \a mode is a null pointer
    //!
    //! - \a stream is a null pointer
    M_INLINE errno_t safe_freopen(FILE* M_RESTRICT* M_RESTRICT newstreamptr,
                                  const char* M_RESTRICT       filename,
                                  const char* M_RESTRICT       mode,
                                  FILE* M_RESTRICT             stream)
    {
        return safe_freopen_impl(newstreamptr, filename, mode, stream, __FILE__, __func__, __LINE__,
                                 "safe_freopen(streamptr, filename, mode, stream)");
    }
#else
//! \def safe_freopen(newstreamptr, filename, mode, stream)
//! \brief Reopens a file with the specified mode using a macro.
//!
//! This macro reopens a file with the specified mode, similar to freopen_s in C11 annex K. This macro passes __FILE__,
//! __func__, __LINE__ to the internal implementation to provide more debug information at compile time to the
//! constraint handler.
//!
//! \param[out] newstreamptr Pointer to the FILE pointer that will be set to the reopened file.
//! \param[in] filename The name of the file to reopen.
//! \param[in] mode The mode in which to reopen the file.
//! \param[in] stream The existing FILE pointer to be reopened.
//! \return Zero on success, or an error code on failure.
//!
//! \note The following errors are detected at runtime and call the currently installed constraint handler function:
//!
//! - \a newstreamptr is a null pointer
//!
//! - \a filename is a null pointer
//!
//! - \a mode is a null pointer
//!
//! - \a stream is a null pointer
#    define safe_freopen(newstreamptr, filename, mode, stream)                                                         \
        safe_freopen_impl(newstreamptr, filename, mode, stream, __FILE__, __func__, __LINE__,                          \
                          "safe_freopen(" #newstreamptr ", " #filename ", " #mode ", " #stream ")")
#endif

#if defined(WANT_SAFE_TMPNAM)

#    if !defined(TMP_MAX_S)
#        define TMP_MAX_S TMP_MAX
#    endif
#    if !defined(L_tmpnam_s)
#        define L_tmpnam_s L_tmpnam
#    endif

#    if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_tmpnam(char* filename_s, rsize_t maxsize)
    //! \brief Generates a temporary file name.
    //!
    //! This function generates a temporary file name, similar to tmpnam_s in C11 annex K.
    //!
    //! \param[out] filename_s Pointer to the buffer where the temporary file name will be stored.
    //! \param[in] maxsize The maximum size of the buffer.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the currently installed constraint handler function:
    //!
    //! - \a filename_s is a null pointer
    //!
    //! - \a maxsize is greater than \a RSIZE_MAX
    //!
    //! - \a maxsize is less than the required size for the temporary file name
    M_INLINE errno_t safe_tmpnam(char* filename_s, rsize_t maxsize)
    {
        return safe_tmpnam_impl(filename_s, maxsize, __FILE__, __func__, __LINE__, "safe_tmpnam(filename_s, maxsize)");
    }
#    else
    //! \def safe_tmpnam(filename_s, maxsize)
    //! \brief Generates a temporary file name using a macro.
    //!
    //! This macro generates a temporary file name, similar to tmpnam_s in C11 annex K. This macro passes __FILE__,
    //! __func__, __LINE__ to the internal implementation to provide more debug information at compile time to the
    //! constraint handler.
    //!
    //! \param[out] filename_s Pointer to the buffer where the temporary file name will be stored.
    //! \param[in] maxsize The maximum size of the buffer.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the currently installed constraint handler function:
    //!
    //! - \a filename_s is a null pointer
    //!
    //! - \a maxsize is greater than \a RSIZE_MAX
    //!
    //! - \a maxsize is less than the required size for the temporary file name
#        define safe_tmpnam(filename_s, maxsize)                                                                       \
            safe_tmpnam_impl(filename_s, maxsize, __FILE__, __func__, __LINE__,                                        \
                             "safe_tmpnam(" #filename_s ", " #maxsize ")")
#    endif
#endif // WANT_SAFE_TMPNAM

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_tmpfile(FILE* M_RESTRICT* M_RESTRICT streamptr)
    //! \brief Creates a temporary file.
    //!
    //! This function creates a temporary file, similar to tmpfile_s in C11 annex K.
    //!
    //! \param[out] streamptr Pointer to the FILE pointer that will be set to the temporary file.
    //! \return Zero on success, or an error code on failure.
    //!
    //! \note The following errors are detected at runtime and call the currently installed constraint handler function:
    //!
    //! - \a streamptr is a null pointer
    M_INLINE errno_t safe_tmpfile(FILE* M_RESTRICT* M_RESTRICT streamptr)
    {
        return safe_tmpfile_impl(streamptr, __FILE__, __func__, __LINE__, "safe_tmpfile(streamptr)");
    }
#else
//! \def safe_tmpfile(streamptr)
//! \brief Creates a temporary file using a macro.
//!
//! This macro creates a temporary file, similar to tmpfile_s in C11 annex K. This macro passes __FILE__, __func__,
//! __LINE__ to the internal implementation to provide more debug information at compile time to the constraint handler.
//!
//! \param[out] streamptr Pointer to the FILE pointer that will be set to the temporary file.
//! \return Zero on success, or an error code on failure.
//!
//! \note The following errors are detected at runtime and call the currently installed constraint handler function:
//!
//! - \a streamptr is a null pointer
#    define safe_tmpfile(streamptr)                                                                                    \
        safe_tmpfile_impl(streamptr, __FILE__, __func__, __LINE__, "safe_tmpfile(" #streamptr ")")
#endif

// Recommend using getline implementation instead as it will dynamically allocate the string for you.-TJE
// NOTE: Max size of n is limited to INT_MAX in many cases. Windows with secure lib can support RSIZE_MAX
#if defined(DEV_ENVIRONMENT)
    //! \fn char* safe_gets(char* str, rsize_t n)
    //! \brief Reads a line from stdin into the provided buffer.
    //!
    //! This function reads a line from stdin into the provided buffer, similar to gets_s in C11 annex K.
    //!
    //! \param[out] str Pointer to the buffer where the input will be stored.
    //! \param[in] n The maximum number of characters to read.
    //! \return Pointer to the buffer containing the input, or NULL on failure.
    //!
    //! \note The following errors are detected at runtime and call the currently installed constraint handler function:
    //!
    //! - \a str is a null pointer
    //!
    //! - \a n is greater than \a RSIZE_MAX
    //!
    //! - \a n is zero
    M_INLINE char* safe_gets(char* str, rsize_t n)
    {
        return safe_gets_impl(str, n, __FILE__, __func__, __LINE__, "safe_gets(streamptr)");
    }
#else
//! \def safe_gets(streamptr, n)
//! \brief Reads a line from stdin into the provided buffer using a macro.
//!
//! This macro reads a line from stdin into the provided buffer, similar to gets_s in C11 annex K. This macro passes
//! __FILE__, __func__, __LINE__ to the internal implementation to provide more debug information at compile time to the
//! constraint handler.
//!
//! \param[out] str Pointer to the buffer where the input will be stored.
//! \param[in] n The maximum number of characters to read.
//! \return Pointer to the buffer containing the input, or NULL on failure.
//!
//! \note The following errors are detected at runtime and call the currently installed constraint handler function:
//!
//! - \a str is a null pointer
//!
//! - \a n is greater than \a RSIZE_MAX
//!
//! - \a n is zero
#    define safe_gets(streamptr, n)                                                                                    \
        safe_gets_impl(streamptr, n, __FILE__, __func__, __LINE__, "safe_gets(" #streamptr ", " #n ")")
#endif

// These are for specifying the base for conversion in strto(u)l(l) functions
#define BASE_0_AUTO     (0)
#define BASE_2_BINARY   (2)
#define BASE_8_OCTAL    (8)
#define BASE_10_DECIMAL (10)
#define BASE_16_HEX     (16)
#define BASE_36_MAX     (36) // this is the max base required by the standards for strtol type functions

// These safe string to long conversion functions check for NULL ptr on str and value.
// They properly check errno for range errors, and detect invalid conversions too
#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strtol(long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base)
    //! \brief Converts a string to a long integer with bounds checking.
    //!
    //! This function converts a string to a long integer with bounds checking, following ISO C secure coding practices.
    //!
    //! \param[out] value Pointer to the long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is greater than 36
    M_INLINE errno_t safe_strtol(long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base)
    {
        return safe_strtol_impl(value, str, endp, base, __FILE__, __func__, __LINE__,
                                "safe_strtol(value, str, endp, base)");
    }
#else
//! \def safe_strtol(value, str, endp, base)
//! \brief Converts a string to a long integer with bounds checking using a macro.
//!
//! This macro converts a string to a long integer with bounds checking, following ISO C secure coding practices. This
//! macro passes __FILE__, __func__, __LINE__ to the internal implementation to provide more debug information at
//! compile time to the constraint handler.
//!
//! \param[out] value Pointer to the long integer that will be set to the converted value.
//! \param[in] str The string to convert.
//! \param[out] endp Pointer to the end of the parsed string.
//! \param[in] base The base to use for the conversion.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
//!
//! - \a base is greater than 36
#    define safe_strtol(value, str, endp, base)                                                                        \
        safe_strtol_impl(value, str, endp, base, __FILE__, __func__, __LINE__,                                         \
                         "safe_strtol(" #value ", " #str ", " #endp ", " #base ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strtoll(long long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base)
    //! \brief Converts a string to a long long integer with bounds checking.
    //!
    //! This function converts a string to a long long integer with bounds checking, following ISO C secure coding
    //! practices.
    //!
    //! \param[out] value Pointer to the long long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is greater than 36
    M_INLINE errno_t safe_strtoll(long long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base)
    {
        return safe_strtoll_impl(value, str, endp, base, __FILE__, __func__, __LINE__,
                                 "safe_strtoll(value, str, endp, base)");
    }
#else
//! \def safe_strtoll(value, str, endp, base)
//! \brief Converts a string to a long long integer with bounds checking using a macro.
//!
//! This macro converts a string to a long long integer with bounds checking, following ISO C secure coding practices.
//! This macro passes __FILE__, __func__, __LINE__ to the internal implementation to provide more debug information at
//! compile time to the constraint handler.
//!
//! \param[out] value Pointer to the long long integer that will be set to the converted value.
//! \param[in] str The string to convert.
//! \param[out] endp Pointer to the end of the parsed string.
//! \param[in] base The base to use for the conversion.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
//!
//! - \a base is greater than 36
#    define safe_strtoll(value, str, endp, base)                                                                       \
        safe_strtoll_impl(value, str, endp, base, __FILE__, __func__, __LINE__,                                        \
                          "safe_strtoll(" #value ", " #str ", " #endp ", " #base ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strtoul(unsigned long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base)
    //! \brief Converts a string to an unsigned long integer with bounds checking.
    //!
    //! This function converts a string to an unsigned long integer with bounds checking, following ISO C secure coding
    //! practices.
    //!
    //! \param[out] value Pointer to the unsigned long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is greater than 36
    M_INLINE errno_t safe_strtoul(unsigned long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base)
    {
        return safe_strtoul_impl(value, str, endp, base, __FILE__, __func__, __LINE__,
                                 "safe_strtoul(value, str, endp, base)");
    }
#else
//! \def safe_strtoul(value, str, endp, base)
//! \brief Converts a string to an unsigned long integer with bounds checking using a macro.
//!
//! This macro converts a string to an unsigned long integer with bounds checking, following ISO C secure coding
//! practices. This macro passes __FILE__, __func__, __LINE__ to the internal implementation to provide more debug
//! information at compile time to the constraint handler.
//!
//! \param[out] value Pointer to the unsigned long integer that will be set to the converted value.
//! \param[in] str The string to convert.
//! \param[out] endp Pointer to the end of the parsed string.
//! \param[in] base The base to use for the conversion.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
//!
//! - \a base is greater than 36
#    define safe_strtoul(value, str, endp, base)                                                                       \
        safe_strtoul_impl(value, str, endp, base, __FILE__, __func__, __LINE__,                                        \
                          "safe_strtoul(" #value ", " #str ", " #endp ", " #base ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strtoull(unsigned long long* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int
    //! base) \brief Converts a string to an unsigned long long integer with bounds checking.
    //!
    //! This function converts a string to an unsigned long long integer with bounds checking, following ISO C secure
    //! coding practices.
    //!
    //! \param[out] value Pointer to the unsigned long long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is greater than 36
    M_INLINE errno_t safe_strtoull(unsigned long long*    value,
                                   const char* M_RESTRICT str,
                                   char** M_RESTRICT      endp,
                                   int                    base)
    {
        return safe_strtoull_impl(value, str, endp, base, __FILE__, __func__, __LINE__,
                                  "safe_strtoull(value, str, endp, base)");
    }
#else
//! \def safe_strtoull(value, str, endp, base)
//! \brief Converts a string to an unsigned long long integer with bounds checking using a macro.
//!
//! This macro converts a string to an unsigned long long integer with bounds checking, following ISO C secure coding
//! practices. This macro passes __FILE__, __func__, __LINE__ to the internal implementation to provide more debug
//! information at compile time to the constraint handler.
//!
//! \param[out] value Pointer to the unsigned long long integer that will be set to the converted value.
//! \param[in] str The string to convert.
//! \param[out] endp Pointer to the end of the parsed string.
//! \param[in] base The base to use for the conversion.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
//!
//! - \a base is greater than 36
#    define safe_strtoull(value, str, endp, base)                                                                      \
        safe_strtoull_impl(value, str, endp, base, __FILE__, __func__, __LINE__,                                       \
                           "safe_strtoull(" #value ", " #str ", " #endp ", " #base ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strtoimax(intmax_t* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base)
    //! \brief Converts a string to an intmax_t integer with bounds checking.
    //!
    //! This function converts a string to an intmax_t integer with bounds checking, following ISO C secure coding
    //! practices.
    //!
    //! \param[out] value Pointer to the intmax_t integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is greater than 36
    M_INLINE errno_t safe_strtoimax(intmax_t* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base)
    {
        return safe_strtoimax_impl(value, str, endp, base, __FILE__, __func__, __LINE__,
                                   "safe_strtoimax(value, str, endp, base)");
    }
#else
//! \def safe_strtoimax(value, str, endp, base)
//! \brief Converts a string to an intmax_t integer with bounds checking using a macro.
//!
//! This macro converts a string to an intmax_t integer with bounds checking, following ISO C secure coding practices.
//! This macro passes __FILE__, __func__, __LINE__ to the internal implementation to provide more debug information at
//! compile time to the constraint handler.
//!
//! \param[out] value Pointer to the intmax_t integer that will be set to the converted value.
//! \param[in] str The string to convert.
//! \param[out] endp Pointer to the end of the parsed string.
//! \param[in] base The base to use for the conversion.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
//!
//! - \a base is greater than 36
#    define safe_strtoimax(value, str, endp, base)                                                                     \
        safe_strtoimax_impl(value, str, endp, base, __FILE__, __func__, __LINE__,                                      \
                            "safe_strtoimax(" #value ", " #str ", " #endp ", " #base ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strtoumax(uintmax_t* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base)
    //! \brief Converts a string to a uintmax_t integer with bounds checking.
    //!
    //! This function converts a string to a uintmax_t integer with bounds checking, following ISO C secure coding
    //! practices.
    //!
    //! \param[out] value Pointer to the uintmax_t integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \param[in] base The base to use for the conversion.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - \a base is greater than 36
    M_INLINE errno_t safe_strtoumax(uintmax_t* value, const char* M_RESTRICT str, char** M_RESTRICT endp, int base)
    {
        return safe_strtoumax_impl(value, str, endp, base, __FILE__, __func__, __LINE__,
                                   "safe_strtoumax(value, str, endp, base)");
    }
#else
//! \def safe_strtoumax(value, str, endp, base)
//! \brief Converts a string to a uintmax_t integer with bounds checking using a macro.
//!
//! This macro converts a string to a uintmax_t integer with bounds checking, following ISO C secure coding practices.
//! This macro passes __FILE__, __func__, __LINE__ to the internal implementation to provide more debug information at
//! compile time to the constraint handler.
//!
//! \param[out] value Pointer to the uintmax_t integer that will be set to the converted value.
//! \param[in] str The string to convert.
//! \param[out] endp Pointer to the end of the parsed string.
//! \param[in] base The base to use for the conversion.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
//!
//! - \a base is greater than 36
#    define safe_strtoumax(value, str, endp, base)                                                                     \
        safe_strtoumax_impl(value, str, endp, base, __FILE__, __func__, __LINE__,                                      \
                            "safe_strtoumax(" #value ", " #str ", " #endp ", " #base ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strtof(float* value, const char* M_RESTRICT str, char** M_RESTRICT endp)
    //! \brief Converts a string to a float with bounds checking.
    //!
    //! This function converts a string to a float with bounds checking, following ISO C secure coding practices.
    //!
    //! \param[out] value Pointer to the float that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    M_INLINE errno_t safe_strtof(float* value, const char* M_RESTRICT str, char** M_RESTRICT endp)
    {
        return safe_strtof_impl(value, str, endp, __FILE__, __func__, __LINE__, "safe_strtof(value, str, endp)");
    }
#else
//! \def safe_strtof(value, str, endp)
//! \brief Converts a string to a float with bounds checking using a macro.
//!
//! This macro converts a string to a float with bounds checking, following ISO C secure coding practices. This macro
//! passes __FILE__, __func__, __LINE__ to the internal implementation to provide more debug information at compile time
//! to the constraint handler.
//!
//! \param[out] value Pointer to the float that will be set to the converted value.
//! \param[in] str The string to convert.
//! \param[out] endp Pointer to the end of the parsed string.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
#    define safe_strtof(value, str, endp)                                                                              \
        safe_strtof_impl(value, str, endp, __FILE__, __func__, __LINE__, "safe_strtof(" #value ", " #str ", " #endp ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strtod(double* value, const char* M_RESTRICT str, char** M_RESTRICT endp)
    //! \brief Converts a string to a double with bounds checking.
    //!
    //! This function converts a string to a double with bounds checking, following ISO C secure coding practices.
    //!
    //! \param[out] value Pointer to the double that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    M_INLINE errno_t safe_strtod(double* value, const char* M_RESTRICT str, char** M_RESTRICT endp)
    {
        return safe_strtod_impl(value, str, endp, __FILE__, __func__, __LINE__, "safe_strtod(value, str, endp)");
    }
#else
//! \def safe_strtod(value, str, endp)
//! \brief Converts a string to a double with bounds checking using a macro.
//!
//! This macro converts a string to a double with bounds checking, following ISO C secure coding practices. This macro
//! passes __FILE__, __func__, __LINE__ to the internal implementation to provide more debug information at compile time
//! to the constraint handler.
//!
//! \param[out] value Pointer to the double that will be set to the converted value.
//! \param[in] str The string to convert.
//! \param[out] endp Pointer to the end of the parsed string.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
#    define safe_strtod(value, str, endp)                                                                              \
        safe_strtod_impl(value, str, endp, __FILE__, __func__, __LINE__, "safe_strtod(" #value ", " #str ", " #endp ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_strtold(long double* value, const char* M_RESTRICT str, char** M_RESTRICT endp)
    //! \brief Converts a string to a long double with bounds checking.
    //!
    //! This function converts a string to a long double with bounds checking, following ISO C secure coding practices.
    //!
    //! \param[out] value Pointer to the long double that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \param[out] endp Pointer to the end of the parsed string.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    M_INLINE errno_t safe_strtold(long double* value, const char* M_RESTRICT str, char** M_RESTRICT endp)
    {
        return safe_strtold_impl(value, str, endp, __FILE__, __func__, __LINE__, "safe_strtold(value, str, endp)");
    }
#else
//! \def safe_strtold(value, str, endp)
//! \brief Converts a string to a long double with bounds checking using a macro.
//!
//! This macro converts a string to a long double with bounds checking, following ISO C secure coding practices. This
//! macro passes __FILE__, __func__, __LINE__ to the internal implementation to provide more debug information at
//! compile time to the constraint handler.
//!
//! \param[out] value Pointer to the long double that will be set to the converted value.
//! \param[in] str The string to convert.
//! \param[out] endp Pointer to the end of the parsed string.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
#    define safe_strtold(value, str, endp)                                                                             \
        safe_strtold_impl(value, str, endp, __FILE__, __func__, __LINE__,                                              \
                          "safe_strtold(" #value ", " #str ", " #endp ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_atoi(int* value, const char* M_RESTRICT str)
    //! \brief Converts a string to an integer with bounds checking.
    //!
    //! This function converts a string to an integer with bounds checking, following ISO C secure coding practices. It
    //! calls the constraint handlers for similar issues as safe_strtol. The only additional error it detects is if
    //! there is text still present after performing a conversion.
    //!
    //! \param[out] value Pointer to the integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - There is text still present after performing the conversion
    M_INLINE errno_t safe_atoi(int* value, const char* M_RESTRICT str)
    {
        return safe_atoi_impl(value, str, __FILE__, __func__, __LINE__, "safe_atoi(value, str)");
    }
#else
//! \def safe_atoi(value, str)
//! \brief Converts a string to an integer with bounds checking using a macro.
//!
//! This macro converts a string to an integer with bounds checking, following ISO C secure coding practices. It calls
//! the constraint handlers for similar issues as safe_strtol. The only additional error it detects is if there is text
//! still present after performing a conversion. This macro passes __FILE__, __func__, __LINE__ to the internal
//! implementation to provide more debug information at compile time to the constraint handler.
//!
//! \param[out] value Pointer to the integer that will be set to the converted value.
//! \param[in] str The string to convert.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
//!
//! - There is text still present after performing the conversion
#    define safe_atoi(value, str)                                                                                      \
        safe_atoi_impl(value, str, __FILE__, __func__, __LINE__, "safe_atoi(" #value ", " #str ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_atol(long* value, const char* M_RESTRICT str)
    //! \brief Converts a string to a long integer with bounds checking.
    //!
    //! This function converts a string to a long integer with bounds checking, following ISO C secure coding practices.
    //! It calls the constraint handlers for similar issues as safe_strtol. The only additional error it detects is if
    //! there is text still present after performing a conversion.
    //!
    //! \param[out] value Pointer to the long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - There is text still present after performing the conversion
    M_INLINE errno_t safe_atol(long* value, const char* M_RESTRICT str)
    {
        return safe_atol_impl(value, str, __FILE__, __func__, __LINE__, "safe_atol(value, str)");
    }
#else
//! \def safe_atol(value, str)
//! \brief Converts a string to a long integer with bounds checking using a macro.
//!
//! This macro converts a string to a long integer with bounds checking, following ISO C secure coding practices. It
//! calls the constraint handlers for similar issues as safe_strtol. The only additional error it detects is if there is
//! text still present after performing a conversion. This macro passes __FILE__, __func__, __LINE__ to the internal
//! implementation to provide more debug information at compile time to the constraint handler.
//!
//! \param[out] value Pointer to the long integer that will be set to the converted value.
//! \param[in] str The string to convert.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
//!
//! - There is text still present after performing the conversion
#    define safe_atol(value, str)                                                                                      \
        safe_atol_impl(value, str, __FILE__, __func__, __LINE__, "safe_atol(" #value ", " #str ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_atoll(long long* value, const char* M_RESTRICT str)
    //! \brief Converts a string to a long long integer with bounds checking.
    //!
    //! This function converts a string to a long long integer with bounds checking, following ISO C secure coding
    //! practices. It calls the constraint handlers for similar issues as safe_strtol. The only additional error it
    //! detects is if there is text still present after performing a conversion.
    //!
    //! \param[out] value Pointer to the long long integer that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - There is text still present after performing the conversion
    M_INLINE errno_t safe_atoll(long long* value, const char* M_RESTRICT str)
    {
        return safe_atoll_impl(value, str, __FILE__, __func__, __LINE__, "safe_atoll(value, str)");
    }
#else
//! \def safe_atoll(value, str)
//! \brief Converts a string to a long long integer with bounds checking using a macro.
//!
//! This macro converts a string to a long long integer with bounds checking, following ISO C secure coding practices.
//! It calls the constraint handlers for similar issues as safe_strtol. The only additional error it detects is if there
//! is text still present after performing a conversion. This macro passes __FILE__, __func__, __LINE__ to the internal
//! implementation to provide more debug information at compile time to the constraint handler.
//!
//! \param[out] value Pointer to the long long integer that will be set to the converted value.
//! \param[in] str The string to convert.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
//!
//! - There is text still present after performing the conversion
#    define safe_atoll(value, str)                                                                                     \
        safe_atoll_impl(value, str, __FILE__, __func__, __LINE__, "safe_atoll(" #value ", " #str ")")
#endif

#if defined(DEV_ENVIRONMENT)
    //! \fn errno_t safe_atof(double* value, const char* M_RESTRICT str)
    //! \brief Converts a string to a double with bounds checking.
    //!
    //! This function converts a string to a double with bounds checking, following ISO C secure coding practices. It
    //! calls the constraint handlers for similar issues as safe_strtof. The only additional error it detects is if
    //! there is text still present after performing a conversion.
    //!
    //! \param[out] value Pointer to the double that will be set to the converted value.
    //! \param[in] str The string to convert.
    //! \return An error code indicating success or failure.
    //!
    //! \note The following errors are detected at runtime and call the installed constraint handler:
    //!
    //! - \a value is a null pointer
    //!
    //! - \a str is a null pointer
    //!
    //! - There is text still present after performing the conversion
    M_INLINE errno_t safe_atof(double* value, const char* M_RESTRICT str)
    {
        return safe_atof_impl(value, str, __FILE__, __func__, __LINE__, "safe_atof(value, str)");
    }
#else
//! \def safe_atof(value, str)
//! \brief Converts a string to a double with bounds checking using a macro.
//!
//! This macro converts a string to a double with bounds checking, following ISO C secure coding practices. It calls the
//! constraint handlers for similar issues as safe_strtof. The only additional error it detects is if there is text
//! still present after performing a conversion. This macro passes __FILE__, __func__, __LINE__ to the internal
//! implementation to provide more debug information at compile time to the constraint handler.
//!
//! \param[out] value Pointer to the double that will be set to the converted value.
//! \param[in] str The string to convert.
//! \return An error code indicating success or failure.
//!
//! \note The following errors are detected at runtime and call the installed constraint handler:
//!
//! - \a value is a null pointer
//!
//! - \a str is a null pointer
//!
//! - There is text still present after performing the conversion
#    define safe_atof(value, str)                                                                                      \
        safe_atof_impl(value, str, __FILE__, __func__, __LINE__, "safe_atof(" #value ", " #str ")")
#endif

#if defined(__cplusplus)
}
#endif
