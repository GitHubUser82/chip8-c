//this source file manages the CHIP-8 interpreter's logic and uses a bool two-dimensional array to represent the CHIP-8 screen

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <graphics.h>

static void clearChip8Screen();
static int loadFileToMemory(const char*);


#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32

#define CHIP8_MEMORY_SIZE 4096 //in bytes
#define STARTING_MEMORY_ADDRESS 0x200

#define STACK_SIZE 16 //number of shorts (16-bit values)

#define INSTRUCTIONS_PER_SECOND 500

#define FONT_DATA_POSITION 0x050
const uint8_t fontData[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

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
    bool     screen[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH]; //Display buffer
    bool waitingForKey;
} Chip8State;
static Chip8State state;


int chip8Init(const char* filepath) {
    state.PC = STARTING_MEMORY_ADDRESS;
    state.SP = 0;
    state.waitingForKey = false;
    memcpy(state.memory+FONT_DATA_POSITION, fontData, sizeof(fontData));
    clearChip8Screen();
    return loadFileToMemory(filepath);
}

static int loadFileToMemory(const char* filepath) {

    FILE* fp = fopen(filepath, "rb");
    if (!fp) {
        fprintf(stderr, "[chip8] ERROR: could not open specified file");
        return 1;
    }

    //size_t nbOfBytesRead = fread(state.memory+STARTING_MEMORY_ADDRESS, 1, CHIP8_MEMORY_SIZE-STARTING_MEMORY_ADDRESS, fp);
    fread(state.memory+STARTING_MEMORY_ADDRESS, 1, CHIP8_MEMORY_SIZE-STARTING_MEMORY_ADDRESS, fp);
    
    fclose(fp);

    return 0;
}

bool (*getChip8Screen())[CHIP8_DISPLAY_WIDTH] {
    return state.screen;
}

static void clearChip8Screen() {
    for (int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++) {
        for (int x = 0; x < CHIP8_DISPLAY_WIDTH; x++) {
            state.screen[y][x] = false;
        }
    }
}

