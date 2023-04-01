#include <game/level.h>

#include <rendering/drawing.h>
#include <rendering/shaders.h>
#include <game/player.h>

#include <helpers/filehelper.h>

#include <paths/shaderPaths.h>
#include <paths/texturePaths.h>

#define CEIL_COLOR COLOR3_1(0.4f, 0.4f, 0.6f)
#define FLOOR_COLOR COLOR3_1(1.0f, 1.0f, 1.0f)
#define DEFAULT_WALL_MODULATE COLOR3_1(1.0f, 1.0f, 1.0f)

enum VAOS
{
	CEILING, WALL, FLOOR
};

enum VBOS
{
	CEIL_HEIGHT, FLOOR_HEIGHT, ATLAS_POS, X 
};

static const int wallFramesH = 1;
static const int wallFramesV = 1;
static const int wallActiveTexture = 14;
static texture wallTexture;

static ObjID vao[3];

static size_t posInBuffer;
static float* buffers[3];

static ObjID vbo[4];

static ObjID ceilOrFloorShader;
static ShdLoc cfModulate, cfXoffset, cfYbase;

static ObjID wallShader;
static ShdLoc wModulate, wOffset;

void levelDrawInit(void)
{

	// generating opengl buffers
	glGenVertexArrays(3, vao);
	glGenBuffers(4, vbo);

	// generating ceiling and floor shader
	ceilOrFloorShader = loadShader(LINE_FROM_XY_VERT, FRAG_EQUALS_MODULATE_UNIFORM_FRAG);

	cfModulate = glGetUniformLocation(ceilOrFloorShader, "modulate");
	cfXoffset = glGetUniformLocation(ceilOrFloorShader, "xoffset");
	cfYbase = glGetUniformLocation(ceilOrFloorShader, "ybase");

	// generating wall shader
	wallShader = loadShader(LINE_FROM_XY_VERTEX_VERT, ATLAS_MODULATE_FRAG);

	wModulate = glGetUniformLocation(wallShader, "modulate");
	wOffset = glGetUniformLocation(wallShader, "xoffset");

	// calculating xoffsets and xbases

	float* xs = (float*) malloc((size_t)RENDER_RAYCAST_COUNT * sizeof(float));

	const float step = 2.0f / F(RENDER_RAYCAST_COUNT);

	float curX = -1.0f;
	for (int i = 0; i < RENDER_RAYCAST_COUNT; i++, curX += step)
	{
		xs[i] = curX;
	}


	// we can just set the xoffset now, seeing as it is static
	glUseProgram(ceilOrFloorShader);
	glUniform1f(cfXoffset, step);

	glUseProgram(wallShader);
	glUniform1f(wOffset, step);

	// save the xs to a buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo[X]);
	glBufferData(GL_ARRAY_BUFFER, (size_t)(RENDER_RAYCAST_COUNT) * sizeof(float), xs, GL_STATIC_DRAW);

	free(xs);


	// binding and setting up ceiling vao
	glBindVertexArray(vao[CEILING]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[CEIL_HEIGHT]);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribDivisor(0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[X]);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 4);

	// binding and setting up floor vao
	glBindVertexArray(vao[FLOOR]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[FLOOR_HEIGHT]);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribDivisor(0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[X]);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 4);

	// binding and setting up wall vao
	glBindVertexArray(vao[WALL]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[FLOOR_HEIGHT]);

	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribDivisor(0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[CEIL_HEIGHT]);
	
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 4);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[X]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 4);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[ATLAS_POS]);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 4);

	// generating the buffers
	// we definitely don't need more than the raycast count lines, ever
	buffers[CEIL_HEIGHT] = (float*)malloc((size_t)RENDER_RAYCAST_COUNT * sizeof(float));
	buffers[FLOOR_HEIGHT] = (float*)malloc((size_t)RENDER_RAYCAST_COUNT * sizeof(float));
	buffers[ATLAS_POS] = (float*)malloc((size_t)RENDER_RAYCAST_COUNT * 2 * sizeof(float));

	// generate wall textures
	int channelNum;
	unsigned char* textureData = readBmp(WALLS, &wallTexture.width, &wallTexture.height, &channelNum);

	glActiveTexture(GL_TEXTURE0 + (GLenum) wallActiveTexture);

	glGenTextures(1, &wallTexture.id);
	glBindTexture(GL_TEXTURE_2D, wallTexture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wallTexture.width, wallTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

	glActiveTexture(GL_TEXTURE0);

	free(textureData);

	vec2f atlasTextureSize = VEC2F2_1((1.0f / F(wallFramesH)) * 2.0f / F(RENDER_RAYCAST_COUNT), -1.0f / F(wallFramesV));

	// set up wall shader with texture data
	glUseProgram(wallShader);
	glUniform1i(glGetUniformLocation(wallShader, "atlasTexture"), wallActiveTexture);
	glUniform2f(glGetUniformLocation(wallShader, "atlasTextureSize"), -atlasTextureSize.x, atlasTextureSize.y);
	
	printf("%f, %f\n", atlasTextureSize.x, atlasTextureSize.y);

	ERROR("no errors?");
}

