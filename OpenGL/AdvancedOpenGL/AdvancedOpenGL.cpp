#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <Camera.h>
#include <model.h>
#include "ft2build.h"
#include FT_FREETYPE_H

#include <iostream>
#include <map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);//返回纹理ID

void texture(GLFWwindow* window);//普通纹理
void coorTexture(GLFWwindow* window);//三维坐标轴+刻度+标签
void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color,
	GLuint& vbo, GLuint& vao);
struct Character {
	GLuint     TextureID;  // 字形纹理的ID
	glm::ivec2 Size;       // 字形大小
	glm::ivec2 Bearing;    // 从基准线到字形左部/顶部的偏移值
	GLuint     Advance;    // 原点距下一个字形原点的距离
};
std::map<GLchar, Character> Characters;

//展示一个bug，OpenGL状态绑定错误会怎么样
void classicBug(GLFWwindow* window);

// settings
const float SCR_WIDTH = 1280.0f;
const float SCR_HEIGHT = 720.0f;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_ALWAYS);
	//glDepthFunc(GL_LESS);

	//texture(window);
	//coorTexture(window);
	classicBug(window);
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	std::cout << "宽度：" << width <<  "\n";
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RED;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, 
			GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		return 0;
	}
	
	return textureID;
}

void texture(GLFWwindow* window)
{
	
	GLfloat vert[] = {
		// Vertex data for face 0
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f,  // v0
		1.0f, -1.0f,  1.0f, 0.0f, 0.0f, // v1
		-1.0f,  1.0f,  1.0f, 0.0f, 0.0f,  // v2
		1.0f,  1.0f,  1.0f, 0.0f, 0.0f, // v3

		// Vertex data for face 1
		1.0f, -1.0f,  1.0f, 0.0f, 0.5f, // v4
		1.0f, -1.0f, -1.0f, 0.33f, 0.5f, // v5
		1.0f,  1.0f,  1.0f, 0.0f, 1.0f,  // v6
		1.0f,  1.0f, -1.0f, 0.33f, 1.0f, // v7

		// Vertex data for face 2
		1.0f, -1.0f, -1.0f, 0.66f, 0.5f, // v8
		-1.0f, -1.0f, -1.0f, 1.0f, 0.5f,  // v9
		1.0f,  1.0f, -1.0f, 0.66f, 1.0f, // v10
		-1.0f,  1.0f, -1.0f, 1.0f, 1.0f,  // v11

		// Vertex data for face 3
		-1.0f, -1.0f, -1.0f, 0.66f, 0.0f, // v12
		-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,  // v13
		-1.0f,  1.0f, -1.0f, 0.66f, 0.5f, // v14
		-1.0f,  1.0f,  1.0f, 1.0f, 0.5f,  // v15

		// Vertex data for face 4
		-1.0f, -1.0f, -1.0f, 0.33f, 0.0f, // v16
		1.0f, -1.0f, -1.0f, 0.66f, 0.0f, // v17
		-1.0f, -1.0f,  1.0f, 0.33f, 0.5f, // v18
		1.0f, -1.0f,  1.0f, 0.66f, 0.5f, // v19

		// Vertex data for face 5
		-1.0f,  1.0f,  1.0f, 0.33f, 0.5f, // v20
		1.0f,  1.0f,  1.0f, 0.66f, 0.5f, // v21
		-1.0f,  1.0f, -1.0f, 0.33f, 1.0f, // v22
		1.0f,  1.0f, -1.0f, 0.66f, 1.0f  // v23
	};

	GLushort indices[] = {
	 0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
	 4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
	 8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
	12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
	16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
	20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
	};

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
		SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);

	glm::mat4 view;

	glm::mat4 model(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -10.0f));

	GLuint vbo, ebo, vao;
	GLuint ubo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert), vert);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), 
		glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
		glm::value_ptr(model));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenVertexArrays(1, &vao);

	GLuint texID = loadTexture("C:\\Users\\jhon\\Desktop\\OpenGL\\AdvancedOpenGL\\cube.png");

	const char* vspath = "C:\\Users\\jhon\\Desktop\\OpenGL\\AdvancedOpenGL\\shaders\\texture\\texturevs.vs";
	const char* fspath = "C:\\Users\\jhon\\Desktop\\OpenGL\\AdvancedOpenGL\\shaders\\texture\\texturefs.fs";
	Shader shader(vspath, fspath);
	shader.use();
	glUniform1i(glGetUniformLocation(shader.ID, "texture"), 0); // 采样器绑定到纹理单元0

	//uniform缓冲区绑定点
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	GLuint bindingPoint = 1;
	glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, 
		ubo, 0, 2 * sizeof(glm::mat4));
	glUniformBlockBinding(shader.ID, 
		glGetUniformBlockIndex(shader.ID, "Matrices"), bindingPoint);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float radius = 5.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 0.0f, camZ),//相机位置
			glm::vec3(0.0f, 0.0f, 0.0f), //相机观察方向--目标
			glm::vec3(0.0f, 1.0f, 0.0f));//相机上轴
		shader.use();
		GLint viewLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);//缓冲区绑定到纹理单元0

		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBindVertexArray(vao);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat)/*步长*/, (void*)0);//解释缓存数据
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);//属性0
		glEnableVertexAttribArray(1);
		//glPointSize(2.0f);
		glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, nullptr);
		
		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}


