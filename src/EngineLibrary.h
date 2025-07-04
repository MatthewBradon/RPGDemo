#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "input.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "renderer_interface.h"
#include "math.h"
#include <iostream>

// ######################################################################
//                              Defines
// ######################################################################
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#define DLL_EXPORT __declspec(dllexport)
#endif
#ifdef __linux__
#define DEBUG_BREAK() __builtin_trap()
#define DLL_EXPORT __attribute__((visibility("default")))

#endif
#ifdef __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#define DLL_EXPORT __attribute__((visibility("default")))

#endif



#pragma once

#ifdef _WIN32
#elif defined(__APPLE__) || defined(__linux__)
#else
  #define DLL_EXPORT extern "C"
#endif


#define BIT(x) (1 << (x))
#define KB(x) ((unsigned long long)1024 * x)
#define MB(x) ((unsigned long long)1024 * KB(x))
#define GB(x) ((unsigned long long)1024 * MB(x))


#define TEXTURE_PATH "assets/textures/texture_atlas.png"

// ######################################################################
//                              OpenGL Structs
// ######################################################################
struct GLContext {
    GLuint shaderProgram;
    GLuint VAO;
    GLuint textureID;
    GLuint transformSBOID;
    GLuint screenSizeID;
};

static GLContext glContext;

// ######################################################################
//                              Logging 
// ######################################################################

enum TextColor {  
  TEXT_COLOR_BLACK,
  TEXT_COLOR_RED,
  TEXT_COLOR_GREEN,
  TEXT_COLOR_YELLOW,
  TEXT_COLOR_BLUE,
  TEXT_COLOR_MAGENTA,
  TEXT_COLOR_CYAN,
  TEXT_COLOR_WHITE,
  TEXT_COLOR_BRIGHT_BLACK,
  TEXT_COLOR_BRIGHT_RED,
  TEXT_COLOR_BRIGHT_GREEN,
  TEXT_COLOR_BRIGHT_YELLOW,
  TEXT_COLOR_BRIGHT_BLUE,
  TEXT_COLOR_BRIGHT_MAGENTA,
  TEXT_COLOR_BRIGHT_CYAN,
  TEXT_COLOR_BRIGHT_WHITE,
  TEXT_COLOR_COUNT
};


template<typename ... Args>
void _log(char* prefix, char* msg, TextColor textColor, Args ... args) {
    static char* TextColorTable[TEXT_COLOR_COUNT] = 
    {    
        "\x1b[30m", // TEXT_COLOR_BLACK
        "\x1b[31m", // TEXT_COLOR_RED
        "\x1b[32m", // TEXT_COLOR_GREEN
        "\x1b[33m", // TEXT_COLOR_YELLOW
        "\x1b[34m", // TEXT_COLOR_BLUE
        "\x1b[35m", // TEXT_COLOR_MAGENTA
        "\x1b[36m", // TEXT_COLOR_CYAN
        "\x1b[37m", // TEXT_COLOR_WHITE
        "\x1b[90m", // TEXT_COLOR_BRIGHT_BLACK
        "\x1b[91m", // TEXT_COLOR_BRIGHT_RED
        "\x1b[92m", // TEXT_COLOR_BRIGHT_GREEN
        "\x1b[93m", // TEXT_COLOR_BRIGHT_YELLOW
        "\x1b[94m", // TEXT_COLOR_BRIGHT_BLUE
        "\x1b[95m", // TEXT_COLOR_BRIGHT_MAGENTA
        "\x1b[96m", // TEXT_COLOR_BRIGHT_CYAN
        "\x1b[97m", // TEXT_COLOR_BRIGHT_WHITE
    };

    char formatBuffer[8192] = {};
    sprintf(formatBuffer, "%s %s %s \033[0m", TextColorTable[textColor], prefix, msg);

    char textBuffer[8192] = {};
    sprintf(textBuffer, formatBuffer, args...);

    puts(textBuffer);
}

#define ENGINE_TRACE(msg, ...) _log("TRACE: ", msg, TEXT_COLOR_BRIGHT_GREEN, ##__VA_ARGS__)
#define ENGINE_WARN(msg, ...) _log("WARN: ", msg, TEXT_COLOR_BRIGHT_YELLOW, ##__VA_ARGS__)
#define ENGINE_ERROR(msg, ...) _log("ERROR: ", msg, TEXT_COLOR_BRIGHT_RED, ##__VA_ARGS__)
#define ENGINE_INFO(msg, ...) _log("INFO: ", msg, TEXT_COLOR_BRIGHT_CYAN, ##__VA_ARGS__)

