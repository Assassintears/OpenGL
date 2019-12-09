#version 330 core
out vec4 FragColor;

//声明结构体
struct Material {
	vec3 ambient;//背景光
	vec3 diffuse;//漫反射光
	vec3 specular;//镜面反射光
	float shininess;//反光度，影响镜面高光的散射/半径
};

struct Light {
    vec3 position;//光源位置
    vec3 ambient;//背景光强度
    vec3 diffuse;//漫反射强度
    vec3 specular;//镜面反射强度
};


uniform Material material;
uniform Light light;

in vec3 Normal;//模型每个顶点的法向量
in vec3 worldPos;//观察空间中模型顶点位置
//所有光照的计算都是在片段着色器里进行

void main()
{
	vec3 norm = normalize(Normal);
	//计算背景光
	vec3 ambient  = light.ambient * material.ambient;
	//计算漫反射光
	vec3 lightDir = normalize(light.position - worldPos);//光线方向
	float diff = max(dot(lightDir, norm), 0);//计算光线与法线的方向余弦
	vec3 diffuse = light.diffuse * (diff * material.diffuse);
	//计算镜面反射
	vec3 viewPos = vec3(0.0f, 0.0f, 0.0f);//观察空间中观察者的坐标为原点	
	vec3 viewDir = normalize(viewPos - worldPos);//观察方向
	//使用reflect函数计算镜面反射光
	vec3 reflectDir = reflect(lightDir, norm);//镜面反射光向量，reflect要求第一个向量的方向是从光源指向目标
	//计算镜面反射光与观察方向之间的夹角
	float spec = pow(max(dot(reflectDir, viewDir), 0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular); 
	//漫反射分量
	vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}