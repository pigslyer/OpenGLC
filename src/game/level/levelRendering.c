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
	CEILING, WALL, FLOOR, OVERHEAD, VAO_COUNT
};

#define RECT_COORD_MAX_COUNT 500

enum VBOS
{
	CEIL_HEIGHT, FLOOR_HEIGHT, ATLAS_POS, X, OVERHEAD_COORDS, VBO_COUNT
};

static const int wallFramesH = 1;
static const int wallFramesV = 1;
static const int wallActiveTexture = 14;
static texture wallTexture;

static ObjID vao[VAO_COUNT];

static size_t posInBuffer;
static float* buffers[VBO_COUNT];

static ObjID vbo[VBO_COUNT];

static ObjID ceilOrFloorShader;
static ShdLoc cfModulate, cfXoffset, cfYbase;

static ObjID wallShader;
static ShdLoc wModulate, wOffset;

static ObjID rectShader;
static ShdLoc rModulate;

void levelDrawInit(void)
{

	// generating opengl buffers
	glGenVertexArrays(VAO_COUNT, vao);
	glGenBuffers(VBO_COUNT, vbo);

	// generating ceiling and floor shader
	ceilOrFloorShader = loadShader(LINE_FROM_XY_VERT, FRAG_EQUALS_MODULATE_UNIFORM_FRAG);

	cfModulate = glGetUniformLocation(ceilOrFloorShader, "modulate");
	cfXoffset = glGetUniformLocation(ceilOrFloorShader, "xoffset");
	cfYbase = glGetUniformLocation(ceilOrFloorShader, "ybase");

	// generating wall shader
	wallShader = loadShader(LINE_FROM_XY_VERTEX_VERT, ATLAS_MODULATE_FRAG);

	wModulate = glGetUniformLocation(wallShader, "modulate");
	wOffset = glGetUniformLocation(wallShader, "xoffset");

	// generating rectangle shader
	rectShader = loadShader(GL_POSITION_EQUALS_UV_2, FRAG_EQUALS_MODULATE_UNIFORM_FRAG);
	rModulate = glGetUniformLocation(rectShader, "modulate");

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

	// binding and setting up rect VAO
	glBindVertexArray(vao[OVERHEAD]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[OVERHEAD_COORDS]);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);

	// generating the buffers
	// we definitely don't need more than the raycast count lines, ever
	buffers[CEIL_HEIGHT] = 		(float*)malloc((size_t)RENDER_RAYCAST_COUNT * sizeof(float));
	buffers[FLOOR_HEIGHT] = 	(float*)malloc((size_t)RENDER_RAYCAST_COUNT * sizeof(float));
	buffers[ATLAS_POS] = 		(float*)malloc((size_t)RENDER_RAYCAST_COUNT * 2 * sizeof(float));
	buffers[OVERHEAD_COORDS] = 		(float*)malloc(RECT_COORD_MAX_COUNT * 2 * sizeof(float));

	// generate wall textures
	glBindVertexArray(vao[WALL]);

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
	glDeleteVertexArrays(VAO_COUNT, vao);
	glDeleteBuffers(VBO_COUNT, vbo);
	
	glDeleteProgram(ceilOrFloorShader);
	glDeleteProgram(wallShader);

	glDeleteTextures(1, &wallTexture.id);

	free(buffers[CEIL_HEIGHT]);
	free(buffers[FLOOR_HEIGHT]);
	free(buffers[ATLAS_POS]);
	free(buffers[OVERHEAD_COORDS]);
}

