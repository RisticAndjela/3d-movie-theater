#version 330 core

in vec3 fragColor;
in vec2 vTexCoord;
out vec4 FragColor;

uniform vec3 color;
uniform sampler2D seatTex;

void main()
{
    vec4 tex = texture(seatTex, vTexCoord);
    FragColor = vec4(fragColor, 1.0) * tex;
}