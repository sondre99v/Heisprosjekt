#ifndef __ELEVATOR_CONTROLLER_H__
#define __ELEVATOR_CONTROLLER_H__

typedef enum {
	hit_1st,
	hit_2nd,
	hit_3rd,
	hit_4th,
	pressed_up_1st,
	pressed_down_2nd,
	pressed_up_2nd,
	pressed_down_3rd,
	pressed_up_3rd,
	pressed_down_4th,
	pressed_elevator_1st,
	pressed_elevator_2nd,
	pressed_elevator_3rd,
	pressed_elevator_4th,
	pressed_stop,
	released_stop,
	timer_timeout,
} Event_t;

void ec_event_raise(Event_t event);

#endif // #ifndef __ELEVATOR_CONTROLLER_H__