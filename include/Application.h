#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "HUD.h"

class Application {
public:
    Application(int width, int height);
    void run();

private:
    int width;
    int height;
    GLFWwindow* window = nullptr;

    Camera camera;
    HUD hud;
    double lastFrameTime;

    void processInput(double deltaTime);
};
