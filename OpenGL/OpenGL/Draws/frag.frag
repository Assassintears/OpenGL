#version 450

in vec3 frag;
layout (location = 0) out vec4 lastColor;

void main()
{
	lastColor = vec4(frag, 1.0f);
}