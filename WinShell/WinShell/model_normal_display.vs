#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normal;

layout (std140) uniform Matrices
{
    uniform mat4 view;
    uniform mat4 projection;
};
out vec4 inFragmentNormal;

uniform mat3 model;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    inFragmentNormal = normalize(vec4(normalMatrix * normal));
}