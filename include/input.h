#pragma once

#include <glfw3.h>
#include<glad/glad.h>

void inputInit(void);
void processInput(void);
bool inputShouldClose(void);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);