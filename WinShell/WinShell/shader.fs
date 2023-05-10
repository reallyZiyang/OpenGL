#version 330 core
struct DirectionLight
{
    vec3 dir;
    vec3 color;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
};

struct PointLight
{
    vec3 pos;
    vec3 color;
    float diffuseStrength;
    float specularStrength;
};

struct SpotLight
{
    vec3 pos;
    vec3 dir;
    float insideCutoff;//聚光灯的内圈角度余弦值
    float outsideCutoff;//聚光灯的外圈角度余弦值
    vec3 color;
    float diffuseStrength;
    float specularStrength;
};

struct Material
{
    vec3 baseColor;//基础颜色
    sampler2D diffuseTexture;//漫反射贴图
    sampler2D specularTexture;//镜面反射贴图
    float shininess;//光滑程度
};

in vec3 fragmentPos;
in vec3 fragmentNormal;
in vec2 fragmentTexCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform DirectionLight directionLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 calDirectionLight(DirectionLight light, vec2 fragTexCoord, vec3 fragPos, vec3 fragNormal)
{
    vec3 texColor = vec3(texture(material.diffuseTexture, fragTexCoord));
    vec3 specularTexColor = vec3(texture(material.specularTexture, fragTexCoord));
    
    //1.环境光照
    vec3 ambientColor = material.baseColor * texColor * light.color * light.ambientStrength;

    //2.漫反射
    vec3 lightDir = normalize(-light.dir);
    float diff = max(0.0f, dot(fragNormal, lightDir));
    vec3 diffuseColor = diff * texColor * light.color * light.diffuseStrength;

    //3.镜面反射
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float spec = max(0.0f, dot(viewDir, reflectDir));
    float shininess = pow(spec, material.shininess);//反光度，幂越大光越聚集
    vec3 specularColor = shininess * specularTexColor * light.color * light.specularStrength;

    return ambientColor + diffuseColor + specularColor;
}

vec3 calPointLight(PointLight light, vec2 fragTexCoord, vec3 fragPos, vec3 fragNormal)
{
    vec3 texColor = vec3(texture(material.diffuseTexture, fragTexCoord));
    vec3 specularTexColor = vec3(texture(material.specularTexture, fragTexCoord));

    //2.漫反射
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(0.0f, dot(fragNormal, lightDir));
    vec3 diffuseColor = diff * texColor * light.color * light.diffuseStrength;

    //3.镜面反射
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float spec = max(0.0f, dot(viewDir, reflectDir));
    float shininess = pow(spec, material.shininess);//反光度，幂越大光越聚集
    vec3 specularColor = shininess * specularTexColor * light.color * light.specularStrength;

    float dis = length(light.pos - fragPos);
    //点光源距离衰减公式
    float value = 1.0 / (1 + 0.18 * dis + 0.12 * dis * dis);

    return (diffuseColor + specularColor) * value;
}

vec3 calSpotLight(SpotLight light, vec2 fragTexCoord, vec3 fragPos, vec3 fragNormal)
{
    vec3 texColor = vec3(texture(material.diffuseTexture, fragTexCoord));
    vec3 specularTexColor = vec3(texture(material.specularTexture, fragTexCoord));

    //2.漫反射
    vec3 lightDir = normalize(light.pos - fragPos);
    float diff = max(0.0f, dot(fragNormal, lightDir));
    vec3 diffuseColor = diff * texColor * light.color * light.diffuseStrength;

    //3.镜面反射
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float spec = max(0.0f, dot(viewDir, reflectDir));
    float shininess = pow(spec, material.shininess);//反光度，幂越大光越聚集
    vec3 specularColor = shininess * specularTexColor * light.color * light.specularStrength;

    float theta = dot(lightDir, normalize(-light.dir));
    float cutoffValue = (theta - light.insideCutoff) / (light.insideCutoff - light.outsideCutoff);
    cutoffValue = clamp(cutoffValue, 0.0f, 1.0f);

    return (diffuseColor + specularColor) * cutoffValue;
}

void main()
{
    vec3 dirColor = calDirectionLight(directionLight, fragmentTexCoord, fragmentPos, fragmentNormal);
    
    vec3 pointColor = vec3(0.0);
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        pointColor += calPointLight(pointLights[i], fragmentTexCoord, fragmentPos, fragmentNormal);
    
    vec3 spotColor = calSpotLight(spotLight, fragmentTexCoord, fragmentPos, fragmentNormal);
    
    vec3 finalColor = dirColor + pointColor + spotColor;
    FragColor = vec4(finalColor, 1.0f);

}