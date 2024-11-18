// SPDX-License-Identifier: MPL-2.0
//
// Do NOT modify or remove this copyright and license
//
// Copyright (c) 2024-2024 Seagate Technology LLC and/or its Affiliates, All
// Rights Reserved
//
// This software is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// ******************************************************************************************
//
// \file unit_conversion.c
// \brief Implements various functions for converting between different types of
// units.

#include "unit_conversion.h"
#include "io_utils.h"
#include "type_conversion.h"

eReturnValues metric_Unit_Convert(double* byteValue, char** metricUnit)
{
    eReturnValues ret         = SUCCESS;
    uint8_t       unitCounter = UINT8_C(0);

    if (!byteValue || !metricUnit || !*metricUnit)
    {
        return BAD_PARAMETER;
    }

    while ((*byteValue / 1000.0) >= 1 && (unitCounter + 1) < 8)
    {
        *byteValue = *byteValue / 1000.00;
        unitCounter += 1;
    }
    switch (unitCounter)
    {
    case 0:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "B");
        break;
    case 1:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "KB");
        break;
    case 2:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "MB");
        break;
    case 3:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "GB");
        break;
    case 4:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "TB");
        break;
    case 5:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "PB");
        break;
    case 6:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "EB");
        break;
    case 7:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "ZB");
        break;
    case 8:
        snprintf(*metricUnit, UNIT_STRING_LENGTH, "YB");
        break;
    default:
        ret = FAILURE;
    }
    return ret;
}

eReturnValues capacity_Unit_Convert(double* byteValue, char** capacityUnit)
{
    eReturnValues ret         = SUCCESS;
    uint8_t       unitCounter = UINT8_C(0);

    if (!byteValue || !capacityUnit || !*capacityUnit)
    {
        return BAD_PARAMETER;
    }

    while ((*byteValue / 1024.0) >= 1 && (unitCounter + 1) < 8)
    {
        *byteValue = *byteValue / 1024.00;
        unitCounter += 1;
    }
    switch (unitCounter)
    {
    case 0:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "B");
        break;
    case 1:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "KiB");
        break;
    case 2:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "MiB");
        break;
    case 3:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "GiB");
        break;
    case 4:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "TiB");
        break;
    case 5:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "PiB");
        break;
    case 6:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "EiB");
        break;
    case 7:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "ZiB");
        break;
    case 8:
        snprintf(*capacityUnit, UNIT_STRING_LENGTH, "YiB");
        break;
    default:
        ret = FAILURE;
    }
    return ret;
}

int16_t celsius_To_Fahrenheit(const int16_t* celsius)
{
    // rewrote order of operations to be better for integer math.
    // formula is C * (9/5) + 32. To scale better for int math we use
    //            ((C * 9) + 160) / 5
    int16_t fahrenheit =
        ((*celsius * INT16_C(9)) + INT16_C(160)) / INT16_C(5); // NOLINT(bugprone-narrowing-conversions)
    return fahrenheit;
}

int16_t fahrenheit_To_celsius(const int16_t* fahrenheit)
{
    int16_t celsius = ((*fahrenheit - INT16_C(32)) * INT16_C(5)) / INT16_C(9); // NOLINT(bugprone-narrowing-conversions)
    return celsius;
}

int16_t celsius_To_Kelvin(const int16_t* celsius)
{
    int16_t kelvin = *celsius + INT16_C(273); // NOLINT(bugprone-narrowing-conversions)
    return kelvin;
}

int16_t fahrenheit_To_Kelvin(const int16_t* fahrenheit)
{
    int16_t kelvin = fahrenheit_To_celsius(fahrenheit) + INT16_C(273); // NOLINT(bugprone-narrowing-conversions)
    return kelvin;
}

int16_t kelvin_To_Celsius(const int16_t* kelvin)
{
    int16_t celsius = *kelvin - INT16_C(273); // NOLINT(bugprone-narrowing-conversions)
    return celsius;
}

int16_t kelvin_To_Fahrenheit(const int16_t* kelvin)
{
    int16_t fahrenheit =
        ((kelvin_To_Celsius(kelvin) * INT16_C(9)) / INT16_C(5)) + INT16_C(32); // NOLINT(bugprone-narrowing-conversions)
    return fahrenheit;
}
