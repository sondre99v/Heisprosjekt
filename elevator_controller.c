//Author: Finn Matras & Sondre Ninive Andersen

#include "elevator_controller.h"

#include "hardware.h"
#include "timer.h"
#include "order_manager.h"

#include <assert.h>
#include <stdio.h>

typedef enum {
	searching_floor,
	idle,
	moving_to_floor,
	door_open,
	emergency_stop
} Fsm_state_t;

struct
{
	Fsm_state_t fsm_state;
	Floor_t last_floor;
	Motor_state_t motor_state;
	Order_t* current_processed_order;		//Find a better name!
} elevator_state;


static Led_t _get_led_for_order_button(Floor_t order_floor, Direction_t direction) {
	switch (order_floor) {
		case floor_1st: return led_up_1st;
		case floor_2nd: return (direction == direction_up) ? led_up_2nd : led_down_2nd;
		case floor_3rd: return (direction == direction_up) ? led_up_3rd : led_down_3rd;
		case floor_4th: return led_down_4th;
		default: return -1;
	}
}

static Led_t _get_led_for_dropoff_button(Floor_t button_floor) {
	switch (button_floor) {
		case floor_1st: return led_dropoff_1st;
		case floor_2nd: return led_dropoff_2nd;
		case floor_3rd: return led_dropoff_3rd;
		case floor_4th: return led_dropoff_4th;
		default: return -1;
	}
}


static void _action_open_door() {
	hw_set_door_state(door_opened);
	timer_reset();
}

static void _action_close_door() {
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
	printf("Moving towards floor %d from floor %d\n", (int)destination_floor, (int)elevator_state.last_floor);

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
			hw_set_floor_indicator(floor_led_1st);
			break;
		case floor_2nd:
			hw_set_floor_indicator(floor_led_2nd);
			break;
		case floor_3rd:
			hw_set_floor_indicator(floor_led_3rd);
			break;
		case floor_4th:
			hw_set_floor_indicator(floor_led_4th);
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
	elevator_state.fsm_state = searching_floor;
}

static void _event_hit_floor(Floor_t hit_floor) {
	printf("Event hit floor\n");
	_set_last_floor(hit_floor);

	// If we hit a floor for the first time, stop and set initialization to finished
	if (elevator_state.fsm_state == searching_floor) {
		_action_stop_moving();
		elevator_state.fsm_state = idle;
		printf("Initialization finished.\n");
		return;
	}

	// First cast current motor direction (enum from hardware) to a pickup direction (enum from order manager)
	Direction_t current_pickup_direction = direction_none;
	switch (elevator_state.motor_state) {
		case moving_up: current_pickup_direction = direction_up; break;
		case moving_down: current_pickup_direction = direction_down; break;
		default: break;
	}

	// Check if the current floor is the drop-off floor of any order
	Order_t* order_at_this_floor = om_contains_dropoff(hit_floor);
	if (order_at_this_floor != NULL) {
		printf("Stopped to drop-off other order.\n");
		_action_stop_moving();
		_action_open_door();
		elevator_state.fsm_state = door_open;
		om_remove_orders_with_dropoff(hit_floor);
		hw_set_led_state(_get_led_for_dropoff_button(hit_floor), false);
	}

	// Check if this floor is the pickup of any order with the same as the current direction
	order_at_this_floor = om_contains_pickup(hit_floor, current_pickup_direction);

	if (order_at_this_floor == NULL) {
		order_at_this_floor = om_get_first_order();
	}

	if (order_at_this_floor != NULL && order_at_this_floor->pickup_floor == hit_floor) {
		printf("Stopped for pick-up.\n");
		_action_stop_moving();
		_action_open_door();
		elevator_state.fsm_state = door_open;
		elevator_state.current_processed_order = order_at_this_floor;

		// Turn off pickup led
		hw_set_led_state(_get_led_for_order_button(hit_floor, order_at_this_floor -> pickup_direction), false);
	}
}

static void _event_handle_pressed_order_button(Floor_t order_floor, Direction_t direction) {
	if (elevator_state.fsm_state == searching_floor || elevator_state.fsm_state == emergency_stop) return;

	printf("Event pressed %d\n", (int)order_floor);
	om_add_new_order(order_floor, direction);
	hw_set_led_state(_get_led_for_order_button(order_floor, direction), true);

	if (elevator_state.fsm_state == idle) {
		_action_go_towards(order_floor);
	}
}


static void _event_handle_pressed_elevator_1st() {
	if (elevator_state.fsm_state == searching_floor || elevator_state.fsm_state == emergency_stop) return;

	/*if ((elevator_state.fsm_state == idle || elevator_state.fsm_state == door_open) && elevator_state.last_floor == floor_led_1st) {
		// Ignore dropoff requests for the current floor
		return;
	}*/

	printf("Event pressed elevator 1st\n");
	if (elevator_state.current_processed_order == NULL) {
		om_add_new_dropoff_only_order(floor_1st);
	} else {
		om_add_dropoff_to_order(elevator_state.current_processed_order, floor_1st);
	}
	hw_set_led_state(led_dropoff_1st, true);

	if (elevator_state.fsm_state == idle) {
		_action_go_towards(floor_1st);
	}
}

