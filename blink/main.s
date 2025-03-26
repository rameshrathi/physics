.cpu cortex-m0plus
.syntax unified
.thumb

.equ GPIO_BASE, 0x40014000
.equ SIO_BASE, 0xD0000000
.equ LED_PIN, 25

.global main

main:
    ldr r0, =SIO_BASE
    movs r1, #1
    lsls r1, r1, #LED_PIN  // LED pin bit

blink_loop:
    // LED ON
    str r1, [r0, #0x14]  // Set pin high
    
    // Delay
    bl delay_1s
    
    // LED OFF
    str r1, [r0, #0x18]  // Clear pin
    
    // Delay
    bl delay_1s
    
    b blink_loop

delay_1s:
    ldr r3, =1000000  // Delay cycles
delay_loop:
    subs r3, #1
    bne delay_loop
    bx lr

.end