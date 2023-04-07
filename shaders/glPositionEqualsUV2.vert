#version 330 core

layout (location = 0) in vec2 UV;

void main()
{
	gl_Position = vec4(UV, 1.0f, 1.0f);
}