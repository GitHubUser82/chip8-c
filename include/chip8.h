#pragma once

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32

int chip8Init(const char* filepath);
bool (*getChip8Screen(void))[CHIP8_DISPLAY_WIDTH];
void chip8UpdateKeypadState(bool keys[16]);
int chip8Update(void);