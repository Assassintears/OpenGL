#define STB_IMAGE_IMPLEMENTATION
#include "glad\glad.h"
#include "GLFW\glfw3.h"
#include "Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "stb_image.h" //这个是加载图片头文件，其实可以不用这个

#include <iostream>
#include <Windows.h>
#include <vector>


glm::mat4 trans = glm::mat4(1.0f);
std::vector<std::vector<float> > history;

enum tag_rotationValue
{
	X,
	Y,
	Z,
	U
}rotationValue;


void InitGLFW();
void InitGLAD();

//窗口回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);

void tran(GLFWwindow* window);//三角形
void texture(GLFWwindow* window);//贴图，纹理
void dynamicDraw(GLFWwindow* window);//动态画图
void transform(GLFWwindow* window);//变换
void threeDimen(GLFWwindow* window);//3D
void lookat(GLFWwindow* window);//相机
void pingyi(GLFWwindow* window);//平移
void Draws(GLFWwindow* window);//不同的绘制命令
void multiInstance(GLFWwindow* window);//多实例
/********************
注意模型动画方式：
1. 模型动；
2. 相机动；
********************/

void EBO(GLFWwindow* window);//使用EBO索引顶点对象绘图
void primativeRestart(GLFWwindow* window);//图元重启

int main()
{
	InitGLFW();
	rotationValue = tag_rotationValue::U;

	//设置绘图区域
	const int width = 800;
	const int height = 600;
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);//创建OpenGL上下文

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//注册回调函数
	glfwSetKeyCallback(window, onKey);

	InitGLAD();	

	//tran(window);
	//texture(window);
	//dynamicDraw(window);
	//transform(window);
	//threeDimen(window);
	//lookat(window);
	//EBO(window);
	//pingyi(window);
	//Draws(window);
	primativeRestart(window);
	glfwTerminate();
	return 0;
}

