#ifndef LEVEL_H_SEEN
#define LEVEL_H_SEEN

#include <global.h>

// this should contain a reference to the hit instance if it records an instance hit
// reference should be NULL otherwise to check if we collided with a wall at the end
// if that isn't null, wallTileHit should contain the tile we hit. 
// Checking wallTileHit when that reference is NULL is undefined
struct rayData
{
	int* mobData;
	vec2f globalPosHit;
	float rayLength;
	int wallTileHit;
	// [0, 1], 0 = hit left side of thing, 1 = hit right side
	float xHit;
} typedef rayData;

// I HATE THIS I HATE THIS I HATE THIS
// has to be a #define for the hHits and vHits static
// declaration in castRay
#define MAX_RAY_RESULTS  32

struct rayResults
{
	rayData* results;
	int resultCount;
} typedef rayResults;


// --------------------------------------------------------- 
// implemented in levelLogic.c
void levelLogicInit(void);
void levelUpdate(void);

void castRay(vec2f from, float direction, rayResults* results);

// ---------------------------------------------------------
// implemented in levelRendering.c
void levelDrawInit(void);
void levelDrawClear(void);
void levelDrawTerminate(void);
void levelDraw(void);


#endif