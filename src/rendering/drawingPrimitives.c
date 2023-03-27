#include <rendering/drawingPrimitives.h>

#include <rendering/shaders.h>
#include <rendering/vboManager.h>
#include <paths/shaderPaths.h>

// if we're using raycasts we're doing ~100 * 3 line draws minimum
const int MAX_PRIMITIVES = 500;

vboManager* primitivesVbo = NULL;

ObjID primitiveShader = NO_PROGRAM;
ShdLoc modulateLocation = NO_LOCATION;

void setupPrimitiveVbo(void);

void drawPrimitivesInit(void)
{
	primitivesVbo = initVboMan(MAX_PRIMITIVES, setupPrimitiveVbo);

	primitiveShader = loadShader(BASIC2D_MODULATE_VERT, BASIC2D_MODULATE_FRAG);
	modulateLocation = glGetUniformLocation(primitiveShader, "modulation");
}

void drawPrimitivesTerminate(void)
{
	freeVboMan(primitivesVbo);
	glDeleteProgram(primitiveShader);
	
	primitivesVbo = NULL;
	primitiveShader = NO_PROGRAM;
	modulateLocation = NO_LOCATION;
}

void drawPrimitivesClear(void)
{
	clearVbos(primitivesVbo);
}

// glLineWidth doesn't make lines wide enough for my needs, so we're faking it with quads
void drawLineColoredv(vec2f from, vec2f to, float width, vec4f color)
{
	from = screenToVertexPos(from);
	to = screenToVertexPos(to);
	width = screenSizeToNorm((vec2f){width, 0.0f}).x * 0.5f;

	float vertexData[] = 
	{
		// first triangle
		// top left
		from.x - width, 
		from.y, 

		// bottom left
		to.x - width, 
		to.y,

		// bottom right
		to.x + width,
		to.y,

		// second triangle
		// top left
		from.x - width, 
		from.y, 

		// top right
		to.x + width, 
		from.y,

		// bottom right
		to.x + width,
		to.y,
	};

	bindUnusedVbo(primitivesVbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_DYNAMIC_DRAW);

	glUseProgram(primitiveShader);
	glUniform4f(modulateLocation, COLOR1_4(color));

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void setupPrimitiveVbo(void)
{
	// vertex position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) (0));
	glEnableVertexAttribArray(0);
}