// SPDX-License-Identifier: MPL-2.0

//! \file unit_conversion.c
//! \brief Implements various functions for converting between different types of units
//! \copyright
//! Do NOT modify or remove this copyright and license
//!
//! Copyright (c) 2024-2026 Seagate Technology LLC and/or its Affiliates, All Rights Reserved
//!
//! This software is subject to the terms of the Mozilla Public License, v. 2.0.
//! If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "unit_conversion.h"
#include "io_utils.h"
#include "type_conversion.h"

typedef enum eMUnitTypes
{
    UNIT_TYPE_BYTE = 0,
    UNIT_TYPE_KILO,
    UNIT_TYPE_MEGA,
    UNIT_TYPE_GIGA,
    UNIT_TYPE_TERA,
    UNIT_TYPE_PETA,
    UNIT_TYPE_EXA,
    UNIT_TYPE_ZETTA,
    UNIT_TYPE_YOTTA
}eMUnitTypes;

typedef enum eCUnitTypes
{
    UNIT_TYPE_BIBYTE = 0,
    UNIT_TYPE_KIBI,
    UNIT_TYPE_MEBI,
    UNIT_TYPE_GIBI,
    UNIT_TYPE_TEBI,
    UNIT_TYPE_PEBI,
    UNIT_TYPE_EBI,
    UNIT_TYPE_ZEBI,
    UNIT_TYPE_YOBI
} eCUnitTypes;

eReturnValues metric_Unit_Convert(double* M_NONNULL byteValue, char* M_NONNULL* M_NONNULL metricUnit)
{
    eReturnValues ret         = SUCCESS;
    eMUnitTypes       unitCounter = UNIT_TYPE_BYTE;

    if (byteValue == M_NULLPTR || metricUnit == M_NULLPTR || *metricUnit == M_NULLPTR)
    {
        return BAD_PARAMETER;
    }

    while ((*byteValue / 1000.0) >= 1 && (unitCounter + 1) <= UNIT_TYPE_YOTTA)
    {
        *byteValue = *byteValue / 1000.00;
        unitCounter += 1;
    }
    switch (unitCounter)
    {
    case UNIT_TYPE_BYTE:
        snprintf_err_handle(*metricUnit, UNIT_STRING_LENGTH, "B");
        break;
    case UNIT_TYPE_KILO:
        snprintf_err_handle(*metricUnit, UNIT_STRING_LENGTH, "KB");
        break;
    case UNIT_TYPE_MEGA:
        snprintf_err_handle(*metricUnit, UNIT_STRING_LENGTH, "MB");
        break;
    case UNIT_TYPE_GIGA:
        snprintf_err_handle(*metricUnit, UNIT_STRING_LENGTH, "GB");
        break;
    case UNIT_TYPE_TERA:
        snprintf_err_handle(*metricUnit, UNIT_STRING_LENGTH, "TB");
        break;
    case UNIT_TYPE_PETA:
        snprintf_err_handle(*metricUnit, UNIT_STRING_LENGTH, "PB");
        break;
    case UNIT_TYPE_EXA:
        snprintf_err_handle(*metricUnit, UNIT_STRING_LENGTH, "EB");
        break;
    case UNIT_TYPE_ZETTA:
        snprintf_err_handle(*metricUnit, UNIT_STRING_LENGTH, "ZB");
        break;
    case UNIT_TYPE_YOTTA:
        snprintf_err_handle(*metricUnit, UNIT_STRING_LENGTH, "YB");
        break;
    }
    return ret;
}

eReturnValues capacity_Unit_Convert(double* M_NONNULL byteValue, char* M_NONNULL* M_NONNULL capacityUnit)
{
    eReturnValues ret         = SUCCESS;
    eCUnitTypes       unitCounter = UNIT_TYPE_BIBYTE;

    if (byteValue == M_NULLPTR || capacityUnit == M_NULLPTR || *capacityUnit == M_NULLPTR)
    {
        return BAD_PARAMETER;
    }

    while ((*byteValue / 1024.0) >= 1 && (unitCounter + 1) <= UNIT_TYPE_YOBI)
    {
        *byteValue = *byteValue / 1024.00;
        unitCounter += 1;
    }
    switch (unitCounter)
    {
    case UNIT_TYPE_BIBYTE:
        snprintf_err_handle(*capacityUnit, UNIT_STRING_LENGTH, "B");
        break;
    case UNIT_TYPE_KIBI:
        snprintf_err_handle(*capacityUnit, UNIT_STRING_LENGTH, "KiB");
        break;
    case UNIT_TYPE_MEBI:
        snprintf_err_handle(*capacityUnit, UNIT_STRING_LENGTH, "MiB");
        break;
    case UNIT_TYPE_GIBI:
        snprintf_err_handle(*capacityUnit, UNIT_STRING_LENGTH, "GiB");
        break;
    case UNIT_TYPE_TEBI:
        snprintf_err_handle(*capacityUnit, UNIT_STRING_LENGTH, "TiB");
        break;
    case UNIT_TYPE_PEBI:
        snprintf_err_handle(*capacityUnit, UNIT_STRING_LENGTH, "PiB");
        break;
    case UNIT_TYPE_EBI:
        snprintf_err_handle(*capacityUnit, UNIT_STRING_LENGTH, "EiB");
        break;
    case UNIT_TYPE_ZEBI:
        snprintf_err_handle(*capacityUnit, UNIT_STRING_LENGTH, "ZiB");
        break;
    case UNIT_TYPE_YOBI:
        snprintf_err_handle(*capacityUnit, UNIT_STRING_LENGTH, "YiB");
        break;
    }
    return ret;
}

