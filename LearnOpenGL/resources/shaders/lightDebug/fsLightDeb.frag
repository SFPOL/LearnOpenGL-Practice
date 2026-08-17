#version 420 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;

void main()
{
    float depth = texture(depthMap, TexCoords).r;
    FragColor = vec4(depth, depth, depth, 1.0);
}