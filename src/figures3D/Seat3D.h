#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "../model/Seat.h"

class Seat3D {
public:
    glm::vec3 position;
    SeatStatus status;
    glm::vec3 size; // dimenzije kvadra

    Seat3D(const glm::vec3& pos, SeatStatus s = SeatStatus::Free);
    glm::mat4 getModelMatrix() const;
    glm::vec3 getColor() const;

    void draw(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection) const;


    static unsigned int cubeVAO;
    static unsigned int cubeVBO;
    static void initCube();
};
