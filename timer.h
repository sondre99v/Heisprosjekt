//Author: Finn Matras & Sondre Ninive Andersen

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdbool.h>

#define TIMEOUT_PERIOD_SECONDS 3


void timer_init( void );

void timer_reset( void );

bool timer_is_timed_out( void );

#endif // #ifndef __TIMER_H__