#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h> // 包含glad来获取所有的必须OpenGL头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
using namespace glm;

enum LightType
{
    DIRECTION_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT
};

class Light
{
private:
    int shaderID;
    LightType type;
    int index;
public:
    Light(int shaderID, LightType type, int index = -1);
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, vec3 value) const;
};

#endif