void levelDrawClear(void)
{
	// the only thing determining how "much" data is saved is the position in the buffer
	// we don't actually have to 0 our temporary buffers
	posInBuffer = 0;
}

void levelDrawTerminate(void)
{
	glDeleteVertexArrays(3, vao);
	glDeleteBuffers(4, vbo);
	
	glDeleteProgram(ceilOrFloorShader);
	glDeleteProgram(wallShader);

	glDeleteTextures(1, &wallTexture.id);

	free(buffers[CEIL_HEIGHT]);
	free(buffers[FLOOR_HEIGHT]);
	free(buffers[ATLAS_POS]);
}

void levelDraw(void)
{

	// "3d" view

	const float ANGLE_STEP = PLAYER_FOV / F(RENDER_RAYCAST_COUNT);
	const float DRAW_STEP = F(VIEWPORT_WIDTH) / F(RENDER_RAYCAST_COUNT);

	float curAngle = playerRotation + PLAYER_FOV * 0.5f;
	float curDraw = DRAW_STEP * 0.5f;
	
	float lineHeight, unmaxLineHeight, lineOff;
	rayData collisionData;

	float breathingAnim = sinf(F(glfwGetTime()) * 1.2f) * 10.0f + 10.0f;
	vec2i atlasPos;
	for (int i = 0; i < RENDER_RAYCAST_COUNT; i++)
	{
		collisionData = castRay(playerPosition, curAngle);
	
		if (collisionData.hasHit)
		{
			unmaxLineHeight = F(50 * VIEWPORT_HEIGHT) / (collisionData.rayLength * cosf(playerRotation - curAngle));
			lineHeight = MIN(unmaxLineHeight, F(VIEWPORT_HEIGHT));
			lineOff = F(VIEWPORT_HEIGHT / 2) - lineHeight * 0.5f;

			buffers[CEIL_HEIGHT][posInBuffer] = LIN_MAP(lineOff + breathingAnim, 0.0f, F(VIEWPORT_HEIGHT), -1.0f, 1.0f);;
			buffers[FLOOR_HEIGHT][posInBuffer] = LIN_MAP(lineHeight + lineOff + breathingAnim, 0.0f, F(VIEWPORT_HEIGHT), -1.0f, 1.0f);

			atlasPos = VEC2I2_1(collisionData.wallHit % wallFramesH, collisionData.wallHit / wallFramesH);
			
			buffers[ATLAS_POS][(posInBuffer * 2) + 0] = F(atlasPos.x) * (1.0f / F(wallFramesH)) + collisionData.wallPercentageHit;
			buffers[ATLAS_POS][(posInBuffer * 2) + 1] = 1.0f - F(atlasPos.y) * (1.0f / F(wallFramesV));

			posInBuffer++;
		}
		else
		{
			buffers[FLOOR_HEIGHT][posInBuffer] = buffers[CEIL_HEIGHT][posInBuffer] = 1.0f + LIN_MAP(breathingAnim, 0.0f, F(VIEWPORT_HEIGHT), -1.0f, 1.0f);
			
			posInBuffer++;
		}

		curDraw += DRAW_STEP;
		curAngle -= ANGLE_STEP;
	}


	glBindBuffer(GL_ARRAY_BUFFER, vbo[CEIL_HEIGHT]);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(posInBuffer * sizeof(float)), buffers[CEIL_HEIGHT], GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[FLOOR_HEIGHT]);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(posInBuffer * sizeof(float)), buffers[FLOOR_HEIGHT], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[ATLAS_POS]);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(posInBuffer * 2 * sizeof(float)), buffers[ATLAS_POS], GL_DYNAMIC_DRAW);

	// ceil or floor
	glUseProgram(ceilOrFloorShader);

	// ceiling
	glBindVertexArray(vao[CEILING]);
	glUniform1f(cfYbase, 1.0f);
	glUniform4f(cfModulate, COLOR1_4(CEIL_COLOR));

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 4 * (GLsizei) posInBuffer);

	// floor
	glBindVertexArray(vao[FLOOR]);
	glUniform1f(cfYbase, -1.0f);
	glUniform4f(cfModulate, COLOR1_4(FLOOR_COLOR));

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 4 * (GLsizei)posInBuffer);

	// wall
	glUseProgram(wallShader);
	glUniform4f(wModulate, COLOR1_4(DEFAULT_WALL_MODULATE));
	glBindVertexArray(vao[WALL]);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 4 * (GLsizei)posInBuffer);

	ERROR("errors?");
}