void InitGLFW()
{
	if (GL_FALSE == glfwInit())
	{
		return;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void InitGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
}

//窗口回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void tran(GLFWwindow* window)
{
	float vert[] =
	{
		0.2f, 0.2f, 0.7f,
		0.5f, 0.2f, 0.3f,
		0.2f,  0.5f, 0.8f
	};

	//创建VBO，VAO，建立cpu与GPU联系的桥梁
	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);//告诉GPU如何解释顶点缓冲对象中的数据
	glBindVertexArray(VAO);

	//与GPU缓冲区关联
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//与cpu数据建立关联--把cpu数据复制到GPU的buffer中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);//开辟缓存大小，并复制数据到缓存中
	//glNamedBufferStorage(VBO, sizeof(vert), vert, GL_STATIC_DRAW);
	//告诉GPU如何解释VBO数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float)/*步长*/, (void*)0);//解释缓存数据
	glEnableVertexAttribArray(0);//属性0

	
	while (!glfwWindowShouldClose(window))
	{

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//glBindVertexArray(VAO);
		//glLineWidth(10);
		//glDrawArrays(GL_LINES, 0, 2);//画线
		
		//glPointSize(10);
		//glDrawArrays(GL_POINTS, 0, 2);//画点

		//如果实际我有10给点，但我只想使用最后5个点来画多边形，则多边形起始点为5，顶点个数为5
		glDrawArrays(GL_TRIANGLES, 0/*多边形的起始点*/, 3/*多边形顶点个数*/);

		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}


void texture(GLFWwindow* window)
{
	//		v3---------v2
	//	  /	|		  / |
	//	 /	|		 /  |
	//	v4--|-------v7  |
	//	|	|v0-----|--v1
	//	|	/		|  /
	//	|  /		| /
	//	| /			|/
	//	v5----------v6
	//
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//加载图片
	int width, height, nrChannels;
	const char* filepath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\container.jpg";
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

	//准备纹理数据
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data != NULL)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);//把图片数据复制到GPU纹理缓冲区
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	stbi_image_free(data);
	//纹理坐标指的是图形需要贴图区域的坐标，而不是纹理图片的区域坐标--实际上我们不知道纹理图片坐标
	//纹理坐标告诉OpenGL，需要把纹理图片贴到图形上的哪一部分
	//纹理坐标原点在左下角，向右为正x轴，而OpenGL屏幕坐标位于屏幕中心
	//模型8个顶点坐标
	float vert[] =
	{
		 0.0f, 0.0f, 0.0f,		//原点
		 50.0f, 0.0f, 0.0f,	    //后右下
		 50.0f,  50.0f, 0.0f,	//后右上
		 0.0f,  50.0f, 0.0f,	//后上
		 0.0f,   50.0f,  50.0f,	//前左上
		 0.0f, 0.0f,  50.0f,	//前左下
		 50.0f, 0.0f,  50.0f,	//前右下
		 50.0f,  50.0f, 50.0f,	//前右上
	};

	GLuint indices[] =	//模型顶点编号--三角形编号
	{
		1, 2, 3,	//后面两个三角形
		3, 0, 2,
		0, 3, 4,	//左面两个三角形
		4, 5, 0,
		4, 7, 6,	//前面两个三角形
		6, 5, 4,
		6, 7, 1,	//右面两个三角形
		1, 7, 2,
		7, 3, 2,	//上面两个三角形
		7, 4, 3,
		0, 5, 6,	//下面两个三角形
		0, 6, 1
	};
	//纹理坐标--假想每个面都转到正面时的纹理坐标
	//纹理映射需按照顶点数组指定，而不是索引数组，
	//可参考：https://gamedev.stackexchange.com/questions/67803/indexed-uv-mapping-problem
	GLuint texcoord[] =
	{
		//后面
		0, 0,
		1, 0,
		1, 1,
		0, 1,
		0, 1,
		0, 0, 
		1, 0,
		1, 1
	};
	GLuint point_vbo, texture_vbo;
	GLuint VAO, EBO;
	glGenBuffers(1, &point_vbo);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	//解释缓冲区数据--使用VAO描述缓冲区数据
	glBindVertexArray(VAO);//绑定到GPU
	glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);//把顶点数据复制到GPU的缓冲区
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*步长*/, NULL);//解释缓存数据
	glGenBuffers(1, &texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);//把纹理数组复制到缓冲区中
	glVertexAttribPointer(1, 2, GL_UNSIGNED_INT, GL_FALSE, 2 * sizeof(GLuint)/*步长*/, NULL);//解释缓存数据
	glEnableVertexAttribArray(0);//属性0
	glEnableVertexAttribArray(1);//属性1

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//使用渲染
	//使用GLSL渲染
	//创建着色器，激活程序--告诉GPU如何渲染缓冲区数据--缓冲区数据是如何排列的
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\texvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\texfs.shader";
	Shader shader(vspath, fspath);
	shader.use();

	//画图
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//允许颜色测试和深度测试

		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(-25.0f, -25.0f, 25.0f));//平移
		//trans = glm::rotate(trans, static_cast<float>(glfwGetTime())/*glm::radians(0.0f)*/, glm::vec3(1.0f, 0.0f, 0.0f));//生成绕x轴旋转的旋转矩阵
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -300.0f));
		//相机
		float radius = 200.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, camX, camZ),//相机位置
			glm::vec3(0.0f, 0.0f, 0.0f), //相机观察方向--目标
			glm::vec3(0.0f, 1.0f, 0.0f));//相机上轴

		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1500.0f);


		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		//glDrawArrays(GL_TRIANGLES, 0, sizeof(vert) / sizeof(float));
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);

}


