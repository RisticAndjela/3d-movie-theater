#pragma once
void InitOverlayRectangle();
void DrawOverlayRectangle(unsigned int shaderProgram, unsigned int vao);
void DrawOverlayText(unsigned int shaderProgram, const char* text, float x, float y, int windowWidth, int windowHeight);

unsigned int CreateShaderProgram(const char* vertexPath, const char* fragmentPath);
unsigned int GetOverlayRectangleVAO();