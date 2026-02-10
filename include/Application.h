#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "figures3D/Seat3D.h"

class Application {
public:
    Application(int width, int height);
    void run();
    unsigned int overlayShader = 0;
    unsigned int overlayVAO = 0;
    bool overlayInitialized = false;
	unsigned int seatShader = 0;

private:
    int width;
    int height;
    std::vector<Seat3D> seats3D;
    GLFWwindow* window = nullptr;

    Camera camera;
    double lastFrameTime;

	void processInput(double deltaTime);
    void initSeats();
};
