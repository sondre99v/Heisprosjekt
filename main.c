#include "hardware.h"
#include "timer.h"
#include <stdio.h>


int main() {
    // Initialize hardware
    hw_init();

    printf("Press STOP button to stop elevator and exit program.\n");

    hw_set_motor_state(moving_up);

    while (1) {
        switch (hw_get_sensors_state()) {
            case floor_1st:
                hw_set_led_state(led_floor_1st, true);
                hw_set_motor_state(stopped);
                timer_reset();
                while(!timer_is_timed_out()) {}
                hw_set_motor_state(moving_up);
                timer_reset();
                while(!timer_is_timed_out()) {}
                break;
            case floor_2nd:
                hw_set_led_state(led_floor_2nd, true);
                break;
            case floor_3rd:
                hw_set_led_state(led_floor_3rd, true);
                break;
            case floor_4th:
                hw_set_led_state(led_floor_4th, true);
                hw_set_motor_state(moving_down);
                break;
        }

        // Stop elevator and exit program if the stop button is pressed
        if (hw_is_button_pressed(button_stop)) {
            hw_set_motor_state(stopped);
            break;
        }
    }

    return 0;
}
