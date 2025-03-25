MEMORY {
    FLASH (rx) : ORIGIN = 0x10000000, LENGTH = 2048K  /* Code and read-only data */
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 264K   /* Variables and stack */
}

SECTIONS {
    .text : {
        *(.vectors)           /* Vector table */
        *(.text*)             /* Code */
        *(.rodata*)           /* Read-only data */
    } > FLASH

    .data : {
        _sdata = .;           /* Start of data in RAM */
        *(.data*)             /* Initialized data */
        _edata = .;           /* End of data in RAM */
    } > RAM AT > FLASH
    _sidata = LOADADDR(.data);  /* Load address of data in flash */

    .bss : {
        _sbss = .;            /* Start of BSS */
        *(.bss*)              /* Uninitialized data */
        *(COMMON)
        _ebss = .;            /* End of BSS */
    } > RAM

    _estack = ORIGIN(RAM) + LENGTH(RAM);  /* Stack pointer at end of RAM */
}