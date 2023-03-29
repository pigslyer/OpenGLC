#include <game/level.h>

#include <rendering/drawing.h>
#include <rendering/shaders.h>
#include <game/player.h>

#include <paths/shaderPaths.h>

#define CEIL_COLOR COLOR3_1(1.0f, 0.0f, 0.0f)
#define FLOOR_COLOR COLOR3_1(0.3f, 0.4f, 0.112668625789f)

enum VAOS
{
	CEILING, WALL, FLOOR
};

// CEIL_FLOOR = ceiling to floor, since we need both for the wall VAO
enum VBOS
{
	CEIL_HEIGHT, FLOOR_HEIGHT, CEIL_FLOOR_HEIGHT, X
};

// 0 is ceiling, 1 will be walls, 2 is floor
static ObjID vao[3];

static size_t posInBuffer;
static float* buffers[3];

static ObjID vbo[4];

static ObjID ceilOrFloorShader;
static ShdLoc modulate, xoffset, ybase;

void levelDrawInit(void)
{

	// generating opengl buffers
	glGenVertexArrays(3, vao);
	glGenBuffers(4, vbo);

	// generating shader
	ceilOrFloorShader = loadShader(LINE_FROM_XY_VERT, LINE_FROM_XY_FRAG);

	modulate = glGetUniformLocation(ceilOrFloorShader, "modulate");
	xoffset = glGetUniformLocation(ceilOrFloorShader, "xoffset");
	ybase = glGetUniformLocation(ceilOrFloorShader, "ybase");

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
	glUniform1f(xoffset, step);

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

	// generating the buffers
	// we definitely don't need more than the raycast count lines, ever
	buffers[CEIL_HEIGHT] = (float*)malloc((size_t)RENDER_RAYCAST_COUNT * sizeof(float));
	buffers[FLOOR_HEIGHT] = (float*)malloc((size_t)RENDER_RAYCAST_COUNT * sizeof(float));

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

	free(buffers[CEIL_HEIGHT]);
	free(buffers[FLOOR_HEIGHT]);
}

void levelDraw(void)
{
	
//	// overhead view
//	glLineWidth(8);
//	glBegin(GL_QUADS);
//
//	vec2i tl;
//
//	for (int x = 0; x < mapWidth; x++)
//	{
//		tl.x = x * CELL_SIZE;
//
//		for (int y = 0; y < mapHeight; y++)
//		{
//			tl.y = y * CELL_SIZE;
//
//			if (map[y * mapWidth + x])
//			{
//				glColor3f(0, 0, 0);
//			}
//			else
//			{
//				glColor3f(1, 1, 1);
//			}
//
//			glVertex2i(tl.x + 1, tl.y + 1);
//			glVertex2i(tl.x + CELL_SIZE - 1, tl.y + 1);
//			glVertex2i(tl.x + CELL_SIZE - 1, tl.y + CELL_SIZE - 1);
//			glVertex2i(tl.x + 1, tl.y + CELL_SIZE - 1);
//		}
//	}
//
//	glEnd();

	// "3d" view

	const float ANGLE_STEP = PLAYER_FOV / F(RENDER_RAYCAST_COUNT);
	const float DRAW_STEP = F(VIEWPORT_WIDTH) / F(RENDER_RAYCAST_COUNT);

	float curAngle = playerRotation + PLAYER_FOV * 0.5f;
	float curDraw = DRAW_STEP * 0.5f;
	
	float lineHeight, unmaxLineHeight, lineOff;
	rayData collisionData;

	float breathingAnim = sinf(F(glfwGetTime()) * 1.2f) * 10.0f + 10.0f;
	
	for (int i = 0; i < RENDER_RAYCAST_COUNT; i++)
	{
		collisionData = castRay(playerPosition, curAngle);
	
		if (collisionData.hasHit)
		{
			unmaxLineHeight = F(50 * VIEWPORT_HEIGHT) / (collisionData.rayLength * cosf(playerRotation - curAngle));
			lineHeight = MIN(unmaxLineHeight, F(VIEWPORT_HEIGHT));
			lineOff = F(VIEWPORT_HEIGHT / 2) - lineHeight * 0.5f;

			// we have to offset this from the ceiling
			buffers[CEIL_HEIGHT][posInBuffer] = -1.0f - LIN_MAP(lineOff + breathingAnim, 0.0f, F(VIEWPORT_HEIGHT), -1.0f, 1.0f);
			
			buffers[FLOOR_HEIGHT][posInBuffer] = 1.0f - LIN_MAP(lineHeight + lineOff + breathingAnim, 0.0f, F(VIEWPORT_HEIGHT), -1.0f, 1.0f);

			posInBuffer++;

			//positionInBuffer += fillInLine(tempBuffer + positionInBuffer, VEC2F2_1(curDraw, 0), VEC2F2_1(curDraw, lineOff + breathingAnim), DRAW_STEP);

			// using primitives until i set up instancing for texture lines
			drawLineColored(curDraw, lineOff + breathingAnim, curDraw, lineHeight + lineOff + breathingAnim, DRAW_STEP, COLOR3_1(0.0f, 0.0f, 0.7f));

			//positionInBuffer += fillInLine(tempBuffer + positionInBuffer, VEC2F2_1(curDraw, lineHeight + lineOff + breathingAnim), VEC2F2_1(curDraw, F(VIEWPORT_HEIGHT)), DRAW_STEP);

			//drawLineColored(curDraw, 0, curDraw, lineOff + breathingAnim, DRAW_STEP, COLOR3_1(0.4f, 0.4f, 0.4f));
			//drawLineColored(curDraw, lineOff + breathingAnim, curDraw, lineHeight + lineOff + breathingAnim, DRAW_STEP, COLOR3_1(0.0f, 0.0f, 0.7f));
			//drawLineColored(curDraw, lineHeight + lineOff + breathingAnim, curDraw, F(VIEWPORT_HEIGHT), DRAW_STEP, COLOR3_1(1.0f, 1.0f, 1.0f));
		}

		curDraw += DRAW_STEP;
		curAngle -= ANGLE_STEP;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo[CEIL_HEIGHT]);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(posInBuffer * sizeof(float)), buffers[CEIL_HEIGHT], GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[FLOOR_HEIGHT]);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(posInBuffer * sizeof(float)), buffers[FLOOR_HEIGHT], GL_DYNAMIC_DRAW);

	glUseProgram(ceilOrFloorShader);

	// ceiling
	glBindVertexArray(vao[CEILING]);
	glUniform1f(ybase, 1.0f);
	glUniform4f(modulate, COLOR1_4(CEIL_COLOR));

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 4 * (GLsizei) posInBuffer);

	// floor
	glBindVertexArray(vao[FLOOR]);
	glUniform1f(ybase, -1.0f);
	glUniform4f(modulate, COLOR1_4(FLOOR_COLOR));

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 4 * (GLsizei)posInBuffer);

	ERROR("errors?");
}
