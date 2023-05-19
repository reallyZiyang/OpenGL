#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout (std140) uniform Matrices
{
    uniform mat4 view;
    uniform mat4 projection;
};
out vec3 inFragmentPos;
out vec3 inFragmentNormal;
out vec2 inFragmentTexCoord;

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    inFragmentPos = vec3(model * vec4(aPos, 1.0));
    inFragmentNormal = normalize(mat3(transpose(inverse(model))) * normal);
    inFragmentTexCoord = texCoord;
}