#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 color;

out vec3 fragColor;
out vec2 vTexCoord;

void main()
{
    fragColor = color;
    vTexCoord = texCoord;
    gl_Position = projection * view * model * vec4(position, 1.0);
}