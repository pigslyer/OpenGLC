#version 330 core

out vec4 FragColor;

uniform vec4 modulation;

void main()
{
	FragColor = modulation;
}