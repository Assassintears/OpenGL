#define STB_IMAGE_IMPLEMENTATION
#include "glad\glad.h"
#include "GLFW\glfw3.h"
#include "Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "stb_image.h" //����Ǽ���ͼƬͷ�ļ�����ʵ���Բ������

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

//���ڻص�����
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);

void tran(GLFWwindow* window);//������
void texture(GLFWwindow* window);//��ͼ������
void dynamicDraw(GLFWwindow* window);//��̬��ͼ
void transform(GLFWwindow* window);//�任
void threeDimen(GLFWwindow* window);//3D
void lookat(GLFWwindow* window);//���
void pingyi(GLFWwindow* window);//ƽ��
void Draws(GLFWwindow* window);//��ͬ�Ļ�������
void multiInstance(GLFWwindow* window);//��ʵ��
/********************
ע��ģ�Ͷ�����ʽ��
1. ģ�Ͷ���
2. �������
********************/

void EBO(GLFWwindow* window);//ʹ��EBO������������ͼ
void primativeRestart(GLFWwindow* window);//ͼԪ����

int main()
{
	InitGLFW();
	rotationValue = tag_rotationValue::U;

	//���û�ͼ����
	const int width = 800;
	const int height = 600;
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);//����OpenGL������

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//ע��ص�����
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

//���ڻص�����
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

	//����VBO��VAO������cpu��GPU��ϵ������
	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);//����GPU��ν��Ͷ��㻺������е�����
	glBindVertexArray(VAO);

	//��GPU����������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//��cpu���ݽ�������--��cpu���ݸ��Ƶ�GPU��buffer��
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);//���ٻ����С�����������ݵ�������
	//glNamedBufferStorage(VBO, sizeof(vert), vert, GL_STATIC_DRAW);
	//����GPU��ν���VBO����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float)/*����*/, (void*)0);//���ͻ�������
	glEnableVertexAttribArray(0);//����0

	
	while (!glfwWindowShouldClose(window))
	{

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//glBindVertexArray(VAO);
		//glLineWidth(10);
		//glDrawArrays(GL_LINES, 0, 2);//����
		
		//glPointSize(10);
		//glDrawArrays(GL_POINTS, 0, 2);//����

		//���ʵ������10���㣬����ֻ��ʹ�����5������������Σ���������ʼ��Ϊ5���������Ϊ5
		glDrawArrays(GL_TRIANGLES, 0/*����ε���ʼ��*/, 3/*����ζ������*/);

		//���й����������ˣ���������������ʾ��ͼ
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
	//����ͼƬ
	int width, height, nrChannels;
	const char* filepath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\container.jpg";
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);

	//׼����������
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data != NULL)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);//��ͼƬ���ݸ��Ƶ�GPU��������
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	stbi_image_free(data);
	//��������ָ����ͼ����Ҫ��ͼ��������꣬����������ͼƬ����������--ʵ�������ǲ�֪������ͼƬ����
	//�����������OpenGL����Ҫ������ͼƬ����ͼ���ϵ���һ����
	//��������ԭ�������½ǣ�����Ϊ��x�ᣬ��OpenGL��Ļ����λ����Ļ����
	//ģ��8����������
	float vert[] =
	{
		 0.0f, 0.0f, 0.0f,		//ԭ��
		 50.0f, 0.0f, 0.0f,	    //������
		 50.0f,  50.0f, 0.0f,	//������
		 0.0f,  50.0f, 0.0f,	//����
		 0.0f,   50.0f,  50.0f,	//ǰ����
		 0.0f, 0.0f,  50.0f,	//ǰ����
		 50.0f, 0.0f,  50.0f,	//ǰ����
		 50.0f,  50.0f, 50.0f,	//ǰ����
	};

	GLuint indices[] =	//ģ�Ͷ�����--�����α��
	{
		1, 2, 3,	//��������������
		3, 0, 2,
		0, 3, 4,	//��������������
		4, 5, 0,
		4, 7, 6,	//ǰ������������
		6, 5, 4,
		6, 7, 1,	//��������������
		1, 7, 2,
		7, 3, 2,	//��������������
		7, 4, 3,
		0, 5, 6,	//��������������
		0, 6, 1
	};
	//��������--����ÿ���涼ת������ʱ����������
	//����ӳ���谴�ն�������ָ�����������������飬
	//�ɲο���https://gamedev.stackexchange.com/questions/67803/indexed-uv-mapping-problem
	GLuint texcoord[] =
	{
		//����
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

	//���ͻ���������--ʹ��VAO��������������
	glBindVertexArray(VAO);//�󶨵�GPU
	glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);//�Ѷ������ݸ��Ƶ�GPU�Ļ�����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*����*/, NULL);//���ͻ�������
	glGenBuffers(1, &texture_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, texture_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoord), texcoord, GL_STATIC_DRAW);//���������鸴�Ƶ���������
	glVertexAttribPointer(1, 2, GL_UNSIGNED_INT, GL_FALSE, 2 * sizeof(GLuint)/*����*/, NULL);//���ͻ�������
	glEnableVertexAttribArray(0);//����0
	glEnableVertexAttribArray(1);//����1

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//ʹ����Ⱦ
	//ʹ��GLSL��Ⱦ
	//������ɫ�����������--����GPU�����Ⱦ����������--������������������е�
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\texvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\texfs.shader";
	Shader shader(vspath, fspath);
	shader.use();

	//��ͼ
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//������ɫ���Ժ���Ȳ���

		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(-25.0f, -25.0f, 25.0f));//ƽ��
		//trans = glm::rotate(trans, static_cast<float>(glfwGetTime())/*glm::radians(0.0f)*/, glm::vec3(1.0f, 0.0f, 0.0f));//������x����ת����ת����
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -300.0f));
		//���
		float radius = 200.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, camX, camZ),//���λ��
			glm::vec3(0.0f, 0.0f, 0.0f), //����۲췽��--Ŀ��
			glm::vec3(0.0f, 1.0f, 0.0f));//�������

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

		//���й����������ˣ���������������ʾ��ͼ
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);

}


