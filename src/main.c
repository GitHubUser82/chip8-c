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

        double startTime = glfwGetTime();

        processInput();
        chip8Update();
        if (graphicsDidFrameChange()==true) {
            graphicsUpdate();
            graphicsSetFrameChanged(false);
        }

        double elapsedTime = glfwGetTime() - startTime;
        //the time that needs to be slept to maintain 60 FPS
        double sleepTime = (1.0 / TARGET_FPS) - elapsedTime;
        if (sleepTime > 0.0)
            glfwWaitEventsTimeout(sleepTime);

    }

    graphicsTerminate();

    return 0;
}