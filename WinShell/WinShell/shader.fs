#version 330 core
in vec3 fragmentPos;
in vec3 fragmentNormal;

out vec4 FragColor;

uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    //1.环境光照
    float ambientStrength = 0.15f;
    vec3 ambientColor = ambientStrength * lightColor;

    //2.漫反射
    float diffussionStrength = 1.0f;
    vec3 lightDir = normalize(lightPos - fragmentPos);
    float diff = max(0.0f, dot(fragmentNormal, lightDir)) * diffussionStrength;
    vec3 diffussionColor = diff * lightColor;

    //3.镜面反射
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - fragmentPos);
    vec3 reflectDir = reflect(-lightDir, fragmentNormal);
    float spec = max(0.0f, dot(viewDir, reflectDir));
    float shininess = pow(spec, 64) * specularStrength;//反光度，幂越大光越聚集
    vec3 specularColor = shininess * lightColor;

    vec3 finalColor = (ambientColor + diffussionColor + specularColor) * objColor;
    FragColor = vec4(finalColor, 1.0f);


}