void dynamicDraw(GLFWwindow* window)
{
	float* vert = new float[100 * 3]; //100����
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
	//����VBO��VAO������cpu��GPU��ϵ������
	GLuint VBO, VAO;
	
	glGenVertexArrays(1, &VAO);//����GPU��ν��Ͷ��㻺������е�����
	glBindVertexArray(VAO);

	//��GPU����������
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//��cpu���ݽ�������--��cpu���ݸ��Ƶ�GPU��buffer��
	glBufferData(GL_ARRAY_BUFFER, /*sizeof(vert)*/ 100 * 3 * sizeof(float), vert, GL_STATIC_DRAW);//���ٻ����С�����������ݵ�������
	//glNamedBufferStorage(VBO, sizeof(vert), vert, GL_STATIC_DRAW);
	//����GPU��ν���VBO����
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*����*/, (void*)0);//���ͻ�������
	glEnableVertexAttribArray(0);//����0
	int idx = 0;
	int num = 1;
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//glPointSize(2);
		//glDrawArrays(GL_POINTS, 0, num);//����
		glLineWidth(2);
		glDrawArrays(GL_LINES, 0, num);//����
		idx += 3;
		num = (num + 1) % 101;
		if (0 == num)
		{
			num = 1;
		}
		Sleep(100);

		//���й����������ˣ���������������ʾ��ͼ
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

	//�󶨵�GPU������
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//�������ݵ�������
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	//���ͻ���������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*����*/, (void*)0);//���ͻ�������
	glEnableVertexAttribArray(0);//����0



	//ʹ��GLSL��Ⱦ
	//������ɫ�����������--����GPU�����Ⱦ����������--������������������е�
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\transformvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\transformfs.shader";
	Shader shader(vspath, fspath);
	shader.use();



	//��ͼ
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 trans;
		trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(0, 0, 1));//������z����ת����ת����
		unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glDrawArrays(GL_TRIANGLES, 0, 3);

		//���й����������ˣ���������������ʾ��ͼ
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);


}


