#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <iostream>
#include "EngineLibrary.h"
#include "game.h"

#include "platform.h"
#ifdef _WIN32
#include "win32_platform.cpp"
const char* gameLibName = "game.dll";
const char* gameLoadLibName = "game-load.dll";
#endif


// #######################################################################################
//                                      Game DLL
// #######################################################################################
typedef decltype(updateGame) updateGameType;
static updateGameType* updateGamePtr;
void reloadGameDLL(BumpAllocator* transientStorage);

int main(int argc, char** argv) {

    BumpAllocator transientStorage = createBumpAllactor(MB(50));
    BumpAllocator persistantStorage = createBumpAllactor(MB(50));


    input = (Input*)bumpAllocate(&persistantStorage, sizeof(Input));

    if (!input) {
        ENGINE_ASSERT(false, "Failed to allocate memory for Input");
        return -1;
    }

    renderData = (RenderData*)bumpAllocate(&persistantStorage, sizeof(RenderData));

    if (!renderData) {
        ENGINE_ASSERT(false, "Failed to allocate memory for RenderData");
        return -1;
    }

    input->screenWidth = 540;
    input->screenHeight = 960;

    // Initialize GLFW
    GLFWwindow* window = initializeGLFW("OpenGL UI Example");
    
    initializeOpenGL(transientStorage);


    // Main loop
    while (!glfwWindowShouldClose(window)) {

        reloadGameDLL(&transientStorage);

        updateGame(renderData, input);
        render(window);

        transientStorage.used = 0;
    }

    glfwTerminate();

    // Free transient storage
    free(transientStorage.memory);
    transientStorage.memory = nullptr;
    return 0;
}

void updateGame(RenderData* renderDataIn, Input* inputIn) {
    updateGamePtr(renderDataIn, inputIn);
}

void reloadGameDLL(BumpAllocator* transientStorage)
{
    static void* gameDLL;
    static long long lastEditTimestampGameDLL;

    long long currentTimestampGameDLL = getTimestamp(gameLibName);
    if(currentTimestampGameDLL > lastEditTimestampGameDLL)
    {
        if(gameDLL) {
            bool freeResult = platformFreeDynamicLibrary(gameDLL);
            ENGINE_ASSERT(freeResult, "Failed to free %s", gameLibName);
            gameDLL = nullptr;
            ENGINE_TRACE("Freed %s", gameLibName);
        }

        while(!copyFile(gameLibName, gameLoadLibName, transientStorage)) {
            ENGINE_ERROR("Failed to copy %s into %s, retrying...", gameLibName, gameLoadLibName);
            Sleep(10);
        }
        ENGINE_TRACE("Copied %s into %s", gameLibName, gameLoadLibName);

        gameDLL = platformLoadDynamicLibrary(gameLoadLibName);
        ENGINE_ASSERT(gameDLL, "Failed to load %s", gameLoadLibName);

        updateGamePtr = (updateGameType*)platformLoadDynamicFunction(gameDLL, "updateGame");
        ENGINE_ASSERT(updateGamePtr, "Failed to load updateGame function");


        lastEditTimestampGameDLL = currentTimestampGameDLL;
    }
}