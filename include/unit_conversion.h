// SPDX-License-Identifier: MPL-2.0

//! \file unit_conversion.h
//! \brief Implements various functions for converting between different types of units
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

#if defined(__cplusplus)
extern "C"
{
#endif

//! \def UNIT_STRING_LENGTH
//! \brief Length of the string needed to hold the unit on conversion output
#define UNIT_STRING_LENGTH 4

    //! \fn eReturnValues metric_Unit_Convert(double* byteValue, char** metricUnit)
    //! \brief Takes a double representing a number of bytes and converts it to the
    //! most easy to display unit value.
    //! For example, instead of outputting 8000GB, it will output 8TB.
    //! Possible output units: KB, MB, GB, TB, PB, EB, ZB, YB
    //! \param[in,out] byteValue number of bytes going in, converted value to match unit on out
    //! \param[out] metricUnit string of size UNIT_STRING_LENGTH to hold the converted unit.
    //! \return SUCCESS if converted without error, otherwise error code
    eReturnValues metric_Unit_Convert(double* byteValue, char** metricUnit);

    //! \fn eReturnValues capacity_Unit_Convert(double* byteValue, char** metricUnit)
    //! \brief Takes a double representing a number of bytes and converts it to the
    //! most easy to display unit value representing drive capacity (or binary unit).
    //! For example, instead of outputting 8000GiB, it will output 8TiB.
    //! Possible output units: KiB, MiB, GiB, TiB, PiB, EiB, ZiB, YiB
    //! \param[in,out] byteValue number of bytes going in, converted value to match unit on out
    //! \param[out] capacityUnit string of size UNIT_STRING_LENGTH to hold the converted unit.
    //! \return SUCCESS if converted without error, otherwise error code
    eReturnValues capacity_Unit_Convert(double* byteValue, char** capacityUnit);

    //! \fn int16_t celsius_To_Fahrenheit(const int16_t* celsius)
    //! \brief Converts a temperature in celsius to fahrenheit
    //! \param[in] celsius temperature in celsius to convert
    //! \return temperature in fahrenheit
    int16_t celsius_To_Fahrenheit(const int16_t* celsius);

    //! \fn int16_t fahrenheit_To_celsius(const int16_t* fahrenheit)
    //! \brief Converts a temperature in fahrenheit to celsius
    //! \param[in] celsius temperature in fahrenheit to convert
    //! \return temperature in celsius
    int16_t fahrenheit_To_celsius(const int16_t* fahrenheit);

    //! \fn int16_t celsius_To_Kelvin(const int16_t* celsius)
    //! \brief Converts a temperature in celsius to kelvin
    //! \param[in] celsius temperature in celsius to convert
    //! \return temperature in kelvin
    int16_t celsius_To_Kelvin(const int16_t* celsius);

    //! \fn int16_t fahrenheit_To_Kelvin(const int16_t* fahrenheit)
    //! \brief Converts a temperature in fahrenheit to kelvin
    //! \param[in] fahrenheit temperature in fahrenheit to convert
    //! \return temperature in kelvin
    int16_t fahrenheit_To_Kelvin(const int16_t* fahrenheit);

    //! \fn int16_t kelvin_To_Celsius(const int16_t* kelvin)
    //! \brief Converts a temperature in kelvin to celsius
    //! \param[in] kelvin temperature in kelvin to convert
    //! \return temperature in celsius
    int16_t kelvin_To_Celsius(const int16_t* kelvin);

    //! \fn int16_t kelvin_To_Fahrenheit(const int16_t* kelvin)
    //! \brief Converts a temperature in kelvin to fahrenheit
    //! \param[in] kelvin temperature in kelvin to convert
    //! \return temperature in fahrenheit
    int16_t kelvin_To_Fahrenheit(const int16_t* kelvin);

#if defined(__cplusplus)
}
#endif
