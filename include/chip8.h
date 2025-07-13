#pragma once

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32

bool (*getChip8Screen(void))[CHIP8_DISPLAY_WIDTH];
void chip8Update(void);



void randomizeScreen(void);