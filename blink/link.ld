MEMORY
{
    FLASH : ORIGIN = 0x10000000, LENGTH = 2048K
    RAM : ORIGIN = 0x20000000, LENGTH = 256K
}

SECTIONS
{
    .text : 
    {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
    } > FLASH

    .data : 
    {
        _data_start = .;
        *(.data*)
        _data_end = .;
    } > RAM AT > FLASH

    .bss : 
    {
        _bss_start = .;
        *(.bss*)
        _bss_end = .;
    } > RAM
}