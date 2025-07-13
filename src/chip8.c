//this source file manages the CHIP-8 interpreter's logic and uses a bool two-dimensional array to represent the CHIP-8 screen

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32

static bool chip8Screen[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];

bool (*getChip8Screen())[CHIP8_DISPLAY_WIDTH] {
    return chip8Screen;
}

static void randomizeScreen() {
    for (int i=0; i<CHIP8_DISPLAY_HEIGHT; i++) {
        for (int j=0; j<CHIP8_DISPLAY_WIDTH; j++) {
            chip8Screen[i][j]=(bool)(rand()%2);
        }
    }
}

void chip8Update() {
    randomizeScreen();
}

/* static void _printScreenToConsole() {
    for (int i=0; i<CHIP8_DISPLAY_HEIGHT; i++) {
        for (int j=0; j<CHIP8_DISPLAY_WIDTH; j++) {
            printf("%d", (int)chip8Screen[i][j]);
        }
        printf("\n");
    }
} */
