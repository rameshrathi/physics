CC = gcc
CFLAGS = -Wall -Wextra -I src/include
SRC_DIR = lib
BUILD_DIR = build
TARGET_LIB = libbm.a
TARGET_EXE = main.out

SRCS := $(wildcard $(SRC_DIR)/*.c) main.c
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(filter-out main.c, $(SRCS))) $(BUILD_DIR)/main.o

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c main.c -o $(BUILD_DIR)/main.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET_LIB): $(OBJS)
	ar rcs $(TARGET_LIB) $(OBJS)

$(TARGET_EXE): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET_EXE)

.PHONY: clean run
clean:
	rm -rf $(BUILD_DIR) $(TARGET_LIB) $(TARGET_EXE)

run: $(TARGET_EXE)
	./$(TARGET_EXE)