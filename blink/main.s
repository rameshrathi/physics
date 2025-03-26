@ main.s - LED Blink Logic for RP2350 (GPIO25)
.syntax unified
.cpu cortex-m33
.thumb

.section .text
.global main
.thumb_func
main:
    @ Enable clock for GPIO (SIO and IO_BANK0) via RESETS register
    ldr r0, =0x4000f000      @ RESETS base address
    mov r1, #0x54            @ Bits for IO_BANK0 and SIO
    ldr r2, =0x2000          @ RESETS_RESET_CLR offset
    add r2, r0               @ r2 = RESETS_RESET_CLR address
    str r1, [r2]             @ Clear reset for IO_BANK0 and SIO
wait_reset:
    ldr r2, =0x3000          @ RESETS_RESET_DONE offset
    add r2, r0               @ r2 = RESETS_RESET_DONE address
    ldr r1, [r2]             @ Read reset done status
    tst r1, #0x54            @ Check if IO_BANK0 and SIO are reset done
    beq wait_reset

    @ Configure GPIO25 as output (SIO function)
    ldr r0, =0xd0000000      @ SIO base address
    mov r1, #1
    lsl r1, r1, #25          @ Set bit 25 for GPIO25
    str r1, [r0, #0x004]     @ GPIO_OE_SET (enable output)

    @ Set GPIO25 to SIO function (function 5)
    ldr r0, =0x40014000      @ IO_BANK0 base address
    mov r1, #5               @ Function 5 (SIO)
    str r1, [r0, #0x0cc]     @ GPIO25_CTRL offset (0x04 + 25*8)

blink_loop:
    @ Turn LED on (GPIO25 high)
    ldr r0, =0xd0000000      @ SIO base address
    mov r1, #1
    lsl r1, r1, #25          @ Set bit 25
    str r1, [r0, #0x01c]     @ GPIO_OUT_SET

    @ Delay
    bl delay

    @ Turn LED off (GPIO25 low)
    ldr r0, =0xd0000000      @ SIO base address
    mov r1, #1
    lsl r1, r1, #25          @ Set bit 25
    str r1, [r0, #0x028]     @ GPIO_OUT_CLR

    @ Delay
    bl delay

    b blink_loop             @ Infinite loop

delay:
    ldr r2, =0x000f0000      @ Delay loop counter (~1 second at 150 MHz)
delay_loop:
    subs r2, r2, #1
    bne delay_loop
    bx lr

@ Ensure a newline follows this line
