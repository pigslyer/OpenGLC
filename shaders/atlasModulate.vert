#version 330 core

// the vertex position
layout (location = 0) in vec2 aVertexPosition;
// this vertex's coordinate the uniform sampler2D
layout (location = 1) in vec2 aTexCoord;

out vec2 vertexCoord;
out vec2 texCoord;

void main()
{
	gl_Position = vec4(aVertexPosition.x, aVertexPosition.y, 1.0, 1.0);
	
	vertexCoord = aVertexPosition;
	texCoord = aTexCoord;
}