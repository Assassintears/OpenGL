# OpenGL编程思想

OpenGL是一个巨大的状态机，创建一个OpenGL上下文环境（一般由窗口系统管理），OpenGL的所有操作都在当前的上下文环境中被激活或者关闭。
想象一个复杂的机器操作面板，上面有各种各样的按钮。这个面板就是OpenGL的上下文，各种操作按钮可以理解为OpenGL的各种状态，当按钮被按下时，
状态激活，再次按下，状态关闭。OpenGL的工作方式由多种不同的激活状态组合而成。
***
OpenGL的工作流程可概述为：

 1.1 客户端准备数据，比如顶点数据，颜色，顶点法向量，纹理等等；<br>
 1.2 创建缓冲区句柄--**glGenBuffers**, **glGenVertexArrays** ;<br>
 1.3 绑定缓冲区与句柄，即使用句柄来代理缓冲区，类似于指针代理一个缓冲区，客户端后续在使用缓冲区的时候可以直接通过句柄来管理--
    **glBindBuffer**, **glBindVertexArray**<br>
 1.4 申请缓冲区空间。上面的步骤只表明客户端可以使用缓冲区，但是客户端需要多大的缓冲区OpenGL不知道，需要客户端自己指定，然后OpenGL分配给客户端要求的缓冲区（内存块）--**glBufferData**；<br>
 1.5 复制数据到缓冲区。到目前位置，OpenGL已经给客户端分配了一定大小的缓冲区，客户端需要把数据复制到缓冲区中供OpenGL使用，减少客户端（CPU）与OpenGL（GPU）的数据交换。--**glBufferSubData**；<br>
 1.6 关闭前面被激活的状态。OpenGL绑定句柄与缓冲区的关系的时候就激活了那个缓冲区，在不使用的时候要及时关闭状态，避免造成资源不必要的浪费;<br>
 1.7 编译、链接、使用着色器程序；<br>
 1.8 渲染循环。在渲染循环体内，需要渲染哪个缓冲区数据就激活哪个缓冲区，并使用VAO解释缓冲区数据，使能相应的属性;<br>

```cpp
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
```
  
  **注意：**
  1. **glBufferData**开辟指定大小的缓冲区（内存块），并使用指针所指向的内存块数据初始化分配的缓冲区，若指针为空，则缓冲区内容为空。
  2. **glBindBuffer**作用是绑定**glGenBuffers**返回的句柄到相应的缓冲区上，方便其他程序通过该句柄找到该缓冲区。**glGenBuffers**返回成功时，句柄值为非0无符号整数，失败时返回0.所以，若**glBindBuffer**指定的句柄为0，则表明该句柄无效，即无法激活该缓冲区，即该缓冲区处于未激活状态，后续也无法使用该缓冲区。

***

# GLSL数据块

OpenGL GLSL数据来源有两种：
**CPU 客户端**
**GPU 服务端**
从GPU缓冲区读取数据时，GLSL要用布局**layout**限定符标记，不然GLSL不知道如何读取缓冲区数据。GLSL数据块类型有3种：**uniform数据块**，**buffer数据块**，**in/out数据块**。其中，**uniform数据块**，**buffer数据块**都是客户端向GPU缓冲区写入数据，然后着色器从缓冲区读取。而**in/out数据块**是在着色器内部定义的接口，供着色器内部数据传输，客户端无法读写该数据块。

