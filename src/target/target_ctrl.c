// Core 0: control of the target's active-low RUN pin and probe LED.
#include "target/target_ctrl.h"
#include "hardware/gpio.h"
#include "platform/board_config.h"

void target_ctrl_init(void) {
    // RUN is active low. Preload a safe HIGH output level before enabling it.
    gpio_init(PIN_TARGET_RUN);
    gpio_put(PIN_TARGET_RUN, TARGET_RUNNING);
    gpio_set_dir(PIN_TARGET_RUN, GPIO_OUT);

    gpio_init(PIN_PROBE_LED);
    gpio_put(PIN_PROBE_LED, LED_OFF);
    gpio_set_dir(PIN_PROBE_LED, GPIO_OUT);
}

void target_reset_low(void) {
    // Assert reset: hold the target RP2040's RUN pin LOW.
    gpio_put(PIN_TARGET_RUN, TARGET_IN_RESET);
    gpio_put(PIN_PROBE_LED, LED_OFF);
}

void target_reset_high(void) {
    // Release reset: allow the target to execute.
    gpio_put(PIN_TARGET_RUN, TARGET_RUNNING);
    gpio_put(PIN_PROBE_LED, LED_ON);
}
