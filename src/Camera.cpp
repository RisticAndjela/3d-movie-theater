#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "figures3D/Seat3D.h"

Camera::Camera(): position(0.0f, 0.0f, 3.0f),yaw(-90.0f),pitch(0.0f), speed(5.0f), sensitivity(0.1f) {}

Camera::Camera(glm::vec3 startPos): position(startPos),yaw(-90.0f),pitch(0.0f),speed(5.0f),sensitivity(0.1f) {}

glm::mat4 Camera::getViewMatrix() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::lookAt(position, position + glm::normalize(front), glm::vec3(0, 1, 0));
}

void Camera::processKeyboard(float forward, float right, float dt) {
    glm::vec3 dir(cos(glm::radians(yaw)),0,sin(glm::radians(yaw)));

    glm::vec3 rightVec = glm::normalize(glm::cross(dir, glm::vec3(0, 1, 0)));

    position += dir * forward * speed * dt;
    position += rightVec * right * speed * dt;
}

void Camera::processMouse(float xoffset, float yoffset) {
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
}

void Camera::clampToBounds() {
    const float MIN_X = -20.0f;
    const float MAX_X = 20.0f;
    const float MIN_Y = 0.5f; 
    const float MAX_Y = 9.5f;
    const float MIN_Z = -30.0f;
    const float MAX_Z = 5.0f;

    if (position.x < MIN_X) position.x = MIN_X;
    if (position.x > MAX_X) position.x = MAX_X;

    if (position.y < MIN_Y) position.y = MIN_Y;
    if (position.y > MAX_Y) position.y = MAX_Y;

    if (position.z < MIN_Z) position.z = MIN_Z;
    if (position.z > MAX_Z) position.z = MAX_Z;
}


void Camera::clampToSeats(const std::vector<Seat3D>& seats) {
    float buffer = 0.3f; // koliko daleko od kvadra kamera sme da ide
    for (const auto& seat : seats) {
        glm::vec3 min = seat.position - seat.size * 0.5f - glm::vec3(buffer);
        glm::vec3 max = seat.position + seat.size * 0.5f + glm::vec3(buffer);

        if (position.x > min.x && position.x < max.x &&
            position.y > min.y && position.y < max.y &&
            position.z > min.z && position.z < max.z) {
            // kamera je unutar kvadra sedišta – vrati je napolje
            if (position.x - min.x < max.x - position.x) position.x = min.x;
            else position.x = max.x;

            if (position.z - min.z < max.z - position.z) position.z = min.z;
            else position.z = max.z;
        }
    }
}