static void _event_handle_pressed_elevator_2nd() {
	if (elevator_state.fsm_state == searching_floor || elevator_state.fsm_state == emergency_stop) return;

	/*if ((elevator_state.fsm_state == idle || elevator_state.fsm_state == door_open) && elevator_state.last_floor == floor_led_2nd) {
		// Ignore dropoff requests for the current floor
		return;
	}*/

	printf("Event pressed elevator 2nd\n");
		if (elevator_state.current_processed_order == NULL) {
		om_add_new_dropoff_only_order(floor_2nd);
	} else {
		om_add_dropoff_to_order(elevator_state.current_processed_order, floor_2nd);
	}
	hw_set_led_state(led_dropoff_2nd, true);

	if (elevator_state.fsm_state == idle) {
		_action_go_towards(floor_2nd);
	}
}

static void _event_handle_pressed_elevator_3rd() {
	if (elevator_state.fsm_state == searching_floor || elevator_state.fsm_state == emergency_stop) return;

	/*if ((elevator_state.fsm_state == idle || elevator_state.fsm_state == door_open) && elevator_state.last_floor == floor_led_3rd) {
		// Ignore dropoff requests for the current floor
		return;
	}*/

	printf("Event pressed elevator 3rd\n");
	if (elevator_state.current_processed_order == NULL){
		om_add_new_dropoff_only_order(floor_3rd);
	} else {
		om_add_dropoff_to_order(elevator_state.current_processed_order, floor_3rd);
	}
	hw_set_led_state(led_dropoff_3rd, true);

	if (elevator_state.fsm_state == idle) {
		_action_go_towards(floor_3rd);
	}	
}

static void _event_handle_pressed_elevator_4th() {
	if (elevator_state.fsm_state == searching_floor || elevator_state.fsm_state == emergency_stop) return;

	/*if ((elevator_state.fsm_state == idle || elevator_state.fsm_state == door_open) && elevator_state.last_floor == floor_led_4th) {
		// Ignore dropoff requests for the current floor
		return;
	}*/

	printf("Event pressed elevator 4th\n");
	if (elevator_state.current_processed_order == NULL){
		om_add_new_dropoff_only_order(floor_4th);
	} else {
		om_add_dropoff_to_order(elevator_state.current_processed_order, floor_4th);
	}
	hw_set_led_state(led_dropoff_4th, true);

	if (elevator_state.fsm_state == idle) {
		_action_go_towards(floor_4th);
	}
}

static void _event_handle_pressed_stop() {
	if (elevator_state.fsm_state == searching_floor || elevator_state.fsm_state == emergency_stop) return;

	printf("Event pressed emergency stop\n");
	_action_stop_moving();
	om_clear_all_orders();

	// Open door if on a floor
	if (hw_get_sensors_state() != floor_sensor_none){
		_action_open_door();
	} else {
		_action_close_door();
	}

	hw_clear_all_leds();
	hw_set_led_state(led_stop, true);
	elevator_state.fsm_state = emergency_stop;
}

static void _event_handle_released_stop() {
	if (elevator_state.fsm_state == searching_floor) return;

	printf("Event released emergency stop\n");
	hw_set_led_state(led_stop, false);
	timer_reset();

	elevator_state.fsm_state = idle;
}

static void _event_handle_timer_timeout() {
	if (elevator_state.fsm_state == searching_floor || elevator_state.fsm_state == emergency_stop) return;

	printf("Event timer timeout\n");
	_action_close_door();

	if (elevator_state.current_processed_order != NULL && elevator_state.current_processed_order -> dropoff_floor == floor_unknown) {
		om_remove_order(elevator_state.current_processed_order);
	}

	elevator_state.current_processed_order = NULL;

	Order_t* next_order = om_get_first_order();
	if (next_order != NULL) {
		if (next_order->dropoff_floor != floor_unknown){
			_action_go_towards(next_order -> dropoff_floor);
		} else {
			_action_go_towards(next_order -> pickup_floor);
		}
		elevator_state.fsm_state = moving_to_floor;
	} else {
		elevator_state.fsm_state = idle;
	}
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
			_event_handle_pressed_order_button(floor_1st, direction_up);
			break;
		case pressed_down_2nd:
			_event_handle_pressed_order_button(floor_2nd, direction_down);
			break;
		case pressed_up_2nd:
			_event_handle_pressed_order_button(floor_2nd, direction_up);
			break;
		case pressed_down_3rd:
			_event_handle_pressed_order_button(floor_3rd, direction_down);
			break;
		case pressed_up_3rd:
			_event_handle_pressed_order_button(floor_3rd, direction_up);
			break;
		case pressed_down_4th:
			_event_handle_pressed_order_button(floor_4th, direction_down);
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
