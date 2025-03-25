#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init(void);    // Initialize LED GPIO
void led_toggle(void);  // Toggle LED state

#endif