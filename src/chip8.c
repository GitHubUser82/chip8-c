//this source file manages the CHIP-8 interpreter's logic and uses a bool two-dimensional array to represent the CHIP-8 screen

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <graphics.h>

static void clearChip8Screen();
static int loadFileToMemory(const char*);
void dumpMemory(void);
void generateTraceLog(const char*, int);


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
    uint8_t  V[16]; //general purpose registers
    uint16_t I; //index register
    uint16_t PC; //srogram counter
    uint8_t  SP; //stack pointer
    uint16_t stack[16]; //stack
    uint8_t  memory[CHIP8_MEMORY_SIZE]; //RAM
    uint8_t  delay_timer;
    uint8_t  sound_timer;
    bool     keypad[16]; //keypad state (true: key is in "pressed" state)
    bool     screen[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH]; //display buffer
    bool     isHalted; //the machine is waiting for a key to be pressed then released to resume its execution
    int      keyPressedDuringHalt; //the last key that was pressed while the interpreter was halted (in "isHalted" state)
} Chip8State;
static Chip8State state;


int chip8Init(const char* filepath) {
    state.PC = STARTING_MEMORY_ADDRESS;
    state.SP = 0;
    state.I = 0;
    for (int i=0; i<16; i++) {
        state.V[i]=0;
        state.stack[i]=0;
    }
    state.delay_timer = 0;
    state.sound_timer = 0;
    state.isHalted = false;
    state.keyPressedDuringHalt = -1;
    memcpy(state.memory+FONT_DATA_POSITION, fontData, sizeof(fontData));
    clearChip8Screen();
    return loadFileToMemory(filepath);
}

