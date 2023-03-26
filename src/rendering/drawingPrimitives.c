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

void drawLineColoredv(vec2f from, vec2f to, vec4f color)
{
	from = screenToVertexPos(from);
	to = screenToVertexPos(to);

	float vertexData[] = 
	{
		from.x, 
		from.y, 
		to.x, 
		to.y,
	};

	bindUnusedVbo(primitivesVbo);

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), vertexData, GL_DYNAMIC_DRAW);

	glUseProgram(primitiveShader);
	glUniform4f(modulateLocation, COLOR1_4(color));

	glDrawArrays(GL_LINES, 0, 2);
}

void setupPrimitiveVbo(void)
{
	// vertex position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) (0));
	glEnableVertexAttribArray(0);
}