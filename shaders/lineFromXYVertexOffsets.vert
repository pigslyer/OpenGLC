#version 330 core

// top of wall
layout (location = 0) in float ytop;
// bottom of wall
layout (location = 1) in float ybottom;
// x to start drawing wall from
layout (location = 2) in float xbase;
// position in atlas texture
layout (location = 3) in vec2 atlasTex;

// x width of scanline
uniform float xoffset;
// x size of texture
uniform vec2 atlasTextureSize;


out vec2 texCoord;

void main()
{
	float usedVertex = (gl_VertexID < 2 ? ytop : ybottom);

	gl_Position = vec4(xbase + xoffset * (gl_VertexID & 1), usedVertex, 1.0f, 1.0f);

	texCoord = atlasTex + atlasTextureSize * vec2(gl_VertexID & 1, (gl_VertexID & 2) >> 1);
}