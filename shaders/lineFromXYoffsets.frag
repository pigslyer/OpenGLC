#version 330 core

out vec4 FragColor;

uniform vec4 modulate;

void main()
{
	FragColor = modulate;
}