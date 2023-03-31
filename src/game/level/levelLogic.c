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
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

rayData castRay(vec2f from, float angle)
{
	int dof = 0;

	float hDist = INFINITY, vDist = INFINITY;

	float sin, cos, tan, cotan;
	sincosf(angle, &sin, &cos);
	tan = sin / cos;
	cotan = 1 / tan;

	vec2f off;
	int mult;

	// vertical bit
	vec2f vertRes; vec2i vertMap;

	// looking right
	if (cos > 0.001f)
	{
		vertRes.x = (float)((((int)from.x) >> POWER_OF_SIZE) << POWER_OF_SIZE) + CELL_SIZEF;
		vertRes.y = (from.x - vertRes.x) * tan + from.y;
		off.x = CELL_SIZEF;
		off.y = -off.x * tan;
	}
	// looking left
	else if (cos < -0.001f)
	{
		vertRes.x = (float)((((int)from.x) >> POWER_OF_SIZE) << POWER_OF_SIZE) - 0.0001f;
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
		vertMap.x = ((int)vertRes.x) >> POWER_OF_SIZE;
		vertMap.y = ((int)vertRes.y) >> POWER_OF_SIZE;


		mult = vertMap.x * vertMap.y;
		if (mult >= 0 && mult < mapArea && map[vertMap.y * mapWidth + vertMap.x] != 0)
		{
			vDist = cos * (vertRes.x - from.x) - sin * (vertRes.y - from.y);
			break;
		}

		vertRes.x += off.x;
		vertRes.y += off.y;
		dof += 1;
	}

	dof = 0;

	vec2f horzRes; vec2i horzMap;

	// looking up
	if (sin > 0.001f)
	{
		horzRes.y = (float)(((int)(from.y) >> POWER_OF_SIZE) << POWER_OF_SIZE) - 0.0001f;
		horzRes.x = (from.y - horzRes.y) * cotan + from.x;
		off.y = -CELL_SIZEF;
		off.x = -off.y * cotan;
	}
	else if (sin < -0.001f)
	{
		horzRes.y = (float)(((int)(from.y) >> POWER_OF_SIZE) << POWER_OF_SIZE) + CELL_SIZEF;
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
		horzMap.x = ((int)horzRes.x) >> POWER_OF_SIZE;
		horzMap.y = ((int)horzRes.y) >> POWER_OF_SIZE;

		mult = horzMap.x * horzMap.y;
		if (mult >= 0 && mult < mapArea && map[horzMap.y * mapWidth + horzMap.x] != 0)
		{
			hDist = cos * (horzRes.x - from.x) - sin * (horzRes.y - from.y);
			break;
		}

		horzRes.x += off.x;
		horzRes.y += off.y;
		dof += 1;
	}

	if (hDist == INFINITY && vDist == INFINITY)
	{
		return (rayData){false, 0, 0, 0, 0, -1, -1, 0};
	}

	if (hDist < vDist)
	{
		vertRes = horzRes; vertMap = horzMap; vDist = hDist;
	}

	int mapTile = map[vertMap.y * mapWidth + vertMap.x];
	// we know that out of the real position coords, one of them will be
	// exactly atop a gridline
	vec2f normalized = VEC2F2_1(vertRes.x / F(CELL_SIZE), vertRes.y / F(CELL_SIZE));
	normalized = VEC2F2_1(normalized.x - floorf(normalized.x), normalized.y - floorf(normalized.y));
	float wallPercentageHit = MIN(normalized.x, normalized.y);

	return (rayData){true, vertRes, vertMap, vDist, mapTile - 1, wallPercentageHit};

}

void levelLogicInit(void)
{
	mapWidth = mapHeight = 8;
	mapArea = 64;
}

float magic = 0.0f;

void levelUpdate(void)
{
	float diff = F(GET_KEY(GLFW_KEY_T) - GET_KEY(GLFW_KEY_G));
	magic += diff;

	if (GET_KEY(GLFW_KEY_SPACE))
	{
		printf("magic number: %f\n", magic);
	}
}