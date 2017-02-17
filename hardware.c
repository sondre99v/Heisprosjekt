//Author: Finn Matras & Sondre Ninive Andersen

#include "hardware.h"
#include "io.h"


void hw_init(){
    //Starts the IO
    io_init();

    //Sets all LED's to off
    hw_clear_all_leds();

    //Turns motor off
    hw_set_motor_state(stopped);
}


void hw_set_motor_state(Motor_state_t state){
    int motor_speed = 2800;

    switch (state) {
        case moving_down:
            io_set_bit(MOTORDIR);
            io_write_analog(MOTOR, motor_speed);
            break;
        case stopped:
            io_write_analog(MOTOR,0);
            break;
        case moving_up:
            io_clear_bit(MOTORDIR);
            io_write_analog(MOTOR, motor_speed);
    }
    return;
}

void hw_set_led_state(Led_t led, bool state) {
    if (state){
        io_set_bit(led);
    } else{
        io_clear_bit(led);
    }
}


void hw_clear_all_leds(void){
    hw_set_led_state(led_stop,false);
    hw_set_led_state(led_dropoff_1st,false);
    hw_set_led_state(led_dropoff_2nd,false);
    hw_set_led_state(led_dropoff_3rd,false);
    hw_set_led_state(led_dropoff_4th,false);
    hw_set_led_state(led_up_1st,false);
    hw_set_led_state(led_up_2nd,false);
    hw_set_led_state(led_down_2nd,false);
    hw_set_led_state(led_up_3rd,false);
    hw_set_led_state(led_down_3rd,false);
    hw_set_led_state(led_down_4th,false);
    hw_set_led_state(led_door,false);
    hw_set_floor_indicator(floor_led_1st); //This has to be implemented correclty.
}


void hw_set_floor_indicator(Floor_led_t floor){
    switch (floor){
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

void hw_set_door_state(Door_state_t state){
    hw_set_led_state(led_door,state);
}



bool hw_is_button_pressed(Button_t button){
    return io_read_bit(button);
}


Floor_sensor_t hw_get_sensors_state(void){
    if(io_read_bit(floor_sensor_1st)){
        return floor_sensor_1st;
    } else if (io_read_bit(floor_sensor_2nd)){
        return floor_sensor_2nd;
    } else if (io_read_bit(floor_sensor_3rd)){
        return floor_sensor_3rd;
    } else if (io_read_bit(floor_sensor_4th)){
        return floor_sensor_4th;
    }
    return floor_sensor_none;
}