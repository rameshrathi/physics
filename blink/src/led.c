#include "led.h"

// Placeholder register addresses (adjust per RP2350 datasheet)
#define GPIO_BASE       0xD0000000
#define GPIO25_CTRL     (GPIO_BASE + 0x0C4)  // Control register for GPIO25
#define GPIO_OUT_XOR    (GPIO_BASE + 0x00C)  // XOR register to toggle
#define GPIO_OUT_CLR    (GPIO_BASE + 0x008)  // Clear register to set low

volatile uint32_t* gpio25_ctrl = (volatile uint32_t*)GPIO25_CTRL;
volatile uint32_t* gpio_out_xor = (volatile uint32_t*)GPIO_OUT_XOR;
volatile uint32_t* gpio_out_clr = (volatile uint32_t*)GPIO_OUT_CLR;

void led_init(void) {
    // Enable GPIO peripheral (placeholder)
    // RESETS->RESET &= ~(1 << RESET_GPIO_BIT);

    // Set GPIO25 to GPIO function (funcsel = 5) and configure as output
    *gpio25_ctrl = (*gpio25_ctrl & ~0x1F) | 5;

    // Initially set LED low
    *gpio_out_clr = (1 << 25);
}

void led_toggle(void) {
    // Toggle GPIO25 state
    *gpio_out_xor = (1 << 25);
}