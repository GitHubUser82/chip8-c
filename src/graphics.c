#include <glad.h>
#include <glfw3.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>

#elif defined(__linux__)
    #include <unistd.h>

#endif

#include <shader_manager.h>
#include <input.h>
#include <chip8.h>

static void chip8ScreenToRGBA(void);


//GLOBAL VARIABLES (accessible outside of this file)
const double TARGET_FPS = 60.0;


//STATIC VARIABLES (only used inside this file)

static const char* windowName = "CHIP-8 interpreter";
static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 720;
static const float clearColor[4] = {0.0f, 0.4f, 0.7f, 1.0f};
static GLFWwindow* window = NULL;

static bool frameChanged = true;

/*points to the chip8Screen bool array of the chip8 module,
and is initialized using the getChip8Screen of the chip8 module (in graphicsInit)*/
static bool (*chip8Screen)[CHIP8_DISPLAY_WIDTH];

//the raw RGBA bytes of the image representing the chip8Screen that will converted to a texture by OpenGL
unsigned char* screenBytes;

//colors used by the rendering system to represent ON and OFF pixels on the CHIP-8 screen
static float screenOnColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
static float screenOffColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

typedef struct {
    unsigned int shaderProgram;
    unsigned int vao;
    unsigned int screenQuadId; //vbo
    unsigned int ebo;
    unsigned int texture;
} OpenGlState;
static OpenGlState renderState;


//should be called after graphicsInit (if needed) by other modules to retrieve a pointer to the window
GLFWwindow* graphicsGetWindow() {
    return window;
}

bool graphicsDidFrameChange() {
    return frameChanged;
}

void graphicsSetFrameChanged(bool newValue) {
    frameChanged = newValue;
}

int graphicsInit() {
    chip8Screen = getChip8Screen();
    screenBytes = malloc(CHIP8_DISPLAY_HEIGHT * CHIP8_DISPLAY_WIDTH * 4 * sizeof(unsigned char));
    if (!screenBytes) {
        fprintf(stderr, "[graphics] ERROR in graphicsInit(): Failed to allocate screenBytes.\n");
        return -1;
    }   

    //part 1: initializing the GLFW window and GLAD (the library that will load OpenGL's functions)

    if (!glfwInit()) {
        fprintf(stderr, "[graphics] ERROR in graphicsInit: glfwInit failed\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowName, NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "[graphics] ERROR: failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowAspectRatio(window, 16, 9);

    glfwSwapInterval(1);

    //glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT);


    /* part2: initializing OpenGL's state to be able to draw the CHIP-8 screen: it will scale up
    a 64x32 raw RGBA image representing the CHIP-8 display to the whole window by applying
    it as a texture to a quad (two OpenGL triangles forming a rectangle) */

    const float screenQuad[] = {
        //positions     //texture coordinates
        -1.0f, -1.0f,   0.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 1.0f
    };
    const unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    //vao
    glGenVertexArrays(1, &(renderState.vao));
    glBindVertexArray(renderState.vao);

    //vbo and attributes
    glGenBuffers(1, &renderState.screenQuadId);
    glBindBuffer(GL_ARRAY_BUFFER, renderState.screenQuadId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuad), screenQuad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (const void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (const void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    //ebo
    glGenBuffers(1, &renderState.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderState.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //texture
    glGenTextures(1, &(renderState.texture));
    glBindTexture(GL_TEXTURE_2D, renderState.texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clearColor);  
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenBytes);

    //shader program
    renderState.shaderProgram = getShaderProgram();

    //setting up the OpenGL context for subsequent graphicsUpdate calls
    glBindVertexArray(renderState.vao);
    glBindTexture(GL_TEXTURE_2D, renderState.texture);
    glUseProgram(renderState.shaderProgram);

    return 0;
}

void graphicsUpdate() {

    chip8ScreenToRGBA();

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CHIP8_DISPLAY_WIDTH, CHIP8_DISPLAY_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBytes);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void*) 0);

    glfwSwapBuffers(window);
}

void graphicsTerminate() {

    free(screenBytes);

    //deletes data sent to GPU via OpenGL
    glDeleteVertexArrays(1, &renderState.vao);
    glDeleteBuffers(1, &renderState.screenQuadId);
    glDeleteBuffers(1, &renderState.ebo);
    glDeleteTextures(1, &renderState.texture);
    glDeleteProgram(renderState.shaderProgram);

    //terminates and closes the window
    glfwDestroyWindow(window);
    glfwTerminate();
}

/* updates screenBytes (RGBA buffer that will be passed as a texture to the OpenGL context)
using chip8Screen (bool array) */
static void chip8ScreenToRGBA() {
    for (unsigned int y = 0; y < CHIP8_DISPLAY_HEIGHT; y++) {
        for (unsigned int x = 0; x < CHIP8_DISPLAY_WIDTH; x++) {
            float* color = chip8Screen[y][x] ? screenOnColor : screenOffColor;
            int index = (y * CHIP8_DISPLAY_WIDTH + x) * 4;
                screenBytes[index + 0] = (unsigned char)(color[0] * 255.0f); // R
                screenBytes[index + 1] = (unsigned char)(color[1] * 255.0f); // G
                screenBytes[index + 2] = (unsigned char)(color[2] * 255.0f); // B
                screenBytes[index + 3] = (unsigned char)(color[3] * 255.0f); // A
        }
    }
}