#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <iostream>

#define WINDOW_WIDTH 540
#define WINDOW_HEIGHT 960

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

int main(int argc, char** argv) {
    // Init GLUT for text rendering
    glutInit(&argc, argv);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RPGDemo", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeLimits(window, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    int screen_width, screen_height;
    glfwGetFramebufferSize(window, &screen_width, &screen_height);
    glViewport(0, 0, screen_width, screen_height);

    // Set up 2D orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // --- TOP HALF: RGB Quad ---
        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(0, 0);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(WINDOW_WIDTH, 0);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT / 2);
        glColor3f(1.0f, 1.0f, 0.0f); glVertex2f(0, WINDOW_HEIGHT / 2);
        glEnd();

        // --- BOTTOM HALF: UI Text ---
        glColor3f(1.0f, 1.0f, 1.0f); // White text
        drawText(20, WINDOW_HEIGHT / 2 + 30, "HP: 100   MANA: 50");

        drawText(20, WINDOW_HEIGHT / 2 + 70, "> Attack");
        drawText(20, WINDOW_HEIGHT / 2 + 100, "  Defend");
        drawText(20, WINDOW_HEIGHT / 2 + 130, "  Inventory");

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
