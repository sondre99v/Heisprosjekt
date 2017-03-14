#include "hardware.h"
#include "io.h"

#define MOTOR_SPEED         2800


void hw_init( void ) {
    io_init();

    hw_clear_all_leds();
    hw_set_floor_indicator(floor_led_1st);

    hw_set_motor_state(motor_stopped);
}


void hw_set_motor_state(Motor_state_t state) {
    switch (state) {
        case motor_moving_down:
            io_set_bit(MOTORDIR);
            io_write_analog(MOTOR, MOTOR_SPEED);
            break;
        case motor_stopped:
            io_write_analog(MOTOR, 0);
            break;
        case motor_moving_up:
            io_clear_bit(MOTORDIR);
            io_write_analog(MOTOR, MOTOR_SPEED);
    }
}


void hw_set_button_led_state(Button_t button, Led_state_t state) {
    short unsigned int address = 0;
    switch (button) {
        case button_floor_up_1st:   address = LIGHT_UP1;		break;
        case button_floor_down_2nd: address = LIGHT_DOWN2;  	break;
        case button_floor_up_2nd:   address = LIGHT_UP2;		break;
        case button_floor_down_3rd: address = LIGHT_DOWN3;  	break;
        case button_floor_up_3rd:   address = LIGHT_UP3;		break;
        case button_floor_down_4th: address = LIGHT_DOWN4;		break;
        case button_elevator_1st:   address = LIGHT_COMMAND1;	break;
        case button_elevator_2nd:   address = LIGHT_COMMAND2;	break;
        case button_elevator_3rd:   address = LIGHT_COMMAND3;	break;
        case button_elevator_4th:   address = LIGHT_COMMAND4;	break;
        case button_stop:           address = LIGHT_STOP;		break;
    }

    if (state == led_on) {
        io_set_bit(address);
    } else {
        io_clear_bit(address);
    }
}


void hw_clear_all_leds( void ) {
    hw_set_button_led_state(button_stop, led_off);
    hw_set_button_led_state(button_elevator_1st, led_off);
    hw_set_button_led_state(button_elevator_2nd, led_off);
    hw_set_button_led_state(button_elevator_3rd, led_off);
    hw_set_button_led_state(button_elevator_4th, led_off);
    hw_set_button_led_state(button_floor_up_1st, led_off);
    hw_set_button_led_state(button_floor_up_2nd, led_off);
    hw_set_button_led_state(button_floor_down_2nd, led_off);
    hw_set_button_led_state(button_floor_up_3rd, led_off);
    hw_set_button_led_state(button_floor_down_3rd, led_off);
    hw_set_button_led_state(button_floor_down_4th, led_off);
}


void hw_set_floor_indicator(Floor_led_t floor) {
    switch (floor) {
        case floor_led_1st:
            io_clear_bit(LIGHT_FLOOR_IND1);
            io_clear_bit(LIGHT_FLOOR_IND2);
            break;
        case floor_led_2nd:
            io_clear_bit(LIGHT_FLOOR_IND1);
            io_set_bit(LIGHT_FLOOR_IND2);
            break;
        case floor_led_3rd:
            io_set_bit(LIGHT_FLOOR_IND1);
            io_clear_bit(LIGHT_FLOOR_IND2);
            break;
        case floor_led_4th:
            io_set_bit(LIGHT_FLOOR_IND1);
            io_set_bit(LIGHT_FLOOR_IND2);
            break;
        default:
            io_clear_bit(LIGHT_FLOOR_IND1);
            io_clear_bit(LIGHT_FLOOR_IND2);
            break;
    }
}


void hw_set_door_state(Door_state_t state) {
    if (state == door_opened) {
        io_set_bit(LIGHT_DOOR_OPEN);
    } else {
        io_clear_bit(LIGHT_DOOR_OPEN);
    }
}


bool hw_is_button_pressed(Button_t button) {
    return io_read_bit(button);
}


Floor_sensor_t hw_get_sensors_state( void ) {
    if(io_read_bit(floor_sensor_1st)) {
        return floor_sensor_1st;
    } else if (io_read_bit(floor_sensor_2nd)) {
        return floor_sensor_2nd;
    } else if (io_read_bit(floor_sensor_3rd)) {
        return floor_sensor_3rd;
    } else if (io_read_bit(floor_sensor_4th)) {
        return floor_sensor_4th;
    }
    return floor_sensor_none;
}
