#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "../model/Seat.h"
#include "stb_image.h" 
class Seat3D {
public:
    glm::vec3 position;
    SeatStatus status;
    glm::vec3 size; // dimenzije kvadra

    Seat3D();
    Seat3D(const glm::vec3& pos, SeatStatus s = SeatStatus::Free);
    glm::mat4 getModelMatrix() const;
    glm::vec3 getColor() const;
    glm::vec3 getSeatColor(const Seat* s);
    void draw(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection) const;


    static unsigned int cubeVAO;
    static unsigned int cubeVBO;
    static unsigned int seatTexture;
    static void initCube();

    static void renderCube(unsigned int shaderProgram, const glm::vec3& pos, const glm::vec3& color, const glm::vec3& scale = glm::vec3(1.0f));
    static void renderPerson(unsigned int shaderProgram, const glm::vec3& pos, const glm::vec3& bodyColor);
};
