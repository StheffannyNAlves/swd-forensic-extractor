.cpu cortex-m0plus
.thumb
.syntax unified
.global _reset
.global vectors


.section .vectors, "ax"
.align 2
vectors:
    .word _stack_top
    .word _reset  

/* Código de inicialização */
.section .text
.type _reset, %function
_reset:
    ldr r0, =_stack_top
    mov sp, r0

    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    
loop_copy_data:
    cmp r0, r1
    bge init_bss
    ldr r3, [r2]
    str r3, [r0]
    adds r0, #4
    adds r2, #4
    b loop_copy_data

init_bss:
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
    bl main
    
hang:
    b hang

.word _sbss
.word _ebss
