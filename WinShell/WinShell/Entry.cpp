#include <glad.h>
#include <glfw3.h>
#include <iostream>
#include <vector>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

unsigned defaultShaderProgram;

void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void setDefaultShaderProgram();
void processInput(GLFWwindow* window);
void render(GLFWwindow* window);

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

	setDefaultShaderProgram();

	unsigned int VAOs[2];
	unsigned int VBOs[2];

	float vertices0[] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.5f,  0.5f, 0.0f,
	 -0.5f,  0.5f, 0.0f,
	};
	unsigned int indices0[] = {
		0, 1, 2,
		2, 3, 0
	};

	float vertices1[] = {
	-1.0f, -0.5f, 0.0f,
	 -0.5f, -0.5f, 0.0f,
	 -0.5f,  0.5f, 0.0f,
	};

	glGenVertexArrays(2, VAOs);
	glBindVertexArray(VAOs[0]);

	glGenBuffers(2, VBOs);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	//将顶点数据复制到该VBO中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices0), vertices0, GL_STATIC_DRAW);
	//定义如何解析顶点数据输入到0号位顶点属性中
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	//启用0号位的顶点属性
	glEnableVertexAttribArray(0);

	//元素缓冲对象用于避免重复的顶点数据，用索引定义绘制的顺序
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices0), indices0, GL_STATIC_DRAW);

	glBindVertexArray(VAOs[1]);

	//VAO存储EBO的绑定引用，所以要在解绑VAO之后再解绑EBO，否则解绑操作也会被储存起来
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
	//将顶点数据复制到该VBO中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	//定义如何解析顶点数据输入到0号位顶点属性中
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	//启用0号位的顶点属性
	glEnableVertexAttribArray(0);


	while (!glfwWindowShouldClose(window))
	{
		//输入
		processInput(window);

		//渲染
		render(window);

		glUseProgram(defaultShaderProgram);
		glBindVertexArray(VAOs[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(VAOs[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//交换缓冲区，检查事件
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	//设置视口大小
	//前两个参数为窗口左下角的位置
	glViewport(0, 0, width, height);
}

void setDefaultShaderProgram()
{
	//创建，设置源码，编译顶点着色器
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//创建，设置源码，编译片段着色器
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//创建、附加、链接、使用着色器程序
	defaultShaderProgram = glCreateProgram();
	glAttachShader(defaultShaderProgram, vertexShader);
	glAttachShader(defaultShaderProgram, fragmentShader);
	glLinkProgram(defaultShaderProgram);

	glGetProgramiv(defaultShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(defaultShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	//删除着色器
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}

}

void render(GLFWwindow* window)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}