void dynamicDraw(GLFWwindow* window)
{
	float* vert = new float[100 * 3]; //100个点
	memset(vert, 0, 100 * 3);
	float x = -0.9;
	float y = 0;
	float step = 2.0f / 100.0f;
	for (int i = 0; i < 100; ++i)
	{
		if (x >= 0.9)
		{
			x = 0.9;
		}
		vert[3 * i] = x;
		y = std::sinf(x);
		if (y > 0.9)
		{
			y = 0.9;
		}
		if (y < -0.9)
		{
			y = -0.9;
		}
		vert[3 * i + 1] = y;
		vert[3 * i+ 2] = 0;
		x = x + step;
	}
	//创建VBO，VAO，建立cpu与GPU联系的桥梁
	GLuint VBO, VAO;
	
	glGenVertexArrays(1, &VAO);//告诉GPU如何解释顶点缓冲对象中的数据
	glBindVertexArray(VAO);

	//与GPU缓冲区关联
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//与cpu数据建立关联--把cpu数据复制到GPU的buffer中
	glBufferData(GL_ARRAY_BUFFER, /*sizeof(vert)*/ 100 * 3 * sizeof(float), vert, GL_STATIC_DRAW);//开辟缓存大小，并复制数据到缓存中
	//glNamedBufferStorage(VBO, sizeof(vert), vert, GL_STATIC_DRAW);
	//告诉GPU如何解释VBO数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*步长*/, (void*)0);//解释缓存数据
	glEnableVertexAttribArray(0);//属性0
	int idx = 0;
	int num = 1;
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//glPointSize(2);
		//glDrawArrays(GL_POINTS, 0, num);//画点
		glLineWidth(2);
		glDrawArrays(GL_LINES, 0, num);//画线
		idx += 3;
		num = (num + 1) % 101;
		if (0 == num)
		{
			num = 1;
		}
		Sleep(100);

		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	//delete vert;
}


void transform(GLFWwindow* window)
{
	float vert[] =
	{
		0.2f, 0.2f, 0.7f,
		0.5f, 0.2f, 0.3f,
		0.2f,  0.5f, 0.8f
	};

	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	//绑定到GPU缓冲区
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//复制数据到缓冲区
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	//解释缓冲区数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*步长*/, (void*)0);//解释缓存数据
	glEnableVertexAttribArray(0);//属性0



	//使用GLSL渲染
	//创建着色器，激活程序--告诉GPU如何渲染缓冲区数据--缓冲区数据是如何排列的
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\transformvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\transformfs.shader";
	Shader shader(vspath, fspath);
	shader.use();



	//画图
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 trans;
		trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(0, 0, 1));//生成绕z轴旋转的旋转矩阵
		unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glDrawArrays(GL_TRIANGLES, 0, 3);

		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);


}


