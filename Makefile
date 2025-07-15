CC = gcc
CFLAGS = -Wall -Wextra
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib

WINDOWS_PROG = chip8_interpreter.exe
LINUX_PROG = chip8_interpreter.out

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

# Builds
linux: clean_build $(BIN_DIR)/$(LINUX_PROG)
windows: clean_build $(BIN_DIR)/$(WINDOWS_PROG)

# Linux target
$(BIN_DIR)/$(LINUX_PROG): $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -Iinclude -L$(LIB_DIR) -lglfw3_linux -lm -lGL -lpthread -ldl

# Windows target
$(BIN_DIR)/$(WINDOWS_PROG): $(OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -Iinclude -L$(LIB_DIR) -lglfw3_windows -lopengl32 -lgdi32

# Rule: .c → .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ -c $< -Iinclude

# Clean only builds (safe)
clean_build:
	rm -rfv $(BUILD_DIR)/*

# Full clean
clean:
	rm -rfv $(BUILD_DIR)/* $(BIN_DIR)/*
