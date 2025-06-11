#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
// ######################################################################
//                              Defines
// ######################################################################
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#endif
#ifdef __linux__
#define DEBUG_BREAK() __builtin_trap()
#endif
#ifdef __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

// ######################################################################
//                              Logging 
// ######################################################################

enum TextColor
{  
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
long long getTimestamp(char* file){
    struct stat fileStat = {};
    stat(file, &fileStat);
    return fileStat.st_mtime;
}

bool fileExists(char* filePath) {
    ENGINE_ASSERT(filePath, "filePath is null");

    auto file = fopen(filePath, "rb");

    if(!file) {
        return false;
    }

    fclose(file);

    return true;
}


long getFileSize(char* filePath) {
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

/*
Read File into Buffer
*/
char* readFile(char* filePath, int* fileSize, char* buffer){
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


bool copyFile(char* fileName, char* outputName, char* buffer) {
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

bool copyFile(char* fileName, char* outputName, BumpAllocator* allocator) {
    char* file = 0;
    long fileSize = getFileSize(fileName);

    if(fileSize){
        char* buffer = bumpAllocate(allocator, fileSize + 1); // +1 for null terminator
        
        return copyFile(fileName, outputName, buffer);
    }

    return false;
}