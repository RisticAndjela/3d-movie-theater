#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "figures3D/Seat3D.h"
#include "../src/service/PersonManager.h"
#include "../src/service/SeatService.h"
#include "model/Person.h"

class Application {
public:
    Application(int width, int height);
    void run();
    unsigned int overlayShader = 0;
    unsigned int overlayVAO = 0;
    bool overlayInitialized = false;
    unsigned int seatShader = 0;

    SeatService seatService{ 5, 10 };
    PersonManager* personManager = nullptr;

private:
    int width;
    int height;
    std::vector<Seat3D> seats3D;
    GLFWwindow* window = nullptr;

    Camera camera;
    double lastFrameTime;

    void processInput(double deltaTime);
    void initSeats();
    void drawSteps(float xStart, float xEnd, int rowIndex, float stepHeight, float spacingZ,
        const glm::mat4& view, const glm::mat4& projection);
};
