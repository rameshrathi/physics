# Makefile for RP2350 LED Blink
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

ASFLAGS = -mcpu=cortex-m33 -mthumb
LDFLAGS = -T rp2350.ld -nostartfiles

# Build directory
BUILD_DIR = build

# Target and object files with build directory prefix
TARGET = $(BUILD_DIR)/blink
OBJECTS = $(BUILD_DIR)/startup.o $(BUILD_DIR)/main.o

all: $(BUILD_DIR) $(TARGET).bin

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/startup.o: startup.s
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/main.o: main.s
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

flash: $(TARGET).bin
	picotool load -x $(TARGET).bin

.PHONY: all clean flash