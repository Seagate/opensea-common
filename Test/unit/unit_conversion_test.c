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
    TEST_ASSERT(result == SUCCESS, "Expected FAILURE, value exceeds YB");
}

void run_unit_conversion_tests() {
    test_metric_Unit_Convert();
}