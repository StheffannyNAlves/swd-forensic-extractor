#ifndef IPC_OPCODES_H
#define IPC_OPCODES_H

// Opcodes enviados pelo Core 0 (Comandos)
#define CMD_CORE1_ENTER_SWD   0x1111
#define CMD_CORE1_READ_IDCODE 0x2222
#define CMD_CORE1_PING 0x0001

// Respostas enviadas pelo Core 1 (Status)
#define RESP_CORE1_OK         0xAA00
#define RESP_CORE1_FAIL       0xFFFF
#define RESP_CORE1_PONG       0xBEEF

#endif // IPC_OPCODES_H