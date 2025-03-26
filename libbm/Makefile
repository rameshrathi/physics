# Toolchain
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

# Flags
CFLAGS = -mcpu=cortex-m0plus -mthumb -O2
LDFLAGS = -nostdlib -T link.ld

# Source files
SRCS = startup.s main.s

# Output
TARGET = blink

all: $(TARGET).bin

$(TARGET).elf: $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

flash:
	openocd -f interface/raspberrypi-swd.cfg -f target/rp2350.cfg -c "program $(TARGET).bin verify reset exit"

clean:
	rm -f *.elf *.bin