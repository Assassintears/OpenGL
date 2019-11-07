# OpenGL编程思想

OpenGL是一个巨大的状态机，创建一个OpenGL上下文环境（一般由窗口系统管理），OpenGL的所有操作都在当前的上下文环境中被激活或者关闭。
想象一个复杂的机器操作面板，上面有各种各样的按钮。这个面板就是OpenGL的上下文，各种操作按钮可以理解为OpenGL的各种状态，当按钮被按下时，
状态激活，再次按下，状态关闭。OpenGL的工作方式由多种不同的激活状态组合而成。
***
OpenGL的工作流程可概述为：

1.1 客户端准备数据，比如顶点数据，颜色，顶点法向量，纹理等等；
1.2 创建缓冲区句柄--**glGenBuffers**, **glGenVertexArrays** ;
1.3 绑定缓冲区与句柄，即使用句柄来代理缓冲区，类似于指针代理一个缓冲区，客户端后续在使用缓冲区的时候可以直接通过句柄来管理--
    **glBindBuffer**, **glBindVertexArray**
1.4 申请缓冲区空间。上面的步骤只表明客户端可以使用缓冲区，但是客户端需要多大的缓冲区OpenGL不知道，需要客户端自己指定，然后OpenGL分配给客户端要求的缓冲区（内存块）--**glBufferData**；
1.5 复制数据到缓冲区。到目前位置，OpenGL已经给客户端分配了一定大小的缓冲区，客户端需要把数据复制到缓冲区中供OpenGL使用，减少客户端（CPU）与OpenGL（GPU）的数据交换。--**glBufferSubData**；
1.6 关闭前面被激活的状态。OpenGL绑定句柄与缓冲区的关系的时候就激活了那个缓冲区，在不使用的时候要及时关闭状态，避免造成资源不必要的浪费。
1.7 编译、链接、使用着色器程序；
1.8 渲染循环。在渲染循环体内，需要渲染哪个缓冲区数据就激活哪个缓冲区，并使用VAO解释缓冲区数据，使能相应的属性。

`//准备数据
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
	glfwDestroyWindow(window);`
  
  **注意：**
  1. **glBufferData**开辟指定大小的缓冲区（内存块），并使用指针所指向的内存块数据初始化分配的缓冲区，若指针为空，则缓冲区内容为空。
  2. **glBindBuffer**作用是绑定**glGenBuffers**返回的句柄到相应的缓冲区上，方便其他程序通过该句柄找到该缓冲区。**glGenBuffers**返回成功时，句柄值为非0无符号整数，失败时返回0.所以，若**glBindBuffer**指定的句柄为0，则表明该句柄无效，即无法激活该缓冲区，即该缓冲区处于未激活状态，后续也无法使用该缓冲区。

