#version 330 core
#define MAX_TEXTURE_SIZE 1
struct Material
{
    sampler2D diffuseTexture[MAX_TEXTURE_SIZE];
};
in vec2 fragTexCoord;

out vec4 FragColor;

uniform Material material;

void main()
{
    FragColor = texture(material.diffuseTexture[0], fragTexCoord);
}