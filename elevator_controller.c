//Author: Finn Matras & Sondre Ninive Andersen

#include "elevator_controller.h"
#include "hardware.h"
#include "timer.h"
#include "order_manager.h"

#include <assert.h>
#include <stdio.h>


#define POSITION_UNKNOWN	-1.0
#define POSITION_HALF_FLOOR	0.5
#define POSITION_AT_1ST 	1.0
#define BETWEEN_1ST_AND_2ND	1.5
#define POSITION_AT_2ND		2.0
#define BETWEEN_2ND_AND_3RD	2.5
#define POSITION_AT_3RD		3.0
#define BETWEEN_3RD_AND_4TH	3.5
#define POSITION_AT_4TH		4.0


typedef enum {
	fsm_state_searching_floor,
	fsm_state_idle,
	fsm_state_moving_to_floor,
	fsm_state_door_open,
	 fsm_state_emergency_stop
} Fsm_state_t;


struct {
	Fsm_state_t fsm_state;
	float position;
	Motor_state_t motor_state;
	Order_t* order_to_finalize;
} elevator_state;


static Button_t _get_floor_button_for_floor(Floor_t order_floor, Direction_t direction) {
	switch (order_floor) {
		case floor_1st: return button_floor_up_1st;
		case floor_2nd: return (direction == direction_up) ? button_floor_up_2nd : button_floor_down_2nd;
		case floor_3rd: return (direction == direction_up) ? button_floor_up_3rd : button_floor_down_3rd;
		case floor_4th: return button_floor_down_4th;
		default: assert(false);
	}
}

static Button_t _get_elevator_button_for_floor(Floor_t button_floor) {
	switch (button_floor) {
		case floor_1st: return button_elevator_1st;
		case floor_2nd: return button_elevator_2nd;
		case floor_3rd: return button_elevator_3rd;
		case floor_4th: return button_elevator_4th;
		default: assert(false);
	}
}

static Direction_t _direction_to_floor(Floor_t test_floor) {
	switch (test_floor) {
		case floor_1st: 
			if (elevator_state.position < POSITION_AT_1ST) return direction_up;
			if (elevator_state.position == POSITION_AT_1ST) return direction_none;
			if (elevator_state.position > POSITION_AT_1ST) return direction_down;
		case floor_2nd:
			if (elevator_state.position < POSITION_AT_2ND) return direction_up;
			if (elevator_state.position == POSITION_AT_2ND) return direction_none;
			if (elevator_state.position > POSITION_AT_2ND) return direction_down;
		case floor_3rd:
			if (elevator_state.position < POSITION_AT_3RD) return direction_up;
			if (elevator_state.position == POSITION_AT_3RD) return direction_none;
			if (elevator_state.position > POSITION_AT_3RD) return direction_down;
		case floor_4th:
			if (elevator_state.position < POSITION_AT_4TH) return direction_up;
			if (elevator_state.position == POSITION_AT_4TH) return direction_none;
			if (elevator_state.position > POSITION_AT_4TH) return direction_down;
		default: return direction_none;
	}
}

static bool _elevator_is_at_floor(Floor_t test_floor) {
	switch (test_floor) {
		case floor_1st: return (elevator_state.position == POSITION_AT_1ST);
		case floor_2nd: return (elevator_state.position == POSITION_AT_2ND);
		case floor_3rd: return (elevator_state.position == POSITION_AT_3RD);
		case floor_4th: return (elevator_state.position == POSITION_AT_4TH);
		default: return false;
	}
}

static bool _elevator_is_at_any_floor() {
	return (elevator_state.position == POSITION_AT_1ST) ||
		(elevator_state.position == POSITION_AT_2ND) ||
		(elevator_state.position == POSITION_AT_3RD) ||
		(elevator_state.position == POSITION_AT_4TH);
}


static void _action_open_door() {
	hw_set_door_state(door_opened);
	timer_reset();
}

static void _action_close_door() {
	hw_set_door_state(door_closed);
}

static void _action_move_up() {
	elevator_state.motor_state = motor_moving_up;
	hw_set_motor_state(motor_moving_up);
}

static void _action_move_down() {
	elevator_state.motor_state = motor_moving_down;
	hw_set_motor_state(motor_moving_down);
}

static void _action_stop_moving() {
	elevator_state.motor_state = motor_stopped;
	hw_set_motor_state(motor_stopped);
}

static void _action_go_towards(Floor_t destination_floor) {
	printf("Moving towards floor %d from position %.1f\n...\n", (int)destination_floor, elevator_state.position);

	switch (_direction_to_floor(destination_floor)) {
		case direction_down:
			_action_move_down();
			if (_elevator_is_at_any_floor()) {
				// Elevator is leaving floor, set position to between floors
				elevator_state.position -= POSITION_HALF_FLOOR;
			}
			break;
		case direction_up:
			_action_move_up();
			if (_elevator_is_at_any_floor()) {
				// Elevator is leaving floor, set position to between floors
				elevator_state.position += POSITION_HALF_FLOOR;
			}
			break;
		case direction_none:
			_action_stop_moving();
	}
}

