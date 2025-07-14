#include <glad.h>
#include <glfw3.h>

#include <graphics.h>

#include <stdio.h>
#include <stdbool.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

static GLFWwindow* window = NULL;

void inputInit() {

    /* simultaneously retrieves the window from the graphics module and compares the GLFW window pointer returned with NULL
    to check for errors */
    if ((window=graphicsGetWindow())==NULL) {
        fprintf(stderr, "[ERROR] input: inputInit failed to retrieve window\n");
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

}

void processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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