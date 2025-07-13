#pragma once

#include <glfw3.h>
#include<glad.h>

GLFWwindow* graphicsGetWindow(void);
int graphicsInit(void);
void graphicsUpdate(void);
bool graphicsShouldClose(void);
void graphicsTerminate(void);