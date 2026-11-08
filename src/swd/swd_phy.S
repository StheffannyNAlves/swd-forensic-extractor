.syntax unified
    .cpu cortex-m0plus
    .thumb

    .set DOLOS_PIN_SWCLK, 2
    .set DOLOS_PIN_SWDIO, 3

    .global swd_phy_init
    .type swd_phy_init, %function
    .global readbit
    .type readbit, %function
    .global writebit
    .type writebit, %function
    .global turnaround_host_to_target
    .type turnaround_host_to_target, %function
    .global turnaround_target_to_host
    .type turnaround_target_to_host, %function
    .global line_reset
    .type line_reset, %function

    @ Constantes de Base para o M0+
    @ SIO_GPIO_IN       = 0x004 
    @ SIO_GPIO_OUT_SET  = 0x014 
    @ SIO_GPIO_OUT_CLR  = 0x018 
    @ SIO_GPIO_OE_SET   = 0x024 
    @ SIO_GPIO_OE_CLR   = 0x028 

swd_phy_init:
    PUSH {r4, lr}

    LDR r1, =0xD0000000
    MOVS r2, #(1 << DOLOS_PIN_SWCLK) | (1 << DOLOS_PIN_SWDIO)
    STR r2, [r1, #0x24]            @ SIO_GPIO_OE_SET (Ambos como saída)

    MOVS r2, #(1 << DOLOS_PIN_SWCLK)
    STR r2, [r1, #0x18]            @ SIO_GPIO_OUT_CLR (SWCLK em LOW)

    MOVS r2, #(1 << DOLOS_PIN_SWDIO)
    STR r2, [r1, #0x14]            @ SIO_GPIO_OUT_SET (SWDIO em HIGH)

    POP {r4, pc}
    .ltorg

writebit:
    PUSH {r4, r5, lr}              @ Registradores otimizados (sem r6, r7)

    LDR r1, =0xD0000000
    MOVS r2, #(1 << DOLOS_PIN_SWCLK)
    MOVS r3, #(1 << DOLOS_PIN_SWDIO)

    @ LOW half-cycle
    STR r2, [r1, #0x18]            @ SIO_GPIO_OUT_CLR (SWCLK LOW)

    @ Matemática Branchless da Sthe (Perfeita)
    LSLS r4, r0, #DOLOS_PIN_SWDIO
    SUBS r5, r3, r4

    @ Aplica a máscara no pino de Dados
    STR r4, [r1, #0x14]            @ SIO_GPIO_OUT_SET 
    STR r5, [r1, #0x18]            @ SIO_GPIO_OUT_CLR 

    @ Atraso (~4.8us)
    MOVS r4, #200
1:  SUBS r4, r4, #1
    BNE 1b

    @ HIGH half-cycle
    STR r2, [r1, #0x14]            @ SIO_GPIO_OUT_SET (SWCLK HIGH)

    @ Atraso (~4.8us)
    MOVS r4, #200
2:  SUBS r4, r4, #1
    BNE 2b

    @ Garante clock em LOW no final
    STR r2, [r1, #0x18]            @ SIO_GPIO_OUT_CLR

    POP {r4, r5, pc}
    .ltorg

readbit:
    PUSH {r4, lr}                  @ R5 e R7 pro lixo, economiza stack

    LDR r1, =0xD0000000
    MOVS r2, #(1 << DOLOS_PIN_SWCLK)
    MOVS r3, #(1 << DOLOS_PIN_SWDIO)

    @ LOW half-cycle
    STR r3, [r1, #0x28]            @ SIO_GPIO_OE_CLR (SWDIO vira INPUT)
    STR r2, [r1, #0x18]            @ SIO_GPIO_OUT_CLR (SWCLK LOW)

    MOVS r4, #200
1:  SUBS r4, r4, #1
    BNE 1b

    @ HIGH half-cycle
    STR r2, [r1, #0x14]            @ SIO_GPIO_OUT_SET (SWCLK HIGH)

    @ Atraso curto para estabilizar leitura
    MOVS r4, #100
2:  SUBS r4, r4, #1
    BNE 2b

    @ Captura do bit
    LDR r0, [r1, #0x004]           @ SIO_GPIO_IN (Lê toda a porta)
    LSRS r0, r0, #DOLOS_PIN_SWDIO  @ Joga o bit de dado pro LSB
    MOVS r4, #1
    ANDS r0, r0, r4                @ Isola o bit (0 ou 1)

    @ Derruba Clock
    STR r2, [r1, #0x18]            @ SIO_GPIO_OUT_CLR (SWCLK LOW)

    MOVS r4, #200
3:  SUBS r4, r4, #1
    BNE 3b

    POP {r4, pc}
    .ltorg

turnaround_host_to_target:
    PUSH {r4, r5, lr}
    LDR  r1, =0xD0000000
    MOVS r2, #(1 << DOLOS_PIN_SWDIO)
    MOVS r4, #(1 << DOLOS_PIN_SWCLK)

    STR  r2, [r1, #0x28]       @ OE_CLR — libera antes de clockar

    STR  r4, [r1, #0x14]       @ SWCLK HIGH
    MOVS r5, #200
1:  SUBS r5, r5, #1
    BNE  1b

    STR  r4, [r1, #0x18]       @ SWCLK LOW
    MOVS r5, #200
2:  SUBS r5, r5, #1
    BNE  2b

    POP  {r4, r5, pc}
    .ltorg

turnaround_target_to_host:
    PUSH {r4, r5, lr}
    LDR  r1, =0xD0000000
    MOVS r2, #(1 << DOLOS_PIN_SWDIO)
    MOVS r4, #(1 << DOLOS_PIN_SWCLK)

    STR  r4, [r1, #0x14]       @ SWCLK HIGH — SWDIO ainda input
    MOVS r5, #200
1:  SUBS r5, r5, #1
    BNE  1b

    STR  r4, [r1, #0x18]       @ SWCLK LOW
    MOVS r5, #200
2:  SUBS r5, r5, #1
    BNE  2b

    STR  r2, [r1, #0x24]       @ OE_SET — assume controle só depois

    POP  {r4, r5, pc}
    .ltorg

line_reset:
    PUSH {r4, r5, lr}

    MOVS r4, #64
1:  MOVS r0, #1
    BL   writebit
    SUBS r4, r4, #1
    BNE  1b

    MOVS r4, #8
2:  MOVS r0, #0
    BL   writebit
    SUBS r4, r4, #1
    BNE  2b

    POP  {r4, r5, pc}
    .ltorg

