#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 texCoord;

layout (std140) uniform Matrices
{
    uniform mat4 view;
    uniform mat4 projection;
};

out vec2 TexCoords;

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = texCoord;
}