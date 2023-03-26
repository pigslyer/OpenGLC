#include <rendering/drawing.h>
#include <rendering/drawingText.h>
#include <rendering/vboManager.h>
#include <rendering/shaders.h>

#include <helpers/filehelper.h>

const int MAX_STRINGS = 8;
const int CHARACTER_COUNT_HOR = 5;
const int CHARACTER_COUNT_VERT = 6;

const float WIDTH_TO_HEIGHT_RATIO = 0.25f;

// nobody but drawingText can use this active texture
const int fontActiveTexture = 15;
texture fontAtlas;

ObjID atlasShader;

vboManager* stringVboManager;

void setupAtlasVbo(void);

void drawTextInit(void)
{
	atlasShader = loadShader("shaders/atlas.vert", "shaders/atlas.frag");

	glUseProgram(atlasShader);
	// this is a constant, might as well set it once
	glUniform1i(glGetUniformLocation(atlasShader, "atlasTexture"), fontActiveTexture);


	stringVboManager = initVboMan(MAX_STRINGS, setupAtlasVbo);

	int channelNum;
	unsigned char* fontAtlasData = readBmp("assets/fontExtended.bmp", &fontAtlas.width, &fontAtlas.height, &channelNum);

	glActiveTexture(GL_TEXTURE0 + (GLenum) fontActiveTexture);
	
	glGenTextures(1, &fontAtlas.id);
	glBindTexture(GL_TEXTURE_2D, fontAtlas.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fontAtlas.width, fontAtlas.height, 0, GL_RED, GL_UNSIGNED_BYTE, fontAtlasData);
	
	glActiveTexture(GL_TEXTURE0);

	free(fontAtlasData);


}

void drawTextTerminate(void)
{
	glDeleteShader(atlasShader);
	freeVboMan(stringVboManager);
	glDeleteTextures(1, &fontAtlas.id);
}

void drawTextClear(void)
{
	clearVbos(stringVboManager);
}


void drawText(char* text, int length, vec2f pos, vec2f size)
{
	screenToVertex(&pos, &size);
	//printf("pos x: %f, pos y: %f, size x: %f, size y: %f\n", pos.x, pos.y, size.x, size.y);

	// 6 - 3 for each of the two triangles per character, 8 - 2 for each vertex UV, 2 for atlas UV 
	const unsigned long long INDEX_PER_CHAR = 6 * (2 + 2);

	vec2f curPos = pos, curUV;
	vec2f sizePerChar = (vec2f) {size.x / F(length + 1), size.y};
	vec2f sizePerTile = (vec2f) {1.0f / F(CHARACTER_COUNT_HOR), 1.0f / F(CHARACTER_COUNT_VERT)};

	float* vertexData = (float*) malloc(((size_t)length) * INDEX_PER_CHAR * sizeof(float));

	char temp;
	long long unsigned offset = 0;
	int visibleCharacterCount = 0;
	for (int i = 0; i < length; i++)
	{
		switch (text[i])
		{
			case '!':
			curUV = (vec2f) {1.0f / 5.0f, 0.0f};
			break;

			case '?':
			curUV = (vec2f) {2.0f / 5.0f, 0.0f};
			break;

			case ',':
			curUV = (vec2f) {3.0f / 5.0f, 0.0f};
			break;

			case ' ':
			curPos.x += sizePerChar.x;
			continue;

			default:

			// temp holds lowercase letter
			temp = (text[i] | 32) - 'a';
			curUV = (vec2f) { F(temp % CHARACTER_COUNT_HOR) / F(CHARACTER_COUNT_HOR), 1.0f - F((temp) / CHARACTER_COUNT_HOR + 1) / F(CHARACTER_COUNT_VERT)};
			break;
		}

		// first triangle

		// top left position
		vertexData[offset + 0] = curPos.x; vertexData[offset + 1] = curPos.y;
		// top left UV
		vertexData[offset + 2] = curUV.x; vertexData[offset + 3] = curUV.y + sizePerTile.y;
		//vertexData[offset + 2] = 1.0f; vertexData[offset + 3] = 1.0f;
		
		// bottom left position
		vertexData[offset + 4] = curPos.x; vertexData[offset + 5] = curPos.y + sizePerChar.y;
		// bottom left UV
		vertexData[offset + 6] = curUV.x; vertexData[offset + 7] = curUV.y;
		//vertexData[offset + 6] = 0.0f; vertexData[offset + 7] = 0.0f;
		
		// bottom right position
		vertexData[offset + 8] = curPos.x + sizePerChar.x; vertexData[offset + 9] = curPos.y + sizePerChar.y;
		// bottom right UV
		vertexData[offset + 10] = curUV.x + sizePerTile.x; vertexData[offset + 11] = curUV.y;
		

		// second triangle

		// top left position
		vertexData[offset + 12] = curPos.x; vertexData[offset + 13] = curPos.y;
		// top left UV
		vertexData[offset + 14] = curUV.x; vertexData[offset + 15] = curUV.y + sizePerTile.y;
		
		// top right position
		vertexData[offset + 16] = curPos.x + sizePerChar.x; vertexData[offset + 17] = curPos.y;
		// top right UV
		vertexData[offset + 18] = curUV.x + sizePerTile.x; vertexData[offset + 19] = curUV.y + sizePerTile.y;
		
		// bottom right position
		vertexData[offset + 20] = curPos.x + sizePerChar.x; vertexData[offset + 21] = curPos.y + sizePerChar.y;
		// bottom right UV
		vertexData[offset + 22] = curUV.x + sizePerTile.x; vertexData[offset + 23] = curUV.y;
		
		curPos.x += sizePerChar.x + 0.01f;
		offset += INDEX_PER_CHAR;
		visibleCharacterCount++;
	}


	bindUnusedVbo(stringVboManager);

	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)((size_t)length * INDEX_PER_CHAR * sizeof(float)), vertexData, GL_DYNAMIC_DRAW);

	glUseProgram(atlasShader);

	// 2 triangles per character, 3 vertices per triangle point
	glDrawArrays(GL_TRIANGLES, 0, visibleCharacterCount * 2 * 3);

	free(vertexData);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

inline void drawTexth(char* text, int length, vec2f pos, float height)
{
	drawText(text, length, pos, (vec2f){1.0f / WIDTH_TO_HEIGHT_RATIO * height, height});
}

void setupAtlasVbo(void)
{
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	
	// vertex position
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	
	// atlas UV
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}