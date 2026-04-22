#include"../framework/test_framework.h"
#include"../../include/precision_timer.h"
#include"../../include/sleep.h"
#include"../testConstants.h"

DECLARE_SEATIMER(timer);

// Creates and frees a seatimer on the heap to test the safe_free_seatimer function.
static void create_free_seatimer_test(void) {
    NEW_SEATIMER(heapTimer);
    TEST_ASSERT(heapTimer != M_NULLPTR, "heapTimer should not be null after allocation");
    safe_free_seatimer(&heapTimer);
    TEST_ASSERT(heapTimer == M_NULLPTR, "heapTimer should be null after freeing");
}

static void get_Nano_Seconds_test(void) {
    start_Timer(&timer);
    sleepns(SLEEP_MICROSECONDS * 1000); // Convert microseconds to nanoseconds
    stop_Timer(&timer);
    uint64_t elapsedNanoSeconds = get_Nano_Seconds(timer);
    TEST_ASSERT(elapsedNanoSeconds >= (SLEEP_MICROSECONDS * 1000), "Elapsed nanoseconds should be at least the sleep time in nanoseconds");
}

static void get_Micro_Seconds_test(void) {
    start_Timer(&timer);
    sleepus(SLEEP_MICROSECONDS);
    stop_Timer(&timer);
    double elapsedMicroSeconds = get_Micro_Seconds(timer);
    TEST_ASSERT(elapsedMicroSeconds >= SLEEP_MICROSECONDS, "Elapsed microseconds should be at least the sleep time in microseconds");
}

static void get_Milli_Seconds_test(void) {
    start_Timer(&timer);
    sleepms(SLEEP_MICROSECONDS / 1000); // Convert microseconds to milliseconds
    stop_Timer(&timer);
    double elapsedMilliSeconds = get_Milli_Seconds(timer);
    TEST_ASSERT(elapsedMilliSeconds >= (SLEEP_MICROSECONDS / 1000.0), "Elapsed milliseconds should be at least the sleep time in milliseconds");
}

static void get_Seconds_test(void) {
    start_Timer(&timer);
    uint32_t threshold = UINT32_MAX / 1000;
    errno_t err = sleepsec(threshold + 1);
    TEST_ASSERT(err == EOVERFLOW, "sleepsec should return EOVERFLOW when sleep time exceeds maximum");
    sleepsec(SLEEP_MICROSECONDS / 1000000); // Convert microseconds to seconds
    stop_Timer(&timer);
    double elapsedSeconds = get_Seconds(timer);
    TEST_ASSERT(elapsedSeconds >= (SLEEP_MICROSECONDS / 1000000.0), "Elapsed seconds should be at least the sleep time in seconds");
}

void run_precision_timer_and_sleep_tests(void) {
    create_free_seatimer_test();
    get_Nano_Seconds_test();
    get_Micro_Seconds_test();
    get_Milli_Seconds_test();
    get_Seconds_test();
}