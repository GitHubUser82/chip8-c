#include <glad.h>
#include <glfw3.h>

#include <stdio.h>
#include <stdbool.h>

#include <graphics.h>
#include <input.h>


int main() {

    graphicsInit();
    inputInit();

    while (!graphicsShouldClose()) {
        processInput();
        graphicsUpdate();
    }

    graphicsTerminate();

    return 0;
}