1. **uniform数据块**
**uniform数据块**是着色器中的一种结构体变量，对该变量的赋值需要从缓冲区读取数据。当一个客户端应用程序需要渲染多个模型，并多个着色器共用同一个数据（比如共用投影矩阵，相机矩阵）的时候，可以使用**uniform数据块**。另外需要注意**uniform变量**与**uniform数据块**的区别，**uniform变量**是客户端直接写入着色器，着色器不需要从缓冲区读取，而**uniform数据块**着色器需要从缓冲区读取的。既然**uniform数据块**需要用到缓冲区，OpenGL对缓冲区的操作无外乎：客户端申请缓冲区句柄，客户端开辟缓冲区大小，客户端复制数据到缓冲区，最后解释缓冲区数据。**uniform数据块**数据块的使用如下图所示：
![](https://learnopengl-cn.github.io/img/04/08/advanced_glsl_binding_points.png)

**uniform数据块**关键在于把着色器中的**uniform数据块**数据块索引映射到一个绑定点，然后把缓冲区也映射到相同的绑定点，这样着色器在起作用的时候遇到这个**uniform数据块**可通过绑定点找到对应的缓冲区，然后读取该缓冲区数据。
具体过程如下：
1. 创建缓冲区句柄--**glGenBuffers(GL_UNIFORM_BUFFER, &ubo)**;
2. 绑定缓冲区句柄--**glBindBuffer**;
3. 分配缓冲区大小--**glBufferData**;
4. 客户端复制数据到缓冲区--**glBufferSubData**;
到目前为止，客户端已经准备好缓冲区，接下来就是把缓冲区数据如何与着色器中的**uniform数据块**对应起来;
5. 客户端把缓冲区绑定到一个绑定点上--**glBindBufferRange**;
6. 获取**uniform数据块**在着色器中的索引--**glGetUniformBlockIndex**;
7. 把该着色器中的**uniform数据块**索引绑定到第5步创建的绑定点上--**glUniformBlockBinding**;

```cpp
GLuint ubo;
glGenBuffers(1, &ubo);
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4) + sizeof(GLfloat),
	NULL, GL_STATIC_DRAW);//分配内存
glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
	glm::value_ptr(camera));//复制数据
glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
	glm::value_ptr(proj));
glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4),
	sizeof(GLfloat), &ps);
glBindBuffer(GL_UNIFORM_BUFFER, 0);


GLuint camera_proj, PointSize;
const char* vspath = "uniformBlockvs.shader";
const char* fspath = "uniformBlockfs.shader";
Shader shader(vspath, fspath);
shader.use();
//获取着色器中ubo数据块索引
camera_proj = glGetUniformBlockIndex(shader.ID, "camera_proj");
PointSize = glGetUniformBlockIndex(shader.ID, "PointSize");
glUniformBlockBinding(shader.ID, camera_proj, 1);
glUniformBlockBinding(shader.ID, PointSize, 2);
//数据块索引与ubo关联起来
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
glBindBufferRange(GL_UNIFORM_BUFFER, 1, ubo, 0, 2 * sizeof(glm::mat4));
glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo,
	2 * sizeof(glm::mat4), sizeof(GLfloat));
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```
顶点着色器：
```cpp
\#version 330 core

layout (location = 0) in vec3 aPos;

//uniform块
layout (std140) uniform camera_proj
{
	mat4 camera;//相机矩阵
	mat4 proj;//投影矩阵
};

//uniform块
layout (std140) uniform PointSize
{
	float pointSize;//点大小
};

uniform mat4 model;//uniform变量

out vec4 Color;

void main()
{
	gl_Position = proj * camera * vec4(aPos, 1.0f);
	//gl_Position = vec4(0.0f, 0.5f, 0.5f, 1.0f);
	gl_PointSize = pointSize;
	Color = gl_Position;
}
```

***
# 纹理
</br>
OpenGL给模型贴图叫做纹理，本节只介绍简单的二维纹理贴图。纹理贴图的基本思路就是读取图片，然后把图片信息映射到模型上。这里关键在于如何把图片信息映射到模型上。纹理坐标在x和y轴上，范围为0到1之间（注意我们使用的是2D纹理图像）。使用纹理坐标获取纹理颜色叫做采样(Sampling)。纹理坐标起始于(0, 0)，也就是纹理图片的左下角，终始于(1, 1)，即纹理图片的右上角。下面的图片展示了我们是如何把纹理坐标映射到三角形上的。
![](https://learnopengl-cn.github.io/img/01/06/tex_coords.png)
具体的纹理使用请参考：![Learn OpenGL](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)
总结起来，纹理使用方式与uniform数据块的使用方式类似，如下图所示：

 1. 创建纹理缓冲区：
 ```c++
 	unsigned int textureID; 
 	glGenTextures(1, &textureID);
 ```
 2. 复制图片数据到纹理缓冲区：
 ```c++
	 glBindTexture(GL_TEXTURE_2D, textureID); 
 	 glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);glGenerateMipmap(GL_TEXTURE_2D);//使能纹理多级渐远**
 ```
 3. 设置纹理属性：
 ```c++
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//x方向环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//y方向含绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//纹理插值，纹理过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```
4. **绑定纹理**：一个模型可以有多个纹理贴图，需要显示的告诉OpenGL哪个缓冲区，哪个着色器中的采样器对应哪个纹理数据
```c++
	glUniform1i(glGetUniformLocation(shader.ID, "texture"), 0); // 采样器绑定到纹理单元0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);//缓冲区绑定到纹理单元0
```


纹理采样器用于从纹理缓冲区取纹理数据，定义在片元着色器中，如下所示：
**纹理片元着色器**
```c++
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture;

void main()
{
	FragColor = texture2D(texture, TexCoord);
}
```

**纹理顶点着色器**
```c++
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 model;
};

uniform mat4 view;

out vec2 TexCoord;


void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
```

**模型顶点数据**
```c++
GLfloat vert[] = {
		// Vertex data for face 0
		-1.0f, -1.0f, 1.0f, 0.0f, 0.0f,  // v0 v0,v1,v2,v3不贴图
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
```

**注意事项**
 1. 不同的面的交点的纹理坐标有可能不同。所以，每个面的顶点都要赋值纹理坐标；
 2. 若局部面不需要贴图，则形成该面的顶点的纹理坐标都是同一个数，即纹理采样器只能采样点，无法形成面纹理；
 

		



