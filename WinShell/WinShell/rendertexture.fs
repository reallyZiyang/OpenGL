#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    vec4 texColor = texture(screenTexture, TexCoords);
    //texColor.rgb = vec3(1.0) - texColor.rgb;//反色
    //float average = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
    //texColor.rgb = vec3(average);//黑白
    FragColor = texColor;
}