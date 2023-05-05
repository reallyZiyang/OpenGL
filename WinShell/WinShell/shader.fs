#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 ourTexCoord;

uniform vec4 uniformColor;
uniform sampler2D bgTexture;
uniform sampler2D fgTexture;
uniform float mixValue;

void main()
{
    //FragColor = uniformColor;;
    //FragColor = vec4(ourColor, 1.0f);
    //FragColor = texture(bgTexture, ourTexCoord);
    FragColor = mix(texture(bgTexture, ourTexCoord), texture(fgTexture, ourTexCoord), mixValue);

}