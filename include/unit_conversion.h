// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
// 
// \file unit_conversion.h
// \brief Implements various functions for converting between different types of units.

#pragma once

#include "env_detect.h"
#include "code_attributes.h"
#include "common_types.h"

#if defined (__cplusplus)
extern "C"
{
#endif

    #define UNIT_STRING_LENGTH 4

    //-----------------------------------------------------------------------------
    //
    //  metric_Unit_Convert()
    //
    //! \brief   Description:  Takes a double value which is a a numberic value specifying a number of bytes, and 
    //!                        converts it to KB, MB, GB, TB, etc in a more easy to display manor. Sets the units for you for display purposes too.
    //
    //  Entry:
    //!   \param[in] byteValue = value specifying a number of bytes
    //!   \param[in] metricUnit = char ptr to hold the metric unit. 
    //!
    //  Exit:
    //!   \return SUCCESS on successful completion, !SUCCESS if problems encountered
    //
    //-----------------------------------------------------------------------------
    eReturnValues metric_Unit_Convert(double* byteValue, char** metricUnit);

    //-----------------------------------------------------------------------------
    //
    //  capacity_Unit_Convert()
    //
    //! \brief   Description:  Takes a double value which is a a numeric value specifying a number of bytes, and 
    //!                        converts it to KiB, MiB, GiB, TiB, etc in a more easy to display manor. Sets the units for you for display purposes too.
    //
    //  Entry:
    //!   \param[in] byteValue = value specifying a number of bytes
    //!   \param[in] capacityUnit = char ptr to hold the metric unit. 
    //!
    //  Exit:
    //!   \return SUCCESS on successful completion, !SUCCESS if problems encountered
    //
    //-----------------------------------------------------------------------------
    eReturnValues capacity_Unit_Convert(double* byteValue, char** capacityUnit);

    //-----------------------------------------------------------------------------
    //
    //  celsius_To_Fahrenheit()
    //
    //! \brief   Description:  convert a temperature in celsius to fahrenheit
    //
    //  Entry:
    //!   \param[in] celsius = a pointer to the data containing a temperature in celsius
    //!
    //  Exit:
    //!   \return The temperature in fahrenheit
    //
    //-----------------------------------------------------------------------------
    int16_t celsius_To_Fahrenheit(int16_t* celsius);

    //-----------------------------------------------------------------------------
    //
    //  fahrenheit_To_celsius()
    //
    //! \brief   Description:  convert a temperature in fahrenheit to celsius
    //
    //  Entry:
    //!   \param[in] fahrenheit = a pointer to the data containing a temperature in fahrenheit
    //!
    //  Exit:
    //!   \return The temperature in celsius
    //
    //-----------------------------------------------------------------------------
    int16_t fahrenheit_To_celsius(int16_t* fahrenheit);

    //-----------------------------------------------------------------------------
    //
    //  celsius_To_Kelvin()
    //
    //! \brief   Description:  convert a temperature in celsius to kelvin
    //
    //  Entry:
    //!   \param[in] celsius = a pointer to the data containing a temperature in celsius
    //!
    //  Exit:
    //!   \return The temperature in celsius
    //
    //-----------------------------------------------------------------------------
    int16_t celsius_To_Kelvin(int16_t* celsius);

    //-----------------------------------------------------------------------------
    //
    //  fahrenheit_To_Kelvin()
    //
    //! \brief   Description:  convert a temperature in fahrenheit to kelvin
    //
    //  Entry:
    //!   \param[in] fahrenheit = a pointer to the data containing a temperature in fahrenheit
    //!
    //  Exit:
    //!   \return The temperature in celsius
    //
    //-----------------------------------------------------------------------------
    int16_t fahrenheit_To_Kelvin(int16_t* fahrenheit);

    //-----------------------------------------------------------------------------
    //
    //  kelvin_To_Celsius()
    //
    //! \brief   Description:  convert a temperature in kelvin to celsius
    //
    //  Entry:
    //!   \param[in] kelvin = a pointer to the data containing a temperature in kelvin
    //!
    //  Exit:
    //!   \return The temperature in celsius
    //
    //-----------------------------------------------------------------------------
    int16_t kelvin_To_Celsius(int16_t* kelvin);

    //-----------------------------------------------------------------------------
    //
    //  kelvin_To_Fahrenheit()
    //
    //! \brief   Description:  convert a temperature in kelvin to fahrenheit
    //
    //  Entry:
    //!   \param[in] kelvin = a pointer to the data containing a temperature in kelvin
    //!
    //  Exit:
    //!   \return The temperature in celsius
    //
    //-----------------------------------------------------------------------------
    int16_t kelvin_To_Fahrenheit(int16_t* kelvin);

#if defined (__cplusplus)
}
#endif
