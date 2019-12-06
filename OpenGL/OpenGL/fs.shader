#version 330 core
in vec3 Pos;
out vec4 FragColor;
void main()
{
    FragColor = vec4(Pos, 1.0f);//指定片段着色器最终渲染的颜色
};