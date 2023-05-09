#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 fragmentPos;
out vec3 fragmentNormal;
out vec2 fragmentTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragmentPos = vec3(model * vec4(aPos, 1.0));
    fragmentNormal = normalize(mat3(transpose(inverse(model))) * normal);
    fragmentTexCoord = texCoord;
}