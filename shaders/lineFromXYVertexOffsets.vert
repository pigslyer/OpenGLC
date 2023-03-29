#version 330 core

layout (location = 0) in float ytop;
layout (location = 1) in float ybottom;
layout (location = 2) in float xbase;

uniform float xoffset;
// currently there's a weird unaccounted offset with breathing, don't know why
uniform float yoffset;

void main()
{
	float usedVertex = (gl_VertexID < 2 ? ytop : ybottom);

	gl_Position = vec4(xbase + xoffset * (gl_VertexID & 1), usedVertex, 1.0f, 1.0f);
}