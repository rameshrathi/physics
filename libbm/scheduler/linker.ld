/* Linker script for RP2350 (ARM Cortex-M33) */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x10000000, LENGTH = 16M   /* External Flash */
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 32K   /* SRAM */
}

SECTIONS
{
    .vectors : {
        KEEP(*(.vectors))  /* Vector table at start of FLASH */
    } > FLASH

    .text : {
        *(.text*)          /* Code */
        *(.rodata*)        /* Read-only data */
    } > FLASH

    .data : {
        *(.data*)          /* Initialized data */
    } > RAM AT > FLASH

    .bss : {
        *(.bss*)           /* Zero-initialized data */
        *(COMMON)
    } > RAM

    /* Stack at the end of RAM */
    _stack_start = ORIGIN(RAM) + LENGTH(RAM);
}