#include <stdio.h>
#include <stdbool.h>
#include "opcodes/ipc_opcodes.h"
#include "pico/multicore.h"
#include "swd/swd_proto.h"
#include "swd/swd_phy.h"
#include "app/app.h"
#include "opcodes/ipc_opcodes.h"
#include "pico/stdlib.h" 

void core1_entry(void){
    gpio_set_function(2, GPIO_FUNC_SIO);
    gpio_set_function(3, GPIO_FUNC_SIO);
    gpio_pull_up(3);
    swd_phy_init();

    while(1){
        uint32_t opcode = multicore_fifo_pop_blocking();

        switch (opcode)
        {
        case CMD_CORE1_ENTER_SWD:{
            swd_enter_swd_mode();

            multicore_fifo_push_blocking(RESP_CORE1_OK);
            break;
        }
        case CMD_CORE1_PING:{
             multicore_fifo_push_blocking(RESP_CORE1_PONG);
             break;
        }
        case CMD_CORE1_READ_IDCODE: {
            uint32_t idcode = swd_read_idcode();
            multicore_fifo_push_blocking(idcode);
            break;
        }

        default:
           multicore_fifo_push_blocking(RESP_CORE1_FAIL);
           break;
        }
    }
}