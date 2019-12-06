#version 330 core
layout (location = 0) in vec3 aPos;//从缓冲区输入模型空间坐标
layout (location = 1) in vec3 aNormal;//从缓冲区输入的模型空间法向量

uniform vec3 objectColor;//目标反射的颜色
uniform vec3 lightColor;//光源颜色
uniform vec3 lightPos;//观察空间中的光源位置

out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	float ambientStrength = 0.1;//杂散光，背景光强度
	float specularStrength = 0.5;//镜面光强度
	
	vec3 Normal = mat3(transpose(inverse(view * model))) * aNormal;//观察空间下的法线
	
	//计算背景光
	vec3 ambient = ambientStrength * lightColor;
	
	//计算漫反射
	vec3 viewSpaceCoord = vec3(view * model * vec4(aPos, 1.0));
	vec3 lightDir = normalize(lightPos - viewSpaceCoord);
	float diffuse_direction_cos = max(dot(lightDir, Normal), 0);//漫反射方向余弦
	vec3 diffuse = diffuse_direction_cos * lightColor;
	
	//计算镜面反射
	
	//1. 计算光线相对于法线的镜面反射光线
	vec3 reflectDir = reflect(lightDir, Normal);
	//2. 计算观察者与目标点之间的方向向量
	vec3 viewDir = normalize(vec3(0.0f, 0.0f, 0.0f) - vec3(view * model * vec4(aPos, 1.0)));
	//3. 计算两者之间的方向余弦
	float specular_direction_cos = max(dot(reflectDir, viewDir), 0);//镜面反射方向余弦
	//4. 计算镜面反射量
	float spec = pow(specular_direction_cos, 32);
	vec3 specular = specularStrength * spec * lightColor;
	
	color = (ambient + diffuse + specular) * objectColor;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	
}