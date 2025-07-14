#pragma once

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32

int chip8Init(const char* filepath);
bool (*getChip8Screen(void))[CHIP8_DISPLAY_WIDTH];
void chip8ExecuteInstructions(int nbOfInstructions);

//void chip8Update(void);
//void randomizeScreen(void);