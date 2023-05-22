#include "shader.h"
using namespace glm;

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::string geometryCode;
    std::ifstream gShaderFile;
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (geometryPath)
        {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }

    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    //创建，设置源码，编译顶点着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    checkCompileError(vertexShader, "VERTEX");

    unsigned int geometryShader = 0;
    if (geometryPath)
    {
        const char* gShaderCode = geometryCode.c_str();
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &gShaderCode, NULL);
        glCompileShader(geometryShader);
        checkCompileError(geometryShader, "GEOMETRY");
    }

    //创建，设置源码，编译片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    checkCompileError(fragmentShader, "FRAGMENT");

    //创建、附加、链接、使用着色器程序
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    if (geometryPath)
        glAttachShader(ID, geometryShader);
    glLinkProgram(ID);
    checkCompileError(ID, "PROGRAM");

    //删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    int location = glGetUniformLocation(ID, name.c_str());
    glUniform1i(location, (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    int location = glGetUniformLocation(ID, name.c_str());
    glUniform1i(location, value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    int location = glGetUniformLocation(ID, name.c_str());
    glUniform1f(location, value);
}

void Shader::setColor(const std::string& name, float value[4]) const
{
    int location = glGetUniformLocation(ID, name.c_str());
    glUniform4f(location, value[0], value[1], value[2], value[3]);
}

void Shader::setVec3(const std::string& name, vec3 vec) const
{
    int location = glGetUniformLocation(ID, name.c_str());
    glUniform3f(location, vec.x, vec.y, vec.z);
}

void Shader::setMat4(const std::string& name, int count, bool transpose, mat4 trans) const
{
    int location = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(location, count, transpose ? GL_TRUE : GL_FALSE, value_ptr(trans));
}

void Shader::setMat4(const std::string& name, mat4 trans) const
{
    int location = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(trans));
}

void Shader::checkCompileError(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