static int executeInstruction() {

    if (state.PC >= CHIP8_MEMORY_SIZE - 1) {
        fprintf(stderr, "[chip8] ERROR: attempt to read from an invalid memory address (out of bounds)\n");
        return 1;
    }

    uint8_t instruction[2] = {state.memory[state.PC], state.memory[state.PC+1]};
    state.PC += 2;

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
    nnn = ((instruction[0] & 0x0F) << 8) | instruction[1];

    /* WARNING: some cases, by only checking the first nibble, my allow some undefined instructions,
    so more checking needs to be added later*/    
    switch (opcode) {

        case 0x0:
            if(nnn==0x0E0) {
                clearChip8Screen();
                graphicsSetFrameChanged(true);
            }
            else if (nnn== 0x0EE) {
                if (state.SP == 0) {
                    fprintf(stderr, "[chip8] ERROR in executeInstruction: chip8 stack underflow\n");
                    return 1;
                }
                state.SP--;
                state.PC = state.stack[state.SP];
            }
            break;

        case 0x1:
            state.PC=nnn;
            break;

        case 0x2:
            if (state.SP>=STACK_SIZE) {
                fprintf(stderr, "[chip8] ERROR in executeInstruction: chip8 stack overflow\n");
                return 1;
                break;
            }
            //pushes current PC to stack
            state.stack[state.SP]=state.PC;
            state.SP++;
            //modifies PC
            state.PC=nnn;
            break;

        case 0x3:
            if (state.V[x]==nn)
                state.PC+=2;
            break;

        case 0x4:
            if (state.V[x]!=nn)
                state.PC+=2;

            break;

        case 0x5:
            if (state.V[x]==state.V[y])
                state.PC+=2;
            break;

        case 0x6:
            state.V[x]=nn;
            break;

        case 0x7:
            state.V[x]+=nn;
            break;

        case 0x8:
            switch(n) {
                case 0:
                    state.V[x]=state.V[y];
                    break;
                case 1:
                    state.V[x]|=state.V[y];
                    break;
                case 2:
                    state.V[x]&=state.V[y];
                    break;
                case 3:
                    state.V[x]^=state.V[y];
                    break;
                case 4:
                    uint16_t result = state.V[x] + state.V[y];
                    if (result>255)
                        state.V[0xF]=1;
                    else
                        state.V[0xF]=0;
                    state.V[x] = (uint8_t) result;
                    break;
                case 5:
                    state.V[0xF] = (state.V[x] >= state.V[y]) ? 1 : 0;
                    state.V[x] = state.V[x] - state.V[y];                 
                    break;
                case 7:
                    state.V[0xF] = (state.V[y] >= state.V[x]) ? 1 : 0;
                    state.V[x] = state.V[y] - state.V[x];               
                    break;
                case 6: //left shift
                    //original COSMAC VIP version
                    state.V[x]=state.V[y];
                    state.V[0xF]=state.V[x]&0b00000001;
                    state.V[x]<<=1;
                    break;
                case 0xE: //right shift
                    //original COSMAC VIP version
                    state.V[x]=state.V[y];
                    state.V[0xF]=state.V[x]>>7;
                    state.V[x]>>=1;
                    break;
            }
            break;

        case 0x9:
            if (state.V[x]!=state.V[y])
                state.PC+=2;
            break;

        case 0xA:
            state.I=nnn;
            break;

        case 0xB:
            //original COSMAC VIP version
            state.PC=nnn+state.V[0];
            break;

        case 0xC:
            state.V[x]=(rand()%256)&nn;
            break;

        case 0xD:
            state.V[0xF] = 0;
            uint8_t vx = state.V[x];
            uint8_t vy = state.V[y];
            for (int i = 0; i < n; i++) {
                if (state.I + i >= CHIP8_MEMORY_SIZE) {
                    fprintf(stderr, "[chip8] ERROR: attemp to draw sprite out of memory bounds\n");
                    return 1;
                }
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
            graphicsSetFrameChanged(true);
            break;
        
        case 0xE:
            if (nn == 0x9E) {
                if (state.keypad[state.V[x]])
                    state.PC += 2;
            }
            else if (nn == 0xA1) {
                if (!state.keypad[state.V[x]])
                    state.PC += 2;
            }
            break;

        case 0xF:
            if (nn==07)
                state.V[x]=state.delay_timer;
            else if (nn==15)
                state.delay_timer=state.V[x];
            else if (nn==18)
                state.sound_timer=state.V[x];
            else if (nn==0x1E)
                state.I+=state.V[x];
            //On the original COSMAC VIP, the key was only registered when it was pressed and then released.
            else if (nn==0x0A) {
                state.waitingForKey = true;
                for (int i=0; i<16; i++) {
                    if (state.keypad[i]==true) {
                        state.waitingForKey = false;
                        state.V[x] = i;
                        break;
                    }
                }
                if (state.waitingForKey)
                    state.PC-=2;
            }
            else if (nn==29) {
                state.I=FONT_DATA_POSITION + (state.V[x]&0x000F) * 5;
            }
            else if (nn==33) {
                if (state.I+2>=CHIP8_MEMORY_SIZE) {
                    fprintf(stderr, "[chip8] ERROR: attempt to write out of memory bounds\n");
                }
                uint16_t n = state.V[x];
                state.memory[state.I]=n/100;
                state.memory[state.I+1]=(n/10)%10;
                state.memory[state.I+2]=n%10;
            }
            else if (nn==55) {
                //modern implementation
                for (int i=0; i<=x; i++) {
                    state.memory[state.I + i] = state.V[i];
                }
            }
            else if (nn==65) {
                //modern implementation
                for (int i=0; i<=x; i++) {
                    state.V[i]=state.memory[state.I + i];
                }
            }
            break;

        default:
            fprintf(stderr, "[chip8] ERROR in executeInstruction: unknown instruction: %x%x\n", instruction[0], instruction[1]);
            return 1;
            break;
    }

    return 0;

}

static int executeInstructions(int nbOfInstructions) {
    for (int i=0; i<nbOfInstructions; i++)
        if (executeInstruction() != 0)
            return 1;
    return 0;
}

void chip8UpdateKeypadState(bool keys[16]) {
    for (int i = 0; i<16; i++) {
        state.keypad[i]=keys[i];
    }
}

int chip8Update() {

    static double lastTick = -1.0;
    if (lastTick<0) //we would otherwise be force to intialize lastTick to a constant value, if this trick is not used
        lastTick=glfwGetTime();
    double elaspedTime;
    if (!state.waitingForKey)  
        elaspedTime = glfwGetTime() - lastTick;
    if (elaspedTime>=1.0/60.0) {
        if (state.delay_timer>0)
            state.delay_timer--;
        if (state.sound_timer>0)
            state.sound_timer--;
        lastTick += 1.0/60.0;
    }
    
    if (executeInstructions((int)INSTRUCTIONS_PER_SECOND/TARGET_FPS) != 0)
        return 1;
    return 0;
} 

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

//prints 32 memory bytes per line
/* static void printMemoryToConsole() {
    for (int i=0; i<CHIP8_MEMORY_SIZE;) {
        for (int j=0; j<32; j++) {
            printf("%x ", state.memory[i]);
            i++;
        }
        printf("\n");
    }
} */