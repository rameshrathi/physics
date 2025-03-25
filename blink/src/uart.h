#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(void);           // Initialize UART
void uart_write(char c);        // Write a character
void uart_print(const char* str);  // Print a string

#endif