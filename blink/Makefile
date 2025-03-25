# Toolchain definitions
CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Directory definitions
SRC_DIR = src
BUILD_DIR = build

# Compiler and linker flags
# CFLAGS = -mcpu=cortex-m33 -mthumb -I $(SRC_DIR)
CFLAGS = -mcpu=cortex-m33 -mthumb -I $(SRC_DIR) -g -Og
LDFLAGS = -nostdlib -T $(SRC_DIR)/linker.ld

# List of object files
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/startup.o $(BUILD_DIR)/led.o $(BUILD_DIR)/adc.o $(BUILD_DIR)/uart.o $(BUILD_DIR)/delay.o

# Default target
all: $(BUILD_DIR)/program.bin

# Generate binary from ELF
$(BUILD_DIR)/program.bin: $(BUILD_DIR)/program.elf
	$(OBJCOPY) -O binary $< $@

# Link object files into ELF
$(BUILD_DIR)/program.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

# Compile root-level C files (main.c)
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C files from src/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files from src/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Declare phony targets
.PHONY: all clean