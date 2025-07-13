#pragma once

#include <glfw3.h>

extern const double TARGET_FPS;

GLFWwindow* graphicsGetWindow(void);

void graphicsSetFrameChanged(bool);
bool graphicsDidFrameChange(void);

int graphicsInit(void);
void graphicsUpdate(void);
void graphicsTerminate(void);