#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>; // 包含glad来获取所有的必须OpenGL头文件

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;


class Shader
{
public:
    // 程序ID
    unsigned int ID;

    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL);
    // 使用/激活程序
    void use();
    // uniform工具函数
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setColor(const std::string& name, float value[4]) const;
    void setVec3(const std::string& name, vec3 vec) const;
    void setMat4(const std::string& name, int count, bool transpose, mat4 trans) const;
    void setMat4(const std::string& name, mat4 trans) const;
private:
    void checkCompileError(unsigned int shader, std::string type);
};

#endif