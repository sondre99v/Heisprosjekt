//Author: Finn Matras & Sondre Ninive Andersen

#include "elevator_controller.h"

#include "hardware.h"
#include "timer.h"
#include "order_manager.h"

#include <assert.h>
#include <stdio.h>

struct
{
	bool initialization_finished;
	Floor_t last_floor;
	Door_state_t door_state;
	Motor_state_t motor_state;
} elevator_state;


static void _action_open_door() {
	elevator_state.door_state = door_opened;
	hw_set_door_state(door_opened);
}

static void _action_close_door() {
	elevator_state.door_state = door_closed;
	hw_set_door_state(door_closed);
}

static void _action_move_up() {
	elevator_state.motor_state = moving_up;
	hw_set_motor_state(moving_up);
}

static void _action_move_down() {
	elevator_state.motor_state = moving_down;
	hw_set_motor_state(moving_down);
}

static void _action_stop_moving() {
	elevator_state.motor_state = stopped;
	hw_set_motor_state(stopped);
}

static void _set_last_floor(Floor_t last_floor) {
	assert (last_floor != floor_unknown);

	elevator_state.last_floor = last_floor;

	switch (last_floor) {
		case floor_1st:
			hw_set_led_state(led_floor_1st, true);
			break;
		case floor_2nd:
			hw_set_led_state(led_floor_2nd, true);
			break;
		case floor_3rd:
			hw_set_led_state(led_floor_3rd, true);
			break;
		case floor_4th:
			hw_set_led_state(led_floor_4th, true);
			break;
		default:
			break;
	}
}


static void _event_handle_startup() {
	printf("Event startup\n");
	_action_close_door(); // Door should already be closed from hw_init, but just in case
	_action_move_up(); // Move up to get to known location
	elevator_state.last_floor = floor_unknown;
	elevator_state.initialization_finished = false;
}

static void _event_handle_hit_1st() {
	printf("Event hit 1st\n");
	_action_move_up();
}

static void _event_handle_hit_2nd() {
	printf("Event hit 2nd\n");

}

static void _event_handle_hit_3rd() {
	printf("Event hit 3rd\n");

}

static void _event_handle_hit_4th() {
	printf("Event hit 4th\n");
	_action_move_down();
}

static void _event_handle_pressed_up_1st() {
	printf("Event pressed up 1st\n");
	om_add_new_order(floor_1st, direction_up);
	hw_set_led_state(led_up_1st, true);
}

static void _event_handle_pressed_down_2nd() {
	printf("Event pressed down 2nd\n");
	om_add_new_order(floor_2nd, direction_down);
	hw_set_led_state(led_down_2nd, true);

}

static void _event_handle_pressed_up_2nd() {
	printf("Event pressed up 2nd\n");
	om_add_new_order(floor_2nd, direction_up);
	hw_set_led_state(led_up_2nd, true);

}

static void _event_handle_pressed_down_3rd() {
	printf("Event pressed down 3rd\n");
	om_add_new_order(floor_3rd, direction_down);
	hw_set_led_state(led_down_3rd, true);

}

static void _event_handle_pressed_up_3rd() {
	printf("Event pressed up 3rd\n");
	om_add_new_order(floor_3rd, direction_up);
	hw_set_led_state(led_up_3rd, true);

}

static void _event_handle_pressed_down_4th() {
	printf("Event pressed down 4th\n");
	om_add_new_order(floor_4th, direction_down);
	hw_set_led_state(led_down_4th, true);

}

static void _event_handle_pressed_elevator_1st() {
	printf("Event pressed elevator 1st\n");

}

static void _event_handle_pressed_elevator_2nd() {
	printf("Event pressed elevator 2nd\n");

}

static void _event_handle_pressed_elevator_3rd() {
	printf("Event pressed elevator 3rd\n");

}

static void _event_handle_pressed_elevator_4th() {
	printf("Event pressed elevator 4th\n");

}

static void _event_handle_pressed_stop() {
	printf("Event pressed emergency stop\n");
}

static void _event_handle_released_stop() {
	printf("Event released emergency stop\n");

}

static void _event_handle_timer_timeout() {
	printf("Event timer timeout\n");

}

void ec_event_raise(Event_t event) {
	switch (event) {
		case startup:
			_event_handle_startup();
			break;
		case hit_1st:
			_event_handle_hit_1st();
			break;
		case hit_2nd:
			_event_handle_hit_2nd();
			break;
		case hit_3rd:
			_event_handle_hit_3rd();
			break;
		case hit_4th:
			_event_handle_hit_4th();
			break;
		case pressed_up_1st:
			_event_handle_pressed_up_1st();
			break;
		case pressed_down_2nd:
			_event_handle_pressed_down_2nd();
			break;
		case pressed_up_2nd:
			_event_handle_pressed_up_2nd();
			break;
		case pressed_down_3rd:
			_event_handle_pressed_down_3rd();
			break;
		case pressed_up_3rd:
			_event_handle_pressed_up_3rd();
			break;
		case pressed_down_4th:
			_event_handle_pressed_down_4th();
			break;
		case pressed_elevator_1st:
			_event_handle_pressed_elevator_1st();
			break;
		case pressed_elevator_2nd:
			_event_handle_pressed_elevator_2nd();
			break;
		case pressed_elevator_3rd:
			_event_handle_pressed_elevator_3rd();
			break;
		case pressed_elevator_4th:
			_event_handle_pressed_elevator_4th();
			break;
		case pressed_stop:
			_event_handle_pressed_stop();
			break;
		case released_stop:
			_event_handle_released_stop();
			break;
		case timer_timeout:
			_event_handle_timer_timeout();
			break;
		default: assert(false); // Unknown event!
	}
}
