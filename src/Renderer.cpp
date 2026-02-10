#include "Renderer.h"
#include <glad/glad.h>

Renderer::Renderer() {
    float vertices[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,0.5f,-0.5f,
         0.5f,0.5f,-0.5f, -0.5f,0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

float quadVertices[] = {
    0.0f, 1.0f,    0.0f, 1.0f,
    1.0f, 0.0f,    1.0f, 0.0f,
    0.0f, 0.0f,    0.0f, 0.0f,

    0.0f, 1.0f,    0.0f, 1.0f,
    1.0f, 1.0f,    1.0f, 1.0f,
    1.0f, 0.0f,    1.0f, 0.0f
};

void Renderer::drawCube(const Shader& shader, const glm::mat4& mvp) {
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MVP"),1, GL_FALSE, &mvp[0][0]);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

