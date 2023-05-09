#include <glad.h>
#include <glfw3.h>
#include <iostream>
#include <vector>
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"

using namespace std;
using namespace glm;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

unsigned defaultShaderProgram;
float mixValue = 0.5f;

Camera camera(vec3(0.0, 0.0, 6.0));
float cameraSpeed = 4.0f;
float deltaTime;
float lastTime;
bool firstEnter = true;
double mouseX, mouseY;

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseInputCallback(GLFWwindow* window, double x, double y);
void scrollInputCallback(GLFWwindow* window, double x, double y);
void processInput(GLFWwindow* window);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ZZY", NULL, NULL);
	if (window == NULL)
	{
		cout << "Create window fail!" << endl;
		glfwTerminate();
		return -1;
	}
	//将我们窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);

	//由于OpenGL驱动版本众多，它大多数函数的位置都无法在编译时确定下来，需要在运行时查询
	//需要在运行时获取函数地址并将其保存在一个函数指针中供以后使用
	//给GLAD传入了用来加载系统相关的OpenGL函数指针地址的函数，GLFW给我们的是glfwGetProcAddress
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Init glad fail!" << endl;
		return -1;
	}

	//注册窗口大小改变时的回调
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

	float cubeVertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int objVAO;
	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	unsigned int objVBO;
	glGenBuffers(1, &objVBO);
	glBindBuffer(GL_ARRAY_BUFFER, objVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	//只需绑定VBO即可，因为上面已经设置过VBO数据
	glBindBuffer(GL_ARRAY_BUFFER, objVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
	glEnableVertexAttribArray(0);

	Shader objShader("shader.vs", "shader.fs");
	Shader lightShader("shader.vs", "light.fs");

	//开启深度测试
	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//鼠标移动回调
	glfwSetCursorPosCallback(window, mouseInputCallback);
	//滚轮回调
	glfwSetScrollCallback(window, scrollInputCallback);

	vec3 lightPos(0.0,0.0,0.0);

	while (!glfwWindowShouldClose(window))
	{
		deltaTime = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();
		//输入
		processInput(window);

		//渲染
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float radius = 2.0f;
		lightPos.x = sin(glfwGetTime()) * radius;
		//lightPos.y = sin(glfwGetTime()) * radius;
		lightPos.z = cos(glfwGetTime()) * radius;

		objShader.use();
		mat4 projection;
		projection = perspective(radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		objShader.setMat4("projection", 1, false, projection);
		objShader.setMat4("view", 1, false, camera.GetViewMatrix());
		mat4 model;
		model = translate(model, vec3(0, -1.5, 0));
		model = rotate(model, radians(30.0f), vec3(1.0f, 1.0f, 1.0f));
		model = scale(model, vec3(6.0f, 0.1f, 6.0f));
		objShader.setMat4("model", 1, false, model);
		objShader.setVec3("viewPos", camera.Position);
		objShader.setVec3("light.pos", lightPos);
		objShader.setFloat("light.intensity", 36.0f);
		objShader.setVec3("light.color", vec3(1.0, 1.0, 1.0));
		objShader.setFloat("light.ambientStrength", 0.2f);
		objShader.setFloat("light.diffuseStrength", 1.0f);
		objShader.setFloat("light.specularStrength", 0.1f);
		objShader.setVec3("material.baseColor", vec3(0.0, 0.05, 0.0));
		objShader.setVec3("material.diffuseColor", vec3(0.4, 0.5, 0.4));
		objShader.setVec3("material.specularColor", vec3(0.04, 0.7, 0.04));
		objShader.setFloat("material.shininess", 32);
		glBindVertexArray(objVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = mat4();
		model = translate(model, vec3(0, 1.0, -3.0));
		model = rotate(model, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		model = scale(model, vec3(6.0f, 0.25f, 6.0f));
		objShader.setMat4("model", 1, false, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = mat4();
		model = translate(model, vec3(3, 0.0, 0.0));
		model = rotate(model, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
		model = scale(model, vec3(6.0f, 0.25f, 6.0f));
		objShader.setMat4("model", 1, false, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = mat4();
		model = translate(model, vec3(-3, 0.0, 0.0));
		model = rotate(model, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
		model = scale(model, vec3(6.0f, 0.25f, 6.0f));
		objShader.setMat4("model", 1, false, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightShader.use();
		lightShader.setMat4("projection", 1, false, projection);
		lightShader.setMat4("view", 1, false, camera.GetViewMatrix());
		model = mat4();
		model = translate(model, lightPos);
		model = scale(model, vec3(0.2f));
		lightShader.setMat4("model", 1, false, model);
		lightShader.setVec3("lightColor", vec3(1.0f, 1.0f, 1.0f));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//交换缓冲区，检查事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &objVAO);
	glDeleteBuffers(1, &objVBO);
	glDeleteVertexArrays(1, &lightVAO);

	glfwTerminate();
	return 0;
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	//设置视口大小
	//前两个参数为窗口左下角的位置
	glViewport(0, 0, width, height);
}

void mouseInputCallback(GLFWwindow* window, double x, double y)
{
	if (firstEnter)
	{
		firstEnter = false;
		mouseX = x;
		mouseY = y;
	}

	double offsetX = (x - mouseX) / SCREEN_WIDTH * 360.0f;
	double offsetY = (y - mouseY) / SCREEN_HEIGHT * 360.0f;
	mouseX = x;
	mouseY = y;

	camera.ProcessMouseMovement(offsetX, -offsetY);
}

void scrollInputCallback(GLFWwindow* window, double x, double y)
{
	camera.ProcessMouseScroll(y);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.01f;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.01f;
	}
	mixValue = mixValue > 1.0f ? 1.0f : mixValue;
	mixValue = mixValue < 0.0f ? 0.0f : mixValue;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(WORLD_UP, deltaTime);
	}
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(WORLD_DOWN, deltaTime);
	}

}