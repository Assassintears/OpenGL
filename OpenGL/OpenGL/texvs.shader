#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;//属性指示器，从绑定的VAO中取第i个属性的值，赋给变量aTexCoord
//顶点着色器：输出模型坐标供片段着色器使用

//out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection *  view * model * vec4(aPos, 1.0);
	//ourColor = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}