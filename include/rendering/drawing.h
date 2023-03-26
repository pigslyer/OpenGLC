#ifndef DRAWING_H_SEEN
#define DRAWING_H_SEEN

#include <global.h>

#include <rendering/drawingText.h>

typedef unsigned int ObjID;

struct texture
{
	ObjID id;
	int width, height;
} typedef texture;

void drawInit(void);
void drawTerminate(void);

// clears all vbos
void drawClear(void);

vec2f screenToVertexPos(vec2f screenPos);
void screenToVertex(vec2f* pos, vec2f* size);

#endif