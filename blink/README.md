## Test using QEMU emulation 
qemu-system-arm -machine mps2-an521 -cpu cortex-m33 -nographic -kernel build/program.bin

## With Serial Output
qemu-system-arm -machine mps2-an521 -cpu cortex-m33 -nographic -serial stdio -kernel build/program.bin
OR If serial port conflict
qemu-system-arm -machine mps2-an521 -cpu cortex-m33 -nographic -serial mon:stdio -kernel build/program.bin

# Machine mps2-an521 settings
#define SYSTEM_CLOCK_HZ 24000000  // For mps2-an521, the default clock is typically 24 MHz
#define UART0_BASE  0x40004000  // For mps2-an521
uint32_t baud_div = SYSTEM_CLOCK_HZ / (16 * 115200);