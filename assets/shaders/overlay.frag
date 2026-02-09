#version 330 core
out vec4 FragColor; // ovo je obavezno u core profilu
uniform vec4 inColor;

void main()
{
    FragColor = inColor;
}
