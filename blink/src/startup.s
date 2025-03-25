.section .vectors, "a"
.align 2
.global _vectors
_vectors:
    .word _estack            // Initial stack pointer (top of RAM)
    .word Reset_Handler      // Reset handler
    .word NMI_Handler        // Non-Maskable Interrupt handler
    .word HardFault_Handler  // Hard Fault handler
    .word 0                  // Reserved
    .word 0                  // Reserved
    .word 0                  // Reserved
    .word 0                  // Reserved
    .word 0                  // Reserved
    .word 0                  // Reserved
    .word 0                  // Reserved
    .word SysTick_Handler    // SysTick handler

.section .text
.global Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    // Copy .data section from flash to RAM
    ldr r0, =_sidata        // Source address (in flash)
    ldr r1, =_sdata         // Destination address (in RAM)
    ldr r2, =_edata         // End of data section
    b copy_data

copy_data_loop:
    ldr r3, [r0]            // Load word from flash
    add r0, r0, #4          // Increment source pointer
    str r3, [r1]            // Store word to RAM
    add r1, r1, #4          // Increment destination pointer
copy_data:
    cmp r1, r2              // Compare current position with end
    bcc copy_data_loop      // Continue if not at end

    // Zero out .bss section
    ldr r0, =_sbss          // Start of BSS
    ldr r1, =_ebss          // End of BSS
    mov r2, #0              // Value to write (zero)
    b zero_bss

zero_bss_loop:
    str r2, [r0]            // Write zero
    add r0, r0, #4          // Increment pointer
zero_bss:
    cmp r0, r1              // Compare current position with end
    bcc zero_bss_loop       // Continue if not at end

    // Call main function
    bl main
    b .                     // Infinite loop if main returns

.weak NMI_Handler
.type NMI_Handler, %function
NMI_Handler:
    b .                     // Infinite loop for unhandled NMI

.weak HardFault_Handler
.type HardFault_Handler, %function
HardFault_Handler:
    b .                     // Infinite loop for unhandled Hard Fault

.global SysTick_Handler
.type SysTick_Handler, %function
SysTick_Handler:
    bl systick_handler      // Call C handler for SysTick
    bx lr                   // Return from interrupt
;// Ensure proper comment termination
