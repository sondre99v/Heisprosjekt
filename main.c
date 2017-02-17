//Author: Finn Matras & Sondre Ninive Andersen

#include "hardware.h"
#include "timer.h"
#include "event_generator.h"
#include <stdio.h>
#include <assert.h>


int main() {
    // Initialize hardware
    hw_init();
    timer_init();

    printf("Starting event generator.\n");

    // Start generating events
    event_generator();

    // Event generator shoul never return
    assert (false);
}
