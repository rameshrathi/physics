#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adc_init(void);    // Initialize ADC
int temp_read(void);    // Read temperature in tenths of a degree

#endif