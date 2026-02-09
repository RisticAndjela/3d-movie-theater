#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>


static std::string loadFile(const char* path) {
    std::ifstream file(path, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Shader load error: ne mogu da otvorim fajl: " << path << std::endl;
        return std::string();
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static void printShaderLog(unsigned int shader) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        int length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string log(length, ' ');
        glGetShaderInfoLog(shader, length, nullptr, &log[0]);
        std::cerr << "Shader compile log:\n" << log << std::endl;
    }
}

static void printProgramLog(unsigned int program) {
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        int length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::string log(length, ' ');
        glGetProgramInfoLog(program, length, nullptr, &log[0]);
        std::cerr << "Program link log:\n" << log << std::endl;
    }
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vCode = loadFile(vertexPath);
    std::string fCode = loadFile(fragmentPath);

    if (vCode.empty() || fCode.empty()) {
        std::cerr << "Shader::Shader - prazni shader izvori (proveri putanje)" << std::endl;
        ID = 0;
        return;
    }

    const char* vSrc = vCode.c_str();
    const char* fSrc = fCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vSrc, nullptr);
    glCompileShader(vertex);
    printShaderLog(vertex);

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fSrc, nullptr);
    glCompileShader(fragment);
    printShaderLog(fragment);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    printProgramLog(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    if (ID != 0) glUseProgram(ID);
}


void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}