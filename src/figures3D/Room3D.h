#pragma once
#include <glm/glm.hpp>

class Room3D {
public:
    Room3D() = default;
    Room3D(const glm::vec3& minB, const glm::vec3& maxB);

    void setBounds(const glm::vec3& minB, const glm::vec3& maxB);
    void render(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection) const;

    glm::vec3 minB{ 0.0f };
    glm::vec3 maxB{ 0.0f };
};