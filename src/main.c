#include <glad.h>
#include <glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include <graphics.h>
#include <input.h>
#include <chip8.h>

#include <windows.h>


#define INSTRUCTIONS_PER_FRAME 50 //multiply by the framerate (i.e. 60) to get the number of instructions per second


/*TO DO:
    - correct quad size to correspond to CHIP-8 screen ration */


/* functions whose name begin with "graphics" are from the graphics.c module ;
same with "input" and "chip8" */

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "[main] ERROR: expected format: %s <filepath>\n", argv[0]);
        return 1;
    }

    graphicsInit();
    inputInit();
    chip8Init(argv[1]);

    while (!inputShouldClose()) {

        double startTime = glfwGetTime();

        glfwPollEvents();
        processInput();
        //chip8Update();
        chip8ExecuteInstructions(INSTRUCTIONS_PER_FRAME);

        if (graphicsDidFrameChange()) {
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