#include "Light.h"
#include <sstream>
using namespace glm;

Light::Light(int shaderID, LightType type, int index)
{
	this->shaderID = shaderID;
	this->type = type;
	this->index = index;
}

void Light::setFloat(const std::string& name, float value) const
{
	std::string uniformName;
	switch (type)
	{
	case DIRECTION_LIGHT:
		uniformName = "directionLight.";
		break;
	case POINT_LIGHT:
	{
		std::stringstream ss;
		ss << "pointLights[" << index << "].";
		uniformName = ss.str();
		break;
	}
	case SPOT_LIGHT:
		uniformName = "spotLight.";
		break;
	}
	uniformName = uniformName.append(name);
	int location = glGetUniformLocation(shaderID, uniformName.c_str());
	glUniform1f(location, value);
}

void Light::setVec3(const std::string& name, vec3 value) const
{
	std::string uniformName;
	switch (type)
	{
	case DIRECTION_LIGHT:
		uniformName = "directionLight.";
		break;
	case POINT_LIGHT:
	{
		std::stringstream ss;
		ss << "pointLights[" << index << "].";
		uniformName = ss.str();
		break;
	}
	case SPOT_LIGHT:
		uniformName = "spotLight.";
		break;
	}
	uniformName = uniformName.append(name);
	int location = glGetUniformLocation(shaderID, uniformName.c_str());
	glUniform3f(location, value.x, value.y, value.z);
}
