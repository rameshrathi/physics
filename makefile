# Makefile

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -g -I./LibRK -I./src
AR := ar
ARFLAGS := rcs

# Directories
BUILD_DIR := build
LIB_DIR := $(BUILD_DIR)/lib
OBJ_DIR := $(BUILD_DIR)/obj
BIN := $(BUILD_DIR)/app

# Sources
LIB_SRC := $(wildcard LibRK/*.cpp)
SRC_SRC := $(wildcard src/*.cpp)

# Object files
LIB_OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(LIB_SRC))
SRC_OBJ := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_SRC))

# Static library
STATIC_LIB := $(LIB_DIR)/libRK.a

# Default target
all: $(BIN)

# Build binary
$(BIN): $(STATIC_LIB) $(SRC_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC_OBJ) -L$(LIB_DIR) -lRK

# Build static library
$(STATIC_LIB): $(LIB_OBJ)
	@mkdir -p $(dir $@)
	$(AR) $(ARFLAGS) $@ $^

# Compile library object files
$(OBJ_DIR)/LibRK/%.o: LibRK/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile source object files
$(OBJ_DIR)/src/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the application
run: all
	./$(BIN)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)