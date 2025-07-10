#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

extern const char* vertexShaderSource; 
extern const char* fragmentShaderSource;
unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);

#endif