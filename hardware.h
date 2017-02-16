//Author: Finn Matras & Sondre Ninive Andersen

#ifndef __INCLUDE_HARDWARE_H__
#define __INCLUDE_HARDWARE_H__

#include <stdbool.h>

//#define SENSOR_FLOOR1       (0x200+4)
//#define SENSOR_FLOOR2       (0x200+5)
//#define SENSOR_FLOOR3       (0x200+6)
//#define SENSOR_FLOOR4       (0x200+7)

//Addresses for motor direction and speed
#define MOTORDIR            (0x300+15)
#define MOTOR               (0x100+0)

//Addresses for controlling the floor indicator lights
#define LIGHT_FLOOR_IND2    (0x300+1)
#define LIGHT_FLOOR_IND1    (0x300+0)

//Door states for door control
typedef enum {
    door_closed,
    door_opened
}   Door_state_t;

//Description of elevator (motor) movement
typedef enum {
    moving_down = -1,
    stopped = 0,
    moving_up  = 1
}   Motor_state_t;

//Addresses for LED's, exception: led_floor_xxx, see on #define's
typedef enum {
    led_stop        = (0x300+14),
    led_dropoff_1st = (0x300+13),
    led_dropoff_2nd = (0x300+12),
    led_dropoff_3rd = (0x300+11),
    led_dropoff_4th = (0x300+10),
    led_up_1st      = (0x300+9),
    led_up_2nd      = (0x300+8),
    led_down_2nd    = (0x300+7),
    led_up_3rd      = (0x300+6),
    led_down_3rd    = (0x300+5),
    led_down_4th    = (0x300+4),
    led_door        = (0x300+3),
    led_floor_1st,
    led_floor_2nd,
    led_floor_3rd,
    led_floor_4th
}   Led_t;

//Addresses to all buttons used to control the elevator
typedef enum {
    button_up_1st       = (0x300+17),
    button_down_2nd     = (0x200+0),
    button_up_2nd       = (0x300+16),
    button_down_3rd     = (0x200+2),
    button_up_3rd       = (0x200+1),
    button_down_4th     = (0x200+3),
    button_stop         = (0x300+22),
    button_floor_1st    = (0x300+21),
    button_floor_2nd    = (0x300+20),
    button_floor_3rd    = (0x300+19),
    button_floor_4th    = (0x300+18)
}   Button_t;


//Addresses to locations of floor sensors
typedef enum {
    floor_sensor_none = -1,
    floor_sensor_1st = (0x200+4),
    floor_sensor_2nd = (0x200+5),
    floor_sensor_3rd = (0x200+6),
    floor_sensor_4th = (0x200+7)
}   Floor_sensor_t;



//Initializes the hardware and sets all LED's and motor to off
void hw_init();

//Sets motor state based on state enum
void hw_set_motor_state(Motor_state_t state);

//Controls all LED's and sets Led_t led to bool state (false = off, true = on)
//Maybe it should be on/off instead of false/true for abastraction level?????
void hw_set_led_state(Led_t led, bool state);

//Sets door state according to coor state
void hw_set_door_state(Door_state_t state);

//Returns true if Button_t button is pressed
bool hw_is_button_pressed(Button_t button);

//Returns the floor that the elevator is at, og floor_none if the elevator is in between two floors
Floor_sensor_t hw_get_sensors_state(void);



#endif // #ifndef __INCLUDE_HARDWARE_H__
