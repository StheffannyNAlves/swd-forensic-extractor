#define SWD_PROTO_C
#include "swd/swd_proto.h"
#include "swd/swd_phy.h"
#include "transport/usb_transport.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hardware/structs/sio.h"

void swd_enter_swd_mode(void) {
    swd_wake_dormant();

    line_reset();

    // Transição JTAG -> SWD (0xE79E LSB-first)  
    // uint16_t sequence = JTAG_TO_SWD_SEQ;
    // for (int i = 0; i < 16; i++) {
    //     uint8_t bit = (sequence >> i) & 1;
    //     writebit(bit);
    // }

    // // 3. Line Reset Mandatório pós-chave
    // line_reset();

    // writebit(0); // Idle cycle 1
    // writebit(0); // Idle cycle 2

    swd_write_targetsel(CORE0_INSTANCE_ID);

    writebit(0); // Idle cycle antes da primeira transição
    writebit(0);
}

static uint8_t calculate_parity(uint32_t apndp, uint8_t rnw, uint8_t a2, uint8_t a3) {
    uint8_t count = (apndp & 1) + (rnw & 1) + (a2 & 1) + (a3 & 1);
    return (count % 2) != 0 ? 1 : 0; // Return 1 for odd parity, 0 for even parity
}

static void send_request(uint8_t apndp, uint8_t rnw, uint8_t a2, uint8_t a3) {
    uint8_t request_packet = 0;
    uint8_t parity = calculate_parity(apndp, rnw, a2, a3);

    request_packet |= (1 << 0);      // Start
    request_packet |= (apndp << 1);  // APnDP
    request_packet |= (rnw << 2);    // RnW
    request_packet |= (a2 << 3);     // A2
    request_packet |= (a3 << 4);     // A3
    request_packet |= (parity << 5); // Parity
    request_packet |= (0 << 6);      // Stop
    request_packet |= (1 << 7);      // Park

    for (int i = 0; i < 8; i++) {
        writebit((request_packet >> i) & 1);
    }
    
}

static uint32_t read_data(void) {
    uint32_t data = 0;
    for (int i = 0; i < 32; i++) {
        data |= (readbit() << i);
    }
    return data;
}

static void write_data(uint32_t data) {
    for (int i = 0; i < 32; i++) {
        writebit((data >> i) & 1);
    }
}

static uint8_t calculate_data_parity(uint32_t data) {
    uint8_t parity = 0;
    for (int i = 0; i < 32; i++) {
        parity ^= (data >> i) & 1;
    }
    return parity;
}


uint32_t swd_read(uint8_t apndp, uint8_t addr, uint8_t rnw) {
    send_request(apndp, rnw, (addr >> 2) & 1, (addr >> 3) & 1); // Envio do header

    turnaround_host_to_target();

    // Lê OE direto do registrador
    uint32_t oe = *(volatile uint32_t *)(0xD0000000 + 0x020);
    char oe_msg[32];
    snprintf(oe_msg, sizeof(oe_msg), "OE=0x%08lX\r\n", oe);
    transport_send_event((uint8_t*)oe_msg, strlen(oe_msg));

    uint8_t ack = 0;
    for (int i = 0; i < 3; i++) {
        ack |= (readbit() << i);
    }
    char ack_msg[32];
    snprintf(ack_msg, sizeof(ack_msg), "ACK=%u\r\n", ack);
    transport_send_event((uint8_t *)ack_msg, strlen(ack_msg));
    
    if (ack == 0b001) { // ACK OK
        uint32_t data = read_data();
        uint8_t parity = readbit();

        turnaround_target_to_host();

        if (parity == calculate_data_parity(data)) {
            //printf("C000\n");
            //printf("IDCODE: 0x%08X\n", (unsigned int)data);
            return data; // Return the read data
        } else {
            //printf("S003\n");  // Parity error
            return 0xFFFFFFFF; // Indicate error with a special value
        }
    } else {
        turnaround_target_to_host(); // Ensure we return to host mode even on ACK error
        //printf("S004\n");            // ACK error
        return 0xFFFFFFFF;           // Indicate error with a special value
    }
}

uint32_t swd_read_idcode(void) {
    return swd_read(0, 0, 1); // APnDP = 0 (DP), addr = 0 (IDCODE), RnW = 1 (read)
}

uint8_t swd_write(uint8_t apndp, uint8_t a2, uint8_t a3, uint32_t data_to_write) {
    send_request(apndp, 0, a2, a3);
    turnaround_host_to_target();
    uint8_t ack = 0;
    for (int i = 0; i < 3; i++) {
        ack |= (readbit() << i);
    }

    if (ack == 0b001) {
        turnaround_target_to_host();

        write_data(data_to_write);
        writebit(calculate_data_parity(data_to_write));

        return ack;
    }
    turnaround_target_to_host();
    return ack; // aborta em caso de WAIT ou FAULT
}

 


void swd_write_targetsel(uint32_t target_id) {
    // Envia Header (DP, Write, Endereço 0x0C)
    send_request(0, 0, 1, 1); // 8 bits

    //  Dummy Turnaround (Exatamente 1 ciclo)
    // O Host solta a linha (passa o pino para INPUT) e dá um clock.
    turnaround_host_to_target();

    // 3 clocks de ACK, lê os valores mas joga fora. Preciso disso aqui para gerar os clocks
    readbit();
    readbit();
    readbit();

    turnaround_target_to_host(); // Reassume SWDIO e SWCLK

    write_data(target_id);
    writebit(calculate_data_parity(target_id));
}
/**
 * @brief Tira o alvo do estado Dormant e força para o modo SWD.
 * Sequência corrigida e cravada nos 4 bits de ativação do padrão ARM CoreSight.
 */
void swd_wake_dormant(void) {
    // Aborta transições pendentes (Mínimo de 8 ciclos em LOW)
    for (int i = 0; i < 8; i++) {
        writebit(0);
    }

    // Selection Alert Sequence (128 bits - Chave de Hardware)
    // Transmitido LSB-first.
    static const uint8_t alert_seq[16] = {0x92, 0xf3, 0x09, 0x62, 0x95, 0x2d, 0x85, 0x86,
                                          0xe9, 0xaf, 0xdd, 0xe3, 0xa2, 0x0e, 0xbc, 0x19};

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            writebit((alert_seq[i] >> j) & 1);
        }
    }

    // Break do Protocolo (4 ciclos de clock com a linha em LOW)
    for (int i = 0; i < 4; i++) {
        writebit(0);
    }

    // Activation Code para SWD (Valor 0x1A em exatamente 8 bits, LSB-first)
    // Vai cuspir sequencialmente na linha: 0,1,0,1,1,0,0,0  
    for (int i = 0; i < 8; i++) {
        writebit((0x1A >> i) & 1);
    }
}


