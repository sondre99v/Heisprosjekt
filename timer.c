//Author: Finn Matras & Sondre Ninive Andersen

#include "timer.h"

#include <time.h>

static clock_t timeout_time;

void timer_init() {
	timeout_time = 0;
}

void timer_reset(void) {
	timeout_time = clock() + TIMEOUT_PERIOD_SECONDS * CLOCKS_PER_SEC;
}

bool timer_is_timed_out(void) {
	return (clock() >= timeout_time);
}
