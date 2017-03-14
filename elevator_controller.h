#ifndef __ELEVATOR_CONTROLLER_H__
#define __ELEVATOR_CONTROLLER_H__

typedef enum {
	event_startup,
	event_hit_1st,
	event_hit_2nd,
	event_hit_3rd,
	event_hit_4th,
	event_pressed_up_1st,
	event_pressed_down_2nd,
	event_pressed_up_2nd,
	event_pressed_down_3rd,
	event_pressed_up_3rd,
	event_pressed_down_4th,
	event_pressed_elevator_1st,
	event_pressed_elevator_2nd,
	event_pressed_elevator_3rd,
	event_pressed_elevator_4th,
	event_pressed_stop,
	event_released_stop,
	event_timer_timeout,
} Event_t;


void ec_raise_event(Event_t event);

#endif // #ifndef __ELEVATOR_CONTROLLER_H__