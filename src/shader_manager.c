#include <glad/glad.h>
#include <glfw3.h>

static const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aTexCoord;\n"
    "out vec2 texCoord;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);\n"
    "   texCoord=aTexCoord;\n"
    "}";

static const char* fragmentShaderSource =
    "#version 330 core\n"
    "in vec2 texCoord;\n"
    "out vec4 fragColor;\n"
    "uniform sampler2D tex;\n"
    "void main() {\n"
    "   fragColor = texture(tex, texCoord);\n"
    "}";

static unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    return id;
}

static unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {

    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

unsigned int getShaderProgram() {
    return createShaderProgram(vertexShaderSource, fragmentShaderSource);
}