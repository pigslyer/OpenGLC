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


// --------------------------------------------------------- 
// implemented in levelLogic.c
void levelLogicInit(void);
void levelUpdate(void);

rayData castRay(vec2f from, float direction);

// ---------------------------------------------------------
// implemented in levelRendering.c
void levelDrawInit(void);
void levelDrawClear(void);
void levelDrawTerminate(void);
void levelDraw(void);


#endif