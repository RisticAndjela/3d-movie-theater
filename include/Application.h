#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

class Application {
public:
    Application(int width, int height);
    void run();
    unsigned int overlayShader = 0;
    unsigned int overlayVAO = 0;
    bool overlayInitialized = false;

private:
    int width;
    int height;
    GLFWwindow* window = nullptr;

    Camera camera;
    double lastFrameTime;

	void processInput(double deltaTime);
};
