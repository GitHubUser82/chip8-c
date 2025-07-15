#include <glad.h>
#include <glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include <graphics.h>
#include <input.h>
#include <chip8.h>

#include <windows.h>


/*TO DO:
    - retrieve screen dimensions using OpenGL (instead of always assuming a 16:9 screen)
    - correct quad size to correspond to CHIP-8 screen ratio (1:2) */


/* functions whose name begin with "graphics" are from the graphics.c module ;
same with "input" and "chip8" */

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "[main] ERROR: expected format: %s <filepath>\n", argv[0]);
        return 1;
    }

    //graphicsInit should always be before inputInit, because the latter retrieves the GLFW window pointer from the graphics module
    graphicsInit();
    inputInit();
    chip8Init(argv[1]);

    while (!inputShouldClose()) {

        double startTime = glfwGetTime();

        glfwPollEvents();
        processInput();

        if (chip8Update() != 0)
            return 1;

        if (graphicsDidFrameChange()==true) {
            graphicsUpdate();
            graphicsSetFrameChanged(false);
        }

        double elapsed = glfwGetTime() - startTime;
        double sleepTime = 1.0/TARGET_FPS - elapsed;
        if (sleepTime > 0.0)
            //glfwWaitEventsTimeout(sleepTime);
            Sleep(sleepTime * 1000);

    }

    graphicsTerminate();

    return 0;
}