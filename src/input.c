#include <glad/glad.h>
#include <glfw3.h>

#include <graphics.h>

#include <stdio.h>
#include <stdbool.h>

#include <chip8.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

static GLFWwindow* window = NULL;

static int keyBindings[16] = {
    GLFW_KEY_X, // 0
    GLFW_KEY_1, // 1
    GLFW_KEY_2, // 2
    GLFW_KEY_3, // 3
    GLFW_KEY_Q, // 4
    GLFW_KEY_W, // 5
    GLFW_KEY_E, // 6
    GLFW_KEY_A, // 7
    GLFW_KEY_S, // 8
    GLFW_KEY_D, // 9
    GLFW_KEY_Z, // A
    GLFW_KEY_C, // B
    GLFW_KEY_4, // C
    GLFW_KEY_R, // D
    GLFW_KEY_F, // E
    GLFW_KEY_V  // F
};

void inputInit() {

    /* simultaneously retrieves the window from the graphics module and compares the GLFW window pointer returned with NULL
    to check for errors */
    if ((window=graphicsGetWindow())==NULL) {
        fprintf(stderr, "[ERROR] input: inputInit failed to retrieve window\n");
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

}

void processInput() {
    //static int ticks = 0;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool keypadState[16];
    for (int i = 0; i < 16; i++) {
        keypadState[i]=glfwGetKey(window, keyBindings[i])==GLFW_PRESS;
        
        /*
        if (ticks==60) {
            if (state==GLFW_PRESS)
                printf("%x ", i);
            else 
                printf(" ");
        }
        */
    }

    /*
    if (ticks==60) {
        printf("\n");
        ticks=0;
    }
    ticks++;
    */

    chip8UpdateKeypadState(keypadState);
}

bool inputShouldClose(void) {
    return glfwWindowShouldClose(window);
}


void framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    glfwMakeContextCurrent(window);

    glClear(GL_COLOR_BUFFER_BIT);

    /* makes sure the viewport (OpenGL's drawing surface) matches the new window dimensions; note that width and 
    height will be significantly larger than specified on retina displays. */
    glViewport(0, 0, width, height);

    graphicsSetFrameChanged(true);

}