# Target name (output executable)
TARGET = libbm

# Directories
SRC_DIR = src
BUILD_DIR = build

# Source files (all .c files in src/)
SOURCES = $(wildcard $(SRC_DIR)/*.c)

# Object files (in build directory)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Compiler
CC = cc

# Compiler flags
CFLAGS = -g -Wall -O2

# Default target
all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

# Create build directory if it doesn’t exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link object files into executable
$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

# Compile .c files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean
