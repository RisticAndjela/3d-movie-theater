#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "figures3D/Seat3D.h"
#include "../src/service/PersonManager.h"
#include "../src/service/SeatService.h"
#include "model/Person.h"
#include "../src/figures3D/Room3D.h"

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
    Room3D room;
    Screen screen;
    unsigned int LoadTexture(const char* path);

    glm::vec3 screenPointToWorldRay(double mouseX, double mouseY, const glm::mat4& view, const glm::mat4& projection);

    void onMouseClick(double mouseX, double mouseY);

    static bool rayIntersectsAABB(const glm::vec3& rayOrig, const glm::vec3& rayDir, const glm::vec3& minB, const glm::vec3& maxB);

    glm::vec3 seatToWorldLocal(int row, int col);

private:
    int width;
    int height;
    std::vector<Seat3D> seats3D;
    GLFWwindow* window = nullptr;

    Camera camera;
    double lastFrameTime;

    void processInput(double deltaTime);
    void initSeats();
    void drawSteps(float xStart, float xEnd, int rowIndex, float stepHeight, float spacingZ,const glm::mat4& view, const glm::mat4& projection);
};

