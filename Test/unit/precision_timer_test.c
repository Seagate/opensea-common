#include"../framework/test_framework.h"
#include"../../include/precision_timer.h"
#include"../testConstants.h"

DECLARE_SEATIMER(timer);

static void start_Timer_test(void) {
    start_Timer(&timer);
    TEST_ASSERT(timer.timerStart != UINT64_C(0), "timer start time should not be zero after starting the timer");
}

void run_precision_timer_tests(void) {
    start_Timer_test();
}