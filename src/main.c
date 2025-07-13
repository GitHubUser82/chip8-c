#include <glad.h>
#include <glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include <graphics.h>
#include <input.h>
#include <chip8.h>

#include <windows.h>


/*TO DO:
    - correct quad size to correspond to CHIP-8 screen ration */


/* functions whose name begin with "graphics" are from the graphics.c module ;
same with "input" and "chip8" */

int main() {

    graphicsInit();
    inputInit();

    randomizeScreen(); //to remove later

    while (!inputShouldClose()) {

        double startTime = glfwGetTime();

        glfwPollEvents();
        processInput();
        chip8Update();

        if (graphicsDidFrameChange()) {
            graphicsUpdate(); // Upload texture + draw
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