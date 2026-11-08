#include "app/app.h"
#include "hardware/gpio.h"
#include "platform/board_config.h"
#include "swd/swd_phy.h"
#include "swd/swd_proto.h"
#include "transport/usb_transport.h"

#include <stdio.h>
#include <string.h>

dolos_fault_t app_enter_swd(void) {
    swd_phy_init(); 
    swd_enter_swd_mode();

    return (dolos_fault_t){.code = ERR_NONE, .is_fatal = false};
}

dolos_fault_t app_read_idcode(uint32_t *idcode) {

    uint32_t read_idcode = swd_read_idcode();

    if (read_idcode == 0xFFFFFFFF || read_idcode != 0x0BC12477) {
        transport_send_event((const uint8_t *)"S004\r\n", 6);
        return (dolos_fault_t){.code = S004, .is_fatal = true};
    }

    *idcode = read_idcode;

    char idcode_msg[32];
    snprintf(idcode_msg, sizeof(idcode_msg), "IDCODE:%08lX\r\n", read_idcode);
    transport_send_event((const uint8_t *)idcode_msg, strlen(idcode_msg));

    return (dolos_fault_t){.code = ERR_NONE, .is_fatal = false};
}