#version 330 core
out vec4 FragColor;

in vec3 fragmentPos;
in vec3 fragmentNormal;
in vec2 fragmentTexCoord;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{             
    //折射率
    float radio = 1.00 / 1.52;
    vec3 I = normalize(fragmentPos - viewPos);
    //vec3 R = reflect(I, normalize(fragmentNormal));
    vec3 R = refract(I, normalize(fragmentNormal), radio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}