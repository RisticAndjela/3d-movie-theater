#include "Room3D.h"
#include "Seat3D.h" // radi pristupa cubeVAO
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

Room3D::Room3D(const glm::vec3& minB_, const glm::vec3& maxB_) : minB(minB_), maxB(maxB_) {}

void Room3D::setBounds(const glm::vec3& minB_, const glm::vec3& maxB_) {
    minB = minB_;
    maxB = maxB_;
}

void renderBox(unsigned int shaderProgram, const glm::vec3& center, const glm::vec3& size, const glm::vec3& color) {
    glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), center);
    model = glm::scale(model, size);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &color[0]);

    glBindVertexArray(Seat3D::cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Room3D::render(unsigned int shaderProgram, const glm::mat4& view, const glm::mat4& projection) const {
    glUseProgram(shaderProgram);

    // floor
    glm::vec3 center = glm::vec3((minB.x + maxB.x) * 0.5f, minB.y - 0.01f, (minB.z + maxB.z) * 0.5f);
    glm::vec3 size = glm::vec3((maxB.x - minB.x), 0.02f, (maxB.z - minB.z));
    renderBox(shaderProgram, center, size, glm::vec3(0.3f, 0.25f, 0.2f));

    // ceiling
    center = glm::vec3((minB.x + maxB.x) * 0.5f, maxB.y + 0.01f, (minB.z + maxB.z) * 0.5f);
    size = glm::vec3((maxB.x - minB.x), 0.02f, (maxB.z - minB.z));
    renderBox(shaderProgram, center, size, glm::vec3(0.8f, 0.8f, 0.85f));

    // back wall (dalje od platna)
    center = glm::vec3((minB.x + maxB.x) * 0.5f, (minB.y + maxB.y) * 0.5f, minB.z - 0.01f);
    size = glm::vec3((maxB.x - minB.x), (maxB.y - minB.y), 0.02f);
    renderBox(shaderProgram, center, size, glm::vec3(0.35f, 0.35f, 0.4f));

    // left wall
    center = glm::vec3(minB.x - 0.01f, (minB.y + maxB.y) * 0.5f, (minB.z + maxB.z) * 0.5f);
    size = glm::vec3(0.02f, (maxB.y - minB.y), (maxB.z - minB.z));
    renderBox(shaderProgram, center, size, glm::vec3(0.35f, 0.35f, 0.4f));

    // right wall
    center = glm::vec3(maxB.x + 0.01f, (minB.y + maxB.y) * 0.5f, (minB.z + maxB.z) * 0.5f);
    renderBox(shaderProgram, center, size, glm::vec3(0.35f, 0.35f, 0.4f));

    if (screen && screen->hasFrames()) {
        glm::vec3 screenSize((maxB.x - minB.x) * 0.7f, (maxB.y - minB.y) * 0.5f, 1.0f);
        glm::vec3 screenCenter((minB.x + maxB.x) * 0.5f, minB.y + screenSize.y * 0.5f + 0.5f, minB.z + 0.001f);
        screen->render(shaderProgram, view, projection, screenCenter, screenSize);
    }
    else {
        // fallback: crtaj zid normalno
        center = glm::vec3((minB.x + maxB.x) * 0.5f, (minB.y + maxB.y) * 0.5f, minB.z - 0.01f);
        size = glm::vec3((maxB.x - minB.x), (maxB.y - minB.y), 0.02f);
        renderBox(shaderProgram, center, size, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}