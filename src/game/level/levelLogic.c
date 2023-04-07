#include <game/level.h>
#include <game/player.h>

#include <rendering/drawing.h>

#define POWER_OF_SIZE 6
#define CELL_SIZE (1 << POWER_OF_SIZE)
#define CELL_SIZEF ((float)(CELL_SIZE))

int mapWidth, mapHeight;
int mapArea;

int map[] = 
{
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

void castRay(vec2f from, float angle, rayResults* results)
{
	// a pair of static tables we fill in as needed. they'll exist outside of all scopes,
	// we then just set which one rayResults should use
	// very clean and efficient, imo
	static rayData hHits[MAX_RAY_RESULTS];
	static rayData vHits[MAX_RAY_RESULTS];

	int dof = 0;

	float hDist = INFINITY, vDist = INFINITY;
	int hHitCount = 0, vHitCount = 0;

	float sin, cos, tan, cotan;
	sincosf(angle, &sin, &cos);
	tan = sin / cos;
	cotan = 1 / tan;

	vec2f off;
	//int mult;

	// vertical bit
	vec2f vertRes; vec2i vertMap;

	// looking right
	if (cos > 0.001f)
	{
		vertRes.x = F(((I(from.x)) >> POWER_OF_SIZE) << POWER_OF_SIZE) + CELL_SIZEF;
		vertRes.y = (from.x - vertRes.x) * tan + from.y;
		off.x = CELL_SIZEF;
		off.y = -off.x * tan;
	}
	// looking left
	else if (cos < -0.001f)
	{
		vertRes.x = F(((I(from.x)) >> POWER_OF_SIZE) << POWER_OF_SIZE) - 0.0001f;
		vertRes.y = (from.x - vertRes.x) * tan + from.y;
		off.x = -CELL_SIZEF;
		off.y = -off.x * tan;
	}
	// god knows where we're looking
	else
	{
		vertRes = from;
		dof = RENDER_RAYCAST_DOF_MAX;
	}


	while (dof < RENDER_RAYCAST_DOF_MAX)
	{
		vertMap.x = I(vertRes.x) >> POWER_OF_SIZE;
		vertMap.y = I(vertRes.y) >> POWER_OF_SIZE;

		if (vertMap.x >= 0 && vertMap.x < mapWidth && vertMap.y >= 0 && vertMap.y < mapHeight && map[vertMap.y * mapWidth + vertMap.x] != 0)
		{
			vDist = cos * (vertRes.x - from.x) - sin * (vertRes.y - from.y);
			break;
		}

		// check for collisions with entities here

		vertRes.x += off.x;
		vertRes.y += off.y;
		dof += 1;
	}

	dof = 0;

	vec2f horzRes; vec2i horzMap;

	// looking up
	if (sin > 0.001f)
	{
		horzRes.y = F((I(from.y) >> POWER_OF_SIZE) << POWER_OF_SIZE) - 0.0001f;
		horzRes.x = (from.y - horzRes.y) * cotan + from.x;
		off.y = -CELL_SIZEF;
		off.x = -off.y * cotan;
	}
	else if (sin < -0.001f)
	{
		horzRes.y = F((I(from.y) >> POWER_OF_SIZE) << POWER_OF_SIZE) + CELL_SIZEF;
		horzRes.x = (from.y - horzRes.y) * cotan + from.x;
		off.y = CELL_SIZEF;
		off.x = -off.y * cotan;
	}
	else
	{
		horzRes.x = from.x; horzRes.y = from.y;
		dof = RENDER_RAYCAST_DOF_MAX;
	}


	while (dof < RENDER_RAYCAST_DOF_MAX)
	{
		horzMap.x = I(horzRes.x) >> POWER_OF_SIZE;
		horzMap.y = I(horzRes.y) >> POWER_OF_SIZE;

		if (horzMap.x >= 0 && horzMap.x < mapWidth && horzMap.y >= 0 && horzMap.y < mapHeight && map[horzMap.y * mapWidth + horzMap.x] != 0)
		{
			hDist = cos * (horzRes.x - from.x) - sin * (horzRes.y - from.y);
			break;
		}

		// check for collisions with entities here

		horzRes.x += off.x;
		horzRes.y += off.y;
		dof += 1;
	}

	// if we've hit a wall we have to properly record data for it
	// and also set which result data to use
	if (hDist != INFINITY || vDist != INFINITY)
	{
		int mapTile;
		float wallPercentageHit;
		if (hDist < vDist)
		{
			results->results = hHits;
			results->resultCount = hHitCount;

			vertRes = horzRes; vertMap = horzMap; vDist = hDist;
			mapTile = map[horzMap.y * mapWidth + horzMap.x];
			
			wallPercentageHit = vertRes.x / CELL_SIZEF;
			wallPercentageHit = wallPercentageHit - floorf(wallPercentageHit);

			results->normal = VEC2F2_1(0, F(-1 + (vertRes.y < from.y) * 2));

			if (sin < -0.001f)
			{
				wallPercentageHit = 1.0f - wallPercentageHit;
			}
		}
		else
		{
			results->results = vHits;
			results->resultCount = vHitCount;

			mapTile = map[vertMap.y * mapWidth + vertMap.x];

			wallPercentageHit = vertRes.y / CELL_SIZEF;
			wallPercentageHit = wallPercentageHit - floorf(wallPercentageHit);

			results->normal = VEC2F2_1(F(-1 + (vertRes.x < from.x) * 2), 0);

			if (cos < -0.001f)
			{
				wallPercentageHit = 1.0f - wallPercentageHit;
			}
		}

		rayData* wallData = &(results->results[results->resultCount++]);

		wallData->globalPosHit = vertRes;
		wallData->rayLength = vDist;
		wallData->wallTileHit = mapTile - 1;
		wallData->xHit = wallPercentageHit;
	}
	// um... if we didn't hit a wall, use the more populated list to be nice?
	// not gonna lie, this seems dangerous
	else
	{
		if (hHitCount > vHitCount)
		{
			results->results = hHits;
			results->resultCount = hHitCount;
		}
		else
		{
			results->results = vHits;
			results->resultCount = vHitCount;
		}

		results->normal = VEC2F2_1(0, 0);
	}

	//return (rayData){true, vertRes, vertMap, vDist, mapTile - 1, wallPercentageHit};

}

void levelLogicInit(void)
{
	mapWidth = mapHeight = 8;
	mapArea = 64;
}

void levelUpdate(void)
{

}