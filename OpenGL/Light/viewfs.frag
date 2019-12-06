#version 330 core
out vec4 FragColor;
  
uniform vec3 objectColor;//目标反射的颜色
uniform vec3 lightColor;//光源颜色
uniform vec3 lightPos;//观察空间中的光源位置

in vec3 Normal;//模型每个顶点的法向量
in vec3 worldPos;//观察空间中模型顶点位置
//所有光照的计算都是在片段着色器里进行

void main()
{
	float ambientStrength = 0.1;//杂散光，背景光强度
	float specularStrength = 0.5;//镜面光强度
	
	vec3 viewPos = vec3(0.0f, 0.0f, 0.0f);//观察空间中观察者的坐标为原点
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - worldPos);//光线方向
	
	float diff = max(dot(lightDir, norm), 0);
	vec3 diffuse = diff * lightColor;
	
	vec3 viewDir = normalize(viewPos - worldPos);//观察方向
	//使用reflect函数计算镜面反射光
	vec3 reflectDir = reflect(lightDir, norm);//镜面反射光向量，reflect要求第一个向量的方向是从光源指向目标
	//计算镜面反射光与观察方向之间的夹角
	float orientation = max(dot(reflectDir, viewDir), 0);
	//计算镜面反射分量
	float spec = pow(orientation, 32);
	vec3 specular = specularStrength * spec * lightColor;
	//漫反射分量
	vec3 ambient  = lightColor * ambientStrength;
	vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}