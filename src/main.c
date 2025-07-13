#include <glad.h>
#include <glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include <graphics.h>
#include <input.h>
#include <chip8.h>


/*TO DO:
    - cross-platform sleep
    - correct quad size to correspond to CHIP-8 screen ration*/


/* functions whose name begin with "graphics" are from the graphics.c module ;
same with "input" and "chip8" */

int main() {

    graphicsInit();
    inputInit();

    while (!graphicsShouldClose()) {
        processInput();
        chip8Update();
        graphicsUpdate();
    }

    graphicsTerminate();

    return 0;
}