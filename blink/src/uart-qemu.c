#include "uart.h"
#include "config.h"


// Clock frequency
#define SYSTEM_CLOCK_HZ_QEMU 24000000  // 24 MHz


#define UART0_BASE  0x40004000  // For mps2-an521
#define UART0_DR    (UART0_BASE + 0x00)
#define UART0_FR    (UART0_BASE + 0x18)
#define UART0_IBRD  (UART0_BASE + 0x24)
#define UART0_FBRD  (UART0_BASE + 0x28)
#define UART0_LCRH  (UART0_BASE + 0x2C)
#define UART0_CR    (UART0_BASE + 0x30)


volatile uint32_t* uart0_dr = (volatile uint32_t*)UART0_DR;
volatile uint32_t* uart0_fr = (volatile uint32_t*)UART0_FR;
volatile uint32_t* uart0_ibrd = (volatile uint32_t*)UART0_IBRD;
volatile uint32_t* uart0_fbrd = (volatile uint32_t*)UART0_FBRD;
volatile uint32_t* uart0_lcrh = (volatile uint32_t*)UART0_LCRH;
volatile uint32_t* uart0_cr = (volatile uint32_t*)UART0_CR;


void uart_init(void) {
    uint32_t baud_div = SYSTEM_CLOCK_HZ_QEMU / (16 * 115200);
    *uart0_ibrd = baud_div;
    *uart0_fbrd = 0;
    *uart0_lcrh = (3 << 5);  // 8 bits
    *uart0_cr = (1 << 0) | (1 << 8) | (1 << 9);  // Enable UART, TX, RX
}


void uart_write(char c) {
    while (*uart0_fr & (1 << 5)) {}  // Wait for TX FIFO
    *uart0_dr = c;
}


void uart_print(const char* str) {
    while (*str) {
        uart_write(*str++);
    }
}