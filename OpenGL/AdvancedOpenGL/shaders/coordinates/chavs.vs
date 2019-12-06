#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 tex;
out vec2 TexCoords;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 model;
};

uniform mat4 view;

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0);
    TexCoords = tex;
}