void coorTexture(GLFWwindow* window)
{
	GLfloat vert[] = {
		//x轴
		0.0f, 0.0f, 0.0f,
		105.0f, 0.0f,  0.0f,
		//50刻度
		50.0f,  0.0f,  0.0,
		50.0f,  2.0f,  0.0f,
		//100刻度
		100.0f,  0.0f,  0.0,
		100.0f,  2.0f,  0.0f,
		//y轴
		0.0f, 0.0f, 0.0f,
		0.0f, 105.0f, 0.0f,
		//y 50刻度
		0.0f, 50.0f, 0.0,
		-2.0f, 50.0f, 0.0,
		//y 100刻度
		0.0f, 100.0f, 0.0,
		-2.0f, 100.0f, 0.0,
		//z轴
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 105.0f,
		//z 50刻度
		0.0f, 0.0f, 50.0,
		0.0f, -2.0f, 50.0,
		//z 100刻度
		0.0f, 0.0f, 100.0,
		0.0f, -2.0f, 100.0,
	};

	glm::mat4 projection = glm::ortho(-SCR_WIDTH / 2, SCR_WIDTH / 2,
		-SCR_HEIGHT / 2, SCR_HEIGHT / 2, 1.0f, 1000.0f);

	glm::mat4 view;

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));

	GLuint vbo, vao;
	GLuint ubo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert), vert);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
		glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
		glm::value_ptr(model));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenVertexArrays(1, &vao);

	const char* vspath = "E:\\GitHub\\LearnOpenGL\\OpenGL\\OpenGL\\AdvancedOpenGL\\shaders\\coordinates\\coorvs.vs";
	const char* fspath = "E:\\GitHub\\LearnOpenGL\\OpenGL\\OpenGL\\AdvancedOpenGL\\shaders\\coordinates\\coorfs.fs";
	const char* charvs = "E:\\GitHub\\LearnOpenGL\\OpenGL\\OpenGL\\AdvancedOpenGL\\shaders\\coordinates\\chavs.vs";
	const char* charfs = "E:\\GitHub\\LearnOpenGL\\OpenGL\\OpenGL\\AdvancedOpenGL\\shaders\\coordinates\\chafs.fs";

	Shader shader(vspath, fspath);
	Shader charshader(charvs, charfs);
	shader.use();
	charshader.use();

	//uniform缓冲区绑定点
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	GLuint bindingPoint = 1;
	glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint,
		ubo, 0, 2 * sizeof(glm::mat4));
	glUniformBlockBinding(shader.ID,
		glGetUniformBlockIndex(shader.ID, "Matrices"), bindingPoint);
	glUniformBlockBinding(charshader.ID,
		glGetUniformBlockIndex(charshader.ID, "Matrices"), bindingPoint);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//纹理字符
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	FT_Error err = FT_New_Face(ft, "arial.ttf", 0, &face);
	if (FT_Err_Unknown_File_Format == err)
	{
		std::cout << "the font file could be opened and read, but it appears, \
			that its font format is unsupported";
	}
	else if (err)
	{
		std::cout << "another error code means that the font file could not \
			be opened or read, or that it is broken";
	}
	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制
	for (GLubyte c = 0; c < 128; c++)
	{
		// 加载字符的字形 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// 生成纹理
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		//纹理缓冲区--专门存储图片纹理
		//不要与纹理坐标缓冲区混淆，纹理坐标
		//缓冲区专门存储的是纹理坐标，告诉GPU如何
		//采样纹理缓冲区数据，以及采样的数据贴图贴在哪里
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// 设置纹理选项
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// 储存字符供之后使用
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint charvbo, charvao;
	glGenVertexArrays(1, &charvao);
	glGenBuffers(1, &charvbo);
	glBindVertexArray(charvao);
	glBindBuffer(GL_ARRAY_BUFFER, charvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float radius = 200.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 20.0f, camZ),//相机位置
			glm::vec3(0.0f, 0.0f, 0.0f), //相机观察方向--目标
			glm::vec3(0.0f, 1.0f, 0.0f));//相机上轴
		shader.use();
		GLint viewLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//画坐标轴
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindVertexArray(vao);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat)/*步长*/, (void*)0);//解释缓存数据
		glEnableVertexAttribArray(0);//属性0
		//glPointSize(2.0f);
		glDrawArrays(GL_LINES, 0, sizeof(vert) / sizeof(GLfloat));

		//画字符
		//在刻度(50,0,0)位置绘制刻度字符
		//计算刻度像素
		glm::vec4 ndc = projection * view * model * glm::vec4(50.0f, 0.0f, 0.0f, 1.0f);
		//计算像素
		float x = (SCR_WIDTH * 0.5f - 0.5f) * (ndc[0] + ndc[3]);
		float y = (SCR_HEIGHT * 0.5f - 0.5f) * (ndc[3] - ndc[1]);
		//std::cout << "x = " << x << " y = " << y << "\n";
		//设置投影矩阵--把像素转化为ndc坐标
		glm::mat4 pro(2 / (SCR_WIDTH - 1), 0.0f, 0.0f, 0.0f,
			0.0f, -2 / (SCR_HEIGHT - 1), 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 1.0f);
		charshader.use();
		glUniformMatrix4fv(glGetUniformLocation(charshader.ID, "projection"),
			1, GL_FALSE, glm::value_ptr(pro));

		RenderText(charshader, "50", x, y, 1, glm::vec3(1.0f, 0.0f, 0.0f),
			charvbo, charvao);


		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
}


void RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color,
	GLuint& vbo, GLuint& vao)
{
	// 激活对应的渲染状态
	s.use();
	glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	// 遍历文本中所有的字符
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// 对每个字符更新VBO
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		// 在四边形上绘制字形纹理
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// 更新VBO内存的内容
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		// 绘制四边形
		//glPointSize(5.0f);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// 更新位置到下一个字形的原点，注意单位是1/64像素
		x += (ch.Advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void classicBug(GLFWwindow* window)
{
	//一个点
	GLfloat vert[] = {0.0f, 0.0f, 0.0f};
	GLuint vbo, vao;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenVertexArrays(1, &vao);

	GLuint ndcvbo, ndcvao;
	glGenBuffers(1, &ndcvbo);
	glBindBuffer(GL_ARRAY_BUFFER, ndcvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenVertexArrays(1, &ndcvao);
	glBindVertexArray(0);

	const char* vspath = "E:\\GitHub\\LearnOpenGL\\OpenGL\\OpenGL\\AdvancedOpenGL\\shaders\\bug\\coorvs.vs";
	const char* fspath = "E:\\GitHub\\LearnOpenGL\\OpenGL\\OpenGL\\AdvancedOpenGL\\shaders\\bug\\coorfs.fs";
	const char* charvs = "E:\\GitHub\\LearnOpenGL\\OpenGL\\OpenGL\\AdvancedOpenGL\\shaders\\bug\\pointvs.vs";
	const char* charfs = "E:\\GitHub\\LearnOpenGL\\OpenGL\\OpenGL\\AdvancedOpenGL\\shaders\\bug\\pointfs.fs";

	Shader shader(vspath, fspath);
	Shader charshader(charvs, charfs);
	shader.use();
	charshader.use();

	glm::mat4 projection = glm::ortho(-SCR_WIDTH / 2, SCR_WIDTH / 2,
		-SCR_HEIGHT / 2, SCR_HEIGHT / 2, 1.0f, 500.0f);

	glm::mat4 view;

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
		glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
		glm::value_ptr(model));

	////uniform缓冲区绑定点
	//glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	//GLuint bindingPoint = 1;
	//glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint,
	//	ubo, 0, 2 * sizeof(glm::mat4));
	//glUniformBlockBinding(shader.ID,
	//	glGetUniformBlockIndex(shader.ID, "Matrices"), bindingPoint);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float radius = 100.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(100, 0.0f, 100.0f),//相机位置
			glm::vec3(0.0f, 0.0f, 0.0f), //相机观察方向--目标
			glm::vec3(0.0f, 1.0f, 0.0f));//相机上轴
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(camX, 0.0f, 0.0f));
		shader.use();
		GLint viewLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"),
			1, GL_FALSE, glm::value_ptr(model));
		//std::cout << projection[1][1] << "\n";

		//画原始点
		//glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindVertexArray(vao);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat)/*步长*/, (void*)0);//解释缓存数据
		glEnableVertexAttribArray(0);//属性0
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//画平移之后的点
		//计算刻度像素
		glm::vec4 ndc = projection * view * model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//计算像素
		float x = (SCR_WIDTH * 0.5f - 0.5f) * (ndc[0] + ndc[3]);
		float y = (SCR_HEIGHT * 0.5f - 0.5f) * (ndc[3] - ndc[1]);
		//设置投影矩阵--把像素转化为ndc坐标
		y = y + 10.0f;//平移10个像素
		//重新计算NDC
		float ndcx = 2.0f / (SCR_WIDTH - 1) * x - 1;
		float ndcy = 1 - 2.0f / (SCR_HEIGHT - 1) * y;

		charshader.use();
		float ndcvert[] = {ndcx, ndcy, 0.0f};
		glBindBuffer(GL_ARRAY_BUFFER, ndcvbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ndcvert), ndcvert);
		glBindVertexArray(ndcvbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, 1);



		//所有工作都做完了，交换缓冲区，显示绘图
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);


}