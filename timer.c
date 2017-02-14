#include "timer.h"

#include <time.c>

static clock_t last_reset_time;

void timer_reset(void) {
	last_reset_time = clock();
}

bool timer_is_timed_out(void) {
	return (last_reset_time - clock() >= TIMEOUT_PERIOD_SECONDS * CLOCKS_PER_SEC);
}
