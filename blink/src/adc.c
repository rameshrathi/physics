#include "adc.h"

// Placeholder register addresses (adjust per RP2350 datasheet)
#define ADC_BASE    0x400A0000
#define ADC_CS      (ADC_BASE + 0x00)  // Control and status
#define ADC_RESULT  (ADC_BASE + 0x04)  // Conversion result

volatile uint32_t* adc_cs = (volatile uint32_t*)ADC_CS;
volatile uint32_t* adc_result = (volatile uint32_t*)ADC_RESULT;

void adc_init(void) {
    // Enable ADC peripheral (placeholder)
    // RESETS->RESET &= ~(1 << RESET_ADC_BIT);

    // Configure ADC: enable and select temperature sensor channel (assumed 4)
    *adc_cs = (1 << 0) | (4 << 8);  // Bit 0: enable, bits 11-8: channel
}

int temp_read(void) {
    // Start conversion
    *adc_cs |= (1 << 2);  // Start bit (placeholder)

    // Wait for conversion complete
    while (!(*adc_cs & (1 << 3))) {}  // Done bit (placeholder)

    int adc_value = *adc_result;

    // Convert ADC value to temperature (tenths of °C)
    // Placeholder formula: assumes 12-bit ADC (0-4095)
    int temperature_tenths = (adc_value * 10) / 4096;  // Simplified; adjust per datasheet
    return temperature_tenths;
}