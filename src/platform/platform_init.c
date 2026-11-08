#include "platform/platform_init.h"
#include "hardware/gpio.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "platform/board_config.h"
#include <stdio.h>

static void configure_run_pin(void) {
    gpio_init(PIN_TARGET_RUN);
    gpio_set_dir(PIN_TARGET_RUN, GPIO_OUT);
    gpio_put(PIN_TARGET_RUN, TARGET_IN_RESET);
}



static void configure_led(void) {
    gpio_init(PIN_PROBE_LED);
    gpio_set_dir(PIN_PROBE_LED, GPIO_OUT);
    gpio_put(PIN_PROBE_LED, LED_OFF);
}

// static void _configure_watchdog(void) { watchdog_enable(1000, true); }

bool platform_init(void) {

    stdio_init_all();

    configure_led();
    configure_run_pin();

    sleep_ms(10);

    return true;
}

void platform_led_on(void) { gpio_put(PIN_PROBE_LED, 1); }

void platform_led_off(void) { gpio_put(PIN_PROBE_LED, 0); }

void platform_led_blink(uint8_t n, uint32_t period_ms) {
    for (uint8_t i = 0; i < n; i++) {
        gpio_put(PIN_PROBE_LED, 1);
        sleep_ms(period_ms / 2);
        gpio_put(PIN_PROBE_LED, 0);
        sleep_ms(period_ms / 2);
    }
}