MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2048k
    RAM(rwx) : ORIGIN = 0x20000000, LENGTH = 264k
}

SECTIONS
{
    /* O bootloader precisa que o código comece logo após os 256 bytes de checksum */
    .text : {
        *(.vectors)   /* Tabela de vetores de interrupção */
        *(.text*)     /* Código do programa */
        *(.rodata*)   /* Constantes (strings, etc) */
    } > FLASH

    /* Variáveis inicializadas (ex: int a = 10;) */
    .data : {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM AT > FLASH

    /* Variáveis não inicializadas (ex: int b;) */
    .bss : {
        _sbss = .;
        *(.bss*)
        _ebss = .;
    } > RAM
}
