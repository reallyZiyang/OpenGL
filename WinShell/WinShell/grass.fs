#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D baseTexture;

void main()
{
	vec4 texColor = texture(baseTexture, TexCoords);
	FragColor = texColor;
}