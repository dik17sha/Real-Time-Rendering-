#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

out vec3 worldPos;
out vec3 normal;

void main()
{
    vec4 worldPos4 = model * vec4(aPos, 1.0);

    worldPos = worldPos4.xyz;

    normal = normalMatrix * aNormal;

    gl_Position = (projection * view) * worldPos4;
}
