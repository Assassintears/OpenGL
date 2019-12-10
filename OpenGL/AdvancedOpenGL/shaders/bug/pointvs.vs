#version 330 core
layout (location = 0) in vec3 aPos;//输入的是顶点的归一化坐标


void main()
{
	gl_Position = vec4(aPos, 1.0);
}