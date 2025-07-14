//this source file manages the CHIP-8 interpreter's logic and uses a bool two-dimensional array to represent the CHIP-8 screen

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <graphics.h>

static void clearChip8Screen();
static int loadFileToMemory(const char*);


#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32

#define CHIP8_MEMORY_SIZE 4096 //in bytes
#define STARTING_MEMORY_ADDRESS 0x200

typedef struct {
    uint8_t  V[16]; //General purpose registers
    uint16_t I; //Index register
    uint16_t PC; //Program counter
    uint8_t  SP; //Stack pointer
    uint16_t stack[16]; //Stack
    uint8_t  memory[CHIP8_MEMORY_SIZE]; //RAM
    uint8_t  delay_timer;
    uint8_t  sound_timer;
    bool     keypad[16]; //Input
    bool     screen[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];; //Display buffer
} chip8State;
static chip8State state;

int chip8Init(const char* filepath) {
    state.PC = STARTING_MEMORY_ADDRESS;
    clearChip8Screen();
    return loadFileToMemory(filepath);
}

//prints 32 memory bytes per line
/* static void printMemory() {
    for (int i=0; i<CHIP8_MEMORY_SIZE;) {
        for (int j=0; j<32; j++) {
            printf("%x ", state.memory[i]);
            i++;
        }
        printf("\n");
    }
} */

static int loadFileToMemory(const char* filepath) {

    FILE* fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "[chip8] ERROR: could not open specified file");
        return 1;
    }
    size_t nbOfBytesRead = fread(state.memory+STARTING_MEMORY_ADDRESS, 1, CHIP8_MEMORY_SIZE-STARTING_MEMORY_ADDRESS, fp);
    //printf("%zu bytes read\n", nbOfBytesRead);
    fclose(fp);

    return 0;
}

bool (*getChip8Screen())[CHIP8_DISPLAY_WIDTH] {
    return state.screen;
}

static void clearChip8Screen() {
    for (int i=0; i<CHIP8_DISPLAY_WIDTH; i++) {
        for (int j=0; j<CHIP8_DISPLAY_HEIGHT; j++) {
            state.screen[j][i]=false;
        }
    }
}

static int executeInstruction() {

    unsigned char instruction[2] = {state.memory[state.PC], state.memory[state.PC+1]};
    if (state.PC<CHIP8_MEMORY_SIZE-2) {
        state.PC+=2;
    }
    else {
        fprintf(stderr, "[chip8] ERROR: attemp to read from an invalid memory address (out of bounds)");
        return 1;
    }

    uint8_t opcode; //first nibble (4 bits)
    uint8_t x;
    uint8_t y;
    uint8_t n;
    uint8_t nn;
    uint16_t nnn;
    
    opcode = instruction[0] >> 4;
    x = instruction[0] & 0x0F;
    y = instruction[1] >> 4;
    n = instruction[1] & 0x0F;
    nn = instruction[1];
    nnn = (x<<8) + nn;

    //TEMPORARY
    switch (opcode) {
        case 0x0:
            clearChip8Screen();
            graphicsSetFrameChanged(true);
            break;
        case 0x1:
            state.PC=nnn;
            break;
        case 0x6:
            state.V[x]=nn;
            break;
        case 0x7:
            state.V[x]+=nn;
            break;
        case 0xA:
            state.I=nnn;
            break;
        case 0xD:
            state.V[0xF] = 0;
            uint8_t vx = state.V[x];
            uint8_t vy = state.V[y];
            for (int i = 0; i < n; i++) {
                uint8_t byte = state.memory[state.I + i];
                for (int j = 0; j < 8; j++) {
                    uint8_t bit = (byte >> (7 - j)) & 1;
                    uint8_t xPos = (vx + j) % CHIP8_DISPLAY_WIDTH;
                    uint8_t yPos = (vy + i) % CHIP8_DISPLAY_HEIGHT;
                    if (bit) {
                        if (state.screen[yPos][xPos])
                            state.V[0xF] = 1;
                        state.screen[yPos][xPos] ^= 1;
                    }
                }
            }
            break;
    }

    return 0;

}

void chip8ExecuteInstructions(int nbOfInstructions) {
    for (int i=0; i<nbOfInstructions; i++)
        executeInstruction();
}

/* void chip8Update() {
    randomizeScreen();
    graphicsSetFrameChanged(true);
} */

/* void randomizeScreen() {
    for (int i=0; i<CHIP8_DISPLAY_HEIGHT; i++) {
        for (int j=0; j<CHIP8_DISPLAY_WIDTH; j++) {
            screen[i][j]=(bool)(rand()%2);
        }
    }
} */

/* static void printScreenToConsole() {
    for (int i=0; i<CHIP8_DISPLAY_HEIGHT; i++) {
        for (int j=0; j<CHIP8_DISPLAY_WIDTH; j++) {
            printf("%d", (int)screen[i][j]);
        }
        printf("\n");
    }
} */