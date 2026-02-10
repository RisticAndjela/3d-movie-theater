#include "Seat3D.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Renderer.h>
#include "../model/Seat.h"

// Staticki clanovi
unsigned int Seat3D::cubeVAO = 0;
unsigned int Seat3D::cubeVBO = 0;

// Default konstruktor
Seat3D::Seat3D()
    : position(0.0f), status(SeatStatus::Free), size(0.5f, 0.5f, 0.5f) {
}

// Konstruktor sa parametrima
Seat3D::Seat3D(const glm::vec3& pos, SeatStatus s)
    : position(pos), status(s), size(0.5f, 0.5f, 0.5f) {
}

glm::vec3 Seat3D::getColor() const {
    // primer boja po statusu
    switch (status) {
    case SeatStatus::Free: return glm::vec3(0.0f, 1.0f, 0.0f);
    case SeatStatus::Reserved: return glm::vec3(1.0f, 1.0f, 0.0f);
    case SeatStatus::Bought: return glm::vec3(1.0f, 0.0f, 0.0f);
    default: return glm::vec3(0.5f, 0.5f, 0.5f);
    }
}

// Vraca model matricu za kocku
glm::mat4 Seat3D::getModelMatrix() const {
    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);
    return model;
}

// Vraca boju sedista po statusu
glm::vec3 Seat3D::getSeatColor(const Seat* s) {
    switch (s->status) {
    case SeatStatus::Free: return glm::vec3(0.2f, 0.4f, 0.9f);
    case SeatStatus::Reserved: return glm::vec3(0.9f, 0.9f, 0.2f);
    case SeatStatus::Bought: return glm::vec3(0.9f, 0.2f, 0.2f);
    default: return glm::vec3(0.2f, 0.4f, 0.9f);
    }
}

// Crta kocku (sediste)
void Seat3D::draw(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection) const {
    glUseProgram(shaderProgram);

    glm::mat4 model = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::vec3 color = getColor();
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), color.r, color.g, color.b);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// Inicijalizacija staticke kocke
void Seat3D::initCube() {
    if (cubeVAO != 0) return; // vec inicijalizovan

    float vertices[] = {
        // prednja strana
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
         // zadnja strana
         -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
          0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
          // leva strana
          -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
          -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
          // desna strana
           0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,
           0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
           // gornja strana
           -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
            // donja strana
            -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f
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

// Render helper za sediste
void Seat3D::renderCube(unsigned int shaderProgram, const glm::vec3& pos, const glm::vec3& color, const glm::vec3& scale) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::scale(model, scale);

    // promenjeno: "uModel" -> "model", "uColor" -> "color"
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), color.r, color.g, color.b);

    glBindVertexArray(Seat3D::cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// Render helper za osobu
void Seat3D::renderPerson(unsigned int shaderProgram, const glm::vec3& pos, const glm::vec3& bodyColor) {
    // telo
    renderCube(shaderProgram, pos + glm::vec3(0.0f, 0.6f, 0.0f), bodyColor, glm::vec3(0.3f, 0.6f, 0.3f));
    // glava
    renderCube(shaderProgram, pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.8f, 0.6f), glm::vec3(0.3f, 0.3f, 0.3f));
}
