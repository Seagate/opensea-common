#include"../framework/test_framework.h"
#include"../../include/unit_conversion.h"
#include"../testConstants.h"

static void test_metric_Unit_Convert() {
    double byteValue = 8000 * 1000 * 1000 * 1000; // 8000 GB in bytes
    char metricUnitBuffer[UNIT_STRING_LENGTH];
    char* metricUnit = metricUnitBuffer;
    eReturnValues result = metric_Unit_Convert(&byteValue, &metricUnit);
    TEST_ASSERT(result == SUCCESS, "Expected SUCCESS, converted 8000 GB to 8TB");
    printf("Converted byteValue: %f, metricUnit: %s\n", byteValue, metricUnit);
    TEST_ASSERT(strcmp(metricUnit, "TB") == 0, "Expected metricUnit is TB");
    TEST_ASSERT(byteValue == 8, "Expected byteValue is 8");
}

void run_unit_conversion_tests() {
    test_metric_Unit_Convert();
}