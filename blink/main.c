#include <stdint.h>
#include "led.h"
#include "adc.h"
#include "uart.h"
#include "delay.h"

// Convert temperature (tenths of °C) to string, e.g., "25.3"
void int_to_str(uint32_t value, char* buffer, uint8_t size) {
    int integer_part = value / 10;
    int fractional_part = value % 10;
    uint8_t i = 0;

    if (integer_part == 0) {
        buffer[i++] = '0';
    } else {
        if (integer_part >= 100) {
            buffer[i++] = '0' + (integer_part / 100);
            integer_part %= 100;
        }
        if (integer_part >= 10 || i > 0) {
            buffer[i++] = '0' + (integer_part / 10);
            integer_part %= 10;
        }
        buffer[i++] = '0' + integer_part;
    }
    buffer[i++] = '.';
    buffer[i++] = '0' + fractional_part;
    buffer[i++] = '\n';
    buffer[i] = '\0';
}

int main(void) {
    // Initialize peripherals
    led_init();
    adc_init();
    uart_init();
    systick_init();

    // Optional: Startup indication (test functionality)
    uart_print("System started\n");

    // Main loop
    while (1) {
        led_toggle();               // Blink LED
        int temp_tenths = temp_read();  // Read temperature
        char buffer[16];
        int_to_str(temp_tenths, buffer);  // Convert to string
        uart_print(buffer);         // Send over UART
        delay_ms(1000);             // Wait 1 second
    }

    return 0;  // Never reached
}

// Stub for _exit (required with -nostdlib)
void _exit(int status) {
    while (1);  // Infinite loop
}