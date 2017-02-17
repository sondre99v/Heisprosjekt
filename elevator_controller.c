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
	Order_t* current_processed_order;		//Find a better name!
} elevator_state;


static void _action_open_door() {
	elevator_state.door_state = door_opened;
	hw_set_door_state(door_opened);
	timer_reset();
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

static void _action_go_towards(Floor_t destination_floor) {
	if (destination_floor > elevator_state.last_floor){
		_action_move_up();
	} else if (destination_floor < elevator_state.last_floor) {
		_action_move_down();
	} else {
		_action_stop_moving();
	}
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

static void _event_hit_floor(Floor_t hit_floor) {
	// If we hit a floor for the first time, stop and set initialization to finished
	if (!elevator_state.initialization_finished) {
		_action_stop_moving();
		elevator_state.initialization_finished = true;
		return;
	}

	printf("Event hit floor\n");

	// Check if this floor is the pickup of the most prioritized order
	Order_t* order_at_this_floor = om_get_first_order();
	if (order_at_this_floor != NULL && order_at_this_floor -> pickup_floor == hit_floor) {
		printf("Stopped to pick-up first order.\n");
		_action_stop_moving();
		_action_open_door();
		elevator_state.current_processed_order = order_at_this_floor;
		return;
	}

	// Check if this floor is the pickup of any order with the same as the current direction
	// First cast current motor direction (enum from hardware) to a pickup direction (enum from order manager)
	Direction_t current_pickup_direction = direction_none;
	switch (elevator_state.motor_state) {
		case moving_up: current_pickup_direction = direction_up; break;
		case moving_down: current_pickup_direction = direction_down; break;
		default: break;
	}

	order_at_this_floor = om_contains_pickup(hit_floor, current_pickup_direction);
	if (order_at_this_floor != NULL) {
		printf("Stopped to pick-up other order.\n");
		_action_stop_moving();
		_action_open_door();
		elevator_state.current_processed_order = order_at_this_floor;
		return;
	}

	// Check if the current floor is the drop-off floor of any order
	order_at_this_floor = om_contains_dropoff(hit_floor);
	if (order_at_this_floor != NULL) {
		printf("Stopped to drop-off other order.\n");
		_action_stop_moving();
		_action_open_door();
		om_remove_order(order_at_this_floor);
		return;
	}
}

static void _event_handle_pressed_up_1st() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed up 1st\n");
	om_add_new_order(floor_1st, direction_up);
	hw_set_led_state(led_up_1st, true);
}

static void _event_handle_pressed_down_2nd() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed down 2nd\n");
	om_add_new_order(floor_2nd, direction_down);
	hw_set_led_state(led_down_2nd, true);

}

static void _event_handle_pressed_up_2nd() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed up 2nd\n");
	om_add_new_order(floor_2nd, direction_up);
	hw_set_led_state(led_up_2nd, true);

}

static void _event_handle_pressed_down_3rd() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed down 3rd\n");
	om_add_new_order(floor_3rd, direction_down);
	hw_set_led_state(led_down_3rd, true);

}

static void _event_handle_pressed_up_3rd() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed up 3rd\n");
	om_add_new_order(floor_3rd, direction_up);
	hw_set_led_state(led_up_3rd, true);

}

static void _event_handle_pressed_down_4th() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed down 4th\n");
	om_add_new_order(floor_4th, direction_down);
	hw_set_led_state(led_down_4th, true);

}

static void _event_handle_pressed_elevator_1st() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed elevator 1st\n");
	if (elevator_state.current_processed_order == NULL) {
		om_add_new_dropoff_only_order(floor_1st);
	} else {
		om_add_dropoff_to_order(elevator_state.current_processed_order, floor_1st);
	}
	hw_set_led_state(led_dropoff_1st, true);

}

static void _event_handle_pressed_elevator_2nd() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed elevator 2nd\n");
		if (elevator_state.current_processed_order == NULL) {
		om_add_new_dropoff_only_order(floor_2nd);
	} else {
		om_add_dropoff_to_order(elevator_state.current_processed_order, floor_2nd);
	}
	hw_set_led_state(led_dropoff_2nd, true);

}

static void _event_handle_pressed_elevator_3rd() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed elevator 3rd\n");
	if (elevator_state.current_processed_order == NULL){
		om_add_new_dropoff_only_order(floor_3rd);
	} else {
		om_add_dropoff_to_order(elevator_state.current_processed_order, floor_3rd);
	}
	hw_set_led_state(led_dropoff_3rd, true);	
}

static void _event_handle_pressed_elevator_4th() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed elevator 4th\n");
	if (elevator_state.current_processed_order == NULL){
		om_add_new_dropoff_only_order(floor_4th);
	} else {
		om_add_dropoff_to_order(elevator_state.current_processed_order, floor_4th);
	}
	hw_set_led_state(led_dropoff_4th, true);
}

static void _event_handle_pressed_stop() {
	if (!elevator_state.initialization_finished) return;

	printf("Event pressed emergency stop\n");
	_action_stop_moving();
	om_clear_all_orders();
	if (hw_get_sensors_state() != floor_sensor_none){
		_action_open_door();
	} else {
		_action_close_door();
	}

	//hw_clear_all_leds();
	hw_set_led_state(led_stop, true);
}

static void _event_handle_released_stop() {
	if (!elevator_state.initialization_finished) return;

	printf("Event released emergency stop\n");
	hw_set_led_state(led_stop, false);
}

static void _event_handle_timer_timeout() {
	if (!elevator_state.initialization_finished) return;

	printf("Event timer timeout\n");
	_action_close_door();
	elevator_state.current_processed_order = NULL;
	Order_t* next_order = om_get_first_order();
	if (next_order->dropoff_floor != floor_unknown){
		_action_go_towards(next_order->dropoff_floor);
	}
	//If there is a dropoff floor, go to it, if not, go to the pickup floor.
}

void ec_event_raise(Event_t event) {
	switch (event) {
		case startup:
			_event_handle_startup();
			break;
		case hit_1st:
			_event_hit_floor(floor_1st);
			break;
		case hit_2nd:
			_event_hit_floor(floor_2nd);
			break;
		case hit_3rd:
			_event_hit_floor(floor_3rd);
			break;
		case hit_4th:
			_event_hit_floor(floor_4th);
			break;
		case pressed_up_1st:
			if (elevator_state.initialization_finished)
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
