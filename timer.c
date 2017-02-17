//Author: Finn Matras & Sondre Ninive Andersen

#include "timer.h"

#include <time.h>

static clock_t last_reset_time = -(TIMEOUT_PERIOD_SECONDS * CLOCKS_PER_SEC + 1);

void timer_reset(void) {
	last_reset_time = clock();
}

bool timer_is_timed_out(void) {
	return (clock() - last_reset_time >= TIMEOUT_PERIOD_SECONDS * CLOCKS_PER_SEC);
}