static int loadFileToMemory(const char* filepath) {

    FILE* fp = fopen(filepath, "rb");
    if (!fp) {
        fprintf(stderr, "[chip8] ERROR: could not open specified file\n");
        return 1;
    }

    size_t nbOfBytesRead = fread(state.memory+STARTING_MEMORY_ADDRESS, 1, CHIP8_MEMORY_SIZE-STARTING_MEMORY_ADDRESS, fp);
    if (nbOfBytesRead>=CHIP8_MEMORY_SIZE-STARTING_MEMORY_ADDRESS)
        fprintf(stderr, "[chip8] WARNING: ROM size too big for memory\n");
    
    fclose(fp);

    dumpMemory();

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

    static double lastTick = -1.0;
    if (lastTick<0) //we would otherwise be force to intialize lastTick to a constant value, if this trick is not used
        lastTick=glfwGetTime();
    double elaspedTime;
    if (!state.isHalted)  
        elaspedTime = glfwGetTime() - lastTick;
    if (elaspedTime>=1.0/60.0) {
        if (state.delay_timer>0)
            state.delay_timer--;
        if (state.sound_timer>0)
            state.sound_timer--;
        lastTick += 1.0/60.0;
    }


    if (state.PC >= CHIP8_MEMORY_SIZE - 1) {
        fprintf(stderr, "[chip8] ERROR: attempt to read from an invalid memory address (out of bounds)\n");
        return 1;
    }



    uint8_t instruction[2] = {state.memory[state.PC], state.memory[state.PC+1]};
    //uint16_t opcode = instruction[0] + instruction[1]<<8;
    uint16_t opcode = (instruction[0] << 8) | instruction[1];

    uint8_t first_nibble; //first nibble (4 bits)
    uint8_t x;
    uint8_t y;
    uint8_t n;
    uint8_t nn;
    uint16_t nnn;
    
    first_nibble = instruction[0] >> 4;
    x = instruction[0] & 0x0F;
    y = instruction[1] >> 4;
    n = instruction[1] & 0x0F;
    nn = instruction[1];
    nnn = ((instruction[0] & 0x0F) << 8) | instruction[1];

    //generateTraceLog("tracelog", opcode);

    state.PC += 2;

    switch (first_nibble) {

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
            state.stack[state.SP]=state.PC;
            state.SP++;
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
            if (n==0) {
                if (state.V[x]==state.V[y])
                    state.PC+=2;
            }
            break;

        case 0x6:
            state.V[x]=nn;
            break;

        case 0x7:
            state.V[x]+=nn;
            break;

        case 0x8:
            switch(n) {
                case 0x0:
                    state.V[x]=state.V[y];
                    break;
                case 0x1:
                    state.V[x]|=state.V[y];
                    break;
                case 0x2:
                    state.V[x]&=state.V[y];
                    break;
                case 0x3:
                    state.V[x]^=state.V[y];
                    break;
                case 0x4:
                    uint16_t result = state.V[x] + state.V[y];
                    state.V[x] = (uint8_t) result;
                    if (result>255)
                        state.V[0xF]=1;
                    else
                        state.V[0xF]=0;
                    break;
                case 0x5:
                    uint8_t tmp1 = state.V[x];
                    state.V[x] = state.V[x] - state.V[y];
                    state.V[0xF] = (tmp1 >= state.V[y]) ? 1 : 0;             
                    break;
                case 0x6: //right shift
                    //state.V[x]=state.V[y];
                    uint8_t tmp2 = state.V[x];
                    state.V[x]>>=1;
                    state.V[0xF]=tmp2 & 0b00000001;
                    break;
                case 0x7:
                    state.V[x] = state.V[y] - state.V[x];               
                    state.V[0xF] = (state.V[y] >= state.V[x]) ? 1 : 0;
                    break;

                case 0xE: //left shift
                    //state.V[x]=state.V[y];
                    uint8_t tmp3 = state.V[x];
                    state.V[x]<<=1;
                    state.V[0xF]=tmp3 >> 7;
                    break;
            }
            break;

        case 0x9:
            if (n==0) {
                if (state.V[x]!=state.V[y])
                    state.PC+=2;
            }
            break;

        case 0xA:
            state.I=nnn;
            break;

        case 0xB:
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
            if (nn==0x07)
                state.V[x]=state.delay_timer;
            else if (nn==0x15)
                state.delay_timer=state.V[x];
            else if (nn==0x18)
                state.sound_timer=state.V[x];
            else if (nn==0x1E)
                state.I+=state.V[x];
            else if (nn==0x0A) {

                if (state.isHalted == false) {
                    state.isHalted=true;
                    state.keyPressedDuringHalt=-1;
                    state.PC-=2;
                    break;
                }

                if (state.keyPressedDuringHalt == -1) {
                    for (int i=0; i<16; i++) {
                        if (state.keypad[i]==true) {
                            state.keyPressedDuringHalt=i;
                            break;
                        }
                    }
                    state.PC-=2;
                    break;
                }

                if (state.keyPressedDuringHalt<0) {
                    fprintf(stderr, "[chip8] ERROR: %d is an invalid value for state.keyPressedDuringHalt", state.keyPressedDuringHalt);
                    return 1;
                }
                else {
                    if (state.keypad[state.keyPressedDuringHalt] == false) {
                        state.isHalted == false;
                        state.V[x] = state.keyPressedDuringHalt;
                        state.keyPressedDuringHalt = -1;
                    }
                    else {
                        state.PC-=2;
                    }
                }

            }
            else if (nn==0x29) {
                state.I = FONT_DATA_POSITION + (state.V[x]&((uint8_t)0x0F)) * 5;
            }
            else if (nn==0x33) {
                /* if (state.I+2>=CHIP8_MEMORY_SIZE) {
                    fprintf(stderr, "[chip8] ERROR: attempt to write out of memory bounds\n");
                    return 1;
                } */
                uint8_t n = state.V[x];
                state.memory[state.I]=n/100;
                state.memory[state.I+1]=(n/10)%10;
                state.memory[state.I+2]=n%10;
            }
            else if (nn==0x55) {
                for (int k=0; k<=x; k++) {
                    state.memory[state.I+k] = state.V[k];
                }
                state.I+=(x+1);
            }
            else if (nn==0x65) {
                for (int k=0; k<=x; k++) {
                    state.V[k]=state.memory[state.I+k];
                }
                state.I+=(x+1);
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
    if (executeInstructions((int)INSTRUCTIONS_PER_SECOND/TARGET_FPS) != 0)
        return 1;
    return 0;
}

void dumpMemory() {
    FILE* fp = fopen("memorydump", "w");
    fwrite(state.memory, 1, CHIP8_MEMORY_SIZE, fp);
    fclose(fp);
}

void generateTraceLog(const char* filepath, int opcode) {
    static FILE* fp = NULL;
    static int nbCycles = 0;
    if (fp==NULL) {
        fp = fopen(filepath, "a");
    }
    //[01:0000] V0:00 V1:00 V2:00 V3:00 V4:00 V5:00 V6:00 V7:00 V8:00 V9:00 VA:00 VB:00 VC:00 VD:00 VE:00 VF:00 I:0000 SP:0 PC:0200 O:120a
    fprintf(fp, "[fn:%04x] V0:%02x V1:%02x V2:%02x V3:%02x V4:%02x V5:%02x V6:%02x V7:%02x V8:%02x V9:%02x VA:%02x VB:%02x VC:%02x VD:%02x VE:%02x VF:%02x I:%04x SP:%0x PC:%04x O:%04x\n", nbCycles, state.V[0],
    state.V[1], state.V[2], state.V[3], state.V[4], state.V[5], state.V[6], state.V[7], state.V[8], state.V[9], state.V[10], state.V[11], state.V[12], state.V[13], state.V[14], state.V[15],
    state.I, state.SP, state.PC, opcode);
    nbCycles+=1;
}