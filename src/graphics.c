#include <glad.h>
#include <glfw3.h>

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <shader_manager.h>
#include <input.h>


//the static keyword makes the variable inaccessible from outside the file
static GLFWwindow* window = NULL;

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

//should be called after graphicsInit() to avoid returning a NULL pointer!
GLFWwindow* getWindow() {
    return window;
}

int graphicsInit() {

    if (!glfwInit()) {
        fprintf(stderr, "[graphics] ERROR in graphicsInit: glfwInit failed\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "[graphics] ERROR: failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowAspectRatio(window, 16, 9);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glClearColor(0.0f, 0.4f, 0.7f, 1.0f);

    return 0;
}

void graphicsUpdate() {

    glClear(GL_COLOR_BUFFER_BIT);

    /*glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);*/

    glfwSwapBuffers(window);
    glfwPollEvents();

}

bool graphicsShouldClose(void) {
    return glfwWindowShouldClose(window);
}

void graphicsTerminate() {
    glfwDestroyWindow(window);
    glfwTerminate();
}