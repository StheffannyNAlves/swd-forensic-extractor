#include "app/app.h"
#include "bsp/board.h"
#include "core0/core0_tasks.h"
#include "core1/core1_tasks.h"
#include "display/lcd.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "platform/platform_init.h"
#include "swd/swd_phy.h"
#include "swd/swd_proto.h"
#include "target/target_ctrl.h"
#include "transport/usb_transport.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h> // Added for debugging

int main() {
    gpio_set_function(2, GPIO_FUNC_SIO);
    gpio_set_function(3, GPIO_FUNC_SIO);
    gpio_pull_up(3);
    multicore_launch_core1(core1_entry);
    while (1) {
        fsm_run(); // core 0
        transport_usb_task();
        // if (current_state != ERROR){
        //   watchdog_update();
        // }
    }

    return 0;
}