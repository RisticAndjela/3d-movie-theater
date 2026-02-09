#include "OverlayRectangle.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static float vertices[] = {
    -1.0f,  0.9f,   // top-left (leva ivica, vrh)
    -0.7f,  0.9f,   // top-right
    -0.7f,  0.7f,   // bottom-right
    -1.0f,  0.7f    // bottom-left
};


static unsigned int indices[] = {
    0, 1, 2, // prvi trougao
    2, 3, 0  // drugi trougao
};

static unsigned int rectangleVAO = 0, rectangleVBO = 0, rectangleEBO = 0;

void InitOverlayRectangle()
{
    if (rectangleVAO) return;

    glGenVertexArrays(1, &rectangleVAO);
    glGenBuffers(1, &rectangleVBO);
    glGenBuffers(1, &rectangleEBO);

    glBindVertexArray(rectangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangleEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void DrawOverlayRectangle(unsigned int shaderProgram, unsigned int vao)
{
    // Prethodno: glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(shaderProgram);

    // Setuj boju i alpha kroz uniform (npr. uniform vec4 u fragment shaderu)
    int colorLoc = glGetUniformLocation(shaderProgram, "inColor");
    glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 0.5f); // crveno sa alpha 0.7

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

unsigned int GetOverlayRectangleVAO() { return rectangleVAO; }

unsigned int CreateShaderProgram(const char* vertPath, const char* fragPath) {
    std::cout << "Vert shader path: " << vertPath << std::endl;
    std::cout << "Frag shader path: " << fragPath << std::endl;

    // 1. Učitaj vertex shader tekst
    std::ifstream vFile(vertPath), fFile(fragPath);
    std::string vertCode((std::istreambuf_iterator<char>(vFile)), std::istreambuf_iterator<char>());
    std::string fragCode((std::istreambuf_iterator<char>(fFile)), std::istreambuf_iterator<char>());
    const char* vShaderCode = vertCode.c_str();
    const char* fShaderCode = fragCode.c_str();

    unsigned int vertex, fragment, program;
    int success; char infoLog[512];

    // Vertex
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) { glGetShaderInfoLog(vertex, 512, NULL, infoLog); printf("Vertex shader error: %s\n", infoLog); }

    // Fragment
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) { glGetShaderInfoLog(fragment, 512, NULL, infoLog); printf("Fragment shader error: %s\n", infoLog); }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) { glGetProgramInfoLog(program, 512, NULL, infoLog); printf("Shader linking error: %s\n", infoLog); }
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

unsigned int textVAO = 0, textVBO = 0;

// inicijalizacija za tekst
void InitOverlayText() {
    if (textVAO) return;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// crtanje teksta
void DrawOverlayText(unsigned int shaderProgram, const char* text, float x, float y, int windowWidth, int windowHeight) {
    InitOverlayText();
    glDisable(GL_DEPTH_TEST);

    glUseProgram(shaderProgram);

    // postavi uniform boju
    int colorLoc = glGetUniformLocation(shaderProgram, "inColor");
    glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

    // kreiraj ortogonalnu matricu u pixelima
    glm::mat4 proj = glm::ortho(0.0f, float(windowWidth), 0.0f, float(windowHeight));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

    char buffer[9999];
    int quads = stb_easy_font_print(30, windowHeight - 50, (char*)"ANDJELA RISTIC", NULL, buffer, sizeof(buffer));

    float vertices[9999];
    for (int i = 0; i < quads * 6 * 2; i++)
        vertices[i] = ((short*)buffer)[i];

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, quads * 6 * 2 * sizeof(float), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, quads * 6);
    glBindVertexArray(0);
}
