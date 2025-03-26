/* rp2350.ld - Linker Script for RP2350 */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x10000000, LENGTH = 16M    /* External Flash */
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 256K   /* SRAM0+SRAM1 */
}

SECTIONS
{
    . = ORIGIN(FLASH);
    .text : {
        KEEP(*(.vector_table))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    .data : {
        *(.data*)
        . = ALIGN(4);
    } > RAM AT > FLASH

    .bss : {
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
    } > RAM

    /DISCARD/ : {
        *(.ARM.attributes)
        *(.comment)
        *(.debug*)
    }
}

ENTRY(reset_handler)