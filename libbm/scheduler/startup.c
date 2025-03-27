// External declarations for handlers and main
extern void SysTick_Handler(void);
extern void PendSV_Handler(void);
extern int main(void);

// Reset handler: entry point after reset
void Reset_Handler(void) {
    main();
}

// Default handler for unhandled interrupts
void Default_Handler(void) {
    while (1);
}

// Vector table placed in the .vectors section
__attribute__((section(".vectors")))
void (*const vector_table[])(void) = {
    (void (*)(void))0x20007FFC,  // Initial stack pointer (adjust based on SRAM size)
    Reset_Handler,               // Reset handler
    Default_Handler,             // NMI
    Default_Handler,             // HardFault
    Default_Handler,             // MemManage
    Default_Handler,             // BusFault
    Default_Handler,             // UsageFault
    0, 0, 0, 0,                  // Reserved
    Default_Handler,             // SVCall
    Default_Handler,             // DebugMon
    0,                           // Reserved
    PendSV_Handler,              // PendSV handler
    SysTick_Handler              // SysTick handler
};