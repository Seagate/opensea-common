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

    byteValue = (double)8000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 8000 B to 8KB");
    TEST_ASSERT(strcmp(metricUnit, "KB") == 0, "Expected metricUnit is KB");
    TEST_ASSERT(byteValue == 8, "Expected byteValue is 8");

    byteValue = (double)8000000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 8000000 B to 8MB");
    TEST_ASSERT(strcmp(metricUnit, "MB") == 0, "Expected metricUnit is MB");
    TEST_ASSERT(byteValue == 8, "Expected byteValue is 8");

    byteValue = (double)8000000000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 8000000000 B to 8GB");
    TEST_ASSERT(strcmp(metricUnit, "GB") == 0, "Expected metricUnit is GB");
    TEST_ASSERT(byteValue == 8, "Expected byteValue is 8");

    byteValue = (double)8000000000000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 8000000000000 B to 8TB");
    TEST_ASSERT(strcmp(metricUnit, "TB") == 0, "Expected metricUnit is TB");
    TEST_ASSERT(byteValue == 8, "Expected byteValue is 8");

    byteValue = (double)8000000000000000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 8000000000000000 B to 8PB");
    TEST_ASSERT(strcmp(metricUnit, "PB") == 0, "Expected metricUnit is PB");
    TEST_ASSERT(byteValue == 8, "Expected byteValue is 8");

    byteValue = (double)8000000000000000000;
    result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 8000000000000000000 B to 8EB");
    TEST_ASSERT(strcmp(metricUnit, "EB") == 0, "Expected metricUnit is EB");
    TEST_ASSERT(byteValue == 8, "Expected byteValue is 8");
}

void run_unit_conversion_tests() {
    test_metric_Unit_Convert();
}