# Executable name
EXEC = sockets

# Test executable name
TEST_EXEC = tests

# Build directory
BUILD_DIR_ROOT = build

# Target OS detection
ifeq ($(OS),Windows_NT) # OS is a preexisting environment variable on Windows
	OS = windows
else
	UNAME := $(shell uname -s)
	ifeq ($(UNAME),Darwin)
		OS = macos
	else ifeq ($(UNAME),Linux)
		OS = linux
	else
	$(error OS not supported by this Makefile)
	endif
endif

COMPILER = g++
OPENSSL_DIR = /usr/include/openssl/
ifeq ($(OS),macos)
	COMPILER = clang++
	OPENSSL_DIR = /opt/homebrew/Cellar/openssl@3/3.4.0/
endif


# Executable sources (found recursively inside SRC_DIR)
SRC_DIR = src
SRCS := $(sort $(shell find $(SRC_DIR) -name '*.cpp'))

# Test sources (found recursively inside TEST_DIR if it exists)
TEST_DIR = tests
TEST_SRCS := $(sort $(shell find $(TEST_DIR) -name '*.cpp' 2> /dev/null))

# Includes
INCLUDE_DIR = include
OPENSSL_INCLUDE_DIR = $(OPENSSL_DIR)$(INCLUDE_DIR)
INCLUDES = $(addprefix -I,$(SRC_DIR) $(INCLUDE_DIR) $(OPENSSL_INCLUDE_DIR))
TEST_INCLUDES = -I$(TEST_DIR)

# C preprocessor settings
CPPFLAGS = $(INCLUDES) -MMD -MP

# C++ compiler settings
CXX = $(COMPILER)
CXXFLAGS = -std=c++20
WARNINGS = -Wall -Wpedantic -Wextra


# Linker flags
LDFLAGS = -lssl -lcrypto
TEST_LDFLAGS =

# Libraries to link
LDLIBS = -lssl -lcrypto
TEST_LDLIBS =


# Platform-specific settings
ifeq ($(OS),windows)
# Link libgcc and libstdc++ statically on Windows
	LDFLAGS += -static-libgcc -static-libstdc++

# Windows-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS +=
else ifeq ($(OS),macos)
# macOS-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS +=
else ifeq ($(OS),linux)
# Linux-specific settings
	INCLUDES +=
	LDFLAGS +=
	LDLIBS +=
endif


################################################################################
#### Final setup
################################################################################

# Add .exe extension to executables on Windows
ifeq ($(OS),windows)
	EXEC := $(EXEC).exe
	TEST_EXEC := $(TEST_EXEC).exe
endif

# Platform-specific build and assets directories
BUILD_DIR := $(BUILD_DIR_ROOT)/$(OS)
ASSETS_OS_DIR := $(ASSETS_OS_DIR)/$(OS)

# Debug (default) and release configuration settings
ifeq ($(release),1)
	BUILD_DIR := $(BUILD_DIR)/release
	CPPFLAGS += -DNDEBUG
	CXXFLAGS += -O3
else
	BUILD_DIR := $(BUILD_DIR)/debug
	CXXFLAGS += -O0 -g
endif

# Object and bin directories
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

# Object files
MAIN_SRC = $(SRC_DIR)/main.cpp
MAIN_OBJ := $(MAIN_SRC:%.cpp=$(OBJ_DIR)/%.o)
SRCS_WITHOUT_MAIN := $(filter-out $(MAIN_SRC),$(SRCS))
SRC_OBJS_WITHOUT_MAIN := $(SRCS_WITHOUT_MAIN:%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJS := $(TEST_SRCS:%.cpp=$(OBJ_DIR)/%.o)
ALL_OBJS := $(MAIN_OBJ) $(SRC_OBJS_WITHOUT_MAIN) $(TEST_OBJS)

# Dependency files
DEPS := $(ALL_OBJS:.o=.d)

# Compilation database fragments
COMPDBS := $(ALL_OBJS:.o=.json)

# All files (sources and headers) (for formatting and linting)
FILES := $(shell find $(SRC_DIR) $(TEST_DIR) $(INCLUDE_DIR) -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -o -name '*.inl' 2> /dev/null)



################################################################################
#### Targets
################################################################################

# Disable default implicit rules
.SUFFIXES:

.PHONY: all
all: $(BIN_DIR)/$(EXEC) $(if $(TEST_SRCS),$(BIN_DIR)/$(TEST_EXEC))

# Build executable
$(BIN_DIR)/$(EXEC): $(MAIN_OBJ) $(SRC_OBJS_WITHOUT_MAIN)
	@echo "Building executable: $@"
	@mkdir -p $(@D)
	@$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

# Build tests
$(BIN_DIR)/$(TEST_EXEC): LDFLAGS += $(TEST_LDFLAGS)
$(BIN_DIR)/$(TEST_EXEC): LDLIBS += $(TEST_LDLIBS)
$(BIN_DIR)/$(TEST_EXEC): $(TEST_OBJS) $(SRC_OBJS_WITHOUT_MAIN)
	@echo "Building tests: $@"
	@mkdir -p $(@D)
	@$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@


# Compile C++ source files
$(OBJ_DIR)/$(TEST_DIR)/%.o: INCLUDES += $(TEST_INCLUDES)
$(ALL_OBJS): $(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling: $<"
	@mkdir -p $(@D)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(WARNINGS) -c $< -o $@

# Include automatically-generated dependency files
-include $(DEPS)


# Build and run executable
.PHONY: run
run: $(BIN_DIR)/$(EXEC)
	@echo "Running program: $<"
	@cd $(BIN_DIR) && ./$(EXEC)

# Build and run tests
.PHONY: test
test: $(BIN_DIR)/$(TEST_EXEC)
	@echo "Running tests: $<"
	@cd $(BIN_DIR) && ./$(TEST_EXEC)


# Copy assets to bin directory for selected platform
.PHONY: copyassets
copyassets:
	@echo "Copying assets from $(ASSETS_DIR) and $(ASSETS_OS_DIR) to $(BIN_DIR)"
	@mkdir -p $(BIN_DIR)
	@cp -r $(ASSETS_DIR)/. $(BIN_DIR)/
	@cp -r $(ASSETS_OS_DIR)/. $(BIN_DIR)/ 2> /dev/null || :

# Clean all assets from bin directories for all platforms
.PHONY: cleanassets
cleanassets:
	@echo "Cleaning assets for all platforms"
	@find $(BUILD_DIR_ROOT) -path '*/bin/*' ! -name $(EXEC) ! -name $(TEST_EXEC) -delete

# Clean build directory for all platforms
.PHONY: clean
clean:
	@echo "Cleaning $(BUILD_DIR_ROOT) directory"
	@$(RM) -r $(BUILD_DIR_ROOT)

.PHONY: compdb
compdb: $(BUILD_DIR_ROOT)/compile_commands.json


