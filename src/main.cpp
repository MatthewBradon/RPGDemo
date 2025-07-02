#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <iostream>
#include "EngineLibrary.h"
#include "input.h"
#include "game.cpp"

int main(int argc, char** argv) {

    BumpAllocator transientStorage = createBumpAllactor(MB(50));

    input.screenWidth = 540;
    input.screenHeight = 960;

    // Initialize GLFW
    GLFWwindow* window = initializeGLFW("OpenGL UI Example");
    
    initializeOpenGL(transientStorage);


    // Main loop
    while (!glfwWindowShouldClose(window)) {

        updateGame();
        render(window);
    }

    glfwTerminate();

    // Free transient storage
    free(transientStorage.memory);
    transientStorage.memory = nullptr;
    return 0;
}
