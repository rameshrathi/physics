#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

void systick_init(void);       // Initialize SysTick timer
void delay_ms(uint32_t ms);    // Delay in milliseconds
void systick_handler(void);    // SysTick interrupt handler

#endif