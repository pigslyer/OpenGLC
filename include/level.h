#ifndef LEVEL_H_SEEN
#define LEVEL_H_SEEN

#include <global.h>

struct rayData
{
	bool hasHit;
	vec2f globalPosHit;
	vec2i mapPosHit;
	float rayLength;
} typedef rayData;

void levelInit(void);
void levelUpdate(void);
void levelDraw(void);

rayData castRay(vec2f from, float direction);

#endif