void threeDimen(GLFWwindow* window)
{
	//ģ����ԭ�������������½�
	float vert[] =
	{
		 0.0f, 0.0f, 0.0f,		//��
		 50.0f, 0.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,		
		 0.0f,  50.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,


		 0.0f, 0.0f,  50.0f,	//ǰ
		 50.0f, 0.0f,  50.0f,
		 50.0f,  50.0f, 50.0f,
		 50.0f,  50.0f,  50.0f,
		 0.0f,  50.0f,  50.0f,
		 0.0f, 0.0f,  50.0f,

		 0.0f,   0.0f,   0.0f, //��
		 0.0f,   0.0f,   50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  0.0f,
		 0.0f,   0.0f,   0.0f,

		 50.0f,  50.0f,  50.0f, //��
		 50.0f,  0.0f,  50.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 50.0f,  0.0f,
		 50.0f,  50.0f,  50.0f,

		 50.0f,  50.0f,  50.0f,//��
		 50.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		0.0f, 50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,

		0.0f,  0.0f, 0.0f,//��
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*����*/, (void*)0);//���ͻ�������
	glEnableVertexAttribArray(0);//����0

	//ʹ����Ⱦ
	//ʹ��GLSL��Ⱦ
	//������ɫ�����������--����GPU�����Ⱦ����������--������������������е�
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dfs.shader";
	Shader shader(vspath, fspath);
	shader.use();

	//�任����

	//��ͼ
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 trans = glm::mat4(1.0f);//ģ�ͱ任--ƽ��-��ת-����--ģ������-->��������
		glm::mat4 view = glm::mat4(1.0f); //�۲����--����ռ�
		glm::mat4 projection = glm::mat4(1.0f);//͸�ӱ任--�۲�ռ�-->��Ļ�ռ�
		trans = glm::translate(trans, glm::vec3(0.0f, 25.0f, 25.0f));//ƽ��
		if (rotationValue == tag_rotationValue::X)
		{
			trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(1.0f, 0.0f, 0.0f));//������x����ת����ת����
		}
		else if (rotationValue == tag_rotationValue::Y)
		{
			trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));//������x����ת����ת����
		}
		else if (rotationValue == tag_rotationValue::Z)
		{
			trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));//������x����ת����ת����
		}
		else
		{
			trans = glm::mat4(1.0f);
		}
		
		trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));//����
		projection = glm::perspective(glm::radians(45.0f), 800.0f/ 600.0f, 0.1f, 300.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -300.0f));

		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glDrawArrays(GL_TRIANGLES, 0, sizeof(vert) / sizeof(float));

		//���й����������ˣ���������������ʾ��ͼ
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	
}


void lookat(GLFWwindow* window)
{
	//ģ����ԭ�������������½�
	float vert[] =
	{
		 0.0f, 0.0f, 0.0f,		//��
		 50.0f, 0.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,
		 0.0f,  50.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,


		 0.0f, 0.0f,  50.0f,	//ǰ
		 50.0f, 0.0f,  50.0f,
		 50.0f,  50.0f, 50.0f,
		 50.0f,  50.0f,  50.0f,
		 0.0f,  50.0f,  50.0f,
		 0.0f, 0.0f,  50.0f,

		 0.0f,   0.0f,   0.0f, //��
		 0.0f,   0.0f,   50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  0.0f,
		 0.0f,   0.0f,   0.0f,

		 50.0f,  50.0f,  50.0f, //��
		 50.0f,  0.0f,  50.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 50.0f,  0.0f,
		 50.0f,  50.0f,  50.0f,

		 50.0f,  50.0f,  50.0f,//��
		 50.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		0.0f, 50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,

		0.0f,  0.0f, 0.0f,//��
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*����*/, (void*)0);//���ͻ�������
	glEnableVertexAttribArray(0);//����0

	//ʹ����Ⱦ
	//ʹ��GLSL��Ⱦ
	//������ɫ�����������--����GPU�����Ⱦ����������--������������������е�
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dfs.shader";
	Shader shader(vspath, fspath);
	shader.use();

	
	//��ͼ
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(25.0f, 25.0f, 25.0f));//ƽ��
		//trans = glm::rotate(trans, /*static_cast<float>(glfwGetTime())*/glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));//������x����ת����ת����
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.0f, 1500.0f);
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -300.0f));
		
		//���
		float radius = 300.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),//���λ��
			glm::vec3(0.0f, 0.0f, 0.0f), //����۲췽��--Ŀ��
			glm::vec3(0.0f, 1.0f, 0.0f));//�������


		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glDrawArrays(GL_TRIANGLES, 0, sizeof(vert) / sizeof(float));

		//���й����������ˣ���������������ʾ��ͼ
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}

