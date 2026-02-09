#pragma once
#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include "Shader.h"
#include <map>

struct Character {
    unsigned int TextureID;
    glm::ivec2   Size;
    glm::ivec2   Bearing;
    unsigned int Advance;
};

class HUD {
public:
    HUD(const char* vertexPath, const char* fragmentPath): hudShader(vertexPath, fragmentPath) {}          
    void init();               // inicijalizacija koja koristi OpenGL
    void draw(int screenWidth, int screenHeight);

private:
    Shader hudShader;
    unsigned int VAO = 0, VBO = 0;
    std::map<char, Character> Characters;
    void renderText(const std::string& text, float x, float y, float scale, glm::vec4 color);
};