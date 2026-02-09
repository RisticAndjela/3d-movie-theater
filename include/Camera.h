#pragma once
#include <glm/glm/glm.hpp>

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
    void processKeyboard(float forward, float right, float deltaTime);
    void processMouse(float xoffset, float yoffset);

    void clampToBounds();

};
