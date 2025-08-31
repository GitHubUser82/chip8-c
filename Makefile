CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib

WINDOWS_PROG = chip8_interpreter.exe
LINUX_PROG = chip8_interpreter.out

SRC = $(wildcard $(SRC_DIR)/*.c)
WINDOWS_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%_WINDOWS.o, $(SRC))
LINUX_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%_LINUX.o, $(SRC))

.PHONY: windows linux clean all

linux: $(BIN_DIR)/$(LINUX_PROG)

windows: $(BIN_DIR)/$(WINDOWS_PROG)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR)/%_WINDOWS.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -Iinclude

$(BUILD_DIR)/%_LINUX.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -Iinclude

$(BIN_DIR)/$(WINDOWS_PROG): $(WINDOWS_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -Iinclude -L$(LIB_DIR) -lglfw3_windows -lopengl32 -lgdi32 -mwindows

$(BIN_DIR)/$(LINUX_PROG): $(LINUX_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -Iinclude -L$(LIB_DIR) -lglfw3_linux -lm -lGL -lpthread

clean:
	rm -rfv $(BUILD_DIR) $(BIN_DIR)
