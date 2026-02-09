#include "HUD.h"
#include <glm/glm/gtc/matrix_transform.hpp>
#include <iostream>


void HUD::init() {
    hudShader = Shader("assets/shaders/hud_quad.vs", "assets/shaders/hud_quad.fs");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    float vertices[6 * 4] = {
        // x, y, u, v
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // pozicija
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1); // tex coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void HUD::draw(int screenWidth, int screenHeight) {
    glDisable(GL_DEPTH_TEST);
    hudShader.use();

    glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
    hudShader.setMat4("projection", projection);

    hudShader.setVec2("offset", glm::vec2(10.0f, screenHeight - 60.0f));
    hudShader.setVec2("scale", glm::vec2(400.0f, 50.0f));
    hudShader.setVec4("color", glm::vec4(0.0f, 0.0f, 1.0f, 0.5f));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

void HUD::renderText(const std::string& text, float x, float y, float scale, glm::vec4 color) {}
