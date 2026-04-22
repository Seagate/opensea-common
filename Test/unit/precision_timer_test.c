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

void run_precision_timer_tests(void) {
    create_free_seatimer_test();
}