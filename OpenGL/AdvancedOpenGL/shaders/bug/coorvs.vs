#version 330 core
layout (location = 0) in vec3 aPos;

// layout (std140) uniform Matrices
// {
//     mat4 projection;
//     mat4 model;
// };

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;


void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
    // gl_Position = vec4(view[0][0], 0.f, 0.0f, 1.0);

}