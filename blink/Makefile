# Toolchain
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

# Flags
CFLAGS = -mcpu=cortex-m0plus -mthumb -O2
LDFLAGS = -nostdlib -T link.ld

# Directories
BUILD_DIR = build

# Source files
SRCS = startup.s main.s
OBJS = $(SRCS:%.s=$(BUILD_DIR)/%.o)

# Target
TARGET = $(BUILD_DIR)/program

# Create build directory
$(shell mkdir -p $(BUILD_DIR))

all: $(TARGET).bin

# Compile assembly files
$(BUILD_DIR)/%.o: %.s
	$(AS) -mcpu=cortex-m0plus -mthumb -o $@ $

# Link object files
$(TARGET).elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash:
	openocd -f interface/raspberrypi-swd.cfg -f target/rp2350.cfg -c "program $(TARGET).bin verify reset exit"

clean:
	rm -rf $(BUILD_DIR)