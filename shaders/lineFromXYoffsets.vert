#version 330 core

layout (location = 0) in float yoffset;
layout (location = 1) in float xbase;

uniform float xoffset;
uniform float ybase;

void main()
{
	float usingy = gl_VertexID < 2 ? ybase : yoffset;
	gl_Position = vec4(xbase + xoffset * (gl_VertexID & 1), usingy, 1.0f, 1.0f);
}