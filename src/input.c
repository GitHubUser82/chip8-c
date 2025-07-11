#include <glad.h>
#include <glfw3.h>

#include <graphics.h>

#include <stdio.h>
#include <stdbool.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

static GLFWwindow* window = NULL;

void inputInit() {
    if ((window=getWindow())==NULL) {
        fprintf(stderr, "[ERROR] input: inputInit failed to retrieve window\n");
    }
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

void processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // make sure the viewport (OpenGL's drawing surface) matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}