static void _set_last_floor(Floor_t last_floor) {
	assert (last_floor != floor_unknown);

	switch (last_floor) {
		case floor_1st:
			elevator_state.position = POSITION_AT_1ST;
			hw_set_floor_indicator(floor_led_1st);
			break;
		case floor_2nd:
			elevator_state.position = POSITION_AT_2ND;
			hw_set_floor_indicator(floor_led_2nd);
			break;
		case floor_3rd:
			elevator_state.position = POSITION_AT_3RD;
			hw_set_floor_indicator(floor_led_3rd);
			break;
		case floor_4th:
			elevator_state.position = POSITION_AT_4TH;
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
	elevator_state.position = POSITION_UNKNOWN;
	elevator_state.fsm_state = fsm_state_searching_floor;
}


static void _event_hit_floor(Floor_t hit_floor) {
	printf("Event hit floor %d\n", (int)hit_floor);
	_set_last_floor(hit_floor);

	// If we hit a floor for the first time, stop and set initialization to finished
	if (elevator_state.fsm_state == fsm_state_searching_floor) {
		_action_stop_moving();
		elevator_state.fsm_state = fsm_state_idle;
		printf("Initialization finished\n");
		return;
	}

	// First cast current motor direction (enum from hardware) to a pickup direction (enum from order manager)
	Direction_t current_pickup_direction = direction_none;
	switch (elevator_state.motor_state) {
		case motor_moving_up: current_pickup_direction = direction_up; break;
		case motor_moving_down: current_pickup_direction = direction_down; break;
		default: break;
	}

	// Check if the current floor is the drop-off floor of any order
	Order_t* order_at_this_floor = om_contains_dropoff(hit_floor);
	if (order_at_this_floor != NULL) {
		printf("Stopped for drop-off\n");
		_action_stop_moving();
		_action_open_door();
		elevator_state.fsm_state = fsm_state_door_open;
		om_remove_orders_with_dropoff(hit_floor);
		hw_set_button_led_state(_get_elevator_button_for_floor(hit_floor), led_off);
	}

	// Check if this floor is the pickup of any order with the same as the current direction
	order_at_this_floor = om_contains_pickup(hit_floor, current_pickup_direction);

	if (order_at_this_floor == NULL) {
		order_at_this_floor = om_get_first_order();
	}

	if (order_at_this_floor != NULL && order_at_this_floor->dropoff_floor == floor_unknown && order_at_this_floor->pickup_floor == hit_floor) {
		printf("Stopped for pick-up\n");
		_action_stop_moving();
		_action_open_door();
		elevator_state.fsm_state = fsm_state_door_open;
		elevator_state.order_to_finalize = order_at_this_floor;

		// Turn off pickup led
		hw_set_button_led_state(_get_floor_button_for_floor(hit_floor, order_at_this_floor -> pickup_direction), led_off);
	}

	// Avoid running off the track, just in case
	if (elevator_state.fsm_state == fsm_state_moving_to_floor && (hit_floor == floor_4th || hit_floor == floor_1st)) {
		_action_stop_moving();
		elevator_state.fsm_state = fsm_state_idle;
	}
}


static void _event_handle_pressed_order_button(Floor_t order_floor, Direction_t direction) {
	if (elevator_state.fsm_state == fsm_state_searching_floor || elevator_state.fsm_state ==  fsm_state_emergency_stop) return;

	printf("Event pressed %d\n", (int)order_floor);

	if (elevator_state.fsm_state == fsm_state_moving_to_floor || !_elevator_is_at_floor(order_floor)) {
		// We are moving, or stationary at the wrong floor
		om_add_new_order(order_floor, direction);
		hw_set_button_led_state(_get_floor_button_for_floor(order_floor, direction), led_on);

		if (elevator_state.fsm_state == fsm_state_idle) {
			_action_go_towards(order_floor);
			elevator_state.fsm_state = fsm_state_moving_to_floor;
		}
	} else if (elevator_state.fsm_state == fsm_state_idle && _elevator_is_at_floor(order_floor)) {
		// We are stationary (with door closed) at the correct floor
		om_add_new_order(order_floor, direction);
		elevator_state.order_to_finalize = om_contains_pickup(order_floor, direction);
		_action_open_door();
		elevator_state.fsm_state = fsm_state_door_open;
	}
}


static void _event_handle_pressed_elevator_button(Floor_t button_floor) {
	if (elevator_state.fsm_state == fsm_state_searching_floor || elevator_state.fsm_state == fsm_state_emergency_stop) return;

	printf("Event pressed elevator %d\n", (int)button_floor);

	if (elevator_state.fsm_state == fsm_state_moving_to_floor) {
		// Moving, add dropoff to queue
		om_add_new_dropoff_only_order(button_floor);
		hw_set_button_led_state(_get_elevator_button_for_floor(button_floor), led_on);
	} else {
		// Not moving, either open door, add dropoff to existing order, or add new dropoff
		if (_elevator_is_at_floor(button_floor)) {
			// Button for this floor pressed, open door if not already open
			if (elevator_state.fsm_state != fsm_state_door_open) {
				_action_open_door();
				elevator_state.fsm_state = fsm_state_door_open;
			}
		} else {
			// Button for other floor pressed, add dropoff to existing or new order
			if (elevator_state.order_to_finalize != NULL) {
				// Expecting a dropoff entry, put it in the order
				om_add_dropoff_to_order(elevator_state.order_to_finalize, button_floor);
			} else {
				// Not expecting a dropoff entry, add new dropoff-only order
				om_add_new_dropoff_only_order(button_floor);
			}
			hw_set_button_led_state(_get_elevator_button_for_floor(button_floor), led_on);

			if (elevator_state.fsm_state == fsm_state_idle) {
				_action_go_towards(button_floor);
				elevator_state.fsm_state = fsm_state_moving_to_floor;
			}
		}
	}
}


static void _event_handle_pressed_stop() {
	if (elevator_state.fsm_state == fsm_state_searching_floor || elevator_state.fsm_state == fsm_state_emergency_stop) return;

	printf("Event pressed emergency stop\n");

	_action_stop_moving();
	om_remove_all_orders();

	// Open door if on a floor
	if (hw_get_sensors_state() != floor_sensor_none){
		_action_open_door();
	} else {
		_action_close_door();
	}

	hw_clear_all_leds();
	hw_set_button_led_state(button_stop, led_on);
	elevator_state.fsm_state = fsm_state_emergency_stop;
}


static void _event_handle_released_stop() {
	if (elevator_state.fsm_state == fsm_state_searching_floor) return;

	printf("Event released emergency stop\n");
	hw_set_button_led_state(button_stop, led_off);
	timer_reset();

	elevator_state.fsm_state = fsm_state_idle;
}


static void _event_handle_timer_timeout() {
	if (elevator_state.fsm_state == fsm_state_searching_floor || elevator_state.fsm_state == fsm_state_emergency_stop) return;

	printf("Event timer timeout\n");
	_action_close_door();

	if (elevator_state.order_to_finalize != NULL && elevator_state.order_to_finalize -> dropoff_floor == floor_unknown) {
		om_remove_order(elevator_state.order_to_finalize);
	}

	elevator_state.order_to_finalize = NULL;

	Order_t* next_order = om_get_first_order();
	if (next_order != NULL) {
		if (next_order->dropoff_floor != floor_unknown){
			_action_go_towards(next_order -> dropoff_floor);
		} else {
			_action_go_towards(next_order -> pickup_floor);
		}
		elevator_state.fsm_state = fsm_state_moving_to_floor;
	} else {
		elevator_state.fsm_state = fsm_state_idle;
	}
}


void ec_raise_event(Event_t event) {
	switch (event) {
		case event_startup:
			_event_handle_startup();
			break;
		case event_hit_1st:
			_event_hit_floor(floor_1st);
			break;
		case event_hit_2nd:
			_event_hit_floor(floor_2nd);
			break;
		case event_hit_3rd:
			_event_hit_floor(floor_3rd);
			break;
		case event_hit_4th:
			_event_hit_floor(floor_4th);
			break;
		case event_pressed_up_1st:
			_event_handle_pressed_order_button(floor_1st, direction_up);
			break;
		case event_pressed_down_2nd:
			_event_handle_pressed_order_button(floor_2nd, direction_down);
			break;
		case event_pressed_up_2nd:
			_event_handle_pressed_order_button(floor_2nd, direction_up);
			break;
		case event_pressed_down_3rd:
			_event_handle_pressed_order_button(floor_3rd, direction_down);
			break;
		case event_pressed_up_3rd:
			_event_handle_pressed_order_button(floor_3rd, direction_up);
			break;
		case event_pressed_down_4th:
			_event_handle_pressed_order_button(floor_4th, direction_down);
			break;
		case event_pressed_elevator_1st:
			_event_handle_pressed_elevator_button(floor_1st);
			break;
		case event_pressed_elevator_2nd:
			_event_handle_pressed_elevator_button(floor_2nd);
			break;
		case event_pressed_elevator_3rd:
			_event_handle_pressed_elevator_button(floor_3rd);
			break;
		case event_pressed_elevator_4th:
			_event_handle_pressed_elevator_button(floor_4th);
			break;
		case event_pressed_stop:
			_event_handle_pressed_stop();
			break;
		case event_released_stop:
			_event_handle_released_stop();
			break;
		case event_timer_timeout:
			_event_handle_timer_timeout();
			break;
		default: assert(false); // Unknown event!
	}
}
