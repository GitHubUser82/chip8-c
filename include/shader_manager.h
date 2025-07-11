#pragma once

extern const char* vertexShaderSource; 
extern const char* fragmentShaderSource;
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);