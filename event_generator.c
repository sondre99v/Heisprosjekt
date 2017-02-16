//Author: Finn Matras & Sondre Ninive Andersen

#include "event_generator.h"

#include <stdbool.h>
#include "hardware.h"
#include "elevator_controller.h"
#include "timer.h"

struct { Button_t button; bool previous_state; Event_t event_to_raise_on_press; } buttons [] = {
	{.button = button_up_1st, .previous_state = false	, pressed_up_1st},
	{.button = button_down_2nd, .previous_state = false	, pressed_down_2nd},
	{.button = button_up_2nd, .previous_state = false	, pressed_up_2nd},
	{.button = button_down_3rd, .previous_state = false	, pressed_down_3rd},
	{.button = button_down_4th, .previous_state = false	, pressed_up_3rd},
	{.button = button_stop, .previous_state = false		, pressed_down_4th},
	{.button = button_floor_1st, .previous_state = false, pressed_elevator_1st},
	{.button = button_floor_2nd, .previous_state = false, pressed_elevator_2nd},
	{.button = button_floor_3rd, .previous_state = false, pressed_elevator_3rd},
	{.button = button_floor_4th, .previous_state = false, pressed_elevator_4th}
};

bool stop_button_previous_state = false;

Floor_sensor_t previous_floor = floor_sensor_none;

void event_generator(void){
	while (true) {
		// Handle buttons other than the stop button
		for (int index = 0; index < sizeof(buttons) / sizeof(buttons[0]) ; index++) {
			bool current_state = hw_is_button_pressed(buttons[index].button);
			if (current_state && !buttons[index].previous_state) {
				ec_event_raise(buttons[index].event_to_raise_on_press);
			}
			buttons[index].previous_state = current_state;
		}
		
		
		// Handle stop button
		bool stop_button_current_state = hw_is_button_pressed(button_stop);
		if (stop_button_current_state == false && stop_button_previous_state == true) {
			ec_event_raise(released_stop);
		}
		if (stop_button_current_state == true && stop_button_previous_state == false) {
			ec_event_raise(pressed_stop);
		}
		stop_button_previous_state = hw_is_button_pressed(button_stop);
		
		
		// Handle floors
		Floor_sensor_t current_floor = hw_get_sensors_state();
		if (current_floor != previous_floor) {
			switch (current_floor) {
				case floor_sensor_1st: 
					ec_event_raise(hit_1st);
					break;
				case floor_sensor_2nd: 
					ec_event_raise(hit_2nd);
					break;
				case floor_sensor_3rd: 
					ec_event_raise(hit_3rd);
					break;
				case floor_sensor_4th: 
					ec_event_raise(hit_4th);
					break;
				default:
					break;
			}
		}
		
		previous_floor = current_floor;
	}
}
