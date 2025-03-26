.cpu cortex-m0plus
.syntax unified
.thumb

.global _start
.global reset_handler

.section .vectors, "a"
vector_table:
    .word _stack_top
    .word reset_handler

reset_handler:
    // Initialize system clock
    bl system_init

    // Initialize GPIO
    bl gpio_init

    // Jump to main application
    bl main

    // Halt if main returns
    b .

system_init:
    // Add any system clock or core initialization
    bx lr

gpio_init:
    // Global GPIO configuration
    ldr r0, =GPIO_BASE
    ldr r1, =SIO_BASE
    
    // Configure LED pin
    movs r2, #5  // SIO function
    str r2, [r0, #0xCC]  // GPIO control register
    
    // Set as output
    movs r2, #1
    lsls r2, r2, #25  // Assuming GPIO25
    str r2, [r1, #0x24]  // GPIO output enable
    
    bx lr