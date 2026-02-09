#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

class Renderer {
public:
    Renderer();
    void drawCube(const Shader& shader, const glm::mat4& mvp);

private:
    unsigned int VAO, VBO;
};
