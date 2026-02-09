#include "OverlayRectangle.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

static float vertices[] = {
    -0.9f,  0.9f,  // top-left
    -0.7f,  0.9f,  // top-right
    -0.7f,  0.7f,  // bottom-right
    -0.9f,  0.7f   // bottom-left
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
    glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 0.7f); // crveno sa alpha 0.7

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