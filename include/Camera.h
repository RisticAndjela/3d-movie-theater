#pragma once
#include <glm/glm.hpp>
#include "../src/figures3D/Seat3D.h"
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 position;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;

    Camera();
    Camera(glm::vec3 startPos);

    glm::mat4 getViewMatrix();

    void clampToBounds();
    void clampToSeats(const std::vector<Seat3D>& seats);
    void clampToRoom(const glm::vec3 & minB, const glm::vec3 & maxB);
    void processKeyboard(float forward, float right, float deltaTime);
    void processRotationKeyboard(GLFWwindow* window, float deltaTime);
};