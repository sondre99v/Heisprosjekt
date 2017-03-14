#include "event_generator.h"

#include <stdbool.h>
#include "hardware.h"
#include "elevator_controller.h"
#include "timer.h"


// Array to iterate over the order-buttons. 
// Stop is handeled separatly, because it needs an event on release as well.
#define NUMBER_OF_BUTTONS 10
struct { 
	Button_t button; 
	bool previous_state; 
	Event_t event_to_raise_on_press; 
} buttons [NUMBER_OF_BUTTONS] = {
	{.button = button_floor_up_1st, 	.previous_state = false, event_pressed_up_1st},
	{.button = button_floor_down_2nd,	.previous_state = false, event_pressed_down_2nd},
	{.button = button_floor_up_2nd, 	.previous_state = false, event_pressed_up_2nd},
	{.button = button_floor_down_3rd, 	.previous_state = false, event_pressed_down_3rd},
	{.button = button_floor_up_3rd, 	.previous_state = false, event_pressed_up_3rd},
	{.button = button_floor_down_4th, 	.previous_state = false, event_pressed_down_4th},
	{.button = button_elevator_1st, 	.previous_state = false, event_pressed_elevator_1st},
	{.button = button_elevator_2nd, 	.previous_state = false, event_pressed_elevator_2nd},
	{.button = button_elevator_3rd, 	.previous_state = false, event_pressed_elevator_3rd},
	{.button = button_elevator_4th, 	.previous_state = false, event_pressed_elevator_4th}
};

bool previous_stop_button_state = false;
Floor_sensor_t previous_floor_sensor = floor_sensor_none;
bool previous_timeout_state = true;


void event_generator( void ) {
	ec_raise_event(event_startup);

	while (true) {
		// Handle buttons other than the stop button
		for (int index = 0; index < NUMBER_OF_BUTTONS; index++) {
			bool current_state = hw_is_button_pressed(buttons[index].button);
			if (current_state && !buttons[index].previous_state) {
				ec_raise_event(buttons[index].event_to_raise_on_press);
			}
			buttons[index].previous_state = current_state;
		}
		
		
		// Handle stop button
		bool current_stop_button_state = hw_is_button_pressed(button_stop);
		if (current_stop_button_state == false && previous_stop_button_state == true) {
			ec_raise_event(event_released_stop);
		}
		if (current_stop_button_state == true && previous_stop_button_state == false) {
			ec_raise_event(event_pressed_stop);
		}
		previous_stop_button_state = current_stop_button_state;
		
		
		// Handle floors
		Floor_sensor_t current_floor_sensor = hw_get_sensors_state();
		if (current_floor_sensor != previous_floor_sensor) {
			switch (current_floor_sensor) {
				case floor_sensor_1st: 
					ec_raise_event(event_hit_1st);
					break;
				case floor_sensor_2nd: 
					ec_raise_event(event_hit_2nd);
					break;
				case floor_sensor_3rd: 
					ec_raise_event(event_hit_3rd);
					break;
				case floor_sensor_4th: 
					ec_raise_event(event_hit_4th);
					break;
				default:
					break;
			}
		}
		
		previous_floor_sensor = current_floor_sensor;


		// Handle timer timeout
		bool current_timeout_state = timer_is_timed_out();
		if (current_timeout_state && !previous_timeout_state) {
			ec_raise_event(event_timer_timeout);
		}
		previous_timeout_state = current_timeout_state;
	}
}
