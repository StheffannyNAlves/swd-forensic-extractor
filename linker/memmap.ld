ENTRY(_reset)

MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2048k
    RAM(rwx)  : ORIGIN = 0x20000000, LENGTH = 264k
}

SECTIONS
{
    .boot2 0x10000000 :
    {
        KEEP(*(.boot2))
        . = 0x100
    } > FLASH
    
    .text 0x10000100 :
    {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    _sidata = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    _stack_top = ORIGIN(RAM) + LENGTH(RAM);
}