#define ENGINE_ASSERT(condition, msg, ...) { \
    if (!(condition)) { \
        ENGINE_ERROR(msg, ##__VA_ARGS__); \
        DEBUG_BREAK(); \
        ENGINE_ERROR("Assertion hit"); \
    } \
} 


// ######################################################################
//                              Bump Allocator 
// ######################################################################

struct BumpAllocator
{
    size_t size;
    size_t used;
    char* memory;
};

BumpAllocator createBumpAllactor(size_t size){
    BumpAllocator allocator = {};

    allocator.memory = (char*)malloc(size);

    if (!allocator.memory) {
        ENGINE_ASSERT(false, "Failed to allocate memory for BumpAllocator");
    }

    allocator.size = size;
    memset(allocator.memory, 0, size);

    return allocator;
}

char* bumpAllocate(BumpAllocator* allocator, size_t size) {
    char* result = nullptr;

    size_t allignedSize = (size + 7) & ~7;
    if (allocator->used + allignedSize <= allocator->size) {
        result = allocator->memory + allocator->used;
        allocator->used += allignedSize;
    } else {
        ENGINE_ASSERT(false, "BumpAllocator out of memory");
    }


    return result;
}

// ######################################################################
//                              File I/O 
// ######################################################################

long long getTimestamp(const char* file){
    struct stat fileStat = {};
    stat(file, &fileStat);
    return fileStat.st_mtime;
}

bool fileExists(const char* filePath) {
    ENGINE_ASSERT(filePath, "filePath is null");

    auto file = fopen(filePath, "rb");

    if(!file) {
        return false;
    }

    fclose(file);

    return true;
}


long getFileSize(const char* filePath) {
    ENGINE_ASSERT(filePath, "filePath is null");

    long fileSize = 0;
    auto file = fopen(filePath, "rb");
    if(!file) {
        ENGINE_ERROR("Failed to open file: %s", filePath);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);

    fseek(file, 0, SEEK_SET);
    fclose(file);

    return fileSize;
}

char* readFile(const char* filePath, int* fileSize, char* buffer){
    ENGINE_ASSERT(filePath, "filePath is null");
    ENGINE_ASSERT(fileSize, "fileSize is null");
    ENGINE_ASSERT(buffer, "buffer is null");

    *fileSize = 0;
    auto file = fopen(filePath, "rb");
    if (!file) {
        ENGINE_ERROR("Failed to open file: %s", filePath);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    memset(buffer, 0, *fileSize + 1); // +1 for null terminator
    fread(buffer, sizeof(char), *fileSize, file);

    fclose(file);

    return buffer;
}


char* readFile(char* filePath, int* fileSize, BumpAllocator* allocator) {
    char* file = nullptr;
    long fileSizeLocal = getFileSize(filePath);

    if(fileSizeLocal) {
        char* buffer = bumpAllocate(allocator, fileSizeLocal + 1); // +1 for null terminator

        file = readFile(filePath, fileSize, buffer);
    }

    return file;
}

void writeFile(char* filePath, char* data, int dataSize) {
    ENGINE_ASSERT(filePath, "filePath is null");
    ENGINE_ASSERT(data, "data is null");

    auto file = fopen(filePath, "wb");
    if (!file) {
        ENGINE_ERROR("Failed to open file for writing: %s", filePath);
        return;
    }

    fwrite(data, sizeof(char), dataSize, file);
    fclose(file);
}


bool copyFile(const char* fileName, const char* outputName, char* buffer) {
    int fileSize = 0;
    char* data = readFile(fileName, &fileSize, buffer);

    auto outputFile = fopen(outputName, "wb");

    if (!outputFile) {
        ENGINE_ERROR("Failed to open output file for writing: %s", outputName);
        return false;
    }

    int result = fwrite(data, sizeof(char), fileSize, outputFile);
    fclose(outputFile);

    if (result != fileSize) {
        ENGINE_ERROR("Failed to write all data to output file: %s", outputName);
        return false;
    }

    return true;
}

bool copyFile(const char* fileName, const char* outputName, BumpAllocator* allocator) {
    char* file = 0;
    long fileSize = getFileSize(fileName);

    if(fileSize){
        char* buffer = bumpAllocate(allocator, fileSize + 1); // +1 for null terminator
        
        return copyFile(fileName, outputName, buffer);
    }

    return false;
}


// ######################################################################
//                              OpenGL
// ######################################################################

GLFWwindow* initializeGLFW(const char* title) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    const char* glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(input->screenWidth, input->screenHeight, "RPGDemo", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwSetWindowSizeLimits(window, input->screenWidth, input->screenHeight, input->screenWidth, input->screenHeight);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return nullptr;
    }

    
    glViewport(0, 0, input->screenWidth, input->screenHeight);

    return window;
}

