#include"../framework/test_framework.h"
#include"../../include/unit_conversion.h"
#include"../testConstants.h"

static void test_metric_Unit_Convert() {
    double byteValue = (double)8;
    char metricUnitBuffer[UNIT_STRING_LENGTH];
    char* metricUnit = metricUnitBuffer;

    eReturnValues result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 8 B to 8B");
    TEST_ASSERT(strcmp(metricUnit, "B") == 0, "Expected metricUnit is B");
    TEST_ASSERT(byteValue == 8, "Expected byteValue is 8");

    byteValue = (double)1000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 1000 B to 1KB");
    TEST_ASSERT(strcmp(metricUnit, "KB") == 0, "Expected metricUnit is KB");
    TEST_ASSERT(byteValue == 1, "Expected byteValue is 1");

    byteValue = (double)2000000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 2000000 B to 2MB");
    TEST_ASSERT(strcmp(metricUnit, "MB") == 0, "Expected metricUnit is MB");
    TEST_ASSERT(byteValue == 2, "Expected byteValue is 2");

    byteValue = (double)3000000000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 3000000000 B to 3GB");
    TEST_ASSERT(strcmp(metricUnit, "GB") == 0, "Expected metricUnit is GB");
    TEST_ASSERT(byteValue == 3, "Expected byteValue is 3");

    byteValue = (double)4000000000000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 4000000000000 B to 4TB");
    TEST_ASSERT(strcmp(metricUnit, "TB") == 0, "Expected metricUnit is TB");
    TEST_ASSERT(byteValue == 4, "Expected byteValue is 4");

    byteValue = (double)5000000000000000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 5000000000000000 B to 5PB");
    TEST_ASSERT(strcmp(metricUnit, "PB") == 0, "Expected metricUnit is PB");
    TEST_ASSERT(byteValue == 5, "Expected byteValue is 5");

    byteValue = (double)6000000000000000000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 6000000000000000000 B to 6EB");
    TEST_ASSERT(strcmp(metricUnit, "EB") == 0, "Expected metricUnit is EB");
    TEST_ASSERT(byteValue == 6, "Expected byteValue is 6");

    byteValue = 7e21;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 7e21 B to 7ZB");
    TEST_ASSERT(strcmp(metricUnit, "ZB") == 0, "Expected metricUnit is ZB");
    TEST_ASSERT(byteValue == 7, "Expected byteValue is 7");

    byteValue = 8e24;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 8e24 B to 8YB");
    // TEST_ASSERT(strcmp(metricUnit, "YB") == 0, "Expected metricUnit is YB");
    // TEST_ASSERT(byteValue == 8, "Expected byteValue is 8");

    // Test failure case for value > YB
    byteValue = 2e27;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    // TEST_ASSERT(result == FAILURE, "Expected FAILURE, value exceeds YB");

    // Testing for NULL byteValue
    result = metric_Unit_Convert(NULL, &metricUnit);
    TEST_ASSERT(result == BAD_PARAMETER, "Expected BAD_PARAMETER for NULL byteValue");

    // Testing for NULL metricUnit
    result = metric_Unit_Convert(&byteValue, NULL);
    TEST_ASSERT(result == BAD_PARAMETER, "Expected BAD_PARAMETER for NULL metricUnit");

    // Testing for NULL *metricUnit
    char* nullMetricUnit = NULL;
    result = metric_Unit_Convert(&byteValue, &nullMetricUnit);
    TEST_ASSERT(result == BAD_PARAMETER, "Expected BAD_PARAMETER for NULL *metricUnit");
}

static void test_capacity_Unit_Convert() {
    double byteValue = (double)8;
    char capacityUnitBuffer[UNIT_STRING_LENGTH];
    char* capacityUnit = capacityUnitBuffer;

    eReturnValues result = capacity_Unit_Convert(&byteValue, &capacityUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 8 B to 8B");
    TEST_ASSERT(strcmp(capacityUnit, "B") == 0, "Expected capacityUnit is B");
    TEST_ASSERT(byteValue == 8, "Expected byteValue is 8");

    byteValue = (double)1024;
    result = capacity_Unit_Convert(&byteValue, &capacityUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 1024 B to 1KiB");
    TEST_ASSERT(strcmp(capacityUnit, "KiB") == 0, "Expected capacityUnit is KiB");
    TEST_ASSERT(byteValue == 1, "Expected byteValue is 1");

    byteValue = (double)1048576;
    result = capacity_Unit_Convert(&byteValue, &capacityUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 1048576 B to 1MiB");
    TEST_ASSERT(strcmp(capacityUnit, "MiB") == 0, "Expected capacityUnit is MiB");
    TEST_ASSERT(byteValue == 1, "Expected byteValue is 1");

    byteValue = (double)2073741824;
    result = capacity_Unit_Convert(&byteValue, &capacityUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 2073741824 B to 1GiB");
    TEST_ASSERT(strcmp(capacityUnit, "GiB") == 0, "Expected capacityUnit is GiB");
    TEST_ASSERT(byteValue == 1.9313225746154785, "Expected byteValue is 1");

    byteValue = (double)1099511627776;
    result = capacity_Unit_Convert(&byteValue, &capacityUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 1099511627776 B to 1TiB");
    TEST_ASSERT(strcmp(capacityUnit, "TiB") == 0, "Expected capacityUnit is TiB");
    TEST_ASSERT(byteValue == 1, "Expected byteValue is 1");

    byteValue = (double)1125899906842624;
    result = capacity_Unit_Convert(&byteValue, &capacityUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 1125899906842624 B to 1PiB");
    TEST_ASSERT(strcmp(capacityUnit, "PiB") == 0, "Expected capacityUnit is PiB");
    TEST_ASSERT(byteValue == 1, "Expected byteValue is 1");

    byteValue = (double)1152921504606846976;
    result = capacity_Unit_Convert(&byteValue, &capacityUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 1152921504606846976 B to 1EiB");
    TEST_ASSERT(strcmp(capacityUnit, "EiB") == 0, "Expected capacityUnit is EiB");
    TEST_ASSERT(byteValue == 1, "Expected byteValue is 1");

    byteValue = 2e21;
    result = capacity_Unit_Convert(&byteValue, &capacityUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 2e21 B to 1.818989403545856ZiB");
    TEST_ASSERT(strcmp(capacityUnit, "ZiB") == 0, "Expected capacityUnit is ZiB");
    TEST_ASSERT(byteValue == 1.818989403545856, "Expected byteValue is 1.818989403545856");

    byteValue = 2e24;
    result = capacity_Unit_Convert(&byteValue, &capacityUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 2e24 B to 1.4551915228366852YiB");
    TEST_ASSERT(strcmp(capacityUnit, "YiB") == 0, "Expected capacityUnit is YiB");
    TEST_ASSERT(byteValue == 1.4551915228366852, "Expected byteValue is 1.4551915228366852");
}

void run_unit_conversion_tests() {
    test_metric_Unit_Convert();
    test_capacity_Unit_Convert();
}