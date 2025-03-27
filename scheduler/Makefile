# Makefile for RP2350 (ARM Cortex-M33)

# Toolchain
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Build directory
BUILD_DIR = build

# Target name
TARGET = scheduler

# Source files
SOURCES = main.c lib/scheduler.c lib/startup.c

# Object files
OBJECTS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))

# Output files
ELF = $(BUILD_DIR)/$(TARGET).elf
BIN = $(BUILD_DIR)/$(TARGET).bin

# Compiler flags
CFLAGS = -mcpu=cortex-m33 -mthumb -g -O0 -Wall -Ilib

# Linker flags
LDFLAGS = -mcpu=cortex-m33 -mthumb -nostartfiles -T linker.ld

# Default target
all: $(BIN)

# Compile .c to .o
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
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
	rm -rf $(BUILD_DIR)

# Note: Ensure that 'build/' is added to .gitignore to avoid committing build artifacts.

.PHONY: all clean