void levelDraw(void)
{
	// "3d" view

	const float ANGLE_STEP = PLAYER_FOV / F(RENDER_RAYCAST_COUNT);
	const float DRAW_STEP = F(VIEWPORT_WIDTH) / F(RENDER_RAYCAST_COUNT);

	float curAngle = playerRotation + PLAYER_FOV * 0.5f;
	float curDraw = DRAW_STEP * 0.5f;
	
	float lineHeight, unmaxLineHeight, lineOff;
	rayResults collisionData;
	rayData* currentData;

	float breathingAnim = sinf(F(glfwGetTime()) * 1.2f) * 10.0f + 10.0f;
	vec2i atlasPos;
	for (int i = 0; i < RENDER_RAYCAST_COUNT; i++)
	{
		castRay(playerPosition, curAngle, &collisionData);
		
		// process mob collisions

		// we've hit a wall
		if (collisionData.resultCount > 0 && (currentData = &collisionData.results[collisionData.resultCount-1])->mobData == NULL)
		{
			unmaxLineHeight = F(50 * VIEWPORT_HEIGHT) / (currentData->rayLength * cosf(playerRotation - curAngle));
			lineHeight = unmaxLineHeight;
			lineOff = F(VIEWPORT_HEIGHT / 2) - lineHeight * 0.5f;

			buffers[CEIL_HEIGHT][posInBuffer] = LIN_MAP(lineOff + breathingAnim, 0.0f, F(VIEWPORT_HEIGHT), -1.0f, 1.0f);;
			buffers[FLOOR_HEIGHT][posInBuffer] = LIN_MAP(lineHeight + lineOff + breathingAnim, 0.0f, F(VIEWPORT_HEIGHT), -1.0f, 1.0f);

			atlasPos = VEC2I2_1(currentData->wallTileHit % wallFramesH, currentData->wallTileHit / wallFramesH);
			
			buffers[ATLAS_POS][(posInBuffer * 2) + 0] = F(atlasPos.x) * (1.0f / F(wallFramesH)) + currentData->xHit;
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

	// overhead map
	if (GET_KEY(GLFW_KEY_TAB))
	{
		const float anchorLeft = -0.8f, anchorRight = 0.8f, anchorTop = -0.8f, anchorBottom = 0.8f;
		const float viewportWidth = anchorRight - anchorLeft, viewportHeight = anchorBottom - anchorTop;


		// background
		float rect[] = 
		{
			// triangle 1
			// top left
			anchorLeft, anchorTop,

			// top right
			anchorRight, anchorTop,

			// bottom right
			anchorRight, anchorBottom,

			// triangle 2
			// top left
			anchorLeft, anchorTop,
			
			// bottom left
			anchorLeft, anchorBottom,

			// bottom right
			anchorRight, anchorBottom,
		};

		glUseProgram(rectShader);
		glUniform4f(rModulate, COLOR1_4(COLOR4_1(0.8f, 1.0f, 0.8f, 1.0f)));

		glBindVertexArray(vao[OVERHEAD]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[OVERHEAD_COORDS]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_DYNAMIC_DRAW);

		glDrawArrays(GL_TRIANGLES, 0, sizeof(rect) / sizeof(float));

		// walls
		const float tileOffset = 0.001f;
		float tileWidth = viewportWidth / F(mapWidth), tileHeight = viewportHeight / F(mapHeight);

		size_t pos = 0;

		for (int x = 0; x < mapWidth; x++)
		{
			for (int y = 0; y < mapHeight; y++)
			{
				if (map[(mapHeight - y - 1) * mapWidth + x] != 0)
				{
					// triangle 1
					// bottom left
					buffers[OVERHEAD_COORDS][pos + 0] = anchorLeft + F(x + 0) * tileWidth + tileOffset;
					buffers[OVERHEAD_COORDS][pos + 1] = anchorTop +  F(y + 0) * tileHeight + tileOffset;
					
					// bottom right
					buffers[OVERHEAD_COORDS][pos + 2] = anchorLeft + F(x + 1) * tileWidth - tileOffset * 2;
					buffers[OVERHEAD_COORDS][pos + 3] = anchorLeft + F(y + 0) * tileWidth + tileOffset;

					// top left
					buffers[OVERHEAD_COORDS][pos + 4] = anchorLeft + F(x + 0) * tileWidth + tileOffset;
					buffers[OVERHEAD_COORDS][pos + 5] = anchorLeft + F(y + 1) * tileWidth - tileOffset * 2;

					// triangle 2
					// bottom left
					buffers[OVERHEAD_COORDS][pos + 6] = anchorLeft + F(x + 1) * tileWidth - tileOffset * 2;
					buffers[OVERHEAD_COORDS][pos + 7] = anchorTop  + F(y + 0) * tileHeight + tileOffset;
					
					// top right
					buffers[OVERHEAD_COORDS][pos + 8] = anchorLeft + F(x + 1) * tileWidth - tileOffset * 2;
					buffers[OVERHEAD_COORDS][pos + 9] = anchorTop  + F(y + 1) * tileWidth - tileOffset * 2;

					// top left
					buffers[OVERHEAD_COORDS][pos + 10] = anchorLeft + F(x + 0) * tileWidth + tileOffset;
					buffers[OVERHEAD_COORDS][pos + 11] = anchorTop  + F(y + 1) * tileWidth - tileOffset * 2;

					pos += 12;
				} 
			}
		}

		glUniform4f(rModulate, COLOR1_4(COLOR4_1(0.0f, 0.0f, 0.0f, 0.8f)));

		glBufferData(GL_ARRAY_BUFFER, I(sizeof(float) * pos), buffers[OVERHEAD_COORDS], GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, I(pos / 2));

		const float playerRadius = 10;

		// player circle
		vec2f mappedPlayer = VEC2F2_1(
			LIN_MAP(playerPosition.x, 0, F(mapWidth * 64), anchorLeft, anchorRight),
			LIN_MAP(playerPosition.y, F(mapHeight * 64), 0, anchorTop, anchorBottom)
		);

		getCircleUVs(mappedPlayer, playerRadius, 100, buffers[OVERHEAD_COORDS]);

		glBufferData(GL_ARRAY_BUFFER, 200 * sizeof(float), buffers[OVERHEAD_COORDS], GL_DYNAMIC_DRAW);
		glDrawArrays(GL_LINE_LOOP, 0, 100);

		// player sight lines

		float sin, cos;
		sincosf(playerRotation, &sin, &cos);

		vec2f playerEyes = VEC2F2_1(
			mappedPlayer.x + LIN_MAP(playerRadius, 0, F(SCREEN_WIDTH), 0, 2) * cos,
			mappedPlayer.y + LIN_MAP(playerRadius, 0, F(SCREEN_HEIGHT), 0, 2) * sin
		);

		rayResults coll;
		rayData* wallData;

		castRay(playerPosition, playerRotation - F(PLAYER_FOV / 2), &coll);

		if (coll.resultCount > 0 && (wallData = &(coll.results[coll.resultCount - 1]))->mobData == NULL)
		{
			float ray[] = 
			{
				playerEyes.x, playerEyes.y,
				LIN_MAP(wallData->globalPosHit.x, 0, F(mapWidth * 64), anchorLeft, anchorRight),
				LIN_MAP(wallData->globalPosHit.y, F(mapHeight * 64), 0, anchorTop, anchorBottom),
			};

			glBufferData(GL_ARRAY_BUFFER, sizeof(ray), ray, GL_DYNAMIC_DRAW);
			glDrawArrays(GL_LINES, 0, 2);

		}

		castRay(playerPosition, playerRotation + F(PLAYER_FOV / 2), &coll);

		if (coll.resultCount > 0 && (wallData = &(coll.results[coll.resultCount - 1]))->mobData == NULL)
		{
			float ray[] = 
			{
				playerEyes.x, playerEyes.y,
				LIN_MAP(wallData->globalPosHit.x, 0, F(mapWidth * 64), anchorLeft, anchorRight),
				LIN_MAP(wallData->globalPosHit.y, F(mapHeight * 64), 0, anchorTop, anchorBottom),
			};

			glBufferData(GL_ARRAY_BUFFER, sizeof(ray), ray, GL_DYNAMIC_DRAW);
			glDrawArrays(GL_LINES, 0, 2);

		}


	}
}
