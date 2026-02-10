#version 330 core

in vec3 fragColor;
in vec2 vTexCoord;
out vec4 FragColor;

uniform vec3 color;
uniform sampler2D seatTex;
uniform int useTexture; // 0 = ne koristiti teksturu, 1 = koristiti

void main()
{
    if (useTexture == 1) {
        vec4 tex = texture(seatTex, vTexCoord);
        FragColor = vec4(fragColor, 1.0) * tex;
    } else {
        FragColor = vec4(fragColor, 1.0);
    }
}