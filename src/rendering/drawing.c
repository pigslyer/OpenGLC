#include <rendering/drawing.h>
#include <rendering/shaders.h>
#include <rendering/vboManager.h>

#include <game/level.h>

#include <helpers/filehelper.h>
#include <paths/shaderPaths.h>

enum Shaders
{
	UniformModulatableBasicShader,
	AtlasShader,
	ShaderCount,
};

void setupUniformModulateBasicVbo(void);

unsigned int shaders[ShaderCount];

void drawInit(void)
{

	// global settings
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

	// glEnable(GL_TEXTURE_2D) is unneeded in non CPU profiles, or something

	// shader loading

	shaders[UniformModulatableBasicShader] = loadShader(BASIC2D_MODULATE_VERT, BASIC2D_MODULATE_FRAG);
	shaders[AtlasShader] = loadShader(ATLAS_MODULATE_VERT, ATLAS_MODULATE_FRAG);

	drawTextInit();
	drawPrimitivesInit();
	levelDrawInit();
}

void drawTerminate(void)
{
	drawTextTerminate();
	drawPrimitivesTerminate();
	levelDrawTerminate();

	for (int i = 0; i < ShaderCount; i++)
	{
		glDeleteProgram(shaders[i]);
	}
}

void drawClear(void)
{
	drawTextClear();
	drawPrimitivesClear();
	levelDrawClear();
}


vec2f screenToVertexPos(vec2f screenPos)
{
	screenPos.x = LIN_MAP(screenPos.x, 0.0f, F(VIEWPORT_WIDTH), -1.0f, 1.0f);
	screenPos.y = LIN_MAP(screenPos.y, 0.0f, F(VIEWPORT_HEIGHT), 1.0f, -1.0f);
	
	return screenPos;
}

void screenToVertex(vec2f* pos, vec2f* size)
{
	*pos = screenToVertexPos(*pos);
	
	size->x = size->x / F(VIEWPORT_WIDTH / 2);
	size->y = -size->y / F(VIEWPORT_HEIGHT / 2);
}

vec2f screenSizeToNorm(vec2f org)
{
	return (vec2f){org.x / F(VIEWPORT_WIDTH / 2), -org.y / F(VIEWPORT_HEIGHT / 2)};
}

void setupUniformModulateBasicVbo(void)
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
}