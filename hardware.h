#include <stdbool.h>


// Wrapper for libComedi Elevator control.
// These functions provides an interface to the elevators in the real time lab
//
// 2007, Martin Korsgaard
#ifndef __INCLUDE_DRIVER_H__
#define __INCLUDE_DRIVER_H__


#define SENSOR_FLOOR1       (0x200+4)
#define SENSOR_FLOOR2       (0x200+5)
#define SENSOR_FLOOR3       (0x200+6)
#define SENSOR_FLOOR4       (0x200+7)
#define MOTORDIR            (0x300+15)
#define MOTOR               (0x100+0)

typedef enum {
    door_closed,
    door_opened
}   Door_state_t;

typedef enum {
    moving_down = -1,
    stopped = 0,
    moving_up  = 1
}   Motor_state_t;

typedef enum {
    hit_1st,
    hit_2nd,
    hit_3rd,
    hit_4th,
}   Sensor_floor_t;

typedef enum {
    led_hit_1st     = (0x300+13),
    led_hit_2nd     = (0x300+12),
    led_hit_3rd     = (0x300+11),
    led_hit_4th     = (0x300+10),
    led_up_1st      = (0x300+9),
    led_down_2nd    = (0x300+7),
    led_up_2nd      = (0x300+8),
    led_down_3rd    = (0x300+5),
    led_up_3rd      = (0x300+6),
    led_down_4th    = (0x300+4),
    led_stop        = (0x300+14),
    led_door        = (0x300+3)
}   Led_button_t;

typedef enum {
    button_up_1st       = (0x300+17),
    button_down_2nd     = (0x200+0),
    button_up_2nd       = (0x300+16),
    button_down_3rd     = (0x200+2),
    button_up_3rd       = (0x200+1),
    button_down_4th     = (0x200+3),
    button_stop         =(0x300+22),
    button_floor_1st    =(0x300+21),
    button_floor_2nd    =(0x300+20),
    button_floor_3rd    =(0x300+19),
    button_floor_4th    =(0x300+18)
}   Button_t;

typedef enum {
    floor_none,
    floor_1st,
    floor_2nd,
    floor_3rd,
    floor_4th
}   Floor_t;


/*
// Number of floors
#define N_FLOORS 4

*/

/**
  Initialize elevator.
  @return Non-zero on success, 0 on failure.
*/
//int elev_init(void);

/**
  Motor direction for function elev_set_motor_direction().
*/

/**
  Sets the motor direction of the elevator.
  @param dirn New direction of the elevator.
*/
//void elev_set_motor_direction(elev_motor_direction_t dirn);

/**
  Turn door-open lamp on or off.
  @param value Non-zero value turns lamp on, 0 turns lamp off.
*/
//void elev_set_door_open_lamp(int value);


/**
  Get signal from obstruction switch.
  @return 1 if obstruction is enabled. 0 if not.
*/
//int elev_get_obstruction_signal(void);



/**
  Get signal from stop button.
  @return 1 if stop button is pushed, 0 if not.
*/
//int elev_get_stop_signal(void);


/**
  Turn stop lamp on or off.
  @param value Non-zero value turns lamp on, 0 turns lamp off.
*/
//void elev_set_stop_lamp(int value);



/**
  Get floor sensor signal.
  @return -1 if elevator is not on a floor. 0-3 if elevator is on floor. 0 is
    ground floor, 3 is top floor.
*/
//int elev_get_floor_sensor_signal(void);



/**
  Set floor indicator lamp for a given floor.
  @param floor Which floor lamp to turn on. Other floor lamps are turned off.
*/
//void elev_set_floor_indicator(int floor);



/**
  Button types for function elev_set_button_lamp() and elev_get_button().
*/
/*
typedef enum tag_elev_lamp_type { 
    BUTTON_CALL_UP = 0,
    BUTTON_CALL_DOWN = 1,
    BUTTON_COMMAND = 2
} elev_button_type_t;
*/


/**
  Gets a button signal.
  @param button Which button type to check. Can be BUTTON_CALL_UP,
    BUTTON_CALL_DOWN or BUTTON_COMMAND (button "inside the elevator).
  @param floor Which floor to check button. Must be 0-3.
  @return 0 if button is not pushed. 1 if button is pushed.
*/
//int elev_get_button_signal(elev_button_type_t button, int floor);



/**
  Set a button lamp.
  @param lamp Which type of lamp to set. Can be BUTTON_CALL_UP,
    BUTTON_CALL_DOWN or BUTTON_COMMAND (button "inside" the elevator).
  @param floor Floor of lamp to set. Must be 0-3
  @param value Non-zero value turns lamp on, 0 turns lamp off.
*/
//void elev_set_button_lamp(elev_button_type_t button, int floor, int value);




//Our own functions

void hw_set_motor_state(Motor_state_t state);

void hw_set_led_state(Led_button_t led, bool state);

void hw_set_door_state(Door_state_t state);

bool hw_is_button_pressed(Button_t button);

Floor_t hw_get_sensors_state(void);



#endif // #ifndef __INCLUDE_DRIVER_H__