//! \def CELSIUS_KELVIN_CONVERSION_CONSTANT
//! \brief The constant value used to convert between Celsius and Kelvin. This is defined as
//!        the difference between the freezing point of water in Celsius and Kelvin, which is 273.15. We use 273
//!        as an integer constant for conversion to avoid using floating point math in our conversions
#define CELSIUS_KELVIN_CONVERSION_CONSTANT (273)

//! \def CELSIUS_FAHRENHEIT_CONVERSION_CONSTANT
//! \brief The constant value used to convert between Celsius and Fahrenheit. This is defined as the
//!        freezing point of water in Fahrenheit, which is 32. We use 32 as an integer constant for conversion to avoid using floating point math in our conversions
#define CELSIUS_FAHRENHEIT_CONVERSION_CONSTANT (32)

#define CELSIUS_ABS_ZERO (-273)

#define FAHRENHEIT_ABS_ZERO (-459)

#define KELVIN_ABS_ZERO (0)

// formula is C * (9/5) + 32. To scale better for int math we use
//            ((C * 9) / 5) + 32
int16_t celsius_To_Fahrenheit(const int16_t* M_NONNULL celsius)
{
    if (*celsius < CELSIUS_ABS_ZERO)
    {
        errno = ERANGE;
        return INT16_C(0);
    }
    // rewrote order of operations to be better for integer math.
    // formula is C * (9/5) + 32. To scale better for int math we use
    //            ((C * 9) / 5) + 32
    int16_t fahrenheit =
        ((*celsius * INT16_C(9)) / INT16_C(5)); // NOLINT(bugprone-narrowing-conversions)
    int16_t remainder = ((*celsius * INT16_C(9)) % INT16_C(5));
    // if the remainder is greater than or equal to half of the divisor, round up the result
    if (remainder >= INT16_C(3) && fahrenheit >= 0)
    {
        fahrenheit += 1;
    }
    else if (remainder <= INT16_C(-3) && fahrenheit < 0)
    {
        fahrenheit -= 1;
    }
    return fahrenheit + CELSIUS_FAHRENHEIT_CONVERSION_CONSTANT;
}

// formula is (F - 32) / (9/5). equivalent to (F - 32) * (5/9). To scale better for int math we use and avoid doubles/floats.
int16_t fahrenheit_To_celsius(const int16_t* M_NONNULL fahrenheit)
{
    if (*fahrenheit < FAHRENHEIT_ABS_ZERO)
    {
        errno = ERANGE;
        return INT16_C(0);
    }
    int16_t celsius = ((*fahrenheit - CELSIUS_FAHRENHEIT_CONVERSION_CONSTANT) * INT16_C(5)) / INT16_C(9); // NOLINT(bugprone-narrowing-conversions)
    int16_t remainder = ((*fahrenheit - CELSIUS_FAHRENHEIT_CONVERSION_CONSTANT) * INT16_C(5)) % INT16_C(9);
    // if the remainder is greater than or equal to half of the divisor, round up the result
    if (remainder >= INT16_C(5) / 2 && celsius >= 0)
    {
        celsius += 1;
    }
    else if (remainder <= INT16_C(-5) / 2 && celsius < 0)
    {
        celsius -= 1;
    }
    return celsius;
}

int16_t celsius_To_Kelvin(const int16_t* M_NONNULL celsius)
{
    if (*celsius < CELSIUS_ABS_ZERO)
    {
        errno = ERANGE;
        return KELVIN_ABS_ZERO;
    }
    int16_t kelvin = *celsius + CELSIUS_KELVIN_CONVERSION_CONSTANT; // NOLINT(bugprone-narrowing-conversions)
    return kelvin;
}

int16_t fahrenheit_To_Kelvin(const int16_t* M_NONNULL fahrenheit)
{
    if (*fahrenheit < FAHRENHEIT_ABS_ZERO)
    {
        errno = ERANGE;
        return KELVIN_ABS_ZERO;
    }
    int16_t kelvin = fahrenheit_To_celsius(fahrenheit) + CELSIUS_KELVIN_CONVERSION_CONSTANT; // NOLINT(bugprone-narrowing-conversions)
    return kelvin;
}

int16_t kelvin_To_Celsius(const int16_t* M_NONNULL kelvin)
{
    if (*kelvin < KELVIN_ABS_ZERO)
    {
        errno = ERANGE;
        return CELSIUS_ABS_ZERO;
    }
    int16_t celsius = *kelvin - CELSIUS_KELVIN_CONVERSION_CONSTANT; // NOLINT(bugprone-narrowing-conversions)
    return celsius;
}

int16_t kelvin_To_Fahrenheit(const int16_t* M_NONNULL kelvin)
{
    if (*kelvin < KELVIN_ABS_ZERO)
    {
        errno = ERANGE;
        return FAHRENHEIT_ABS_ZERO;
    }
    int16_t celsius = kelvin_To_Celsius(kelvin);
    int16_t fahrenheit =
        ((celsius * INT16_C(9)) / INT16_C(5)) + CELSIUS_FAHRENHEIT_CONVERSION_CONSTANT; // NOLINT(bugprone-narrowing-conversions)
    int16_t remainder = ((celsius * INT16_C(9)) % INT16_C(5));
    // if the remainder is greater than or equal to half of the divisor, round up the result
    if (remainder >= INT16_C(3) && fahrenheit >= 0)
    {
        fahrenheit += 1;
    }
    else if (remainder <= INT16_C(-3) && fahrenheit < 0)
    {
        fahrenheit -= 1;
    }
    return fahrenheit;
}
