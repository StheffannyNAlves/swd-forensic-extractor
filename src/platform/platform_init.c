#include "platform/platform_init.h"
#include "platform/board_config.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include <stdio.h>

/**
 * platform_init.c
 *
 * Implementação da inicialização da plataforma da sonda.
 *
 * Decisões de engenharia:
 *  - stdio_init_all() habilita UART e USB stdio do SDK; útil para
 *    debug inicial via minicom antes da CDC própria estar pronta.
 *  - O pino RUN do alvo é configurado como saída e colocado em LOW
 *    imediatamente, mantendo o alvo em reset desde o boot da sonda.
 *    Isso garante que o alvo nunca sai do reset sem autorização explícita
 *    da FSM — princípio de segurança por padrão.
 *  - Os pinos SWD são configurados aqui como entrada (hi-z) com
 *    pull-up habilitado. Eles ficam passivos até que a camada SWD
 *    (Fase 2) os assuma. Isso evita glitches indesejados no barramento
 *    enquanto o alvo está em reset.
 */


static void _configure_run_pin(void)
{
    gpio_init(PIN_TARGET_RUN);
    gpio_set_dir(PIN_TARGET_RUN, GPIO_OUT);
    gpio_put(PIN_TARGET_RUN, TARGET_IN_RESET);   /* alvo em reset desde o boot */
}

static void _configure_swd_pins_passive(void)
{
    /* SWCLK: saída, começa LOW */
    gpio_init(PIN_SWCLK);
    gpio_set_dir(PIN_SWCLK, GPIO_OUT);
    gpio_put(PIN_SWCLK, 0); 

    /* SWDIO: entrada com pull-up (hi-z seguro enquanto SWD inativo) */
    gpio_init(PIN_SWDIO);
    gpio_set_dir(PIN_SWDIO, GPIO_IN);
    gpio_pull_up(PIN_SWDIO);
}

static void _configure_led(void)
{
    gpio_init(PIN_PROBE_LED);
    gpio_set_dir(PIN_PROBE_LED, GPIO_OUT);
    gpio_put(PIN_PROBE_LED, LED_OFF);
}

static void _configure_watchdog(void){

}

// API publica
bool platform_init(void)
{

    stdio_init_all();

    
    _configure_led();
    _configure_run_pin();
    // _configure_swd_pins_passive();


    sleep_ms(10);

    return true;
}

void platform_led_on(void)
{
    gpio_put(PIN_PROBE_LED, 1);
}

void platform_led_off(void)
{
    gpio_put(PIN_PROBE_LED, 0);
}

void platform_led_blink(uint8_t n, uint32_t period_ms)
{
    for (uint8_t i = 0; i < n; i++) {
        gpio_put(PIN_PROBE_LED, 1);
        sleep_ms(period_ms / 2);
        gpio_put(PIN_PROBE_LED, 0);
        sleep_ms(period_ms / 2);
    }
}