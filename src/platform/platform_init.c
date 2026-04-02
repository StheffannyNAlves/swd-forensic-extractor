#include "plataform_init.h"
#include "board_config.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>


void platform_init(void){
    stdio_init_all();


    gpio_init(PIN_PROBE_LED);
    gpio_set_dir(PIN_TARGET_RUN, GPIO_OUT)




}