void threeDimen(GLFWwindow* window)
{
	//模型坐原点在立方体左下角
	float vert[] =
	{
		 0.0f, 0.0f, 0.0f,		//后
		 50.0f, 0.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,		
		 0.0f,  50.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,


		 0.0f, 0.0f,  50.0f,	//前
		 50.0f, 0.0f,  50.0f,
		 50.0f,  50.0f, 50.0f,
		 50.0f,  50.0f,  50.0f,
		 0.0f,  50.0f,  50.0f,
		 0.0f, 0.0f,  50.0f,

		 0.0f,   0.0f,   0.0f, //左
		 0.0f,   0.0f,   50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  0.0f,
		 0.0f,   0.0f,   0.0f,

		 50.0f,  50.0f,  50.0f, //右
		 50.0f,  0.0f,  50.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 50.0f,  0.0f,
		 50.0f,  50.0f,  50.0f,

		 50.0f,  50.0f,  50.0f,//上
		 50.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		0.0f, 50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,

		0.0f,  0.0f, 0.0f,//下
		 0.0f,  0.0f, 50.0f,
		 50.0f,  0.0f,  50.0f,
		 50.0f,  0.0f,  50.0f,
		50.0f,  0.0f,  0.0f,
		0.0f,  0.0f, 0.0f
	};

	

	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*步长*/, (void*)0);//解释缓存数据
	glEnableVertexAttribArray(0);//属性0

	//使用渲染
	//使用GLSL渲染
	//创建着色器，激活程序--告诉GPU如何渲染缓冲区数据--缓冲区数据是如何排列的
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dfs.shader";
	Shader shader(vspath, fspath);
	shader.use();

	//变换矩阵

	//画图
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 trans = glm::mat4(1.0f);//模型变换--平移-旋转-缩放--模型坐标-->世界坐标
		glm::mat4 view = glm::mat4(1.0f); //观察矩阵--相机空间
		glm::mat4 projection = glm::mat4(1.0f);//透视变换--观察空间-->屏幕空间
		trans = glm::translate(trans, glm::vec3(0.0f, 25.0f, 25.0f));//平移
		if (rotationValue == tag_rotationValue::X)
		{
			trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 0.0f, 0.0f));//生成绕x轴旋转的旋转矩阵
		}
		else if (rotationValue == tag_rotationValue::Y)
		{
			trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));//生成绕x轴旋转的旋转矩阵
		}
		else if (rotationValue == tag_rotationValue::Z)
		{
			trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));//生成绕x轴旋转的旋转矩阵
		}
		else
		{
			trans = glm::mat4(1.0f);
		}
		
		trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));//缩放
		projection = glm::perspective(glm::radians(45.0f), 800.0f/ 600.0f, 0.1f, 300.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -300.0f));

		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glDrawArrays(GL_TRIANGLES, 0, sizeof(vert) / sizeof(float));

		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	
}


void lookat(GLFWwindow* window)
{
	//模型坐原点在立方体左下角
	float vert[] =
	{
		 0.0f, 0.0f, 0.0f,		//后
		 50.0f, 0.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,
		 0.0f,  50.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,


		 0.0f, 0.0f,  50.0f,	//前
		 50.0f, 0.0f,  50.0f,
		 50.0f,  50.0f, 50.0f,
		 50.0f,  50.0f,  50.0f,
		 0.0f,  50.0f,  50.0f,
		 0.0f, 0.0f,  50.0f,

		 0.0f,   0.0f,   0.0f, //左
		 0.0f,   0.0f,   50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  0.0f,
		 0.0f,   0.0f,   0.0f,

		 50.0f,  50.0f,  50.0f, //右
		 50.0f,  0.0f,  50.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 50.0f,  0.0f,
		 50.0f,  50.0f,  50.0f,

		 50.0f,  50.0f,  50.0f,//上
		 50.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		0.0f, 50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,

		0.0f,  0.0f, 0.0f,//下
		 0.0f,  0.0f, 50.0f,
		 50.0f,  0.0f,  50.0f,
		 50.0f,  0.0f,  50.0f,
		50.0f,  0.0f,  0.0f,
		0.0f,  0.0f, 0.0f
	};



	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*步长*/, (void*)0);//解释缓存数据
	glEnableVertexAttribArray(0);//属性0

	//使用渲染
	//使用GLSL渲染
	//创建着色器，激活程序--告诉GPU如何渲染缓冲区数据--缓冲区数据是如何排列的
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dfs.shader";
	Shader shader(vspath, fspath);
	shader.use();

	
	//画图
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(25.0f, 25.0f, 25.0f));//平移
		//trans = glm::rotate(trans, /*static_cast<float>(glfwGetTime())*/glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));//生成绕x轴旋转的旋转矩阵
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.0f, 1500.0f);
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -300.0f));
		
		//相机
		float radius = 300.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),//相机位置
			glm::vec3(0.0f, 0.0f, 0.0f), //相机观察方向--目标
			glm::vec3(0.0f, 1.0f, 0.0f));//相机上轴


		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glDrawArrays(GL_TRIANGLES, 0, sizeof(vert) / sizeof(float));

		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}