void EBO(GLFWwindow* window)
{
	//glEnable(GL_DEPTH_TEST);
	
	//ģ��8����������
	float vert[] =
	{
		 0.0f, 0.0f, 0.0f,		//ԭ��
		 50.0f, 0.0f, 0.0f,	    //������
		 50.0f,  50.0f, 0.0f,	//������
		 0.0f,  50.0f, 0.0f,	//����
		 0.0f,   50.0f,  50.0f,	//ǰ����
		 0.0f, 0.0f,  50.0f,	//ǰ����
		 50.0f, 0.0f,  50.0f,	//ǰ����
		 50.0f,  50.0f, 50.0f,	//ǰ����
	};

	GLuint indices[] =	//ģ�Ͷ�����--�����α��
	{
		0, 1, 2,	//��������������
		2, 3, 0,	
		0, 3, 4,	//��������������
		4, 5, 0,
		4, 5, 6,	//ǰ������������
		4, 6, 7,
		6, 7, 1,	//��������������
		1, 7, 2,
		7, 2, 3,	//��������������
		7, 3, 4,	
		0, 5, 6,	//��������������
		0, 6, 1
	};



	GLuint VBO, VAO, EBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);//�󶨵�GPU--״̬��
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);//�Ѷ������ݸ��Ƶ�GPU�Ļ�����
	//���ͻ���������
	glBindVertexArray(VAO);//�󶨵�GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*����*/, (void*)0);//���ͻ�������
	glEnableVertexAttribArray(0);//����0

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//ʹ����Ⱦ
	//ʹ��GLSL��Ⱦ
	//������ɫ�����������--����GPU�����Ⱦ����������--������������������е�
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dfs.shader";
	Shader shader(vspath, fspath);
	shader.use();

	//��ͼ
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//������ɫ���Ժ���Ȳ���

		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(25.0f, 25.0f, 25.0f));//ƽ��
		//trans = glm::rotate(trans, /*static_cast<float>(glfwGetTime())*/glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));//������x����ת����ת����
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.0f, 150000.0f);
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -300.0f));

		//���
		float radius = 300.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),//���λ��
			glm::vec3(0.0f, 0.0f, 0.0f), //����۲췽��--Ŀ��
			glm::vec3(0.0f, 1.0f, 0.0f));//�������


		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		//glDrawArrays(GL_TRIANGLES, 0, sizeof(vert) / sizeof(float));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

		//���й����������ˣ���������������ʾ��ͼ
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}


void pingyi(GLFWwindow* window)
{
	//ģ����ԭ�������������½�
	float vert[] =
	{
		 0.0f, 0.0f, 0.0f,		//��
		 50.0f, 0.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,
		 50.0f,  50.0f, 0.0f,
		 0.0f,  50.0f, 0.0f,
		 0.0f,  0.0f, 0.0f,


		 0.0f, 0.0f,  50.0f,	//ǰ
		 50.0f, 0.0f,  50.0f,
		 50.0f,  50.0f, 50.0f,
		 50.0f,  50.0f,  50.0f,
		 0.0f,  50.0f,  50.0f,
		 0.0f, 0.0f,  50.0f,

		 0.0f,   0.0f,   0.0f, //��
		 0.0f,   0.0f,   50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  50.0f,
		 0.0f,   50.0f,  0.0f,
		 0.0f,   0.0f,   0.0f,

		 50.0f,  50.0f,  50.0f, //��
		 50.0f,  0.0f,  50.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 0.0f, 0.0f,
		 50.0f, 50.0f,  0.0f,
		 50.0f,  50.0f,  50.0f,

		 50.0f,  50.0f,  50.0f,//��
		 50.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		 0.0f,  50.0f,  0.0f,
		0.0f, 50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,

		0.0f,  0.0f, 0.0f,//��
		 0.0f,  0.0f, 50.0f,
		 50.0f,  0.0f,  50.0f,
		 50.0f,  0.0f,  50.0f,
		50.0f,  0.0f,  0.0f,
		0.0f,  0.0f, 0.0f
	};

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.0f, 1500.0f);
	
	//���
	float radius = 300.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;
	view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),//���λ��
		glm::vec3(0.0f, 0.0f, 0.0f), //����۲췽��--Ŀ��
		glm::vec3(0.0f, 1.0f, 0.0f));//�������



	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*����*/, (void*)0);//���ͻ�������
	glEnableVertexAttribArray(0);//����0

	//ʹ����Ⱦ
	//ʹ��GLSL��Ⱦ
	//������ɫ�����������--����GPU�����Ⱦ����������--������������������е�
	const char* vspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dvs.shader";
	const char* fspath = "C:\\Users\\John\\source\\repos\\OpenGL\\OpenGL\\Dfs.shader";
	Shader shader(vspath, fspath);
	shader.use();


	//��ͼ
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//trans = glm::translate(trans, glm::vec3(dx, dy, 0.0f));//ƽ��

		unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
		unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glDrawArrays(GL_TRIANGLES, 0, sizeof(vert) / sizeof(float));

		//���й����������ˣ���������������ʾ��ͼ
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}

void onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//ƽ��
	float dx = 0.0f;
	float dy = 0.0f;
	std::vector<float> tmp;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (GLFW_KEY_UP == key && action == GLFW_PRESS)
	{
		dy = 10.0f;//����ƽ�ƶ��ٿ��Լ�����
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
		std::cout << "R��������\n";
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
	//׼������
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
	glGenBuffers(1, &VBO);//�������������
	//�������״̬���Ѿ���󶨵�����Ļ������ϣ�����ͨ���þ���ҵ��û�����
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//���ٻ������ռ䣬Ϊ�������ݵ�����������׼��
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) + sizeof(colors),
		nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex);//�������ݵ�������
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex), sizeof(colors), colors);
	glBindBuffer(GL_ARRAY_BUFFER, 0);//�û�����ʹ����ϣ��رոû�����״̬

	glGenVertexArrays(1, &VAO);//����VAO���


	glGenBuffers(1, &EBO);//����EBO���
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//���롢����ʹ����ɫ��
	const char* vspath = "./Draws/vert.vert";
	const char* fspath = "./Draws/frag.frag";
	Shader shader(vspath, fspath);
	shader.use();

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//���¼���׼����Ⱦ�Ļ�����
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//���ͻ���������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]), NULL);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);
		glEnableVertexAttribArray(0);//ʹ��׼����Ⱦ������
		glEnableVertexAttribArray(1);
		
		//��ͼ
		glDrawElements(GL_TRIANGLES, sizeof(index) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		//��ͼ��ϣ��������ԣ��رջ�����
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//���й����������ˣ���������������ʾ��ͼ
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);

}


void primativeRestart(GLFWwindow* window)
{
	GLfloat vertex[] =
	{
		//x��
		0.0f, 0.0f, 0.0f,
		50.0f, 0.0f, 0.0f,
		//د
		53.0f, 1.0f, 0.0f,
		51.0f, -1.0f, 0.0f,
		//x�ַ���
		51.0f, 1.0f, 0.0f,
		53.0f, -1.0f, 0.0f,
		//y��
		0.0f, 0.0f, 0.0f,
		0.0f, 50.0f, 0.0f,
		//د
		1.0f, 53.0f, 0.0f,
		-1.0f, 51.0f, 0.0f,
		//y����
		-1.0f, 53.0f, 0.0f,
		0.0f, 52.0f, 0.0f,

		//z
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 50.0f,
		//z�ַ�
		-1.0f, 1.0f, 51.0f,
		0.0f, 1.0f, 51.0f,

		0.0f, 1.0f, 51.0f,
		-1.0f, -1.0f, 52.0f,

		-1.0f, -1.0f, 52.0f,
		1.0f, -1.0f, 52.0f
	};

	GLuint index[] =
	{
		0, 1,//x��
		2, 3,//x�ַ�Ʋ
		4, 5,//x�ַ���
		0, 6,//y��
		7, 8,//yƲ
		9, 10,//y��
		0, 11,//z��
		12/*, 13,*///z�Ϻ�
		//13, 14,//zƲ
		//14, 15//z�º�
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

	//ʹ����Ⱦ
	//ʹ��GLSL��Ⱦ
	//������ɫ�����������--����GPU�����Ⱦ����������--������������������е�
	const char* vspath = "C:\\Users\\jhon\\Desktop\\OpenGL\\OpenGL\\shaders\\primative\\Dvs.shader";
	const char* fspath = "C:\\Users\\jhon\\Desktop\\OpenGL\\OpenGL\\shaders\\primative\\Dfs.shader";
	Shader shader(vspath, fspath);
	


	glm::mat4 model(1.0f);
	model = model * glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f ,1.0f);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.0f, 150.0f);


	//��ͼ
	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		float radius = 300.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		glm::mat4 view = glm::lookAt(glm::vec3(camX, 100.0f, camZ),//���λ��
			glm::vec3(0.0f, 0.0f, 0.0f), //����۲췽��--Ŀ��
			glm::vec3(0.0f, 1.0f, 0.0f));//�������

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

		//���й����������ˣ���������������ʾ��ͼ
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);

}


void multiInstance(GLFWwindow* window)
{

}