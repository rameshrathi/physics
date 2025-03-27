/* Linker script for RP2350 (ARM Cortex-M33) */
MEMORY
{
    FLASH (rx)     : ORIGIN = 0x10000000, LENGTH = 16M    /* External Flash */
    SRAM0_3 (rwx)  : ORIGIN = 0x20000000, LENGTH = 256K   /* Striped SRAM0-3 */
    SRAM4_7 (rwx)  : ORIGIN = 0x20040000, LENGTH = 256K   /* Striped SRAM4-7 */
    SRAM8 (rwx)    : ORIGIN = 0x20080000, LENGTH = 4K     /* Non-striped SRAM8 */
    SRAM9 (rwx)    : ORIGIN = 0x20081000, LENGTH = 4K     /* Non-striped SRAM9 */
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
    } > SRAM0_3 AT > FLASH

    .bss : {
        *(.bss*)           /* Zero-initialized data (e.g., task_stacks) */
        *(COMMON)
    } > SRAM0_3

    /* Task stacks in SRAM0-3 (striped for parallel access) */
    .task_stacks : {
        *(.task_stacks)    /* Dedicated section for task stacks */
    } > SRAM0_3

    /* Main stack in SRAM9 (non-striped for processor stack) */
    .stack : {
        . = ALIGN(8);
        _stack_start = .;
        . = . + 4K;        /* Reserve 4KB for main stack */
        _stack_end = .;
    } > SRAM9
}