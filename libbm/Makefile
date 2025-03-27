# Makefile for RP2350 (ARM Cortex-M33)

# Toolchain (adjust path to your ARM GCC toolchain)
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Source files
SOURCES = main.c scheduler.c startup.c
OBJECTS = $(SOURCES:.c=.o)

# Output files
TARGET = scheduler
ELF = $(TARGET).elf
BIN = $(TARGET).bin

# Compiler flags
CFLAGS = -mcpu=cortex-m33 -mthumb -g -O0 -Wall -I.
LDFLAGS = -mcpu=cortex-m33 -mthumb -nostartfiles -T linker.ld

# Linker script (minimal)
LINKER_SCRIPT = linker.ld

# Default target
all: $(BIN)

# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link objects to ELF
$(ELF): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Convert ELF to BIN
$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@
	$(SIZE) $(ELF)

# Clean up
clean:
	rm -f $(OBJECTS) $(ELF) $(BIN)

# Flash (optional, requires openocd or similar)
flash: $(BIN)
	# Add your flashing command here, e.g., openocd
	# openocd -f interface/cmsis-dap.cfg -f target/rp2350.cfg -c "program $(BIN) verify reset exit 0x10000000"

.PHONY: all clean flash