bool initializeOpenGL(BumpAllocator& transientStorage) {
    
    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    int fileSize = 0;
    char* vertShader = readFile("assets/shaders/quad.vert", &fileSize, &transientStorage);
    char* fragShader = readFile("assets/shaders/quad.frag", &fileSize, &transientStorage);

    if(!vertShader || !fragShader) {
        ENGINE_ERROR("Failed to read shader files");
        glfwTerminate();
        return false;
    }

    glShaderSource(vertShaderID, 1, &vertShader, 0);
    glShaderSource(fragShaderID, 1, &fragShader, 0);

    glCompileShader(vertShaderID);
    glCompileShader(fragShaderID);


    {
        int success;
        char shaderLog[2048] = {};

        glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &success);

        if(!success) {
            glGetShaderInfoLog(vertShaderID, sizeof(shaderLog), 0, shaderLog);
            ENGINE_ASSERT(false, "Vertex shader compilation failed: %s", shaderLog);
            return false;
        }
    }

    {
        int success;
        char shaderLog[2048] = {};

        glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &success);

        if(!success) {
            glGetShaderInfoLog(fragShaderID, sizeof(shaderLog), 0, shaderLog);
            ENGINE_ASSERT(false, "Fragment shader compilation failed: %s", shaderLog);
            return false;
        }
    }

    

    glContext.shaderProgram = glCreateProgram();
    glAttachShader(glContext.shaderProgram, vertShaderID);
    glAttachShader(glContext.shaderProgram, fragShaderID);
    glLinkProgram(glContext.shaderProgram);

    glDetachShader(glContext.shaderProgram, vertShaderID);
    glDetachShader(glContext.shaderProgram, fragShaderID);
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);


    glGenVertexArrays(1, &glContext.VAO);
    glBindVertexArray(glContext.VAO);


    {
        int width, height, channels;
        char * data = (char*)stbi_load(TEXTURE_PATH, &width, &height, &channels, 4);

        if(!data){
            ENGINE_ASSERT(false, "Failed to load texture: %s", TEXTURE_PATH);
            return false;
        }

        // Generate and bind the texture
        glGenTextures(1, &glContext.textureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glContext.textureID);


        // Set the texture wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
        stbi_image_free(data);
    }

    // Create a Vertex Buffer Object (VBO) for the vertex data
    {
        glGenBuffers(1, &glContext.transformSBOID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, glContext.transformSBOID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * MAX_TRANSFORMS, renderData->transforms, GL_DYNAMIC_DRAW);

    }

    // Uniforms
    {
        glContext.screenSizeID = glGetUniformLocation(glContext.shaderProgram, "screenSize");
        if (glContext.screenSizeID == -1) {
            ENGINE_ERROR("Failed to get uniform location for screenSize");
            return false;
        }
        
    }
    
    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(GL_MULTISAMPLE);

    // Depth Testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext.shaderProgram);

    return true;
}


bool render(GLFWwindow* window) {
    glfwPollEvents();
    glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, input->screenWidth, input->screenHeight);


    Vec2 screenSize = Vec2((float)input->screenWidth, (float)input->screenHeight);
    glUniform2fv(glContext.screenSizeID, 1, &screenSize.x);
    {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * renderData->transformCount, renderData->transforms);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData->transformCount);

        renderData->transformCount = 0; // Reset transform count after drawing
    }

    // Swap buffers
    glfwSwapBuffers(window);

    return true;
}