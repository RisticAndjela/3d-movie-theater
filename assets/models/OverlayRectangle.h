#pragma once
void InitOverlayRectangle();
void DrawOverlayRectangle(unsigned int shaderProgram, unsigned int vao);
unsigned int CreateShaderProgram(const char* vertexPath, const char* fragmentPath);
unsigned int GetOverlayRectangleVAO();