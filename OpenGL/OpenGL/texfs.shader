#version 330 core
out vec4 FragColor;

//in vec3 ourColor;
in vec2 TexCoord;

// 纹理采样器，从纹理缓冲对象中采集纹理
uniform sampler2D ourTexture;

void main()
{
	FragColor = texture(ourTexture, TexCoord);//片段着色器：输出颜色或者纹理
}