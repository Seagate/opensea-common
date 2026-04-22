#include"../framework/test_framework.h"
#include"../../include/precision_timer.h"
#include"../testConstants.h"
#include <unistd.h>

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
    usleep(SLEEP_MICROSECONDS);
    stop_Timer(&timer);
    uint64_t elapsedNanoSeconds = get_Nano_Seconds(timer);
    TEST_ASSERT(elapsedNanoSeconds >= (SLEEP_MICROSECONDS * 1000), "Elapsed nanoseconds should be at least the sleep time in nanoseconds");
}

void run_precision_timer_tests(void) {
    create_free_seatimer_test();
    get_Nano_Seconds_test();
}