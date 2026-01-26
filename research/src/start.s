.cpu cortex-m0plus
.thumb

/* O linker precisa saber onde começa */
.global _reset
.global vectors

/* Tabela de Vetores (Obrigatória para ARM) */
.section .vectors, "ax"
vectors:
    .word 0x20042000   /* Stack Pointer (Fim da RAM) */
    .word _reset       /* Reset Handler (Onde o código começa) */

/* Código de inicialização */
.section .text
.type _reset, %function
_reset:
    /* 1. Inicializa a Stack Pointer (garantia) */
    ldr r0, =0x20042000
    mov sp, r0

    /* 2. Copia .data da Flash para RAM (Simplificado: pulamos se não houver var global) */
    /* Para bare-metal simples, podemos pular direto pro main se não usarmos variaveis globais complexas */

    /* 3. Zera a seção .bss (Variáveis não inicializadas) */
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0
loop_bss:
    cmp r0, r1
    bge call_main
    str r2, [r0]
    adds r0, #4
    b loop_bss

call_main:
    /* 4. Chama o main.c */
    bl main
    
    /* Se o main retornar, trava num loop infinito */
hang:
    b hang

/* Definições vindas do Linker Script */
.word _sbss
.word _ebss
