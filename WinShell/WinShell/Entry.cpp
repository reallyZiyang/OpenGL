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
#include "Light.h"
#include "model.h"

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
bool spotOn;

unsigned int loadCubemapTexture(vector<string> faces);
void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseInputCallback(GLFWwindow* window, double x, double y);
void scrollInputCallback(GLFWwindow* window, double x, double y);
void processInput(GLFWwindow* window);
void draw1(GLFWwindow* window);
void draw2(GLFWwindow* window);
unsigned int loadTexture(char const* path);

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

	draw1(window);
	//draw2(window);

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
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		spotOn = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		spotOn = false;
	}
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, height, channel;
	unsigned char* data = stbi_load(path, &width, &height, &channel, 0);
	if (data)
	{
		GLenum format;
		if (channel == 1)
			format = GL_RED;
		else if (channel == 3)
			format = GL_RGB;
		else if (channel == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
	}
	stbi_image_free(data);

	return textureID;
}


unsigned int loadCubemapTexture(vector<string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, channel;
	for (size_t i = 0; i < faces.size(); i++)
	{
		string path = faces[i];
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channel, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
		}
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

void draw1(GLFWwindow* window)
{
	float cubeVertices[] = {
		// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	unsigned int objVAO;
	glGenVertexArrays(1, &objVAO);
	glBindVertexArray(objVAO);

	unsigned int objVBO;
	glGenBuffers(1, &objVBO);
	glBindBuffer(GL_ARRAY_BUFFER, objVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	//只需绑定VBO即可，因为上面已经设置过VBO数据
	glBindBuffer(GL_ARRAY_BUFFER, objVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(0);

	float planeVertices[] = {
		// positions          // texture Coords 
		 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,

		 5.0f, -0.5f,  5.0f,  1.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 1.0f,
		 5.0f, -0.5f, -5.0f,  1.0f, 1.0f
	};
	unsigned int grassVAO, grassVBO;
	glGenVertexArrays(1, &grassVAO);
	glBindVertexArray(grassVAO);

	glGenBuffers(1, &grassVBO);
	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	Shader grassShader("grass.vs", "grass.fs");


	Shader objShader("shader.vs", "shader.fs");
	Shader lightShader("shader.vs", "light.fs");
	Shader modelShader("explode_model.vs", "model.fs", "model_explode.gs");
	Shader outlineShader("shader.vs", "outline.fs");
	Shader rendertextureShader("rendertexture.vs", "rendertexture.fs");
	Model nanosuitModel("model/nanosuit/nanosuit.obj");
	
	//开启深度测试
	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//鼠标移动回调
	glfwSetCursorPosCallback(window, mouseInputCallback);
	//滚轮回调
	glfwSetScrollCallback(window, scrollInputCallback);

	unsigned int diffuseMap = loadTexture("container2.png");
	unsigned int specularMap = loadTexture("container2_specular.png");
	unsigned int grassMap = loadTexture("blending_transparent_window.png");

	vector<Shader> shaders;
	shaders.push_back(objShader);
	shaders.push_back(modelShader);
	Light dirLight(shaders, DIRECTION_LIGHT);
	Light pointLight[4] =
	{
		{Light(shaders, POINT_LIGHT, 0)},
		{Light(shaders, POINT_LIGHT, 1)},
		{Light(shaders, POINT_LIGHT, 2)},
		{Light(shaders, POINT_LIGHT, 3)},
	};
	Light spotLight(shaders, SPOT_LIGHT);

	glm::vec3 pointLightPositions[] = {
	glm::vec3(2.0f,  3.0f,  2.0f),
	glm::vec3(-2.0f, -3.0f, -2.0f),
	glm::vec3(2.0f,  -3.0f, 2.0f),
	glm::vec3(-2.0f,  3.0f, -2.0f)
	};

	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	unsigned int frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	//数据传null来创建一张空纹理
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	//将纹理附加到帧缓冲对象中，该纹理只存储颜色信息
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//创建渲染缓冲来存储深度和模板信息
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//将渲染缓冲附加到帧缓冲对象中，渲染缓冲只存储深度和模板信息
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	vector<std::string> faces
	{
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/front.jpg",
		"skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemapTexture(faces);

	unsigned int skyVAO, skyVBO;
	glGenVertexArrays(1, &skyVAO);
	glBindVertexArray(skyVAO);

	glGenBuffers(1, &skyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	Shader skyShader("cubemap.vs", "cubemap.fs");
	Shader reflectShader("shader.vs", "reflect.fs");

	unsigned int reflectUniformBlockIndex = glGetUniformBlockIndex(reflectShader.ID, "Matrices");
	unsigned int grassUniformBlockIndex = glGetUniformBlockIndex(grassShader.ID, "Matrices");
	unsigned int objUniformBlockIndex = glGetUniformBlockIndex(objShader.ID, "Matrices");
	unsigned int lightUniformBlockIndex = glGetUniformBlockIndex(lightShader.ID, "Matrices");
	unsigned int modelUniformBlockIndex = glGetUniformBlockIndex(modelShader.ID, "Matrices");
	unsigned int outlineUniformBlockIndex = glGetUniformBlockIndex(outlineShader.ID, "Matrices");
	unsigned int rendertextureUniformBlockIndex = glGetUniformBlockIndex(rendertextureShader.ID, "Matrices");
	
	glUniformBlockBinding(reflectShader.ID, reflectUniformBlockIndex, 0);
	glUniformBlockBinding(grassShader.ID, grassUniformBlockIndex, 0);
	glUniformBlockBinding(objShader.ID, objUniformBlockIndex, 0);
	glUniformBlockBinding(lightShader.ID, lightUniformBlockIndex, 0);
	glUniformBlockBinding(modelShader.ID, modelUniformBlockIndex, 0);
	glUniformBlockBinding(outlineShader.ID, outlineUniformBlockIndex, 0);
	glUniformBlockBinding(rendertextureUniformBlockIndex, rendertextureShader.ID, 0);

	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);

	while (!glfwWindowShouldClose(window))
	{
		deltaTime = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();
		//输入
		processInput(window);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		//glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		//渲染
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		mat4 projection;
		projection = perspective(radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		mat4 view = camera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), value_ptr(view));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), value_ptr(projection));

		glStencilMask(0x00);

		mat4 model;
		model = translate(model, vec3(0, 0, 1));
		model = scale(model, vec3(0.25f));
		modelShader.use();
		modelShader.setMat4("model", 1, false, model);

		for (size_t i = 0; i < 4; i++)
		{
			objShader.use();
			float radius = i % 2 == 1 ? 3.0f : 1.5f;
			vec3 pos = pointLightPositions[i] + vec3(sin(glfwGetTime()) * radius, i % 2 == 0 ? sin(glfwGetTime()) * radius : cos(glfwGetTime()) * radius, cos(glfwGetTime()) * radius);
			pointLight[i].setVec3("pos", pos);
			pointLight[i].setVec3("color", vec3(1.0));
			pointLight[i].setFloat("diffuseStrength", 1.0f);
			pointLight[i].setFloat("specularStrength", 2.0f);

			lightShader.use();
			mat4 lightModel;
			lightModel = translate(lightModel, pos);
			lightModel = scale(lightModel, vec3(0.2f));
			lightShader.setMat4("model", 1, false, lightModel);
			lightShader.setVec3("lightColor", vec3(1.0));
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		dirLight.setVec3("dir", vec3(0.5, 0.0, 0.0));
		dirLight.setVec3("color", vec3(1.0));
		dirLight.setFloat("ambientStrength", 0.05f);
		dirLight.setFloat("diffuseStrength", 0.1f);
		dirLight.setFloat("specularStrength", 0.5f);

		spotLight.setVec3("pos", camera.Position);
		spotLight.setVec3("dir", camera.Front);
		spotLight.setVec3("color", spotOn ? vec3(1.0, 1.0, 1.0) : vec3(0.0));
		spotLight.setFloat("insideCutoff", cos(radians(15.0f)));
		spotLight.setFloat("outsideCutoff", cos(radians(20.0f)));
		spotLight.setFloat("diffuseStrength", 3.0f);
		spotLight.setFloat("specularStrength", 6.0f);

		objShader.use();
		objShader.setVec3("material.baseColor", vec3(1.0, 1.0, 1.0));
		objShader.setVec3("material.diffuseColor", vec3(0.4, 0.5, 0.4));
		objShader.setFloat("material.shininess", 64);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		objShader.setInt("material.diffuseTexture", 0);
		objShader.setInt("material.specularTexture", 1);

		model = mat4();
		model = translate(model, vec3(0, -1.5, 0));
		model = rotate(model, radians(30.0f), vec3(1.0f, 1.0f, 1.0f));
		model = scale(model, vec3(6.0f, 0.1f, 6.0f));
		objShader.setMat4("model", 1, false, model);
		objShader.setVec3("viewPos", camera.Position);

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

		model = mat4();
		model = translate(model, vec3(0, 4.25, 0.0));
		objShader.setMat4("model", 1, false, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = mat4();
		model = translate(model, vec3(0.75, 3.5, -1.0));
		objShader.setMat4("model", 1, false, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = mat4();
		model = translate(model, vec3(-1.5, 5.0, 0.0));
		objShader.setMat4("model", 1, false, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		reflectShader.use();
		reflectShader.setVec3("viewPos", camera.Position);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		reflectShader.setInt("skybox", 0);
		model = mat4();
		model = translate(model, vec3(0, 2, 7));
		reflectShader.setMat4("model", 1, false, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		modelShader.use();
		modelShader.setFloat("time", glfwGetTime());
		modelShader.setVec3("material.baseColor", vec3(1.0, 1.0, 1.0));
		modelShader.setFloat("material.shininess", 256);
		modelShader.setVec3("viewPos", camera.Position);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		modelShader.use();
		model = mat4();
		model = translate(model, vec3(0, 0, 1));
		model = scale(model, vec3(0.25f));
		modelShader.setMat4("model", 1, false, model);
		nanosuitModel.draw(modelShader);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		//glDisable(GL_DEPTH_TEST);

		//outlineShader.use();
		//model = mat4();
		//model = translate(model, vec3(0, 0, 1));
		//model = scale(model, vec3(0.255f));
		//outlineShader.setMat4("model", 1, false, model);
		//nanosuitModel.draw(outlineShader);


		//glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);//天空盒深度值永远为1，1是最远的深度，equal使得空白的地方才能通过深度测试
		skyShader.use();
		skyShader.setMat4("projection", 1, false, projection);
		//转成3维矩阵丢失位移信息，再转回4维矩阵，天空盒以0为原点不需要位移
		skyShader.setMat4("view", 1, false, mat4(mat3(camera.GetViewMatrix())));
		skyShader.setInt("skybox", 0);
		glBindVertexArray(skyVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);
		//glDepthMask(GL_TRUE);

		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		glBindVertexArray(grassVAO);
		grassShader.use();
		model = mat4();
		model = translate(model, vec3(-1.5, 2.0, 2.0));
		model = rotate(model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
		model = scale(model, vec3(0.25f));
		grassShader.setMat4("model", 1, false, model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassMap);
		grassShader.setInt("baseTexture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		model = mat4();
		model = translate(model, vec3(1.5, 2.0, 2.0));
		model = rotate(model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
		model = scale(model, vec3(0.25f));
		grassShader.setMat4("model", 1, false, model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassMap);
		grassShader.setInt("baseTexture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		model = mat4();
		model = translate(model, vec3(0, 2.0, 5.0));
		model = rotate(model, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
		model = scale(model, vec3(0.25f));
		grassShader.setMat4("model", 1, false, model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassMap);
		grassShader.setInt("baseTexture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		//glLineWidth(5.0f);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		rendertextureShader.use();
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//交换缓冲区，检查事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &objVAO);
	glDeleteBuffers(1, &objVBO);
	glDeleteVertexArrays(1, &lightVAO);

}

void draw2(GLFWwindow* window)
{
	float points[] = {
	-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // 左上
	 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // 右上
	 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 右下
	-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // 左下
	};

	unsigned int gVAO, gVBO;
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

	Shader geometryShader("geometry.vs", "geometry.fs", "geometry.gs");

	while (!glfwWindowShouldClose(window))
	{
		deltaTime = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();
		//输入
		processInput(window);

		//渲染
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		geometryShader.use();
		glBindVertexArray(gVAO);
		glDrawArrays(GL_POINTS, 0, 4);

		//交换缓冲区，检查事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
