#include "include/platform/platform_init.h"
#include "include/platform/target_ctrl.h"
#include "include/app/probe_state.h"
#include "include/trasnsport/usb_transport.h"
#include "include/app/app.h"

/**
 * main.c
 *
 * Entry point da sonda forense P1.
 *
 * Sequência de boot (Fase 1):
 *
 *   1. probe_state_init()      — FSM em ST_BOOT
 *   2. platform_init()         — clocks, GPIOs, alvo em reset
 *   3. probe_state_advance()   — ST_BOOT → ST_PLATFORM_INIT
 *   4. target_ctrl_init()      — registra estado IN_RESET
 *   5. usb_transport_init()    — TinyUSB + Core 1 + aguarda mount
 *                                (internamente avança FSM → ST_IDLE)
 *   6. app_run()               — loop principal, não retorna
 *
 * Invariante de segurança:
 *   O alvo permanece em reset durante todo o boot da sonda.
 *   Nenhuma operação no alvo ocorre antes de ST_IDLE.
 */
int main(void)
{
    probe_state_init();

    platform_init();
    probe_state_advance();   /* ST_BOOT → ST_PLATFORM_INIT */

    target_ctrl_init();

    usb_transport_init();    /* internamente: ST_PLATFORM_INIT → ST_IDLE */

    app_run();               /* não retorna */

    return 0;   /* nunca atingido */
}