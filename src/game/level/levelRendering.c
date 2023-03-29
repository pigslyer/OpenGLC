#include <game/level.h>

#include <rendering/drawing.h>
#include <rendering/shaders.h>
#include <game/player.h>

#include <paths/shaderPaths.h>

#define MAX_LINES 500

#define CEIL_COLOR COLOR3_1(0.1f,1,1)
#define FLOOR_COLOR COLOR3_1(0.3f, 0.4f, 0.112668625789f)

size_t positionInBuffer;
float* tempBuffer;

ObjID VAO;
ObjID VBO_instance, VBO_shared;
ObjID lineShader;

#define test(a) printf("got here %d\n", a);

void levelDrawInit(void)
{
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO_instance);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_instance);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 100, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
//	glVertexAttribDivisor(0, 1);

	// shader loading
	lineShader = loadShader(LINE_MODULATE_INSTANCE_VERT, LINE_MODULATE_INSTANCE_FRAG);

	glUseProgram(lineShader);

	float colors[] = 
	{
		COLOR1_4(CEIL_COLOR),
		COLOR1_4(FLOOR_COLOR),
	};

	glGenBuffers(1, &VBO_shared);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_shared);

	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	ERROR("errors?");	
	
	tempBuffer = (float*) malloc(MAX_LINES * (6 * 2 * sizeof(float)));
}

void levelDrawClear(void)
{
	positionInBuffer = 0;
}

void levelDrawTerminate(void)
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO_instance);

	VAO = VBO_instance = 0;

	glDeleteProgram(lineShader);
	lineShader = NO_PROGRAM;
}

size_t fillInLine(float* dataStart, vec2f from, vec2f to, float width);

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
	
	int lineCount = 0;
	for (int i = 0; i < RENDER_RAYCAST_COUNT; i++)
	{
		collisionData = castRay(playerPosition, curAngle);
	
		if (collisionData.hasHit)
		{
			unmaxLineHeight = F(50 * VIEWPORT_HEIGHT) / (collisionData.rayLength * cosf(playerRotation - curAngle));
			lineHeight = MIN(unmaxLineHeight, F(VIEWPORT_HEIGHT));
			lineOff = F(VIEWPORT_HEIGHT / 2) - lineHeight * 0.5f;

			positionInBuffer += fillInLine(tempBuffer + positionInBuffer, VEC2F2_1(curDraw, 0), VEC2F2_1(curDraw, lineOff + breathingAnim), DRAW_STEP);

			// using primitives until i set up instancing for texture lines
			drawLineColored(curDraw, lineOff + breathingAnim, curDraw, lineHeight + lineOff + breathingAnim, DRAW_STEP, COLOR3_1(0.0f, 0.0f, 0.7f));

			positionInBuffer += fillInLine(tempBuffer + positionInBuffer, VEC2F2_1(curDraw, lineHeight + lineOff + breathingAnim), VEC2F2_1(curDraw, F(VIEWPORT_HEIGHT)), DRAW_STEP);

			lineCount += 2;			
			//drawLineColored(curDraw, 0, curDraw, lineOff + breathingAnim, DRAW_STEP, COLOR3_1(0.4f, 0.4f, 0.4f));
			//drawLineColored(curDraw, lineOff + breathingAnim, curDraw, lineHeight + lineOff + breathingAnim, DRAW_STEP, COLOR3_1(0.0f, 0.0f, 0.7f));
			//drawLineColored(curDraw, lineHeight + lineOff + breathingAnim, curDraw, F(VIEWPORT_HEIGHT), DRAW_STEP, COLOR3_1(1.0f, 1.0f, 1.0f));
		}

		curDraw += DRAW_STEP;
		curAngle -= ANGLE_STEP;
	}

//	float test[] = 
//	{
//		-1.0f, -1.0f,
//		1.0f, -1.0f,
//		1.0f, 1.0f,
//
//		-1.0f, -1.0f,
//		-1.0f, 1.0f,
//		1.0f, 1.0f,
//	};

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_instance);

	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(positionInBuffer * sizeof(float)), tempBuffer, GL_DYNAMIC_DRAW);

	glUseProgram(lineShader);

	// this draws stuff
	glDrawArraysInstanced(GL_TRIANGLES, 0, lineCount * 6, 2);
	//glDrawArrays(GL_TRIANGLES, 0, lineCount * 6);

	ERROR("hi 2");
}

size_t fillInLine(float* dataStart, vec2f from, vec2f to, float width)
{
	from = screenToVertexPos(from);
	to = screenToVertexPos(to);
	width = screenSizeToNorm(VEC2F2_1(width, 0.0f)).x;

	// first triangle
	// top left
	dataStart[0] = from.x - width;
	dataStart[1] = from.y;

	// bottom left
	dataStart[2] = to.x - width;
	dataStart[3] = to.y;

	// bottom right
	dataStart[4] = to.x + width;
	dataStart[5] = to.y;

	// second triangle
	// top left
	dataStart[6] = from.x - width;
	dataStart[7] = from.y;

	// top right
	dataStart[8] = from.x + width;
	dataStart[9] = from.y;

	// bottom right
	dataStart[10] = to.x + width;
	dataStart[11] = to.y;

	return 12;

}
