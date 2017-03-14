#ifndef __INCLUDE_HARDWARE_H__
#define __INCLUDE_HARDWARE_H__

#include <stdbool.h>
#include "channels.h"

typedef enum {
    led_off = false,
    led_on = true
} Led_state_t;

typedef enum {
    door_closed = false,
    door_opened = true
} Door_state_t;

typedef enum {
    motor_moving_down = -1,
    motor_stopped = 0,
    motor_moving_up  = 1
} Motor_state_t;

typedef enum {
    floor_led_1st,
    floor_led_2nd,
    floor_led_3rd,
    floor_led_4th
} Floor_led_t;


//Addresses to all buttons used to control the elevator
typedef enum {
    button_floor_up_1st     = BUTTON_UP1,
    button_floor_down_2nd   = BUTTON_DOWN2,
    button_floor_up_2nd     = BUTTON_UP2,
    button_floor_down_3rd   = BUTTON_DOWN3,
    button_floor_up_3rd     = BUTTON_UP3,
    button_floor_down_4th   = BUTTON_DOWN4,
    button_elevator_1st     = BUTTON_COMMAND1,
    button_elevator_2nd     = BUTTON_COMMAND2,
    button_elevator_3rd     = BUTTON_COMMAND3,
    button_elevator_4th     = BUTTON_COMMAND4,
    button_stop             = STOP
} Button_t;


//Addresses to locations of floor sensors
typedef enum {
    floor_sensor_none = -1,
    floor_sensor_1st = SENSOR_FLOOR1,
    floor_sensor_2nd = SENSOR_FLOOR2,
    floor_sensor_3rd = SENSOR_FLOOR3,
    floor_sensor_4th = SENSOR_FLOOR4
} Floor_sensor_t;



//Initializes the hardware and sets all LED's and motor to off
void hw_init( void );

void hw_set_motor_state(Motor_state_t state);

void hw_set_button_led_state(Button_t button, Led_state_t state);

void hw_clear_all_leds( void );

void hw_set_floor_indicator(Floor_led_t floor);

void hw_set_door_state(Door_state_t state);

bool hw_is_button_pressed(Button_t button);

Floor_sensor_t hw_get_sensors_state( void );


#endif // #ifndef __INCLUDE_HARDWARE_H__