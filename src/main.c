#include <glad.h>
#include <glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include <graphics.h>
#include <input.h>
#include <chip8.h>


/*TO DO:
    - correct quad size to correspond to CHIP-8 screen ration */


/* functions whose name begin with "graphics" are from the graphics.c module ;
same with "input" and "chip8" */

int main() {

    graphicsInit();
    inputInit();

    randomizeScreen(); //to remove later

    while (!inputShouldClose()) {

        processInput();
        chip8Update();
        if (graphicsDidFrameChange() == true) {
            graphicsUpdate();
            graphicsSetFrameChanged(false);
        } else {
            glfwWaitEventsTimeout(1.0 / TARGET_FPS);
        }

    }

    graphicsTerminate();

    return 0;
}