void EBO(GLFWwindow* window)
{
	//glEnable(GL_DEPTH_TEST);
	
	//模型8个顶点坐标
	float vert[] =
	{
		 0.0f, 0.0f, 0.0f,		//原点
		 50.0f, 0.0f, 0.0f,	    //后右下
		 50.0f,  50.0f, 0.0f,	//后右上
		 0.0f,  50.0f, 0.0f,	//后上
		 0.0f,   50.0f,  50.0f,	//前左上
		 0.0f, 0.0f,  50.0f,	//前左下
		 50.0f, 0.0f,  50.0f,	//前右下
		 50.0f,  50.0f, 50.0f,	//前右上
	};

	GLuint indices[] =	//模型顶点编号--三角形编号
	{
		0, 1, 2,	//后面两个三角形
		2, 3, 0,	
		0, 3, 4,	//左面两个三角形
		4, 5, 0,
		4, 5, 6,	//前面两个三角形
		4, 6, 7,
		6, 7, 1,	//右面两个三角形
		1, 7, 2,
		7, 2, 3,	//上面两个三角形
		7, 3, 4,	
		0, 5, 6,	//下面两个三角形
		0, 6, 1
	};



	GLuint VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);//绑定到GPU--状态机
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);//把顶点数据复制到GPU的缓冲区
	//解释缓冲区数据
	glBindVertexArray(VAO);//绑定到GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*步长*/, (void*)0);//解释缓存数据
	glEnableVertexAttribArray(0);//属性0

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//使用渲染
	//使用GLSL渲染
	//创建着色器，激活程序--告诉GPU如何渲染缓冲区数据--缓冲区数据是如何排列的
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dfs.shader";
	Shader shader(vspath, fspath);
	shader.use();

	//画图
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//允许颜色测试和深度测试

		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(25.0f, 25.0f, 25.0f));//平移
		//trans = glm::rotate(trans, /*static_cast<float>(glfwGetTime())*/glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));//生成绕x轴旋转的旋转矩阵
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.0f, 150000.0f);
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -300.0f));

		//相机
		float radius = 300.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),//相机位置
			glm::vec3(0.0f, 0.0f, 0.0f), //相机观察方向--目标
			glm::vec3(0.0f, 1.0f, 0.0f));//相机上轴


		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		//glDrawArrays(GL_TRIANGLES, 0, sizeof(vert) / sizeof(float));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}


void pingyi(GLFWwindow* window)
{
	//模型坐原点在立方体左下角
	float vert[] =
	{
		 0.0f, 0.0f, 0.0f,		//后
		 50.0f, 0.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,
		 0.0f,  50.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,


		 0.0f, 0.0f,  50.0f,	//前
		 50.0f, 0.0f,  50.0f,
		 50.0f,  50.0f, 50.0f,
		 50.0f,  50.0f,  50.0f,
		 0.0f,  50.0f,  50.0f,
		 0.0f, 0.0f,  50.0f,

		 0.0f,   0.0f,   0.0f, //左
		 0.0f,   0.0f,   50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  0.0f,
		 0.0f,   0.0f,   0.0f,

		 50.0f,  50.0f,  50.0f, //右
		 50.0f,  0.0f,  50.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 50.0f,  0.0f,
		 50.0f,  50.0f,  50.0f,

		 50.0f,  50.0f,  50.0f,//上
		 50.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		0.0f, 50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,

		0.0f,  0.0f, 0.0f,//下
		 0.0f,  0.0f, 50.0f,
		 50.0f,  0.0f,  50.0f,
		 50.0f,  0.0f,  50.0f,
		50.0f,  0.0f,  0.0f,
		0.0f,  0.0f, 0.0f
	};

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.0f, 1500.0f);
	
	//相机
	float radius = 300.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;
	view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),//相机位置
		glm::vec3(0.0f, 0.0f, 0.0f), //相机观察方向--目标
		glm::vec3(0.0f, 1.0f, 0.0f));//相机上轴



	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*步长*/, (void*)0);//解释缓存数据
	glEnableVertexAttribArray(0);//属性0

	//使用渲染
	//使用GLSL渲染
	//创建着色器，激活程序--告诉GPU如何渲染缓冲区数据--缓冲区数据是如何排列的
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dfs.shader";
	Shader shader(vspath, fspath);
	shader.use();


	//画图
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//trans = glm::translate(trans, glm::vec3(dx, dy, 0.0f));//平移

		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glDrawArrays(GL_TRIANGLES, 0, sizeof(vert) / sizeof(float));

		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}

void onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//平移
	float dx = 0.0f;
	float dy = 0.0f;
	std::vector<float> tmp;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (GLFW_KEY_UP == key && action == GLFW_PRESS)
	{
		dy = 10.0f;//这里平移多少可自己设置
		tmp.push_back(0.0f);
		tmp.push_back(-dy);
		history.push_back(tmp);
		trans = glm::translate(trans, glm::vec3(dx, dy, 0.0f));
	}
	if (GLFW_KEY_DOWN == key && action == GLFW_PRESS)
	{
		dy = -10.0f;
		tmp.push_back(0.0f);
		tmp.push_back(-dy);
		history.push_back(tmp);
		trans = glm::translate(trans, glm::vec3(dx, dy, 0.0f));
	}
	if (GLFW_KEY_LEFT == key && action == GLFW_PRESS)
	{
		dx = -10.0f;
		tmp.push_back(-dx);
		tmp.push_back(0.0f);
		history.push_back(tmp);
		trans = glm::translate(trans, glm::vec3(dx, dy, 0.0f));
	}
	if (GLFW_KEY_RIGHT == key && action == GLFW_PRESS)
	{
		dx = 10.0f;
		tmp.push_back(-dx);
		tmp.push_back(0.0f);
		history.push_back(tmp);
		trans = glm::translate(trans, glm::vec3(dx, dy, 0.0f));
	}
	if (GLFW_KEY_R == key && action == GLFW_RELEASE)
	{
		if (!history.empty())
		{
			tmp = history.at(history.size() - 1);
			trans = glm::translate(trans, glm::vec3(tmp[0], tmp[1], 0.0f));
			history.erase(history.end() - 1, history.end());
		}
		std::cout << "R键被按下\n";
	}
	if (GLFW_KEY_X == key && action == GLFW_RELEASE)
	{
		rotationValue = tag_rotationValue::X;
	}
	if (GLFW_KEY_Y == key && action == GLFW_RELEASE)
	{
		rotationValue = tag_rotationValue::Y;
	}
	if (GLFW_KEY_Z == key && action == GLFW_RELEASE)
	{
		rotationValue = tag_rotationValue::Z;
	}
}

void Draws(GLFWwindow* window)
{
	//准备数据
	GLfloat vertex[][3] = {
		{-1.0f, -1.0f, 0.0f},
		{1.0f, -1.0f, 0.0f},
		{-1.0f, 1.0f, 0.0f},
		{-1.0f, -1.0f, 0.0f} };

	GLfloat colors[] = {
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f
	};

	GLuint index[] = {
	0, 1, 2};

	
	GLuint VBO, VAO, EBO;
	glGenBuffers(1, &VBO);//创建缓冲区句柄
	//激活缓冲区状态，把句柄绑定到具体的缓冲区上，后续通过该句柄找到该缓冲区
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//开辟缓冲区空间，为复制数据到缓冲区中做准备
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) + sizeof(colors),
		nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex);//复制数据到缓冲区
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex), sizeof(colors), colors);
	glBindBuffer(GL_ARRAY_BUFFER, 0);//该缓冲区使用完毕，关闭该缓冲区状态

	glGenVertexArrays(1, &VAO);//申请VAO句柄


	glGenBuffers(1, &EBO);//申请EBO句柄
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//编译、链接使用着色器
	const char* vspath = "./Draws/vert.vert";
	const char* fspath = "./Draws/frag.frag";
	Shader shader(vspath, fspath);
	shader.use();

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//重新激活准备渲染的缓冲区
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//解释缓冲区数据
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
		glEnableVertexAttribArray(0);//使能准备渲染的属性
		glEnableVertexAttribArray(1);
		
		//绘图
		glDrawElements(GL_TRIANGLES, sizeof(index) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		//绘图完毕，禁用属性，关闭缓冲区
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);

}


