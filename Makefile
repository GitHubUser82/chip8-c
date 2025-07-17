CC = gcc
CFLAGS = -Wall -Wextra
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib

WINDOWS_PROG = chip8_interpreter.exe
LINUX_PROG = chip8_interpreter.out

SRC = $(wildcard $(SRC_DIR)/*.c)
WINDOWS_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%_WINDOWS.o, $(SRC))
LINUX_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%_LINUX.o, $(SRC))

windows: $(BIN_DIR)/$(WINDOWS_PROG)
linux: $(BIN_DIR)/$(LINUX_PROG)

$(BIN_DIR)/$(WINDOWS_PROG): $(WINDOWS_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -Iinclude -L$(LIB_DIR) -lglfw3_windows -lopengl32 -lgdi32

$(BIN_DIR)/$(LINUX_PROG): $(LINUX_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -Iinclude -L$(LIB_DIR) -lglfw3_linux -lm -lGL -lpthread -ldl

$(BUILD_DIR)/%_WINDOWS.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ -Iinclude

$(BUILD_DIR)/%_LINUX.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ -Iinclude
	
clean:
	rm -rfv $(BUILD_DIR)/* $(BIN_DIR)/*
