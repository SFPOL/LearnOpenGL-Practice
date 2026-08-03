#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
    gl_Position = view * model * vec4(aPos, 1.0); 
    vs_out.normal = normalize(normalMatrix * aNormal);
}