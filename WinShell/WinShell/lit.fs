#version 330 core

struct DirectionLight
{
    vec3 dir;
    vec3 color;
    float ambientStrength;
    float diffuseStrength;
};

in vec3 fragmentPos;
in vec3 fragmentNormal;
in vec2 fragmentTexCoord;

out vec4 FragColor;

uniform DirectionLight directionLight;
uniform sampler2D diffuseTexture;

vec3 calDirectionLight(DirectionLight light, vec2 fragTexCoord, vec3 fragPos, vec3 fragNormal)
{
    vec3 texColor = vec3(texture(diffuseTexture, fragTexCoord));
    
    //1.环境光照
    vec3 ambientColor = texColor * light.color * light.ambientStrength;

    //2.漫反射
    vec3 lightDir = normalize(-light.dir);
    float diff = max(0.0f, dot(fragNormal, lightDir));
    vec3 diffuseColor = diff * texColor * light.color * light.diffuseStrength;

    return ambientColor + diffuseColor;
}


void main()
{
    vec3 dirColor = calDirectionLight(directionLight, fragmentTexCoord, fragmentPos, fragmentNormal);
    
    vec3 finalColor = dirColor;
    FragColor = vec4(finalColor, 1.0f);

}