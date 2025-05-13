# Chat Service Makefile
# Supports building the chat service and other components

# Compiler and flags
CC = g++
CXX_FLAGS = -std=c++17 -Wall -Wpedantic -Wextra

# Directories
RK_DIR = RK
SRC_DIR = src
LIB_DIR = $(SRC_DIR)/lib
OBJ_DIR = .bin/obj
BIN_DIR = .bin

BOOST_INCLUDE = /usr/include
OPENSSL_INCLUDE = /usr/include
BOOST_LIB = /usr/lib/x86_64-linux-gnu
OPENSSL_LIB = /usr/lib/x86_64-linux-gnu

# Linker flags and libraries
LDFLAGS = -L$(BOOST_LIB) -L$(OPENSSL_LIB)
# Libraries needed: boost_system, boost_thread, boost_chrono, boost_regex, boost_atomic, ssl, crypto, pthread
LIBS = -lboost_system -lboost_thread -lboost_chrono -lboost_regex -lboost_atomic -lssl -lcrypto -lpthread

# Library
LIB_NAME = RKLib
STATIC_LIB = $(LIB_DIR)/lib$(LIB_NAME).a
AK_SRCS = $(wildcard $(AK_DIR)/*.cpp)
AK_OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(AK_SRCS)))

# Output executable name
EXEC = service_manager

# Find all source files recursively
SRC_FILES := $(shell find $(SRC_DIR) -name "*.cpp")
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Include directories
INCLUDES = -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIB_DIR)/include

# Libraries to link
LDFLAGS = -pthread

# Platform-specific settings
ifeq ($(OS),Windows_NT)
    # Windows
    LDFLAGS += -lws2_32
    RM = del /Q
    MKDIR = mkdir
    EXE_EXT = .exe
else
    # Unix-like (Linux, macOS)
    RM = rm -f
    MKDIR = mkdir -p
    EXE_EXT =
endif

# Build static library
$(STATIC_LIB): $(RK_OBJS)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

# Define the executable with platform-specific extension
EXECUTABLE = $(BIN_DIR)/$(EXEC)$(EXE_EXT)

# Default target
all: directories $(EXECUTABLE)

# Create necessary directories
directories:
	@$(MKDIR) $(BIN_DIR) $(OBJ_DIR) $(OBJ_DIR)/lib

# Link the executable
$(EXECUTABLE): $(OBJ_FILES) $(STATIC_LIB)
	@echo "Linking $@..."
	@$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	@$(MKDIR) $(dir $@)
	@$(CC) $(CXX_FLAGS) $(INCLUDES) -I$(BOOST_INCLUDE) -I$(OPENSSL_INCLUDE) -c $< -o $@

# Clean build files
clean:
	@echo "Cleaning build files..."
	@$(RM) $(OBJ_DIR)/*.o $(EXECUTABLE) -rf $(BIN_DIR)
	@echo "Clean complete"

# Run the service manager
run: $(EXECUTABLE)
	@echo "Running service manager..."
	@$(EXECUTABLE)

# Run the chat service directly (option 9 in the menu)
chat: $(EXECUTABLE)
	@echo "Running chat service..."
	@$(EXECUTABLE) 9

# Re-build everything from scratch
rebuild: clean all

# Display help
help:
	@echo "Chat Service Makefile"
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build the service manager (default)"
	@echo "  clean     - Remove build files"
	@echo "  run       - Build and run the service manager"
	@echo "  chat      - Build and run service manager, directly opening chat service"
	@echo "  rebuild   - Clean and rebuild everything"
	@echo "  help      - Display this help message"

.PHONY: all clean run chat rebuild help directories