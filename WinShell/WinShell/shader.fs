#version 330 core
struct Light
{
    vec3 pos;
    vec3 dir;
    float cutoff;//聚光灯的内圈角度余弦值
    float outCutoff;//聚光灯的外圈角度余弦值
    vec3 color;
    float ambientStrength;
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
uniform Light light;
uniform Material material;

void main()
{
    vec3 texColor = vec3(texture(material.diffuseTexture, fragmentTexCoord));
    vec3 specularTexColor = vec3(texture(material.specularTexture, fragmentTexCoord));

    //1.环境光照
    vec3 ambientColor = material.baseColor * texColor * light.color * light.ambientStrength;

    //2.漫反射
    vec3 lightDir = normalize(light.pos - fragmentPos);
    //vec3 lightDir = normalize(-light.dir);
    float diff = max(0.0f, dot(fragmentNormal, lightDir));
    vec3 diffuseColor = diff * texColor * light.color * light.diffuseStrength;

    //3.镜面反射
    vec3 viewDir = normalize(viewPos - fragmentPos);
    vec3 reflectDir = reflect(-lightDir, fragmentNormal);
    float spec = max(0.0f, dot(viewDir, reflectDir));
    float shininess = pow(spec, material.shininess);//反光度，幂越大光越聚集
    vec3 specularColor = shininess * specularTexColor * light.color * light.specularStrength;

    float dis = length(light.pos - fragmentPos);
    //点光源距离衰减公式
    float value = 1.0 / (1 + 0.18 * dis + 0.12 * dis * dis);

    //聚光灯
    float theta = dot(lightDir, normalize(-light.dir));
    float cutoffValue = (theta - light.cutoff) / (light.cutoff - light.outCutoff);
    cutoffValue = clamp(cutoffValue, 0.1f, 1.0f);
    
    vec3 finalColor = (ambientColor + diffuseColor + specularColor) * cutoffValue;//value;
    FragColor = vec4(finalColor, 1.0f);


}