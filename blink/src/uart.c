#include "uart.h"
#include "config.h"

// Placeholder register addresses (adjust per RP2350 datasheet)
#define UART0_BASE  0x40070000
#define UART0_DR    (UART0_BASE + 0x00)  // Data register
#define UART0_FR    (UART0_BASE + 0x18)  // Flag register
#define UART0_IBRD  (UART0_BASE + 0x24)  // Integer baud rate divisor
#define UART0_FBRD  (UART0_BASE + 0x28)  // Fractional baud rate divisor
#define UART0_LCRH  (UART0_BASE + 0x2C)  // Line control
#define UART0_CR    (UART0_BASE + 0x30)  // Control register

volatile uint32_t* uart0_dr = (volatile uint32_t*)UART0_DR;
volatile uint32_t* uart0_fr = (volatile uint32_t*)UART0_FR;
volatile uint32_t* uart0_ibrd = (volatile uint32_t*)UART0_IBRD;
volatile uint32_t* uart0_fbrd = (volatile uint32_t*)UART0_FBRD;
volatile uint32_t* uart0_lcrh = (volatile uint32_t*)UART0_LCRH;
volatile uint32_t* uart0_cr = (volatile uint32_t*)UART0_CR;

void uart_init(void) {
    // Enable UART0 peripheral (placeholder)
    // RESETS->RESET &= ~(1 << RESET_UART0_BIT);

    // Set GPIO0 to UART0 TX (funcsel = 2, placeholder)
    // GPIO0_CTRL->FUNCSEL = 2;

    // Set baud rate to 115200
    uint32_t baud_div = SYSTEM_CLOCK_HZ / (16 * 115200);
    *uart0_ibrd = baud_div;
    *uart0_fbrd = 0;  // Simplified; adjust fractional part if needed

    // Configure: 8 bits, 1 stop bit, no parity
    *uart0_lcrh = (3 << 5);  // Word length = 8 bits

    // Enable UART, TX, and RX
    *uart0_cr = (1 << 0) | (1 << 8) | (1 << 9);
}

void uart_write(char c) {
    // Wait until TX FIFO is not full
    while (*uart0_fr & (1 << 5)) {}  // TXFF bit
    *uart0_dr = c;
}

void uart_print(const char* str) {
    while (*str) {
        uart_write(*str++);
    }
}