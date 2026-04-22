#include"../framework/test_framework.h"
#include"../../include/precision_timer.h"
#include"../testConstants.h"

DECLARE_SEATIMER(timer);

// Creates and frees a seatimer on the heap to test the safe_free_seatimer function.
static void create_free_seatimer_test(void) {
    NEW_SEATIMER(heapTimer);
    TEST_ASSERT(heapTimer != M_NULLPTR, "heapTimer should not be null after allocation");
    safe_free_seatimer(&heapTimer);
    TEST_ASSERT(heapTimer == M_NULLPTR, "heapTimer should be null after freeing");
}

static void start_Timer_test(void) {
    start_Timer(&timer);
    TEST_ASSERT(timer.timerStart != UINT64_C(0), "timer start time should not be zero after starting the timer");
}

static void stop_Timer_test(void) {
    stop_Timer(&timer);
    TEST_ASSERT(timer.timerStop != UINT64_C(0), "timer stop time should not be zero after stopping the timer");
}

void run_precision_timer_tests(void) {
    start_Timer_test();
    stop_Timer_test();
}