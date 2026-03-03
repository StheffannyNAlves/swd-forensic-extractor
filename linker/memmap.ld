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
    
     /* aplicação começa exatamente após o boot2 */
    .text 0x10000100 :
    {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    /* data vive em RAM, mas a imagem inicial está em FLASH */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT > FLASH

    _sidata = LOADADDR(.data);

    /* bss em RAM (zero em runtime) */
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

    /* boot2 exatamente onde deve está */
    ASSERT(ADDR(.boot2) == ORIGIN(FLASH), "boot2 VMA != 0x10000000");
    ASSERT(SIZEOF(.boot2) == 0x100,       "boot2 size != 256 bytes");

    /* text exatamente depois do boot2 */
    ASSERT(ADDR(.text)  == ORIGIN(FLASH) + 0x100, "text VMA != 0x10000100");
    ASSERT(ADDR(.text)  >= ADDR(.boot2) + SIZEOF(.boot2), "text overlaps boot2");

    /* sidata tem que estar em FLASH; sdata/edata em RAM */
    ASSERT(_sidata >= ORIGIN(FLASH) && _sidata < (ORIGIN(FLASH)+LENGTH(FLASH)),
           "_sidata not in FLASH");
    ASSERT(_sdata  >= ORIGIN(RAM)   && _sdata  < (ORIGIN(RAM)+LENGTH(RAM)),
           "_sdata not in RAM");
    ASSERT(_edata  >= ORIGIN(RAM)   && _edata  <= (ORIGIN(RAM)+LENGTH(RAM)),
           "_edata not in RAM");

    ASSERT(_sbss >= ORIGIN(RAM) && _ebss <= (ORIGIN(RAM)+LENGTH(RAM)) && _ebss >= _sbss,
           ".bss range invalid/outside RAM");
}

