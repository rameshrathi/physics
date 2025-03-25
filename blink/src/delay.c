#include "delay.h"
#include "config.h"

// SysTick registers (Cortex-M33 standard)
#define SYSTICK_BASE    0x40108000
#define SYSTICK_CTRL    (SYSTICK_BASE + 0x00)
#define SYSTICK_LOAD    (SYSTICK_BASE + 0x04)
#define SYSTICK_VAL     (SYSTICK_BASE + 0x08)

volatile uint32_t* SysTick_CTRL = (volatile uint32_t*)SYSTICK_CTRL;
volatile uint32_t* SysTick_LOAD = (volatile uint32_t*)SYSTICK_LOAD;
volatile uint32_t* SysTick_VAL  = (volatile uint32_t*)SYSTICK_VAL;

volatile uint32_t tick_counter = 0;

void systick_init(void) {
    // Configure SysTick for 1 kHz (1 ms ticks)
    uint32_t reload_value = SYSTEM_CLOCK_HZ / 1000;
    *SysTick_LOAD = reload_value - 1;
    *SysTick_VAL = 0;
    *SysTick_CTRL = (1 << 0) | (1 << 1) | (1 << 2);  // Enable, interrupt, processor clock
}

void delay_ms(uint32_t ms) {
    uint32_t start = tick_counter;
    while (tick_counter - start < ms) {}  // Busy wait
}

void systick_handler(void) {
    tick_counter++;
}