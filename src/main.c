#include "include/platform/platform_init.h"
#include "include/platform/target_ctrl.h"
#include "include/app/probe_state.h"
#include "include/trasnsport/usb_transport.h"
#include "include/app/app.h"

/**
 * Entry point - Fase 1 boot:
 * 1. probe_state_init() → ST_BOOT
 * 2. platform_init() → clocks, GPIOs, reset
 * 3. probe_state_advance() → ST_PLATFORM_INIT
 * 4. target_ctrl_init()
 * 5. usb_transport_init() → ST_IDLE
 * 6. app_run() → loop principal
 *
 * Invariante: alvo em reset até ST_IDLE
 */
int main(void)
{
    probe_state_init();

    platform_init();
    probe_state_advance();   /* → ST_PLATFORM_INIT */
    target_ctrl_init();
    usb_transport_init();    /* → ST_IDLE */
    app_run();               /* loop */
    return 0;                /* unreachable */
}