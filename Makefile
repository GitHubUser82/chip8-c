CC = gcc
CFLAGS = -Wall -Wextra
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib
PROG = chip8_interpreter.exe

# Find all .c files in src/, convert to .o in build/
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

all: $(BIN_DIR)/$(PROG)

# Link final executable
$(BIN_DIR)/$(PROG): $(OBJ)
	#The @ symbol prevents Make from printing the command before executing it.
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -Iinclude -L$(LIB_DIR) -lglfw3 -lopengl32 -lgdi32

# Compile .c to .o automatically
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	#The @ symbol prevents Make from printing the command before executing it.
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ -c $< -Iinclude

# Clean project
clean:
	rm -rfv $(BUILD_DIR)/* $(BIN_DIR)/*
# Run final executable