#include "Seat3D.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Renderer.h>
#include "../model/Seat.h"

// Staticki clanovi
unsigned int Seat3D::cubeVAO = 0;
unsigned int Seat3D::cubeVBO = 0;
unsigned int Seat3D::seatTexture = 0;

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
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  0.5f, -0.5f,  0.5f, 1.0f, 0.0f,  0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f, 1.0f, -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
         // zadnja strana
         -0.5f, -0.5f, -0.5f,0.0f, 0.0f, -0.5f,  0.5f, -0.5f,1.0f, 0.0f,   0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f,0.0f, 0.0f,
          // leva strana
          -0.5f,  0.5f,  0.5f,0.0f, 0.0f, -0.5f,  0.5f, -0.5f,1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
          -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, -0.5f,  0.5f,  0.5f,0.0f, 0.0f,
          // desna strana
           0.5f,  0.5f,  0.5f,0.0f, 0.0f, 0.5f, -0.5f,  0.5f,1.0f, 0.0f,  0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
           0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.5f,  0.5f, -0.5f, 0.0f, 1.0f,  0.5f,  0.5f,  0.5f,0.0f, 0.0f,
           // gornja strana
           -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
            0.5f,  0.5f,  0.5f, 1.0f, 1.0f,  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, -0.5f,  0.5f, -0.5f,0.0f, 0.0f,
            // donja strana
            -0.5f, -0.5f, -0.5f,0.0f, 0.0f,  0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  0.5f, -0.5f,  0.5f, 1.0f, 1.0f,
             0.5f, -0.5f,  0.5f, 1.0f, 1.0f, -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texcoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Seat3D::renderCube(unsigned int shaderProgram, const glm::vec3& pos, const glm::vec3& color, const glm::vec3& scale) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::scale(model, scale);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), color.r, color.g, color.b);

    // aktiviraj i binduj teksturu
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Seat3D::seatTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "seatTex"), 0);

    glBindVertexArray(Seat3D::cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
void Seat3D::renderCube(unsigned int shaderProgram, const glm::vec3& pos, const glm::vec3& color, bool useTexture, const glm::vec3& scale) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::scale(model, scale);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), color.r, color.g, color.b);

    // postavi flag u shaderu
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), useTexture ? 1 : 0);

    if (useTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Seat3D::seatTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "seatTex"), 0);
    }
    else {
        // optional: odveži teksturu
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(Seat3D::cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
void Seat3D::renderPerson(unsigned int shaderProgram, const glm::vec3& pos, const glm::vec3& bodyColor) {
    // telo (bez teksture)
    renderCube(shaderProgram, pos + glm::vec3(0.0f, 0.6f, 0.0f), bodyColor, false, glm::vec3(0.3f, 0.6f, 0.3f));
    // glava (bez teksture)
    renderCube(shaderProgram, pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.8f, 0.6f), false, glm::vec3(0.3f, 0.3f, 0.3f));
}