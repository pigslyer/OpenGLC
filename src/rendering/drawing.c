#include <rendering/drawing.h>
#include <rendering/shaders.h>
#include <rendering/vboManager.h>

#include <helpers/filehelper.h>

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

	shaders[UniformModulatableBasicShader] = loadShader("shaders/basicUniformModulate.vert", "shaders/basicUniformModulate.frag");
	shaders[AtlasShader] = loadShader("shaders/atlas.vert", "shaders/atlas.frag");

	drawTextInit();
}

void drawTerminate(void)
{
	drawTextTerminate();

	for (int i = 0; i < ShaderCount; i++)
	{
		glDeleteProgram(shaders[i]);
	}
}

void drawClear(void)
{
	drawTextClear();
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
	
	size->x = size->x / F(SCREEN_WIDTH / 2);
	size->y = -size->y / F(SCREEN_HEIGHT / 2);
}

void setupUniformModulateBasicVbo(void)
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
}