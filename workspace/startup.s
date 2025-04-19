@ startup.s - Startup Code for RP2350
.syntax unified
.cpu cortex-m33
.thumb

.section .vector_table, "a"
.align 2
.global _vectors
_vectors:
    .word 0x20041fff         @ Stack pointer (end of SRAM1, 256KB + 4KB)
    .word reset_handler      @ Reset vector
    .word nmi_handler        @ NMI handler
    .word hardfault_handler  @ Hard Fault handler
    .word 0                  @ Placeholder for unused vectors
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word systick_handler    @ SysTick handler (offset 15)

.section .text
.global reset_handler
.thumb_func
reset_handler:
    ldr r0, =0x20041fff
    mov sp, r0
    bl main
hang:
    b hang

.thumb_func
nmi_handler:
    b hang

.thumb_func
hardfault_handler:
    b hang

.thumb_func
systick_handler:
    bx lr

@ Ensure a newline follows this line
