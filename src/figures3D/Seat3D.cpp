
#include "Seat3D.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "../model/Seat.h"
#include <Renderer.h>

Seat3D::Seat3D(const glm::vec3& pos, SeatStatus s)
    : position(pos), status(s), size(0.5f, 0.5f, 0.5f) {
}

glm::mat4 Seat3D::getModelMatrix() const {
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);
    return model;
}

glm::vec3 Seat3D::getColor() const {
    switch (status) {
    case SeatStatus::Free: return glm::vec3(0.0f, 0.0f, 1.0f);    // plavo
    case SeatStatus::Reserved: return glm::vec3(1.0f, 1.0f, 0.0f); // žuto
    case SeatStatus::Bought: return glm::vec3(1.0f, 0.0f, 0.0f);   // crveno
    }
    return glm::vec3(1.0f);
}

void Seat3D::draw(unsigned int shaderProgram) const {
    glUseProgram(shaderProgram);

    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

    glm::vec3 color = getColor();
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), color.r, color.g, color.b);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


unsigned int Seat3D::cubeVAO = 0;
unsigned int Seat3D::cubeVBO = 0;

void Seat3D::initCube() {
    if (cubeVAO != 0) return; // već inicijalizovan

    float vertices[] = {
        // prednja strana (z = 0.5)
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // zadnja strana (z = -0.5)
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // leva strana (x = -0.5)
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        // desna strana (x = 0.5)
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,

         // gornja strana (y = 0.5)
         -0.5f,  0.5f, -0.5f,
         -0.5f,  0.5f,  0.5f,
          0.5f,  0.5f,  0.5f,
          0.5f,  0.5f,  0.5f,
          0.5f,  0.5f, -0.5f,
         -0.5f,  0.5f, -0.5f,

         // donja strana (y = -0.5)
         -0.5f, -0.5f, -0.5f,
          0.5f, -0.5f, -0.5f,
          0.5f, -0.5f,  0.5f,
          0.5f, -0.5f,  0.5f,
         -0.5f, -0.5f,  0.5f,
         -0.5f, -0.5f, -0.5f
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

