#version 330 core
layout (location = 0) in vec3 aPos;//输入模型空间坐标
layout (location = 1) in vec3 aNormal;//模型空间法向量

out vec3 Normal;//观察空间法向量
out vec3 worldPos;//观察空间点坐标

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	worldPos = vec3(view * model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(view * model))) * aNormal;
}