void primativeRestart(GLFWwindow* window)
{
	GLfloat vertex[] =
	{
		//x轴
		0.0f, 0.0f, 0.0f,
		50.0f, 0.0f, 0.0f,
		//丿
		53.0f, 1.0f, 0.0f,
		51.0f, -1.0f, 0.0f,
		//x字符捺
		51.0f, 1.0f, 0.0f,
		53.0f, -1.0f, 0.0f,
		//y轴
		0.0f, 0.0f, 0.0f,
		0.0f, 50.0f, 0.0f,
		//丿
		1.0f, 53.0f, 0.0f,
		-1.0f, 51.0f, 0.0f,
		//y的捺
		-1.0f, 53.0f, 0.0f,
		0.0f, 52.0f, 0.0f,

		//z
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 50.0f,
		//z字符
		-1.0f, 1.0f, 51.0f,
		0.0f, 1.0f, 51.0f,

		0.0f, 1.0f, 51.0f,
		-1.0f, -1.0f, 52.0f,

		-1.0f, -1.0f, 52.0f,
		1.0f, -1.0f, 52.0f
	};

	GLuint index[] =
	{
		0, 1,//x轴
		2, 3,//x字符撇
		4, 5,//x字符捺
		0, 6,//y轴
		7, 8,//y撇
		9, 10,//y捺
		0, 11,//z轴
		12/*, 13,*///z上横
		//13, 14,//z撇
		//14, 15//z下横
	};

	GLfloat san[] =
	{
		0.0f, 0.0f, 0.0f,
		50.0f, 0.0f, 0.0f,
		25.0f, 25.0f, 0.0f
	};

	GLuint sanvbo, sanvao;
	glGenBuffers(1, &sanvbo);
	glBindBuffer(GL_ARRAY_BUFFER, sanvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(san), san, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &sanvao);
	 

	GLuint vbo, ebo, vao;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &vao);

	//使用渲染
	//使用GLSL渲染
	//创建着色器，激活程序--告诉GPU如何渲染缓冲区数据--缓冲区数据是如何排列的
	const char* vspath = "C:\\Users\\jhon\\Desktop\\OpenGL\\OpenGL\\shaders\\primative\\Dvs.shader";
	const char* fspath = "C:\\Users\\jhon\\Desktop\\OpenGL\\OpenGL\\shaders\\primative\\Dfs.shader";
	Shader shader(vspath, fspath);
	


	glm::mat4 model(1.0f);
	model = model * glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f ,1.0f);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.0f, 150.0f);


	//画图
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float radius = 300.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		glm::mat4 view = glm::lookAt(glm::vec3(camX, 100.0f, camZ),//相机位置
			glm::vec3(0.0f, 0.0f, 0.0f), //相机观察方向--目标
			glm::vec3(0.0f, 1.0f, 0.0f));//相机上轴

		shader.use();
		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));


		//Coordinates
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBindVertexArray(vao);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(0);
		//glDrawElements(GL_LINES, sizeof(index) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);


		glLineWidth(2.0f);
		glDrawArrays(GL_LINES, 0, sizeof(vertex) / sizeof(float));
		//unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//Triangles
		glBindBuffer(GL_ARRAY_BUFFER, sanvbo);
		glBindVertexArray(sanvao);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(0);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(san) / sizeof(float));
		//unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);

}


void multiInstance(GLFWwindow* window)
{

}